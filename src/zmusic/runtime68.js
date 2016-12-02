// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

mergeInto(LibraryManager.library, {
  jsrt_dos_keepr: function(code) {
    self.postMessage({
      type: 'dos',
      func: 'keepr',
      code: code
    });
  },
  magic2_call: function(cmd_adr) {
    return -1;
  },
  zmusic_call: function() {
    return -1;
  }
});