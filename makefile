CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
SRCS = server.c game.c
OBJS = $(SRCS:.c=.o)
SERVER = awal_server
CLIENT = awal_client

all: $(SERVER) $(CLIENT)

$(SERVER): server.c game.c game.h
	$(CC) $(CFLAGS) -o $(SERVER) server.c game.c

$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $(CLIENT) client.c

clean:
	rm -f $(SERVER) $(CLIENT) *.o saved_games/*.txt

.PHONY: all clean
