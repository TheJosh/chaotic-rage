#!/bin/sh

DISTTMP="chaoticrage-mxe-$VERSION"

# Build
make clean
make MXE=~/mxe

# Package
mkdir "$DISTTMP"
mv chaoticrage.exe "$DISTTMP"
cp -r data "$DISTTMP"
cp -r maps "$DISTTMP"
cp -r README.md "$DISTTMP"
cp -r LICENSE "$DISTTMP"

# Everything else is (currently) compiled statically
# except Assimp for some reason. This is probably a bug in MXE.
cp ~/mxe/usr/i686-w64-mingw32.static/lib/libassimp.dll "$DISTTMP"

# Compress
zip -9 -r "chaoticrage-mxe-$VERSION.zip" "$DISTTMP"
mv "chaoticrage-mxe-$VERSION.zip" "$DESTDIR"

# Cleanup
rm -r "$DISTTMP"
