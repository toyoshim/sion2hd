// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern void jsrt_dos_keepr(UShort code);

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

int dos_call(UChar code) {
  switch (code) {
    case 0x00:    // EXIT
      printf("$%06x FUNC(EXIT).\n", pc - 2);
      return -1;
    case 0x09: {  // PRINT
      ULong mesptr = mem_get(ra[7], S_LONG);
      printf("$%06x FUNC(PRINT); %s\n", pc - 2, &prog_ptr[mesptr]);
      break;
    }
    case 0x20:    // SUPER
      super();
      break;
    case 0x31: {  // KEEPR
      ULong prglen = mem_get(ra[7], S_LONG);
      UShort code = mem_get(ra[7] + 4, S_WORD);
      printf("$%06x FUNC(KEEPR); prglen=$%08x, code=$%04x.\n", pc - 2, prglen,
          code);
      jsrt_dos_keepr(code);
      return -1;
    }
    case 0x3D: {  // OPEN
      ULong nameptr = mem_get(ra[7], S_LONG);
      UShort mode = mem_get(ra[7] + 4, S_WORD);
      // Support readonly mode.
      rd[0] = open(&prog_ptr[nameptr], O_RDONLY);
      printf("$%06x FUNC(OPEN); file=%s, mode=$%04x => $%08x.\n", pc - 2,
          &prog_ptr[nameptr], mode, rd[0]);
      break;
    }
    case 0x3E: {  // CLOSE
      UShort fileno = mem_get(ra[7], S_WORD);
      rd[0] = close(fileno);
      printf("$%06x FUNC(CLOSE), fd=$%04x => $%08x.\n", pc - 2, fileno, rd[0]);
      break;
    }
    case 0x3F: {  // READ
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong buffer = mem_get(ra[7] + 2, S_LONG);
      ULong len = mem_get(ra[7] + 6, S_LONG);
      rd[0] = read(fileno, &prog_ptr[buffer], len);
      printf("$%06x FUNC(READ); fd=$%04x, buffer=$%08x, len=$%08x => $%08x.\n",
          pc - 2, fileno, buffer, len, rd[0]);
      break;
    }
    case 0x42: {  // SEEK
      UShort fileno = mem_get(ra[7], S_WORD);
      ULong offset = mem_get(ra[7] + 2, S_LONG);
      UShort mode = mem_get(ra[7] + 6, S_WORD);
      rd[0] = lseek(fileno, offset, mode);
      printf("$%06x FUNC(SEEK); fd=$%04x, offset=$%08x, mode=$%04x => $%08x.\n",
          pc - 2, fileno, offset, mode, rd[0]);
      break;
    }
    case 0x4C: {  // EXIT2
      UShort code = mem_get(ra[7], S_WORD);
      printf("$%06x FUNC(EXIT2); code=$%04x.\n", pc - 2, code);
      return -1;
    }
    case 0xF7: {  // BUS_ERR
      UShort md = mem_get(ra[7] + 8, S_WORD);
      ULong d_adr = mem_get(ra[7] + 4, S_LONG);
      ULong s_adr = mem_get(ra[7], S_LONG);
      // Note: $eafaxx: MIDI board.
      printf("$%06x FUNC(BUS_ERR); md=$%04x, d_adr=$%08x, s_adr=$%08x.\n",
          pc - 2, md, d_adr, s_adr);
      rd[0] = 2;  // Bus error on read: MIDI board does not exit.
      break;
    }
    default:
      printf("$%06x FUNC(%02X): NOT IMPL.\n", pc - 2, code);
      break;
  }
  return 0;
}
