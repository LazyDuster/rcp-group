#include "proj.h"

void fail(char msg[]) {
    perror(msg);
    exit(1);
}

struct send_msg receive_msg(int receiverSocket) { // receives a single message from the network
    int err = 0;
    
    struct send_msg msgbuf;

    if ((err = recv(receiverSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
        fail("recv");
    }

    return msgbuf;
} 

int send_mesg(int msgtype, int size, char name[], int senderSocket) { // sends a single message to the network
    int err;

    struct send_msg msgbuf;

    msgbuf.file_size = size;

    strncpy(msgbuf.filename, name, 128);
    //msgbuf.filename = name;

    if (msgtype !=  CMD_SEND || msgtype != CMD_RECV) {
        // throw some error
    }
    else {
        msgbuf.msg_type = msgtype;
    }

    if ((err = send(senderSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
        fail("sendto");
    }

    return 0;
}

int send_data(char name[], int senderSocket, int socketType) { // loops through reads on the file and sends 1024 blocks to the network
    int err;

    int totalDataWrote = 0;

    FILE * data;

    if ((data = fopen(name, "r")) == NULL) {
        fail("file open");
    }

    while (!feof (data)) {
        struct data_msg msgbuf;

        fgets(msgbuf.buffer, MAX_DATA_SIZE, (FILE*)data);

        msgbuf.msg_type = CMD_DATA;
        msgbuf.data_leng = strlen(msgbuf.buffer);

        totalDataWrote += strlen(msgbuf.buffer);

        if ((err = send(senderSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
            fclose(data);
            fail("sendto");
        }

        printf("Sent %lu bytes from file\n", strlen(msgbuf.buffer));
    }

    printf("Wrote %d bytes to remote %s\n", totalDataWrote, socketType == CLIENT ? "client" : "server");

    fclose(data);

    return 0;
}

int recv_data(int receiverSocket, int size, char name[]) { // loops through reads on the network and writes the blocks received to the file
    int err;
    
    FILE * output;

    int currentSize = 0;

    if ((output = fopen(name, "wb")) == NULL) {
        fail("file open");
    }

    while (currentSize <= size) {
        struct data_msg msgbuf;

        if ((err = recv(receiverSocket, &msgbuf, sizeof(msgbuf), 0)) < 0) {
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
