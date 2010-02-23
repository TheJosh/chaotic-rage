
CXX=g++
CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse --cflags` -Werror -Wall -ggdb
LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse --libs`

OBJPATH=build
SRCPATH=src

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(wildcard $(SRCPATH)/*.cpp))


default: all

.PHONY: all client clean

all: client

client: $(OBJFILES)
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) $(OBJFILES) -o chaoticrage $(LIBS) -ggdb 
	
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f chaoticrage
	rm -f $(OBJFILES)
	
	
	
ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
