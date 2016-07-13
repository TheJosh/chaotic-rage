# Makefile for Chaotic Rage

# Build dir and sources dir
OBJPATH=build
SRCPATH=src

# Verbose vs quiet builds
ifdef VERBOSE
	Q =
else
	Q = @
endif


# MXE cross-compiler environment
# Use it like this:
#    make MXE=/path/to/mxe
ifdef MXE
	CROSS := i686-w64-mingw32.static-
	CXX := $(CROSS)g++
	CC := $(CROSS)gcc
	PATH := $(MXE)/usr/bin:$(PATH)
	PLATFORM := $(OBJPATH)/win32.o
	LIBS := -liphlpapi
	POSTFIX := .exe
	DONT_COMPILE = $(OBJPATH)/touch.o \
		$(OBJPATH)/gui/controls_touch.o
	LUA_CFLAGS := -DLUA_COMPAT_ALL

# Standard Linux build
else
	CXX ?= g++
	CC ?= gcc
	CFLAGS := -DGETOPT -Werror -Wall -MMD
	LIBS := -lGL -lGLU -L/usr/X11R6/lib -lX11 -lfontconfig -lm -lstdc++
	PLATFORM := $(OBJPATH)/linux.o
	DONT_COMPILE = $(OBJPATH)/touch.o \
		$(OBJPATH)/gui/controls_touch.o
	LUA_CFLAGS := -DLUA_COMPAT_ALL -DLUA_USE_LINUX
endif


# Debug vs optimised
ifdef DEBUG
	CFLAGS := $(CFLAGS) -Og -ggdb
else
	CFLAGS := $(CFLAGS) -O2 -g -ffast-math
endif


# Both Bullet and GLM generate tons of warnings on clang
ifeq ($(CXX),clang)
	CFLAGS := $(CFLAGS) -Wno-unknown-warning-option -Wno-overloaded-virtual -Wno-shift-op-parentheses
endif


# Set other executables, with support for cross-compilers
PKG_CONFIG := $(CROSS)pkg-config
SDL2_CONFIG := $(CROSS)sdl2-config
FREETYPE_CONFIG := $(CROSS)freetype-config

# cflags
CFLAGS := $(shell export PATH=$(PATH);$(SDL2_CONFIG) --cflags) \
	$(shell export PATH=$(PATH) PKG_CONFIG_PATH=$(PKG_CONFIG_PATH);$(PKG_CONFIG) gl glu glew bullet assimp SDL2_mixer SDL2_image SDL2_net --cflags) \
	$(shell export PATH=$(PATH);$(FREETYPE_CONFIG) --cflags) \
	$(CFLAGS) \
	-Itools/include -I$(SRCPATH) -I$(SRCPATH)/guichan -I$(SRCPATH)/confuse -I$(SRCPATH)/spark -I$(SRCPATH)/lua

# libs
LIBS := $(shell export PATH=$(PATH);$(SDL2_CONFIG) --libs) \
	$(shell export PATH=$(PATH) PKG_CONFIG_PATH=$(PKG_CONFIG_PATH);$(PKG_CONFIG) glew bullet assimp SDL2_mixer SDL2_image SDL2_net --libs) \
	$(shell export PATH=$(PATH);$(FREETYPE_CONFIG) --libs) \
	$(LIBS)


# Extract the version from rage.h
# Only used for releases
ifndef VERSION
	VERSION := $(shell grep -E -o 'VERSION ".+"' $(SRCPATH)/rage.h | sed -n 1p | sed "s/VERSION //" | sed 's/"//g')
endif
DISTTMP := chaoticrage-$(VERSION)

