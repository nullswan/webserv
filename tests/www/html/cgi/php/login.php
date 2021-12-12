<?php
	session_start();

	if (isset($_GET['user']) && isset($_GET['pwd'])) {
		$_SESSION['user'] = $_GET['user'];
		$_SESSION['pwd'] = $_GET['pwd'];

		echo 'Logged-in as ' . $_SESSION['user'] . ' using GET';
	} else if (isset($_POST['user']) && isset($_POST['pwd'])) {
		$_SESSION['user'] = $_POST['user'];
		$_SESSION['pwd'] = $_POST['pwd'];

		echo 'Logged-in as ' . $_SESSION['user'] . ' using POST';
	} else {
		echo 'Please enter your login and password. <br />';
		echo 'Ex: login.php?user=xx&pwd=yy';
	}
?>