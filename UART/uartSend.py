import time
import serial

#note: pi to atmega tx to rx, rx to tx
# pi to xbee tx to tx, rx to rx
ser = serial.Serial(port='/dev/serial0', baudrate = 9600)


if __name__ == '__main__':
    try:
        _out = 0
        while True:
            ser.write(chr(_out).encode())
            print(_out)
            if _out == 255:
               _out = 0
            else:
                _out = 255
            time.sleep(1)
    except KeyboardInterrupt:
        print('')
        exit()
