import os.path
import subprocess
from time import sleep


files = ["-r build", "-r source", "*.ipynb", "*.py", "*.txt"]
version_number_file = "version.txt"


if os.path.isfile(version_number_file):
	with open(version_number_file, "r") as version_file:
		version = int(version_file.readline()) + 1
else:
	version = 1

with open(version_number_file, "w") as file:
	file.write(str(version))


os.system("mkdir version_history/%i/" % version)
for file in files:
	os.system("cp %s version_history/%i/" % (file, version))



process = subprocess.Popen(['bash', 'run.sh'], stdout=subprocess.PIPE)

while True:
	output = process.stdout.readline()
	print(output.strip())
	# Do something else
	return_code = process.poll()
	if return_code is not None:
		print('RETURN CODE', return_code)
		# Process has finished, read rest of the output 
		for output in process.stdout.readlines():
			print(output.strip())
		break
	sleep(0.01)