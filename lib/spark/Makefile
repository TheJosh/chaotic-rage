CXX ?= g++
CC ?= gcc
AR ?= ar

# List of source files is determined using a wildcard
CPPFILES=$(wildcard *.cpp \
	Core/*.cpp \
	Extensions/Emitters/*.cpp \
	Extensions/Modifiers/*.cpp \
	Extensions/Renderers/*.cpp \
	Extensions/Zones/*.cpp \
	RenderingAPIs/OpenGL/*.cpp \
)
OBJFILES=$(patsubst %.cpp, %.o, $(CPPFILES))

CFLAGS = -Werror -Wall -MMD -O2 -ffast-math \
	-I. \
	$(shell pkg-config gl glu --cflags)


all: ../libspark.a

../libspark.a: $(OBJFILES)
	$(AR) rs $@ $(OBJFILES)

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJFILES)
	rm -f ../libspark.a
