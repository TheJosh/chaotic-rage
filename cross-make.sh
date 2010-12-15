#!/bin/bash

# Where is 'crossenv' installed?
CROSSENV="/home/josh/crossenv"

# You will want an install of crossenv with the following packages:
#  - core
#  - libsdl
#  - libzzip
#  - libconfuse
#  - libsdl-mixer
#  - libsdl-image
#  - opengl
#  - glew


PREFIX="$CROSSENV/environments/i386-mingw32"
PATH="$PREFIX/bin:$PREFIX/local/bin:$PREFIX/$PLATFORM/bin:$PATH"
export PATH

LIBDIR="$CROSSENV/environments/i386-mingw32/lib"
export LIBDIR

make clean
make PLATFORM=i386-mingw32 all
