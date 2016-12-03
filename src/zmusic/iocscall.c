// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static ULong zmusic_trap3 = 0;

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
      printf("$%06lx IOCS(B_PRINT): %s", (ULong)(pc - 2), &prog_ptr[ra[1]]);
      size_t len = strlen(&prog_ptr[ra[1]]);
      ra[1] += len + 1;
      break;
    }
    case 0x60:    // ADPCMOUT
      printf("$%06lx IOCS(ADPCMOUT): ignore.\n", (ULong)(pc - 2));
      break;
    case 0x6a:    // OPMINTST
      printf("$%06lx IOCS(OPMINST): adr=$%08lx.\n", (ULong)(pc - 2), ra[1]);
      break;
    case 0x80:    // B_INTVCS
      printf("$%06lx IOCS(B_INTVCS): vec=$%04lx, adr=$%08lx.\n",
          (ULong)(pc - 2), rd[1] & 0xffff, ra[1]);
      switch (rd[1] & 0xffff) {
        case 0x0023:  // Trap #3 vector
          zmusic_trap3 = ra[1];
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
    case 0x8A: {  // DMAMOVE
      int i;
      int mode = rd[1] & 0xff;
      int smode = (mode >> 2) & 3;
      int dmode = mode & 3;
      int sdiff = (smode == 1) ? 1 : (smode == 2) ? -1 : 0;
      int ddiff = (dmode == 1) ? 1 : (dmode == 2) ? -1 : 0;
      printf("$%06lx IOCS(DMAMOVE): mode=%02x, len=%08x, src=%08x, dst=%08x.\n",
          (ULong)(pc - 2), mode, rd[2], ra[1], ra[2]);
      for (i = 0; i < rd[2]; ++i) {
        prog_ptr[ra[2]] = prog_ptr[ra[1]];
        ra[1] += sdiff;
        ra[2] += ddiff;
      }
      break;
    }
    default:
      printf("$%06lx IOCS(%02X): NOT IMPL.\n", (ULong)(pc - 2), no);
      break;
  }
  return 0;
}

char* zmusic_work = NULL;

void zmusic_trap(
    ULong d1, ULong d2, ULong d3, ULong d4, ULong a1, const char* filename) {
  printf("ZMUSIC ENTER: d1=$%08x, d2=$%08x, d3=$%08x, d4=$%08x, a1=$%08x(%s)\n",
      d1, d2, d3, d4, a1, filename);
  if (d1 == 0x08 || d1 == 0x12 || d1 == 0x14) {
    // Should be modified to work over RPC.
    return;
  }
  if (d1 == 0x11) {
    // PLAY_CNV_DATA
    if (NULL == zmusic_work)
      zmusic_work = malloc(0x100000);
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
      printf("ZMUSIC CNV_DATA: file open error\n");
      return;
    }
    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    read(fd, zmusic_work, size);
    close(fd);
    a1 = 0x100007;
  }
  // Destroy them all.
  rd[1] = d1;
  rd[2] = d2;
  rd[3] = d3;
  rd[4] = d4;
  ra[1] = a1;
  ra[7] -= 4;
  // Virtual stack to return 0.
  pc = 0;
  mem_set(ra[7], pc, S_LONG);
  ra[7] -= 2;
  mem_set(ra[7], sr, S_WORD);
  pc = zmusic_trap3;
  SR_S_ON();
  while (pc && FALSE == prog_exec());
  printf("ZMUSIC LEAVE => $%08x\n", rd[0]);
}
