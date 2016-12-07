// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
extern "C" {
#include "run68.h"

ULong zmusic_trap3 = 0;
ULong zmusic_timer = 0;
char* zmusic_work = NULL;

}

#include "x68sound.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

short* opm_buffer = NULL;
int opm_count = 0;
struct {
  UChar reg;
  UChar val;
} preset[1024];
int presets = 0;

// TODO: Wire X68Sound_OpmPeek
void zmusic_timerb() {
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

}  // namespace

extern "C" void zmusic_init(int rate, int count) {
  X68Sound_StartPcm(rate);
  zmusic_work = (char*)malloc(0x100000);
  opm_count = count;
  opm_buffer = (short*)malloc(count * 2 * 2);
  for (int i = 0; i < presets; ++i) {
    X68Sound_OpmReg(preset[i].reg);
    X68Sound_OpmPoke(preset[i].val);
  }
  X68Sound_OpmInt(zmusic_timerb);
}

extern "C" void zmusic_set_reg(UChar reg) {
  if (opm_buffer == NULL) {
    if (presets == 1024) {
      printf("presets overflow\n");
      return;
    }
    preset[presets].reg = reg;
  } else {
    X68Sound_OpmReg(reg);
  }
}

extern "C" void zmusic_set_val(UChar val) {
  if (opm_buffer == NULL) {
    if (presets == 1024) {
      printf("presets overflow\n");
      return;
    }
    preset[presets].val = val;
    presets++;
    if (presets != 1024)
      preset[presets].reg = preset[presets - 1].reg;
  } else {
    X68Sound_OpmPoke(val);
  }
}

extern "C" short* zmusic_update() {
  X68Sound_GetPcm(opm_buffer, opm_count);
  return opm_buffer;
}

extern "C" void zmusic_trap(
    ULong d1, ULong d2, ULong d3, ULong d4, ULong a1, const char* filename) {
  //printf("ZMUSIC ENTER: d1=$%08x, d2=$%08x, d3=$%08x, d4=$%08x, a1=$%08x(%s)\n",
  //    d1, d2, d3, d4, a1, filename);
  if (d1 == 0x06 || d1 == 0x08 || d1 == 0x12 || d1 == 0x14) {
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

extern "C" int pcm8_call() {
  switch (rd[0]) {
    case 0x000:  // Normal play at ch.0
      X68Sound_Pcm8_Out(0, &prog_ptr[ra[1]], rd[1], rd[2]);
      break;
    case 0x101:  // Abort
      X68Sound_Pcm8_Abort();
      break;
    default:
      printf("$%06x PCM8($%08x)\n", pc - 2, rd[0]);
      break;
  }
  return 0;
}