import Tkinter
import tkMessageBox
#from Tkinter import *
import tkSimpleDialog

class CameraConnection(tkSimpleDialog.Dialog):
    # ########################################
    def __init__(self, master, arg_camID):
        print 'init'
        self.__cameraID= arg_camID
    	tkSimpleDialog.Dialog.__init__(self, master, "Camera Connection Setting")
    # ########################################

    def body(self, master):
        print 'body'
        Tkinter.Label(master, text="Caemra ID:").grid(row=0)

        self.entry_cameraID = Tkinter.Entry(master)
        self.entry_cameraID.insert(Tkinter.END,str(self.__cameraID))
        self.entry_cameraID.grid(row=0, column=1)
        
        return self.entry_cameraID # initial focus

    def apply(self):
        try:
            self.__cameraID = int(self.entry_cameraID.get())
             
            self.result= self.__cameraID
            print self.result # or something
        except ValueError:
            tkMessageBox.showwarning("Bad input","Illegal values, please try again")


'''
class MotorSetting(Tkinter.Toplevel):
    def __init__(self, parent, prompt):
        Tkinter.Toplevel.__init__(self, parent)
        self.title("Setting")
        self.var = Tkinter.StringVar()

        self.label = Tkinter.Label(self, text=prompt)
        self.entry = Tkinter.Entry(self, textvariable=self.var)
        self.ok_button = Tkinter.Button(self, text="OK", command=self.on_ok)

        self.label.pack(side="top", fill="x")
        self.entry.pack(side="top", fill="x")
        self.ok_button.pack(side="right")

        self.entry.bind("<Return>", self.on_ok)

    def on_ok(self, event=None):
        self.destroy()

    def show(self):
        self.wm_deiconify()
        self.entry.focus_force()
        self.wait_window()
        return self.var.get()
        #return self.entry.get()
'''
