var attempts;

Pebble.addEventListener("ready", function(e){
	console.log("ready");
});

Pebble.addEventListener("showConfiguration", function(e){
	var config_url = "http://tjstein.me/pebble/coinflip_configuration.html"; // Change this URL to the URL of your own configuration page.
	if (window.localStorage.getItem("coin") !== null) {
		var options = encodeURIComponent(window.localStorage.getItem("coin"));
		config_url = config_url + "?" + options;
	}
	Pebble.openURL(config_url);
});

Pebble.addEventListener("webviewclosed",function(e){
	if (e.response === "CANCELLED"){
		return;
	}
	var response = JSON.parse(e.response);
	var coin = response["coin"];
	window.localStorage.setItem("coin", JSON.stringify(response));
	var dict = {};
	dict["COIN"] = coin;
	attempts = 0;
	sendMsg(dict);
});

function sendMsg(dict){
	Pebble.sendAppMessage(dict,
		function(e){
			console.log("success");
		},
		function(e){
			console.log("ERROR: " + e.error.message);
			if (attempts < 5){
				console.log("Attempting to send again.");
				attempts++;
				sendStuff(dict, j);
			}
			else{
				console.log("Too many Nacks. Giving up.");
			}
		});
}