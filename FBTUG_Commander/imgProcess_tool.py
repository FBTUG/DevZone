# -*- coding: utf-8 -*-
import math
import cv2
import numpy as np
import Tkinter as tk
import tkFont
import json
import sys 
from os import listdir, path, makedirs, remove
from PIL import Image
from PIL import ImageTk

def mark_cross_line( frame, arg_x, arg_y, arg_color, arg_linewidth):
    cv2.line(frame , (arg_x - 15 , arg_y) , (arg_x + 15 , arg_y) , arg_color , arg_linewidth)
    cv2.line(frame , (arg_x , arg_y - 15) , (arg_x , arg_y + 15) , arg_color , arg_linewidth)
    return frame

def angle_between(p1, p2):
    point = (p2[0] - p1[0] , p2[1] - p1[1])
    ang = np.arctan2(*point[::-1])
    return np.rad2deg(ang % (2 * np.pi))

def draw_XYcoord(arg_frame, arg_pt, arg_dirList):
    arg_x_axis_reverse= arg_dirList[0]
    arg_y_axis_reverse= arg_dirList[1]
    arg_xy_axis_swap= arg_dirList[2]

    frame= arg_frame.copy()
    hor_word = "X"
    ver_word = "Y"
    hor_color = (0, 255, 0)
    ver_color = (0, 0, 255)
    #hor_start, hor_stop = (60, 50), (150, 50)
    #ver_start, ver_stop = (50, 60), (50, 150)
    hor_start, hor_stop = (arg_pt[0]+10, arg_pt[1]), (arg_pt[0]+ 90, arg_pt[1]+ 0)
    ver_start, ver_stop = (arg_pt[0], arg_pt[1]+10), (arg_pt[0]+ 0, arg_pt[1]+ 90) 

    #print arg_xy_axis_swap
    if arg_x_axis_reverse:
        hor_start, hor_stop = hor_stop, hor_start
    if arg_y_axis_reverse:
        ver_start, ver_stop = ver_stop, ver_start
    if arg_xy_axis_swap:
        #(hor_word, hor_color, hor_start, hor_stop, ver_word, ver_color, ver_start, ver_stop) =\
        #(ver_word, ver_color, ver_start, ver_stop, hor_word, hor_color, hor_start, hor_stop)
        hor_word, hor_color, ver_word, ver_color =\
        ver_word, ver_color, hor_word, hor_color
    
    #print hor_word, hor_color, ver_word, ver_color
    cv2.arrowedLine(frame, hor_start, hor_stop, hor_color, 5, 8, 0, 0.2)
    cv2.arrowedLine(frame, ver_start, ver_stop, ver_color, 5, 8, 0, 0.2)
    cv2.putText(frame, hor_word, (arg_pt[0]+ 30, arg_pt[1]- 10) , cv2.FONT_HERSHEY_DUPLEX, 0.7, hor_color, 2)
    cv2.putText(frame, ver_word, (arg_pt[0]- 20, arg_pt[1]+ 50) , cv2.FONT_HERSHEY_DUPLEX, 0.7, ver_color, 2)
    return frame

def binarialization(arg_frame, arg_binaryMethod, arg_thresholdValue= 100):
    if len(arg_frame.shape)==3:
        tmp = cv2.cvtColor(arg_frame, cv2.COLOR_RGB2GRAY)
    else:
        tmp= arg_frame.copy()
    # Otsu's thresholding after Gaussian filtering
    blur = cv2.GaussianBlur(tmp,(5,5),0)
    if arg_binaryMethod== 0:
	ret, thresholdedImg= cv2.threshold(blur.copy() , arg_thresholdValue, 255 , 0)
    elif arg_binaryMethod == 1:
	ret,thresholdedImg = cv2.threshold(blur.copy(),0 ,255 ,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    elif arg_binaryMethod== 2:
	thresholdedImg = cv2.adaptiveThreshold(blur.copy(),255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,5,0)
    return thresholdedImg

def resize_frame(arg_frame, arg_canvas_size):
    canvas= np.zeros((arg_canvas_size[0], arg_canvas_size[1],3),np.uint8)
    
    s= arg_frame.shape
    h, w= s[0], s[1]
    ratio_frame= float(h)/w
    ratio_canvase= arg_canvas_size[0]/ arg_canvas_size[1]
    #'''
    if ratio_frame >ratio_canvase:
	new_width, new_height= int(arg_canvas_size[0]/ ratio_frame), int(arg_canvas_size[0])
    else:
	new_width, new_height= int(arg_canvas_size[1]), int(arg_canvas_size[1]* ratio_frame)
	
    new_frame= cv2.resize(arg_frame,(new_width, new_height),interpolation=cv2.INTER_LINEAR)
    #print new_frame.shape 
    h_beg= int((arg_canvas_size[0]- new_height)/2)
    w_beg= int((arg_canvas_size[1]- new_width)/2)
    canvas[h_beg: h_beg+ new_height, w_beg:w_beg+ new_width]= new_frame
    #'''
    return canvas

def mapCoord_image(arg_img, arg_infocoord):
    img= arg_img.copy()
    if arg_infocoord[0] is True:
        img= mirror_image(img, 1)
    if arg_infocoord[1] is True:
        img= mirror_image(img, 0)
    if arg_infocoord[2] is True:
        img= swap_image(img)
    return img
    

def mirror_image(arg_img, arg_coord=1):
    '''
    arg_coord= 1 : MirrorAxis= Y-Axis
             = 0 :             X-Axis
             =-1 :             X + Y Axis
    '''
    img= arg_img.copy()
    return cv2.flip(img, arg_coord)

def swap_image(arg_img):
    img= arg_img.copy()
    img= rotate_image(img, 90)
    img= mirror_image(img, 1)
    return img

def rotate_image(mat, angle):
    height, width = mat.shape[:2]
    image_center = (width / 2, height / 2)

    rotation_mat = cv2.getRotationMatrix2D(image_center, angle, 1)

    radians = math.radians(angle)
    sin = math.sin(radians)
    cos = math.cos(radians)
    bound_w = int((height * abs(sin)) + (width * abs(cos)))
    bound_h = int((height * abs(cos)) + (width * abs(sin)))

    rotation_mat[0, 2] += ((bound_w / 2) - image_center[0])
    rotation_mat[1, 2] += ((bound_h / 2) - image_center[1])

    rotated_mat = cv2.warpAffine(mat, rotation_mat, (bound_w, bound_h))
    return rotated_mat

def findContours(arg_img,arg_canvas, arg_MinMaxArea=False, arg_debug= False):
    image= arg_img.copy()
    #print image
    canvas= arg_canvas.copy()
    if len(image)==3:
        image = cv2.cvtColor(self.image, cv2.COLOR_GRAY2BGR)
    if sys.version_info.major == 2: 
        ctrs, hier = cv2.findContours(image.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    else:
        _, ctrs, hier = cv2.findContours(image.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    if arg_MinMaxArea is not False:
        ctrs = filter(lambda x : arg_MinMaxArea[1]> cv2.contourArea(x) > arg_MinMaxArea[0] , ctrs)
    
    print '>>> ', len(ctrs)
    for ctr in ctrs:
        print 'Area: ', cv2.contourArea(ctr)
        cv2.drawContours(canvas, [ctr], 0, (0, 128, 255), 3)
    if arg_debug:
        cv2.imwrite('Debug/debug_findContours.jpg',canvas)
    return canvas
