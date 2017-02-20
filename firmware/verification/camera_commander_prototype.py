# README: prototyp of FarmBot Camera Commander
# Features:
# Author:
#    Wuulong, Created 20/02/2017
# Arch:
import cmd
import threading
import time
import socket

VERSION = "0.0.3"
CMD_VERSION = "0.1"

#Command socket thread
class MonitorThread(threading.Thread):
    def __init__(self, wait=0.01):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        self.wait = wait
        self.exit = False

    def set_ts(self, ts):
        self.wait = ts

    def do_function(self):
        print("thread running...")
        time.sleep(5)

    def run(self):
        bfirst = True
        # Create a TCP/IP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Bind the socket to the port
        server_address = ('localhost', 10000)
        print("starting up on %s port %s" % server_address)
        sock.bind(server_address)
        
        # Listen for incoming connections
        sock.settimeout(1.0)
        sock.listen(1)
        connection = None
        while 1:
            if self.exit:
                break
                # Wait for a connection
            if bfirst:
                print('waiting for a connection...')
                bfirst=False
            try:
                
                connection, client_address = sock.accept()
                if connection:            
                    print("connection from %s,%s" % client_address)
            
                    # Receive the data in small chunks and retransmit it
                    while True:
                        data = connection.recv(256)
                        print("received:%s" % data)
                        if data:
                            print('demo code: echo data back to client')
                            connection.sendall(data)
                        else:
                            print("no more data from %s,%s" % client_address)
                            break
            except socket.timeout:
                #timeout is normal
                pass
            except :
                #print("%s:%s" %( socket.error.errno, socket.error.message)) #FIXME
                pass
                            
            finally:
                # Clean up the connection
                if connection:
                    connection.close()
            #self.do_function()
            self.event.wait(self.wait)



#FarmBot Camera Commander CLI
class CameraCommanderCli(cmd.Cmd):
    """FarmBot Camera Commander CLI"""

    def __init__(self):
        cmd.Cmd.__init__(self)
        self.prompt = 'FCC>'
        self.monitor_thread = None

############ cli maintain ####################
    def do_quit(self, line):
        """quit"""
        if self.monitor_thread:
            self.monitor_thread.exit=True
            time.sleep(1)
        return True

    def do_net_open(self,line):
        """open socket to accept commands"""
        if self.monitor_thread==None:
            self.monitor_thread = MonitorThread()
            self.monitor_thread.start()
        else:
            print("Command socket already exist! Don't start again")

    def do_stop(self,line):
        """ stop socket
        stop
        ex: stop"""
        if self.monitor_thread:
            self.monitor_thread.exit=True
        time.sleep(2)
        self.monitor_thread = None
        
############ commands  ####################
    def do_r83(self,line):
        """Report software version"""
        print("V" + VERSION)

def main():
    print("----- FarmBot Camera Commander V" + str(VERSION) + " -----")
    CameraCommanderCli().cmdloop()
if __name__ == "__main__":
    main()

