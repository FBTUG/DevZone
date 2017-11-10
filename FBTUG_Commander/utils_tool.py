import os
import cv2
import tkMessageBox

def check_path(arg_path, arg_mkdir= True):
    # make sure output dir exists
    if(not os.path.isdir(arg_path)):
        if arg_mkdir:
            os.makedirs(arg_path)
        return False
    else:
        return True

def check_file(arg_filepath):
    # check whether the file does exist
    if os.path.isfile(arg_filepath):
        return True
    else:
        return False

def getList_path(arg_path):
    if check_path(arg_path, False):
        return [name for name in os.listdir(arg_path)]
    else:
        return False

def readImage(arg_path):
    if check_file(arg_path):
        return cv2.imread(arg_path)
    else:
        tkMessageBox.showerror("Error", arg_path+'\n is NOT found')
        return False
