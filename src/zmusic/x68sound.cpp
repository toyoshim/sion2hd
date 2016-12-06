// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "x68sound.h"

#include "global.h"
#include "op.h"
#include "lfo.h"
#include "adpcm.h"
#include "pcm8.h"
#include "../../mod/opm.h"

Opm opm;

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
