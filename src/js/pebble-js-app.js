function fetchCardInfo(username, password)
{
	var response;
	var req = new XMLHttpRequest();

	console.log("Fetching balance");
	Pebble.sendAppMessage({"balance": "$0.00", "stars": 0, "rewards": 0, "error": 0});
	/*req.open("GET", "https://terminal.cluster1.openmobl.com/cgi-bin/starbucks/starbucks.php" + spotId, true);
	
	req.onload = function(e) {
		if (req.readyState == 4) {
			if(req.status == 200) {
				response = JSON.parse(req.responseText);
  
				if (response && response.length > 0) {
					var i = 0;
					var currHour = currTwelveHourTime();
					console.log("Getting report for " + currHour);
					
					while (response[i].hour != currHour) {
						i++;
					}
					
					var surfReport = {
							"spot": spotId,
							"shape_full": response[i].shape_full,
							"swell": response[i].shape_detail.swell,
							"tide": response[i].shape_detail.tide,
							"wind": response[i].shape_detail.wind,
							"size": response[i].size + "ft"
						};
					console.log("Sending surf report to watch: " + JSON.stringify(surfReport));
					Pebble.sendAppMessage(surfReport);
				}
			} else {
				console.log("Error");
			}
		}
	}

	req.send(null);*/
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

