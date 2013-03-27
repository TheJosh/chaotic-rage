#!/bin/bash


#
#  Bullet Physics
#

# Fetch
rm -r assimp--3.0.1270-source-only
wget -O assimp--3.0.1270-source-only.zip "http://downloads.sourceforge.net/project/assimp/assimp-3.0/assimp--3.0.1270-source-only.zip?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fassimp%2Ffiles%2Fassimp-3.0%2Fassimp--3.0.1270-source-only.zip%2Fdownload%3Fuse_mirror%3Dinternode&ts=1359513943&use_mirror=internode"
unzip assimp--3.0.1270-source-only.zip

# Prep
rm -r assimp-build
mkdir assimp-build
cd assimp-build
cmake ../assimp--3.0.1270-source-only -G "Unix Makefiles" -DBUILD_STATIC_LIB=ON -DBUILD_ASSIMP_TOOLS=ON -DBUILD_ASSIMP_SAMPLES=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo

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
echo "Installed, doing an 'updatedb'."
echo
sudo updatedb

