#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

#define BUFLEN 2048

int main(int argc, char **argv) {
    if (argc < 2) { printf("Usage: %s host [port]\n", argv[0]); return 1; }
    char *host = argv[1];
    int port = argc>=3 ? atoi(argv[2]) : 5555;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }
    struct hostent *he = gethostbyname(host);
    if (!he) { perror("gethostbyname"); return 1; }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr*)he->h_addr);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("connect"); return 1; }
    printf("Connected to %s:%d\n", host, port);
    fd_set master, readfds;
    FD_ZERO(&master); FD_ZERO(&readfds);
    FD_SET(0, &master); // stdin
    FD_SET(sock, &master);
    while(1) {
        readfds = master;
        if (select(sock+1, &readfds, NULL, NULL, NULL) < 0) { perror("select"); break;}
        if (FD_ISSET(0, &readfds)) {
            char line[BUFLEN];
            if (!fgets(line, sizeof(line), stdin)) break;
            // send as-is
            send(sock, line, strlen(line), 0);
        }
        if (FD_ISSET(sock, &readfds)) {
            char r[BUFLEN];
            ssize_t n = recv(sock, r, sizeof(r)-1, 0);
            if (n <= 0) { printf("Disconnected from server\n"); break; }
            r[n] = '\0';
            printf("%s", r);
        }
    }

    close(sock);
    return 0;
}
