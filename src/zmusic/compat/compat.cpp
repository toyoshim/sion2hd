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

BOOL CloseHandle(HANDLE) {
  return TRUE;
}

HANDLE CreateThread(
    LPSECURITY_ATTRIBUTES, DWORD, LPTHREAD_START_ROUTINE, LPVOID, DWORD,
    LPDWORD) {
  return INVALID_HANDLE_VALUE;
}

LPVOID GlobalAllocPtr(UINT fuFlags, DWORD cbBytes) {
  return malloc(cbBytes);
}

BOOL GlobalFreePtr(LPCVOID) {
  return TRUE;
}

BOOL PostThreadMessage(DWORD, UINT, WPARAM, LPARAM) {
  return TRUE;
}

BOOL SetThreadPriority(HANDLE, int) {
  return TRUE;
}

VOID Sleep(DWORD) {
}

DWORD WaitForSingleObject(HANDLE, DWORD) {
  return 0;
}

MMRESULT timeBeginPeriod(UINT) {
  return MMSYSERR_NOERROR;
}

MMRESULT timeEndPeriod(UINT) {
  return MMSYSERR_NOERROR;
}

DWORD timeGetTime() {
  return MMSYSERR_NOERROR;
}

MMRESULT timeKillEvent(UINT) {
  return MMSYSERR_NOERROR;
}

MMRESULT timeSetEvent(UINT, UINT, LPTIMECALLBACK, DWORD, UINT) {
  return MMSYSERR_NOERROR;
}

MMRESULT waveOutClose(HWAVEOUT) {
  return MMSYSERR_NOERROR;
}

MMRESULT waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh) {
  return MMSYSERR_NOERROR;
}

MMRESULT waveOutUnprepareHeader(HWAVEOUT, LPWAVEHDR, UINT) {
  return MMSYSERR_NOERROR;
}

MMRESULT waveOutOpen(LPHWAVEOUT, UINT, LPWAVEFORMATEX, DWORD, DWORD, DWORD) {
  return MMSYSERR_NOERROR;
}
