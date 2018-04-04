#include "proj.h"

int main(int argc, int *argv[])
{
    struct sockaddr_in sock;
    struct hostent *host;

    // Convert command line arguments and initialize socket structure
    if (argc != 5) {
        printf("Usage: client <address> <port> <copytype> <filename>\n\n");
        printf("-S client to server\n");
        printf("-R server to client\n");
        exit(1);
    } else {
        sock.sin_family = AF_INET;
        sock.sin_port = htons((uint16_t) atoi(argv[2]));
        if (inet_pton(AF_INET, argv[1], &sock.sin_addr.s_addr) <= 0) {
            perror("Error getting address\n");
            exit(1);
        }
    }

    // Open socket
    int s;
    if ((s = socket(sock.sin_family, SOCK_DGRAM, IPPROTO_TCP)) < 0) {
        perror("Error initializing socket\n");
        exit(1);
    }

    // Sending stuff
}
