// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(function (global) {
'use strict';

// A special symbol to hide private members.
const _ = Symbol();

// MAGIC2 Commands.
const C_LINE = 0x00;
const C_SPLINE = 0x01;
const C_BOX = 0x02;
const C_TRIANGLE = 0x03;
const C_BOX_FULL = 0x04;
const C_CIRCLE_FULL = 0x05;
const C_SET_WINDOW = 0x06;
const C_SET_MODE = 0x07;
const C_POINT = 0x08;
const C_CLS = 0x09;
const C_SET_3D_PARAMETER = 0x0b;
const C_SET_3D_DATA = 0x0c;
const C_TRANSLATE_3D_TO_2D = 0x0d;
const C_DISPLAY_2D = 0x0e;
const C_DONE = 0x0f;
const C_COLOR = 0x10;
const C_CRT = 0x11;
const C_INIT = 0x12;
const C_AUTO = 0x13;
const C_APAGE = 0x14;
const C_DEPTH = 0x15;

// 3D Parameters.
const P_CX = 0;
const P_CY = 1;
const P_CZ = 2;
const P_DX = 3;
const P_DY = 4;
const P_DZ = 5;
const P_HEAD = 6;
const P_PITCH = 7;
const P_BANK = 8;

/**
 * Decodes an unsigned 16-bit number in big endian.
 * @param {Uint8Array} memory memory image
 * @param {Number} addr memory address
 * @param {Number} read value
 */
const mem_read_u16be = function (memory, addr) {
  return (memory[addr] << 8) | memory[addr + 1];
}

/**
 * Decodes a signed 16-bit number in big endian.
 * @param {Uint8Array} memory memory image
 * @param {Number} addr memory address
 * @param {Number} read value
 */
const mem_read_s16be = function (memory, addr) {
  const u16be = mem_read_u16be(memory, addr);
  if (u16be < 0x8000)
    return u16be;
  return u16be - 0x10000;
}

