CXX ?= g++
CC ?= gcc
AR ?= ar

OBJFILES = lapi.o lbitlib.o lctype.o ldo.o lgc.o llex.o loadlib.o loslib.o \
	lstring.o ltablib.o lvm.o lauxlib.o lcode.o ldblib.o ldump.o linit.o \
	lmathlib.o lobject.o lparser.o lstrlib.o ltm.o lzio.o lbaselib.o lcorolib.o \
	ldebug.o lfunc.o liolib.o lmem.o lopcodes.o lstate.o ltable.o lundump.o

CFLAGS = -O2 -ffast-math
LUA_CFLAGS = -DLUA_COMPAT_ALL -DLUA_USE_LINUX


all: ../liblua.a

../liblua.a: $(OBJFILES)
	$(AR) rs $@ $(OBJFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(LUA_CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm -f ../liblua.a
