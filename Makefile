CC=g++
CFLAGS =-Wall -std=c++11 -g
#OPTFLAGS=-O2
OPTFLAGS=
#INCLUDES=-I$(PWD)
INCLUDES=
LIBS=-lpthread -lX11

PROGRAMS=bin/multilane bin/streetmap

all:$(PROGRAMS)
.PHONY:$(PROGRAMS)

bin/multilane:
	$(CC) $(CFLAGS) src/multilaneMain.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)
bin/streetmap:
	$(CC) $(CFLAGS) src/streetmapMain.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)

clean:
	rm $(PROGRAMS) 2>/dev/null
