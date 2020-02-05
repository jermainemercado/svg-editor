UNAME := $(shell uname)
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L./bin
XMLFLAGS = `xml2-config --cflags --libs`
IDIR = ./includes/
SRCDIR = ./src/
BINDIR = ./bin/


ifeq ($(UNAME), Linux)
	INC_PATH = /usr/include/libxml2
endif
ifeq ($(UNAME), Darwin)
	INC_PATH = /System/Volumes/Data/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2
endif




#all: LinkedListAPI.o liblist.so xmlExample StructListDemo.o StructListDemo
#	
#StructListDemo: $(BINDIR)StructListDemo.o $(BINDIR)liblist.so
#	$(CC) $(CFLAGS) $(LDFLAGS) -I$(INC_PATH) -I$(IDIR) -llist $(BINDIR)StructListDemo.o  -o $(BINDIR)StructListDemo 
#	
LinkedListAPI.o: $(SRCDIR)LinkedListAPI.c $(IDIR)LinkedListAPI.h
	$(CC) $(CFLAGS) -I$(IDIR) -c -fpic $(SRCDIR)LinkedListAPI.c -o $(BINDIR)LinkedListAPI.o
#
liblist.so: $(BINDIR)LinkedListAPI.o
	$(CC) -shared $(BINDIR)LinkedListAPI.o -o $(BINDIR)liblist.so
#
SVGParser.o: $(SRCDIR)SVGParser.c $(IDIR)SVGParser.h
	$(CC) $(CFLAGS) -I$(IDIR) -c -fpic $(XMLFLAGS) $(SRCDIR)SVGParser.c -o $(BINDIR)$@

libsvg.so: $(BINDIR)SVGParser.o 
	$(CC) -shared $(BINDIR)SVGParser.o -o $(BINDIR)$@
#xmlExample: $(SRCDIR)libXmlExample.c
#	$(CC) $(CFLAGS) -I$(IDIR) $(XMLFLAGS) $(SRCDIR)libXmlExample.c -o $(BINDIR)xmlExample
#
#StructListDemo.o: $(SRCDIR)StructListDemo.c
#	$(CC) $(CFLAGS) -I$(IDIR) -c $(SRCDIR)StructListDemo.c -o $(BINDIR)StructListDemo.o
#	
#runDemo: $(BINDIR)StructListDemo
#	./bin/StructListDemo
#
clean:
	rm -rf *.o *.so **/*.o **/*.so
