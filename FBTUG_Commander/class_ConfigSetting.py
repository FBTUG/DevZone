import json
import os
from os import path, listdir, makedirs, remove

class ConfigSetting:
    def __init__(self, arg_path, arg_filename, arg_default):
        self.path= arg_path
        self.filename= arg_filename
        #self.ItemList= arg_ItemList
        self.default= arg_default
        self.check_path(self.path)

    def write_json(self, arg_dict):
        #data= dict()
        data= arg_dict
        #for item, value  in zip(arg_ItemList, arg_ValueList):
        #    data[item]= value
        self.check_path(self.path)
        with open(self.path+self.filename , 'w') as file_pointer:
            json.dump(data, file_pointer)
        
    def read_json(self):
        if path.isfile(self.path+self.filename):
            with open(self.path+self.filename , 'r') as file_pointer:
                data = json.load(file_pointer)
            for ky in self.default.keys():
                if not(data.has_key(ky)):
                    data.update({ky: self.default[ky]})
            #for i in range(0,len(arg_ItemList)):
            #    try:
            #        tmp= data[arg_ItemList[i]]
            #    except:
            #        data[arg_ItemList[i]]= self.default[i]
        else:
            data = self.default
        return data

    def check_path(self,arg_path):
	# make sure output dir exists
	if(not os.path.isdir(arg_path)):
	    os.makedirs(arg_path)
