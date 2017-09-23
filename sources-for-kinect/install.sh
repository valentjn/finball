# INSTALLATION SCRIPT
# for the sources of the OpenNI framework 
# in order to get a kinect running on a PC.

# Install dependencies (local packages from APT)
#sudo dpkg -i *.deb

# New command to install packeges with apt
sudo apt install git-core cmake freeglut3-dev pkg-config build-essential libxmu-dev libxi-dev libusb-1.0-0-dev doxygen graphviz mono-complete

#mkdir kinect
#cd ./kinect

# Extract the archives
tar -xzvf OpenNI-Unstable-1.5.4.0.tar.gz
tar -xzvf SensorKinect-0.93-5.1.2.1.tar.gz
tar -xvjpf nite-bin-linux-x64-v1.5.2.21.tar.bz2

# Install OpenNI
cd OpenNI-Unstable-1.5.4.0/Platform/Linux/CreateRedist/
chmod +x RedistMaker
./RedistMaker
cd ..
cd Redist/OpenNI-Bin-Dev-Linux-x64-v1.5.4.0/
sudo ./install.sh

cd ../../../../..

# Install SensorKinect
cd SensorKinect-0.93-5.1.2.1/Platform/Linux/CreateRedist/
chmod +x RedistMaker
./RedistMaker
cd ..
cd Redist/Sensor-Bin-Linux-x64-v5.1.2.1/
sudo ./install.sh

cd ../../../../..

# Install NITE
cp *.xml NITE-Bin-Dev-Linux-x64-v1.5.2.21/Data
cd NITE-Bin-Dev-Linux-x64-v1.5.2.21/
sudo ./install.sh

sudo modprobe -r gspca_kinect 
sudo modprobe -r gspca_main



