# Get Snail Meter information from internet, setup LED strip color
#    by sending serial commands to arduino
# expected arduino interface:
#    UART (57600N81) accept commands string
#        'R','r','G','g','Y','y' to setup LED color
# Current code polling internet information every minutes.
import serial
import time
import urllib2
import simplejson 
snail_link = "http://45.79.98.14:10001/snail" 
delay_seconds = 60
# example API output
#{
#  "color": "Y"
#}

#LASS examples
def get_lass_info():
    device_id = "74DA38B05380"
    json_link = "http://pm25.lass-net.org/data/last.php?device_id=%s" % (device_id)
    try: 
        response = urllib2.urlopen(json_link)  
        json_data = simplejson.load(response)
        for feeds in json_data['feeds']:
            #print "s_d0=%s" % (feeds['s_d0'])
            return float(feeds['AirBox']['s_d0'])

    except IOError, e: 
        return False


def lass_to_color(value):
    if value:
        if value<35:
            color='G'
        elif value<70:
            color='Y'
        else:
            color='R'    
    else:
        color = 'R'
    return color


def get_snail_info():
    try: 
        response = urllib2.urlopen(snail_link)  
        json_data = simplejson.load(response)
        return json_data['color']

    except IOError, e: 
        return False

def setup():
    global s
    # open serial COM port to /dev/ttyS0, which maps to UART0(D0/D1) # the baudrate is set to 57600 and should be the same as the one # specified in the Arduino sketch uploaded to ATmega32U4.
    s = serial.Serial("/dev/ttyS0", 57600)

def loop():
    
    color = str(get_snail_info())
    
    if color:
        #s.write("R")
        s.write(color)
    time.sleep(delay_seconds)

if __name__ == '__main__':
    setup()
    while True:
        loop()