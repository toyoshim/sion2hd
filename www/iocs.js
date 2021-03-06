// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* global magic2 */
/* global navigator */
/* global performance */
(function() {
  window.addEventListener('deviceorientation', function(e) {
    if (e.alpha)
      magic2.orientation(e.alpha, e.beta, e.gamma);
  }, false);

  var touchbits = 0xff;
  var touches = {
    esc: {
      id: -1,
      pressed: false
    },
    f4: {
      id: -1,
      pressed: false
    },
    f5: {
      id: -1,
      pressed: false
    },
    z: { id: -1 },
    x: { id: -1 },
    axes: {
      x: 0,
      ax: 0,
      y: 0,
      ay : 0,
      id: -1
    }
  };

  document.addEventListener('touchstart', function(e) {
    var w = window.innerWidth;
    var h = window.innerHeight;
    for (var i = 0; i < e.changedTouches.length; ++i) {
      var touch = e.changedTouches[i];
      if (touch.clientY < (h / 3)) {
        if (touch.clientX > (w * 2 / 3)) {
          touches.esc.id = touch.identifier;
          touches.esc.pressed = true;
        } else if (touch.clientX < (w * 2 / 3)) {
          touches.f4.id = touch.identifier;
          touches.f4.pressed = true;
        } else {
          touches.f5.id = touch.identifier;
          touches.f5.pressed = true;
        }
        return;
      }
      if (touch.clientX < (w / 2)) {
        touches.axes.id = touch.identifier;
        touches.axes.x = touch.clientX;
        touches.axes.y = touch.clientY;
        touches.axes.ax = 0;
        touches.axes.ay = 0;
      } else if (touch.clientX > (w * 4 / 5)) {
        touches.z.id = touch.identifier;
        touchbits &= ~0x40;
      } else {
        touches.x.id = touch.identifier;
        touchbits &= ~0x20;
      }
    }
  }, { passive: true, capture: false });

  document.addEventListener('touchend', function(e) {
    for (var i = 0; i < e.changedTouches.length; ++i) {
      var touch = e.changedTouches[i];
      var id = touch.identifier;
      if (id == touches.esc.id) {
        touches.esc.id = -1;
        touches.esc.pressed = false;
      } else if (id == touches.f5.id) {
        touches.f5.id = -1;
        touches.f5.pressed = false;
      } else if (id == touches.f4.id) {
        touches.f4.id = -1;
        touches.f4.pressed = false;
      } else if (id == touches.axes.id) {
        touches.axes.id = -1;
        touchbits |= 0x0f;
      } else if (id == touches.z.id) {
        touchbits |= 0x40;
        touches.z.id = -1;
      } else if (id == touches.x.id) {
        touchbits |= 0x20;
        touches.x.id = -1;
      }
    }
  }, { passive: true, capture: false });

  var updateTouch = function(a, d) {
    var offset = a * -4;
    if (d > (3 + offset))
      return 1;
    if (d < (-3 + offset))
      return -1;
    return 0;
  };

  document.addEventListener('touchmove', function(e) {
    for (var i = 0; i < e.changedTouches.length; ++i) {
      var touch = e.changedTouches[i];
      if (touch.identifier != touches.axes.id)
        continue;
      var dx = touches.axes.x - touch.clientX;
      touches.axes.x = touch.clientX;
      touches.axes.ax = updateTouch(touches.axes.ax, dx);
      if (touches.axes.ax < 0) {
        touchbits &= ~0x08;
        touchbits |= 0x04;
      } else if (touches.axes.ax > 0) {
        touchbits |= 0x08;
        touchbits &= ~0x04;
      } else {
        touchbits |= 0x0c;
      }
      var dy = touches.axes.y - touch.clientY;
      touches.axes.y = touch.clientY;
      touches.axes.ay = updateTouch(touches.axes.ay, dy);
      if (touches.axes.ay < 0) {
        touchbits &= ~0x02;
        touchbits |= 0x01;
      } else if (touches.axes.ay > 0) {
        touchbits |= 0x02;
        touchbits &= ~0x01;
      } else {
        touchbits |= 0x03;
      }
    }
  }, { passive: false, capture: true });

  var keyStates = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
  var keyRepeatStates = [];

  var emulateKeyRepeat = function(group, status) {
    if (!keyRepeatStates[group])
      keyRepeatStates[group] = [
        { pressed: false, count: 0 },
        { pressed: false, count: 0 },
        { pressed: false, count: 0 },
        { pressed: false, count: 0 },
        { pressed: false, count: 0 },
        { pressed: false, count: 0 },
        { pressed: false, count: 0 },
        { pressed: false, count: 0 }
    ];
    var now = performance.now();
    for (var i = 0; i < 8; ++i) {
      if (!keyRepeatStates[group][i].pressed && ((status >> i) & 1) != 0)
        keyRepeatStates[group][i].count = now;
      keyRepeatStates[group][i].pressed = ((status >> i) & 1) != 0;
      var count = now - keyRepeatStates[group][i].count;
      // Mask key while 50ms.
      if (0 < count && count < 50)
        status &= ~(1 << i);
    }
    // Mask ESC key to hook for native configuration.
    if (group == 0)
      return status & ~(1 << 1);
    return status;
  };

  var SafariWhichToCode = function (which) {
    switch (which) {
      case 27:
        return 'Escape';
      case 37:
        return 'ArrowLeft';
      case 38:
        return 'ArrowUp';
      case 39:
        return 'ArrowRight';
      case 40:
        return 'ArrowDown';
      case 51:
      case 112:
        return 'Digit1';
      case 52:
      case 113:
        return 'Digit2';
      case 53:
      case 114:
        return 'Digit3';
      case 54:
      case 115:
        return 'Digit4';
      case 55:
      case 116:
        return 'Digit5';
      case 86:
        return 'KeyV';
      case 88:
        return 'KeyX';
      case 90:
        return 'KeyZ';
    }
  }

  // ESC, F1-F5
  window.addEventListener('keydown', function(e) {
    if (!e.code) {
      if (e.which)
        e.code = SafariWhichToCode(e.which);
    }
    switch (e.code) {
      case 'ArrowDown':
        keyStates[7] |= (1 << 6);
        keyStates[9] |= (1 << 4);  // Num 2
        break;
      case 'ArrowLeft':
        keyStates[7] |= (1 << 3);
        keyStates[8] |= (1 << 7);  // Num 4
        break;
      case 'ArrowRight':
        keyStates[7] |= (1 << 5);
        keyStates[9] |= (1 << 1);  // Num 6
        break;
      case 'ArrowUp':
        keyStates[7] |= (1 << 4);
        keyStates[8] |= (1 << 4);  // Num 8
        break;
      case 'Escape':
      case 'KeyO':
        keyStates[0] |= (1 << 1);
        break;
      case 'F1':
      case 'Digit1':
        keyStates[12] |= (1 << 3);
        break;
      case 'F2':
      case 'Digit2':
        keyStates[12] |= (1 << 4);
        break;
      case 'F3':
      case 'Digit3':
        keyStates[12] |= (1 << 5);
        break;
      case 'F4':
      case 'Digit4':
        keyStates[12] |= (1 << 6);
        break;
      case 'F5':
      case 'Digit5':
        keyStates[12] |= (1 << 7);
        break;
      case 'KeyV':
        magic2.vr((magic2.vr() + 1) % 3);
        window.io_set_mode(magic2.vr());
        break;
      case 'KeyX':
        keyStates[5] |= (1 << 3);
        break;
      case 'KeyZ':
        keyStates[5] |= (1 << 2);
        break;
      default:
        //console.log(e);
        break;
    }
  }, false);

  window.addEventListener('keyup', function(e) {
    if (!e.code) {
      if (e.which)
        e.code = SafariWhichToCode(e.which);
    }
    switch (e.code) {
      case 'ArrowDown':
        keyStates[7] &= ~(1 << 6);
        keyStates[9] &= ~(1 << 4);  // Num 2
        break;
      case 'ArrowLeft':
        keyStates[7] &= ~(1 << 3);
        keyStates[8] &= ~(1 << 7);  // Num 4
        break;
      case 'ArrowRight':
        keyStates[7] &= ~(1 << 5);
        keyStates[9] &= ~(1 << 1);  // Num 6
        break;
      case 'ArrowUp':
        keyStates[7] &= ~(1 << 4);
        keyStates[8] &= ~(1 << 4);  // Num 8
        break;
      case 'Escape':
      case 'KeyO':
        keyStates[0] &= ~(1 << 1);
        break;
      case 'F1':
      case 'Digit1':
        keyStates[12] &= ~(1 << 3);
        break;
      case 'F2':
      case 'Digit2':
        keyStates[12] &= ~(1 << 4);
        break;
      case 'F3':
      case 'Digit3':
        keyStates[12] &= ~(1 << 5);
        break;
      case 'F4':
      case 'Digit4':
        keyStates[12] &= ~(1 << 6);
        break;
      case 'F5':
      case 'Digit5':
        keyStates[12] &= ~(1 << 7);
        break;
      case 'KeyX':
        keyStates[5] &= ~(1 << 3);
        break;
      case 'KeyZ':
        keyStates[5] &= ~(1 << 2);
        break;
      default:
        //console.log(e);
        break;
    }
  }, false);

  window.iocs_bitsns = function(group, raw) {
    var touch = 0;
    if (group == 0 && touches.esc.pressed)
      touch |= (1 << 1);
    if (group == 12 && touches.f4.pressed)
      touch |= (1 << 6);
    if (group == 12 && touches.f5.pressed)
      touch |= (1 << 7);
    var rawStatus = keyStates[group] | touch;
    if (raw)
      return rawStatus;
    return emulateKeyRepeat(group, rawStatus);
  };

  var contrast = {
    now: 15,
    target: 15
  };

  window.iocs_contrast = function(c) {
    contrast.target = c;
  };

  magic2.vsync(function () {
    if (contrast.now == contrast.target)
      return;
    if (contrast.now < contrast.target)
      contrast.now++;
    else
      contrast.now--;
    magic2.contrast(contrast.now);
  });

  window.iocs_joyget = function (id) {
    if (!navigator.getGamepads)
      return touchbits;
    var pad = navigator.getGamepads()[id];
    if (!pad)
      return touchbits;
    var bits = 0xff;
    var x = pad.axes[0] || pad.axes[2] || 0;
    var y = pad.axes[1] || pad.axes[3] || 0;
    var a = (pad.buttons[0] && pad.buttons[0].pressed) ||
        (pad.buttons[5] && pad.buttons[5].pressed);
    var b = (pad.buttons[1] && pad.buttons[1].pressed) ||
        (pad.buttons[6] && pad.buttons[6].pressed);
    if (pad.buttons[2] && pad.buttons[2].pressed && !magic2.vr()) {
        magic2.vr(true);
        window.io_set_mode(true);
    }
    if (pad.buttons[3] && pad.buttons[3].pressed && magic2.vr()) {
        magic2.vr(false);
        window.io_set_mode(false);
    }
    if (x < -0.5)
      bits &= ~0x04;
    else if (x > 0.5)
      bits &= ~0x08;
    if (y < -0.5)
      bits &= ~0x01;
    else if (y > 0.5)
      bits &= ~0x02;
    if (a)
      bits &= ~0x40;
    if (b)
      bits &= ~0x20;
    return bits & touchbits;
  };

  var sprites = new Array(128);
  for (var i = 0; i < 128; ++i) {
    sprites[i] = {
      x: 0,
      y: 0,
      id: 0,
      flipX: false,
      flipY: false,
      show: false,
      tick: 0
    };
  }
  var sprite = false;

  magic2.vsync(function(c, contexts) {
    if (!sprite)
      return;
    for (let i = 0; i < 128; ++i) {
      const s = sprites[i];
      if (s.id == 0 || s.id == 1) {
        // 2x2 / 1x1 star
        const x = s.x - 12;
        const y = s.y - 12;
        const z = 10000 - Math.abs(128 - y) * 100;
        const r = s.id ? 0.3 : 1.0;
        const d = z / 256;
        magic2.point3d(x * d, y * d, z, r);
      }
    }
    // TODO: Update following code to use magic2 extra call.
    for (let context of contexts) {
      var scaleX = c.canvas.height / 256 * context.aspect;
      var scaleY = c.canvas.height / 256;
      var offsetX = context.offset;
      for (var i = 0; i < 128; ++i) {
        var s = sprites[i];
        if (!s.show)
          continue;
        var x = s.x - 16;
        var y = s.y - 16;
        switch (s.id) {
          case 0:
          case 1:
            continue;
          case 7: {  // scope
            var scale = 1 + (s.tick++ % 10) / 10;
            var w = 16 * scaleX / scale;
            var h = 16 * scaleY / scale;
            var bx = x * scaleX + offsetX + (16 * scaleX - w) / 2;
            var by = y * scaleY + (16 * scaleY - h) / 2;
            c.strokeStyle = magic2.palette(11)[context.color];
            c.beginPath();
            c.moveTo(bx, by + h / 2);
            c.lineTo(bx, by + h / 5);
            c.lineTo(bx + w / 5, by);
            c.lineTo(bx + w / 2, by);
            c.moveTo(bx + w, by + h / 2);
            c.lineTo(bx + w, by + h * 4 / 5);
            c.lineTo(bx + w * 4 / 5, by + h);
            c.lineTo(bx + w / 2, by + h);
            c.moveTo(bx + w / 4, by + h / 2);
            c.lineTo(bx + w / 3, by + h * 2 / 3);
            c.lineTo(bx + w * 2 / 3, by + h * 2 / 3);
            c.lineTo(bx + w * 3 / 4, by + h / 2);
            c.moveTo(bx + w * 2 / 5, by + h / 2);
            c.lineTo(bx + w * 4 / 11, by + h * 4 / 7);
            c.moveTo(bx + w * 3 / 5, by + h / 2);
            c.lineTo(bx + w * 7 / 11, by + h * 4 / 7);
            c.moveTo(bx + w * 4 / 16, by + h * 12 / 16);
            c.lineTo(bx + w * 3 / 16, by + h * 13 / 16);
            c.moveTo(bx + w * 2 / 7, by + h / 3);
            c.lineTo(bx + w * 5 / 7, by + h / 3);
            c.stroke();
            c.closePath();
            c.strokeStyle = magic2.palette(5)[context.color];
            c.beginPath();
            c.moveTo(bx + w * 12 / 16, by + h * 12 / 16);
            c.lineTo(bx + w * 13 / 16, by + h * 13 / 16);
            c.stroke();
            c.closePath();
            continue; }
          default:
            console.warn('sprite ' + s.id + ' is not supported, showing a TOFU.')
          break;
        }
        // Default: TOFU.
        c.fillStyle = magic2.palette(15)[context.color];
        c.fillRect(
            x * scaleX + offsetX,
            y * scaleY,
            16 * scaleX,
            16 * scaleY);
      }
    }
  });

  window.iocs_sp_on = function() {
    sprite = true;
  };

  window.iocs_sp_off = function() {
    sprite = false;
  };
 
  window.iocs_sp_regst = function(id, x, y, code, prio) {
    var sprite = sprites[id & 0x1f];
    sprite.x = x & 0x3ff;
    sprite.y = y & 0x3ff;
    sprite.id = code & 0xff;
    sprite.flipX = (code & 0x4000) != 0;
    sprite.flipY = (code & 0x8000) != 0;
    sprite.show = prio != 0;
  };

  window.io_sprite_data = function(index, data) {
    var id = index >> 2;
    var sprite = sprites[id];
    switch (index & 3) {
      case 0:  // x
        sprite.x = data & 0x3ff;
        break;
      case 1:  // y
        sprite.y = data & 0x3ff;
        break;
      case 2:  // code
        sprite.id = data & 0xff;
        sprite.flipX = (data & 0x4000) != 0;
        sprite.flipY = (data & 0x8000) != 0;
        break;
      case 3:  // prio
        if (!sprite.show && data != 0)
          sprite.tick = 0;
        sprite.show = data != 0;
        break;
    }
  };

  var bg = [null, null];
  var bgtext =
      '_________________________@______' +  // 0x00-0x1F
      " !'#$%&#()*+,-./0123456789:;<=>?" +  // 0x20-0x3F
      'ABCDEFGHIJKLMNOPQRSTUVWXYZ@_____' +  // 0x40-0x5F
      '____@@@@@@@@____________________' +  // 0x60-0x7F
      '________________________________' +  // 0x80-0x9F
      '________________________________' +  // 0xA0-0xBF
      '________________________________' +  // 0xC0-0xDF
      '________________________________';   // 0xE0-0xFF

  window.bg_update = function(c, page, context) {
    var scaleX = c.canvas.height / 256 * context.aspect * 8;
    var scaleY = c.canvas.height / 256 * 8;
    var offsetX = context.base - context.position / 3
        + (context.width - c.canvas.height * context.aspect) / 2;
    c.textAlign = 'center';
    c.textBaseline = 'middle';
    var styleW = magic2.palette(bg[page].fg)[context.color];
    var styleB = magic2.palette(11)[context.color];
    var styleR = magic2.palette( 5)[context.color];
    var fontA = scaleY + 'px \'Audiowide\'';
    var fontF = scaleY + 'px \'Fira Mono\'';
    var pattern = bg[page].pattern;
    for (var i = 0; i < pattern.length; ++i) {
      var style = styleW;
      var font = fontA;
      var ix = i % 64;
      var iy = (i / 64) | 0;
      var id = pattern[i].id;
      if (id == 1)
        continue;
      var chr = bgtext[id];
      //if (chr == '_')
      //  continue;
      if (chr == '@') {
        switch (id) {
          case 0x19:
            font = fontF;
            style = styleB;
            chr = '■';
            break;
          case 0x5A:
            chr = 'II';
            break;
          case 0x64:
          case 0x68:
            font = fontF;
            style = styleR;
            chr = '⬅';
            break;
          case 0x65:
          case 0x69:
            font = fontF;
            style = styleR;
            chr = '➡';
            break;
          case 0x66:
          case 0x6A:
            font = fontF;
            style = styleR;
            chr = '⬆';
            break;
          case 0x67:
          case 0x6B:
            font = fontF;
            style = styleR;
            chr = '⬇';
            break;
        }
      }
      var x = (ix - bg[page].scroll.x / 8) * scaleX + offsetX;
      var y = (iy - bg[page].scroll.y / 8) * scaleY;
      c.font = font;
      c.fillStyle = style;
      c.fillText(chr, x + scaleX / 2, y + scaleY / 2, scaleX);
    }
  };

  // TODO: Not to draw on each frame, but use a dedicated canvas.
  magic2.vsync(function(c, contexts) {
    for (let context of contexts)
      window.bg_update(c, 0, context);
  });

  window.iocs_bgscrlst = function(page, x, y) {
    page &= 1;
    bg[page].scroll.x = x;
    bg[page].scroll.y = y;
  };

  window.iocs_bgtextcl = function(page, code) {
    page &= 1;
    bg[page] = {
      fg: 15,
      pattern: new Array(64 * 64),
      scroll: {
        x: 0,
        y: 0
      }
    };
    var id = code & 0xff;
    var flipX = (code & 0x4000) != 0;
    var flipY = (code & 0x8000) != 0;
    var pattern = bg[page].pattern;
    for (var i = 0; i < pattern.length; ++i) {
      pattern[i] = {
        id: id,
        flipX: flipX,
        flipY: flipY
      };
    }
  };
  window.iocs_bgtextcl(0, 1);
  window.iocs_bgtextcl(1, 1);

  window.iocs_bgtextst = function(page, x, y, code) {
    page &= 1;
    var index = (y & 0x3f) * 64 + (x & 0x3f);
    var id = code & 0xff;
    var flipX = (code & 0x4000) != 0;
    var flipY = (code & 0x8000) != 0;
    var pattern = bg[page].pattern;
    pattern[index].id = id;
    pattern[index].flipX = flipX;
    pattern[index].flipY = flipY;
  };
})();
