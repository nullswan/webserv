import os
import sys
import json
import signal
import subprocess

def run_test(reps: int, siege_bin: str) -> bool:
	siege_process = subprocess.Popen(
		[
			siege_bin, "http://127.0.0.1:8000/ping/index.html", 
			"--concurrent", "255", "--reps", str(reps), "--benchmark", "--quiet", "--json-output"
		],
		stdout=subprocess.PIPE)
	siege_process.wait()

	data, err = siege_process.communicate()
	if siege_process.returncode != 0:
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

def setup() -> (int, str):
	reps = 1000
	if len(sys.argv) > 1:
		reps = int(sys.argv[1])
		if (reps <= 0 or reps >= 10000):
			reps = 100

	siege_bin = "siege"
	if len(sys.argv) > 2:
		siege_bin = sys.argv[2]

	# Create siege conf by starting a siege client once.
	_ = subprocess.Popen([siege_bin])
	_.wait()

	return reps, siege_bin

def main():
	webserv = subprocess.Popen(["./webserv"])

	try:
		reps, siege_bin = setup()
		status = run_test(reps, siege_bin)
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