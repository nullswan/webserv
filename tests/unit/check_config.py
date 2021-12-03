import os
import sys
import subprocess

def	get_cwd():
	return subprocess.check_output(['/usr/bin/git', 'rev-parse', '--show-toplevel']).decode('ascii').strip()

DIR = get_cwd() + "/tests/configs/errors/"
TEST_FAILED = 0

def main() -> None:
	for file in os.listdir(DIR):
		print("Testing: " + file, end="\n\t")
		process = subprocess.Popen([
			get_cwd() + '/webserv', DIR + file
		])
		if (process.wait() != 1):
			print("Error: " + file + " should have failed")
			TEST_FAILED += 1

if __name__ == '__main__':
	main()
	
	if (TEST_FAILED > 0):
		print("\033[1;41;37m{} Tests failed\033[0m".format(TEST_FAILED))
		sys.exit(1)
	print("\033[1;42;37mAll tests passed\033[0m")