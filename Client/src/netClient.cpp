#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "netClient.hpp"

#define HEADER_SIZE sizeof(int)

// Given the server's name, fills the ip address in the sockaddr_in struct.
// Returns -1 in case the server name could not be resolved.
int resolve_server_name(char* serverName, struct sockaddr_in* servaddr) {

    struct addrinfo* res;
    if (getaddrinfo(serverName, NULL, NULL, &res) != 0)
        return -1;

    servaddr->sin_addr.s_addr = res->ai_addr->sa_data[2];
    freeaddrinfo(res);

    return 0;
}

// Creates the connection with server and returns the file descriptor of
// the socket. Returns -1 if it was not able to establish the connection.
int create_conn(struct sockaddr_in* servaddr) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr*) servaddr, sizeof(*servaddr)) == -1)
        return -1;

    return sock;
}

// Sends a message to the server, returning the number of bytes sent.
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

// Receives a message from the server, returns the number of bytes read.
// Returns -1 in case of failure.
int recv_msg(int sock, char* buff) {

    int len;
    char header_buff[HEADER_SIZE];

    if (read(sock, header_buff, HEADER_SIZE) == -1)
        return -1;

    len = *header_buff;
    int bytes_recvd = 0;
    while (bytes_recvd < len) {
        int b = read(sock, &buff[bytes_recvd], len-bytes_recvd);
        if (b == -1)
            return -1;
        bytes_recvd += b;
    }

    // Add null to the end of the string
    buff[bytes_recvd] = '\0';

    return bytes_recvd;
}