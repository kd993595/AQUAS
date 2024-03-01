import requests


API_ENDPOINT = "http://127.0.0.1:3030/upload"

API_KEY = "XXXXXXXXXXXXXXXXX"

data = None

with open("MOCK_DATA2.csv","r") as f:
    data = f.read()

r = requests.post(url=API_ENDPOINT, data=data, headers={"Content-Type": "text/csv",'Authorization': API_KEY})

print(r.status_code)
print(r.content)