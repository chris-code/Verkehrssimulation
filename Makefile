CC=g++
CFLAGS =-Wall -std=c++11 -g
#OPTFLAGS=-O2
OPTFLAGS=
#INCLUDES=-I$(PWD)
INCLUDES=
LIBS=-lpthread -lX11

#PROGRAMS=einspurig mehrspurig kreisverkehr
PROGRAMS=multilane roundabout

all:$(PROGRAMS)
.PHONY:$(PROGRAMS)

#einspurig:
#	$(CC) $(CFLAGS) src/mainEinspurig.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)
multilane:
	$(CC) $(CFLAGS) src/mainMultilane.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)
roundabout:
	$(CC) $(CFLAGS) src/mainRoundabout.cpp -o $@ $(INCLUDES) $(LIBS) $(OPTFLAGS)

clean:
	rm $(PROGRAMS) 2>/dev/null
