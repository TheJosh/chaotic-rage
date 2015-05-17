#!/bin/bash
set -e

cd working

if [ ! -d "SDL2-2.0.1" ]; then
	if [ ! -f "SDL2-2.0.1.tar.gz" ]; then
		wget http://www.libsdl.org/release/SDL2-2.0.1.tar.gz
	fi
	tar -xzf SDL2-2.0.1.tar.gz || exit 1
fi

cd SDL2-2.0.1

./configure	|| exit 1

make || exit 1

sudo make install || exit 1

sudo ldconfig || exit 1
