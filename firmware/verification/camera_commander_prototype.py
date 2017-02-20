# README: prototyp of FarmBot Camera Commander
#    Current code only focus on prototype behavior, also can be used for basic verificaition
#    Very limited, only for concept proven.
# Features:
#    1. Support CLI
#    2. start/stop socket command interface
#    3. support socket input command and response
#    4. CLI and socket command can be support at the same time.
# Author:
#    Wuulong, Created 20/02/2017
# Arch:
import cmd
import threading
import time
import socket

VERSION = "0.0.4"
CMD_VERSION = "0.1"

ccli = None
socket_output = []

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
        global ccli
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
                        data = connection.recv(16)
                        print("received:%s" % data)
                        if data:
                            print('demo code: echo data back to client')
                            if ccli:
                                #ccli.do_r83("")
                                ccli.cmd_handler(data)
                                global socket_output
                                output = "\n"
                                if len(socket_output)>0:
                                    output = "\n".join(socket_output)
                                    socket_output = []
                                connection.sendall(output)
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
    def cmd_handler(self,line):
        try:
            cmds = line.split("\n")
            #print("cmd_handler: %s" %(line))
            for cmd1 in cmds:
                if cmd1.startswith('R83'):
                    self.do_r83(cmd1)
        except:
            print("cmd_handler have exception, current line = %s" % (line))
############ commands  ####################
    def do_r83(self,line):
        """Report software version"""
        global socket_output
        output = "V" + VERSION
        print(output)
        socket_output.append(output)
def main():
    global ccli
    print("----- FarmBot Camera Commander V" + str(VERSION) + " -----")
    ccli = CameraCommanderCli()
    ccli.cmdloop()
if __name__ == "__main__":
    main()

