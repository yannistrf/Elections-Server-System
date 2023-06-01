#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "netClient.hpp"

#define HEADER_SIZE sizeof(int)     // first msg is an int indicating the second's msg size

// Given the server's name, fills the ip address in the sockaddr_in struct.
// Returns -1 in case the server name could not be resolved.
int resolve_server_name(char* serverName, struct sockaddr_in* servaddr) {

    struct hostent *rem;
    if ((rem = gethostbyname(serverName)) == NULL)
        return -1;

    memcpy(&servaddr->sin_addr, rem->h_addr, rem->h_length);

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


/*
    The protocol we follow for the communication is simple.
    The first message is an integer (4 bytes) which indicates
    the size of the actual message. After that we send the actual
    message. In that way, the recipient will know what to expect.
*/


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