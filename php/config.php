<?php

$pebbleHeader = $_SERVER['HTTP_X_PEBBLE_ID'];
$date = date('Y-m-d H:i:s');

$errorlog = "errors.log";
$requestslog = "dailyrequests.log";

if(is_null($pebbleHeader)) {
	error_log($date . " | Non-pebble tried to request data.\n", 3, $errorlog);
}
else {
	if (strpos($pebbleHeader,':') === false) {
		$pebbleHeader = $pebbleHeader . " (iOS)"; }
	else {
		$pebbleHeader = $pebbleHeader . " (Android)"; }
	
	$request = $date . " | Request from Pebble with Serial #" . $pebbleHeader . "\n";
	}
	
	file_put_contents($requestslog, $request, FILE_APPEND | LOCK_EX);

$payload = json_decode(file_get_contents('php://input'), true);
//if(!$payload) die();

$returnArray = array(
	'username'=> $payload[1],
	'password'=> $payload[2]
);

return $returnArray;

?>