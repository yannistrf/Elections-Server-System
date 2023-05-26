#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "net_cl.hpp"

#define HEADER_SIZE sizeof(int)

int send_msg(int sock, char* msg) {
    int len = strlen(msg) + 1;
    char header_buff[HEADER_SIZE];
    bzero(header_buff, HEADER_SIZE);
    *header_buff = len;
    write(sock, header_buff, HEADER_SIZE);
    write(sock, msg, len);

    return len;
}

int recv_msg(int sock, char* buff) {
    int len;
    char header_buff[HEADER_SIZE];

    read(sock, header_buff, HEADER_SIZE);
    len = *header_buff;
    int bytes_recvd = 0;
    while (bytes_recvd < len) {
        bytes_recvd += read(sock, &buff[bytes_recvd], len-bytes_recvd);
    }

    buff[bytes_recvd] = '\0';

    return bytes_recvd;
}