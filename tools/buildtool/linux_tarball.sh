#!/bin/sh

make clean
make dist
cp "chaoticrage-linux-$VERSION.tar.bz2" "$DESTDIR"
