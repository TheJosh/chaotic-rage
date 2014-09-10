#!/bin/bash
#
# Build all the deps we need for emscripten
#


mkdir -p lib
mkdir -p include
mkdir -p working

cd working

## Bullet
if [ ! -f "../lib/libLinearMath.so" ]; then
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
if [ ! -f "../lib/libassimp.so" ]; then
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


## Freetype
if [ ! -f "../lib/libfreetype.so" ]; then
	rm -rf freetype
	if [ ! -f "freetype-2.5.3.tar.bz2" ]; then
		wget http://download.savannah.gnu.org/releases/freetype/freetype-2.5.3.tar.bz2
	fi
	tar -xjf freetype-2.5.3.tar.bz2 || exit 1
	mv freetype-2.5.3 freetype || exit 1
	cd freetype
	echo "# XXX emptied out for emscripten" > builds/exports.mk     
	emconfigure ./configure \
		--with-zlib=no \
		--with-bzip2=no \
		--with-png=no \
		--with-harfbuzz=no \
		--prefix=`readlink -f ../..` \
		|| exit 1
	make || exit 1
	make install || exit 1
	cd ..
fi

