import time
import serial
from digi.xbee.devices import XBeeDevice
#note: pi to atmega tx to rx, rx to tx
# pi to xbee tx to tx, rx to rx

#ser = serial.Serial(port='/dev/serial0', baudrate = 9600)

device = XBeeDevice("/dev/serial0", 9600)
device.open()

if __name__ == '__main__':
    try:
        _out = '0'
        while True:
            device.send_data_broadcast(_out.encode())
            if _out == '9':
                _out = '0'
            else:
                _out = chr(ord(_out) + 1)
            time.sleep(1)
    except KeyboardInterrupt:
        print('')
        device.close()
        exit()
