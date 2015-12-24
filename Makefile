#!/bin/make
CC = gcc
CXX = g++
CFLAGS = -Wall -O3 -march=native -pipe -lm
CXXFLAGS = -std=c++11 -Wall -O3 -march=native -pipe -DCGAL_NDEBUG -I/usr/local/include
LDFLAGS = -lCGAL -lgmp
OBJS = MyMain delete2

.PHONY: all clean
#.SUFFIXES: .cpp .o

all: $(OBJS)

clean:
	$(RM) $(OBJS)

#.cpp.o:
#	$(CXX) $(CXXFLAGS) -o $@ -c $^
.cpp:
	$(CXX) $^ -o $@ $(LDFLAGS) $(CXXFLAGS)

.c:
	$(CC) $^ -o $@ $(CFLAGS)
