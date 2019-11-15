import time
import serial

ser = serial.Serial(port='/dev/serial0', baudrate = 9600)

if __name__ == '__main__':
    try:
        _out = 0x00;
        while True:
            _out=ser.read()
#            ser.write(_out)
            print(_out)
#            if _out == 0x01:
#               _out = 0x00
#            else:
#                _out = 0x01
            time.sleep(1)
    except KeyboardInterrupt:
        exit()
