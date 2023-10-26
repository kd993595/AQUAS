import serial


#Just connect Arduino USB Plug to Raspberry PI with USB cable and check the connection between Arduino and Raspberry pi by type "ls /dev/tty*" in Raspberry Pi terminal, the result should be content "/dev/ttyACM0" and you are good to go.

def readserial(comport, baudrate):

    ser = serial.Serial(comport, baudrate, timeout=1.0)         # 1/timeout is the frequency at which the port is read
    ser.reset_input_buffer()
    while True:
        data = ser.readline().decode().strip()
        if data:
            print(data)


if __name__ == '__main__':

    readserial('COM28', 115200)