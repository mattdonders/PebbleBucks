function fetchCardInfo(username, password)
{
	var response;
	var req = new XMLHttpRequest();

	console.log("Fetching balance");
	Pebble.sendAppMessage({"balance": "$0.00", "stars": 0, "rewards": 0, "error": 0});

}

PebbleEventListener.addEventListener("ready",
						function(e) {
							console.log("connect! " + e.ready);

						});

PebbleEventListener.addEventListener("appmessage",
						function(e) {
							console.log(e.type);
							console.log("message!");
							fetchCardInfo(e.payload.username, e.payload.password);
						});

