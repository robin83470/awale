#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "game.h"

#define PORT 5555
#define MAX_CLIENTS 64
#define BUFLEN 1024
#define USERNAME_MAX 32
#define BIO_LINES 10
#define GAME_SLOTS 64
#define SAVED_DIR "saved_games"

typedef enum { STATE_IDLE, STATE_IN_GAME } ClientState;

typedef struct Client {
    int fd;
    char username[USERNAME_MAX];
    char bio[1024];
    ClientState state;
    int game_id; // -1 not in game
    bool private_mode;
    struct Client *next;
} Client;

typedef struct GameSlot {
    int id;
    Awale board;
    Client *player0;
    Client *player1;
    Client *observers[MAX_CLIENTS];
    int obs_count;
    bool active;
} GameSlot;

Client *clients = NULL;
GameSlot games[GAME_SLOTS];

void ensure_saved_dir() {
    struct stat st = {0};
    if (stat(SAVED_DIR, &st) == -1) {
        mkdir(SAVED_DIR, 0755);
    }
}

Client* find_client_by_name(const char *name) {
    for(Client *c=clients; c!=NULL; c=c->next) {
        if (strcmp(c->username, name)==0) return c;
    }
    return NULL;
}

void broadcast_all(const char *msg, int except_fd) {
    for(Client *c=clients;c;c=c->next) {
        if (c->fd!=except_fd) {
            send(c->fd, msg, strlen(msg), 0);
        }
    }
}

void send_to_client(Client *c, const char *msg) {
    if (!c) return;
    send(c->fd, msg, strlen(msg), 0);
}

void remove_client(Client *c) {
    close(c->fd);
    // remove from linked list
    Client **pc = &clients;
    while(*pc && *pc != c) pc = &(*pc)->next;
    if (*pc) *pc = c->next;
    // if in game, mark player disconnected
    if (c->state == STATE_IN_GAME && c->game_id>=0) {
        GameSlot *g = &games[c->game_id];
        char tmp[BUFLEN];
        snprintf(tmp, sizeof(tmp), "SERVER: joueur %s déconnecté. Partie %d terminée.\n", c->username, g->id);
        if (g->player0 && g->player0 != c) send_to_client(g->player0,tmp);
        if (g->player1 && g->player1 != c) send_to_client(g->player1,tmp);
        // notify observers
        for(int i=0;i<g->obs_count;i++) send_to_client(g->observers[i], tmp);
        g->active = false;
    }
    free(c);
}

int next_game_id() {
    for(int i=0;i<GAME_SLOTS;i++) if (!games[i].active) return i;
    return -1;
}

void save_game_to_file(const GameSlot *g) {
    ensure_saved_dir();
    char filename[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(filename, sizeof(filename), "%s/game_%d_%04d%02d%02d_%02d%02d%02d.txt",
        SAVED_DIR, g->id,
        tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    FILE *f = fopen(filename,"w");
    if (!f) return;
    fprintf(f,"Game %d\nPlayers: %s vs %s\n", g->id,
        g->player0?g->player0->username:"(none)",
        g->player1?g->player1->username:"(none)");
    char buf[2048];
    awal_serialize(&g->board, buf, sizeof(buf));
    fprintf(f,"Board: %s\n", buf);
    fclose(f);
}

void send_board_to_players(GameSlot *gslot) {
    char buf[2048];
    awal_print(&gslot->board, buf, sizeof(buf));
    char out[4096];
    snprintf(out, sizeof(out), "GAME %d BOARD\n%s\n", gslot->id, buf);
    if (gslot->player0) send_to_client(gslot->player0, out);
    if (gslot->player1) send_to_client(gslot->player1, out);
    for(int i=0;i<gslot->obs_count;i++) send_to_client(gslot->observers[i], out);
}

void handle_register(Client *c, char *arg) {
    if (!arg || strlen(arg)==0) {
        send_to_client(c, "SERVER: REGISTER requires a username\n");
        return;
    }
    if (find_client_by_name(arg)) {
        send_to_client(c, "SERVER: Username already taken\n");
        return;
    }
    strncpy(c->username, arg, USERNAME_MAX-1);
    c->username[USERNAME_MAX-1]='\0';
    send_to_client(c, "SERVER: REGISTER OK\n");
    char tmp[BUFLEN];
    snprintf(tmp,sizeof(tmp),"SERVER: %s joined\n", c->username);
    broadcast_all(tmp, c->fd);
}

void handle_list(Client *c) {
    char out[BUFLEN];
    strcpy(out, "SERVER: Online users:\n");
    send(c->fd, out, strlen(out), 0);
    for(Client *cl=clients; cl; cl=cl->next) {
        if (strlen(cl->username)>0) {
            char line[128];
            snprintf(line,sizeof(line), " - %s%s%s\n", cl->username,
                cl->private_mode ? " (private)":"", cl->st_
