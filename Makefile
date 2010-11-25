PLATFORM=linux


ifeq ($(PLATFORM),linux)
	CXX=g++
	CLIENT=chaoticrage
	CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse gl glu --cflags` -Werror -Wall -ggdb
	LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse --libs` -lGL -lGLU -lSDL_mixer -lSDL_image
endif

ifeq ($(PLATFORM),i386-mingw32)
	CXX=i386-mingw32-g++
	CLIENT=chaoticrage.exe
	CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse gl glu --cflags` -Werror -Wall -ggdb
	LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse --libs` -lopengl32 -lglu32 -lSDL_mixer -lSDL_image
endif


OBJPATH=build
SRCPATH=src

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(wildcard $(SRCPATH)/*.cpp)) $(OBJPATH)/objload.o


default: all

.PHONY: all client clean

all: client

client: $(OBJFILES)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES) -o $(CLIENT) $(LIBS) -ggdb 
	
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f chaoticrage chaoticrage.exe 
	rm -f $(OBJFILES)
	rm -f $(OBJPATH)/objload.cpp
	
	
$(OBJPATH)/objload.o: $(SRCPATH)/objload.l $(SRCPATH)/objload.h Makefile
	@echo [FLEX] $(SRCPATH)/objload.l
	@flex -o $(OBJPATH)/objload.cpp $(SRCPATH)/objload.l
	@echo [CC] $(OBJPATH)/objload.cpp
	@$(CXX) $(CFLAGS) -o $(OBJPATH)/objload.o -c $(OBJPATH)/objload.cpp -Wno-unused-function -Wno-unused-variable -I src
	
	
	
	
ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
