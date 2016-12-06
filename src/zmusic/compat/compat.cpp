// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "windows.h"

#include <stdio.h>

uint32_t _byteswap_ulong(uint32_t data) {
  uint8_t h = data >> 24;
  uint8_t j = data >> 16;
  uint8_t k = data >> 8;
  uint8_t l = data;
  return (l << 24) | (k << 16) | (j << 8) | h;
}

uint16_t _byteswap_ushort(uint16_t data) {
  uint8_t h = data >> 8;
  uint8_t l = data;
  return (l << 8) | h;
}

DWORD timeGetTime() {
  return 0;
}

// Since we do not create a thread, these methods can be non-atomic operations.
void _InterlockedIncrement(volatile long* value) {
  *value = (*value + 1);
}

void _InterlockedDecrement(volatile long* value) {
  *value = (*value - 1);
}

long _InterlockedCompareExchange(volatile long* dst, long ex, long cmp) {
  if (*dst != cmp)
    return *dst;
  *dst = ex;
  return cmp;
}

LPVOID GlobalAllocPtr(UINT fuFlags, DWORD cbBytes) {
  return malloc(cbBytes);
}

MMRESULT waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh) {
  return MMSYSERR_NOERROR;
}
