#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "netServer.hpp"

#define QUEUE_SIZE 32767
#define HEADER_SIZE sizeof(int)


int accept_sock_init(int port) {

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM,0)) == -1) {
        return SOCKERR;
    }
    
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    int option_value = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &option_value, sizeof(int));

    if (bind(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
        perror("Failed to bind");
        return BINDERR;
    }

    if (listen(sock, QUEUE_SIZE) != 0) {
        return LISTENERR;
    }

    return sock;
}

int accept_conn(int accept_sock) {
    int sock;
    if ((sock = accept(accept_sock, NULL, NULL)) == -1) {
        if (errno == EINTR)
            return SHUTDOWN;

        return -1;
    }

    return sock;
}


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