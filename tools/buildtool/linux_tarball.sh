#!/bin/sh

make clean
make dist
cp "chaoticrage-linux-$VERSION" "$DESTDIR"
