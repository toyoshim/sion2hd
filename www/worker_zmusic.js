var running = false;
var verbose = true;
var sampleRate = 44100;
var sampleCount = 2048;
var bufferCount = 0;
var buffers = [];
var bufferId = 0;

self.addEventListener('message', function(e) {
  var data = e.data;
  if (running && verbose) {
    console.error('aborted:', data);
    verbose = false;
    return;
  }
  running = true;
  if (data.type == 'init') {
    sampleRate = data.rate;
    sampleCount = data.count;
    bufferCount = data.bufs;
    for (var i = 0; i < bufferCount; ++i) {
      buffers[i] = [
          new Float32Array(sampleCount),
          new Float32Array(sampleCount)
      ];
    }
    Module._zmusic_init(sampleRate, sampleCount);
  } else if (data.type == 'update') {
    var work = Module._zmusic_update();
    var index = work >> 1;
    var s16 = Module.HEAP16;
    var buffer = buffers[bufferId];
    bufferId = (bufferId + 1) % bufferCount;
    for (var i = 0; i < sampleCount; ++i) {
      buffer[1][i] = s16[index + sampleCount] / 32768;
      buffer[0][i] = s16[index++] / 32768;
    }
    self.postMessage({ type: 'update', l: buffer[0], r: buffer[1] });
  } else if (data.type == 'zmusic') {
    var filename = data.zmd ? Module.allocate(
        Module.intArrayFromString(data.zmd), 'i8', Module.ALLOC_NORMAL) : null;
    Module._zmusic_trap(data.d1, data.d2, data.d3, data.d4, data.a1, filename);
  }
  running = false;
});

var Module = {
  arguments: [
    'ZMUSIC.X',
    '-n',
    '-u',
    '-t0',
    '-w0',
    '-bSION2.ZPD',
    '-sNEIRO.ZMS'
  ],
  preInit: function() {
    var files = [
        'ZMUSIC.X', 'SION2.ZPD', 'NEIRO.ZMS', 'LEGEND_FM.ZMD', 'WAY1_FM.ZMD',
        'ASTERO_FM.ZMD', 'AMOUNT_FM.ZMD', 'SMART2_FM.ZMD', 'SIGH_FM.ZMD',
        'OLD_FM.ZMD', 'BOSS_FM.ZMD'
    ];
    for (var file of files) {
      var url = "sion2/" + file;
      FS.createLazyFile('/', file, url, true, false);
    }
  }
};

importScripts('zmusic.js');