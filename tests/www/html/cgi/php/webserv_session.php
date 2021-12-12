<?php

setcookie("WEBSERV_SESSION_TEST", "1");
setcookie("WEBSERV_SESSION_TEST_2", "2");
setcookie("WEBSERV_SESSION_TEST_3", "3");

if (isset($_COOKIE["WEBSERV_SESSION_ID"])) {
	echo "Session id: " . $_COOKIE["WEBSERV_SESSION_ID"] . "<br>";
} else {
	echo "No session id<br>";
}

if (isset($_GET['WEBSERV_USER'])) {
	setcookie("WEBSERV_USER", $_GET['WEBSERV_USER']);
}
if (isset($_COOKIE['WEBSERV_USER'])) {
	echo 'WEBSERV_USER = ' . $_COOKIE['WEBSERV_USER'];
}

?>