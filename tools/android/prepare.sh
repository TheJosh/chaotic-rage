#!/bin/bash


# If no SDL directory, download and copy in there
if [ ! -d "jni/SDL" ]; then
	cd working/
	if [ ! -f "SDL2-2.0.1.tar.gz" ]; then
		wget http://www.libsdl.org/release/SDL2-2.0.1.tar.gz
	fi
	tar -xzf SDL2-2.0.1.tar.gz || exit 1
	mv SDL2-2.0.1 ../jni/SDL
	cd ..
fi

# SDL_image
if [ ! -d "jni/SDL_image" ]; then
	cd working/
	if [ ! -f "SDL2_image-2.0.0.tar.gz" ]; then
		wget http://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.0.tar.gz
	fi
	tar -xzf SDL2_image-2.0.0.tar.gz || exit 1
	mv SDL2_image-2.0.0 ../jni/SDL_image
	cd ..
fi

# SDL_mixer
if [ ! -d "jni/SDL_mixer" ]; then
	cd working/
	if [ ! -f "SDL2_mixer-2.0.0.tar.gz" ]; then
		wget http://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.0.tar.gz
	fi
	tar -xzf SDL2_mixer-2.0.0.tar.gz || exit 1
	mv SDL2_mixer-2.0.0 ../jni/SDL_mixer
	cd ..
fi

# SDL_net
if [ ! -d "jni/SDL_net" ]; then
	cd working/
	if [ ! -f "SDL2_net-2.0.0.tar.gz" ]; then
		wget http://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.0.tar.gz
	fi
	tar -xzf SDL2_net-2.0.0.tar.gz || exit 1
	mv SDL2_net-2.0.0 ../jni/SDL_net
	cd ..
fi

# bullet physics
if [ ! -d "jni/bullet" ]; then
	cd working/
	if [ ! -f "bullet-2.81-rev2613.tgz" ]; then
		wget http://chaoticrage.com/dev_libs/bullet-2.81-rev2613.tgz
	fi
	tar -xzf bullet-2.81-rev2613.tgz || exit 1
	mv bullet-2.81-rev2613 ../jni/bullet
	cd ..
fi

# freetype
if [ ! -d "jni/freetype" ]; then
	cd jni/
	git clone https://github.com/android/platform_external_freetype.git freetype
	rm -rf freetype/.git
	cd ..
fi

# assimp
if [ ! -d "jni/assimp" ]; then
	cd working/
	if [ ! -f "assimp--3.0.1270-source-only.zip" ]; then
		wget http://chaoticrage.com/dev_libs/assimp--3.0.1270-source-only.zip
	fi
	unzip -q assimp--3.0.1270-source-only.zip || exit 1
	mv assimp--3.0.1270-source-only ../jni/assimp
	cd ..
fi

# lua
if [ ! -d "jni/lua" ]; then
	cd working/
	if [ ! -f "lua-5.1.5.tar.gz" ]; then
		wget http://www.lua.org/ftp/lua-5.1.5.tar.gz
	fi
	tar -xzf lua-5.1.5.tar.gz || exit 1
	mv lua-5.1.5 ../jni/lua
	cd ..
fi

# Copy stuff from the prep dir to the JNI dir.
cp -r prepare_data/* jni
rm jni/README.txt

# Link up src directory
rm -f jni/src
ln -s ../../../src jni/src

# Link up assets
rm -rf assets
mkdir assets
mkdir assets/data
ln -s ../../../../data/cr assets/data/
ln -s ../../../../data/intro assets/data/
ln -s ../../../maps assets/

