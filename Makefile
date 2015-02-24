CC=g++
CFLAGS =-Wall -std=c++11 -g
OPTFLAGS=-O2
INCLUDES=-I$(PWD)
LIBS=-lpthread -lX11

PROGRAMS=einspurig mehrspurig

all:$(PROGRAMS)
.PHONY:einspurig mehrspurig

einspurig:
	$(CC) $(CFLAGS) src/mainEinspurig.cpp -o $@ src/Grid.hpp src/StreetMap.hpp src/StreetSegment.hpp $(INCLUDES) $(LIBS) $(OPTFLAGS)
mehrspurig:
	$(CC) $(CFLAGS) src/mainMehrspurig.cpp -o $@ src/Road.hpp src/Vehicle.hpp src/Simulation.hpp $(INCLUDES) $(LIBS) $(OPTFLAGS)

clean:
	rm $(PROGRAMS) 2>/dev/null
