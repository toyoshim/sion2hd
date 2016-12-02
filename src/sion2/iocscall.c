// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

extern void jsrt_iocs_bgscrlst(ULong page, UShort x, UShort y);
extern void jsrt_iocs_bgtextcl(UChar page, UShort code);
extern void jsrt_iocs_bgtextst(UChar page, UChar x, UChar y, UShort code);
extern int jsrt_iocs_bitsns(UChar group);
extern void jsrt_iocs_contrast(UChar c);
extern int jsrt_iocs_joyget(UChar id);
extern void jsrt_iocs_sp_on();
extern void jsrt_iocs_sp_off();
extern void jsrt_iocs_sp_regst(
    ULong id, ULong x, ULong y, ULong code, ULong prio);
// FIXME: rename to jsrt_iocs_gpalet.
extern void jsrt_io_graphic_palette(UShort index, UShort color);

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
    case 0x04:  // BITSNS
      rd[0] = jsrt_iocs_bitsns(rd[1]);
      break;
    case 0x10:  // CRTMOD
      printf("$%06x IOCS(CRTMOD): ignore.\n", pc - 2);
      break;
    case 0x11:  // CONTRAST
      if (rd[1] < 0)
        printf("$%06x IOCS(CONTRAST): read is not impl.\n", pc - 2);
      else
        jsrt_iocs_contrast(rd[1]);
      break;
    case 0x1F:  // B_CUROFF
      printf("$%06x IOCS(B_CUROFF): ignore.\n", pc - 2);
      break;
    case 0x3b:  // JOYGET
      rd[0] = jsrt_iocs_joyget(rd[1]);
      break;
    case 0x81:  // B_SUPER
      super();
      break;
    case 0x90:  // G_CLR_ON
      printf("$%06x IOCS(G_CLR_ON): ignore.\n", pc - 2);
      break;
    case 0x94:  // GPALET
      jsrt_io_graphic_palette(rd[1], rd[2]);
      break;
    case 0xc0:  // SP_INIT
      printf("$%06x IOCS(SP_INIT): ignore.\n", pc - 2);
      break;
    case 0xc1:  // SP_ON
      jsrt_iocs_sp_on();
      break;
    case 0xc2:  // SP_OFF
      jsrt_iocs_sp_off();
      break;
    case 0xc6:  // SP_REGST
      jsrt_iocs_sp_regst(rd[1], rd[2], rd[3], rd[4], rd[5]);
      break;
    case 0xc8:  // BGSCRLST
      jsrt_iocs_bgscrlst(rd[1], rd[2], rd[3]);
      break;
    case 0xcc:  // BGTEXTCL
      jsrt_iocs_bgtextcl(rd[1], rd[2]);
      break;
    case 0xcd:  // BGTEXTST
      jsrt_iocs_bgtextst(rd[1], rd[2], rd[3], rd[4]);
      break;
    default:
      printf("$%06x IOCS(%02X): NOT IMPL.\n", pc - 2, no);
      break;
  }
  return 0;
}
