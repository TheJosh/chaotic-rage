PLATFORM=linux


ifeq ($(PLATFORM),linux)
	CXX=g++
	CLIENT=chaoticrage
	SERVER=dedicatedserver
	ANIMVIEWER=animviewer
	CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse gl glu lua5.1 bullet --cflags` `freetype-config --cflags` -Werror -Wall -ggdb
	LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse lua5.1 bullet --libs` `freetype-config --libs` -lGL -lGLU -lGLEW -lSDL_mixer -lSDL_image -lSDL_net
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

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(wildcard $(SRCPATH)/*.cpp)) $(OBJPATH)/objload.o $(OBJPATH)/linux.o
OBJMAINS=build/server.o build/client.o build/animviewer.o

OBJFILES_CLIENT=build/client.o $(filter-out $(OBJMAINS), $(OBJFILES))
OBJFILES_SERVER=build/server.o $(filter-out $(OBJMAINS), $(OBJFILES))
OBJFILES_ANIMVIEWER=build/animviewer.o $(filter-out $(OBJMAINS), $(OBJFILES))


default: all

.PHONY: all client clean

all: client server animviewer

client: $(OBJFILES_CLIENT)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_CLIENT) -o $(CLIENT) $(LIBS) -ggdb 
	
server: $(OBJFILES_SERVER)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_SERVER) -o $(SERVER) $(LIBS) -ggdb 
	
animviewer: $(OBJFILES_ANIMVIEWER)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES_ANIMVIEWER) -o $(ANIMVIEWER) $(LIBS) -ggdb 
	
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f chaoticrage chaoticrage.exe
	rm -f dedicatedserver dedicatedserver.exe 
	rm -f $(OBJFILES)
	rm -f $(OBJPATH)/objload.cpp
	
$(OBJPATH)/objload.o: $(SRCPATH)/objload.l $(SRCPATH)/objload.h Makefile
	@echo [FLEX] $(SRCPATH)/objload.l
	@flex -o $(OBJPATH)/objload.cpp $(SRCPATH)/objload.l
	@echo [CC] $(OBJPATH)/objload.cpp
	@$(CXX) $(CFLAGS) -o $(OBJPATH)/objload.o -c $(OBJPATH)/objload.cpp -Wno-unused-function -Wno-unused-variable -I src
	
$(OBJPATH)/linux.o: $(SRCPATH)/platform/linux.cpp $(SRCPATH)/platform/platform.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
	
	
ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
