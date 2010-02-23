
CXX=g++
CFLAGS=`sdl-config --cflags` `pkg-config zziplib libconfuse --cflags` -Werror -Wall
LIBS=`sdl-config --libs` `pkg-config zziplib libconfuse --libs`

OBJPATH=build
SRCPATH=src

OBJFILES=$(patsubst $(SRCPATH)/%.cpp,$(OBJPATH)/%.o,$(wildcard $(SRCPATH)/*.cpp))


default: all

.PHONY: all client clean

all: client

client: $(OBJFILES)
	@echo -n [LINK] $@ ...\ 
	@$(CXX) $(CFLAGS) $(OBJFILES) -o chaoticrage $(LIBS) -ggdb 
	@echo  OK
	
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/rage.h Makefile
	@echo -n [CC] $< ...\ 
	@$(CXX) $(CFLAGS) -o $@ -c $<
	@echo  OK
	
clean:
	rm -f chaoticrage
	rm -f $(OBJFILES)
	
	
	
ifeq ($(wildcard $(OBJPATH)/),)
$(shell mkdir -p $(OBJPATH))
endif
