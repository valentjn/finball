# INSTALLATION SCRIPT
# for the sources of the WiiCpp framework 
# in order to get a wiimote connected to a PC.

# Install dependencies (local packages from APT)
sudo dpkg -i *.deb

# Extract the archive
tar -xzvf wiic_1.1.tar.gz

cd WiiC

mkdir build
cd build
cmake ../src
make
sudo make install
make doc
