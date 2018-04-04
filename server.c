#include "sendrecv.c"

int main (int argc, char *argv[]) {

    int err;

    int serverSocket;
    int clientSocket;

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    char cAddress[INET_ADDRSTRLEN];


    if (argc != 3) {
        printf("Usage: server <address> <port>\n");
        exit(1);
    }


    serverAddress.sin_family = AF_INET;

    if ((err = inet_pton(AF_INET, argv[1], &serverAddress.sin_addr)) != 1) {
        printf("Invalid address: %d\n", err);
        exit(1);
    }

    serverAddress.sin_port = strtol(argv[2], NULL, 10);
    if (serverAddress.sin_port <= 1024 || serverAddress.sin_port >= 64000) {
        printf("Invalid port, port must be >greater than 1024 and less than 64k\n");
        exit(1);
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0) {
        perror("bind");
        exit(1);
    }



    while (1) {
        //printf("Server: listening for connection\n");
        if (listen(serverSocket, 50) != 0) {
            perror("listen");
            exit(1);
        }

        //printf("Server: connection received, waiting for connection...\n");
        socklen_t cSize = sizeof(clientAddress);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &cSize)) < 0) {
            perror("accept");
            exit(1);
        }

        inet_ntop(AF_INET, &clientAddress, cAddress, INET_ADDRSTRLEN);
        //printf("Server: Accepted %s\n", cAddress, clientAddress.sin_port);
        printf("Connection accepted from %s\n", cAddress);

        struct send_msg msgbuf;
        msgbuf = receive_msg(serverSocket);

        if (msgbuf.msg_type == CMD_RECV) {
            printf("Receive file: %s\n", msgbuf.filename);
            printf("Receive file length: %d\n", msgbuf.file_size);

            if ((err = recv_data(serverSocket, msgbuf.file_size, msgbuf.filename)) != 0) {
                perror("recv_data");
                exit(1);
            }
        }

        else if (msgbuf.msg_type == CMD_SEND) {
            char fileName[100];

            printf("Enter the first file to scan:\n");
            scanf("%s", fileName);

            FILE * fp = fopen("filename", "rb");
            fseek(fp, 0, SEEK_END);
            int length = ftell(fp);
            fclose(fp);

            send_mesg(CMD_RECV, length, fileName, serverSocket);

            send_data(fileName, serverSocket, SERVER);
        }

        close(clientSocket);
    }
    
    close(serverSocket);
}