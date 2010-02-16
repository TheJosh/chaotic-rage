all: general
	
general:
	g++ src/*.cpp \
	`sdl-config --libs --cflags` `pkg-config zziplib libconfuse --libs --cflags` \
	-o chaoticrage -Werror -Wall -O2
	
debug:
	g++ src/*.cpp \
	`sdl-config --libs --cflags` `pkg-config zziplib libconfuse --libs --cflags` \
	-o chaoticrage -Werror -Wall -ggdb -DDEBUG_MODE
