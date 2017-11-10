from class_ConfigSetting import*

ItemList=[]
ItemList.append("thrshd_gray")
ItemList.append("thrshd_size")
ItemList.append("Scan_X (Beg,Interval,Amount)")
ItemList.append("Scan_Y (Beg,Interval,Amount)")
ItemList.append("limit Maximum (X,Y)")

defaultValueList=[]
defaultValueList.append(128)
defaultValueList.append(20)
defaultValueList.append([0,500,4])
defaultValueList.append([0,500,4])
defaultValueList.append([8000,95000])

ValueList=[]
ValueList.append(0)
ValueList.append(1000)
ValueList.append([0,900,4])
ValueList.append([0,900,4])
ValueList.append([90,555])

configName= 'config_test.json'
saveParaPath= 'Data/'
config= ConfigSetting(saveParaPath, configName, ItemList, defaultValueList)
config.write_json(ValueList)

