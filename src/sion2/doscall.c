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

// Mimimum hack to pass file read operations.
static void read() {
  long data = mem_get(ra[7] + 2, S_LONG);
  long len = mem_get(ra[7] + 6, S_LONG);
  char* p = prog_ptr + data;
  for (int i = 0; i < len; ++i)
    p[i] = i == 1 ? 0x31 : 0;
  rd[0] = len;
}

int dos_call(UChar code) {
  switch (code) {
    case 0x20:  // SUPER
      super();
      break;
    case 0x3F:  // READ
      read();
      break;
    default:
      printf( "$%06x FUNC(%02X): NOT IMPL.\n", pc - 2, code);
      rd[0] = 0;
      break;
  }
  return 0;
}
