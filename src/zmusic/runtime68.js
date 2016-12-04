// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

mergeInto(LibraryManager.library, {
  jsrt_dos_keepr: function(code) {
    zmusic_ready(code);
  },
  magic2_call: function(cmd_adr) {
    console.error("magic2_call: should not be called.");
    return -1;
  },
  zmusic_call: function() {
    console.error("zmusic_call: should not be called.");
    return -1;
  }
});