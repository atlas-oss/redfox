CC=clang++

WARN= -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -Wformat-security -Wformat -Werror=format-security -Wstack-protector
SEC= -march=native -fstack-protector-all --param ssp-buffer-size=4 -fpie -ftrapv -D_FORTIFY_SOURCE=2

CFLAGS= ${WARN} ${SEC} -I/usr/local/include -std=c++14 -O2 -fomit-frame-pointer -funroll-loops
LDFLAGS= -L/usr/local/lib/ -lX11 -lasound -Wl,-z,relro,-z,now -pie

MU := $(shell which mu)

ifdef MU
CFLAGS+= -DFOUND_MU
LDFLAGS+= -lxapian
endif

SRC= src/redfox.cpp main.cpp
OBJECT= redfox.o main.o

all: _bar_
	@echo "Linking..."
	@${CC} ${OBJECT} -g -o redfox ${LDFLAGS}

_bar_:
	@echo "Compiling..."
	@${CC} -c ${CFLAGS} ${SRC}

.PHONY: clean

clean:
	@echo "Cleaning..."
	@rm -rf *.o *~ redfox
