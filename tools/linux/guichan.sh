#!/bin/bash


cd working

if [ ! -d "guichan-master" ]; then
	if [ ! -f "guichan-master.zip" ]; then
		wget https://github.com/TheJosh/guichan/archive/master.zip -O guichan-master.zip
	fi
	
	unzip -q guichan-master.zip || exit 1
	
	cd guichan-master
	patch -p1 -i ../../guichan.patch || exit 1
	cd ..
fi

cd guichan-master

./configure	--disable-allegro || exit 1

make || exit 1

sudo make install || exit 1

sudo ldconfig || exit 1
