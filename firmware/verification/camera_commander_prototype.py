# README: prototyp of FarmBot Camera Commander
# Features:
# Author:
#    Wuulong, Created 20/02/2017
# Arch:
#cli
import cmd

VERSION = "0.0.1"

#FarmBot Camera Commander CLI
class CameraCommanderCli(cmd.Cmd):
    """FarmBot Camera Commander CLI"""

    def __init__(self):
        cmd.Cmd.__init__(self)
        self.prompt = 'FCC>'

############ cli maintain ####################
    def do_quit(self, line):
        """quit"""
        return True

############ commands  ####################
    def do_r83(self,line):
        """Report software version"""
        print("V" + VERSION)

def main():
    print("----- FarmBot Camera Commander V" + str(VERSION) + " -----")
    CameraCommanderCli().cmdloop()
if __name__ == "__main__":
    main()

