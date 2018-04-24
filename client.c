#include "proj.h"

int main(int argc, char *argv[])
{
    int err;
    
    struct sockaddr_in serverAddress;
    int serverSocket;

    char send[] = "-S";
    char rec[] = "-R";

    // Convert command line arguments and initialize socket structure
    if (argc != 5) {
        printf("Usage: client <address> <port> <copytype> <filename>\n\n");
        printf("-S client to server\n");
        printf("-R server to client\n");
        exit(1);
    } 
    
    serverAddress.sin_family = AF_INET;

    if ((err = inet_pton(AF_INET, argv[1], &serverAddress.sin_addr)) != 1) {
        printf("Invalid address\n");
        exit(1);
    }
    
    serverAddress.sin_port = strtol(argv[2], NULL, 10);
    if (serverAddress.sin_port <= 1024 || serverAddress.sin_port >= 64000) {
        printf("Invalid port, port must be >greater than 1024 and less than 64k\n");
        exit(1);
    }

    // Open socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Error initializing socket\n");
        exit(1);
    }

    if (connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0) {
        perror("connect");
        exit(1);
    }

    // Sending stuff
    if (strcmp(argv[3], send) == 0) {
        FILE * fp = fopen(argv[4], "rb");
        if (fp == NULL) {
            perror("fopen");
            exit(1);
        }
        
        fseek(fp, 0, SEEK_END);
        int length = ftell(fp);
        fclose(fp);

        //printf("Length: %d", length);

        send_message(CMD_RECV, length, argv[4], serverSocket);
        send_data(argv[4], length, serverSocket, CLIENT);

    } else if (strcmp(argv[3], rec) == 0) {
        struct send_msg msgbuf;
        
        send_message(CMD_SEND, 0, argv[4], serverSocket);
        msgbuf = receive_msg(serverSocket);
        
        recv_data(serverSocket, msgbuf.file_size, argv[4]);
    }

    exit(1);
}
