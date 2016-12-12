// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _WINDOWS_H_
#define _WINDOWS_H_

#include <stdint.h>

// Windows core type definitions.
typedef void VOID;
typedef VOID* LPVOID;
typedef const VOID* LPCVOID;
typedef LPVOID HANDLE;
typedef LPVOID LPSECURITY_ATTRIBUTES;
typedef HANDLE HWAVEOUT;
typedef HWAVEOUT* LPHWAVEOUT;
typedef char* LPSTR;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t WPARAM;
typedef uint32_t LPARAM;
typedef DWORD* LPDWORD;
typedef uint32_t UINT;
typedef int32_t MMRESULT;
typedef DWORD (*LPTHREAD_START_ROUTINE)(LPVOID);
typedef void (*LPTIMECALLBACK)(UINT, UINT, DWORD, DWORD, DWORD);

#ifndef __int64
# define __int64 long long
#endif
#ifndef BOOL
# define BOOL int
#endif
#ifndef TRUE
# define TRUE -1
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
#define CALLBACK
#define CALLBACK_FUNCTION NULL
#define GMEM_MOVEABLE 0
#define GMEM_SHARE 0
#define INFINITE 0
#define INVALID_HANDLE_VALUE (HANDLE)(-1)
#define MMSYSERR_NOERROR 0
#define THREAD_PRIORITY_HIGHEST 0
#define TIME_PERIODIC 0
#define WAVE_FORMAT_PCM 0
#define WAVE_MAPPER 0
#define WINAPI
#define WM_USER 0

// Define _WIN64 to disable inline asm.
#define _WIN64

// Dependent functions.
uint32_t _byteswap_ulong(uint32_t);
uint16_t _byteswap_ushort(uint16_t);
void _InterlockedIncrement(volatile long*);
void _InterlockedDecrement(volatile long*);
long _InterlockedCompareExchange(volatile long*, long, long);
BOOL CloseHandle(HANDLE);
HANDLE CreateThread(
  LPSECURITY_ATTRIBUTES, DWORD, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
LPVOID GlobalAllocPtr(UINT, DWORD);
BOOL GlobalFreePtr(LPCVOID);
BOOL PostThreadMessage(DWORD, UINT, WPARAM, LPARAM);
BOOL SetThreadPriority(HANDLE, int);
VOID Sleep(DWORD);
DWORD WaitForSingleObject(HANDLE, DWORD);
MMRESULT timeBeginPeriod(UINT);
MMRESULT timeEndPeriod(UINT);
DWORD timeGetTime();
MMRESULT timeKillEvent(UINT);
MMRESULT timeSetEvent(UINT, UINT, LPTIMECALLBACK, DWORD, UINT);
MMRESULT waveOutClose(HWAVEOUT);
MMRESULT waveOutPrepareHeader(HWAVEOUT, LPWAVEHDR, UINT);
MMRESULT waveOutUnprepareHeader(HWAVEOUT, LPWAVEHDR, UINT);
MMRESULT waveOutOpen(LPHWAVEOUT, UINT, LPWAVEFORMATEX, DWORD, DWORD, DWORD);

#endif  // _WINDOWS_H_
