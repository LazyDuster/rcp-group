#include "proj.h"

int main (int argc, char *argv[]) {

    int err;

    int socketDescriptor;
    int clientDescriptor;

    struct sockaddr_in sSocket;
    struct sockaddr_in cSocket;

    char cAddress[INET_ADDRSTRLEN];

    if (argc != 3) {
        printf("Usage: server <address> <port>\n");
        exit(1);
    }

    

    sSocket.sin_family = AF_INET;

    if ((err = inet_pton(AF_INET, argv[2], &sSocket.sin_addr)) != 1) {
        printf("Invalid address: %d\n", err);
        exit(1);
    }

    sSocket.sin_port = strtol(argv[3], NULL, 10);
    if (sSocket.sin_port <= 1024 || sSocket.sin_port >= 64000) {
        printf("Invalid port, port must be >greater than 1024 and less than 64k\n");
        exit(1);
    }

    if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(socketDescriptor, (struct sockaddr *)&sSocket, sizeof(sSocket)) != 0) {
        perror("bind");
        exit(1);
    }

    while (1) {
        printf("Server: listening for connection\n");
        if (listen(socketDescriptor, 50) != 0) {
            perror("listen");
            exit(1);
        }

        printf("Server: connection received, waiting for connection...\n");
        socklen_t cSize = sizeof(cSocket);
        if ((clientDescriptor = accept(socketDescriptor, (struct sockaddr *)&cSocket, &cSize)) < 0) {
            perror("accept");
            exit(1);
        }

        inet_ntop(AF_INET, &cSocket, cAddress, INET_ADDRSTRLEN);
        printf("Server: Accepted %s\n", cAddress, cSocket.sin_port);

        receiveMessage();
    }
    

}