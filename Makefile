CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp Server.cpp ClientHandle.cpp DB.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clear: 	
	rm -f *.o $(EXECUTABLE) 