class Magic2 {
  // constructor
  // @param {CanvasRenderingContext2D} context
  constructor (contexts) {
    this[_] = {
      // private members
      window: {
        x: 0,
        y: 0,
        w: 0,
        h: 0
      },
      depth: {
        minz: 50,
        maxz: 2000,
      },
      cext: false,
      color: 15,
      parameters: [0, 0, 0, 0, 0, 0, 0, 0, 0],
      data: {
        pct: 0,
        vertices: new Int16Array(8192 * 3),
        lct: 0,
        indices: new Uint16Array(8192 * 2),
        color: 0
      },
      translate: {
        vertices: new Float32Array(8192 * 3),
        indices: new Float32Array(8192 * 2)
      },
      palette: [
        'rgba(  0,   0,   0, 1.0)',
        'rgba( 85,  85,  85, 1.0)',
        'rgba(  0,   0, 127, 1.0)',
        'rgba(  0,   0, 255, 1.0)',
        'rgba(127,   0,   0, 1.0)',
        'rgba(255,   0,   0, 1.0)',
        'rgba(127,   0, 127, 1.0)',
        'rgba(255,   0, 255, 1.0)',
        'rgba(  0, 127,   0, 1.0)',
        'rgba(  0, 255,   0, 1.0)',
        'rgba(  0, 127, 127, 1.0)',
        'rgba(  0, 255, 255, 1.0)',
        'rgba(127, 127,   0, 1.0)',
        'rgba(255, 255,   0, 1.0)',
        'rgba(170, 170, 170, 1.0)',
        'rgba(255, 255, 255, 1.0)'
      ],
      contexts: contexts,
      fgcontext: 0,
      bgcontext: 1,
      clients: [],
      apage: 0,
      vr: false,
      draw: function (context) {
        // FIXME: Use const for var other than 'i', and use let for 'i'.

        // Rotate and translate
        var math = Math;
        var src = this[_].data.vertices;
        var pctx3 = this[_].data.pct * 3;
        var vertices = this[_].translate.vertices;
        var cx = this[_].parameters[P_CX] - context.position;
        var cy = this[_].parameters[P_CY];
        var cz = this[_].parameters[P_CZ];
        var dx = this[_].parameters[P_DX];
        var dy = this[_].parameters[P_DY];
        var dz = this[_].parameters[P_DZ];
        var rh = this[_].parameters[P_HEAD] / 180 * math.PI;
        var rp = this[_].parameters[P_PITCH] / 180 * math.PI;
        var rb = this[_].parameters[P_BANK] / 180 * math.PI;
        var ch = math.cos(rh);
        var cp = math.cos(rp);
        var cb = math.cos(rb);
        var sh = math.sin(rh);
        var sp = math.sin(rp);
        var sb = math.sin(rb);
        var m11 = sh * sp * sb + ch * cb;
        var m12 = sb * cp;
        var m13 = ch * sp * sb - sh * cb;
        var m14 = dx + cx;
        var m21 = sh * sp * cb - sb * ch;
        var m22 = cp * cb;
        var m23 = ch * sp * cb + sh * sb;
        var m24 = dy + cy;
        var m31 = sh * cp;
        var m32 = -sp;
        var m33 = ch * cp;
        var m34 = dz + cz + this[_].depth.minz;
        var i;
        for (i = 0; i < pctx3; i += 3) {
          var x = src[i + 0] - dx;
          var y = src[i + 1] - dy;
          var z = src[i + 2] - dz;
          vertices[i + 0] = m11 * x + m12 * y + m13 * z + m14;
          vertices[i + 1] = m21 * x + m22 * y + m23 * z + m24;
          vertices[i + 2] = m31 * x + m32 * y + m33 * z + m34;
        }
        // Perspective
        var maxz = this[_].depth.maxz;
        for (i = 0; i < pctx3; i += 3) {
          var nz = vertices[i + 2];
          if (nz < 0 || maxz < nz)
            continue;
          var d = nz / 256;
          vertices[i + 0] /= d;
          vertices[i + 1] /= d;
        }
        // Draw
        var indices = this[_].data.indices;
        var lctx2 = this[_].data.lct * 2;
        var c = this[_].contexts[this[_].bgcontext];
        c.save();
        c.beginPath();
        c.rect(context.base, 0, context.width, c.canvas.height);
        c.clip();
        c.closePath();
        c.beginPath();
        c.strokeStyle = this[_].palette[this[_].data.color];
        var w = 256 * context.scaleX;
        var h = 256 * context.scaleY;
        var ox = context.base + context.width / 2;
        var oy = h / 2;
        // FIXME: Use window information
        var zx = w / 256;
        var zy = h / 256;
        for (i = 0; i < lctx2; i += 2) {
          var s = indices[i + 0] * 3;
          var e = indices[i + 1] * 3;
          var sz = vertices[s + 2];
          var ez = vertices[e + 2];
          if (sz < 0 || maxz < sz || ez < 0 || maxz < ez)
            continue;
          c.moveTo(ox + vertices[s + 0] * zx, oy + vertices[s + 1] * zy);
          c.lineTo(ox + vertices[e + 0] * zx, oy + vertices[e + 1] * zy);
        }
        c.closePath();
        c.stroke();
        c.restore();
      }.bind(this)
    };

    const fg = this[_].contexts[this[_].fgcontext];
    fg.fillStyle = this[_].palette[0];
    fg.fillRect(0, 0, fg.canvas.width, fg.canvas.height);
    fg.canvas.style.display = 'block';

    const bg = this[_].contexts[this[_].bgcontext];
    bg.fillStyle = this[_].palette[0];
    bg.fillRect(0, 0, fg.canvas.width, fg.canvas.height);
    bg.canvas.style.display = 'none';
  }

  palette (index, color) {
    if (color == undefined)
      return this[_].palette[index];
    var i = (color & 1) == 0 ? 0 : 4;
    var r = (((color >>  6) & 0x1f) << 3) + i;
    var g = (((color >> 11) & 0x1f) << 3) + i;
    var b = (((color >>  1) & 0x1f) << 3) + i;
    this[_].palette[index] = 'rgba(' + r + ',' + g + ',' + b + ',1.0)';
  }

  vr (mode) {
    if (mode === undefined)
      return this[_].vr;
    var result = this[_].vr;
    this[_].vr = mode;
    return result;
  }

