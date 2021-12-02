<?php
	session_start();

	if (isset($_SESSION['user'])) {
		echo 'Logged as ' . $_SESSION['user'];
	} else {
		echo 'Not logged';
	}
?>