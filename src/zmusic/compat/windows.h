// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _WINDOWS_H_
#define _WINDOWS_H_

#include <stdint.h>

// Windows core type definitions.
typedef void* LPVOID;
typedef LPVOID HANDLE;
typedef HANDLE HWAVEOUT;
typedef char* LPSTR;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef DWORD* LPDWORD;
typedef uint32_t UINT;
typedef int32_t MMRESULT;
#ifndef __int64
# define __int64 long long
#endif

// WINMM specific type definitions.
struct _WAVEFORMATEX {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
  WORD  wBitsPerSample;
  WORD  cbSize;
};
typedef struct _WAVEFORMATEX WAVEFORMATEX;
typedef WAVEFORMATEX* LPWAVEFORMATEX;

struct _WAVEHDR {
  LPSTR lpData;
  DWORD dwBufferLength;
  DWORD dwBytesRecorded;
  LPDWORD dwUser;
  DWORD dwFlags;
  DWORD dwLoops;
  struct _WAVEHDR* lpNext;
  LPDWORD reserved;
};
typedef struct _WAVEHDR WAVEHDR;
typedef WAVEHDR* LPWAVEHDR;

// Dummy macro definitions.
#define WINAPI
#define CALLBACK

#define CALLBACK_FUNCTION NULL

#define MMSYSERR_NOERROR 0

#define WAVE_FORMAT_PCM 0
#define WAVE_MAPPER 0

#define GMEM_MOVEABLE 0
#define GMEM_SHARE 0

#define WM_USER 0

// Define _WIN64 to disable inline asm.
#define _WIN64

// Dependent functions.
uint32_t _byteswap_ulong(uint32_t);
uint16_t _byteswap_ushort(uint16_t);
DWORD timeGetTime();
void _InterlockedIncrement(volatile long*);
void _InterlockedDecrement(volatile long*);
long _InterlockedCompareExchange(volatile long*, long, long);
LPVOID GlobalAllocPtr(UINT fuFlags, DWORD cbBytes);
MMRESULT waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);

#endif  // _WINDOWS_H_
