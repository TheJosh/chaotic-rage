#!/bin/bash
set -e

rm -f "chaoticrage_*_amd64.deb"

make clean
make deb

mv "chaoticrage_*_amd64.deb" "${DESTDIR}/chaoticrage_${VERSION}_amd64.deb"

rm -f "chaoticrage_*_amd64.changes"
rm -f "chaoticrage_*_amd64.build"
rm -f "chaoticrage_*_amd64.dsc"
rm -f "chaoticrage_*_amd64.tar.xz"
