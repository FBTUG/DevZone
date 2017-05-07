# extract video to frames, and run sprout
# Command format:            
#     'exe_cmd [ -h ] [ -v ] [ -d ] [ -t ] [-g]')
#       -v: video pathname
#       -d: output directory
#       -t: start time, ex: 20170507010203'
#       -g: frames gap time in seconds. default: 3600'
#output example:
#frames/image_20170508020203.jpg, -204
#frames/image_20170508030203.jpg, -189
import os
import getopt
import sys
import cv2
import datetime
from lib.libFarm import SPROUT # from https://github.com/ch-tseng/farmbot/blob/master/countSprout.py

# You can adjust the values here ---------------
createImage = True
reSize=(1000,563)
#-----------------------------------------------

if __name__ =='__main__':
    #ex: python test_video_to_frames.py -v video-1493972721.mp4 -t 20170304010203 -g 3600 -d images
    try:
        opts,args = getopt.getopt(sys.argv[1:], "hv:d:t:g:", [])
    except getopt.GetoptError:
        print ('opterro: test_video_to_frames.py [ -h ] [ -v ] [ -d ] [ -t ] [ -g ]')       
        sys.exit(2) 
        
    dir = 'images'
    video ="abc.mp4" #"video-1493972721.mp4"
    starttime_str = '20170507010203'
    starttime = datetime.datetime.strptime(starttime_str, '%Y%m%d%H%M%S')  
    seconds = 1*60*60 
    for opt, arg in opts:
        #print(arg)
        if opt == '-h':
            print ('exe_cmd [ -h ] [ -v ] [ -d ] [ -t ] [-g]')
            print ('    -v: video pathname')
            print ('    -d: output directory')
            print ('    -t: start time, ex: 20170507010203')
            print ('    -g: frames gap time in seconds. default: 3600')
            sys.exit()
        elif opt in ("-v"):
            video = arg  
        elif opt in ("-d"):
            dir = arg
        elif opt in ("-t"):
            starttime_str = arg
            #print("starttime_str" + starttime_str)
            starttime = datetime.datetime.strptime(starttime_str, '%Y%m%d%H%M%S')
        elif opt in ("-g", "--gap"):
            seconds = int(arg)

    if not os.path.isdir(dir):
        os.mkdir(dir)
    if not os.path.isfile(video):
        print("video file %s not found!" %(video))
        sys.exit(2)
        
    # use opencv to do the job    
    print(cv2.__version__)  # my version is 2.4.9.1
    vidcap = cv2.VideoCapture(video)
    count = 0
    while True:
        success,image = vidcap.read()
        if not success:
            break
        frame_time = starttime + datetime.timedelta(seconds=count*seconds)
        frame_datetime = frame_time.strftime("%Y%m%d%H%M%S")
        
        pathname = os.path.join(dir,"image_{:s}.jpg".format(frame_datetime))
        print(pathname)
        cv2.imwrite(pathname, image)     # save frame as JPEG file
        count += 1
    print("{} images are extacted in {}.".format(count,dir))

    # travel images to count Sprout
    # by default use first image as reference
    first_pathname = os.path.join(dir,"image_{:s}.jpg".format(starttime_str))
    #list_of_files = {}
    objSprout = SPROUT(cv2.imread(first_pathname), reSize, (5,5), 120, 2,4)

    for (dirpath, dirnames, filenames) in os.walk(dir):
        for filename in sorted(filenames):
            if filename.endswith('.jpg'): 
                filename_cur = os.sep.join([dirpath, filename])
                numSprout = objSprout.countSprout(cv2.imread(filename_cur)) - objSprout.notSprout
                print("%s,%i" %(filename_cur,numSprout))
