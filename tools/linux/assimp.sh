#!/bin/bash
set -e

cd working

if [ ! -d "assimp-3.1.1" ]; then
	if [ ! -f "assimp-3.1.1_no_test_models.zip" ]; then
		wget http://downloads.sourceforge.net/project/assimp/assimp-3.1/assimp-3.1.1_no_test_models.zip
	fi
	unzip -q assimp-3.1.1_no_test_models.zip || exit 1
fi

cd assimp-3.1.1

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=RelWithDbgInfo \
	-DBUILD_STATIC_LIB=OFF \
	-DBUILD_ASSIMP_TOOLS=OFF \
	-DBUILD_ASSIMP_SAMPLES=OFF \
	-DENABLE_BOOST_WORKAROUND=ON \
	|| exit 1
	
make || exit 1

sudo make install || exit 1

sudo ldconfig || exit 1
