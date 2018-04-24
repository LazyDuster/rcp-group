#include "proj.h"

void fail(char msg[]) {
    perror(msg);
    exit(1);
}

struct send_msg receive_msg(int fromSocket) { // receives a single message from the network
    int err = 0;
    
    struct send_msg msgbuf;

    if ((err = recv(fromSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
        fail("recv");
    }

    return msgbuf;
} 

int send_message(int msgtype, int size, char name[], int toSocket) { // sends a single message to the network
    int err;

    struct send_msg msgbuf;

    msgbuf.file_size = size;

    strncpy(msgbuf.filename, name, 128);

    msgbuf.msg_type = msgtype;

    //printf("type: %d\n", msgbuf.msg_type);

    if ((err = send(toSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
        fail("sendto");
    }

    return 0;
}

int send_data(char name[], int length, int senderSocket, int socketType) { // loops through reads on the file and sends 1024 blocks to the network
    int err1;
    int err2;
    int totalDataWrote = 0;

    int fd;
    if ((fd = open(name, O_RDONLY)) < 0) {
        fail("open");
    }

    do {
        struct data_msg msgbuf;
        err1 = read(fd, msgbuf.buffer, MAX_DATA_SIZE - 1);

        msgbuf.buffer[err1] = '\0'; 
        // We only read 1023 byte and replace the 1024th byte with a null byte
        // I can't seem to get it to work if I was to send 1024 bytes directly

        msgbuf.msg_type = CMD_DATA;
        msgbuf.data_leng = err1;

        totalDataWrote += err1;

        if ((err2 = send(senderSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
            close(fd);
            fail("sendto");
        }

        printf("Sent %d bytes from file\n", err1);
    } while (err1 >= MAX_DATA_SIZE - 1);

    printf("Wrote %d bytes to remote %s\n", totalDataWrote, socketType == CLIENT ? "client" : "server");

    close(fd);

    return 0;
}

int recv_data(int fromSocket, int size, char name[]) { // loops through reads on the network and writes the blocks received to the file
    int err;
    
    FILE * output;

    int currentSize = 0;

    if ((output = fopen(name, "wb")) == NULL) {
        fail("file open");
    }

    while (currentSize < size) {
        struct data_msg msgbuf;

        if ((err = recv(fromSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
            fclose(output);
            fail("recv");
        }

        printf("Read %lu bytes\n", strlen(msgbuf.buffer));

        currentSize += msgbuf.data_leng;

        fprintf(output, "%s", msgbuf.buffer);
    }

    printf("Wrote %d bytes to output file\n", currentSize);

    fclose(output);

    return 0;
}