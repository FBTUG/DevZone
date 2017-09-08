sudo apt-get install git
sudo apt-get update -y
sudo apt-get upgrade -y

# Download FBTUS relatcd code to /tmp
#====================================================================
cd ~
mkdir FBTUG
cd FBTUG
git clone https://github.com/SpongeYao/StepperControl.git
# Add below code into /Tmp/StepperControl/gui_main.py for support "sudo python gui_main.py"
# import sys
# sys.path.append('/usr/local/lib/python2.7/site-packages')
git clone https://github.com/FBTUG/DevZone.git
cd ~

# For Arduino
#====================================================================
sudo apt-get install -y arduino gcc-avr avr-libc avrdude python-configobj python-jinja2 python-serial
cd FBTUG
git clone https://github.com/miracle2k/python-glob2
cd python-glob2
wget https://bootstrap.pypa.io/ez_setup.py -O - | sudo python
sudo python setup.py install
git clone git://github.com/amperka/ino.git
cd ino
sudo make install

# for OpenCV
#====================================================================
sudo apt-get install -y build-essential cmake pkg-config
sudo apt-get install -y libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev
sudo apt-get install -y libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
sudo apt-get install -y libxvidcore-dev libx264-dev
sudo apt-get install -y libgtk2.0-dev
sudo apt-get install -y libatlas-base-dev gfortran
sudo apt-get install -y python2.7-dev python3-dev

sudo apt-get -y dist-upgrade
sudo apt-get install -y python-dev python-tk python-numpy python3-dev python3-tk python3-numpy
sudo apt-get install -y python-imaging-tk sudo python-tk idle python-pmw python-imaging
sudo apt-get install -y qt5-default libvtk6-dev
sudo apt-get install -y zlib1g-dev libjpeg-dev libwebp-dev libpng-dev libtiff5-dev libjasper-dev libopenexr-dev libgdal-dev
sudo apt-get install -y doxygen

cd ~
wget -O opencv.zip https://github.com/Itseez/opencv/archive/3.1.0.zip
unzip opencv.zip
wget -O opencv_contrib.zip https://github.com/Itseez/opencv_contrib/archive/3.1.0.zip
unzip opencv_contrib.zip
wget https://bootstrap.pypa.io/get-pip.py
sudo python get-pip.py
sudo pip install virtualenv virtualenvwrapper
sudo rm -rf ~/.cache/pip

# modify ~/.profile
cat ./FBTUG/DevZone/mini-farmbot/Rpi_OS/Script/profile_change >> ~/.profile

source ~/.profile
mkvirtualenv cv -p python2
pip install numpy
workon cv
cd ~/opencv-3.1.0/
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D INSTALL_PYTHON_EXAMPLES=ON \
-D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-3.1.0/modules \
-D BUILD_EXAMPLES=ON ..
make -j4 (or make)
sudo make install -y
sudo ldconfig

#====================================================================
sudo apt-get install -y minicom
sudo apt-get install -y screen

cat ./FBTUG/DevZone/mini-farmbot/Rpi_OS/Script/bashrc_change >> ~/.bashrc

