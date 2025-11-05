# Nom des exécutables
SERVER = server
CLIENT = client

# Fichiers sources
SERVER_SRC = server2.c
CLIENT_SRC = client2.c

# Fichiers objets
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g

# Règle par défaut
all: $(SERVER) $(CLIENT)

# Compilation du serveur
$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation du client
$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Règle générique pour les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f *.o $(SERVER) $(CLIENT)

# Pour forcer la recompilation complète
re: clean all

.PHONY: all clean re
