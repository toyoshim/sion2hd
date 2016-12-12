// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "x68sound.h"

#include "global.h"
#include "op.h"
#include "lfo.h"
#include "adpcm.h"
#include "pcm8.h"
#include "opm.h"

Opm opm;

void OpmTimeProc(UINT, UINT, DWORD, DWORD, DWORD) {
}

void waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD) {
}

DWORD waveOutThread(LPVOID) {
  return 0;
}

int X68Sound_StartPcm(int samprate, int opmflag, int adpcmflag, int pcmbuf) {
  return opm.StartPcm(samprate, opmflag, adpcmflag, pcmbuf);
}

int X68Sound_GetPcm(void* buf, int len) {
  return opm.GetPcm(buf, len);
}

unsigned char X68Sound_OpmPeek() {
  return opm.OpmPeek();
}

void X68Sound_OpmReg(unsigned char no) {
  opm.OpmReg(no);
}

void X68Sound_OpmPoke(unsigned char data) {
  opm.OpmPoke(data);
}

void X68Sound_OpmInt(void (CALLBACK* proc)()) {
  opm.OpmInt(proc);
}

int X68Sound_Pcm8_Out(int ch, void* adrs, int mode, int len) {
  return opm.Pcm8_Out(ch, adrs, mode, len);
}

int X68Sound_Pcm8_Abort() {
  return opm.Pcm8_Abort();
}
