#!/bin/bash
set -e

cd working

if [ ! -d "assimp-3.2" ]; then
	if [ ! -f "v3.2.tar.gz" ]; then
		wget https://github.com/assimp/assimp/archive/v3.2.tar.gz
	fi
	tar -xzf v3.2.tar.gz || exit 1
fi

cd assimp-3.2

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=RelWithDbgInfo \
	-DBUILD_STATIC_LIB=OFF \
	-DBUILD_ASSIMP_TOOLS=OFF \
	-DBUILD_ASSIMP_SAMPLES=OFF \
	-DENABLE_BOOST_WORKAROUND=ON \
	-DASSIMP_BUILD_TESTS=OFF \
	|| exit 1
	
make || exit 1

sudo make install || exit 1

sudo ldconfig || exit 1
