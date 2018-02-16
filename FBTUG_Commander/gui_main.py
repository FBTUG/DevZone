import cv2
import numpy as np
import threading
import json
import random
import math
import time
import types
#import Tkinter
from Tkinter import *
import tkFileDialog
import ttk
import tkMessageBox
import tkFont
import ScrolledText
import Pmw
from PIL import Image
from PIL import ImageTk
from os import listdir, path, makedirs, remove
from datetime import datetime

from class_ArduinoSerMntr import*
from class_CameraMntr import*
import class_MyThread
import class_ImageProcessing
from class_PlantIdentifier import PlantIdentifier
import imgProcess_tool 
import gui_vars
from class_ConfigSetting import ConfigSetting
#from class_ConfigSetting_new import ConfigSetting
from dialog_PeripheralSetting import PeripheralSetting
from dialog_MotorSetting import MotorSetting
from dialog_CameraConnection import CameraConnection
import utils_tool
class App:
    # Ininitalization
    def __init__(self,root):
        strFont= 'Arial'
        myfont14 = tkFont.Font(family=strFont, size=14, weight= tkFont.BOLD)
        myfont12 = tkFont.Font(family=strFont, size=12)#, weight= tkFont.BOLD)
        myfont12_Bold = tkFont.Font(family=strFont, size=12, weight= tkFont.BOLD)
        myfont10 = tkFont.Font(family=strFont, size=10)
        myfont8 = tkFont.Font(family=strFont, size=8, weight= tkFont.BOLD)
        self.bgGreen= '#007700'
        self.bgGreen_active= '#00aa00'
        bgGray= '#333333333'
        bgGray_active= 'gray'
        bgGray_select= '#999'
        self.bgRed= '#aa0000'
        self.bgRed_active= '#ee0000'
        self.Move_intervalUnit= 1
        '''
        self.root = Tkinter.Tk()
        self.root.title("[Arduino] Stepper Control")
        self.root.attributes('-zoomed', True) # FullScreen
        '''
        self.root= root
        self.root.update()
        # =================================
        # Parameters
        # =================================
        if utils_tool.check_path(gui_vars.saveParaPath):
            print 'ICON...'
            self.img_icon = Tkinter.PhotoImage(file = gui_vars.saveParaPath+'Icon_2.png')
            #img_icon = Tkinter.PhotoImage(file = gui_vars.saveParaPath+'img_Seed.png')
            #print self.img_icon 
            self.root.tk.call('wm', 'iconphoto', self.root._w, self.img_icon)
        self.config= ConfigSetting(gui_vars.saveParaPath, gui_vars.configName, gui_vars.defaultDict)
        params= self.config.read_json()
        #print 'para: ',params
        self.threshold_graylevel= params['thrshd_gray']
        self.threshold_MinSize= params['thrshd_Minsize'] 
        self.threshold_MaxSize= params['thrshd_Maxsize'] 
        self.scan_X= params['Scan_X (Beg,Interval,Amount)']
        self.scan_Y= params['Scan_Y (Beg,Interval,Amount)']
        self.limit= params['limit Maximum (X,Y)']
        self.MaxSpeed= params['Max Speed (X, Y)']
        self.Acceleration= params['Ac/Deceleration (X, Y)']
        self.CameraID= params['Camera ID']
        self.Peripheral_para= params['Peripheral Setting']
        self.rdbtnMvAmount_Mode= params['Move Amount type (5 types)']
        self.scriptPath= params['script Path']
        self.pinNumb_fan= 8
        self.pinNumb_water= 9
        self.pinNumb_seed= 10
        for key, value in params['Peripheral Setting']:
            if key.strip().replace(' ','').lower() is 'waterpump':
                self.pinNumb_water= value
            if key.strip().replace(' ','').lower() is 'vaccumpump':
                self.pinNumb_seed= value
            if key.strip().replace(' ','').lower() is 'fan':
                self.pinNumb_fan= value

        self.imageProcessor= class_ImageProcessing.contour_detect(gui_vars.savePath,gui_vars.saveParaPath)
        self.checkmouse_panel_mergeframe= False
        self.x1, self.y1, self.x2, self.y2= -1,-1,-1,-1        
        self.StartScan_judge= False
        self.StartRunScript_judge= False
        self.saveScanning= 'XXX'
        self.strStatus= 'Idling...'
        self.readmergeframeIndex= ''

        self.root.update()
        self.screen_width, self.screen_height= self.root.winfo_width(), self.root.winfo_height()
        print 'screen: ',[self.root.winfo_screenwidth(), self.root.winfo_screenheight()]
        print 'w, h: ',[self.root.winfo_width(), self.root.winfo_height()]
        btn_width, btn_height= 8, 1
        #gui_vars.interval_x, gui_vars.interval_y= 6, 6
        self.mergeframe_spaceY= 50
        #print width,',', height,' ; ',btn_width,',', btn_height
        
        # =======================================
        # [Config] Menu Bar
        # =======================================
        self.menubar= Tkinter.Menu(self.root)
        self.FileMenu = Tkinter.Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="File",underline=0, menu=self.FileMenu)
        self.FileMenu.add_command(label="Load Image", command=self.btn_loadImg_click)
        self.FileMenu.add_command(label="Save Image", command=self.btn_saveImg_click)
        self.SettingMenu = Tkinter.Menu(self.menubar, tearoff=0)
        self.SettingMenu.add_command(label= "Peripheral Setting", command= self.set_Peripheral)
        self.SettingMenu.add_command(label= "Motor Setting", command= self.set_Motor)
        self.menubar.add_cascade(label="Setting", underline=0, menu=self.SettingMenu)
        self.ConnectMenu = Tkinter.Menu(self.menubar, tearoff=0)
        self.ConnectMenu.add_command(label="Connect to Arduino", command=self.set_ArdConnect)
        self.ConnectMenu.add_command(label="Connect to Camera", command=self.set_CamConnect)
        self.menubar.add_cascade(label="Communication", underline= 0, menu=self.ConnectMenu)
        self.ImgProcess= Tkinter.Menu(self.menubar, tearoff=0)
        self.ImgProcess.add_command(label="Set Background", command= self.plastic_set_background)
        self.ImgProcess.add_command(label='Otsu Binary', command= self.method_OtsuBinary)
        self.menubar.add_cascade(label="Image Processing", underline=0, menu= self.ImgProcess)
        self.root.config(menu= self.menubar)
        self.root.update()

        # =======================================
        # [Config] Status Bar
        # =======================================
        self.statuslabel = Tkinter.Label(self.root, bd = 1, relief = Tkinter.SUNKEN, anchor = "w")
        self.statuslabel.config(text="IDLING ..................")
        self.statuslabel.pack(side = Tkinter.BOTTOM,fill=Tkinter.X)
        self.root.update()
        # ==================================================
        # [ROOT] Current position of motor
        # ==================================================
        self.lbl_CurrPos= Tkinter.Label(self.root, text="Location: (X, Y, Z)= (-1, -1, -1)",font= myfont14)
        self.lbl_CurrPos.place(x= gui_vars.interval_x, y= gui_vars.interval_y)
        self.root.update()

        # ====================
        # [Config] Tabpages
        # ====================
        self.screen_width, self.screen_height= self.root.winfo_width(), self.root.winfo_height()
        #Left_width= self.lbl_MoveCoord.winfo_reqwidth()+ gui_vars.interval_x*11
        Left_width= int((self.screen_width-gui_vars.interval_x*2)*0.25)
        Left_height= int((self.screen_height-self.FileMenu.winfo_reqheight()*1- self.statuslabel.winfo_reqheight()*0-gui_vars.interval_y*2- self.lbl_CurrPos.winfo_reqheight()))
        self.tabbox = ttk.Notebook(self.root, width=Left_width, height=Left_height)
        self.tab_control = Tkinter.Frame(self.root)
	self.tab_loadscript = Tkinter.Frame(self.root)
	self.tab_imageprocess = Tkinter.Frame(self.root)

	self.tabbox.add(self.tab_control, text="CONTROL")
	self.tabbox.add(self.tab_loadscript, text="LOAD SCRIPT")
	self.tabbox.add(self.tab_imageprocess, text="IMAGE")

	#self.tabbox.place(x= 0, y= 0)
	self.tabbox.place(x= 0, y= self.lbl_CurrPos.winfo_y()+ self.lbl_CurrPos.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        print '*** Input Tab', Left_width, Left_height
        print '*** TAB',self.tabbox.winfo_reqwidth(), self.tabbox.winfo_reqheight()

        # ==================================================
        # [TAB CONTROL] Step Motor Control 
        # ==================================================
        self.lbl_MoveCoord= Tkinter.Label(self.tab_control, text="[ MOVE ]", font= myfont14)
        #self.lbl_MoveCoord.place(x= gui_vars.interval_x, y= self.lbl_CurrPos.winfo_y()+ self.lbl_CurrPos.winfo_height()+gui_vars.interval_y)
        self.lbl_MoveCoord.place(x= gui_vars.interval_x, y= gui_vars.interval_y)
        self.root.update()

        # ==================================================
        #  [TAB CONTROL] Move Amount Radio Button
        # ==================================================
        #self.rdbtnMvAmount_Mode= [('100', 100),('500', 500),('1k',1000),('10k',10000), ('100k',100000)]
        self.MvAmount= Tkinter.IntVar()
        self.rdbtn_MvAmount_1= Tkinter.Radiobutton(self.tab_control, text= self.rdbtnMvAmount_Mode[0][0], value= self.rdbtnMvAmount_Mode[0][1],variable= self.MvAmount,font= myfont12_Bold, command= self.rdbtn_MvAmount_click, indicatoron=0, width=5, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active,selectcolor= bgGray_select)
        self.rdbtn_MvAmount_1.place(x= gui_vars.interval_x, y=self.lbl_MoveCoord.winfo_y()+ self.lbl_MoveCoord.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        self.rdbtn_MvAmount_5= Tkinter.Radiobutton(self.tab_control, text= self.rdbtnMvAmount_Mode[1][0], value=self.rdbtnMvAmount_Mode[1][1], variable= self.MvAmount,font= myfont12_Bold, command= self.rdbtn_MvAmount_click, indicatoron=0, width=5, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active,selectcolor= bgGray_select)
        self.rdbtn_MvAmount_5.place(x= gui_vars.interval_x+ self.rdbtn_MvAmount_1.winfo_x()+ self.rdbtn_MvAmount_1.winfo_reqwidth(),y= self.rdbtn_MvAmount_1.winfo_y())
        self.root.update()
        self.rdbtn_MvAmount_10= Tkinter.Radiobutton(self.tab_control, text= self.rdbtnMvAmount_Mode[2][0], value=self.rdbtnMvAmount_Mode[2][1], variable= self.MvAmount,font= myfont12_Bold, command= self.rdbtn_MvAmount_click, indicatoron=0, width=5, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active,selectcolor= bgGray_select)
        self.rdbtn_MvAmount_10.place(x= gui_vars.interval_x+ self.rdbtn_MvAmount_5.winfo_x()+ self.rdbtn_MvAmount_5.winfo_reqwidth(),y= self.rdbtn_MvAmount_1.winfo_y())
        self.root.update()
        self.rdbtn_MvAmount_50= Tkinter.Radiobutton(self.tab_control, text= self.rdbtnMvAmount_Mode[3][0], value=self.rdbtnMvAmount_Mode[3][1], variable= self.MvAmount,font= myfont12_Bold, command= self.rdbtn_MvAmount_click, indicatoron=0, width=5, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active,selectcolor= bgGray_select)
        self.rdbtn_MvAmount_50.place(x= gui_vars.interval_x+ self.rdbtn_MvAmount_10.winfo_x()+ self.rdbtn_MvAmount_10.winfo_reqwidth(),y= self.rdbtn_MvAmount_1.winfo_y())
        self.root.update()
        self.rdbtn_MvAmount_100= Tkinter.Radiobutton(self.tab_control, text= self.rdbtnMvAmount_Mode[4][0], value=self.rdbtnMvAmount_Mode[4][1], variable= self.MvAmount,font= myfont12_Bold, command= self.rdbtn_MvAmount_click, indicatoron=0, width=5, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active,selectcolor= bgGray_select)
        self.rdbtn_MvAmount_100.place(x= gui_vars.interval_x+ self.rdbtn_MvAmount_50.winfo_x()+ self.rdbtn_MvAmount_50.winfo_reqwidth(),y= self.rdbtn_MvAmount_1.winfo_y())
        self.root.update()
        self.rdbtn_MvAmount_10.select()
        self.Move_interval= self.rdbtnMvAmount_Mode[2][1]
        self.lbl_posUnit_1= Tkinter.Label(self.tab_control, text='(step)')
        self.lbl_posUnit_1.place(x= self.rdbtn_MvAmount_100.winfo_x()+ self.rdbtn_MvAmount_100.winfo_width(), y= self.rdbtn_MvAmount_1.winfo_y()+gui_vars.interval_y)
        self.root.update()
        
        # ==================================================
        # [TAB CONTROL] Move 1 interval at specific Axis
        # ==================================================
        photo_up= self.IconResize(gui_vars.saveParaPath+'img_Up.png')
        self.btn_MoveUp= Tkinter.Button(self.tab_control,image= photo_up, cursor= 'hand2', command= lambda: self.btn_MoveAmount_click('Up'))
        self.btn_MoveUp.image= photo_up
        self.btn_MoveUp.place(x= self.rdbtn_MvAmount_10.winfo_x()+int(self.rdbtn_MvAmount_10.winfo_reqwidth()*0), y=self.rdbtn_MvAmount_1.winfo_y()+ self.rdbtn_MvAmount_1.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        photo_down= self.IconResize(gui_vars.saveParaPath+'img_Down.png')
        self.btn_MoveDown= Tkinter.Button(self.tab_control,image= photo_down, cursor= 'hand2', command= lambda: self.btn_MoveAmount_click('Down'))
        self.btn_MoveDown.image= photo_down
        self.btn_MoveDown.place(x= self.btn_MoveUp.winfo_x(), y=self.btn_MoveUp.winfo_y()+ self.btn_MoveUp.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        photo_left= self.IconResize(gui_vars.saveParaPath+'img_Left.png')
        self.btn_MoveLeft= Tkinter.Button(self.tab_control,image= photo_left, cursor= 'hand2', command= lambda: self.btn_MoveAmount_click('Left'))
        self.btn_MoveLeft.image= photo_left
        self.btn_MoveLeft.place(x= self.btn_MoveDown.winfo_x()- self.btn_MoveDown.winfo_width()- gui_vars.interval_x, y=self.btn_MoveDown.winfo_y())
        self.root.update()
        photo_right= self.IconResize(gui_vars.saveParaPath+'img_Right.png')
        self.btn_MoveRight= Tkinter.Button(self.tab_control,image= photo_right, cursor= 'hand2', command= lambda: self.btn_MoveAmount_click('Right'))
        self.btn_MoveRight.image= photo_right
        self.btn_MoveRight.place(x= self.btn_MoveDown.winfo_x()+ self.btn_MoveDown.winfo_width()+ gui_vars.interval_x, y=self.btn_MoveDown.winfo_y())
        self.root.update()


        self.btn_MoveZUp= Tkinter.Button(self.tab_control,image= photo_up, cursor= 'hand2', command= lambda: self.btn_MoveAmountZaxis_click('Up'))
        self.btn_MoveZUp.image= photo_up
        self.btn_MoveZUp.place(x= self.btn_MoveRight.winfo_x()+ self.btn_MoveRight.winfo_reqwidth()+ gui_vars.interval_x*4, y=self.btn_MoveUp.winfo_y())
        self.root.update()
        self.btn_MoveZDown= Tkinter.Button(self.tab_control,image= photo_down, cursor= 'hand2', command= lambda: self.btn_MoveAmountZaxis_click('Down'))
        self.btn_MoveZDown.image= photo_down
        self.btn_MoveZDown.place(x= self.btn_MoveZUp.winfo_x(), y=self.btn_MoveDown.winfo_y())
        self.root.update()
        
        # ==================================================
        # [TAB CONTROL] Seeding, Watering, Lighting, Grab Image
        # ==================================================
        photo_seed= self.IconResize(gui_vars.saveParaPath+'img_Seed.png')
        self.btn_Seed= Tkinter.Button(self.tab_control,image= photo_seed, cursor= 'hand2', command= self.btn_Seed_click)
        self.btn_Seed.image= photo_seed
        self.btn_Seed.place(x= self.btn_MoveUp.winfo_x()- int(self.btn_MoveUp.winfo_reqwidth()*2)- gui_vars.interval_x, y=self.btn_MoveDown.winfo_y()+ self.btn_MoveDown.winfo_reqheight()+ gui_vars.interval_y*2)
        self.root.update()
        photo_water= self.IconResize(gui_vars.saveParaPath+'img_Water.png')
        self.btn_Water= Tkinter.Button(self.tab_control,image= photo_water, cursor= 'hand2', command= self.btn_Water_click)
        self.btn_Water.image= photo_water
        self.btn_Water.place(x= self.btn_Seed.winfo_x()+ int(self.btn_Seed.winfo_reqwidth()*1.5)+ gui_vars.interval_x, y=self.btn_Seed.winfo_y())
        self.root.update()
        photo_light= self.IconResize(gui_vars.saveParaPath+'img_Light.png')
        self.btn_Light= Tkinter.Button(self.tab_control,image= photo_light, cursor= 'hand2', command= self.btn_Light_click)
        self.btn_Light.image= photo_light 
        self.btn_Light.place(x= self.btn_Water.winfo_x()+ int(self.btn_Water.winfo_reqwidth()*1.5)+ gui_vars.interval_x, y=self.btn_Seed.winfo_y())
        self.root.update()
        photo_cam= self.IconResize(gui_vars.saveParaPath+'img_Cam.png')
        self.btn_CamGrab= Tkinter.Button(self.tab_control,image= photo_cam, cursor= 'hand2', command= self.btn_saveImg_click)
        self.btn_CamGrab.image= photo_cam
        self.btn_CamGrab.place(x= self.btn_Light.winfo_x()+ int(self.btn_Light.winfo_reqwidth()*1.5)+ gui_vars.interval_x, y=self.btn_Seed.winfo_y())
        self.root.update()

        # ==================================================
        # [TAB CONTROL] Move To 
        # ==================================================
        self.lbl_Xpos= Tkinter.Label(self.tab_control, text= 'X :',font= myfont12)
        #self.lbl_Xpos.place(x= gui_vars.interval_x, y = self.btn_MoveDown.winfo_y()+ self.btn_MoveDown.winfo_height()+gui_vars.interval_y*3)
        self.lbl_Xpos.place(x= gui_vars.interval_x, y = self.btn_Seed.winfo_y()+ self.btn_Seed.winfo_height()+gui_vars.interval_y*3)
        self.root.update()
        self.entry_Xpos= Tkinter.Entry(self.tab_control, font= myfont12, width=4)
        self.entry_Xpos.insert(Tkinter.END, "0")
        self.entry_Xpos.place(x= self.lbl_Xpos.winfo_x()+ self.lbl_Xpos.winfo_width(), y= self.lbl_Xpos.winfo_y())
        self.root.update()
        self.lbl_Ypos= Tkinter.Label(self.tab_control, text= 'Y :',font= myfont12)
        self.lbl_Ypos.place(x= self.entry_Xpos.winfo_x()+ self.entry_Xpos.winfo_width()+ gui_vars.interval_x, y = self.lbl_Xpos.winfo_y())
        self.root.update()
        self.entry_Ypos= Tkinter.Entry(self.tab_control, font= myfont12, width=4)
        self.entry_Ypos.insert(Tkinter.END, "0")
        self.entry_Ypos.place(x= self.lbl_Ypos.winfo_x()+ self.lbl_Ypos.winfo_width(), y= self.lbl_Ypos.winfo_y())
        self.root.update()
        
        self.lbl_Zpos= Tkinter.Label(self.tab_control, text= 'Z :',font= myfont12)
        self.lbl_Zpos.place(x= self.entry_Ypos.winfo_x()+ self.entry_Ypos.winfo_width()+ gui_vars.interval_x, y = self.lbl_Xpos.winfo_y())
        self.root.update()
        self.entry_Zpos= Tkinter.Entry(self.tab_control, font= myfont12, width=4)
        self.entry_Zpos.insert(Tkinter.END, "0")
        self.entry_Zpos.place(x= self.lbl_Zpos.winfo_x()+ self.lbl_Zpos.winfo_width(), y= self.lbl_Zpos.winfo_y())
        self.root.update()

        self.lbl_posUnit= Tkinter.Label(self.tab_control, text='(step)')
        self.lbl_posUnit.place(x= self.entry_Zpos.winfo_x()+ self.entry_Zpos.winfo_width(), y= self.entry_Zpos.winfo_y()+gui_vars.interval_y)
        self.root.update()
        self.btn_MoveTo= Tkinter.Button(self.tab_control, text= 'GO', command= self.btn_MoveTo_click,font= myfont12_Bold, bg= self.bgGreen, fg= 'white', activebackground= self.bgGreen_active, activeforeground= 'white')
        self.btn_MoveTo.place(x= self.lbl_posUnit.winfo_x()+ self.lbl_posUnit.winfo_reqwidth()+ gui_vars.interval_x, y=self.lbl_Ypos.winfo_y())
        self.btn_MoveTo.focus_set()
        self.root.update()

        # ==================================================
        # [TAB CONTROL] Scanning Control 
        # ==================================================
        self.lbl_Scan= Tkinter.Label(self.tab_control, text="[ AUTO-SCAN ]", font= myfont14)
        self.lbl_Scan.place(x= gui_vars.interval_x, y= self.btn_MoveTo.winfo_y()+ self.btn_MoveTo.winfo_height()+gui_vars.interval_y)
        self.root.update()

        self.lbl_Scan1stPt= Tkinter.Label(self.tab_control, text= '*Start point (X, Y):',font= myfont12)
        self.lbl_Scan1stPt.place(x= gui_vars.interval_x, y = self.lbl_Scan.winfo_y()+ self.lbl_Scan.winfo_height()+gui_vars.interval_y)
        self.root.update()
        self.entry_1stXpos= Tkinter.Entry(self.tab_control, font= myfont12, width= 6)
        self.entry_1stXpos.insert(Tkinter.END, '{0}'.format(self.scan_X[0]))
        self.entry_1stXpos.place(x= self.lbl_Scan1stPt.winfo_x(), y= self.lbl_Scan1stPt.winfo_y()+ self.lbl_Scan1stPt.winfo_height())
        self.root.update()

        self.lbl_Scan1stPt_comma= Tkinter.Label(self.tab_control, text= ', ', font= myfont12)
        self.lbl_Scan1stPt_comma.place(x=self.entry_1stXpos.winfo_x()+self.entry_1stXpos.winfo_width(), y= self.entry_1stXpos.winfo_y())
        self.root.update()

        self.entry_1stYpos= Tkinter.Entry(self.tab_control, font= myfont12, width=6)
        self.entry_1stYpos.insert(Tkinter.END, '{0}'.format(self.scan_Y[0]))
        self.entry_1stYpos.place(x= self.lbl_Scan1stPt_comma.winfo_x()+self.lbl_Scan1stPt_comma.winfo_width(), y= self.lbl_Scan1stPt_comma.winfo_y())
        self.root.update()
       
        self.lbl_ScanInterval= Tkinter.Label(self.tab_control, text='* Interval (X, Y) :', font= myfont12)
        self.lbl_ScanInterval.place(x= self.entry_1stYpos.winfo_x()+ self.entry_1stYpos.winfo_reqwidth()+ gui_vars.interval_x*4, y= self.lbl_Scan1stPt.winfo_y())
        self.root.update()
        self.entry_ScanInterval_X= Tkinter.Entry(self.tab_control, font=myfont12, width=6)
        self.entry_ScanInterval_X.insert(Tkinter.END, '{0}'.format(self.scan_X[1]))
        self.entry_ScanInterval_X.place(x= self.lbl_ScanInterval.winfo_x(), y= self.lbl_ScanInterval.winfo_y()+self.lbl_ScanInterval.winfo_height())
        self.root.update()
        self.lbl_ScanInterval_comma= Tkinter.Label(self.tab_control, text= ', ', font= myfont12)
        self.lbl_ScanInterval_comma.place(x=self.entry_ScanInterval_X.winfo_x()+self.entry_ScanInterval_X.winfo_width(), y= self.entry_ScanInterval_X.winfo_y())
        self.root.update()
        self.entry_ScanInterval_Y= Tkinter.Entry(self.tab_control, font= myfont12, width=6)
        self.entry_ScanInterval_Y.insert(Tkinter.END, '{0}'.format(self.scan_Y[1]))
        self.entry_ScanInterval_Y.place(x= self.lbl_ScanInterval_comma.winfo_x()+self.lbl_ScanInterval_comma.winfo_width(), y= self.lbl_ScanInterval_comma.winfo_y())
        self.root.update()

        self.lbl_ScanAmount= Tkinter.Label(self.tab_control, text='* Scanning Step (X, Y) :', font= myfont12)
        self.lbl_ScanAmount.place(x= self.entry_1stXpos.winfo_x(), y= self.entry_1stXpos.winfo_y()+ self.entry_1stXpos.winfo_height()+gui_vars.interval_y)
        self.root.update()
        self.entry_ScanAmount_X= Tkinter.Entry(self.tab_control, font=myfont12, width=6)
        self.entry_ScanAmount_X.insert(Tkinter.END, '{0}'.format(self.scan_X[2]))
        self.entry_ScanAmount_X.place(x= self.lbl_ScanAmount.winfo_x(), y= self.lbl_ScanAmount.winfo_y()+self.lbl_ScanAmount.winfo_height())
        self.root.update()
        self.lbl_ScanAmount_comma= Tkinter.Label(self.tab_control, text= ', ', font= myfont12)
        self.lbl_ScanAmount_comma.place(x=self.entry_ScanAmount_X.winfo_x()+self.entry_ScanAmount_X.winfo_width(),y= self.entry_ScanAmount_X.winfo_y())
        self.root.update()
        self.entry_ScanAmount_Y= Tkinter.Entry(self.tab_control, font= myfont12, width=6)
        self.entry_ScanAmount_Y.insert(Tkinter.END, '{0}'.format(self.scan_Y[2]))
        self.entry_ScanAmount_Y.place(x= self.lbl_ScanAmount_comma.winfo_x()+self.lbl_ScanAmount_comma.winfo_width(), y= self.lbl_ScanAmount_comma.winfo_y())
        self.root.update()

        self.btn_StartScan= Tkinter.Button(self.tab_control, text= 'Start Scan', command= self.btn_StartScan_click,font= myfont12_Bold, fg= 'white', activeforeground='white', bg=self.bgGreen, activebackground=self.bgGreen_active, width= btn_width, height= btn_height)
        self.btn_StartScan.place(x= self.entry_ScanInterval_X.winfo_x()+ gui_vars.interval_x*6, y=self.lbl_ScanAmount.winfo_y()+gui_vars.interval_y*2)
        self.root.update()
        
        # ==================================================
        # [TAB LOAD SCRIPT]  
        # ==================================================
        self.lbl_loadscript= Tkinter.Label(self.tab_loadscript, text="[ Load & Run Script ]", font= myfont14)
        self.lbl_loadscript.place(x= gui_vars.interval_x, y= gui_vars.interval_y)
        self.root.update()
        
        self.entry_scriptPath= Tkinter.Entry(self.tab_loadscript, font= myfont12, width=25)
        self.entry_scriptPath.insert(Tkinter.END, self.scriptPath)
        self.entry_scriptPath.place(x= self.lbl_loadscript.winfo_x(), y= self.lbl_loadscript.winfo_y()+ self.lbl_loadscript.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        self.btn_choosescript= Tkinter.Button(self.tab_loadscript, text='...', command= self.btn_choosescript_click, font= myfont8, width=0, height=0)
        self.btn_choosescript.place(x= self.entry_scriptPath.winfo_x()+ self.entry_scriptPath.winfo_reqwidth()+ gui_vars.interval_x, y= self.entry_scriptPath.winfo_y())
        self.root.update()
        self.btn_loadscript= Tkinter.Button(self.tab_loadscript, text='Load', command= self.btn_loadscript_click, font= myfont12_Bold, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active)
        self.btn_loadscript.place(x= self.entry_scriptPath.winfo_x(), y= self.entry_scriptPath.winfo_y()+ self.entry_scriptPath.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        self.btn_savescript= Tkinter.Button(self.tab_loadscript, text='Save', command= self.btn_savescript_click, font= myfont12_Bold, fg= 'white', activeforeground='white', bg= bgGray, activebackground= bgGray_active)
        self.btn_savescript.place(x= self.btn_loadscript.winfo_x()+ self.btn_loadscript.winfo_reqwidth()+ gui_vars.interval_x*2, y= self.btn_loadscript.winfo_y())
        self.root.update()
        self.btn_runscript= Tkinter.Button(self.tab_loadscript, text='RUN', command= self.btn_runscript_click, font= myfont12_Bold, fg= 'white', activeforeground='white', bg= self.bgGreen, activebackground= self.bgGreen_active)
        self.btn_runscript.place(x= self.btn_savescript.winfo_x()+ self.btn_savescript.winfo_reqwidth()+ gui_vars.interval_x*2, y= self.btn_savescript.winfo_y())
        self.btn_runscript.focus_set()
        self.root.update()
        
        #self.txtbox_script = ScrolledText.ScrolledText(self.tab_loadscript, width=40, height= 30 ,font = myfont10, bd = 2, relief = RIDGE, vscrollmode= 'dynamic')
        self.txtbox_script = Pmw.ScrolledText(self.tab_loadscript, text_width=40, text_height= 20, hscrollmode= 'dynamic', vscrollmode= 'static', text_wrap= 'none', labelpos= 'n', label_text= "NaN")#, rowheader= 1)
        self.txtbox_script.place(x= self.btn_loadscript.winfo_x(), y= self.btn_loadscript.winfo_y()+ self.btn_loadscript.winfo_reqheight()+ gui_vars.interval_y)

        # ==================================================
        # [TAB IMAGE] Image Processing 
        # ==================================================
        self.btn_saveImg= Tkinter.Button(self.tab_imageprocess, text='Save Image', command= self.btn_saveImg_click,font= myfont14, width= btn_width, height= btn_height)
        self.btn_saveImg.place(x= gui_vars.interval_x, y= gui_vars.interval_y)
        self.root.update()

        self.lbl_scracth_detect= Tkinter.Label(self.tab_imageprocess, text="[ Detect Green Plant ]", font= myfont14)
        self.lbl_scracth_detect.place(x= gui_vars.interval_x, y= self.btn_saveImg.winfo_y()+ self.btn_saveImg.winfo_reqheight()+ gui_vars.interval_y)
        self.root.update()
        
        self.btn_detect= Tkinter.Button(self.tab_imageprocess, text='Detect', command= self.detectGreenPlant,font= myfont12_Bold, width= btn_width, height= btn_height, fg= 'white',activeforeground='white', bg= bgGray,activebackground= bgGray_active)
        self.btn_detect.place(x= self.lbl_scracth_detect.winfo_x()+ self.lbl_scracth_detect.winfo_reqwidth()+ gui_vars.interval_x, y= self.lbl_scracth_detect.winfo_y())
        self.root.update()
        #=============================================
        # [group] Plant Index 
        #=============================================
        self.grp_PlantIndex= Tkinter.LabelFrame(self.tab_imageprocess, text= 'Plant Index', width=Left_width-gui_vars.interval_x*2 ,height=40, relief=Tkinter.RIDGE, padx=0, pady=0)#, font= self.__myfont12_Bold)
        
        y_rdbox= self.lbl_scracth_detect.winfo_y()+ self.lbl_scracth_detect.winfo_height()+ gui_vars.interval_y
        self.lst_PlantIndex_rdbox = list()
        self.PlantIndex= Tkinter.IntVar()
        for idx, name in enumerate(gui_vars.rdbox_PlantIndexItem):
            self.lst_PlantIndex_rdbox.append(Tkinter.Radiobutton(self.grp_PlantIndex, text = name, value=idx, variable = self.PlantIndex, indicatoron=1, command= self.rdbtn_PlantINdex_click))
            self.lst_PlantIndex_rdbox[idx].place(x= gui_vars.interval_x+ gui_vars.interval_rdbox*idx, y=0)
        self.lst_PlantIndex_rdbox[0].select()
        self.grp_PlantIndex.place(x= gui_vars.interval_x, y=y_rdbox)
        self.root.update()
        #=============================================
        # [group] Binary Method  
        #=============================================
        self.grp_BinaryMethod= Tkinter.LabelFrame(self.tab_imageprocess, text= 'Binary Method', width=Left_width-gui_vars.interval_x*2 ,height=40, relief=Tkinter.RIDGE, padx=0, pady=0)#, font= self.__myfont12_Bold)
        
        self.lst_BinaryMethod_rdbox = list()
        self.BinaryMethodIndex= Tkinter.IntVar()
        for idx, name in enumerate(gui_vars.rdbox_BinaryMethodItem):
            self.lst_BinaryMethod_rdbox.append(Tkinter.Radiobutton(self.grp_BinaryMethod, text = name, value=idx, variable = self.BinaryMethodIndex, indicatoron=1, command= self.rdbtn_BinaryMethodIndex_click))
            self.lst_BinaryMethod_rdbox[idx].place(x= gui_vars.interval_x+ (gui_vars.interval_rdbox+9)*idx, y=0)
        self.lst_BinaryMethod_rdbox[0].select()
        self.grp_BinaryMethod.place(x= gui_vars.interval_x, y=self.grp_PlantIndex.winfo_y()+ self.grp_PlantIndex.winfo_reqheight()+ gui_vars.interval_y*1)
        self.root.update()
        
        self.scale_threshold_graylevel = Tkinter.Scale(self.tab_imageprocess , from_= 0 , to = 255 , orient = Tkinter.HORIZONTAL , label = "Gray_level", font = myfont12, width = 7, length = 300 )
        self.scale_threshold_graylevel.set(self.threshold_graylevel)
        self.scale_threshold_graylevel.place(x= self.grp_BinaryMethod.winfo_x(), y= self.grp_BinaryMethod.winfo_y()+ self.grp_BinaryMethod.winfo_reqheight()+gui_vars.interval_y*2)
        #self.scale_threshold_graylevel.config(state= 'disabled')
        self.root.update()

        self.scale_threshold_MinSize = Tkinter.Scale(self.tab_imageprocess, from_ = 0 , to = 99999 , orient = Tkinter.HORIZONTAL , label = "Min Contour_size", font = myfont12, width = 7, length = 300 )
        self.scale_threshold_MinSize.set(self.threshold_MinSize)

        self.scale_threshold_MinSize.place(x= self.scale_threshold_graylevel.winfo_x(), y= self.scale_threshold_graylevel.winfo_y()+ self.scale_threshold_graylevel.winfo_height())
        self.root.update()
        self.scale_threshold_MaxSize = Tkinter.Scale(self.tab_imageprocess, from_ = 0 , to = 99999 , orient = Tkinter.HORIZONTAL , label = "Max Contour_size", font = myfont12, width = 7, length = 300 )
        self.scale_threshold_MaxSize.set(self.threshold_MaxSize)

        self.scale_threshold_MaxSize.place(x= self.scale_threshold_MinSize.winfo_x(), y= self.scale_threshold_MinSize.winfo_y()+ self.scale_threshold_MinSize.winfo_height())
        
        # ==================================================
        # [ROOT] Main Image Frame 
        # ==================================================
        #self.frame_width, self.frame_height= int(0.5*(self.screen_width-Left_width- gui_vars.interval_x*2)), int(0.5*(self.screen_height-self.FileMenu.winfo_reqheight()- self.statuslabel.winfo_reqheight() -gui_vars.interval_y*2))
        self.frame_width, self.frame_height= int(0.5*(self.screen_width-Left_width- gui_vars.interval_x*2)), int(0.5*(self.screen_height-self.FileMenu.winfo_reqheight()*0- self.statuslabel.winfo_reqheight() -gui_vars.interval_y*1))
        print '*** Frame w,h: ',self.frame_width, self.frame_height 
        self.frame= np.zeros((int(self.frame_height), int(self.frame_width),3),np.uint8)
        #frame= cv2.resize(frame,(self.frame_width,self.frame_height),interpolation=cv2.INTER_LINEAR)
        result = Image.fromarray(self.frame)
        result = ImageTk.PhotoImage(result)
        self.panel = Tkinter.Label(self.root , image = result)
        self.panel.image = result
        self.panel.place(x=Left_width+gui_vars.interval_x, y= 0)
        self.root.update()
        # ==================================================
        # [ROOT] Display merge Image Frame 
        # ==================================================
        self.mergeframe_width, self.mergeframe_height= self.frame_width, self.frame_height*2+2
        self.mergeframe= np.zeros((int(self.mergeframe_height), int(self.mergeframe_width),3),np.uint8)
        #frame= cv2.resize(frame,(self.frame_width,self.frame_height),interpolation=cv2.INTER_LINEAR)
        cv2.putText(self.mergeframe, 'Display Scanning Result',(10,20),cv2.FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),1)
        result = Image.fromarray(self.mergeframe)
        result = ImageTk.PhotoImage(result)
        self.panel_mergeframe = Tkinter.Label(self.root , image = result)
        self.panel_mergeframe.image = result
        self.panel_mergeframe.place(x=self.panel.winfo_x()+ self.panel.winfo_reqwidth(), y= 0)
        self.root.update()
        # ==================================================
        # [ROOT] One Shot Image Frame 
        # ==================================================
        self.singleframe_width, self.singleframe_height= self.frame_width, self.frame_height
        self.singleframe= np.zeros((int(self.singleframe_height), int(self.singleframe_width),3),np.uint8)
        cv2.putText(self.singleframe, '1 shot Result',(10,20),cv2.FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),1)
        result = Image.fromarray(self.singleframe)
        result = ImageTk.PhotoImage(result)
        self.panel_singleframe = Tkinter.Label(self.root , image = result)
        self.panel_singleframe.image = result
        self.panel_singleframe.place(x=self.panel.winfo_x(), y= self.panel.winfo_y()+ self.panel.winfo_height())
        self.root.update()
       
        # ==================================================
        #  Camera & Arduino Connection
        # ==================================================
        self.ArdMntr= MonitorThread()
        self.ArdMntr.start()
        
        self.CamMntr= CameraLink(self.CameraID)
        #self.CamMntr.connect_camera()
        
        # ==================================================
        #  Green Plant Indetifier
        # ==================================================
        self.plantsArea = PlantIdentifier()

        # ==================================================
        #  UI callback setting
        # ==================================================
        self.panel.after(50, self.check_frame_update)
        self.lbl_CurrPos.after(5, self.UI_callback)
        self.statuslabel.after(5, self.check_status)
        self.panel_mergeframe.bind('<Button-1>',self.mouse_LeftClick)
        self.root.bind('<F1>',self.rdbtn_MvAmount_click)
        self.root.bind('<F2>',self.rdbtn_MvAmount_click)
        self.root.bind('<F3>',self.rdbtn_MvAmount_click)
        self.root.bind('<F4>',self.rdbtn_MvAmount_click)
        self.root.bind('<F5>',self.rdbtn_MvAmount_click)
        #self.root.bind('<Up>',self.btn_MoveUp_click)
        self.root.bind('<Up>',self.btn_MoveAmount_click)
        self.root.bind('<Down>',self.btn_MoveAmount_click)
        self.root.bind('<Left>',self.btn_MoveAmount_click)
        self.root.bind('<Right>',self.btn_MoveAmount_click)
        self.root.bind('<Control-Up>',self.btn_MoveAmountZaxis_click)
        self.root.bind('<Control-Down>',self.btn_MoveAmountZaxis_click)
        '''
        self.root.bind('<Down>',self.btn_MoveDown_click)
        self.root.bind('<Left>',self.btn_MoveLeft_click)
        self.root.bind('<Right>',self.btn_MoveRight_click)
        self.root.bind('<Control-Up>',self.btn_MoveZUp_click)
        self.root.bind('<Control-Down>',self.btn_MoveZDown_click)
        '''
        # ====== Override CLOSE function ==============
        self.root.protocol('WM_DELETE_WINDOW',self.on_exit)
        # ==================================================
        #   Thread
        # ==================================================
        self.main_run_judge= True
        #self.thread_main= threading.Thread(target= self.main_run)
        self.thread_main= class_MyThread.Thread(self.main_run)
        self.thread_main.start()
        self.scanning_judge= True
        #self.thread_scanning= threading.Thread(target= self.scanning_run)
        #self.thread_scanning= class_MyThread.Thread(self.scanning_run)
        #self.thread_scanning.start()
        time.sleep(1)
        if self.ArdMntr.connect: 
            self.ArdMntr.set_MaxSpeed(self.MaxSpeed[0],'x')
            self.ArdMntr.set_MaxSpeed(self.MaxSpeed[1],'y')
            self.ArdMntr.set_MaxSpeed(self.MaxSpeed[2],'z')
            self.ArdMntr.set_Acceleration(self.Acceleration[0],'x')
            self.ArdMntr.set_Acceleration(self.Acceleration[1],'y')
            self.ArdMntr.set_Acceleration(self.Acceleration[2],'z')

    def store_para(self, arg_filepath, arg_filename):
        saveDict={}
        saveDict['thrshd_gray']= self.scale_threshold_graylevel.get()
        saveDict['thrshd_Minsize']= self.scale_threshold_MinSize.get()
        saveDict['thrshd_Maxsize']= self.scale_threshold_MaxSize.get()
        saveDict['Scan_X (Beg,Interval,Amount)']= [int(self.entry_1stXpos.get()), int(self.entry_ScanInterval_X.get()), int(self.entry_ScanAmount_X.get())]
        saveDict['Scan_Y (Beg,Interval,Amount)']= [int(self.entry_1stYpos.get()), int(self.entry_ScanInterval_Y.get()), int(self.entry_ScanAmount_Y.get())]
        saveDict['limit Maximum (X,Y)']= self.limit
        saveDict['Max Speed (X, Y)']= self.MaxSpeed
        saveDict['Ac/Deceleration (X, Y)']= self.Acceleration
        saveDict['Camera ID']= self.CameraID
        saveDict['Peripheral Setting']= self.Peripheral_para
        saveDict['Move Amount type (5 types)']= self.rdbtnMvAmount_Mode
        saveDict['script Path']= self.scriptPath
        self.config.write_json(saveDict)
        print "Para set"

    # Override CLOSE function
    def on_exit(self):
        #When you click to exit, this function is called
        if tkMessageBox.askyesno("Exit", "Do you want to quit the application?"):
            self.store_para(gui_vars.saveParaPath, gui_vars.configName)
            print 'Close Main Thread...'
            self.main_run_judge= False
            self.ArdMntr.exit= True
            self.scanning_judge= False
            #self.CamMntr.stop_clean_buffer()
            #del(self.thread_main)
            self.thread_main.exit()
            print 'Close Arduino Thread...'
            #del(self.CamMntr.thread_clean_buffer)
            #print 'Close Scanning Thread...'
            #del(self.thread_scanning)
            print self.MaxSpeed
            
            self.CamMntr.release_cap()
            self.root.destroy()

    def UI_callback(self):
        if self.ArdMntr.connect== True:
            tmp_text= 'Location: (X, Y, Z)= ('+self.ArdMntr.cmd_state.strCurX+', '+self.ArdMntr.cmd_state.strCurY+', '+self.ArdMntr.cmd_state.strCurZ+')'
        else:
            tmp_text='Arduino Connection Refuesed!'

        self.lbl_CurrPos.config(text= tmp_text)
        self.lbl_CurrPos.after(10,self.UI_callback)
    
    def IconResize(self, arg_readPath, arg_zoom=1, arg_subsample= 4):
        photo_resize=PhotoImage(file=arg_readPath)
        photo_resize= photo_resize.zoom(arg_zoom)
        photo_resize= photo_resize.subsample(arg_subsample)
        return photo_resize

    def mouse_LeftClick(self, event):
        if self.checkmouse_panel_mergeframe:
            mouse_x, mouse_y= event.x, event.y
            #print '>> mouse(X,Y): ',mouse_x, mouse_y
            #print '>> split(X,Y): ', self.mergeframe_splitX, self.mergeframe_splitY

            begX= gui_vars.interval_x
            begY= self.mergeframe_spaceY
            tmp_X, tmp_Y= int((mouse_x-begX)/self.mergeframe_splitX), int((mouse_y-begY)/self.mergeframe_splitY)
            #print '>> RANGE(X,Y): ',begY+ self.mergeframe_splitY*self.scan_Y[2] ,begX+ self.mergeframe_splitX*self.scan_X[2]
            if begX< mouse_x < begX+ self.mergeframe_splitX*self.scan_Y[2] and begY< mouse_y< begY+ self.mergeframe_splitY*self.scan_X[2]:
                if self.readmergeframeIndex == gui_vars.scanIndex:
                    readPath= gui_vars.saveScanningPath
                    tmp_filename= '{0}_{1}'.format(tmp_Y*self.scan_X[1], tmp_X*self.scan_Y[1]) 
                else:
                    readPath= gui_vars.saveImageProccesPath 
                    tmp_filename= '{0}_{1}'.format(tmp_Y, tmp_X) 
                #print 'click file: ', tmp_filename
                tmp_frame= utils_tool.readImage(readPath+ self.readmergeframeIndex+'_'+self.saveTimeIndex+'_'+tmp_filename+'.jpg')
                if tmp_frame is not False:
                    self.imagename= self.readmergeframeIndex+'_'+self.saveTimeIndex+tmp_filename
                    self.singleframe= tmp_frame.copy()
                    self.display_panel_singleframe(tmp_frame)

                    mergeframe_canvas= self.mergeframe.copy()
                    cv2.rectangle(mergeframe_canvas,(begX+self.mergeframe_splitX*tmp_X,begY+self.mergeframe_splitY*tmp_Y),(begX+self.mergeframe_splitX*(tmp_X+1), begY+self.mergeframe_splitY*(tmp_Y+1)),(0,255,100),2 )
                    result = Image.fromarray(mergeframe_canvas)
                    result = ImageTk.PhotoImage(result)
                    self.panel_mergeframe.configure(image = result)
                    self.panel_mergeframe.image = result
            

    def check_status(self):
        self.statuslabel.config(text= self.strStatus)
        self.statuslabel.after(10,self.check_status)

    def Lock_Menubar(self, arg_Lock):
        if arg_Lock:
            self.menubar.entryconfig('File', state='disabled')
            self.menubar.entryconfig('Setting', state='disabled')
            self.menubar.entryconfig('Communication', state='disabled')
            self.menubar.entryconfig('Image Processing', state='disabled')
            self.checkmouse_panel_mergeframe= False
        else:
            self.menubar.entryconfig('File', state='normal')
            self.menubar.entryconfig('Setting', state='normal')
            self.menubar.entryconfig('Communication', state='normal')
            self.menubar.entryconfig('Image Processing', state='normal')
            self.checkmouse_panel_mergeframe= True

    def Lock_tabcontrol(self, arg_Lock):
        if arg_Lock:
            self.btn_MoveTo.config(state= 'disabled')
            self.entry_Xpos.config(state= 'disabled')
            self.entry_Ypos.config(state= 'disabled')
            self.entry_Zpos.config(state= 'disabled')
            self.btn_detect.config(state= 'disabled')
            self.btn_saveImg.config(state= 'disabled')
            self.entry_1stXpos.config(state= 'disabled')
            self.entry_1stYpos.config(state= 'disabled')
            self.entry_ScanInterval_X.config(state= 'disabled')
            self.entry_ScanInterval_Y.config(state= 'disabled')
            self.entry_ScanAmount_X.config(state= 'disabled')
            self.entry_ScanAmount_Y.config(state= 'disabled')
            self.checkmouse_panel_mergeframe= False
            self.btn_MoveUp.config(state= 'disabled')
            self.btn_MoveDown.config(state= 'disabled')
            self.btn_MoveLeft.config(state= 'disabled')
            self.btn_MoveRight.config(state= 'disabled')
            self.btn_MoveZUp.config(state= 'disabled')
            self.btn_MoveZDown.config(state= 'disabled')
            self.btn_Water.config(state= 'disabled') 
            self.btn_Seed.config(state= 'disabled') 
            self.btn_CamGrab.config(state= 'disabled') 
        else:
            self.btn_MoveTo.config(state= 'normal')
            self.entry_Xpos.config(state= 'normal')
            self.entry_Ypos.config(state= 'normal')
            self.entry_Zpos.config(state= 'normal')
            self.btn_detect.config(state= 'normal')
            self.btn_saveImg.config(state= 'normal')
            self.entry_1stXpos.config(state= 'normal')
            self.entry_1stYpos.config(state= 'normal')
            self.entry_ScanInterval_X.config(state= 'normal')
            self.entry_ScanInterval_Y.config(state= 'normal')
            self.entry_ScanAmount_X.config(state= 'normal')
            self.entry_ScanAmount_Y.config(state= 'normal')
            self.checkmouse_panel_mergeframe= True
            self.btn_MoveUp.config(state= 'normal')
            self.btn_MoveDown.config(state= 'normal')
            self.btn_MoveLeft.config(state= 'normal')
            self.btn_MoveRight.config(state= 'normal')
            self.btn_MoveZUp.config(state= 'normal')
            self.btn_MoveZDown.config(state= 'normal')
            self.btn_Water.config(state= 'normal') 
            self.btn_Seed.config(state= 'normal') 
            self.btn_CamGrab.config(state= 'normal') 

    def Lock_tabloadscript(self, arg_Lock):
        if arg_Lock:
            self.entry_scriptPath.config(state= 'disabled')
            self.btn_loadscript.config(state= 'disabled')
            self.btn_choosescript.config(state= 'disabled')
            self.btn_savescript.config(state= 'disabled')
            self.txtbox_script.configure(text_state= 'disabled')
        else:
            self.entry_scriptPath.config(state= 'normal')
            self.btn_loadscript.config(state= 'normal')
            self.btn_choosescript.config(state= 'normal')
            self.btn_savescript.config(state= 'normal')
            self.txtbox_script.configure(text_state= 'normal')

    def plastic_set_background(self):
        frame= self.CamMntr.get_frame()
        self.imageProcessor.set_background(frame)

    def rdbtn_PlantINdex_click(self):
        pass
    def rdbtn_BinaryMethodIndex_click(self):
        print 'BinaryMethodIndex: ',self.BinaryMethodIndex.get()
        if self.BinaryMethodIndex.get()==0:
            self.scale_threshold_graylevel.config(state= 'normal', label='Gray_level', fg='black')
        else:
            self.scale_threshold_graylevel.config(state= 'disabled', label='Gray_level (Disable)', fg= 'gray')


    def detectGreenPlant(self):
        self.plantsArea.setimage(self.singleframe)
        if self.PlantIndex.get()==0:
            _, image_plantIndex,_= self.plantsArea.LABimage(True)
        elif self.PlantIndex.get()==1:
            image_plantIndex= self.plantsArea.NDIimage(True)
        elif self.PlantIndex.get()==2:
            image_plantIndex= self.plantsArea.ExGimage(True)
        
        self.threshold_graylevel= self.scale_threshold_graylevel.get()
        image_plantIndex_thr= imgProcess_tool.binarialization(image_plantIndex.astype(np.uint8), self.BinaryMethodIndex.get(), self.threshold_graylevel)
        cv2.imwrite('Debug/img_thr.jpg',image_plantIndex_thr)
        
        self.threshold_MinSize, self.threshold_MaxSize=int(self.scale_threshold_MinSize.get()), int(self.scale_threshold_MaxSize.get())
        result= imgProcess_tool.findContours(image_plantIndex_thr, self.plantsArea.image_raw, (self.threshold_MinSize, self.threshold_MaxSize),True)
        #self.singleframe= result_ExG
        self.display_panel_singleframe(result)
        self.set_mergeframe_size(2,2)
        self.reset_mergeframe()
        self.display_panel_mergeframe(self.singleframe.copy(), 0, 0)
        self.display_panel_mergeframe(image_plantIndex.astype(np.uint8), 1, 0)
        self.display_panel_mergeframe(image_plantIndex_thr, 0, 1)
        self.display_panel_mergeframe(result, 1, 1)
        self.saveTimeIndex= datetime.now().strftime('%Y%m%d%H%M%S')
        self.readmergeframeIndex= gui_vars.rdbox_PlantIndexItem[self.PlantIndex.get()]
        
        print '=== ', gui_vars.saveImageProccesPath, self.readmergeframeIndex+'_'+self.saveTimeIndex
        self.saveImg_function(self.singleframe, gui_vars.saveImageProccesPath, self.readmergeframeIndex+'_'+self.saveTimeIndex+'_0_0')
        self.saveImg_function(image_plantIndex.astype(np.uint8), gui_vars.saveImageProccesPath, self.readmergeframeIndex+'_'+self.saveTimeIndex+'_0_1')
        self.saveImg_function(image_plantIndex_thr, gui_vars.saveImageProccesPath, self.readmergeframeIndex+'_'+self.saveTimeIndex+'_1_0')
        self.saveImg_function(result, gui_vars.saveImageProccesPath, self.readmergeframeIndex+'_'+self.saveTimeIndex+'_1_1')
        self.checkmouse_panel_mergeframe= True
        pass

    def method_OtsuBinary(self):
        print 'Start Otsu Binary.... '
        '''
        self.imageProcessor.set_threshold_size(int(self.scale_threshold_MinSize.get()))
        self.imageProcessor.set_threshold_graylevel(int(self.scale_threshold_graylevel.get()))
        result= self.imageProcessor.get_contour(self.singleframe, True, gui_vars.savePath, 'Otsu_Binary_'+self.imagename, 1)
        '''
        self.threshold_MaxSize= int(self.scale_threshold_MaxSize.get())
        img_thr= imgProcess_tool.binarialization(self.singleframe, 1)
        result= imgProcess_tool.findContours(img_thr, self.singleframe, [0,self.threshold_MaxSize] )
        self.display_panel_singleframe(result)

    def method_SimpleBinary(self):
        print 'rdbtn: ',self.PlantIndex.get()
        print 'Start Binarization with ... '
        '''
        self.imageProcessor.set_threshold_size(int(self.scale_threshold_MinSize.get()))
        self.imageProcessor.set_threshold_graylevel(int(self.scale_threshold_graylevel.get()))
        result= self.imageProcessor.get_contour(self.singleframe, True, gui_vars.savePath, 'Simple_Binary_'+self.imagename, 0)
        '''
        self.threshold_MaxSize= int(self.scale_threshold_MaxSize.get())
        self.threshold_graylevel= int(self.scale_threshold_graylevel.get())
        img_thr= imgProcess_tool.binarialization(self.singleframe, 0, self.threshold_graylevel)
        result= imgProcess_tool.findContours(img_thr, self.singleframe, [0,self.threshold_MaxSize])

        self.display_panel_singleframe(result)

    def set_ArdConnect(self):
        self.ArdMntr.connect_serial()

    def set_CamConnect(self):
        cameraID= CameraConnection(self.root, self.CamMntr.camera_id)
        print '*** ',cameraID.result, ', ', self.CamMntr.camera_id
        if cameraID.result is not None and cameraID.result != self.CamMntr.camera_id:
            print 'Switch Camera ID'
            self.CamMntr.connect_camera(cameraID.result)
            self.CameraID= self.CamMntr.camera_id

    def set_Peripheral(self):
        #Var= PeripheralSetting(self.root, [('Fan',8),('Water Pump',9)])
        #print '>>> ',self.Peripheral_para
        Var= PeripheralSetting(self.root, self.Peripheral_para)
        if Var.result is not None:
            self.Peripheral_para= Var.result
        print '*** Return Value: ',Var.result

    def set_Motor(self):
        if self.ArdMntr.connect:
            Var= MotorSetting(self.root, self.MaxSpeed, self.Acceleration)
            if Var.result is not None:
                print 'result: ',Var.result
                #self.MaxSpeed= [Var.result[0], Var.result[2]]
                #self.Acceleration= [Var.result[1], Var.result[3]]
                self.MaxSpeed= [Var.result[0], Var.result[2], Var.result[4]]
                self.Acceleration= [Var.result[1], Var.result[3], Var.result[5]]
                self.ArdMntr.set_MaxSpeed(self.MaxSpeed[0],'x')
                self.ArdMntr.set_MaxSpeed(self.MaxSpeed[1],'y')
                self.ArdMntr.set_MaxSpeed(self.MaxSpeed[2],'z')
                self.ArdMntr.set_Acceleration(self.Acceleration[0],'x')
                self.ArdMntr.set_Acceleration(self.Acceleration[1],'y')
                self.ArdMntr.set_Acceleration(self.Acceleration[2],'z')
            #self.ArdMntr.set_MaxSpeed()
        else:
            tkMessageBox.showerror("Error", "Arduino connection refused!\n Please check its connection.")


    def set_frame(self, frame):
        self.frame= frame
    
    def display_panel_singleframe(self, arg_frame):
        tmp_frame= cv2.cvtColor(arg_frame, cv2.COLOR_BGR2RGB)
        #tmp_frame = self.mark_cross_line(tmp_frame)
	tmp_frame= cv2.resize(tmp_frame,(self.singleframe_width,self.singleframe_height),interpolation=cv2.INTER_LINEAR)
        result = Image.fromarray(tmp_frame)
        result = ImageTk.PhotoImage(result)
        self.panel_singleframe.configure(image = result)
        self.panel_singleframe.image = result

    def reset_mergeframe(self):
        self.mergeframe= np.zeros((int(self.mergeframe_height), int(self.mergeframe_width),3),np.uint8)
        cv2.putText(self.mergeframe, 'Display Scanning Result',(10,20),cv2.FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),1)

    def set_mergeframe_size(self, arg_x, arg_y):
        self.mergeframe_splitX= int((self.mergeframe_width-gui_vars.interval_x*2)/arg_y)
        self.mergeframe_splitY= int((self.mergeframe_height-100)/arg_x)
    
    def display_panel_mergeframe(self, arg_frame, arg_stepX, arg_stepY): 
        print '*** ',len(arg_frame.shape)
        if len(arg_frame.shape)==3:
            tmp_frame= cv2.cvtColor(arg_frame, cv2.COLOR_BGR2RGB)
        else: 
            tmp_frame= cv2.cvtColor(arg_frame, cv2.COLOR_GRAY2RGB)

        tmp_frame= cv2.resize(tmp_frame,(self.mergeframe_splitX,self.mergeframe_splitY),interpolation=cv2.INTER_LINEAR)
        begX= gui_vars.interval_x+self.mergeframe_splitX*arg_stepX
        begY= self.mergeframe_spaceY+ self.mergeframe_splitY* arg_stepY 
        self.mergeframe[begY:begY+ self.mergeframe_splitY, begX: begX+ self.mergeframe_splitX]= tmp_frame
        #begY= self.mergeframe_height- 50- self.mergeframe_splitY*arg_stepY
        #self.mergeframe[begY-self.mergeframe_splitY:begY, begX: begX+ self.mergeframe_splitX]= tmp_frame
        self.mergeframe_stepX= arg_stepX
        self.mergeframe_stepY= arg_stepY
        print '>> mergeframe_splitY, splitX= ', self.mergeframe_splitY, ', ', self.mergeframe_splitX
        print '>> tmp_frame.shape[0,1]= ', tmp_frame.shape[0],', ',tmp_frame.shape[1]
        
        result = Image.fromarray(self.mergeframe)
        result = ImageTk.PhotoImage(result)
        self.panel_mergeframe.configure(image = result)
        self.panel_mergeframe.image = result

    def rdbtn_MvAmount_click(self, event= None):
        if event is not None:
            if event.keysym == 'F1':
                self.rdbtn_MvAmount_1.select()
            elif event.keysym == 'F2':
                self.rdbtn_MvAmount_5.select()
            elif event.keysym == 'F3':
                self.rdbtn_MvAmount_10.select()
            elif event.keysym == 'F4':
                self.rdbtn_MvAmount_50.select()
            elif event.keysym == 'F5':
                self.rdbtn_MvAmount_100.select()
        self.Move_interval= self.MvAmount.get()
        print 'rdVal',self.Move_interval

    def btn_MoveAmount_click(self, event= None):
        #print '*** ',self.tabbox.index(self.tabbox.select())
        #print '*** ',self.tabbox.select()
        if self.tabbox.index(self.tabbox.select())==0:
            if type(event) is types.StringType:
                move_type= event 
            else:
                print'event.keysym ', event.keysym
                print 'event.keycode', event.keycode
                move_type= event.keysym
                print 'Test ',move_type is 'Up'
            #self.Move_interval= self.MvAmount.get()
            tmp_x, tmp_y, tmp_z= self.ArdMntr.get_CurPosition()
            print '==>>> ',tmp_x, tmp_y, tmp_z
            print '==>>> ',self.Move_interval*self.Move_intervalUnit
            if move_type == 'Up':
                self.ArdMntr.move_Coord(tmp_x+ self.Move_interval*self.Move_intervalUnit, tmp_y, tmp_z)
            elif move_type == 'Down':
                self.ArdMntr.move_Coord(tmp_x- self.Move_interval*self.Move_intervalUnit, tmp_y, tmp_z)
            elif move_type == 'Left':
                self.ArdMntr.move_Coord(tmp_x, tmp_y-self.Move_interval*self.Move_intervalUnit, tmp_z)
            elif move_type == 'Right':
                self.ArdMntr.move_Coord(tmp_x, tmp_y+self.Move_interval*self.Move_intervalUnit, tmp_z)

    def btn_MoveAmountZaxis_click(self, event= None):
        if self.tabbox.index(self.tabbox.select())==0:
            if type(event) is types.StringType:
                move_type= event 
            else:
                move_type= event.keysym
            
            tmp_x, tmp_y, tmp_z= self.ArdMntr.get_CurPosition()
            if move_type == 'Up':
                self.ArdMntr.move_Coord(tmp_x, tmp_y, tmp_z+ self.Move_interval*self.Move_intervalUnit)
            elif move_type == 'Down':
                self.ArdMntr.move_Coord(tmp_x, tmp_y, tmp_z- self.Move_interval*self.Move_intervalUnit)

    def btn_Seed_click(self):
        if self.ArdMntr.connect:
            self.ArdMntr.switch_Seed(self.pinNumb_seed, not(self.ArdMntr.SeedOn))
            print 'Seeding... '
    def btn_Water_click(self):
        if self.ArdMntr.connect:
            self.ArdMntr.switch_Water(self.pinNumb_water,not(self.ArdMntr.WaterOn) , -1)
            print 'Watering... '
    def btn_Light_click(self):
        if self.ArdMntr.connect:
            self.ArdMntr.switch_Light(self.pinNumb_fan, not(self.ArdMntr.LightOn))
            print 'Lighting... '
            pass 

    def btn_choosescript_click(self):
        str_scriptPath = tkFileDialog.askopenfilename(title = "Select file",filetypes = (("all files","*.*"),("Text File", "*.txt"),("jpeg files","*.jpg")))
        print '>>>> ', str_scriptPath
        if str_scriptPath !="":
            self.entry_scriptPath.delete(0,"end")
            self.entry_scriptPath.insert(Tkinter.END, str_scriptPath)
            self.scriptPath= str_scriptPath

    def btn_loadscript_click(self):
        #self.scriptPath= self.entry_scriptPath.get()
        tmpPath= self.entry_scriptPath.get()
        if utils_tool.check_file(tmpPath):
            #self.txtbox_script.delete('1.0', END)
            self.txtbox_script.clear()
            self.txtbox_script.importfile(tmpPath)
            self.txtbox_script.configure(label_text= "- "+ tmpPath.split("/")[-1]+" -")
        else:
            tkMessageBox.showerror("Error", "'%s' dost not exist !" % tmpPath)
        '''
        cmd_file = open(self.scriptPath, "r")
        lines = cmd_file.readlines()
        for line in lines:
            cmd = line.strip()
            if len(cmd)>0:
                self.txtbox_script.insert(END, cmd+'\n')
        cmd_file.close()
        '''
        
    def btn_savescript_click(self):
        tmpPath= self.entry_scriptPath.get()
        self.txtbox_script.exportfile(tmpPath)

    

    def btn_runscript_click(self):
        if self.ArdMntr.connect:
            if self.StartRunScript_judge:
                #===================================
                # Delete Scanning Thread
                #===================================
                self.StartRunScript_judge= False
                del(self.thread_runningScript)
                '''
                self.tabbox.tab(self.tab_control, state='normal')
                self.tabbox.tab(self.tab_imageprocess, state='normal')
                self.Lock_tabloadscript(False)
                self.btn_runscript.config(text= 'RUN', fg='white', activeforeground= 'white', bg= self.bgGreen,activebackground= self.bgGreen_active)
                self.StartRunScript_judge= False
                '''
            else:
                '''
                content= self.txtbox_script.get("1.0", "end-1c")
                test= self.txtbox_script.getvalue()
                print 'type test:', type(test)
                with open('tmp.txt', "w") as out:
                    out.write(content)
                '''
                self.txtbox_script.exportfile("tmp.txt")
                #=================================
                # New Thread of Scanning process
                #================================
                self.thread_runningScript= threading.Thread(target= self.runningScript_run)
                self.thread_runningScript.start()

                self.tabbox.tab(self.tab_control, state='disable')
                self.tabbox.tab(self.tab_imageprocess, state='disable')
                self.Lock_tabloadscript(True)
                self.btn_runscript.config(text= 'STOP', fg='white', activeforeground= 'white', bg= self.bgRed,activebackground= self.bgRed_active)
                self.StartRunScript_judge= True
        else:
            tkMessageBox.showerror("Error", "Arduino connection refused!")


    def btn_StartScan_click(self):
        self.imageProcessor.set_threshold_size(int(self.scale_threshold_MinSize.get()))
        self.imageProcessor.set_threshold_graylevel(int(self.scale_threshold_graylevel.get()))
        self.input_Zpos= int(self.entry_Zpos.get())
        self.readmergeframeIndex= gui_vars.scanIndex 
        print 'Start'
        if self.StartScan_judge:
            #===================================
            # Delete Scanning Thread
            #===================================
            self.StartScan_judge= False
            del(self.thread_scanning)
            '''
            self.Lock_tabcontrol(False)
            self.Lock_Menubar(False)
            self.tabbox.tab(self.tab_loadscript, state='normal')
            self.tabbox.tab(self.tab_imageprocess, state='normal')
            self.btn_StartScan.config(text= 'Start Scan', fg='white', activeforeground= 'white', bg= self.bgGreen,activebackground= self.bgGreen_active)
            '''
        else:
            if self.ArdMntr.connect:
                try:
                    self.reset_mergeframe()
                    self.scan_X= [int(self.entry_1stXpos.get()), int(self.entry_ScanInterval_X.get()), int(self.entry_ScanAmount_X.get())]
                    self.scan_Y= [int(self.entry_1stYpos.get()), int(self.entry_ScanInterval_Y.get()), int(self.entry_ScanAmount_Y.get())]
                    self.set_mergeframe_size(self.scan_X[2], self.scan_Y[2])
                    self.reset_mergeframe()
                    #print '### ', self.scan_X, self.scan_Y
                
                    self.ArdMntr.move_Coord(self.scan_X[0], self.scan_Y[0], self.input_Zpos)
                    if self.scan_X[0]+self.scan_X[1]*self.scan_X[2]<self.limit[0] | self.scan_Y[0]+self.scan_Y[1]*self.scan_Y[2]<self.limit[1]:
                        self.StartScan_judge= True
                        #self.saveTimeIndex= datetime.now().strftime("%Y%m%d%H%M%S")
                        self.saveTimeIndex= datetime.now().strftime('%Y%m%d%H%M%S')
                        #=================================
                        # New Thread of Scanning process
                        #================================
                        self.thread_scanning= threading.Thread(target= self.scanning_run)
                        self.thread_scanning.start()
                        print '*** scanning...'
                    	self.Lock_tabcontrol(True)
                        self.Lock_Menubar(True)
                        self.tabbox.tab(self.tab_loadscript, state='disable')
                        self.tabbox.tab(self.tab_imageprocess, state='disable')
                    	self.btn_StartScan.config(text= 'STOP Scan', fg='white', activeforeground= 'white', bg= self.bgRed, activebackground= self.bgRed_active)
                    else:
                        tkMessageBox.showerror("Error", "The scanning of X should be in [0~{0}]\nThe range of Y should be in [0~{1}]".format(self.limit[0],self.limit[1]))
                except:
                    tkMessageBox.showerror('Error', 'Please enter nubmer')
            else:
                tkMessageBox.showerror("Error", "Arduino connection refused!")


    def btn_saveImg_click(self):
        #self.saveImg= True
        self.imagename= 'Frame1'
        self.singleframe = self.CamMntr.get_frame()
        self.saveImg_function(self.singleframe, gui_vars.savePath, self.imagename)
        self.display_panel_singleframe(self.singleframe)
    
    def btn_loadImg_click(self):
        str_imagePath = tkFileDialog.askopenfilename(title = "Select image",filetypes = (("jpeg files","*.jpg"), ("png files","*.png"), ("tif files","*.tif"),("all files","*.*")))
        print '>>>> ', str_imagePath
        if str_imagePath !="":
            img= utils_tool.readImage(str_imagePath)
            if img is not False:
                self.singleframe= img.copy()
                self.display_panel_singleframe(self.singleframe)
            else:
                tkMessageBox.showerror('Image does not exist', 'The image\n{0}\n does not exist. Please check the path again')

    def btn_MoveTo_click(self):
        if self.ArdMntr.connect:
            try:
                Target_X= int(self.entry_Xpos.get())
                Target_Y= int(self.entry_Ypos.get())
                Target_Z= int(self.entry_Zpos.get())
                if (Target_X>=0) & (Target_X<=self.limit[0]) & (Target_Y>=0) & (Target_Y<=self.limit[1]):
                    cmd= 'G00 X{0} Y{1} Z{2}'.format(Target_X, Target_Y, Target_Z)
                    #self.ArdMntr.serial_send(cmd)
                    print 'ArdMntr.move_Coord...'
                    self.ArdMntr.move_Coord(Target_X, Target_Y, Target_Z)
                    print 'Command: ',cmd
                    time.sleep(1)                
                else:
                    tkMessageBox.showerror("Error", "The range of X should be in [0~{0}]\nThe range of Y should be in [0~{1}]".format(self.limit[0],self.limit[1]))
            
            except:
                tkMessageBox.showerror("Error", "Please enter number!")
        else:
            tkMessageBox.showerror("Error", "Arduino connection refused!")

    def mark_cross_line(self , frame):
        w = frame.shape[0] / 2
        h = frame.shape[1] / 2
        cv2.line(frame , (h - 15 , w) , (h + 15 , w) , (255 , 0 , 0) , 1)
        cv2.line(frame , (h , w - 15) , (h , w + 15) , (255 , 0 , 0) , 1)
        return frame

    def saveImg_function(self, arg_frame,arg_savePath, arg_filename):
        utils_tool.check_path(arg_savePath)
        # make sure output dir exists
        #if(not path.isdir(arg_savePath)):
        #    makedirs(arg_savePath)
        #tmp= cv2.cvtColor(arg_frame, cv2.COLOR_RGB2BGR)
        cv2.imwrite(arg_savePath+arg_filename+'.jpg',arg_frame)
    
    def runningScript_run(self):
        cmd_file = open('tmp.txt', "r")
        lines = cmd_file.readlines()
        for line in lines:
            cols = line.split("#")
            print '***', self.StartRunScript_judge,line
            print("line=%s,cols_count=%i" %(line,len(cols)))
            if len(cols)>=1:
                cmd = cols[0]
                cmd = cmd.strip()
                if len(cmd)>0:
                    print(">> "+cmd)
                    cmd_code= cmd.strip().split(' ')[0].replace(' ','')
                    if cmd_code[0]== 'C':
                        if cmd_code[1:]== '00':
                            TimeIndex= datetime.now().strftime('%Y%m%d%H%M%S')
                            tmp_x, tmp_y, tmp_z= self.ArdMntr.get_CurPosition()
                            imgName= '{0}_{1}_{2}_{3}'.format(TimeIndex, tmp_x, tmp_y, tmp_z)
                            self.singleframe= self.CamMntr.get_frame()
                            self.saveImg_function(self.singleframe, gui_vars.savePath, imgName)
                            self.display_panel_singleframe(self.singleframe)
                                    
                    else:
                        while 1:
                            if self.ArdMntr.cmd_state.is_ready(): #wait system ready to accept commands
                                self.ArdMntr.serial_send("%s" %cmd)
                                time.sleep(1)
                                break
                            else:
                                time.sleep(1)
            time.sleep(1)
            if self.StartRunScript_judge== False:
                break
        cmd_file.close()
        print 'CLOSE FILE...'
        self.tabbox.tab(self.tab_control, state='normal')
        self.tabbox.tab(self.tab_imageprocess, state='normal')
        self.Lock_tabloadscript(False)
        self.btn_runscript.config(text= 'RUN', fg='white', activeforeground= 'white', bg= self.bgGreen,activebackground= self.bgGreen_active)
        self.StartRunScript_judge= False


    def scanning_run(self):
        step=0
        #while self.scanning_judge:
        if self.StartScan_judge:
            print '>>> Scanning...'
            for step_X in range(0, self.scan_X[2]):
                for step_Y in range(0, self.scan_Y[2]):
                    if self.StartScan_judge== False:
                        break
                    if step_X % 2 ==0:
                        tmp_step_Y= step_Y
                    else:
                        tmp_step_Y= self.scan_Y[2]- step_Y-1
                    tmp_X, tmp_Y= self.scan_X[0]+ step_X*self.scan_X[1], self.scan_Y[0]+ tmp_step_Y*self.scan_Y[1]
                    #tmp_X, tmp_Y= self.scan_X[0]+ step_X*self.scan_X[1], self.scan_Y[0]+ step_Y*self.scan_Y[1]
                    print '>> X, Y: ', tmp_X, ', ', tmp_Y
                    #self.saveScanning= 'Raw_{0}_{1}.png'.format(self.scan_X[0]+ step_X*self.scan_X[1], self.scan_Y[0]+ step_Y*self.scan_Y[1])
                    self.ArdMntr.move_Coord(tmp_X, tmp_Y, self.input_Zpos)
                    time.sleep(1)
                    while 1:
                        if (self.ArdMntr.cmd_state.is_ready()):
                            time.sleep(0.5)
                            #self.saveScanning= '{0}_'.format(step)+self.ArdMntr.cmd_state.strCurX+'_'+self.ArdMntr.cmd_state.strCurY
                            #self.saveScanning= self.ArdMntr.cmd_state.strCurX+'_'+self.ArdMntr.cmd_state.strCurY
                            self.saveScanning= '{0}_{1}'.format(tmp_X, tmp_Y)
                            frame= self.CamMntr.get_frame()
                            self.saveImg_function(frame, gui_vars.saveScanningPath,self.readmergeframeIndex+'_'+self.saveTimeIndex+'_'+self.saveScanning)
                            result= frame.copy()
                            self.display_panel_singleframe(result)
                            #self.display_panel_mergeframe(result, step_X, step_Y)
                            #self.display_panel_mergeframe(result, step_Y, step_X)
                            self.display_panel_mergeframe(result, tmp_step_Y, step_X)
                            
                            print self.saveScanning
                            #time.sleep(2)
                            break
                        else:
                            time.sleep(1)
                    if self.StartScan_judge== False:
                        break
                    step= step+1
            self.StartScan_judge= False
            self.Lock_tabcontrol(False)
            self.Lock_Menubar(False)
            self.tabbox.tab(self.tab_loadscript, state='normal')
            self.tabbox.tab(self.tab_imageprocess, state='normal')
            self.btn_StartScan.config(text= 'Start Scan', fg='white', activeforeground='white', bg= self.bgGreen, activebackground= self.bgGreen_active)
        else:
            time.sleep(0.2)
            step=0      


    def check_frame_update(self):
        result = Image.fromarray(self.frame)
        result = ImageTk.PhotoImage(result)
        self.panel.configure(image = result)
        self.panel.image = result
        self.panel.after(8, self.check_frame_update)

    def main_run(self):
        frame= self.CamMntr.get_frame()
        if frame is not -1:
            frame= cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame = self.mark_cross_line(frame)
            frame= cv2.resize(frame,(self.frame_width,self.frame_height),interpolation=cv2.INTER_LINEAR)
            text='Arduino Connection Refused ...'
            text_water=''
            text_seed=''
            color= (0,0,0)
            if self.ArdMntr.connect== True:
                if self.StartScan_judge == False:
                    if self.ArdMntr.cmd_state.is_ready() :
                        text= 'Idling ...'
                        color = (0 , 255 , 0)
                    else:
                        text= 'Moving ...'
                        color = (255,0,0)
                else:
                    if self.ArdMntr.cmd_state.is_ready():
                        text= 'Processing...'
                        color = (0 , 255 , 0)
                    else:
                        text= 'Scanning...'+'(X, Y)= ('+self.ArdMntr.cmd_state.strCurX+', '+self.ArdMntr.cmd_state.strCurY+')'
                        color = (255,0,0)
                if self.ArdMntr.WaterOn:
                    text_water= 'Water: On  '
                    cv2.putText(frame, text_water,(10,70),cv2.FONT_HERSHEY_SIMPLEX, 0.7,(255,0,0),1)
                if self.ArdMntr.SeedOn:
                    text_seed= 'Vaccum: On  '
                    cv2.putText(frame, text_seed,(10,100),cv2.FONT_HERSHEY_SIMPLEX, 0.7,(255,0,0),1)
            cv2.putText(frame, text,(10,40),cv2.FONT_HERSHEY_SIMPLEX, 0.7,color,1)
            self.strStatus= text+ ' ; '+ text_water+ text_seed
            self.set_frame(frame)
        time.sleep(0.01)

root = Tkinter.Tk()
root.title("[FBTUG] offline Farmbot GUI for development")
root.attributes('-zoomed', True) # FullScreen
app= App(root)
root.mainloop()
