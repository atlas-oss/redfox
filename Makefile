CC=g++
CFLAGS= -std=c++14 -Os -Wall
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
