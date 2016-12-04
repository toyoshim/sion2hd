// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <stdio.h>
#include <stdlib.h>
#include "run68.h"

extern char* zmusic_work;
extern void zmusic_set_reg(UChar reg);
extern void zmusic_set_val(UChar val);

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

static int hsync = 0x80;

long mem_get(long adr, char size) {
  UChar   *mem;
  long	d;
  
  adr &= 0x00FFFFFF;
  mem = (UChar*)prog_ptr + adr;
  if (adr < ENV_TOP || adr >= mem_aloc) {
    // ZMUSIC tried to check PCM8 installation.
    if (adr == 0x000088)  // Trap #2 vector (PCM8).
      return 0x000408;    // Returns a dummy PCM8 vector.
    if (adr == 0x000400)  // Vector - 8 returns MAGIC number.
      return 'PCM8';

    // Mimic vector information.
    if (adr == 0x00010C)  // MFP FM vector.
      return 0x43FF0540;  // Returns default vector for ZMUSIC not being upset.
    if (adr == 0x000228)  // MIDI FIFO-Rx ready vector.
      return 0x8AFF0540;  // Returns default vector for ZMUSIC not being upset.

    // ZMUSIC tried to check NUL driver installation.
    if (0x006810 <= adr && adr < 0x006828) {
      // First, it tried to find 'NUL     ' mark in memory region after the
      // first memory block, 0x006800. This is the device name of NUL driver
      // header.
      if (adr == 0x00681E)
        return 'N';
      if (adr == 0x00681F)
        return 'U';
      if (adr == 0x006820)
        return 'L';
      // Second, it tried to check if found address - 10 returns 0x8024.
      // This is the device attribute of NUL driver header.
      if (adr == 0x006814)
        return 0x8024;
      // Last, it reads founc address - 14 to check the next driver. Returns
      // 0xFFFFFFFF to express this is the last one.
      if (adr == 0x006810)
        return 0xFFFFFFFF;
      return ' ';
    }

    // ZMUSIC tried to find 0x4E73 inside IPL area for some reasons.
    // Here, we just returns it immediately.
    if (adr == 0xFF0000)
      return 0x4E73;

    // ZMUSIC counts H-SYNC flips for waiting a little.
    if (adr == 0xE88001) {  // MFP GPIP
      hsync ^= 0x80;
      return 0xFF ^ hsync;
    }

    if (adr == 0xE88015)    // MFP IMRB
      return 0;

    // OPM registers.
    if (adr == 0xE90003) {  // Status
      // Bit 7: BUSY
      // Bit 1: TIMER-B OVERFLOW
      // Bit 0: TIMER-A OVERFLOW
      return 3;
    }

    // ZMUSIC virtual work area to realize remote PLAY_CNV_DATA.
    if (0x100000 <= adr && adr < 0x200000) {
      mem = &zmusic_work[adr - 0x100000];
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
    if (adr == 0x0000B0)  // Trap #12 vector (COPY).
      return;
    if (adr == 0x000228)  // MIDI FIFO-Rx ready vector.
      return;

    // MFP IMRB
    if (adr == 0xE88015)
      return;

    // OPM registers.
    if (adr == 0xE90001) {  // Register
      zmusic_set_reg(d);
      return;
    }
    if (adr == 0xE90003) {  // Data
      zmusic_set_val(d);
      return;
    }
    // ZMUSIC virtual work area to realize remote PLAY_CNV_DATA.
    if (0x100000 <= adr && adr < 0x200000) {
      mem = &zmusic_work[adr - 0x100000];
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
}
