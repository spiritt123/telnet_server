CC=g++
CFLAGS=-c -Wall -std=c++17
LDFLAGS= -pthread
SOURCES=main.cpp Server.cpp ClientHandle.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clear: 	
	rm -f *.o $(EXECUTABLE) 
