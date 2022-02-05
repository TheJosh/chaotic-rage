CXX ?= g++
CC ?= gcc
AR ?= ar

OBJFILES = confuse.o lexer.o
CFLAGS = -O2


all: ../libconfuse.a

../libconfuse.a: $(OBJFILES)
	$(AR) rs $@ $(OBJFILES)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm -f ../libconfuse.a
