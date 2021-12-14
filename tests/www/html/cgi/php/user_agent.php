<html>
	<head></head>
	<body>
	<?php
		if (isset($_SERVER['HTTP_USER_AGENT']))
			echo 'Your UA is: ' . $_SERVER['HTTP_USER_AGENT'];
		else
			echo 'No UA';
	?>
	<hr />
	<a href=".">Goto Index</a>
	</body>
</html>