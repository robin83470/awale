#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PITS_PER_SIDE 6
#define TOTAL_PITS (PITS_PER_SIDE * 2)
#define INITIAL_SEEDS 4
#define TOTAL_SEEDS (INITIAL_SEEDS * TOTAL_PITS)

typedef enum { PLAYER1 = 0, PLAYER2 = 1 } Player;

void init_board(int pits[]) {
    for (int i = 0; i < TOTAL_PITS; ++i) pits[i] = INITIAL_SEEDS;
}

void print_board(int pits[], int score1, int score2) {
    printf("\n      --- AWALÉ ---\n\n");
    printf("  Joueur 2 (haut)\n\n    ");
    // print indices for player 2 as they see (1..6)
    for (int i = PITS_PER_SIDE - 1; i >= 0; --i)
        printf(" [%d]", i+1);
    printf("\n    ");
    // print seeds in player2 pits (indices 6..11 but printed reversed)
    for (int i = PITS_PER_SIDE - 1; i >= 0; --i)
        printf("  %2d ", pits[PITS_PER_SIDE + i]);
    printf("\n\nScore P2: %2d\t\tScore P1: %2d\n\n    ", score2, score1);
    // print seeds in player1 pits (0..5)
    for (int i = 0; i < PITS_PER_SIDE; ++i)
        printf("  %2d ", pits[i]);
    printf("\n    ");
    // indices for player1
    for (int i = 0; i < PITS_PER_SIDE; ++i)
        printf(" [%d]", i+1);
    printf("\n\n  Joueur 1 (bas)\n\n");
}

int next_index(int idx) {
    return (idx + 1) % TOTAL_PITS;
}

int prev_index(int idx) {
    return (idx - 1 + TOTAL_PITS) % TOTAL_PITS;
}

// returns 1 if index belongs to player (player owns indices):
int is_own_pit(Player player, int idx) {
    if (player == PLAYER1) return (idx >= 0 && idx < PITS_PER_SIDE);
    else return (idx >= PITS_PER_SIDE && idx < TOTAL_PITS);
}

// returns 1 if index belongs to opponent:
int is_opponent_pit(Player player, int idx) {
    return !is_own_pit(player, idx);
}

// simulate a move, writing into tmp array, performing sow and captures, and return capture count
int simulate_move_and_capture(const int pits[], int tmp[], Player player, int chosen_idx) {
    // copy
    for (int i = 0; i < TOTAL_PITS; ++i) tmp[i] = pits[i];

    int seeds = tmp[chosen_idx];
    if (seeds == 0) return -1; // invalid
    tmp[chosen_idx] = 0;

    int pos = chosen_idx;
    // sow seeds
    while (seeds > 0) {
        pos = next_index(pos);
        tmp[pos]++;
        seeds--;
    }

    int captured = 0;
    // capture only if last seed landed in opponent pit
    if (is_opponent_pit(player, pos)) {
        // check backward capturing while pits are in opponent side and have 2 or 3
        int check_pos = pos;
        while (is_opponent_pit(player, check_pos) && (tmp[check_pos] == 2 || tmp[check_pos] == 3)) {
            captured += tmp[check_pos];
            tmp[check_pos] = 0;
            check_pos = prev_index(check_pos);
        }
    }
    return captured;
}

// apply move (modifies pits and returns captured)
int apply_move_and_capture(int pits[], Player player, int chosen_idx) {
    int seeds = pits[chosen_idx];
    pits[chosen_idx] = 0;
    int pos = chosen_idx;
    while (seeds > 0) {
        pos = next_index(pos);
        pits[pos]++;
        seeds--;
    }
    int captured = 0;
    if (is_opponent_pit(player, pos)) {
        int check_pos = pos;
        while (is_opponent_pit(player, check_pos) && (pits[check_pos] == 2 || pits[check_pos] == 3)) {
            captured += pits[check_pos];
            pits[check_pos] = 0;
            check_pos = prev_index(check_pos);
        }
    }
    return captured;
}

int opponent_has_any_seeds(const int pits[], Player opponent) {
    int start = (opponent == PLAYER1) ? 0 : PITS_PER_SIDE;
    for (int i = 0; i < PITS_PER_SIDE; ++i)
        if (pits[start + i] > 0) return 1;
    return 0;
}

// check if move chosen_idx is allowed under starvation rule
int move_is_allowed(const int pits[], Player player, int chosen_idx) {
    if (pits[chosen_idx] == 0) return 0;
    int tmp[TOTAL_PITS];
    int captured = simulate_move_and_capture(pits, tmp, player, chosen_idx);
    if (captured < 0) return 0;
    Player opponent = (player == PLAYER1) ? PLAYER2 : PLAYER1;
    if (opponent_has_any_seeds(tmp, opponent)) return 1;
    // move would leave opponent without seeds -> allowed only if no other move avoids that
    // We'll let caller check "exists move that avoids starvation"
    return 0;
}

