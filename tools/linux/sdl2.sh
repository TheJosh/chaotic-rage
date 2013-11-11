#!/bin/bash


cd working

if [ ! -d "SDL2-2.0.1" ]; then
	if [ ! -f "SDL2-2.0.1.tar.gz" ]; then
		wget http://www.libsdl.org/release/SDL2-2.0.1.tar.gz
	fi
	tar -xzf SDL2-2.0.1.tar.gz
fi

cd SDL2-2.0.1

./configure --prefix=`pwd`/../../environment

make

make install
