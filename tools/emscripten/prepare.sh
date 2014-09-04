#!/bin/bash
#
# Build all the deps we need for emscripten
#


mkdir -p lib
mkdir -p working

cd working

## Bullet
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
	|| exit 1
emmake make || exit 1
cp src/LinearMath/libLinearMath.so ../../lib/libLinearMath.bc
cp src/BulletDynamics/libBulletDynamics.so ../../lib/libBulletDynamics.bc
cp src/BulletCollision/libBulletCollision.so ../../lib/libBulletCollision.bc
cd ..
