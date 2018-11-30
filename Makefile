CC=g++

all: unpacker test

unpacker: unpacker.cpp
	$(CC) -o unpacker unpacker.cpp -Wall -Werror

test: test.cpp
	$(CC) -o test test.cpp -Wall -Werror