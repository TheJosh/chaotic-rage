Compiling Chaotic Rage
======================

Chaotic Rage is supported on a number of platforms and through various build environments. In many cases you don't need to compile Chaotic Rage yourself, but can instead use one of the [release builds](http://www.chaoticrage.com/download) or [weekly builds](http://dev1.chaoticrage.com/weekly).


Dependencies
------------

You'll need the following dependencies:
* SDL 2.0.1
* SDL_Mixer 2.0
* SDL_Image 2.0
* SDL_Net 2.0
* gl
* glu
* glew
* Freetype2
* Bullet Physics
* Assimp
* Fontconfig (Linux only)
* libmicrohttp


Compiling on Debian/Ubuntu/Mint
-------------------------------

This build configuration has been tested on Debian 7, Debian 8, Ubuntu 10.04, Ubuntu 12.04 and Mint 17.

Install the dependencies:

```.sh
sudo apt-get install g++ libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-net-dev libgl1-mesa-dev libglu1-mesa-dev libglew-dev libfreetype6-dev libbullet-dev libassimp-dev libfontconfig-dev libmicrohttp
```

Some of these (SDL, Bullet, Assimp) have scripts in the `tools/linux` directory
for downloading, compiling and installing these libraries. These are useful
for older distributions which might not package all of these dependencies.

Both Bullet and Assimp use cmake, so you'll need that as well if you're using
their install scripts.


Compiling on Gentoo
-------------------

To compile this game in gentoo, add these USE flags in package.use:
media-libs/sdl2-mixer flac wav vorbis
media-libs/sdl2-image jpeg png

Install the dependencies:
* sdl2-image
* sdl2-mixer
* sdl2-net
* libsdl2
* mesa
* glew
* freetype
* bullet
* assimp
* fontconfig
* libmicrohttp


Compiling on Windows using MSVC
-------------------------------

This build configuration has been tested on MSVC++ 2010 Express Edition on Windows 7 and Windows XP.

There is a project in the `tools/msvc2010` directory.

I haven't tested any other version of MSVC. You should still be able to
download this older version.


Cross-compiling for Windows using MXE
-------------------------------------

This build configuration has been tested on Debian 7 and Debian 8.

Check out the latest master version of MXE from GitHub and install it's dependencies
```
git clone https://github.com/mxe/mxe.git
```

Then cd to the MXE directory and build all the dependencies
```
make MXE_TARGETS='i686-w64-mingw32.static' sdl2 sdl2_mixer sdl2_image sdl2_net freetype bullet assimp glew
```

You should then be able to build the game using `make` with an additional option specified
```
make MXE=/path/to/mxe
```
