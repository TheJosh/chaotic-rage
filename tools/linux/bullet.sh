#!/bin/bash
set -e

cd working

if [ ! -d "bullet3-2.83.6" ]; then
	if [ ! -f "2.83.6.tar.gz" ]; then
		wget https://github.com/bulletphysics/bullet3/archive/2.83.6.tar.gz
	fi
	tar -xzf 2.83.6.tar.gz || exit 1
fi

cd bullet3-2.83.6

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=Release \
	-DINSTALL_LIBS=ON \
	-DBUILD_SHARED_LIBS=ON \
	-DBUILD_DEMOS:BOOL=OFF \
	-DBUILD_CPU_DEMOS:BOOL=OFF \
	-DBUILD_EXTRAS:BOOL=OFF \
	-DBUILD_BULLET3:BOOL=OFF \
	-DBUILD_UNIT_TESTS:BOOL=OFF \
	-DBUILD_BULLET2_DEMOS:BOOL=OFF \
	|| exit 1
	
make || exit 1

sudo make install || exit 1

sudo ldconfig || exit 1
