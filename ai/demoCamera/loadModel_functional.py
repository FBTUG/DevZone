from keras.models import model_from_yaml
from keras.optimizers import Adam
from keras import backend as K

# load YAML and create model
yaml_file = open('models/leaf.yaml', 'r')
loaded_model_yaml = yaml_file.read()
yaml_file.close()
loaded_model = model_from_yaml(loaded_model_yaml)
# load weights into new model
loaded_model.load_weights("models/leaf.h5")
print("Loaded model from disk")
#ADAM = Adam(lr=0.001, beta_1=0.9, beta_2=0.999, epsilon=1e-08, decay=0.0)
#loaded_model.compile(loss='categorical_crossentropy', optimizer=ADAM, metrics=['accuracy'])
from keras import optimizers
loaded_model.compile(loss='categorical_crossentropy', optimizer='sgd', metrics=['accuracy'])


#predict image
from keras.preprocessing import image
import numpy as np
import pandas as pd 

#Lable的對應, 每個Lable對應到一個數值
dict_labels = {"Tilia tomentosa": 9, "Quercus suber": 0, "Buxus sempervirens": 28,
               "Chelidonium majus": 37, "Acer palmaturu": 10, "Pinus sp": 19,
               "Salix atrocinerea": 1, "Schinus terebinthifolius": 38, "Celtis sp": 11,
               "Fraxinus sp": 20, "Urtica dioica": 29, "Populus nigra": 2,
               "Corylus avellana": 12, "Primula vulgaris": 21, "Podocarpus sp": 30,
               "Fragaria vesca": 39, "Castanea sativa": 13, "Erodium sp": 22,
               "Acca sellowiana": 31, "Alnus sp": 3, "Populus alba": 14, "Bougainvillea sp": 23,
               "Hydrangea sp": 32, "Quercus robur": 4, "Acer negundo": 15, "Arisarum vulgare": 24,
               "Pseudosasa japonica": 33, "Crataegus monogyna": 5, "Taxus bacatta": 16, "Euonymus japonicus": 25,
               "Magnolia grandiflora": 34, "Ilex aquifolium": 6, "Papaver sp": 17, "Ilex perado ssp azorica": 26,
               "Geranium sp": 35, "Nerium oleander": 7, "Polypodium vulgare": 18, "Magnolia soulangeana": 27,
               "Aesculus californica": 36, "Betula pubescens": 8 }
inv_dict_labels = {v: k for k, v in dict_labels.items()}

img = image.load_img('takePics/iPAD2_C02_EX07.jpg', target_size=(96, 96))
x = image.img_to_array(img)
x = np.expand_dims(x, axis=0)
x = x.astype('float32') / 255.0

images = np.vstack([x])

#K.clear_session()
prediction = loaded_model.predict(x)
print(prediction)
predict_label_tmp = np.argmax(prediction,axis=1)
print(predict_label_tmp)
predict_label = predict_label_tmp[0]
print(inv_dict_labels[predict_label])
