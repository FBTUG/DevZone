import cv2
import numpy as np
import imutils

class SPROUT:
    def __init__(self, reSize=(1000,563), vThresh1=210, vThresh2=120, vErode=2, vDilate=4, debug=False):
        self.resize = reSize
        self.thresh1 = vThresh1
        self.thresh2 = vThresh2
        self.erode = vErode
        self.dilate = vDilate
        self.indexNum = 0
        self.debug = debug
        self.tft = True
        self.lcd = True

    def countSprout(self, image, minSize=25, maxSize=450):
        self.indexNum += 1
        numSprouts = 0

        image = cv2.resize(image, self.resize, interpolation = cv2.INTER_AREA)
        if(self.debug==True and self.indexNum==2):   
            cv2.imshow("Original #" + str(self.indexNum) , image)
            cv2.imwrite("original.png", image)

        gray1 = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        (T, gray1) = cv2.threshold(gray1, self.thresh1, 255, cv2.THRESH_BINARY)
        gray1 = cv2.erode(gray1, None, iterations=self.erode)
        gray1 = cv2.dilate(gray1, None, iterations=self.dilate)

        lab = cv2.cvtColor(image, cv2.COLOR_BGR2LAB)
        (L, A, gray2) = cv2.split(lab)
        (T, gray2) = cv2.threshold(gray2, self.thresh2, 255, cv2.THRESH_BINARY)
        gray2 = cv2.erode(gray2, None, iterations=self.erode)
        gray2 = cv2.dilate(gray2, None, iterations=self.dilate)

        gray = cv2.bitwise_or(gray1, gray2)

        if(self.debug==True and self.indexNum==2):
            cv2.imshow("GRAY1", gray1)
            cv2.imshow("GRAY2", gray2)
            cv2.imshow("Final GRAY", gray)

        (_, cnts, _) = cv2.findContours(gray, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        for (i, c) in enumerate(cnts):
            area = cv2.contourArea(c)
            (x, y, w, h) = cv2.boundingRect(c)
            hull = cv2.convexHull(c)
            hullArea = cv2.contourArea(hull)
            solidity = (area / float(hullArea)) if (hullArea>0) else 0

            if(self.debug==True and self.indexNum==2):
                print ("area:{} , hullArea:{}, solidity:{}".format(area,hullArea,solidity))

            if((area>=minSize and area<=maxSize)):
            #if((solidity>=0.5 and solidity<=1)):
                numSprouts += 1
                ((x, y), radius) = cv2.minEnclosingCircle(c)
                cv2.drawContours(image, [c], -1, (0, 255, 0), 2)

        #if((self.debug==True or self.tft==True or self.lcd==True) and self.indexNum==2):
        if(self.indexNum==2):
            font = cv2.FONT_HERSHEY_COMPLEX_SMALL
            cv2.putText(image, "Sprout count: " + str(numSprouts), (image.shape[1]-500, 40), font, 2, (255, 1, 126), 3)
            #cv2.imwrite("detectSprout.png", image)
            #cv2.imshow("SPROUTS #" + str(self.indexNum) , image)

            #if(self.tft==True):
            #    image = cv2.resize(image, (240, 320))
            #    image = imutils.rotate(image, 90)

        return image

class PLANTSAREA:
    def __init__(self, imgPath, reSize=(250,250)):
        image = cv2.imread(imgPath)
        self.image = cv2.resize(image, reSize, interpolation = cv2.INTER_AREA)
        self.colorSpace = cv2.COLOR_BGR2LAB

    def extractPlantsArea(self, b_threshold=80, a_threshold=80):
        zeros = np.zeros(self.image.shape[:2], dtype = "uint8")

        imgLAB = cv2.cvtColor(self.image, self.colorSpace)
        (L, A, B) = cv2.split(imgLAB)

        (T_weeds_b, thresh_weeds_b) = cv2.threshold(B, b_threshold, 255, cv2.THRESH_BINARY)
        (T_weeds_a, thresh_weeds_a) = cv2.threshold(A, a_threshold, 255, cv2.THRESH_BINARY)
        imgRGB = cv2.merge([zeros, thresh_weeds_b, thresh_weeds_a])
        return imgRGB

    def countPlantsArea(self, image):
        width = image.shape[1]
        height = image.shape[0]
        gArea = 0.0
        rArea = 0.0
        grArea = 0.0

        for pixel_w in range(0, width, 1):
            for pixel_h in range(0, height, 1):
                (b, g, r) = image[pixel_h, pixel_w]
                if(g>0):
                    gArea += 1     
                if(r>0):
                    rArea += 1
                if(r>0 and g>0):
                    grArea += 1

        totalArea = width*height
        return (gArea/totalArea, rArea/totalArea, grArea/totalArea)
