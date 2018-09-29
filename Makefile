CC=g++
CFLAGS=-c -Wall -std=c++11 -L/usr/lib -lmysqlcppconn
LDFLAGS=-std=c++11 -L/usr/lib -lmysqlcppconn
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(SOURCES:.cpp=)

all: $(EXECUTABLE)

%.o: %.cpp
	$(CC) $(CFLAGS) $^ -o $@

$(EXECUTABLE): % : %.o
	$(CC) $(LDFLAGS) $^ -o $@	