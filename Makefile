CC=g++
CFLAGS= -std=c++14 -O2 -Wall -fomit-frame-pointer -funroll-loops
LDFLAGS= -lX11

SRC= src/redfox.cpp main.cpp
OBJECT= redfox.o main.o

all: _bar_
	${CC} ${OBJECT} -o redfox ${LDFLAGS}

_bar_:
	${CC} -c ${CFLAGS} ${SRC}

.PHONY: clean

clean:
	rm -rf *.o *~ redfox
