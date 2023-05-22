#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <queue>
#include <signal.h>
#include <string.h>
#include <map>
#include <string>

#include "net.hpp"
#include "util_ser.hpp"
#include "threads.hpp"
#include "socket_buffer.hpp"

#include <iostream>
using namespace std;


SocketBuffer sockbuff;
map<string, string> votes;

void sig_handle(int signo) {
    printf("Caught signal: %d\n", signo);
    for (auto it = votes.cbegin(); it != votes.cend(); ++it) {
        cout << "{" << (*it).first << ": " << (*it).second << "}\n";
    }

    exit(EXIT_SUCCESS);

}

void* client_handler(void* x) {

    int sock = socket_buffer_pop(&sockbuff);
    printf("Accepted connection: %d\n", sock);
    size_t size = 256;
    char buff[size];

    bzero(buff, size);
    strcpy(buff, "SEND NAME PLEASE");
    send_msg(sock, buff);

    bzero(buff, size);
    recv_msg(sock, buff);
    printf("Received name: %s\n", buff);
    char* name = (char*) malloc(strlen(buff) + 1);
    strcpy(name, buff);

    bzero(buff, size);
    strcpy(buff, "SEND VOTE PLEASE\0");
    send_msg(sock, buff);
    
    bzero(buff, size);
    recv_msg(sock, buff);
    printf("Received vote: %s\n", buff);

    char* party = (char*) malloc(strlen(buff) + 1);
    strcpy(party, buff);
    bzero(buff, size);
    strcpy(buff, "VOTE FOR \0");
    strcat(buff, party);
    strcat(buff, " RECORDED\0");
    send_msg(sock, buff);
    
    printf("last msg = %s\n", buff);

    votes[name] = party;
    free(name);
    free(party);

    pthread_detach(pthread_self());
}


int main(int argc, char** argv) {

    int portNum;
    int numWorkerThreads;
    int bufferSize;
    char* poll_log;
    char* poll_stats;

    if (get_args(argc, argv, &portNum, &numWorkerThreads, &bufferSize, &poll_log, &poll_stats) == -1)
        exit(1);

    struct sigaction sigact = {0};
    sigact.sa_handler = sig_handle;
    sigfillset(&(sigact.sa_mask));
    sigaction (SIGINT, &sigact, NULL);

    int accept_sock;
    if ((accept_sock = accept_sock_init(portNum)) == -1) {
        exit(1);
    }

    socket_buffer_init(&sockbuff);
    
    pthread_t threads[numWorkerThreads];
    create_threads(threads, 3, client_handler);
    printf("[THREADS INITIALISED]\n");

    while (1) {
        int client_sock;
        if ((client_sock = accept_conn(accept_sock)) == -1) {
            exit(1);
        }        
        socket_buffer_push(&sockbuff, client_sock);
    }

}
