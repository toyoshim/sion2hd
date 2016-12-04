// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

#include "compat.h"
#include "ym2151.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

ULong zmusic_trap3 = 0;
ULong zmusic_timer = 0;

char* zmusic_work = NULL;
short* opm_buffer = NULL;
short* opm_handle[2] = { NULL, NULL };
int opm_count = 0;
UChar opm_reg = 0;
struct {
  UChar reg;
  UChar val;
} preset[1024];
int presets = 0;

void zmusic_timerb(int irq) {
  if (irq != 1)
    return;
  // Virtual stack to return 0.
  pc = 0;
  ra[7] -= 4;
  mem_set(ra[7], pc, S_LONG);
  ra[7] -= 2;
  mem_set(ra[7], sr, S_WORD);
  pc = zmusic_timer;
  SR_S_ON();
  while (pc && FALSE == prog_exec());
}

void zmusic_init(int rate, int count) {
  YM2151Init(1, 4000000, rate);
  zmusic_work = malloc(0x100000);
  opm_count = count;
  opm_buffer = malloc(count * 2 * 2);
  opm_handle[0] = &opm_buffer[0];
  opm_handle[1] = &opm_buffer[opm_count];
  for (int i = 0; i < 1024; ++i)
    YM2151WriteReg(0, preset[presets].reg, preset[presets].val);
  YM2151SetIrqHandler(0, zmusic_timerb);
}

void zmusic_set_reg(UChar reg) {
  opm_reg = reg;
}

void zmusic_set_val(UChar val) {
  if (opm_buffer == NULL) {
    if (presets == 1024) {
      printf("presets overflow\n");
      return;
    }
    preset[presets].reg = opm_reg;
    preset[presets].val = val;
    presets++;
  } else {
    YM2151WriteReg(0, opm_reg, val);
  }
}

short* zmusic_update() {
  for (int offset = 0; offset < opm_count; offset += 256) {
    opm_handle[0] = &opm_buffer[offset + 0];
    opm_handle[1] = &opm_buffer[offset + opm_count];
    YM2151UpdateOne(0, opm_handle, 256);
  }
  return opm_buffer;
}

void zmusic_trap(
    ULong d1, ULong d2, ULong d3, ULong d4, ULong a1, const char* filename) {
  //printf("ZMUSIC ENTER: d1=$%08x, d2=$%08x, d3=$%08x, d4=$%08x, a1=$%08x(%s)\n",
  //    d1, d2, d3, d4, a1, filename);
  if (d1 == 0x08 || d1 == 0x12 || d1 == 0x14) {
    // Should be modified to work over RPC.
    return;
  }
  if (d1 == 0x11) {
    // PLAY_CNV_DATA
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
  
  // Virtual stack to return 0.
  pc = 0;
  ra[7] -= 4;
  mem_set(ra[7], pc, S_LONG);
  ra[7] -= 2;
  mem_set(ra[7], sr, S_WORD);
  pc = zmusic_trap3;
  SR_S_ON();
  while (pc && FALSE == prog_exec());
  //printf("ZMUSIC LEAVE => $%08x\n", rd[0]);
}
