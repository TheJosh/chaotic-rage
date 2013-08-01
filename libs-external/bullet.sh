#!/bin/bash


#
#  Bullet Physics
#

# Fetch
wget https://bullet.googlecode.com/files/bullet-2.81-rev2613.tgz
tar -zvxf bullet-2.81-rev2613.tgz

# Prep
rm -r bullet-build
mkdir bullet-build
cd bullet-build
cmake ../bullet-2.81-rev2613 -G "Unix Makefiles" \
	-DINSTALL_LIBS=ON \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_BUILD_TYPE=RelWithDebInfo \
	-DBUILD_DEMOS:BOOL=OFF \
	-DBUILD_CPU_DEMOS:BOOL=OFF \
	-DBUILD_EXTRAS:BOOL=OFF

# Compile
make -j4

# Install
echo
echo "We are about to a 'sudo make install'."
echo "You will be asked your password."
echo
sudo make install

# Update lib caches
echo
echo "Installed, doing an 'updatedb' and 'ldconfig'."
echo
#sudo updatedb
sudo ldconfig

