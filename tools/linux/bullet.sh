#!/bin/bash
set -e

cd working

if [ ! -d "bullet-2.82-r2704" ]; then
	if [ ! -f "bullet-2.82-r2704.tgz" ]; then
		wget http://bullet.googlecode.com/files/bullet-2.82-r2704.tgz
	fi
	tar -xzf bullet-2.82-r2704.tgz || exit 1
fi

cd bullet-2.82-r2704

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=Release \
	-DINSTALL_LIBS=ON \
	-DBUILD_SHARED_LIBS=ON \
	-DBUILD_DEMOS:BOOL=OFF \
	-DBUILD_CPU_DEMOS:BOOL=OFF \
	-DBUILD_EXTRAS:BOOL=OFF \
	|| exit 1
	
make || exit 1

sudo make install || exit 1

sudo ldconfig || exit 1
