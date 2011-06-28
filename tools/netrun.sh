#!/bin/bash

echo "Building:"
make -j4 || exit;

echo "Starting server."
./dedicatedserver > server.log &

echo "Starting client."
./chaoticrage > client.log

echo "All done"
killall dedicatedserver
