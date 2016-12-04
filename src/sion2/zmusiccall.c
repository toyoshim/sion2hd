// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

extern jsrt_zmusic_call(ULong d1, ULong d2, ULong d3, ULong d4, ULong a1);

int zmusic_call() {
  rd[0] = 0;
  switch (rd[1]) {
    case 0x00:  // M_INIT
      //printf("$%06x ZMUSIC(M_INIT)\n", pc - 2);
      break;
    case 0x08:  // M_PLAY
      //printf("$%06x ZMUSIC(M_PLAY); d2=$%08x, d3=$%08x, d4=$08x\n", pc - 2,
      //    rd[2], rd[3], rd[4]);
      break;
    case 0x11:  // PLAY_CNV_DATA
      //printf("$%06x ZMUSIC(PLAY_CNV_DATA); size=$%08x, addr=$%08x\n", pc - 2,
      //    rd[2], ra[1]);
      break;
    case 0x12:  // SE_PLAY
      //printf("$%06x ZMUSIC(SE_PLAY); track=$%08x, addr=$%08x\n", pc - 2,
      //    rd[2], ra[1]);
      break;
    case 0x14:  // SE_ADPCM2
      //printf("$%06x ZMUSIC(SE_ADPCM2); d2=$%08x, d3=$%08x\n", pc - 2, rd[2],
      //    rd[3]);
      break;
    default:
      printf("$%06x ZMUSIC($%08x)\n", pc - 2, rd[1]);
      return -1;
  }
  jsrt_zmusic_call(rd[1], rd[2], rd[3], rd[4], ra[1]);
  return 0;
}
