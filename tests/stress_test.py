import os
import sys
import json
import signal
import subprocess

def run_test():
	# Create siege conf by starting a siege client once.
	siege_default = subprocess.Popen(["siege"])
	siege_default.wait()

	siege = subprocess.Popen(
		[
			"siege", "http://127.0.0.1:8080/", 
			"--concurrent", "255", "--reps", "1000", "--benchmark", "--quiet", "--json-output"
		],
		stdout=subprocess.PIPE)
	siege.wait()

	data, err = siege.communicate()
	if siege.returncode != 0:
		print("Siege failed")
		return False

	try:
		json_data = json.loads(data.decode("utf-8"))
		print(json_data)
		if json_data["availability"] < 100.00:
			print("availability too low: " + str(json_data["availability"]))
			return False
		print("availability ok !")

		return True
	except:
		print("Error while parsing siege output")
		print(data.decode("utf-8"))
		return False

def main():
	webserv = subprocess.Popen(["./webserv"])

	try:
		status = run_test()
	except KeyboardInterrupt:
		pass
	
	os.kill(webserv.pid, signal.SIGTERM)
	if status == False:
		sys.exit(1)

if __name__ == "__main__":
	try:
		main()

	except KeyboardInterrupt:
		print("\nExiting...")