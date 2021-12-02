<?php
	session_start();

	if (isset($_GET['user']) && isset($_GET['pwd'])) {
		$_SESSION['user'] = $_GET['user'];
		$_SESSION['pwd'] = $_GET['pwd'];

		echo 'Logged-in as ' . $_SESSION['user'];
	} else {
		echo 'Please enter your login and password.';
	}
?>