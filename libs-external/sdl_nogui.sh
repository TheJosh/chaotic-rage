#!/bin/bash

wget http://www.libsdl.org/release/SDL-1.2.15.tar.gz
tar -zvxf SDL-1.2.15.tar.gz
cd SDL-1.2.15

./configure \
	--disable-audio \
	--disable-video \
	--disable-events \
	--disable-joystick \
	--disable-cdrom

make
sudo make install


wget http://www.libsdl.org/projects/SDL_net/release/SDL_net-1.2.8.tar.gz
tar -zvxf SDL_net-1.2.8.tar.gz
cd SDL_net-1.2.8

./configure

make
sudo make install


wget http://guichan.googlecode.com/files/guichan-0.8.2.tar.gz
tar -zvxf guichan-0.8.2.tar.gz
cd guichan-0.8.2

./configure \
	--disable-opengl \
	--disable-sdl-image \
	--disable-allegro \
	--disable-sdl

make
sudo make install
