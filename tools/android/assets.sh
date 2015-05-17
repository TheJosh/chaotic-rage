#!/bin/bash

#  Copies assets from the main game data directories
#  Scales down PNGs for a smaller APK
#  You'll need to re-run this every time the assets change
set -e

echo "Building assets..."

rm -rf assets

mkdir -p assets
mkdir -p assets/data

cp -r ../../data/australia_day assets/data/ || exit 1
cp -r ../../data/cr assets/data/ || exit 1
cp -r ../../data/i18n assets/data/ || exit 1
cp -r ../../data/intro assets/data/ || exit 1
cp -r ../../data/test assets/data/ || exit 1
cp -r ../../maps assets/ || exit 1

# Remove most songs
find ./assets/data/cr/songs/ -name "*.ogg" -not -name "Alex*" -delete || exit 1

# Android-specific assets overrides
cp -r tweaked_assets/* assets/ || exit 1


echo "Compressing assets..."

find assets/data/ | grep -i png | xargs mogrify -resize 256x256 -quality 99 -depth 24 || exit 1
find assets/data/ | grep -i jpg | xargs mogrify -resize 256x256 -quality 80 || exit 1

echo "Done."
