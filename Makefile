# Makefile for Chaotic Rage


# If MXE is use, set cross compiler vars and PATH
# Use it like this:
#    make MXE=/path/to/mxe
ifdef MXE
	CROSS := i686-pc-mingw32-
	PATH := $(MXE)/usr/bin:$(PATH)
else
	CROSS :=
endif


# Set executables, with support for cross-compilers
CXX := $(CROSS)g++
CC := $(CROSS)gcc
PKG_CONFIG := $(CROSS)pkg-config
SDL2_CONFIG := $(CROSS)sdl2-config
FREETYPE_CONFIG := $(CROSS)freetype-config

# Cflags and Libs
CFLAGS := `$(SDL2_CONFIG) --cflags` `$(PKG_CONFIG) gl glu lua5.1 bullet assimp --cflags` `$(FREETYPE_CONFIG) --cflags` -DGETOPT -Werror -Wall -ggdb -Itools/include -Isrc -Isrc/guichan -Isrc/confuse -Isrc/spark/include
LIBS := `$(SDL2_CONFIG) --libs` `$(PKG_CONFIG) lua5.1 bullet assimp --libs` `$(FREETYPE_CONFIG) --libs` -lGL -lGLU -lGLEW -lSDL2_mixer -lSDL2_image -lSDL2_net -L/usr/X11R6/lib -lX11

# Extract the version from rage.h
# Only used for releases
VERSION := $(shell grep -E --only-matching 'VERSION ".+"' src/rage.h | sed -n 1p | sed "s/VERSION //" | sed 's/"//g')
DISTTMP := chaoticrage-$(VERSION)


OBJPATH=build
SRCPATH=src

CPPFILES=$(wildcard \
	$(SRCPATH)/*.cpp \
	$(SRCPATH)/audio/*.cpp \
	$(SRCPATH)/mod/*.cpp \
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
	$(SRCPATH)/guichan/*.cpp \
	$(SRCPATH)/guichan/opengl/*.cpp \
	$(SRCPATH)/guichan/sdl/*.cpp \
	$(SRCPATH)/guichan/widgets/*.cpp \
	$(SRCPATH)/spark/src/*.cpp \
)

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(CPPFILES))
OBJMAINS=build/client.o

OBJFILES_CLIENT=build/client.o build/linux.o build/confuse/confuse.o build/confuse/lexer.o $(filter-out $(OBJMAINS), $(OBJFILES))


default: chaoticrage
.PHONY: all chaoticrage clean
all: chaoticrage


chaoticrage: $(OBJFILES_CLIENT)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_CLIENT) -o chaoticrage $(LIBS)


install: chaoticrage
	mkdir -p $(DESTPATH)/usr/bin
	install chaoticrage $(DESTPATH)/usr/bin
	
	mkdir -p $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership data $(DESTPATH)/usr/share/chaoticrage
	cp -r --no-preserve=ownership maps $(DESTPATH)/usr/share/chaoticrage


dist: src data maps
	mkdir -p $(DISTTMP)
	
	cp -r Makefile $(DISTTMP)
	cp -r LICENSE $(DISTTMP)
	cp -r README.md $(DISTTMP)
	cp -r src $(DISTTMP)
	cp -r data $(DISTTMP)
	cp -r maps $(DISTTMP)
	
	mkdir -p $(DISTTMP)/tools
	mkdir -p $(DISTTMP)/tools/linux
	mkdir -p $(DISTTMP)/tools/linux/working
	
	cp -r tools/include $(DISTTMP)/tools
	cp tools/linux/*.sh $(DISTTMP)/tools/linux/
	chmod 755 $(DISTTMP)/tools/linux/*.sh
	
	tar -cvjf chaoticrage-linux-$(VERSION).tar.bz2 $(DISTTMP)
	rm -rf $(DISTTMP)


dist-bin: chaoticrage data maps
	mkdir -p $(DISTTMP)
	
	cp -r LICENSE $(DISTTMP)
	cp -r README.md $(DISTTMP)
	cp -r chaoticrage $(DISTTMP)
	cp -r data $(DISTTMP)
	cp -r maps $(DISTTMP)
	
	tar -cvjf chaoticrage-linuxbin-$(VERSION).tar.bz2 $(DISTTMP)
	rm -rf $(DISTTMP)


clean:
	rm -f chaoticrage
	rm -f $(OBJFILES)
	rm -f $(OBJPATH)/linux.o


$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@mkdir -p `dirname $< | sed "s/src/build/"`
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



ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
