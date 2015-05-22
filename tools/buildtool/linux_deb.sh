#!/bin/bash
set -e

# Clear these to avoid when we do the mv below
rm -f chaoticrage_*.deb
rm -f chaoticrage_*.dsc
rm -f chaoticrage_*.changes
rm -f chaoticrage_*.tar.xz

# Do the build
make clean
make deb

# The deb filename will ALWAYS be the version number
# It ignores our $VERSION string
# Just grab whatever file was created and rename it as required
mv chaoticrage_*_amd64.deb "${DESTDIR}/chaoticrage_${VERSION}_amd64.deb"
mv chaoticrage-data_*_all.deb "${DESTDIR}/chaoticrage-data_${VERSION}_all.deb"

# Move across the source description and changes
mv chaoticrage_*.dsc "${DESTDIR}/chaoticrage_${VERSION}.dsc"
mv chaoticrage_*.changes "${DESTDIR}/chaoticrage_${VERSION}.changes"

# Also move across the source archive, but not the .orig archive
rm -f chaoticrage_*.orig.tar.xz
mv chaoticrage_*.tar.xz "${DESTDIR}/chaoticrage_${VERSION}.tar.xz"

# Clear any reminant junk
rm -f chaoticrage_*.deb
rm -f chaoticrage_*.changes
rm -f chaoticrage_*.build
rm -f chaoticrage_*.dsc
rm -f chaoticrage_*.tar.xz
