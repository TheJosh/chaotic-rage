#!/bin/bash

#  Copies assets from the main game data directories
#  Scales down PNGs for a smaller APK
#  You'll need to re-run this every time the assets change


echo "Building assets..."

rm -rf assets

mkdir assets
mkdir assets/data

cp -r ../../data/cr assets/data/ || exit 1
cp -r ../../data/intro assets/data/ || exit 1
cp -r ../../maps assets/ || exit 1

echo "Compressing assets..."

# PNGs
mogrify -resize 256x256 -quality 99 assets/data/cr/campaigns/*.png || exit 1
mogrify -resize 256x256 -quality 99 assets/data/cr/floortypes/*.png || exit 1
mogrify -resize 256x256 -quality 99 assets/data/cr/models/*.png || exit 1

# JPGs
mogrify -resize 256x256 -quality 80 assets/data/cr/models/*.jpg || exit 1
mogrify -resize 256x256 -quality 80 assets/data/cr/menu/bg.jpg || exit 1

echo "Done."
