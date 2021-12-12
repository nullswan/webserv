<?php
	session_start();

	if (isset($_SESSION['user'])) {
		session_destroy();
		echo 'Logged out!';
	} else {
		echo 'No php_session to destroy.';
	}

?>