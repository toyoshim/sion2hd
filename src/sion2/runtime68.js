// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

mergeInto(LibraryManager.library, {
  jsrt_magic2: function(cmd_adr) {
    if (magic2.auto(Module.HEAPU8, cmd_adr))
      return 5;
    return 0;
  },
  jsrt_iocs_bgscrlst: function(page, x, y) {
    iocs_bgscrlst(page, x, y);
  },
  jsrt_iocs_bgtextcl: function(page, code) {
    iocs_bgtextcl(page, code);
  },
  jsrt_iocs_bgtextst: function(page, x, y, code) {
    iocs_bgtextst(page, x, y, code);
  },
  jsrt_iocs_bitsns: function(group) {
    return iocs_bitsns(group);
  },
  jsrt_iocs_contrast: function(c) {
    iocs_contrast(c);
  },
  jsrt_iocs_joyget: function(id) {
    return iocs_joyget(id);
  },
  jsrt_iocs_sp_on: function () {
    iocs_sp_on();
  },
  jsrt_iocs_sp_off: function () {
    iocs_sp_off();
  },
  jsrt_iocs_sp_regst: function (id, x, y, code, prio) {
    iocs_sp_regst(id, x, y, code, prio);
  },
  jsrt_io_graphic_data: function(page, index, color) {
    io_graphic_data(page, index, color);
  },
  jsrt_io_graphic_palette: function(index, color) {
    magic2.palette(index, color);
  },
  jsrt_io_sprite_data: function(index, data) {
    io_sprite_data(index, data);
  }
});