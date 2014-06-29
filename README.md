Chaotic Rage
============

Chaotic Rage is a crazy top-view shooter.
You can destroy everything with the many weapons, turrets and vehicles.
Choose from multiple characters with unique skills and abilities.

See also:
* http://chaoticrage.com


Compiling on Linux
------------------

Compiling on Linux should be a simple `make`.

You'll need the following dependencies:
* SDL 2.0.1
* SDL_Mixer 2.0
* SDL_Image 2.0
* SDL_Net 2.0
* gl
* glu
* glew
* Lua
* Freetype2
* Bullet Physics
* Assimp
* php5-cli

Some of these (SDL, Bullet, Assimp) have scripts in the `tools/linux` directory
for downloading, compiling and installing these libraries. Both Bullet and Assimp
use cmake, so you'll need that as well.


Compiling on Windows
--------------------

There is a project in the `tools/msvc2010` directory for MSVC++ 2010 Express Edition.
I haven't tested any other version of MSVC.
You should still be able to download that older version.


Compiling for Android on Linux
-----------------------------

This is a little incomplete and fussy at the moment.

There is a script in `tools/android` called `prepare.sh` which will try to set everything up for you.

You should then be able to run `ndk-build` to compile and `ant debug` to create the .apk.


Cross-compiling for Windows using MXE
-------------------------------------

Check out the latest master version of MXE from GitHub and install it's dependencies
```
git clone https://github.com/mxe/mxe.git
```

Then cd to the MXE directory and build all the dependencies
```
make sdl2 sdl2_mixer sdl2_image sdl2_net lua freetype bullet assimp
```

You should then be able to build the game using `make` with an additional option specified
```
make MXE=/path/to/mxe
```


Making Modifications
--------------------

The game is designed with extensability in mind. All of game data
is abstracted into data modules, and multiple mods can be loaded at once.

The mods contain the definitions for everything in the game,
as well as 3D models, sounds, songs, etc. The game also uses the scripting
language Lua, which provides logic for AIs, as well as managed game types.

You can read the official modding documentation at: http://chaoticrage.com/modding_docs


Contributing
------------

All contributions (even documentation) are welcome :) Open a pull request and I would be happy to merge them.

If you've got ideas, code or content, but don't know where to start, email me at help@chaoticrage.com.

You can read the technical documentation at: http://chaoticrage.com/dev_docs


Build Status
------------
Builds are tested on Travis CI

[![Build Status](https://travis-ci.org/TheJosh/chaotic-rage.png?branch=master)](https://travis-ci.org/TheJosh/chaotic-rage)
