<?php

error_reporting(0);
date_default_timezone_set("Asia/Shanghai");

function handle_get() {
	$content = array(
		'year'  => 'year',
		'mon'   => 'mon',
		'day'   => 'day',
		'hour'  => 'hour',
		'min'   => 'min',
		'sec'   => 'sec',
	);

	echo json_encode($content);
	// {"name":"wifi","hdr_size":32,"max_size":393216}
}

if (isset($_POST['rtcget'])) {
	// echo '{"year":"year","mon":"mon","day":"day","hour":"port"}';
	// echo '{"ssid":["ssid0","ssid1","ssid2"],"psk":["psk0","psk1","psk2"],"host":"host","port":"port","apssid":"apssid","appsk":"appsk"}';
	handle_get();
}

if (isset($_POST['rtcset'])) {
	var_dump($_POST);
}
?>
