CXX ?= g++
CC ?= gcc
AR ?= ar

# List of source files is determined using a wildcard
CPPFILES=$(wildcard *.cpp opengl/*.cpp sdl/*.cpp widgets/*.cpp )
OBJFILES=$(patsubst %.cpp, %.o, $(CPPFILES))

CFLAGS = -Werror -Wall -MMD -O2 -ffast-math -I..

CFLAGS := $(CFLAGS) $(shell $(CROSS)sdl2-config --cflags)


all: ../libguichan.a

../libguichan.a: $(OBJFILES)
	$(AR) rs $@ $(OBJFILES)

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm -f ../libguichan.a
