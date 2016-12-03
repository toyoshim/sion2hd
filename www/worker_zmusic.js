var running = false;
self.addEventListener('message', function(e) {
  var data = e.data;
  if (!running && data.type == 'zmusic') {
    running = true;
    var filename = data.zmd ? Module.allocate(
        Module.intArrayFromString(data.zmd), 'i8', Module.ALLOC_NORMAL) : null;
    Module._zmusic_trap(data.d1, data.d2, data.d3, data.d4, data.a1, filename);
    running = false;
  } else {
    console.log(data);
  }
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