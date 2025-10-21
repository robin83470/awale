#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // pour sleep() et usleep()

void clear() {
    // Nettoie le terminal (Linux / macOS)
    printf("\033[2J\033[1;1H");
}

void frame1() {
    printf("        o\n");
    printf("       /|\\        🎉 BON ANNIVERSAIRE TOM !!! 🎉\n");
    printf("       / \\ \n");
}

void frame2() {
    printf("         o\n");
    printf("        \\|/       🎂 Passe une journée incroyable ! 🎂\n");
    printf("        / \\ \n");
}

void frame3() {
    printf("        \\o/\n");
    printf("         |         🕺 Le bonhomme danse pour toi, Tom ! 💃\n");
    printf("        / \\ \n");
}

int main() {
    clear();
    printf("Prépare-toi Tom... le show commence ! 😎\n");
    sleep(2);

    for (int i = 0; i < 6; i++) {
        clear();
        switch (i % 3) {
            case 0: frame1(); break;
            case 1: frame2(); break;
            case 2: frame3(); break;
        }
        fflush(stdout);
        usleep(400000);  // 0.4 seconde par frame
    }

    clear();
    printf("       \\o/ \n");
    printf("        |         🎊 BON ANNIVERSAIRE TOM !!! 🎊\n");
    printf("       / \\ \n\n");

    printf("💫 Que ta journée soit pleine de rires, de gâteaux et d’amis 💫\n");
    printf("🥳 De la part de ton pote codeur !\n\n");

    return 0;
}
