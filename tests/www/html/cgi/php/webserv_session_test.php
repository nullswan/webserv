<?php

setcookie("WEBSERV_SESSION_TEST", "1");
setcookie("WEBSERV_SESSION_TEST_2", "2");
setcookie("WEBSERV_SESSION_TEST_3", "3");

if (isset($_COOKIE["WEBSERV_SESSION_ID"])) {
	echo "Session id: " . $_COOKIE["WEBSERV_SESSION_ID"] . "<br>";
} else {
	echo "No session id<br>";
}

?>