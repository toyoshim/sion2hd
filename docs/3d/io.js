// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(function() {
  // IO access patterns for each fonts.
  var fonts = {
    '000000000000000000000000': 32,
    '010001000100000001000000': 33,
    '101010100000000000000000': 34,
    '101011101010111010100000': 35,
    '011011001110011011000000': 36,
    '101000100100100010100000': 37,
    '010001000110101001100000': 38,
    '010001000000000000000000': 39,
    '001001000100010000100000': 40,
    '010000100010001001000000': 41,
    '010011100100111001000000': 42,
    '000001001110010000000000': 43,
    '000000000000010001000000': 44,
    '000000001110000000000000': 45,
    '000000000000000001000000': 46,
    '000000100100100000000000': 47,
    '111010101010101011100000': 48,
    '010011000100010011100000': 49,
    '111010100110100011100000': 50,
    '111000100110001011100000': 51,
    '101010101110001000100000': 52,
    '111010001110001011100000': 53,
    '111010001110101011100000': 54,
    '111000100100010001000000': 55,
    '111010101110101011100000': 56,
    '111010101110001011100000': 57,
    '000001000000010000000000': 58,
    '000001000000010001000000': 59,
    '001001001000010000100000': 60,
    '000011100000111000000000': 61,
    '100001000010010010000000': 62,
    '010010100010010001000000': 63,
    '010010101110110001100000': 64,
    '111010101110101010100000': 65,
    '111010101100101011100000': 66,
    '111010101000101011100000': 67,
    '110010101010101011000000': 68,
    '111010001100100011100000': 69,
    '111010001100100010000000': 70,
    '111010001010101011000000': 71,
    '101010101110101010100000': 72,
    '111001000100010011100000': 73,
    '011000100010101011100000': 74,
    '101011001100101010100000': 75,
    '100010001000100011100000': 76,
    '101011101010101010100000': 77,
    '110010101010101010100000': 78,
    '011010101010101011000000': 79,
    '111010101110100010000000': 80,
    '011010101010110011100000': 81,
    '111010101110110010100000': 82,
    '011010001110001011000000': 83,
    '111001000100010001000000': 84,
    '101010101010101011100000': 85,
    '101010101010101001000000': 86,
    '101010101010111010100000': 87,
    '101010100100101010100000': 88,
    '101010101110010001000000': 89,
    '111000100100100011100000': 90,
    '011001000100010001100000': 91,
    '101001001110010001000000': 92,
    '110001000100010011000000': 93,
    '010010100000000000000000': 94,
    '000000000000000011100000': 95,
    '100001000000000000000000': 96,
    '000001001010101001100000': 97,
    '100011001010101011000000': 98,
    '000001001010100001100000': 99,
    '001001101010101011100000': 100,
    '000011101010110011100000': 101,
    '011001001110010001000000': 102,
    '000001101010111000101100': 103,
    '100010001110101010100000': 104,
    '010000000100010001000000': 105,
    '001000000010001010101110': 106,
    '100010001010110010100000': 107,
    '110001000100010001000000': 108,
    '000010101110101010100000': 109,
    '000011001010101010100000': 110,
    '000001101010101011000000': 111,
    '000001101010101011101000': 112,
    '000011001010101011100010': 113,
    '000010101100100010000000': 114,
    '000001101000011011100000': 115,
    '010011100100010001100000': 116,
    '000010101010101001100000': 117,
    '000010101010101001000000': 118,
    '000010101010111010100000': 119,
    '000010100100010010100000': 120,
    '000010100100010001000000': 121,
    '000011100100100011100000': 122,
    '011001001100010001100000': 123,
    '010001000100010001000000': 124,
    '110001000110010011000000': 125,
    '111000000000000000000000': 126,
    '111111111111111111111111': 127
  };

  var started = false;
  var base = 0;
  var baseX = 0;
  var baseY = 0;
  var x = 0;
  var y = 0;
  var pal = 0;
  var data = new Array(4 * 6);
  var vr = magic2.vr();
  var context = magic2.context(vr ? 1 : 0);
  
  var c = document.getElementById('graphic2').getContext('2d');
  var scaleX = c.canvas.height / 256 * context.aspect;
  var scaleY = c.canvas.height / 256;
  var center = context.width;
  var offsetX = (context.width - c.canvas.height * context.aspect) / 2;
  var fontScale = 1.2;
  var fontWidth = (4 * scaleX * fontScale) | 0;
  var fontHalfWidth = (fontWidth / 2) | 0;
  var fontHeight = (6 * scaleY * fontScale) | 0;
  var fontHalfHeight = (fontHeight / 2) | 0;
  c.font = fontHeight + 'px \'Geo\'';
  c.textAlign = 'center';
  c.textBaseline = 'middle';

  window.io_set_mode = function (mode) {
    vr = mode;
    context = magic2.context(vr ? 1 : 0);
    scaleX = c.canvas.height / 256 * context.aspect;
    scaleY = c.canvas.height / 256;
    center = context.width;
    offsetX = (context.width - c.canvas.height * context.aspect) / 2;
    fontWidth = (4 * scaleX * fontScale) | 0;
    fontHalfWidth = (fontWidth / 2) | 0;
    fontHeight = (6 * scaleY * fontScale) | 0;
    fontHalfHeight = (fontHeight / 2) | 0;
    c.font = fontHeight + 'px \'Geo\'';
    c.textAlign = 'center';
    c.textBaseline = 'middle';
  };

  window.io_graphic_data = function(page, index, color) {
    if (!started) {
      started = true;
      base = index;
      baseX = index % 512;
      baseY = (index / 512) | 0;
      x = 0;
      y = 0;
      pal = 0;
    } else if (index != (base + y * 512 + x)) {
      console.error('io graphic monitor detects unexpected write sequence');
    }
    data[y * 4 + x] = color != 0 ? 1 : 0;
    if (color != 0)
      pal = color;
    x++;
    if (x == 4) {
      x = 0;
      y++;
      if (y == 6) {
        var code = fonts[data.join('')];
        var d = 5;
        if (vr) {
          var tx = baseX * scaleX + offsetX;
          var ty = baseY * scaleY;
          var tw = fontWidth;
          c.clearRect(tx + d, ty, tw, fontHeight);
          c.clearRect(center + tx - d, ty, tw, fontHeight);
          var ts = String.fromCharCode(code);
          tx += fontHalfWidth;
          ty += fontHalfHeight;
          c.fillStyle = magic2.palette(pal)[vr == 2 ? 'cl' : 'c'];  /* global magic2 */
          c.fillText(ts, tx + d, ty, tw);
          c.fillStyle = magic2.palette(pal)[vr == 2 ? 'cr' : 'c'];  /* global magic2 */
          c.fillText(ts, center + tx - d, ty, tw);
          started = false;
        } else {
          c.clearRect(
              baseX * scaleX + offsetX, baseY * scaleY, fontWidth, fontHeight);
          c.fillStyle = magic2.palette(pal)['c'];  /* global magic2 */
          c.fillText(
              String.fromCharCode(code),
              baseX * scaleX + fontHalfWidth + offsetX,
              baseY * scaleY + fontHalfHeight,
              fontWidth);
          started = false;
        }
      }
    }
  };
})();