# Complete list of source files
CPPFILES=$(wildcard \
	$(SRCPATH)/*.cpp \
	$(SRCPATH)/audio/*.cpp \
	$(SRCPATH)/mod/*.cpp \
	$(SRCPATH)/map/*.cpp \
	$(SRCPATH)/net/*.cpp \
	$(SRCPATH)/render/*.cpp \
	$(SRCPATH)/render_opengl/*.cpp \
	$(SRCPATH)/entity/*.cpp \
	$(SRCPATH)/fx/*.cpp \
	$(SRCPATH)/util/*.cpp \
	$(SRCPATH)/script/*.cpp \
	$(SRCPATH)/gui/*.cpp \
	$(SRCPATH)/http/*.cpp \
	$(SRCPATH)/weapons/*.cpp \
	$(SRCPATH)/i18n/*.cpp \
	$(SRCPATH)/guichan/*.cpp \
	$(SRCPATH)/guichan/opengl/*.cpp \
	$(SRCPATH)/guichan/sdl/*.cpp \
	$(SRCPATH)/guichan/widgets/*.cpp \
	$(SRCPATH)/spark/Core/*.cpp \
	$(SRCPATH)/spark/Extensions/Emitters/*.cpp \
	$(SRCPATH)/spark/Extensions/Modifiers/*.cpp \
	$(SRCPATH)/spark/Extensions/Renderers/*.cpp \
	$(SRCPATH)/spark/Extensions/Zones/*.cpp \
	$(SRCPATH)/spark/RenderingAPIs/OpenGL/*.cpp \
)

CFILES=$(wildcard \
	$(SRCPATH)/lua/*.c \
)

# The list of source files gets transformed into a list of obj files
OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(CPPFILES)) $(patsubst $(SRCPATH)/%.c,$(OBJPATH)/%.o,$(CFILES))

# Files which define main() methods
OBJMAINS=$(OBJPATH)/client.o

# Client = everything but the main() method files + some other bits
OBJFILES_CLIENT=$(OBJPATH)/client.o $(PLATFORM) $(OBJPATH)/confuse/confuse.o $(OBJPATH)/confuse/lexer.o $(filter-out $(OBJMAINS) $(DONT_COMPILE), $(OBJFILES))

# Dependencies of the source files
DEPENDENCIES := $(OBJFILES:.o=.d)

# DESTDIR is used by debuild/dpkg-buildpackage
ifdef DESTDIR
	DESTPATH := DESTDIR
endif


default: chaoticrage
.PHONY: all chaoticrage clean
all: chaoticrage

# Include directive for dependencies of the source files
-include $(DEPENDENCIES)


help:		## This help dialog
	@fgrep -h "##" Makefile | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'


chaoticrage: $(OBJFILES_CLIENT)
	@echo [LINK] $@
	$(Q)$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJFILES_CLIENT) -o chaoticrage$(POSTFIX) $(LIBS)


test: chaoticrage
	./chaoticrage --mod test --arcade generaltest,pound,robot --render null --audio null


install:	## Install game in Linux environment
install: chaoticrage
	mkdir -p $(DESTPATH)/usr/games
	install chaoticrage $(DESTPATH)/usr/games

	mkdir -p $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership data $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership maps $(DESTPATH)/usr/share/chaoticrage

	mkdir -p $(DESTPATH)/usr/share/applications
	cp -r --no-preserve=ownership tools/linux/install/chaoticrage.desktop $(DESTPATH)/usr/share/applications

	mkdir -p $(DESTPATH)/usr/share/icons
	cp -r --no-preserve=ownership tools/linux/install/chaoticrage.png $(DESTPATH)/usr/share/icons
	
	mkdir -p $(DESTPATH)/usr/local/man/man1
	cp -r --no-preserve=ownership tools/linux/install/chaoticrage.1 $(DESTPATH)/usr/local/man/man1
	gzip $(DESTPATH)/usr/local/man/man1/chaoticrage.1

uninstall:	## Uninstall game in Linux environment
	rm -f $(DESTPATH)/usr/games/chaoticrage
	rm -fr $(DESTPATH)/usr/share/chaoticrage
	rm -f $(DESTPATH)/usr/share/applications/chaoticrage.desktop
	rm -f $(DESTPATH)/usr/share/icons/chaoticrage.png
	rm -f $(DESTPATH)/usr/local/man/man1/chaoticrage.1.gz

dist: $(SRCPATH) data maps
	rm -rf $(DISTTMP)
	mkdir $(DISTTMP)

	cp Makefile $(DISTTMP)
	cp LICENSE $(DISTTMP)
	cp README.md $(DISTTMP)
	cp COMPILE.md $(DISTTMP)
	cp CHANGES $(DISTTMP)
	cp -r $(SRCPATH) $(DISTTMP)
	cp -r data $(DISTTMP)
	cp -r maps $(DISTTMP)

	rm $(DISTTMP)/data/cr/*.ttf

	mkdir -p $(DISTTMP)/tools
	mkdir -p $(DISTTMP)/tools/i18n
	mkdir -p $(DISTTMP)/tools/linux
	mkdir -p $(DISTTMP)/tools/linux/working

	cp -r tools/include $(DISTTMP)/tools
	cp -r tools/i18n $(DISTTMP)/tools
	cp tools/linux/*.sh $(DISTTMP)/tools/linux/
	cp -r tools/linux/install $(DISTTMP)/tools/linux/
	chmod 755 $(DISTTMP)/tools/linux/*.sh

	sed -i "s/\"git\"/\"$(VERSION)\"/" $(DISTTMP)/tools/linux/install/chaoticrage.1

	tar -cvjf chaoticrage-linux-$(VERSION).tar.bz2 $(DISTTMP)

	rm -r $(DISTTMP)

chaoticrage-linux-$(VERSION).tar.bz2: dist

deb:		## Create signed Debian package
deb: chaoticrage-linux-$(VERSION).tar.bz2
	mv chaoticrage-linux-$(VERSION).tar.bz2 chaoticrage_$(VERSION).orig.tar.bz2
	tar -xf chaoticrage_$(VERSION).orig.tar.bz2
	mkdir $(DISTTMP)/debian
	cp -r tools/debian_package/debian/* $(DISTTMP)/debian/
	cd $(DISTTMP); debuild
	rm -r $(DISTTMP)

deb-ubuntu-ppa:	## Create signed Ubuntu package for PPA
deb-ubuntu-ppa: chaoticrage-linux-$(VERSION).tar.bz2
	mv chaoticrage-linux-$(VERSION).tar.bz2 chaoticrage_$(VERSION)ubuntu.orig.tar.bz2
	tar -xf chaoticrage_$(VERSION)ubuntu.orig.tar.bz2
	mkdir $(DISTTMP)/debian
	cp -r tools/debian_package/debian/* $(DISTTMP)/debian/
	cp -r tools/debian_package/ubuntu/* $(DISTTMP)/debian/
	cd $(DISTTMP); debuild -S -sa
	rm -r $(DISTTMP)


dist-bin: chaoticrage data maps
	rm -rf $(DISTTMP)
	mkdir $(DISTTMP)

	cp LICENSE $(DISTTMP)
	cp README.md $(DISTTMP)
	cp chaoticrage $(DISTTMP)
	cp -r data $(DISTTMP)
	cp -r maps $(DISTTMP)

	tar -cvjf chaoticrage-linuxbin-$(VERSION).tar.bz2 $(DISTTMP)
	rm -r $(DISTTMP)


env:		## Print the environment
	@echo $(CC)
	@echo $(CXX)
	@echo $(CFLAGS)
	@echo $(LIBS)

clean:		## Clean, removes generated build files
	rm -f chaoticrage
	rm -f $(OBJFILES)
	rm -f $(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.d,$(CPPFILES))
	rm -f $(patsubst $(SRCPATH)/%.c,$(OBJPATH)/%.d,$(CFILES))
	rm -f $(OBJPATH)/linux.o $(OBJPATH)/linux.d
	rm -f $(OBJPATH)/client.o $(OBJPATH)/client.d
	rm -f $(OBJPATH)/win32.o $(OBJPATH)/win32.d
	rm -f $(OBJPATH)/confuse/confuse.o $(OBJPATH)/confuse/confuse.d
	rm -f $(OBJPATH)/confuse/lexer.o $(OBJPATH)/confuse/lexer.d

cleaner:	## Clean, removes the entire build folder
	rm -f chaoticrage
	rm -rf $(OBJPATH)/


$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CXX] $<
	$(Q)mkdir -p `dirname $< | sed "s/$(SRCPATH)/$(OBJPATH)/"`
	$(Q)$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJPATH)/happyhttp.o: $(SRCPATH)/http/happyhttp.cpp $(SRCPATH)/http/happyhttp.h Makefile
	@echo [CXX] $<
	$(Q)$(CXX) $(CFLAGS) -Wno-error -o $@ -c $<

$(OBJPATH)/confuse/%.o: $(SRCPATH)/confuse/%.c $(SRCPATH)/confuse/confuse.h Makefile
	@echo [CC] $<
	$(Q)mkdir -p $(OBJPATH)/confuse
	$(Q)$(CC) $(CFLAGS) -Wno-error -o $@ -c $<

$(OBJPATH)/lua/%.o: $(SRCPATH)/lua/%.c $(SRCPATH)/lua/lua.h Makefile
	@echo [CC] $<
	$(Q)mkdir -p $(OBJPATH)/lua
	$(Q)$(CC) $(CFLAGS) $(LUA_CFLAGS) -o $@ -c $<

$(OBJPATH)/linux.o: $(SRCPATH)/platform/linux.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CXX] $<
	$(Q)$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJPATH)/win32.o: $(SRCPATH)/platform/win32.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CXX] $<
	$(Q)$(CXX) $(CFLAGS) -o $@ -c $<



ifeq ($(wildcard $(OBJPATH)/),)
	$(shell mkdir -p $(OBJPATH))
endif
