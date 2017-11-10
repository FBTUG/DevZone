#==========================
#  Config Parameters
#==========================
interval_x= 6
interval_y= 6
entry_width= 30
btn_width= 5
btn_hegiht= 1
grp_offsetX= -2
grp_offsetY= -16
interval_rdbox= 60
#===================================================
# Save Path
#===================================================
savePath= 'Data/'
saveParaPath= 'Para/'
saveScanningPath= savePath+ 'Scanning/'
saveImageProccesPath= savePath+ 'ImageProcess/'
configName= 'config.json'
scanIndex= 'Raw'
#==========================
#  Config Text
#==========================
rdbox_PlantIndexItem= ["LAB", "NDI", "ExG"]
rdbox_BinaryMethodItem= ["Simple", "Otsu", "Adaptive"]

#===================================================
# Defalut Value
#===================================================
defaultDict={\
        'thrshd_gray': 128,\
        'thrshd_Minsize': 1000,\
        'thrshd_Maxsize': 9999,\
        'Scan_X (Beg,Interval,Amount)': [0,500,4],\
        'Scan_Y (Beg,Interval,Amount)':[0,500,4],\
        'limit Maximum (X,Y)':[100000, 100000],\
        'Max Speed (X, Y)':[400,400,400],\
        'Ac/Deceleration (X, Y)':[100,100,100],\
        'Camera ID':0,\
        'Peripheral Setting': [('Fan',8),('Water Pump',9),('Vaccum Pump',10)],\
        'Move Amount type (5 types)':[('100', 100),('500', 500),('1k',1000),('10k',10000), ('100k',100000)],\
        'script Path':"Script/test.txt"\
        }
