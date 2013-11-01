#
#  Helper Makefile for downloading and installing libraries we need for CR.
#


# Bullet Physics
#
bullet-2.81-rev2613:
	rm -rf bullet-2.81-rev2613.tgz
	wget http://chaoticrage.com/dev_libs/bullet-2.81-rev2613.tgz
	tar -zvxf bullet-2.81-rev2613.tgz
	rm bullet-2.81-rev2613.tgz

bullet-build: bullet-2.81-rev2613
	mkdir -p bullet-build
	
	cd bullet-build; cmake ../bullet-2.81-rev2613 -G "Unix Makefiles" \
		-DINSTALL_LIBS=ON \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DBUILD_DEMOS:BOOL=OFF \
		-DBUILD_CPU_DEMOS:BOOL=OFF \
		-DBUILD_EXTRAS:BOOL=OFF
	
	cd bullet-build; make -j4

bullet-install: bullet-build
	echo "We are about to a 'sudo make install'."
	echo "You will be asked your password."
	cd bullet-build; sudo make install
	sudo ldconfig
	
bullet: bullet-install

bullet-clean:
	rm -rf bullet-build



# Assimp
#
assimp--3.0.1270-source-only:
	rm -rf assimp--3.0.1270-source-only
	wget http://chaoticrage.com/dev_libs/assimp--3.0.1270-source-only.zip
	unzip assimp--3.0.1270-source-only.zip
	rm assimp--3.0.1270-source-only.zip
	
assimp-build: assimp--3.0.1270-source-only
	mkdir -p assimp-build
	
	cd assimp-build; cmake ../assimp--3.0.1270-source-only -G "Unix Makefiles" \
		-DBUILD_STATIC_LIB=ON \
		-DBUILD_ASSIMP_TOOLS=OFF \
		-DBUILD_ASSIMP_SAMPLES=OFF \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DENABLE_BOOST_WORKAROUND=ON
	
	cd assimp-build; make -j4

assimp-install: assimp-build
	echo "We are about to a 'sudo make install'."
	echo "You will be asked your password."
	cd assimp-build; sudo make install
	sudo ldconfig

assimp: assimp-install

assimp-clean:
	rm -rf assimp-build


