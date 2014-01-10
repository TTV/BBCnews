Pebble.addEventListener("appmessage",
  function(e) {
    Pebble.sendAppMessage({"0": "Hello World"});
  }
);