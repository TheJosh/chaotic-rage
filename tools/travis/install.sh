#!/bin/bash

sudo apt-get update -qq || exit 1
sudo apt-get install -qq libgl1-mesa-dev libglu1-mesa-dev libglew-dev libfreetype6-dev minizip freeglut3-dev libmicrohttpd-dev >/dev/null || exit 1

cd tools/linux;
./assimp.sh >/dev/null || exit 1;
./bullet.sh >/dev/null || exit 1;
./sdl2.sh >/dev/null || exit 1;
./sdl2_image.sh >/dev/null || exit 1;
./sdl2_mixer.sh >/dev/null || exit 1;
./sdl2_net.sh >/dev/null || exit 1;
cd ../..;
