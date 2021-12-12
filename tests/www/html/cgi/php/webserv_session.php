<html>
<head></head>
<body>

<h1>Session ID</h1>
<?php
if (isset($_COOKIE["WEBSERV_SESSION_ID"])) {
	echo "Session id: " . $_COOKIE["WEBSERV_SESSION_ID"] . "<br>";
} else {
	echo "No session id<br>";
}
?>

<hr />
<h1>Session USER</h1>
<?php
if (isset($_GET['WEBSERV_USER'])) {
	setcookie("WEBSERV_USER", $_GET['WEBSERV_USER']);
	echo 'WEBSERV_USER = ' . $_GET['WEBSERV_USER'];
} else if (isset($_COOKIE['WEBSERV_USER'])) {
	echo 'WEBSERV_USER = ' . $_COOKIE['WEBSERV_USER'];
}
?>

<hr />
<h1>Webserv User</h1>
<form action="webserv_session.php" method="get">
	<input type="text" name="WEBSERV_USER">
	<input type="submit">

</body>
</html>