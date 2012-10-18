#!/bin/bash


#
#  Bullet Physics
#

# Fetch
wget http://bullet.googlecode.com/files/bullet-2.79-rev2440.tgz
tar -zvxf bullet-2.79-rev2440.tgz

# Prep
mkdir bullet-build
cd bullet-build
cmake ../bullet-2.79 -G "Unix Makefiles" -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=ON

# Compile
make -j4

# Install
sudo make install
sudo updatedb
sudo ldconfig

