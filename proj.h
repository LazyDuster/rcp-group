#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 1024
#define CMD_SEND 0
#define CMD_RECV 1
#define CMD_RESP 2
#define CMD_DATA 3

struct send_msg
{
    int msg_type; // CMD_SEND or CMD_RECV
    int file_size; // size of file to be sent (if applicable, if not, set to 0)
    char filename[128]; // name of file
};

struct resp_msg // handshake message
{
    int msg_type; // either CMS_SEND or CMS_RECV
    int status; // 0 = OK, otherwise put errno here
    int file_size; // used only by server when transferring a file from the server to the client and status is okay
};

struct data_msg
{
    int msg_type; // CMD_DATA
    int data_leng; // Length of data in buffer
    char buffer[MAX_DATA_SIZE];
};