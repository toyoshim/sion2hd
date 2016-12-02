// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

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
      return -1;
    case 0x09: {  // PRINT
      long mesptr = mem_get(ra[7], S_LONG);
      printf("%s", &prog_ptr[mesptr]);
      rd[0] = 0;
      break;
    }
    case 0x20:    // SUPER
      super();
      break;
    case 0x4C: {  // EXIT2
      short code = mem_get(ra[7], S_WORD);
      printf("$%06x FUNC(EXIT2); code=$%04x.\n", code);
      return -1;
    }
    case 0xF7: {  // BUS_ERR
      short md = mem_get(ra[7] + 8, S_WORD);
      long d_adr = mem_get(ra[7] + 4, S_LONG);
      long s_adr = mem_get(ra[7], S_LONG);
      // Note: $eafaxx: MIDI board.
      printf("$%06x FUNC(BUS_ERR); md=$%04x, d_adr=$%08x, s_adr=$%08x.\n",
          pc - 2, md, d_adr, s_adr);
      rd[0] = 2;  // Bus error on read: MIDI board does not exit.
      break;
    }
    default:
      printf("$%06x FUNC(%02X): NOT IMPL.\n", pc - 2, code);
      rd[0] = 0;
      break;
  }
  return 0;
}
