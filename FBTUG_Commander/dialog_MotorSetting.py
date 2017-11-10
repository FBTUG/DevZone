import Tkinter
import tkMessageBox
#from Tkinter import *
import tkSimpleDialog

class MotorSetting(tkSimpleDialog.Dialog):
    # ########################################
    def __init__(self, master, arg_MaxSpeed, arg_Acceleration):
        print 'init'
        self.MaxSpeed= arg_MaxSpeed
        self.Acceleration= arg_Acceleration
    	tkSimpleDialog.Dialog.__init__(self, master, "Motor Setting")
    # ########################################


    def body(self, master):
        print 'body'
        Tkinter.Label(master, text="Max Speed (X):").grid(row=0)
        Tkinter.Label(master, text="Acceleration (X) :").grid(row=1)
        Tkinter.Label(master, text="Max Speed (Y):").grid(row=2)
        Tkinter.Label(master, text="Acceleration (Y) :").grid(row=3)
        Tkinter.Label(master, text="Max Speed (Z):").grid(row=4)
        Tkinter.Label(master, text="Acceleration (Z) :").grid(row=5)

        self.entry_Speed_X = Tkinter.Entry(master)
        self.entry_Acc_X = Tkinter.Entry(master)
        self.entry_Speed_Y = Tkinter.Entry(master)
        self.entry_Acc_Y = Tkinter.Entry(master)
        self.entry_Speed_Z = Tkinter.Entry(master)
        self.entry_Acc_Z = Tkinter.Entry(master)
        self.entry_Speed_X.insert(Tkinter.END,str(self.MaxSpeed[0]))
        self.entry_Acc_X.insert(Tkinter.END,str(self.Acceleration[0]))
        self.entry_Speed_Y.insert(Tkinter.END,self.MaxSpeed[1])
        self.entry_Acc_Y.insert(Tkinter.END,self.Acceleration[1])
        self.entry_Speed_Z.insert(Tkinter.END,self.MaxSpeed[2])
        self.entry_Acc_Z.insert(Tkinter.END,self.Acceleration[2])

        self.entry_Speed_X.grid(row=0, column=1)
        self.entry_Acc_X.grid(row=1, column=1)
        self.entry_Speed_Y.grid(row=2, column=1)
        self.entry_Acc_Y.grid(row=3, column=1)
        self.entry_Speed_Z.grid(row=4, column=1)
        self.entry_Acc_Z.grid(row=5, column=1)
        return self.entry_Speed_X # initial focus

    def apply(self):
        try:
            spd_X = int(self.entry_Speed_X.get())
            acc_X = int(self.entry_Acc_X.get())
            spd_Y = int(self.entry_Speed_Y.get())
            acc_Y = int(self.entry_Acc_Y.get())
            spd_Z = int(self.entry_Speed_Z.get())
            acc_Z = int(self.entry_Acc_Z.get())
             
            self.result= spd_X, acc_X, spd_Y, acc_Y, spd_Z, acc_Z
            print spd_X, spd_Y, spd_Z # or something
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
