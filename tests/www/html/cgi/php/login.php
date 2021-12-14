<?php
	session_start();
?>
<html>
	<head></head>
	<body>
		<?php
			if (isset($_GET['user']) && isset($_GET['pwd'])) {
				$_SESSION['user'] = $_GET['user'];
				$_SESSION['pwd'] = $_GET['pwd'];

				echo 'Logged-in as ' . $_SESSION['user'] . ' using GET <br />';
			} else if (isset($_POST['user']) && isset($_POST['pwd'])) {
				$_SESSION['user'] = $_POST['user'];
				$_SESSION['pwd'] = $_POST['pwd'];

				echo 'Logged-in as ' . $_SESSION['user'] . ' using POST <br />';
			} else {
				echo 'Please enter your login and password. <br />';
			}
		?>
	<hr />
	<a href="login_form.html">Goto Login Form</a>
	<hr />
	<a href=".">Goto	 Index</a>
	</body>
</html>