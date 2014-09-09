#!/bin/bash
#
# Does a native build and an emscripten build of the various test scripts
#

#set -e
#set -v
#export EMCC_DEBUG=1

# Clean up previous test results
rm -f tests/*.o tests/*.bc tests/*.js tests/*.js.map tests/*.gcc

# Basic hello world
g++ tests/hello.cpp -o tests/hello.gcc
em++ tests/hello.cpp -o tests/hello.html

# Bullet
g++ tests/bullet.cpp `pkg-config --cflags bullet` -c -o tests/bullet.o
g++ tests/bullet.o `pkg-config --libs bullet` -o tests/bullet.gcc
em++ tests/bullet.cpp `pkg-config --cflags bullet` -c -o tests/bullet.bc
em++ tests/bullet.bc lib/libBulletDynamics.bc lib/libBulletCollision.bc lib/libLinearMath.bc -o tests/bullet.html

# Assimp
g++ tests/assimp.cpp `pkg-config --cflags assimp` -c -o tests/assimp.o
g++ tests/assimp.o `pkg-config --libs assimp` -o tests/assimp.gcc
em++ tests/assimp.cpp `pkg-config --cflags assimp` -c -o tests/assimp.bc
em++ tests/assimp.bc lib/libassimp.bc o tests/assimp.html
