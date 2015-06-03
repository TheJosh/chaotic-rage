#!/bin/bash
set -e

# If no SDL directory, download and copy in there
if [ ! -d "jni/SDL" ]; then
	cd working/
	if [ ! -f "SDL2-2.0.1.tar.gz" ]; then
		wget http://www.libsdl.org/release/SDL2-2.0.1.tar.gz
	fi
	tar -xzf SDL2-2.0.1.tar.gz || exit 1
	mv SDL2-2.0.1 ../jni/SDL || exit 1
	cd ..
fi

# SDL_image
if [ ! -d "jni/SDL_image" ]; then
	cd working/ || exit 1
	if [ ! -f "SDL2_image-2.0.0.tar.gz" ]; then
		wget http://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.0.tar.gz
	fi
	tar -xzf SDL2_image-2.0.0.tar.gz || exit 1
	mv SDL2_image-2.0.0 ../jni/SDL_image || exit 1
	cd ..
fi

# SDL_mixer
if [ ! -d "jni/SDL_mixer" ]; then
	cd working/ || exit 1
	if [ ! -f "SDL2_mixer-2.0.0.tar.gz" ]; then
		wget http://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.0.tar.gz
	fi
	tar -xzf SDL2_mixer-2.0.0.tar.gz || exit 1
	mv SDL2_mixer-2.0.0 ../jni/SDL_mixer || exit 1
	cd ..
fi

# SDL_net
if [ ! -d "jni/SDL_net" ]; then
	cd working/ || exit 1
	if [ ! -f "SDL2_net-2.0.0.tar.gz" ]; then
		wget http://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.0.tar.gz
	fi
	tar -xzf SDL2_net-2.0.0.tar.gz || exit 1
	mv SDL2_net-2.0.0 ../jni/SDL_net || exit 1
	cd ..
fi

# bullet physics
if [ ! -d "jni/bullet" ]; then
	cd working/ || exit 1
	if [ ! -f "bullet-2.82-r2704.tgz" ]; then
		wget http://bullet.googlecode.com/files/bullet-2.82-r2704.tgz
	fi
	tar -xzf bullet-2.82-r2704.tgz || exit 1
	mv bullet-2.82-r2704 ../jni/bullet || exit 1
	cd ..
fi

# freetype
if [ ! -d "jni/freetype" ]; then
	cd jni/ || exit 1
	git clone https://github.com/android/platform_external_freetype.git freetype
	cd freetype || exit 1
	git checkout -b cr_branch 8afe960626dbf326354b836fd4d63e05b6974195
	rm -rf .git
	cd ../../
fi

# assimp
if [ ! -d "jni/assimp" ]; then
	cd working/ || exit 1
	if [ ! -f "assimp-3.1.1_no_test_models.zip" ]; then
		wget http://downloads.sourceforge.net/project/assimp/assimp-3.1/assimp-3.1.1_no_test_models.zip
	fi
	unzip -q assimp-3.1.1_no_test_models.zip || exit 1
	mv assimp-3.1.1 ../jni/assimp || exit 1
	cd ..
fi

# Copy stuff from the prep dir to the JNI dir.
cp -r prepare_data/* jni || exit 1
rm jni/README.txt

# Link up src directory
rm -f jni/src
ln -s ../../../src jni/src || exit 1
