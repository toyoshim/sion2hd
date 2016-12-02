// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(function() {
  var ws = new WebSocket("wss://wshub.herokuapp.com/");
  ws.addEventListener('message', function(e) {
    try {
      var json = JSON.parse(e.data);
      if (json.type == 'syslog') {
        console.log('wslog:', json.log);
        console.log(JSON.parse(json.log));
      }
    } catch (e) {}
  });

  window.wslog = function(message) {
    if (ws.readyState != ws.OPEN)
      return;
    var log = typeof message == "string" ? message : JSON.stringify(message);
    ws.send(JSON.stringify({
      type: 'syslog',
      log: log
    }));
  };
})();