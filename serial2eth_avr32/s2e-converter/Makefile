CC = /home/haustiq/development/buildroot-2010.11/output/staging/usr/bin/avr32-linux-gcc
CFLAGS = -g -Wall
INCLUDE=-I/inc/
SOURCES=src/s2e-converter.c

s2e-converter_OBJECTS= s2e-converter.o

OBJECTS= $(s2e-converter_OBJECTS)

PROGRAMS= s2e-converter

all:	$(PROGRAMS)

s2e-converter:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROGRAMS) $(SOURCES)

clean:
	rm -fr *.o s2e-converter
