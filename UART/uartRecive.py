import time
import serial

#note: pi to atmega tx to rx, rx to tx
# pi to xbee tx to tx, rx to rx
ser = serial.Serial(port='/dev/serial0', baudrate = 9600)


if __name__ == '__main__':
    try:
        while True:
            _out=ser.read()
            print(_out)
            time.sleep(1)
    except KeyboardInterrupt:
        print('')
        exit()
