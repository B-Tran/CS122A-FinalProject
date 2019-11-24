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
        _out = 0
        while True:
            xbee_message = device.read_data()
            data = xbee_message
            if data != None:
               msg = xbee_message.data
               print(msg.decode())
            else:
                print("nothing received")
            time.sleep(1)
    except KeyboardInterrupt:
        print('')
        device.close()
        exit()
