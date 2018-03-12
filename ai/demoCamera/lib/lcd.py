#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import datetime
import Adafruit_ILI9341 as TFT
import Adafruit_GPIO.SPI as SPI
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont, ImageOps
import numpy as np
DC = 18
RST = 23
SPI_PORT = 0
SPI_DEVICE = 0


class ILI9341:
    def __init__(self, LCD_size_w=240, LCD_size_h=320, LCD_Rotate=0):
        DC = 18
        RST = 23
        SPI_PORT = 0
        SPI_DEVICE = 0
        self.LCD_size_w = LCD_size_w
        self.LCD_size_h = LCD_size_h
        self.LCD_Rotate = LCD_Rotate
        disp = TFT.ILI9341(DC, rst=RST, spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE, max_speed_hz=64000000))
        self.disp = disp

        disp.begin()  # Initialize display.

    def displayImgFile(self, imagePath):
        image = Image.open(imagePath)
        image = Image.resize((self.LCD_size_w, self.LCD_size_h)).rotate(self.LCD_Rotate)
        #image = image.resize((self.LCD_size_w, self.LCD_size_h)).rotate(self.LCD_Rotate)
        self.disp.display(image)

    def displayImg(self, image):
        image = Image.fromarray(image)
        #image = image.rotate(180)
        image = image.resize((240, 320))
        #image = image.rotate(90)
        #image = image.transpose(Image.FLIP_LEFT_RIGHT)
        #image = image.resize((self.LCD_size_w, self.LCD_size_h))
        #image = image.rotate(self.LCD_Rotate).resize((self.LCD_size_w, self.LCD_size_h))
        #image = image.transpose(Image.FLIP_LEFT_RIGHT)
        self.disp.display(image)


    def displayClear(self):
        self.disp.clear((0, 0, 0))

    def printText(self, fontPath, fontSize=18, text="Hello world.", rotate=0, position=(10, 10), fontColor=(255, 255, 255), imagePath=""):
        # get an image
        base = Image.open(imagePath).convert('RGBA')

        # make a blank image for the text, initialized to transparent text color
        txt = Image.new('RGBA', base.size, (255,255,255,0))

        # get a font
        fnt = ImageFont.truetype(fontPath, fontSize)
        # get a drawing context
        d = ImageDraw.Draw(txt)

        # draw text, full opacity
        d.text(position, text, font=fnt, fill=(255,255,255,255))
        # draw text, half opacity
        d.text((10,60), "ppm", font=fnt, fill=(255,255,255,128))

        txt = txt.rotate(rotate)
        out = Image.alpha_composite(base, txt)
        self.disp.display(out)

    def drawLineChart(self, data, fontPath, bgpath):
        img = Image.open(bgpath).convert('RGBA')
        chart = Image.new('RGBA', img.size, (255,255,255,0))
        #img = img.rotate(90)
        draw_img = ImageDraw.Draw(chart)
        fnt_data = ImageFont.truetype(fontPath, 56)
        fnt_time = ImageFont.truetype(fontPath, 13)
 
        x = 0
        r = 5
        for i in data:
            x = x + 5
            y = 270 - int(i) 
            #draw_img.line((x, y, x,20), width=10, fill=(255,0,0,255))
            #draw_img.point((x, y), 'red')
            draw_img.ellipse((x, y, x+2, y+2), fill = 'blue', outline ='blue')

        draw_img.text((0,67), str(data[len(data)-1]), font=fnt_data, fill=(255,0,0,255))
        timenow = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        draw_img.text((0,40), timenow, font=fnt_time, fill=(0,0,0,128))

        chart = chart.rotate(90)
        chart = Image.alpha_composite(img, chart)
        self.disp.display(chart)


    def printPMdata(self, fontPath, pm10=(20,30), pm25=(24,32), pm100=(30,32), imagePath=""):
        # get an image
        base = Image.open(imagePath).convert('RGBA')
    
        # make a blank image for the text, initialized to transparent text color
        txt = Image.new('RGBA', base.size, (255,255,255,0))
        # get a font
        fnt_data = ImageFont.truetype(fontPath, 42)
        fnt_time = ImageFont.truetype(fontPath, 12)
        # get a drawing context
        d = ImageDraw.Draw(txt)

        posY = (132, 173, 220)
        posX = (15, 172)
        posTime = (60,46)
        # draw text, full opacity
        d.text((posX[0],posY[0]), str(pm10[0]), font=fnt_data, fill=(255,255,255,255))
        d.text((posX[0],posY[1]), str(pm25[0]), font=fnt_data, fill=(255,255,255,255))
        d.text((posX[0],posY[2]), str(pm100[0]), font=fnt_data, fill=(255,255,255,255))
        d.text((posX[1],posY[0]), str(pm10[1]), font=fnt_data, fill=(255,255,255,255))
        d.text((posX[1],posY[1]), str(pm25[1]), font=fnt_data, fill=(255,255,255,255))
        d.text((posX[1],posY[2]), str(pm100[1]), font=fnt_data, fill=(255,255,255,255))

        # draw text, half opacity
        #d.text((posX[0]+posPPM,posY[0]), "ppm", font=fnt_ppm, fill=(255,255,255,128))
        #d.text((posX[0]+posPPM,posY[1]), "ppm", font=fnt_ppm, fill=(255,255,255,128))
        #d.text((posX[0]+posPPM,posY[2]), "ppm", font=fnt_ppm, fill=(255,255,255,128))
        #d.text((posX[1]+posPPM,posY[0]), "ppm", font=fnt_ppm, fill=(255,255,255,128))
        #d.text((posX[1]+posPPM,posY[1]), "ppm", font=fnt_ppm, fill=(255,255,255,128))
        #d.text((posX[1]+posPPM,posY[2]), "ppm", font=fnt_ppm, fill=(255,255,255,128))

        timenow = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        d.text(posTime, timenow, font=fnt_time, fill=(248,250,181,128))

        txt = txt.rotate(90)
        out = Image.alpha_composite(base, txt)
        self.disp.display(out)

