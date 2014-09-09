#!/bin/bash

#  Copies assets from the main game data directories
#  Scales down PNGs for a smaller APK
#  You'll need to re-run this every time the assets change


echo "Building assets..."

rm -rf assets

mkdir assets
mkdir assets/data

cp -r ../../data/australia_day assets/data/ || exit 1
cp -r ../../data/cr assets/data/ || exit 1
cp -r ../../data/i18n assets/data/ || exit 1
cp -r ../../data/intro assets/data/ || exit 1
cp -r ../../data/test assets/data/ || exit 1
cp -r ../../maps assets/ || exit 1

echo "Compressing assets..."

# PNGs
find assets/data/ | grep -i png | xargs mogrify -resize 256x256 -quality 99 || exit 1

# JPGs
find assets/data/ | grep -i jpg | xargs mogrify -resize 256x256 -quality 80 || exit 1

echo "Done."
