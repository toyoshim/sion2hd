// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* global localStorage */
/* global magic2 */
/* global window */
(function () {

var getConfig = function(key, def) {
  return localStorage.getItem(key) || def;
};

var setConfig = function(key, value) {
  localStorage.setItem(key, value);
};

var putc = function(x, y, c) {
  var n = c.charCodeAt(0);
  if (0x40 <= n)
    n--;
  window.iocs_bgtextst(1, x, y, n);
};

var puts = function(x, y, s) {
  for (var i = 0; i < s.length; ++i)
    putc(x + i, y, s[i]);
};

var draw = function() {
  var canvas = document.getElementById('bg2');
  canvas.style.opacity = opacity / 100;
  var context = canvas.getContext('2d');
  context.clearRect(0, 0, canvas.width, canvas.height);
  context.fillStyle = 'rgba(0, 0, 0, 0.8)';
  context.fillRect(0, 0, canvas.width, canvas.height);

  for (var name in strings) {
    var s = strings[name];
    if (s.on)
      puts(s.x, s.y, s.text);
  }
  for (var i = 0; i < optionKeys.length; ++i)
    putc(7, options[optionKeys[i]].y, ' ');
  putc(7, options[optionKeys[select]].y, '>');
  window.bg_update(context, 1);
};

var update = function() {
  requestAnimationFrame(update);
  if (!window.config.shown && opacity != 0) {
    opacity -= 5;
    document.getElementById('bg2').style.opacity = opacity / 100;
    if (opacity == 0 && resolver) {
      resolver();
      resolver = null;
    }
  } else if (window.config.shown && opacity != 100) {
    opacity += 5;
    document.getElementById('bg2').style.opacity = opacity / 100;
  }
  var key0 = window.iocs_bitsns(0, true);
  if (key0 & (1 << 1) && !window.config.shown)
    window.config.show();

  if (!window.config.shown)
    return;

  var key7 = window.iocs_bitsns(7);
  var up = 0 != (key7 & (1 << 4));
  var down = 0 != (key7 & (1 << 6));
  var left = 0 != (key7 & (1 << 3));
  var right = 0 != (key7 & (1 << 5));
  select += optionKeys.length + (up ? -1 : down ? 1 : 0);
  select %= optionKeys.length;
  var option = options[optionKeys[select]];
  var item = option.i;
  item += option.entries.length + (left ? -1 : right ? 1 : 0);
  item %= option.entries.length;
  option.i = item;
  for (var i = 0; i < option.entries.length; ++i)
    strings[option.entries[i]].on = i == item;
  if (up | down | left | right)
    draw();
  if (left | right)
    window.config.onupdate();
  if (select == 2 && window.iocs_bitsns(5) & (1 << 2))
    window.config.hide();
};

var strings = {
  title1:  { on:  true, x: 12, y:  5, text: 'SION\x5b HD' },
  title2:  { on:  true, x: 11, y:  7, text: '- OPTIONS -' },
  sounds:  { on:  true, x:  9, y: 11, text: 'SOUND EMULATION' },
  sound1:  { on:  true, x:  9, y: 13, text: '      OFF      ' },
  sound2:  { on: false, x:  9, y: 13, text: '    X68SOUND   ' },
  sound3:  { on: false, x:  9, y: 13, text: 'X68SOUND+REVERB' },
  options: { on:  true, x: 10, y: 16, text: 'SKIP OPTIONS' },
  option1: { on:  true, x: 15, y: 18, text: 'OFF' },
  option2: { on: false, x: 15, y: 18, text: 'ON ' },
  exit:    { on:  true, x: 14, y: 21, text: 'EXIT' },
  copy1:   { on:  true, x:  9, y: 28, text: '2016\x22DEC SION\x5b HD' },
  copy2:   { on:  true, x:  7, y: 29, text: 'BY TOYOSHIMA-HOUSE' },
};

var optionKeys = ['sound', 'option', 'exit'];
var options = {
  sound: { y: 11, i: 0, entries: ['sound1', 'sound2', 'sound3'] },
  option: { y: 16, i: 0, entries: ['option1', 'option2'] },
  exit: { y: 21, i: 0, entries: [] },
};

var select = 0;
var opacity = 0;
var resolver = null;

window.config = {
  ready: new Promise(function(resolve, reject) {
    for (var select = 0; select < optionKeys.length; ++select) {
      var key = optionKeys[select];
      if (key == 'exit')
        continue;
      options[key].i = Number(getConfig('sion2hd-' + key, 0));
      for (var i = 0; i < options[key].entries.length; ++i)
        strings[options[key].entries[i]].on = i == options[key].i;
    }
    document.fonts.ready.then(function() {
      draw();
      if (options['option'].i == 0) {
        resolver = resolve;
        window.config.shown = true;
      } else {
        resolve();
      }
      requestAnimationFrame(update);
    });
  }),

  show: function() {
    window.config.shown = true;
    window.Module.pauseMainLoop();
  },

  hide: function() {
    window.config.shown = false;
    for (var select = 0; select < optionKeys.length; ++select) {
      var key = optionKeys[select];
      if (key == 'exit')
        continue;
      setConfig('sion2hd-' + key, options[key].i);
    }
    window.Module.resumeMainLoop();
    if (window.config.onupdate)
      window.config.onupdate();
  },

  useSound: function() {
    return options['sound'].i != 0;
  },
  useReverb: function() {
    return options['sound'].i == 2;
  },
  shown: false,
  onupdate: null
};

})();
