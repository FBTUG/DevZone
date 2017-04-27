#!/bin/bash
DATE=$(date +"%Y-%m-%d_%H%M%S")
fswebcam -d /dev/video0 -r 1280x960 --no-banner /home/pi/cam0_$DATE.jpg
