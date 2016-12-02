var Module = {
  arguments: [
    "ZMUSIC.X",
    "-n",
    "-u",
    "-t0",
    "-w0",
    "-BSION2.ZPD",
    "-sNEIRO.ZMS"
  ],
  preInit: function() {
    FS.createLazyFile('/', "ZMUSIC.X", "sion2/ZMUSIC.X", true, false);
    FS.createLazyFile('/', "SION2.ZPD", "sion2/SION2.ZPD", true, false);
    FS.createLazyFile('/', "NEIRO.ZMS", "sion2/NEIRO.ZMS", true, false);
  }
};
importScripts('zmusic.js');