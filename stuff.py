import os
import requests

if not os.path.exists(r"C:\Program Files\positron"):
    import requests

    url = ""
    response = requests.get(url)
    
    print(response.text)