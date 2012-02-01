#!/bin/bash

wget http://bullet.googlecode.com/files/bullet-2.79-rev2440.tgz

tar -zvxf bullet-2.79-rev2440.tgz

mkdir bullet-build

cd bullet-build

cmake ../bullet-2.79 -G "Unix Makefiles" -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=ON

make -j4

echo
echo "All built"
echo "Installing now."
echo 

sudo make install

sudo updatedb

sudo ldconfig

