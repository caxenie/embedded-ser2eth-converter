CC = gcc
CFLAGS = -g -Wall
INCLUDE=-I/inc/
SOURCES=src/s2e-tester.c

s2e-tester_OBJECTS= s2e-tester.o

OBJECTS= $(s2e-tester_OBJECTS)

PROGRAMS= s2e-tester


all:	$(PROGRAMS)

s2e-tester:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROGRAMS) $(SOURCES)

clean:
	rm -fr *.o s2e-tester
