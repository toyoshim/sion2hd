// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <stdio.h>
#include <stdlib.h>
#include "run68.h"

static  UChar graphic_palette[512] = {
  0x00, 0x00,
  0x52, 0x94,
  0x00, 0x20,
  0x00, 0x3E,
  0x04, 0x00,
  0x07, 0xC0,
  0x04, 0x20,
  0x07, 0xFE,
  0x08, 0x00,
  0xF8, 0x00,
  0x80, 0x20,
  0xF8, 0x3E,
  0x84, 0x00,
  0xFF, 0xC0,
  0xAD, 0x6A,
  0xFF, 0xFF,
};

extern void jsrt_io_graphic_data(UShort page, ULong index, UShort color);
extern void jsrt_io_sprite_data(ULong index, UShort data);
extern void jsrt_iocs_gpalet(UShort index, UShort color);

void run68_abort(long adr) {
  int	i;
  printf("*ABORT*");
  printf("d0-7=%08lx", rd[0]);
  for (i = 1; i < 8; i++)
    printf(",%08lx", rd[i]);
  printf("\n");
  printf("a0-7=%08lx", ra[0]);
  for (i = 1; i < 8; i++)
    printf(",%08lx", ra[i]);
  printf("\n");
  printf( "pc=%08lx,sr=%04x,adr=%08lx\n", pc, sr, adr);
  abort();
}

long mem_get(long adr, char size) {
  UChar   *mem;
  long	d;
  
  adr &= 0x00FFFFFF;
  mem = (UChar*)prog_ptr + adr;
  if (adr < ENV_TOP || adr >= mem_aloc) {
    if ((0xE82200 <= adr && adr < 0xE82400) ||  // Text/BG Palette
        (0xE82500 == adr) ||                    // Priority Control
        (0xEB8000 <= adr && adr < 0xEBC000)) {  // PCG Area
      return 0;
    }
    if (0xE82000 <= adr && adr < 0xE82200) {    // Graphic Palette
      mem = &graphic_palette[adr - 0xE82000];
    } else {
      if (adr >= 0xC00000) {
        printf("ERROR: I/O port or ROM read access\n");
        run68_abort(adr);
      }
      if (SR_S_REF() == 0 || adr >= mem_aloc) {
        printf("ERROR: Invalid read access\n");
        run68_abort(adr);
      }
    }
  }

  switch(size) {
    case S_BYTE:
      return *mem;
    case S_WORD:
      d = *(mem++);
      d = ((d << 8) | *mem);
      return d;
    default:	/* S_LONG */
      d = *(mem++);
      d = ((d << 8) | *(mem++));
      d = ((d << 8) | *(mem++));
      d = ((d << 8) | *mem);
      return d;
  }
}

void mem_set( long adr, long d, char size )
{
  UChar   *mem;

  adr &= 0x00FFFFFF;
  mem = (UChar *)prog_ptr + adr;
  if (adr < ENV_TOP || adr >= mem_aloc) {
    if ((0xE82200 <= adr && adr < 0xE82400 ) ||  // Text/BG Palette
        (0xE82500 == adr) ||                     // Priority Control
        (0xEB8000 <= adr && adr < 0xEBC000 )) {  // PCG Area
      return;
    }
    if (0xD00000 <= adr && adr < 0xD80000) {  // Graphic Page 2
      if (size != S_WORD)
        abort();
      jsrt_io_graphic_data(2, (adr - 0xD00000) / 2, d);
      return;
    }
    if (0xEB0000 <= adr && adr < 0xEB0400) {  // Sprite Control
      if (size != S_WORD)
        abort();
      jsrt_io_sprite_data((adr - 0xEB0000) / 2, d );
      return;
    }
    if (0xE82000 <= adr && adr < 0xE82200) {  // Graphic Palette
      mem = &graphic_palette[adr - 0xE82000];
    } else {
      if (adr >= 0xC00000) {
        printf("ERROR: I/O port or ROM write access\n");
        run68_abort(adr);
      }
      if (SR_S_REF() == 0 || adr >= mem_aloc) {
        printf("ERROR: Invalid write access\n");
        run68_abort(adr);
      }
    }
  }

  switch( size ) {
    case S_BYTE:
      *mem = (d & 0xFF);
      break;
    case S_WORD:
      *(mem++) = ((d >> 8) & 0xFF);
      *mem = (d & 0xFF);
      break;
    default:	/* S_LONG */
      *(mem++) = ((d >> 24) & 0xFF);
      *(mem++) = ((d >> 16) & 0xFF);
      *(mem++) = ((d >> 8) & 0xFF);
      *mem = (d & 0xFF);
      break;
  }

  if (0xE82000 <= adr && adr < 0xE82200) {  // Graphic Palette
    if (size != S_WORD || ((adr & 1) != 0))
      abort();
    jsrt_iocs_gpalet((adr - 0xE82000) / 2, d);
  }
}
