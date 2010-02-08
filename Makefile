all:
	g++ src/*.cpp \
	`sdl-config --libs --cflags` `pkg-config zziplib libconfuse --libs --cflags` \
	-o chaoticrage -Werror -Wall
