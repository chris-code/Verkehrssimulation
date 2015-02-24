CC=g++
CFLAGS =-Wall -std=c++11 -g
#OPTFLAGS=-O2
OPTFLAGS=
#INCLUDES=-I$(PWD)
INCLUDES=
LIBS=-lpthread -lX11

PROGRAMS=einspurig mehrspurig

all:$(PROGRAMS)
.PHONY:$(PROGRAMS)

einspurig:
	$(CC) $(CFLAGS) src/mainEinspurig.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)
mehrspurig:
	$(CC) $(CFLAGS) src/mainMehrspurig.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)

clean:
	rm $(PROGRAMS) 2>/dev/null
