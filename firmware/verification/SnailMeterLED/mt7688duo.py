# Get Snail Meter information from internet, setup LED strip color
#    by sending serial commands to arduino
import serial
import time
s = None
def setup():
    global s
    # open serial COM port to /dev/ttyS0, which maps to UART0(D0/D1) # the baudrate is set to 57600 and should be the same as the one # specified in the Arduino sketch uploaded to ATmega32U4.
    s = serial.Serial("/dev/ttyS0", 57600)

def loop():
    # send "1" to the Arduino sketch on ATmega32U4.
    # the sketch will turn on the LED attached to D13 on the board 
    s.write("R")
    time.sleep(1)
    # send "0" to the sketch to turn off the LED
    s.write("G")
    time.sleep(1)

    s.write("B")
    time.sleep(1)

if __name__ == '__main__':
    setup()
    while True:
        loop()