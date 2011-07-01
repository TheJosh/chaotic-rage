#!/bin/bash

echo "Building:"
make -j4 || exit;

echo "Starting server."
SDL_VIDEO_WINDOW_POS=0,0 ./dedicatedserver > server.log &

echo "Starting client."
SDL_VIDEO_WINDOW_POS=1000,0 ./chaoticrage > client.log

echo "All done"
killall dedicatedserver
