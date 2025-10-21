#include "game.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void awal_init(Awale *g) {
    memset(g,0,sizeof(*g));
    for(int p=0;p<PLAYERS;p++){
        for(int i=0;i<PITS;i++){
            g->pits[p][i] = 4; // case classique 4 graines
        }
        g->score[p]=0;
    }
    g->current_player = rand()%2;
    g->finished = false;
}

static int opposite_pit(int pit) {
    return PITS - 1 - pit;
}

bool awal_is_legal(const Awale *g, int player, int pit) {
    if (pit < 0 || pit >= PITS) return false;
    if (g->pits[player][pit] == 0) return false;
    // simple rule: cannot starve opponent (optional advanced), we skip starvation check for simplicity
    return true;
}

// distribute seeds; returns 0 ok, -1 illegal, -2 ended
bool awal_make_move(Awale *g, int player, int pit_index, char *errbuf, size_t errlen) {
    if (g->finished) {
        if (errbuf) snprintf(errbuf,errlen,"La partie est terminée");
        return false;
    }
    if (player != g->current_player) {
        if (errbuf) snprintf(errbuf,errlen,"Ce n'est pas ton tour");
        return false;
    }
    if (!awal_is_legal(g, player, pit_index)) {
        if (errbuf) snprintf(errbuf,errlen,"Coup illégal");
        return false;
    }

    int seeds = g->pits[player][pit_index];
    g->pits[player][pit_index] = 0;
    int pos_player = player;
    int pos_pit = pit_index;

    while (seeds > 0) {
        // move to next pit
        pos_pit++;
        if (pos_pit >= PITS) {
            pos_pit = 0;
            pos_player = 1 - pos_player;
        }
        // skip the original pit where we picked up if we looped (standard rule still allows)
        g->pits[pos_player][pos_pit]++;
        seeds--;
    }

    // capture rule: if last seed landed in opponent side and now pit has 2 or 3, capture and continue backwards
    int last_player = pos_player;
    int last_pit = pos_pit;
    if (last_player != player) {
        int captured = 0;
        while (last_pit >=0) {
            int seeds_here = g->pits[last_player][last_pit];
            if (seeds_here == 2 || seeds_here == 3) {
                captured += seeds_here;
                g->pits[last_player][last_pit] = 0;
                last_pit--;
            } else break;
        }
        g->score[player] += captured;
    }

    // check end: if a player has >= 25 (out of 48) or all pits empty
    int totalCaptured = g->score[0] + g->score[1];
    if (g->score[player] >= 25 || totalCaptured >= 48) {
        g->finished = true;
    } else {
        // also if opponent has no seeds after move, end game and award remaining seeds to player who still has them
        int opp = 1 - player;
        int sumOpp=0,sumPlayer=0;
        for(int i=0;i<PITS;i++){ sumOpp += g->pits[opp][i]; sumPlayer += g->pits[player][i]; }
        if (sumOpp == 0 || sumPlayer == 0) {
            // collect remaining
            for(int i=0;i<PITS;i++){
                g->score[0] += g->pits[0][i];
                g->score[1] += g->pits[1][i];
                g->pits[0][i] = g->pits[1][i] = 0;
            }
            g->finished = true;
        }
    }

    // switch turn if not finished
    if (!g->finished) g->current_player = 1 - g->current_player;
    return true;
}

void awal_print(const Awale *g, char *buf, size_t buflen) {
    // produce simple ASCII board
    char tmp[1024];
    int n = 0;
    n += snprintf(tmp+n, sizeof(tmp)-n, "Scores: P0=%d  P1=%d\n", g->score[0], g->score[1]);
    n += snprintf(tmp+n, sizeof(tmp)-n, "Current: P%d\n", g->current_player);
    n += snprintf(tmp+n, sizeof(tmp)-n, "  Opponent side:\n   ");
    for(int i=PITS-1;i>=0;i--) n += snprintf(tmp+n, sizeof(tmp)-n, " [%2d] ", g->pits[1][i]);
    n += snprintf(tmp+n, sizeof(tmp)-n, "\n   ");
    for(int i=0;i<PITS;i++) n += snprintf(tmp+n, sizeof(tmp)-n, "  ^%d  ", i);
    n += snprintf(tmp+n, sizeof(tmp)-n, "\n  Your side:\n   ");
    for(int i=0;i<PITS;i++) n += snprintf(tmp+n, sizeof(tmp)-n, " [%2d] ", g->pits[0][i]);
    n += snprintf(tmp+n, sizeof(tmp)-n, "\n");
    strncpy(buf, tmp, buflen-1);
    buf[buflen-1] = '\0';
}

int awal_winner(const Awale *g) {
    if (!g->finished) return -1;
    if (g->score[0] > g->score[1]) return 0;
    if (g->score[1] > g->score[0]) return 1;
    return 2;
}

void awal_serialize(const Awale *g, char *buf, size_t len) {
    // very simple: flatten numbers into a line
    int n=0;
    n += snprintf(buf+n, len>n?len-n:0, "%d %d %d ", g->current_player, g->finished?1:0, g->score[0]);
    n += snprintf(buf+n, len>n?len-n:0, "%d ", g->score[1]);
    for(int p=0;p<PLAYERS;p++){
        for(int i=0;i<PITS;i++){
            n += snprintf(buf+n, len>n?len-n:0, "%d ", g->pits[p][i]);
        }
    }
    buf[len-1] = '\0';
}

void awal_deserialize(Awale *g, const char *buf) {
    const char *p = buf;
    sscanf(p, "%d %d %d %d", &g->current_player, (int*)&g->finished, &g->score[0], &g->score[1]);
    // after reading those 4 ints, scan the rest
    const char *q = buf;
    int idx=0;
    // skip first 4 ints
    for(int k=0;k<4;k++){
        while(*q && *q!=' ') q++;
        if(*q==' ') q++;
    }
    for(int i=0;i<PLAYERS;i++){
        for(int j=0;j<PITS;j++){
            int v=0;
            if (sscanf(q, "%d", &v)==1) {
                g->pits[i][j] = v;
            }
            while(*q && *q!=' ') q++;
            if(*q==' ') q++;
        }
    }
}
