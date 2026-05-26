import subprocess
import sys
import os

path = sys.argv[1]
if not os.path.exists(path):
    raise FileNotFoundError("'" + sys.argv[1] + "'")
path = os.path.abspath(path)

subprocess.run(f"chrome\\chrome.exe --app=\"file:///{path}\"", shell=True)