  context(mode) {
    var c = this[_].contexts[0].canvas;
    var base = mode != 2 ? 0 : c.width / 2;
    var width = mode == 0 ? c.width : c.width / 2;
    var aspect = mode == 0 ? 4 / 3 : 1;
    return {
      base: base,
      width: width,
      offset: base + (width - (c.height * aspect)) / 2,
      position: mode == 0 ? 0 : mode == 1 ? -10 : 10,
      aspect: aspect,
      scaleX: c.height / 256 * aspect,
      scaleY: c.height / 256
    };
  }

  vsync (client) {
    this[_].clients.push(client);
  }

  boxFull (x1, y1, x2, y2) {
    const left = Math.min(x1, x2);
    const top = Math.min(y1, y2);
    const width = Math.abs(x2 - x1);
    const height = Math.abs(y2 - y1);
    const c = this[_].contexts[this[_].apage];
    c.fillStyle = this[_].palette[this[_].color];
    if (this[_].vr) {
      var c1 = this.context(1);
      c.fillRect(left * c1.scaleX + c1.offset,
                 top * c1.scaleY,
                 width * c1.scaleX,
                 height * c1.scaleY);
      var c2 = this.context(2);
      c.fillRect(left * c2.scaleX + c2.offset,
                 top * c2.scaleY,
                 width * c2.scaleX,
                 height * c2.scaleY);
    } else {
      var context = this.context(0);
      c.fillRect(left * context.scaleX + context.offset,
                 top * context.scaleY,
                 width * context.scaleX,
                 height * context.scaleY);
    }
  }

  setWindow (x1, y1, x2, y2) {
    this[_].window.x = x1;
    this[_].window.y = y1;
    this[_].window.w = x2 - x1;
    this[_].window.h = y2 - y1;
  }

  cls () {
    const c = this[_].contexts[this[_].apage];
    c.clearRect(0, 0, c.canvas.width, c.canvas.height);
  }

  set3dParameter (num, data) {
    this[_].parameters[num] = data;
  }

  set3dData (pct, vertices, lct, indices, color) {
    this[_].data.pct = pct;
    this[_].data.vertices = vertices;
    this[_].data.lct = lct;
    this[_].data.indices = indices;
    this[_].data.color = color !== undefined ? color : this[_].color;
  }

  set3dRawData (memory, addr) {
    const base = addr;
    this[_].data.pct = mem_read_u16be(memory, addr);
    addr += 2;
    for (let i = 0; i < this[_].data.pct; ++i) {
      this[_].data.vertices[i * 3 + 0] = mem_read_s16be(memory, addr + 0);
      this[_].data.vertices[i * 3 + 1] = mem_read_s16be(memory, addr + 2);
      this[_].data.vertices[i * 3 + 2] = mem_read_s16be(memory, addr + 4);
      addr += 6;
    }
    this[_].data.lct = mem_read_u16be(memory, addr);
    addr += 2;
    if (this[_].cext) {
      this[_].data.color = mem_read_u16be(memory, addr) & 0x0f;
      addr += 2;
    } else {
      this[_].data.color = this[_].color;
    }
    for (let i = 0; i < this[_].data.lct; ++i) {
      this[_].data.indices[i * 2 + 0] = mem_read_u16be(memory, addr + 0);
      this[_].data.indices[i * 2 + 1] = mem_read_u16be(memory, addr + 2);
      addr += 4;
    }
    return addr - base;
  }

  translate3dTo2d () {
    if (this[_].vr) {
      this[_].draw(this.context(1));
      this[_].draw(this.context(2));
    } else {
      this[_].draw(this.context(0));
    }
  }

  display2d () {
    const previous = this[_].fgcontext;
    this[_].fgcontext = this[_].bgcontext;
    this[_].bgcontext = previous;
    const fg = this[_].contexts[this[_].fgcontext];
    if (this[_].vr) {
      var c1 = this.context(1);
      var c2 = this.context(2);
      for (var client of this[_].clients) {
        client(fg, c1);
        client(fg, c2);
      }
    } else {
      var c = this.context(0);
      for (var client of this[_].clients)
        client(fg, c);
    }
    fg.canvas.style.display = 'block';
    const bg = this[_].contexts[this[_].bgcontext];
    bg.canvas.style.display = 'none';
    bg.fillStyle = this[_].palette[0];
    bg.fillRect(0, 0, bg.canvas.width, bg.canvas.height);
  }

