#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import imutils
import numpy as np
import cv2
import os, sys, traceback
import RPi.GPIO as GPIO
from lib.lcd import ILI9341
import time
import string
from plant_detection.PlantDetection import PlantDetection
import plantcv as pcv
import cv2

picturePath = "takePics/"
lcd = ILI9341(LCD_size_w=320, LCD_size_h=240, LCD_Rotate=0)

# initialize the camera
cam = cv2.VideoCapture(0)
cam.set(3,1280)
cam.set(4,1024)

shutterPin = 2
ledShutterPin = 3
btn1 = 4
btn2 = 5
btn3 = 6
GPIO.setmode(GPIO.BCM)

# GPIO setup
GPIO.setup(shutterPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)#Button to GPIO2
GPIO.setup(ledShutterPin, GPIO.OUT)  #LED
GPIO.setup(btn1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(btn2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(btn3, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def createMask(grayImg, th=104):
    masked = cv2.inRange(grayImg, th, 255)
    return masked

def getRedArea(image, thG=75, thY=132, thR=158):
    #thR =255 - thR
    cspace = cv2.cvtColor(image, cv2.COLOR_BGR2LAB)
    #channels = cv2.split(cspace)
    #cv2.imshow("Red Channel", channels[2])
    mask = createMask(cspace[:,:,2], thR)   # NDVI red area
    points = np.count_nonzero(mask)
    ratio = points / (image.shape[1] * image.shape[2])
    print("Red area: {} pixls, ratio: {}%".format(points, ratio ))
    
    return mask

def getGreenArea(image, thG=75, thY=132, thR=158):
    cspace = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    mask = createMask(cspace[:,:,1], thG)  #NDVI green area
    points = np.count_nonzero(mask)
    ratio = points / (image.shape[1] * image.shape[2])
    print("Green area: {} pixls, ratio: {}%".format(points, ratio ))

    return mask

def getYellowArea(image, thG=75, thY=132, thR=158 ):
    thY =255 - thY
    cspace = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    mask = createMask(cspace[:,:,1], thY)
    mask = (255-mask)
    points = np.count_nonzero(mask)
    ratio = points / (image.shape[1] * image.shape[2])
    print("Yellow+Red area: {} pixls, ratio: {}%".format(points, ratio ))

    return mask

def contrast_stretch(im):
    """
    Performs a simple contrast stretch of the given image, from 5-95%.
    """
    in_min = np.percentile(im, 5)
    in_max = np.percentile(im, 95)

    out_min = 0.0
    out_max = 255.0

    out = im - in_min
    out *= ((out_min - out_max) / (in_min - in_max))
    out += in_min

    return out

def ndvi1(image):
    # use Standard NDVI method, smaller for larger area
    thRED1 = 210
    thYELLOW1 = 112
    thGREEN1 = 0

    b, g, r = cv2.split(image)
    divisor = (r.astype(float) + b.astype(float))
    divisor[divisor == 0] = 0.01  # Make sure we don't divide by zero!

    ndvi = (b.astype(float) - r) / divisor

    #Paint the NDVI image
    ndvi2 = contrast_stretch(ndvi)
    ndvi2 = ndvi2.astype(np.uint8)

    redNDVI = cv2.inRange(ndvi2, thRED1, 255)
    yellowNDVI = cv2.inRange(ndvi2, thYELLOW1, thRED1)
    greenNDVI = cv2.inRange(ndvi2, thGREEN1, thYELLOW1)
    merged = cv2.merge([yellowNDVI, greenNDVI, redNDVI])

    print('\nMax NDVI: {m}'.format(m=ndvi.max()))
    print('Mean NDVI: {m}'.format(m=ndvi.mean()))
    print('Median NDVI: {m}'.format(m=np.median(ndvi)))
    print('Min NDVI: {m}'.format(m=ndvi.min()))

    return merged

def ndvi2(image):
    # use LAB channels, smaller for larger area
    thRED2 = 140
    thYELLOW2 = 175
    thGREEN2 = 75

    #create mask
    redArea = getRedArea(image=image, thR=thRED2)
    greenArea = getGreenArea(image=image, thG=thGREEN2)
    yellowArea = getYellowArea(image=image, thY=thYELLOW2, thR=thRED2)

    redImage = image.copy()
    redImage[redArea == 255] = [0, 0, 255]
    cv2.imwrite(picturePath + "NDVI-red.png", redImage)

    yellowImage = image.copy()
    yellowImage[yellowArea == 255] = [5, 255, 252]
    cv2.imwrite(picturePath + "NDVI-yellow.png", yellowImage)

    greenImage = image.copy()
    greenImage[greenArea == 255] = [0, 255, 0]
    cv2.imwrite(picturePath + "NDVI-green.png", greenImage)

def plantDetect(imagePath):
    PD = PlantDetection(image=imagePath, verbose=False, text_output=False)
    try:
        PD.detect_plants()
        print("python test.py -i " + picPath + ".jpg")
    except:
        GPIO.cleanup()
        pass

def platCV(imagePath):
    img, path, filename = pcv.readimage(imagePath)
    # Pipeline step
    device = 0
    debug = 'print'

    # Convert RGB to HSV and extract the Saturation channel
    device, s = pcv.rgb2gray_hsv(img, 's', device, debug)
    # Threshold the Saturation image
    device, s_thresh = pcv.binary_threshold(s, 85, 255, 'light', device, debug)
    # Median Filter
    device, s_mblur = pcv.median_blur(s_thresh, 5, device, debug)
    device, s_cnt = pcv.median_blur(s_thresh, 5, device, debug)
    # Convert RGB to LAB and extract the Blue channel
    device, b = pcv.rgb2gray_lab(img, 'b', device, debug)
    # Threshold the blue image
    device, b_thresh = pcv.binary_threshold(b, 130, 255, 'light', device, debug)
    device, b_cnt = pcv.binary_threshold(b, 130, 255, 'light', device, debug)
    # Fill small objects
    device, b_fill = pcv.fill(b_thresh, b_cnt, 10, device, debug)
    # Join the thresholded saturation and blue-yellow images
    #device, bs = pcv.logical_or(s_mblur, b_cnt, device, debug)
    # Apply Mask (for vis images, mask_color=white)
    device, masked = pcv.apply_mask(img, b_fill, 'white', device, debug)
    # Convert RGB to LAB and extract the Green-Magenta and Blue-Yellow channels
    #device, masked_a = pcv.rgb2gray_lab(masked, 'a', device, debug)
    #device, masked_b = pcv.rgb2gray_lab(masked, 'b', device, debug)
    # Threshold the green-magenta and blue images
    #device, maskeda_thresh = pcv.binary_threshold(masked_a, 100, 255, 'dark', device, debug)
    #device, maskeda_thresh1 = pcv.binary_threshold(masked_a, 135, 255, 'light', device, debug)
    #device, maskedb_thresh = pcv.binary_threshold(masked_b, 128, 255, 'light', device, debug)
    # Join the thresholded saturation and blue-yellow images (OR)
    #device, ab1 = pcv.logical_or(maskeda_thresh, maskedb_thresh, device, debug)
    #device, ab = pcv.logical_or(maskeda_thresh1, ab1, device, debug)
    #device, ab_cnt = pcv.logical_or(maskeda_thresh1, ab1, device, debug)
    # Fill small objects
    #device, ab_fill = pcv.fill(ab, ab_cnt, 200, device, debug)
    # Apply mask (for vis images, mask_color=white)
    #device, masked2 = pcv.apply_mask(masked, ab_fill, 'white', device, debug)
    # Identify objects
    #device, id_objects,obj_hierarchy = pcv.find_objects(masked2, ab_fill, device, debug)
    # Define ROI
    #device, roi1, roi_hierarchy= pcv.define_roi(masked2, 'rectangle', device, None, 'default', debug, True, 550, 0, -500, -1900)

try:
    while True:
        ret, source = cam.read()

        if ret:
            source = imutils.rotate_bound(source, 90)
            lcd.displayImg(cv2.cvtColor(source, cv2.COLOR_BGR2RGB))

            btnShutter_state = GPIO.input(shutterPin)
            btn1_state = GPIO.input(btn1)
            btn2_state = GPIO.input(btn2)
            btn3_state = GPIO.input(btn3)
            imgPreview_state = False

            if btnShutter_state == False:
                GPIO.output(ledShutterPin, True)
                print('Shutter button Pressed...')
                imgName = str(int(time.time()))
                picPath = picturePath+imgName
                cv2.imwrite(picPath+'.jpg', source)

                imgPreview_state = True
                btnShutter_state = True
                GPIO.output(ledShutterPin, False)
                time.sleep(1)
                while imgPreview_state == True:
                    btnShutter_state = GPIO.input(shutterPin)
                    btn1_state = GPIO.input(btn1)
                    btn2_state = GPIO.input(btn2)
                    btn3_state = GPIO.input(btn3)

                    if btnShutter_state==False:
                        GPIO.output(ledShutterPin, True)
                        imgPreview_state = False
                        btnShutter_state = True

                    if(btn1_state == False):
                        GPIO.output(ledShutterPin, True)
                        print("btn1 button Pressed...")
                        image = picPath+'.jpg'
                        platCV(image)
                        image = cv2.imread("9_wmasked.png")
                        lcd.displayImg(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
                        GPIO.output(ledShutterPin, False)

                    if(btn2_state == False):
                        GPIO.output(ledShutterPin, True)
                        print("btn2 button Pressed...")
                        ndvi2(source)
                        image = ndvi1(source)
                        lcd.displayImg(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
                        GPIO.output(ledShutterPin, False)

                    if(btn3_state == False):
                        print('btn3 button Pressed...')


                time.sleep(2)
            else:
                GPIO.output(ledShutterPin, False)

except Exception as ex:
    cam.release()
    print(ex)
    GPIO.cleanup()
