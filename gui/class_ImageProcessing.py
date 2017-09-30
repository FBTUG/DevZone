# -*- coding: utf-8 -*-

import cv2
import numpy as np
import Tkinter as tk
import tkFont
import json
from os import listdir, path, makedirs, remove
from PIL import Image
from PIL import ImageTk

class contour_detect():
    def __init__(self, arg_savePath, arg_saveParaPath):
        self.savePath= arg_savePath
        self.backgroundName= 'background.jpg'
        self.saveParaPath= arg_saveParaPath
        self.threshold_graylevel=128
        self.threshold_size= 20

    def set_threshold_graylevel(self, arg_threshold):
        self.threshold_graylevel= arg_threshold

    def set_threshold_size(self, arg_threshold):
        self.threshold_size= arg_threshold

    def store_all_para(self):
        data= dict()
        data["thrshd_gray"] = self.threshold_graylevel
        data["thrshd_size"] = self.threshold_size
        with open(self.saveParaPath+"Para.json" , 'w') as out:
            json.dump(data , out)
            print "Para set"


    def set_background(self, arg_frame):
        # make sure output dir exists
        if(not path.isdir(self.saveParaPath)):
            makedirs(self.saveParaPath)
        cv2.imwrite(self.saveParaPath+self.backgroundName,arg_frame)

    def check_background(self):
        if path.isfile(self.saveParaPath+self.backgroundName):
            return True
        else:
            return False

    def get_contour(self, arg_frame, arg_export_index, arg_export_path, arg_export_filename, arg_binaryMethod):
        # Otsu's thresholding after Gaussian filtering
        tmp = cv2.cvtColor(arg_frame, cv2.COLOR_RGB2GRAY)
        blur = cv2.GaussianBlur(tmp,(5,5),0)
        if arg_binaryMethod== 0:
            ret, thresholdedImg= cv2.threshold(blur.copy() , self.threshold_graylevel, 255 , 0)
        elif arg_binaryMethod == 1:
            ret,thresholdedImg = cv2.threshold(blur.copy(),0 ,255 ,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        elif arg_binaryMethod== 2:
            thresholdedImg = cv2.adaptiveThreshold(blur.copy(),255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,5,0)

        result = cv2.cvtColor(thresholdedImg, cv2.COLOR_GRAY2RGB)
        ctrs, hier = cv2.findContours(thresholdedImg, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        ctrs = filter(lambda x : cv2.contourArea(x) > self.threshold_size , ctrs)

        rects = [[cv2.boundingRect(ctr) , ctr] for ctr in ctrs]

        for rect , cntr in rects:
            cv2.drawContours(result, [cntr], 0, (0, 128, 255), 3)
        if arg_export_index:
            cv2.imwrite(arg_export_path+ arg_export_filename+'.jpg', result)
        print "Get Contour success"
        return result
