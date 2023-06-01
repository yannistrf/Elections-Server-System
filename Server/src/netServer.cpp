#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "netServer.hpp"

#define QUEUE_SIZE SHRT_MAX         // backlog arg for listen call
#define HEADER_SIZE sizeof(int)     // first msg is an int indicating the second's msg size


// Initialise the socket for accepting connections
int accept_sock_init(int port) {

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM,0)) == -1) {
        perror("sock");
        return -1;
    }
    
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // We don't need to wait after server shutdown to make him listen to
    // the same port
    int option_value = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &option_value, sizeof(int));

    if (bind(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
        perror("bind");
        return -1;
    }

    if (listen(sock, QUEUE_SIZE) != 0) {
        perror("listen");
        return -1;
    }

    return sock;
}

// Accepts incoming connections and returns the file descriptor
// on which the communication will happen
int accept_conn(int accept_sock) {
    
    int sock;
    if ((sock = accept(accept_sock, NULL, NULL)) == -1) {
        // Will be true if accept is interrupted from a signal
        // If so, that means we shutdown the server
        if (errno == EINTR)
            return SHUTDOWN;

        return -1;
    }

    return sock;
}


/*
    The protocol we follow for the communication is simple.
    The first message is an integer (4 bytes) which indicates
    the size of the actual message. After that we send the actual
    message. In that way, the recipient will know what to expect.
*/


// Sends a message to the client, returning the number of bytes sent.
// Returns -1 in case of failure.
int send_msg(int sock, char* msg) {

    int len = strlen(msg) + 1;
    char header_buff[HEADER_SIZE];
    bzero(header_buff, HEADER_SIZE);
    *header_buff = len;
    if (write(sock, header_buff, HEADER_SIZE) == -1 || write(sock, msg, len) == -1)
        return -1;
    
    return len;
}

// Receives a message from the client, returns the number of bytes read.
// Returns -1 in case of failure.
int recv_msg(int sock, char* buff) {

    int len;
    char header_buff[HEADER_SIZE];

    int r = read(sock, header_buff, HEADER_SIZE);
    if (r == -1 || r == 0)
        return -1;

    len = *header_buff;
    int bytes_recvd = 0;
    while (bytes_recvd < len) {
        r = read(sock, &buff[bytes_recvd], len-bytes_recvd);
        if (r == -1 || r == 0)
            return -1;
        bytes_recvd += r;
    }

    // Add null to the end of the string
    buff[bytes_recvd] = '\0';

    return bytes_recvd;
}