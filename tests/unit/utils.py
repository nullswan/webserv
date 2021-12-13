import os
import time
import string
import random
import subprocess

def get_git_root() -> str:
    return subprocess.Popen(
		['git', 'rev-parse', '--show-toplevel'], stdout=subprocess.PIPE) \
			.communicate()[0] \
			.rstrip() \
			.decode('utf-8')

def get_html_file(filename) -> str:
	filepath = 'tests/www/html/' + filename
	with open(filepath, 'r') as f:
		return f.read()
	return ""

def get_random_string(length) -> str:
	letters = string.ascii_lowercase + string.ascii_uppercase + string.digits
	result_str = ''.join(random.choice(letters) for i in range(length))
	return result_str

def start_server(config: str):
	f = open(get_git_root() + '/tests/logs/stdout', 'a')
	os.chdir(get_git_root())
	pid = subprocess.Popen(["./webserv", config], stdout=f)
	time.sleep(.5)
	return pid, f

def stop_server(pid, stdout):
	pid.terminate()
	stdout.close()