CC = gcc
CFLAGS = -Wall
SRC = server.c peer_tree.c file_tree.c
#OBJ = $(SRC:.c=.o)
HEAD = stdhead.h server.h file_info.h peer_info.h peer_tree.h file_tree.h

all: $(SRC) $(HEAD)
	$(CC) server.c peer_tree.c file_tree.c -o server -pthread
#	$(CC) $(CFLAGS) peer_info.o -o peer_info
#	$(CC) $(CFLAGS) file_info.o -o file_info

clean:
	rm server
