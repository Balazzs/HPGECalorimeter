import os.path
import subprocess
from time import sleep


files_before = ["-r build", "-r source", "-r macros", "*.ipynb", "*.py", "*.txt"]
files_after  = ["-r run"]
version_number_file = "version.txt"


if os.path.isfile(version_number_file):
	with open(version_number_file, "r") as version_file:
		version = int(version_file.readline()) + 1
else:
	version = 1

with open(version_number_file, "w") as file:
	file.write(str(version))


os.system("mkdir version_history/%i/" % version)
for file in files_before:
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

for file in files_after:
	os.system("cp %s version_history/%i/run/" % (file, version))