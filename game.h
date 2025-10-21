
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h> 
#include <stdbool.h>

#define PITS 6
#define PLAYERS 2

typedef struct {
    int pits[PLAYERS][PITS]; // pits[player][pit_index]
    int score[PLAYERS];
    int current_player; // 0 or 1
    bool finished;
} Awale;

void awal_init(Awale *g);
bool awal_make_move(Awale *g, int player, int pit_index, char *errbuf, size_t errlen);
void awal_print(const Awale *g, char *buf, size_t buflen);
bool awal_is_legal(const Awale *g, int player, int pit);
int awal_winner(const Awale *g); // -1 none / 0 or 1 winner / 2 draw
void awal_serialize(const Awale *g, char *buf, size_t len);
void awal_deserialize(Awale *g, const char *buf);

#endif