int exists_move_avoiding_starvation(const int pits[], Player player) {
    for (int i = 0; i < PITS_PER_SIDE; ++i) {
        int idx;
        if (player == PLAYER1) idx = i;
        else idx = TOTAL_PITS - 1 - i; // map 1..6 to 11..6
        if (pits[idx] == 0) continue;
        int tmp[TOTAL_PITS];
        simulate_move_and_capture(pits, tmp, player, idx);
        Player opponent = (player == PLAYER1) ? PLAYER2 : PLAYER1;
        if (opponent_has_any_seeds(tmp, opponent)) return 1;
    }
    return 0;
}

int any_move_possible(const int pits[], Player player) {
    for (int i = 0; i < PITS_PER_SIDE; ++i) {
        int idx = (player == PLAYER1) ? i : (TOTAL_PITS - 1 - i);
        if (pits[idx] > 0) return 1;
    }
    return 0;
}

int total_remaining_seeds(const int pits[]) {
    int s = 0;
    for (int i = 0; i < TOTAL_PITS; ++i) s += pits[i];
    return s;
}

int read_move(Player player) {
    int mv;
    printf("Joueur %d, entrez le numéro de case (1-%d) à jouer : ",
           player == PLAYER1 ? 1 : 2, PITS_PER_SIDE);
    if (scanf("%d", &mv) != 1) {
        // flush badly read input
        while (getchar() != '\n');
        return -1;
    }
    return mv;
}

int map_input_to_index(Player player, int in) {
    if (in < 1 || in > PITS_PER_SIDE) return -1;
    if (player == PLAYER1) return in - 1;         // 1..6 -> 0..5
    else return TOTAL_PITS - in;                   // 1..6 -> 11..6
}

int main(void) {
    int pits[TOTAL_PITS];
    int score1 = 0, score2 = 0;
    Player turn = PLAYER1;

    init_board(pits);

    while (1) {
        print_board(pits, score1, score2);

        // end if someone reached >=25
        if (score1 >= (TOTAL_SEEDS/2 + 1) || score2 >= (TOTAL_SEEDS/2 + 1)) break;

        // if no move possible for current player -> game ends, remaining seeds to opponent
        if (!any_move_possible(pits, turn)) {
            printf("Joueur %d ne peut plus jouer.\n", turn == PLAYER1 ? 1 : 2);
            int remaining = total_remaining_seeds(pits);
            if (turn == PLAYER1) score2 += remaining;
            else score1 += remaining;
            break;
        }

        int mv = -1;
        int idx = -1;
        while (1) {
            mv = read_move(turn);
            if (mv == -1) {
                printf("Entrée invalide, réessayez.\n");
                continue;
            }
            idx = map_input_to_index(turn, mv);
            if (idx < 0) {
                printf("Numéro hors de portée, réessayez.\n");
                continue;
            }
            if (pits[idx] == 0) {
                printf("Cette case est vide, choisissez-en une autre.\n");
                continue;
            }
            // starvation check: if move is allowed normally (doesn't starve) -> ok
            if (move_is_allowed(pits, turn, idx)) break;
            // else move would starve opponent; allow only if no alternative avoids that
            if (!exists_move_avoiding_starvation(pits, turn)) {
                // allowed because no alternative
                break;
            } else {
                printf("Ce coup laisserait l'adversaire sans graines — coup interdit.\n");
                continue;
            }
        }

        int captured = apply_move_and_capture(pits, turn, idx);
        if (turn == PLAYER1) score1 += captured;
        else score2 += captured;

        printf("Joueur %d a capturé %d graine(s) ce tour.\n", turn == PLAYER1 ? 1 : 2, captured);

        // check immediate win
        if (score1 >= (TOTAL_SEEDS/2 + 1) || score2 >= (TOTAL_SEEDS/2 + 1)) break;

        // If only a few seeds remain and no one can capture further, break to assign rest
        if (!any_move_possible(pits, PLAYER1) || !any_move_possible(pits, PLAYER2)) {
            // give remaining seeds to the side that still has moves (or split)
            int rem = total_remaining_seeds(pits);
            if (!any_move_possible(pits, PLAYER1)) score2 += rem;
            else if (!any_move_possible(pits, PLAYER2)) score1 += rem;
            break;
        }

        // switch turn
        turn = (turn == PLAYER1) ? PLAYER2 : PLAYER1;
    }

    // final board and scores
    print_board(pits, score1, score2);
    printf("Partie terminée !\n");
    printf("Score final - Joueur 1 : %d, Joueur 2 : %d\n", score1, score2);
    if (score1 > score2) printf("Joueur 1 gagne !\n");
    else if (score2 > score1) printf("Joueur 2 gagne !\n");
    else printf("Match nul !\n");

    return 0;
}


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int affichage(int l[12])
{
    int i;
    for(i=0; i<6; i++)
    {
        printf("%d   ", l[i]);
    }
    printf("\n");
    for(i=6; i<12; i++)
    {
        printf("%d   ", l[i]);
    }
    printf("\n");
    return 0;
}

int main()
{
    int l[12];
    int joueur1=0, joueur2=0, i;
    for(i=0; i<12; i++)
    {
        l[i]=0;
    }
    affichage(l);
}