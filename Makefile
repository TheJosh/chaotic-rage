# Makefile for Chaotic Rage

# Build dir and sources dir
OBJPATH=build
SRCPATH=src
LUAPKG=lua5.1


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
	LUAPKG := lua
	POSTFIX := .exe


# emscripten llvm to javascript compiler
# Use it like this:
#    make EMSCRIPTEN=1
else ifdef EMSCRIPTEN
	CXX := em++
	CC := emcc
	PLATFORM := $(OBJPATH)/emscripten.o


# Standard Linux build
else
	CXX ?= g++
	CC ?= gcc
	CFLAGS := -DGETOPT -Werror -Wall -MMD
	LIBS := -lGL -lGLU -L/usr/X11R6/lib -lX11 -lm -lstdc++
	PLATFORM := $(OBJPATH)/linux.o
endif


# Debug vs optimised
ifdef DEBUG
	CFLAGS := $(CFLAGS) -Og -ggdb
else
	CFLAGS := $(CFLAGS) -O2 -ffast-math
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
	$(shell export PATH=$(PATH);$(PKG_CONFIG) gl glu glew $(LUAPKG) bullet assimp SDL2_mixer SDL2_image SDL2_net --cflags) \
	$(shell export PATH=$(PATH);$(FREETYPE_CONFIG) --cflags) \
	$(CFLAGS) \
	-Itools/include -I$(SRCPATH) -I$(SRCPATH)/guichan -I$(SRCPATH)/confuse -I$(SRCPATH)/spark

# libs
LIBS := $(shell export PATH=$(PATH);$(SDL2_CONFIG) --libs) \
	$(shell export PATH=$(PATH);$(PKG_CONFIG) glew $(LUAPKG) bullet assimp SDL2_mixer SDL2_image SDL2_net --libs) \
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
	$(SRCPATH)/lua/*.cpp \
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

# The list of source files gets transformed into a list of obj files
OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(CPPFILES))

# Files which define main() methods
OBJMAINS=$(OBJPATH)/client.o

# Client = everything but the main() method files + some other bits
OBJFILES_CLIENT=$(OBJPATH)/client.o $(PLATFORM) $(OBJPATH)/confuse/confuse.o $(OBJPATH)/confuse/lexer.o $(filter-out $(OBJMAINS), $(OBJFILES))

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


chaoticrage: $(OBJFILES_CLIENT)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_CLIENT) -o chaoticrage$(POSTFIX) $(LIBS)


install: chaoticrage
	mkdir -p $(DESTPATH)/usr/bin
	install chaoticrage $(DESTPATH)/usr/bin

	mkdir -p $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership data $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership maps $(DESTPATH)/usr/share/chaoticrage


dist: $(SRCPATH) data maps
	rm -rf $(DISTTMP)
	mkdir $(DISTTMP)

	cp Makefile $(DISTTMP)
	cp LICENSE $(DISTTMP)
	cp README.md $(DISTTMP)
	cp COMPILE.md $(DISTTMP)
	cp -r $(SRCPATH) $(DISTTMP)
	cp -r data $(DISTTMP)
	cp -r maps $(DISTTMP)

	mkdir -p $(DISTTMP)/tools
	mkdir -p $(DISTTMP)/tools/i18n
	mkdir -p $(DISTTMP)/tools/linux
	mkdir -p $(DISTTMP)/tools/linux/working

	cp -r tools/include $(DISTTMP)/tools
	cp -r tools/i18n $(DISTTMP)/tools
	cp tools/linux/*.sh $(DISTTMP)/tools/linux/
	chmod 755 $(DISTTMP)/tools/linux/*.sh

	tar -cvjf chaoticrage-linux-$(VERSION).tar.bz2 $(DISTTMP)

	mkdir -p $(DISTTMP)/debian
	cp -r tools/debian_package/debian $(DISTTMP)
	cp tools/debian_package/chaoticrage.desktop $(DISTTMP)
	cp orig/2d/game_icon.PNG $(DISTTMP)/chaoticrage.png
	tar -cvJf chaoticrage_$(VERSION).orig.tar.xz $(DISTTMP)
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


clean:
	rm -f chaoticrage
	rm -f $(OBJFILES)
	rm -f $(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.d,$(CPPFILES))
	rm -f $(OBJPATH)/linux.o $(OBJPATH)/linux.d
	rm -f $(OBJPATH)/client.o $(OBJPATH)/client.d
	rm -f $(OBJPATH)/emscripten.o $(OBJPATH)/emscripten.d
	rm -f $(OBJPATH)/win32.o $(OBJPATH)/win32.d
	rm -f $(OBJPATH)/confuse/confuse.o $(OBJPATH)/confuse/confuse.d
	rm -f $(OBJPATH)/confuse/lexer.o $(OBJPATH)/confuse/lexer.d


cleaner:
	rm -f chaoticrage
	rm -rf $(OBJPATH)/


$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@mkdir -p `dirname $< | sed "s/$(SRCPATH)/$(OBJPATH)/"`
	@$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJPATH)/happyhttp.o: $(SRCPATH)/http/happyhttp.cpp $(SRCPATH)/http/happyhttp.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -Wno-error -o $@ -c $<

$(OBJPATH)/confuse/%.o: $(SRCPATH)/confuse/%.c $(SRCPATH)/confuse/confuse.h Makefile
	@echo [CC] $<
	@mkdir -p $(OBJPATH)/confuse
	@$(CC) $(CFLAGS) -Wno-error -o $@ -c $<

$(OBJPATH)/linux.o: $(SRCPATH)/platform/linux.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJPATH)/win32.o: $(SRCPATH)/platform/win32.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJPATH)/emscripten.o: $(SRCPATH)/platform/emscripten.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<


ifeq ($(wildcard $(OBJPATH)/),)
	$(shell mkdir -p $(OBJPATH))
endif
