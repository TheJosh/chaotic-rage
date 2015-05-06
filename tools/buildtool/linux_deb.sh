#!/bin/bash
set -e

make clean
make deb
mv "chaoticrage_${VERSION}_amd64.deb" "$DESTDIR"
