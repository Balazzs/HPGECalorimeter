#!/usr/bin/env python
# coding: utf-8
from sys import argv
from os  import path
from git import Repo
from datetime import datetime
from time import sleep
import json
import subprocess

def update_settings_file_with_sha (run_directory, repo):
    settings_path = path.join(run_directory, "settings.json")

    with open(settings_path, "r") as settings_file:
        settings = json.load (settings_file)

    settings["version"] = repo.head.commit.hexsha

    with open(settings_path, "w") as settings_file:
        json.dump(settings, settings_file)

def log_process_output_until_it_ends (process):
    while True:
        output = process.stdout.readline()
        print(output.strip())
        # Do something else
        return_code = process.poll()

        if return_code is not None:
            # Process has finished, read rest of the output 
            for output in process.stdout.readlines():
                print(output.strip())
            
            print('RETURN CODE', return_code)

            break
        sleep(0.01)

def move_run_data_to_runs_folder (repo):
    timestamp = datetime.now().strftime("%Y_%m_%d-%H_%M_%S")
    run_directory = "runs/%s/" % timestamp

    os.system("mv run/* %s" % run_directory)
    os.system("cp settings.json %s" % run_directory)

    update_settings_file_with_sha (run_directory, repo)





allow_dirty_repo = False

for arg in argv[1:]:
    if arg == "-allow-uncommited":
        allow_dirty_repo = True

repo = Repo(path.dirname (__file__))

if repo.is_dirty() and not allow_dirty_repo:
    print ("You have uncommited changes in the git repo. Please commit before running a simulation. (For reproducibility)")
    exit ()

process = subprocess.Popen(['bash', 'run.sh'], stdout=subprocess.PIPE)

log_process_output_until_it_ends (process)

move_run_data_to_runs_folder (repo)