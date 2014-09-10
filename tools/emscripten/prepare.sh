#!/bin/bash
#
# Build all the deps we need for emscripten
#


mkdir -p lib
mkdir -p include
mkdir -p working

cd working

## Bullet
if [ ! -f "../lib/libLinearMath.bc" ]; then
	rm -rf bullet
	if [ ! -f "bullet-2.82-r2704.tgz" ]; then
		wget http://bullet.googlecode.com/files/bullet-2.82-r2704.tgz
	fi
	tar -xzf bullet-2.82-r2704.tgz || exit 1
	mv bullet-2.82-r2704 bullet || exit 1
	cd bullet
	emconfigure cmake \
		-DBUILD_SHARED_LIBS=ON \
		-DBUILD_DEMOS=OFF \
		-DBUILD_CPU_DEMOS=OFF \
		-DBUILD_EXTRAS=OFF \
		-DUSE_GLUT=OFF \
		-DCMAKE_INSTALL_PREFIX:PATH=`readlink -f ../..` \
		|| exit 1
	emmake make || exit 1
	emmake make install || exit 1
	cd ..
fi


## Assimp
if [ ! -f "../lib/libassimp.bc" ]; then
	rm -rf assimp
	if [ ! -f "assimp--3.0.1270-source-only.zip" ]; then
		wget http://chaoticrage.com/dev_libs/assimp--3.0.1270-source-only.zip
	fi
	unzip -q assimp--3.0.1270-source-only.zip || exit 1
	mv assimp--3.0.1270-source-only assimp || exit 1
	cd assimp
	patch -p1 -N --binary -i ../../assimp1.patch || exit 1
	patch -p1 -N --binary -i ../../assimp2.patch || exit 1
	emconfigure cmake \
		-DBUILD_STATIC_LIB=OFF \
		-DBUILD_ASSIMP_TOOLS=OFF \
		-DBUILD_ASSIMP_SAMPLES=OFF \
		-DENABLE_BOOST_WORKAROUND=ON \
		-DCMAKE_INSTALL_PREFIX:PATH=`readlink -f ../..` \
		|| exit 1
	emmake make || exit 1
	emmake make install || exit 1
	cd ..
fi

