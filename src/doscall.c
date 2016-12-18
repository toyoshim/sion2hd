// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

#include <emscripten.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define fprintf(...)

extern void jsrt_zmusic_bind(ULong addr, char* filename);

static void super() {
  long data = mem_get(ra[7], S_LONG);
  if (!data) {
    // to SUPER
    rd[0] = ra[7];
    usp = ra[7];
    SR_S_ON();
  } else {
    // to USER
    ra[7] = data;
    rd[0] = 0;
    usp = 0;
    SR_S_OFF();
  }
}

// Mimimum hack to pass sion2_pcg.SPD file read operations.
static ULong pcg_read() {
  long data = mem_get(ra[7] + 2, S_LONG);
  long len = mem_get(ra[7] + 6, S_LONG);
  char* p = prog_ptr + data;
  for (int i = 0; i < len; ++i)
    p[i] = i == 1 ? 0x31 : 0;
  return len;
}

static char* lastOpenedFilename = NULL;

static const int kPcgFd = 128;
static const int kZmdFd = 129;

int dos_call(UChar code) {
  switch (code) {
    case 0x0C:    // KFLUSH
      fprintf(stderr, "$%06x FUNC(KFLUSH): ignore.\n", pc - 2);
      rd[0] = 0;
      break;
    case 0x20:    // SUPER
      super();
      break;
    case 0x3C: {  // CREATE
      ULong nameptr = mem_get(ra[7], S_LONG);
      UShort atr = mem_get(ra[7] + 4, S_WORD);
      if (!strcmp(&prog_ptr[nameptr], "HI_SCORE.DAT")) {
        rd[0] = open("/persistent/HI_SCORE.DAT", O_RDWR | O_CREAT, 0777);
      } else
        rd[0] = -1;
      fprintf(stderr, "$%06x FUNC(CREATE): file=%s, atr=$%04x => $%08x.\n",
          pc - 2, &prog_ptr[nameptr], atr, rd[0]);
      break;
    }
    case 0x3D: {  // OPEN
      ULong nameptr = mem_get(ra[7], S_LONG);
      UShort mode = mem_get(ra[7] + 4, S_WORD);
      lastOpenedFilename = &prog_ptr[nameptr];
      // TODO: Use IDBFS for HI_SCORE.DAT.
      if (!strcmp(&prog_ptr[nameptr], "sion2_pcg.SPD"))
        rd[0] = kPcgFd;  // Returns a MAGIC fd.
      else if (!strcmp(&prog_ptr[nameptr], "HI_SCORE.DAT"))
        rd[0] = open("/persistent/HI_SCORE.DAT", O_RDWR | O_CREAT, 0777);
      else 
        rd[0] = kZmdFd;  // Returns a MAGIC fd for ZMD.
      fprintf(stderr, "$%06x FUNC(OPEN); file=%s, mode=$%04x => $%08x.\n",
          pc - 2, &prog_ptr[nameptr], mode, rd[0]);
      break;
    }
    case 0x3E: {  // CLOSE
      UShort fileno = mem_get(ra[7], S_WORD);
      if (fileno != kPcgFd && fileno != kZmdFd)
        rd[0] = close(fileno);
      else
        rd[0] = 0;
      fprintf(stderr, "$%06x FUNC(CLOSE); fd=$%04x => $%08x.\n", pc - 2, fileno,
          rd[0]);
      break;
    }
    case 0x3F: {  // READ
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong buffer = mem_get(ra[7] + 2, S_LONG);
      ULong len = mem_get(ra[7] + 6, S_LONG);
      if (fileno == kPcgFd)
        rd[0] = pcg_read();
      else if (fileno == kZmdFd)
        rd[0] = 0;
      else
        rd[0] = read(fileno, &prog_ptr[buffer], len);
      if (fileno == kZmdFd)
        jsrt_zmusic_bind(buffer + 7, lastOpenedFilename);
      fprintf(stderr,
          "$%06x FUNC(READ); fd=$%04x, buffer=$%08x, len=$%08x => $%08x.\n",
          pc - 2, fileno, buffer, len, rd[0]);
      break;
    }
    case 0x40: {  // WRITE
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong buffer = mem_get(ra[7] + 2, S_LONG);
      ULong len = mem_get(ra[7] + 6, S_LONG);
      if (fileno != kPcgFd && fileno != kZmdFd) {
        rd[0] = write(fileno, &prog_ptr[buffer], len);
        EM_ASM(FS.syncfs(false, function(){}));
      }
      fprintf(stderr,
          "$%06x FUNC(WRITE); fd=$%04x, buffer=$%08x, len=$%08x => $%08x.\n",
          pc - 2, fileno, buffer, len, rd[0]);
      break;
    }
    case 0x42: {  // SEEK
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong offset = mem_get(ra[7] + 2, S_LONG);
      UShort mode = mem_get(ra[7] + 6, S_WORD);
      if (fileno == kZmdFd)
        rd[0] = 16;
      else if (fileno == kPcgFd)
        rd[0] = 0;
      else
        rd[0] = lseek(fileno, offset, mode);
      fprintf(stderr,
          "$%06x FUNC(SEEK); fd=$%04x, offset=$%08x, mode=$%04x => $%08x.\n",
          pc - 2, fileno, offset, mode, rd[0]);
      break;
    }
    default:
      printf("$%06x FUNC(%02X): NOT IMPL.\n", pc - 2, code);
      rd[0] = 0;
      break;
  }
  return 0;
}
