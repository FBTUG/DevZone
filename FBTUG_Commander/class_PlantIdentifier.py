import cv2
import numpy as np
import imgProcess_tool

class PlantIdentifier:
    def __init__(self, imgPath=False, reSize=(250,250)):
        #self.image = cv2.imread(imgPath)
        #self.image = cv2.resize(image, reSize, interpolation = cv2.INTER_AREA)
        #self.image_raw = self.image.copy()
        self.image= np.zeros((reSize[0], reSize[1],3),np.uint8)
        if imgPath:
            self.loadimage(imgPath)
        self.colorSpace = cv2.COLOR_BGR2LAB

    def setimage(self, arg_img):
        self.image= arg_img.copy()
        self.image_raw= self.image.copy()
        (self.imgRGB_B, self.imgRGB_G, self.imgRGB_R)= cv2.split(self.image.copy())

    def loadimage(self, arg_imgPath):
        self.image = cv2.imread(arg_imgPath)
        self.image_raw= self.image.copy()
        (self.imgRGB_B, self.imgRGB_G, self.imgRGB_R)= cv2.split(self.image.copy())

    def NDIimage(self, arg_debug= False):
        G= self.imgRGB_G.astype('float')
        R= self.imgRGB_R.astype('float')
        NDIimage= 128*((G-R)/(G+R)+1)
        NDIimage= cv2.normalize(NDIimage, NDIimage, 0, 255, cv2.NORM_MINMAX)
        if arg_debug:
            cv2.imwrite('Debug/debug_NDIimage.jpg', NDIimage)
        return NDIimage 

    def ExGimage(self, arg_debug= False):
        print 'ExGimage'
        R_star= self.imgRGB_R.astype('float')/255
        G_star= self.imgRGB_G.astype('float')/255
        B_star= self.imgRGB_B.astype('float')/255
        ExGimage= (2*G_star-R_star- B_star)/(G_star+ B_star+ R_star)
        ExGimage= cv2.normalize(ExGimage, ExGimage, 0, 255, cv2.NORM_MINMAX)
        if arg_debug:
            cv2.imwrite('Debug/debug_ExGimage.jpg', ExGimage)
        #print ExGimage 
        return ExGimage


    def LABimage(self, arg_debug):
        imgLAB = cv2.cvtColor(self.image, self.colorSpace)
        (L, A, B) = cv2.split(imgLAB)
        return L, A, B

    def extractPlantsArea(self, arg_mode=0,arg_INV= False, b_threshold=80, a_threshold=80):
        zeros = np.zeros(self.image.shape[:2], dtype = "uint8")

        imgLAB = cv2.cvtColor(self.image, self.colorSpace)
        (L, A, B) = cv2.split(imgLAB)
        cv2.imwrite('Debug/imgB.jpg',B)
        cv2.imwrite('Debug/imgA.jpg',A)
        #(T_weeds_b, thresh_weeds_b) = cv2.threshold(B, b_threshold, 255, cv2.THRESH_BINARY)
        #(T_weeds_a, thresh_weeds_a) = cv2.threshold(A, a_threshold, 255, cv2.THRESH_BINARY)
        if arg_mode==0:
            thresh_weeds_a= imgProcess_tool.binarialization(A,0,arg_INV, a_threshold)
            thresh_weeds_b= imgProcess_tool.binarialization(B,0,arg_INV, b_threshold)
        elif arg_mode==1:
            thresh_weeds_b= imgProcess_tool.binarialization(B, 1, arg_INV)
            thresh_weeds_a= imgProcess_tool.binarialization(A, 1, arg_INV)
        elif arg_mode==2:
            thresh_weeds_b= imgProcess_tool.binarialization(B, 2, arg_INV)
            thresh_weeds_a= imgProcess_tool.binarialization(A, 2, arg_INV)
        cv2.imwrite('Debug/imgB_thr.jpg',thresh_weeds_b)
        cv2.imwrite('Debug/imgA_thr.jpg',thresh_weeds_a)
        imgRGB = cv2.merge([zeros, thresh_weeds_b, thresh_weeds_a])
        return thresh_weeds_a, thresh_weeds_b 

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
