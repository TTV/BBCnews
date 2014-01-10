Pebble.addEventListener("ready", function() {
	Pebble.sendAppMessage({"0": "ready"});
});

Pebble.addEventListener("appmessage", function(e) {
	var method = e.payload["0"];
	if (method == "story"){
  		Pebble.sendAppMessage({"0": method, "1": "Hello World 2"});
	} else {
		console.log("Unknown appmessage method [" + method + "]");
	}
});