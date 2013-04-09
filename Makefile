PLATFORM=linux


ifeq ($(PLATFORM),linux)
	CXX=g++
	CLIENT=chaoticrage
	SERVER=dedicatedserver
	SERVER_NOGUI=dedicatedserver-nogui
	ANIMVIEWER=animviewer
	
	CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse gl glu lua5.1 bullet assimp --cflags` `freetype-config --cflags` -DGETOPT -Werror -Wall -ggdb -Itools/include
	LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse lua5.1 bullet assimp --libs` `freetype-config --libs` -lGL -lGLU -lGLEW -lSDL_mixer -lSDL_image -lSDL_net -lguichan_sdl -lguichan_opengl -lguichan -L/usr/X11R6/lib -lX11
	
	CFLAGS_NOGUI=`sdl-config --cflags` `pkg-config zziplib libconfuse lua5.1 bullet assimp --cflags` -DGETOPT -Werror -Wall -ggdb
	LIBS_NOGUI=`sdl-config --libs` `pkg-config zziplib libconfuse lua5.1 bullet assimp --libs` -lSDL_image -lSDL_net -lguichan
endif

ifeq ($(PLATFORM),i386-mingw32)
	CXX=i386-mingw32-g++
	CLIENT=chaoticrage.exe
	SERVER=dedicatedserver.exe
	ANIMVIEWER=animviewer.exe
	CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse gl glu --cflags` -Werror -Wall -ggdb
	LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse --libs` -lopengl32 -lglu32 -lglew32 -lSDL_mixer -lSDL_image -lSDL_net
endif


OBJPATH=build
SRCPATH=src

CPPFILES=$(wildcard \
	$(SRCPATH)/*.cpp \
	$(SRCPATH)/audio/*.cpp \
	$(SRCPATH)/mod/*.cpp \
	$(SRCPATH)/net/*.cpp \
	$(SRCPATH)/render/*.cpp \
	$(SRCPATH)/util/*.cpp \
	$(SRCPATH)/lua/*.cpp \
	$(SRCPATH)/gui/*.cpp \
	$(SRCPATH)/http/*.cpp \
	$(SRCPATH)/weapons/*.cpp \
)

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(CPPFILES)) $(OBJPATH)/objload.o
OBJMAINS=build/server.o build/client.o build/animviewer.o

OBJFILES_CLIENT=build/client.o build/linux.o $(filter-out $(OBJMAINS), $(OBJFILES))
OBJFILES_SERVER=build/server.o build/linux.o $(filter-out $(OBJMAINS), $(OBJFILES))
OBJFILES_ANIMVIEWER=build/animviewer.o build/linux.o $(filter-out $(OBJMAINS), $(OBJFILES))


OBJGUI=build/gui/controls.o \
	build/gui/dialog.o \
	build/gui/network.o \
	build/gui/newgame.o \
	build/gui/settings.o \
	build/render/render_opengl.o \
	build/render/render_opengl_compat.o \
	build/render/render_debug.o \
	build/render/hud.o \
	build/audio/audio_sdlmixer.o \
	build/intro.o \
	build/menu.o \
	build/http/client_stats.o \
	build/util/clientconfig.o \
	build/events.o

OBJFILES_SERVER_NOGUI=build/server.o build/linux.o build/platform/nogui.o $(filter-out $(OBJMAINS) $(OBJGUI), $(OBJFILES))


default: all

.PHONY: all client clean

all: client server animviewer


client: $(OBJFILES_CLIENT)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_CLIENT) -o $(CLIENT) $(LIBS)
	
server: $(OBJFILES_SERVER)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_SERVER) -o $(SERVER) $(LIBS) 
	
server-nogui: $(OBJFILES_SERVER_NOGUI:%.o=%_nogui.o)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS_NOGUI) $(OBJFILES_SERVER_NOGUI:%.o=%_nogui.o) -o $(SERVER_NOGUI) $(LIBS_NOGUI)
	
animviewer: $(OBJFILES_ANIMVIEWER)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_ANIMVIEWER) -o $(ANIMVIEWER) $(LIBS)


clean:
	rm -f chaoticrage
	rm -f dedicatedserver
	rm -f dedicatedserver-nogui
	rm -f animviewer
	rm -f $(OBJFILES)
	rm -f $(OBJFILES:%.o=%_nogui.o)
	rm -f $(OBJPATH)/objload.cpp
	rm -f $(OBJPATH)/linux.o
	rm -f $(OBJPATH)/linux_nogui.o


$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@mkdir -p `dirname $< | sed "s/src/build/"`
	@$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJPATH)/%_nogui.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@mkdir -p `dirname $< | sed "s/src/build/"`
	@$(CXX) $(CFLAGS) -DNOGUI -o $@ -c $<

$(OBJPATH)/objload.o: $(SRCPATH)/util/objload.l $(SRCPATH)/util/obj.h Makefile
	@echo [FLEX] $(SRCPATH)/util/objload.l
	@flex -o $(OBJPATH)/objload.cpp $(SRCPATH)/util/objload.l
	@echo [CC] $(OBJPATH)/objload.cpp
	@$(CXX) $(CFLAGS) -o $(OBJPATH)/objload.o -c $(OBJPATH)/objload.cpp -Wno-unused-function -Wno-unused-variable -I src
	
$(OBJPATH)/happyhttp.o: $(SRCPATH)/http/happyhttp.cpp $(SRCPATH)/http/happyhttp.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -Wno-error -o $@ -c $< 
	
$(OBJPATH)/linux.o: $(SRCPATH)/platform/linux.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<


$(OBJPATH)/linux_nogui.o: $(SRCPATH)/platform/linux.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -DNOGUI -o $@ -c $<
	
$(OBJPATH)/objload_nogui.o: $(SRCPATH)/util/objload.l $(SRCPATH)/util/obj.h Makefile
	@echo [FLEX] $(SRCPATH)/util/objload.l
	@flex -o $(OBJPATH)/objload.cpp $(SRCPATH)/util/objload.l
	@echo [CC] $(OBJPATH)/objload.cpp
	@$(CXX) $(CFLAGS) -o $(OBJPATH)/objload_nogui.o -c $(OBJPATH)/objload.cpp -Wno-unused-function -Wno-unused-variable -I src


ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
