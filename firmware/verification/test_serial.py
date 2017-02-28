# verification code to verify serial connection
# program send serial F83 command every second to get FarmBot Arduion software version and print serial output out
# Ctrl-C can terminate program, pySerial needed.
# sample output, ver_cmd_mode = False:
#R83 GENESIS V.01.04
#
#R02
#
#R01
import threading
from serial import *
import time
import sys

CMDSTATE_R00 = 0 #idle
CMDSTATE_R01 = 1 #running
CMDSTATE_R02 = 2 #end success
CMDSTATE_R03 = 3 #end with error
CMDSTATE_R04 = 4 #running

RUNMODE_CHECKIF = 1
RUNMODE_VERIFY_CMDS=2
RUNMODE_CMDSCRIPT=3

class CmdState:
    def __init__(self):
        self.cmd_curid = ""
        self.cmd_curpar = ""
        self.cmd_str = ""
        self.cmd_state = CMDSTATE_R00
    def is_ready(self):
        if self.cmd_state == CMDSTATE_R00 or self.cmd_state == CMDSTATE_R02 or self.cmd_state == CMDSTATE_R03:
            return True
        else:
            return False
    def set_by_send(self, cmd_str):
        if self.is_ready():
            self.cmd_state = CMDSTATE_R00
            self.cmd_str =  cmd_str
        
    def set_by_recv(self, cmd_str):
        cmd_str1 = cmd_str.strip()
        if cmd_str1 == "R00":
            self.cmd_state = CMDSTATE_R00
        if cmd_str1 == "R01":
            self.cmd_state = CMDSTATE_R01
        if cmd_str1 == "R02":
            self.cmd_state = CMDSTATE_R02
        if cmd_str1 == "R03":
            self.cmd_state = CMDSTATE_R03
        if cmd_str1 == "R04":
            self.cmd_state = CMDSTATE_R04
        self.cmd_str = cmd_str
        #print("state by recv:%i" %(self.cmd_state))

#Serial process thread
class MonitorThread(threading.Thread):
    def __init__(self, wait=0.01):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        self.wait = wait
        self.exit = False
        self.ser = Serial('/dev/cu.usbmodem1421', 115200, timeout=1) #FIXME, change device id to your system device
        self.cmd_state = CmdState()

    def set_ts(self, ts):
        self.wait = ts

    def do_function(self):
        #print("thread running...")
        line = self.ser.readline()
        if len(line)>0:
            #print(line)
            sys.stdout.write(line)
            self.cmd_state.set_by_recv(line)
    
    def run(self):
        while 1:
            if self.exit:
                break
                # Wait for a connection
            self.do_function()
            self.event.wait(self.wait)

    def serial_send(self,send_str):
        sys.stdout.write("[%s]\n" % send_str)
        self.ser.write(send_str + "\n")
        self.cmd_state.set_by_send(send_str)

def main():
    
    th = MonitorThread()
    th.start()

    cmd_delay_second =1
    wait_ready_second =3 
    run_mode = RUNMODE_VERIFY_CMDS # RUNMODE_CHECKIF, RUNMODE_VERIFY_CMDS, RUNMODE_CMDSCRIPT, 
    
    file_name = "serial_commands_list.txt"
    if run_mode == RUNMODE_CMDSCRIPT:
        file_name = "serial_script.txt"
    
        
    while 1:
        try:
            if run_mode == RUNMODE_CHECKIF:
                th.serial_send("F83")
                time.sleep(1)
                
            else : # verify current commands.
                cmd_file = open(file_name, "r")
                lines = cmd_file.readlines()
                for line in lines:
                    
                    cols = line.split("#")
                    #print("line=%s,cols_count=%i" %(line,len(cols)))
                    if len(cols)>=1:
                        cmd = cols[0]
                        cmd = cmd.strip()
                        if len(cmd)>0:
                            #print(cmd)
                            while 1:
                                if th.cmd_state.is_ready(): #wait system ready to accept commands
                                    th.serial_send("%s" %cmd)
                                    break
                                else:
                                    time.sleep(wait_ready_second)
                            #ser.write("F83\n")
                    time.sleep(cmd_delay_second)
                cmd_file.close()
                
                th.exit = True
                break

        except:
            th.exit = True
            break

if __name__ == "__main__":
    main()


