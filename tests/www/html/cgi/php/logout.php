<?php
	session_start();
?>
<html>
	<head></head>
	<body>
	<?php
		if (isset($_SESSION['user'])) {
			session_destroy();
			echo 'Logged out! <br />';
		} else {
			echo 'No session to destroy.<br />';
		}
	?>
	<hr />
	<a href="login_form.html">Goto Login Form</a>
	<hr />
	<a href=".">Goto Index</a>
	</body>
</html>