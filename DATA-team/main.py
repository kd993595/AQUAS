import requests
import csv
import time

API_ENDPOINT = "http://127.0.0.1:3030/upload"

API_KEY = "XXXXXXXXXXXXXXXXX"

with open("MOCK_DATA2.csv","r") as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        # print(f"{row[0]} --- {row[1]} --- {row[2]} --- {row[3]} raw:{','.join(row)}\n")
        rawdata = ','.join(row)
        line_count += 1
        try: 
            r = requests.post(url=API_ENDPOINT, data=rawdata, headers={"Content-Type": "text/csv",'Authorization': API_KEY},timeout=1)
            print(r.status_code)
            print(r.content)
        except requests.exceptions.HTTPError as errh:
            print ("Http Error:",errh)
        except requests.exceptions.ConnectionError as errc:
            print ("Error Connecting:",errc)
        except requests.exceptions.Timeout as errt:
            print ("Timeout Error:",errt)
        except requests.exceptions.RequestException as err:
            print ("OOps: Something Else",err)
        time.sleep(10)
        if line_count >= 100:
            break
    print(f'Processed {line_count} lines.')



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