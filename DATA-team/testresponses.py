"""
mock data generated here
api testing functions go here
"""
import random
import time
import requests

def randph():
    return random.randint(10,140)/10

def randSalinity():
    return random.randint(0,100)/10

def randTemp():
    return random.randint(3000,10000)/100

def write_test_data():
    with open("testdata.csv","w") as f:
        now =time.time()
        data = ""
        for i in range(50):
            line = f"{now+i},{randTemp()},{randph()},{randSalinity()}\n"
            data += line
        f.write(data)
#write_test_data()

def test_get():
    r = requests.get('http://127.0.0.1:8000')
    print(r)
    print(r.content)

def test_post():
    data = None
    with open("testdata.csv","r") as f:
        data = f.read()
    r = requests.post('http://127.0.0.1:8000/water/', json ={'data':data,'token':'gibberish'})
    print(r)

test_post()