  color (color) {
    this[_].color = color;
  }

  crt (crt) {
    console.warn('magic2: partially ignoring command CRT, ' + crt);
    this[_].cext = (crt & 0x100) != 0;
  }

  /**
   * Executes magic commands stored in the specified memory.
   * @param {Uint8Array} memory image
   * @param {Number} addr memory address
   * @return {Boolean} true if canvas is updated and it requires waiting vsync
   */
  auto (memory, addr) {
    let shown = false;
    for (;;) {
      var cmd = mem_read_u16be(memory, addr);
      addr += 2;
      switch (cmd) {
        case C_LINE:
          throw new Error('magic2: unsupported command LINE');
        case C_SPLINE:
          throw new Error('magic2: unsupported command SPLINE');
        case C_BOX:
          addr += 8;
          throw new Error('magic2: unsupported command SPLINE');
        case C_TRIANGLE:
          throw new Error('magic2: unsupported command TRIANGLE');
        case C_BOX_FULL: {
          const x1 = mem_read_u16be(memory, addr + 0);
          const y1 = mem_read_u16be(memory, addr + 2);
          const x2 = mem_read_u16be(memory, addr + 4);
          const y2 = mem_read_u16be(memory, addr + 6);
          addr += 8;
          this.boxFull(x1, y1, x2, y2);
          break; }
        case C_CIRCLE_FULL:
          throw new Error('magic2: unsupported command CIRCLE_FULL');
        case C_SET_WINDOW: {
          const x1 = mem_read_u16be(memory, addr + 0);
          const y1 = mem_read_u16be(memory, addr + 2);
          const x2 = mem_read_u16be(memory, addr + 4);
          const y2 = mem_read_u16be(memory, addr + 6);
          addr += 8;
          this.setWindow(x1, y1, x2, y2);
          break; }
        case C_SET_MODE: {
          const mode = mem_read_u16be(memory, addr);
          addr += 2;
          console.warn('magic2: ignoring command SET_MODE, ' + mode);
          break; }
        case C_POINT:
          throw new Error('magic2: unsupported command POINT');
        case C_CLS:
          this.cls();
          break;
        case C_SET_3D_PARAMETER: {
          const param_num = mem_read_u16be(memory, addr + 0);
          const param_val = mem_read_s16be(memory, addr + 2);
          addr += 4;
          this.set3dParameter(param_num, param_val);
          break; }
        case C_SET_3D_DATA:
          addr += this.set3dRawData(memory, addr);
          break;
        case C_TRANSLATE_3D_TO_2D:
          this.translate3dTo2d();
          break;
        case C_DISPLAY_2D:
          this.display2d();
          shown = true;
          break;
        case C_DONE:
          return shown;
        case C_COLOR: {
          const color = mem_read_u16be(memory, addr);
          addr += 2;
          this.color(color);
          break; }
        case C_CRT: {
          const crt = mem_read_u16be(memory, addr);
          addr += 2;
          this.crt(crt);
          break; }
        case C_INIT:
          // TODO: Initialize palette, etc.
          break;
        case C_AUTO:
          throw new Error('magic2: AUTO should not be used inside AUTO');
        case C_APAGE: {
          const apage = mem_read_u16be(memory, addr);
          addr += 2;
          this.apage(apage);
          break; }
        case C_DEPTH: {
          const minz = mem_read_u16be(memory, addr + 0);
          const maxz = mem_read_u16be(memory, addr + 2);
          addr += 4;
          this.depth(minz, maxz);
          break; }
        default:
          throw new Error('magic2: unknown command ' + cmd);
      }
    }
    // not reached.
  }

  apage (apage) {
    this[_].apage = apage;
  }

  depth (minz, maxz) {
    this[_].depth.minz = minz;
    this[_].depth.maxz = maxz;
  }
}

global.Magic2 = Magic2;

})(typeof global !== 'undefined' ? global : window);