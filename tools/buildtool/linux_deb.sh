#!/bin/bash
set -e

rm "chaoticrage_*_amd64.deb"

make clean
make deb

mv "chaoticrage_*_amd64.deb" "${DESTDIR}/chaoticrage_${VERSION}_amd64.deb"

rm "chaoticrage_*_amd64.changes"
rm "chaoticrage_*_amd64.build"
rm "chaoticrage_*_amd64.dsc"
rm "chaoticrage_*_amd64.tar.xz"
