#!/bin/bash
#
# Build all the deps we need for emscripten
#


mkdir -p lib
mkdir -p working

# Bullet
if [ ! -d "working/bullet" ]; then
	cd working/
	if [ ! -f "bullet-2.82-r2704.tgz" ]; then
		wget http://bullet.googlecode.com/files/bullet-2.82-r2704.tgz
	fi
	tar -xzf bullet-2.82-r2704.tgz || exit 1
	mv bullet-2.82-r2704 bullet
	
	cd bullet
	cmake -G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Release \
		-DINSTALL_LIBS=ON \
		-DBUILD_SHARED_LIBS=ON \
		-DBUILD_DEMOS:BOOL=OFF \
		-DBUILD_CPU_DEMOS:BOOL=OFF \
		-DBUILD_EXTRAS:BOOL=OFF \
		|| exit 1
	emmake make || exit 1
	cd ..
fi

