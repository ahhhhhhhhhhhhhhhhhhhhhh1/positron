import sys
import os
import subprocess

if len(sys.argv) < 2:
    raise ValueError("not enough args given, please pass in the name of the html file to package into an exe.")

if not os.path.exists(sys.argv[1]):
    raise FileNotFoundError(f"file not found '{sys.argv[1]}'")

file = os.path.abspath(sys.argv[1])
name = os.path.basename(file)
with open(file, 'r') as f:
    html_content = f.read()

with open(r"exe template\template_1", "r") as f1, \
     open(r"exe template\template_2", "r") as f2, \
     open(r"exe template\template_3", "r") as f3:
        content = f1.read() + name + f2.read() + html_content.replace('\n', "\\\n") + f3.read()

with open(name + ".c", 'w') as f:
    f.write(content)

subprocess.run(f"gcc {name + ".c"} -o {name}.exe -Wl,-subsystem,windows", shell=True)