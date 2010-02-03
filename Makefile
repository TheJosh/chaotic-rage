all:
	g++ src/*.cpp `sdl-config --libs --cflags` -o chaoticrage
