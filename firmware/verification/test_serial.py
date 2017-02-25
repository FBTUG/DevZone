# verification code to verify serial connection
# program send serial F83 command every second to get FarmBot Arduion software version and print serial output out
# Ctrl-C can terminate program, pySerial needed.
# sample output:
#R83 GENESIS V.01.04
#
#R02
#
#R01
import threading
from serial import *
import time

        
#Serial process thread
class MonitorThread(threading.Thread):
    def __init__(self, ser, wait=0.01):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        self.wait = wait
        self.exit = False
        self.ser = ser

    def set_ts(self, ts):
        self.wait = ts

    def do_function(self):
        #print("thread running...")
        line = self.ser.readline()
        print(line)

    def run(self):
        while 1:
            if self.exit:
                break
                # Wait for a connection
            self.do_function()
            self.event.wait(self.wait)

def main():
    ser = Serial('/dev/cu.usbmodem1421', 115200, timeout=1) #FIXME, change device id to your system device
    th = MonitorThread(ser)
    th.start()
    while 1:
        try:
            ser.write("F83\n")
            time.sleep(1)
        except:
            th.exit = True
            break
        
if __name__ == "__main__":
    main()            


