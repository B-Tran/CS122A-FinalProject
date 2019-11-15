import RPi.GPIO as GPIO #import raspberry gpio library
from time import sleep #import the sleep function from the time module

GPIO.setmode(GPIO.BOARD) #use physical pin numbering
GPIO.setup(36, GPIO.OUT, initial=GPIO.LOW) # set pin 22 to be an output pin initialized to low


if __name__ == '__main__':
    try:
        while True:
            GPIO.output(36, GPIO.HIGH)
            sleep(1)
            GPIO.output(36, GPIO.LOW)
            sleep(1)
    except KeyboardInterrupt:
        GPIO.cleanup()
        exit()
