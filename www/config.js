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
    putc(5, options[optionKeys[i]].y, ' ');
  putc(5, options[optionKeys[select]].y, '>');
  if (magic2.vr()) {
    window.bg_update(context, 1, magic2.context(1));
    window.bg_update(context, 1, magic2.context(2));
  } else {
    window.bg_update(context, 1, magic2.context(0));
  }
};

var keys = { up: false, down: false, left: false, right: false };

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
  if (!window.config.shown) {
    var key0 = window.iocs_bitsns(0, true);
    var pad = navigator.getGamepads && navigator.getGamepads()[0];
    if ((key0 & (1 << 1)) || (pad && pad.buttons[9] && pad.buttons[9].pressed)) {
      draw();
      window.config.show();
    }
  }

  if (!window.config.shown)
    return;

  var key7 = window.iocs_bitsns(7, true);
  var up = 0 != (key7 & (1 << 4));
  var down = 0 != (key7 & (1 << 6));
  var left = 0 != (key7 & (1 << 3));
  var right = 0 != (key7 & (1 << 5));
  var joy = window.iocs_joyget(0);
  up |= (joy & 0x01) == 0;
  down |= (joy & 0x02) == 0;
  left |= (joy & 0x04) == 0;
  right |= (joy & 0x08) == 0;
  var newKeys = { up: up, down: down, left: left, right: right };
  up &= !keys.up;
  down &= !keys.down;
  left &= !keys.left;
  right &= !keys.right;
  keys = newKeys;
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
  if (optionKeys[select] == 'exit' &&
      (window.iocs_bitsns(5) & (1 << 2)) || (joy & 0x60) != 0x60) {
    window.config.hide();
  }
};

var strings = {
  title1:  { on:  true, x: 10, y:  3, text: 'SION\x5b HD' },
  title2:  { on:  true, x:  9, y:  5, text: '- OPTIONS -' },
  sounds:  { on:  true, x:  7, y:  8, text: 'SOUND EMULATION' },
  sound1:  { on:  true, x:  7, y: 10, text: '      OFF      ' },
  sound2:  { on: false, x:  7, y: 10, text: '   X68SOUND    ' },
  sound3:  { on: false, x:  7, y: 10, text: 'X68SOUND+REVERB' },
  speeds:  { on:  true, x:  9, y: 13, text: 'GAME SPEED' },
  speed1:  { on:  true, x: 10, y: 15, text: ' NORMAL  ' },
  speed2:  { on:  true, x: 10, y: 15, text: '  SLOW   ' },
  speed3:  { on:  true, x: 10, y: 15, text: 'VERY SLOW' },
  options: { on:  true, x:  8, y: 18, text: 'SKIP OPTIONS' },
  option1: { on:  true, x: 13, y: 20, text: 'OFF' },
  option2: { on: false, x: 13, y: 20, text: 'ON ' },
  exit:    { on:  true, x: 12, y: 23, text: 'EXIT' },
  copy1:   { on:  true, x:  9, y: 28, text: '2016\x22DEC SION\x5b HD' },
  copy2:   { on:  true, x:  4, y: 29, text: 'BY TOYOSHIMA-HOUSE' },
};

var optionKeys = ['sound', 'speed', 'option', 'exit'];
var options = {
  sound: {
    y: strings['sounds'].y, i: 0, entries: ['sound1', 'sound2', 'sound3'] },
  speed: {
    y: strings['speeds'].y, i: 1, entries: ['speed1', 'speed2', 'speed3'] },
  option: { y: strings['options'].y, i: 0, entries: ['option1', 'option2'] },
  exit: { y: strings['exit'].y, i: 0, entries: [] },
};

var select = 0;
var opacity = 0;
var resolver = null;
var paused = false;

window.config = {
  ready: new Promise(function(resolve, reject) {
    for (var select = 0; select < optionKeys.length; ++select) {
      var key = optionKeys[select];
      if (key == 'exit')
        continue;
      options[key].i = Number(getConfig('sion2hd-' + key, options[key].i));
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
    paused = true;
  },

  hide: function() {
    window.config.shown = false;
    for (var select = 0; select < optionKeys.length; ++select) {
      var key = optionKeys[select];
      if (key == 'exit')
        continue;
      setConfig('sion2hd-' + key, options[key].i);
    }
    if (paused)
      window.Module.resumeMainLoop();
    if (window.config.onupdate)
      window.config.onupdate();
  },

  draw: function() {
    draw();
  },

  useSound: function() {
    return options['sound'].i != 0;
  },
  useReverb: function() {
    return options['sound'].i == 2;
  },
  slowMode: function() {
    return options['speed'].i;
  },
  shown: false,
  onupdate: null
};

})();
