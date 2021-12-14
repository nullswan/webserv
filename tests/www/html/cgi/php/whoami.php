<?php
	session_start();
?>
<html>
	<head></head>
	<body>
	<?php
		if (isset($_SESSION['user'])) {
			echo 'Logged as ' . $_SESSION['user'];
		} else {
			echo 'Not logged';
		}
	?>
	<hr />
	<a href="login_form.html">Goto Login Form</a>
	<hr />
	<a href=".">Goto Index</a>
	</body>
</html>