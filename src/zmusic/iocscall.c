// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

static void super() {
  if (!ra[1]) {
    // to SUPER
    rd[0] = ra[7];
    SR_S_ON();
  } else {
    // to USER
    ra[7] = ra[1];
    rd[0] = 0;
    SR_S_OFF();
  }
}

int iocs_call() {
  UChar no = rd[0] & 0xff;
  rd[0] = 0;
  switch (no) {
    case 0x0f:    // DEFCHR
      break;
    case 0x21: {  // B_PRINT
      printf("%s", &prog_ptr[ra[1]]);
      size_t len = strlen(&prog_ptr[ra[1]]);
      ra[1] += len + 1;
      break;
    }
    case 0x6a:    // OPMINTST
      printf("$%06lx IOCS(OPMINST): ignore.\n", (ULong)(pc - 2));
      break;
    case 0x80:    // B_INTVCS
      printf("$%06lx IOCS(B_INTVCS): vec=$%04lx, adr=$%08lx.\n",
          (ULong)(pc - 2), rd[1] & 0xffff, ra[1]);
      switch (rd[1] & 0xffff) {
        case 0x0023:  // Trap #3 vector
          rd[0] = 0x00ff0000;
          break;
        case 0x01f0:  // IOCS OPMDRV vector
          rd[0] = 0x00ff0000;
          break;
        default:
          printf("  unknown vector\n");
          rd[0] = 0x00ff0000;
          break;
      }
      break;
    case 0x81:    // B_SUPER
      super();
      break;
    default:
      printf("$%06lx IOCS(%02X): NOT IMPL.\n", (ULong)(pc - 2), no);
      break;
  }
  return 0;
}
