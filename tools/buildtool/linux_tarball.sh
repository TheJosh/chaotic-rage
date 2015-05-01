#!/bin/bash
set -e

make clean
make dist
mv "chaoticrage-linux-$VERSION.tar.bz2" "$DESTDIR"
