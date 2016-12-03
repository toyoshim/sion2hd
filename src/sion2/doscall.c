// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
static const int kDatFd = 129;
static const int kZmdFd = 130;

int dos_call(UChar code) {
  switch (code) {
    case 0x0C:    // KFLUSH
      printf("$%06x FUNC(KFLUSH): ignore.\n", pc - 2);
      rd[0] = 0;
      break;
    case 0x20:    // SUPER
      super();
      break;
    case 0x3D: {  // OPEN
      ULong nameptr = mem_get(ra[7], S_LONG);
      UShort mode = mem_get(ra[7] + 4, S_WORD);
      lastOpenedFilename = &prog_ptr[nameptr];
      // TODO: Use IDBFS for HI_SCORE.DAT.
      if (!strcmp(&prog_ptr[nameptr], "sion2_pcg.SPD"))
        rd[0] = kPcgFd;  // Returns a MAGIC fd.
      else if (!strcmp(&prog_ptr[nameptr], "HI_SCORE.DAT"))
        rd[0] = kDatFd;  // Returns a MAGIC fd.
      else 
        rd[0] = kZmdFd;  // Returns a MAGIC fd for ZMD.
      printf("$%06x FUNC(OPEN); file=%s, mode=$%04x => $%08x.\n", pc - 2,
          &prog_ptr[nameptr], mode, rd[0]);
      break;
    }
    case 0x3E: {  // CLOSE
      UShort fileno = mem_get(ra[7], S_WORD);
      rd[0] = 0;
      printf("$%06x FUNC(CLOSE); fd=$%04x => $%08x.\n", pc - 2, fileno, rd[0]);
      break;
    }
    case 0x3F: {  // READ
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong buffer = mem_get(ra[7] + 2, S_LONG);
      ULong len = mem_get(ra[7] + 6, S_LONG);
      if (fileno == kPcgFd)
        rd[0] = pcg_read();
      else
        rd[0] = 0;
      if (fileno == kZmdFd)
        jsrt_zmusic_bind(buffer + 7, lastOpenedFilename);
      printf("$%06x FUNC(READ); fd=$%04x, buffer=$%08x, len=$%08x => $%08x.\n",
          pc - 2, fileno, buffer, len, rd[0]);
      break;
    }
    case 0x42: {  // SEEK
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong offset = mem_get(ra[7] + 2, S_LONG);
      UShort mode = mem_get(ra[7] + 6, S_WORD);
      if (fileno == kZmdFd)
        rd[0] = 16;
      else
        rd[0] = 0;
      printf("$%06x FUNC(SEEK); fd=$%04x, offset=$%08x, mode=$%04x => $%08x.\n",
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
