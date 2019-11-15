import RPi.GPIO as GPIO #import raspberry gpio library
from time import sleep #import the sleep function from the time module

GPIO.setmode(GPIO.BOARD) #use physical pin numbering
GPIO.setup(36, GPIO.IN) # set pin 26 to be an input pin


if __name__ == '__main__':
    try:
        print("system start")
        _j=2
        while True:
            _i = GPIO.input(36)
            if( _i == 0 and _j != 0):
                print("motion not detected")
            elif( _i == 1 and _j != 1):
                print("motion detected")
            _j=_i
            sleep(1)
    except KeyboardInterrupt:
        GPIO.cleanup()
        exit()
