#!/bin/bash
#
# Does a native build and an emscripten build of the various test scripts
#

set -e
set -v

# Clean up previous test results
rm -f tests/*.o tests/*.bc tests/*.js tests/*.js.map tests/*.gcc

# Basic hello world
g++ tests/hello.cpp -o tests/hello.gcc
em++ tests/hello.cpp -o tests/hello.html

# Bullet
g++ tests/bullet.cpp `pkg-config --cflags bullet` -c -o tests/bullet.o
g++ tests/bullet.o `pkg-config --libs bullet` -o tests/bullet.gcc
em++ tests/bullet.cpp `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --cflags bullet` -c -o tests/bullet.bc
em++ tests/bullet.bc `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --libs bullet` -o tests/bullet.html

# Assimp
g++ tests/assimp.cpp `pkg-config --cflags assimp` -c -o tests/assimp.o
g++ tests/assimp.o `pkg-config --libs assimp` -o tests/assimp.gcc
em++ tests/assimp.cpp `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --cflags assimp` -Iinclude -c -o tests/assimp.bc
em++ tests/assimp.bc `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --libs assimp` -o tests/assimp.html

# Freetype
g++ tests/freetype.cpp `pkg-config --cflags freetype2` -c -o tests/freetype.o
g++ tests/freetype.o `pkg-config --libs freetype2` -o tests/freetype.gcc
em++ tests/freetype.cpp `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --cflags freetype2` -c -o tests/freetype.bc
em++ tests/freetype.bc `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --libs freetype2` -o tests/freetype.html

# Lua
g++ tests/lua.cpp `pkg-config --cflags lua5.1` -c -o tests/lua.o
g++ tests/lua.o `pkg-config --libs lua5.1` -o tests/lua.gcc
em++ tests/lua.cpp `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --cflags lua5.1` -c -o tests/lua.bc
em++ tests/lua.bc `PKG_CONFIG_PATH=lib/pkgconfig pkg-config --libs lua5.1` -o tests/lua.html
