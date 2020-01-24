CFLAGS = -Wall -g -std=c11
BIN = bin/
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2
VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt
SVG_FILE = 

# Standard common makefile
parser: $(BIN)libsvgparse.so test_harness

$(BIN)libsvgparse.so: $(BIN)SVGParser.o $(BIN)LinkedListAPI.o
	gcc -shared -o $(BIN)libsvgparse.so $(BIN)SVGParser.o $(BIN)LinkedListAPI.o -lxml2 -lm

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

test_harness: $(SRC)LinkedListAPI.c $(SRC)SVGParser.c
	gcc $(CFLAGS) -g -I$(INC) -I$(XML_PATH) -L./bin -lsvgparse -lxml2 -lm $(SRC)test.c -o $(BIN)test_harness.o 

valgrind:
	cd bin; valgrind $(VALGRIND_FLAGS) ./test_harness.o $(SVG_FILE)

run_test:
	cd bin; ./test_harness.o $(SVG_FILE)

xmlExample: 
	$(CC) $(CFLAGS) -I$(INC) -lxml2 -I$(XML_PATH) $(XMLFLAGS) $(SRC)libXmlExample.c -o $(BIN)xmlExample.o

clean:
	rm -rf $(BIN)*.so $(BIN)*.o $(BIN)*.txt
