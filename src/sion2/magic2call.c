// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "run68.h"

extern int jsrt_magic2(char* cmd_adr);

int magic2_call() {
  return jsrt_magic2(&prog_ptr[ra[0]]);
}

