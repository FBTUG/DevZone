import picamera
from time import *
camera = picamera.PiCamera()
#camera setting
camera.hflip = True
camera.vflip = True
camera.sharpness = 0
camera.contrast = 0
camera.brightness = 50
camera.saturation = 0
camera.ISO = 0
camera.video_stabilization = False
camera.exposure_compensation = 0
camera.exposure_mode = 'auto'
camera.meter_mode = 'average'
camera.awb_mode = 'auto'
camera.image_effect = 'none'
camera.color_effects = None
camera.rotation = 0
camera.crop = (0.0, 0.0, 1.0, 1.0)
camera.start_preview()
sleep(5)
camera.stop_preview()
camera.capture('image.jpg')
camera.start_recording('video.h264')
sleep(5)
camera.stop_recording()
