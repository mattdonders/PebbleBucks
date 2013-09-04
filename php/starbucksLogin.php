<?php

$cookie = "cookie.txt";
//$cookie = tempnam ("/tmp", "cookieMyWeb");

if (is_writable($cookie)) {
    // echo 'The cookie is writable';
} else {
    // echo 'The cookie is not writable';
    error_log("The cookie is not writable!", 3, "errors.log");
    ## start by making it writable :
   if (! chmod ( $cookie, 0777 ))
        die ( 'chmod() failed on file cookie.txt' );
}

function curl($url, $binary=false, $post=false, $cookie=false ){

$ch = curl_init();

$headers[] = "Accept: */*";
$headers[] = "Connection: Keep-Alive";
$headers[] = "Content-type: application/x-www-form-urlencoded;charset=UTF-8";

		curl_setopt ($ch, CURLOPT_HTTPHEADER,  $headers);
        curl_setopt ($ch, CURLOPT_URL, $url);
        curl_setopt ($ch, CURLOPT_HEADER, false);
        curl_setopt ($ch, CURLOPT_REFERER, $url);
        curl_setopt ($ch, CURLOPT_ENCODING, 'gzip,deflate');
        curl_setopt ($ch, CURLOPT_AUTOREFERER, true);
        curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt ($ch, CURLOPT_TIMEOUT, 60);
        curl_setopt ($ch, CURLOPT_RETURNTRANSFER, true);



        if($cookie){

            $agent = "Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.4) Gecko/20030624 Netscape/7.1 (ax)";
            curl_setopt($ch, CURLOPT_USERAGENT, $agent);
            curl_setopt($ch, CURLOPT_COOKIEFILE, $cookie);
            curl_setopt($ch, CURLOPT_COOKIEJAR, $cookie);

        }


        if($binary)
            curl_setopt($ch, CURLOPT_BINARYTRANSFER, 1);

        if($post){
            curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
        }

	 $result = curl_exec($ch);
	 curl_close($ch);
	 
	 return $result;
}

$getUser = $_GET["u"];
$getPass = $_GET["p"];

if (empty($getUser) || empty($getPass))
{
	$config = include 'config.php';
	$username = $config['username'];
	$password = $config['password'];
}

else 
{
	//echo "Using passed User & P/W!";
	$username = $getUser;
	$password = $getPass;
}

$url="https://www.starbucks.com/account/signin"; 

$postdata = "Account.UserName=".$username."&Account.PassWord=".$password."&action=/account/signin";

// 1. Curl to get cookies
$c1h = curl($url, false, false, $cookie);

// function
$ch = curl($url, false, $postdata, $cookie);

// The below will request a URL inside your account to check for login success
$loginCheckURL = "https://www.starbucks.com/account/home";
$loginCheck = curl($loginCheckURL, false, false, $cookie);

preg_match('/<title>(.*)<\/title>/i', substr($loginCheck, 300, 400), $titles);

if (strpos($titles[1],'Account Sign In') !== false) {
	//echo "Login Failed! <br><br>";
	$loginStatus = FALSE;
}
else {
	 // echo "Login Successful! <br><br>";
	 $loginStatus = TRUE;
}

?>