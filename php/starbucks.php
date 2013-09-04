<?php

require_once('starbucksLogin.php');

function balance() {
	$urlBalance ='https://www.starbucks.com/account/card/history';
	$cookie="cookie.txt"; 
	
	$ch = curl($urlBalance,false,false,$cookie);
	
	$needleBalance = '<p class="card_balance numbers">';
	$needleBalanceLen = strlen($needleBalance);
	
	$balancePos = strpos($ch, $needleBalance);
	$balancePos = $balancePos + $needleBalanceLen;
	
	$balanceSub = substr($ch, $balancePos, 50);
	preg_match('/<span>(.*)<\/span>/i', $balanceSub, $balance);
	
	$balanceHundred = intval(100 * str_replace("$", "", $balance[1]));
	
	$balanceDollars = intval($balanceHundred / 100);
	$balanceCents = $balanceHundred % 100;
	$balanceArray[0] = $balanceDollars;
	$balanceArray[1] = $balanceCents;
	return $balanceArray;
}

function stars() {
	$urlStars ='https://www.starbucks.com/account/rewards';
	$cookie="cookie.txt"; 
	
	$ch = curl($urlStars,false,false,$cookie);
	
	$needleStars = '<div class="rewards-number">';
	$needleStarsLen = strlen($needleStars);
	
	$starsPos = strpos($ch, $needleStars);
	//$starsPos = $starsPos + $needleStarsLen;
	
	$starsSub = substr($ch, $starsPos, 50);
	preg_match('/<div class="rewards-number">(.*)<\/div>/i', $starsSub, $stars);
	
	$starsInt = intval($stars[1]);
	return $starsInt;
	
	//return $stars[1];
}

function rewards() {
	$urlRewards ='https://www.starbucks.com/account/home';
	$cookie="cookie.txt"; 
	
	$ch = curl($urlRewards,false,false,$cookie);
	
	$needleRewards = '<span class="rewards_cup_gold">';
	$needleRewardsLen = strlen($needleRewards);
	
	$rewardsPos = strpos($ch, $needleRewards);
	//$starsPos = $starsPos + $needleStarsLen;
	
	$rewardsSub = substr($ch, $rewardsPos, 50);
	preg_match('/<span class="rewards_cup_gold">(.*)<\/span>/i', $rewardsSub, $rewards);
	
	$rewardsInt = intval($rewards[1]);
	return $rewardsInt;
}

function barcode() {
	// The file
	$filename = 'pebbleBucksORIG.jpg';
	$destImage = 'pebbleBucksResize.jpg';
	$maxwidth = 160;
	
	// Content type
	//header('Content-Type: image/jpeg');
	
	// Get new dimensions
	list($width, $height) = getimagesize($filename);
	
	if ($width < $height) {
		$percent = $maxwidth / $height;
		$rotate = true;
	}
	else {
		$percent = $maxwidth / $width;
		$degrees = false;
	}
	
	$new_width = $width * $percent;
	$new_height = $height * $percent;
	
	// Resample
	$image_p = imagecreatetruecolor($new_width, $new_height);
	$image = imagecreatefromjpeg($filename);
	imagecopyresampled($image_p, $image, 0, 0, 0, 0, $new_width, $new_height, $width, $height);
	
	
	
	if ($rotate)
	{
		// Rotate the image
		$rotated = imagerotate($image_p, 90, 0);
		imagejpeg($rotated, $destImage, 100);
	}
	else {
		imagejpeg($image_p, $destImage, 100);
	}
	
	//echo '<img src="' . $destImage . '">';
	return $destImage;
}

if ($getText == "y" || $getText == "yes")
{
	echo "Balance: " . balance();
	echo "<br> Stars: " . stars();
	echo "<br> Rewards: " . rewards();
}

else {
	if (!$loginStatus) {
		$arr = array('5' => 999);
	}
	
	else $arr = array('1' => balance()[0], '2' => balance()[1], '3' => stars(), '4' => rewards());
	
	header('Content-Type: application/json');
	echo json_encode($arr);
}

// Check if URL asks for barcode
if ($dispBarcode) {
	echo "<br><br>";
	
	echo '<img src="' . barcode() . '">';
}
file_put_contents($cookie, '');
	
?>