CC=g++

unpacker: unpacker.cpp
	$(CC) -o unpacker unpacker.cpp -Wall -Werror