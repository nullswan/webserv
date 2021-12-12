<?php
	if (isset($_SERVER['HTTP_USER_AGENT']))
		echo 'Your UA is: ' . $_SERVER['HTTP_USER_AGENT'];
	else
		echo 'No UA';
?>