import requests


API_ENDPOINT = "http://127.0.0.1:3030/upload"

API_KEY = "XXXXXXXXXXXXXXXXX"

data = None

with open("MOCK_DATA2.csv","r") as f:
    data = f.read()

r = requests.post(url=API_ENDPOINT, data=data, headers={"Content-Type": "text/csv",'Authorization': API_KEY})

print(r.status_code)
print(r.content)


"""
#combine eventually
import serial
import datetime
import pytz

def nowtime():
    dtime = datetime.datetime.now()
    timezone = pytz.timezone("America/New_York")
    dtzone = timezone.localize(dtime)
    tstamp = dtzone.timestamp()
    epoch_time_secs = int(round(tstamp))
    return epoch_time_secs

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            tstamp = nowtime()
            line += ";"+str(tstamp)
            print(line)

"""