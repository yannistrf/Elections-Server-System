#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include "netServer.hpp"
#include "socket_buffer.hpp"
#include "poll.hpp"
#include "threadsServer.hpp"


int set_sig_handler();
int get_args(int, char**, int*, int*, int*, char**, char**);
int online = 1;

int main(int argc, char** argv) {

    int portNum;
    int numWorkerThreads;
    int bufferSize;
    char* poll_log;
    char* poll_stats;

    SocketBuffer sockbuff;
    Poll poll;

    if (set_sig_handler() == -1) {
        fprintf(stderr, "Couldn't set the signal handler.\n");
        exit(EXIT_FAILURE);
    }

    if (get_args(argc, argv, &portNum, &numWorkerThreads, &bufferSize, &poll_log, &poll_stats) == -1)
        exit(EXIT_FAILURE);

    int accept_sock;
    accept_sock = accept_sock_init(portNum);

    switch (accept_sock) {
        case SOCKERR:
            perror("sock");
            exit(EXIT_FAILURE);
        case BINDERR:
            perror("bind");
            exit(EXIT_FAILURE);
        case LISTENERR:
            perror("listen");
            exit(EXIT_FAILURE);
        default:
            break;
    }

    socket_buffer_init(&sockbuff, bufferSize);
    poll_init(&poll, poll_log);
    
    ThreadInfo info;
    info.sockbuff = &sockbuff;
    info.poll = &poll;
    pthread_t* threads =  create_threads(numWorkerThreads, &info);
    printf("[THREADS INITIALISED]\n");
    
    int count = 0;
    while (online) {
        socket_buffer_accept_ready(&sockbuff);
        int client_sock;
        if ((client_sock = accept_conn(accept_sock)) < 0) {
            if (client_sock == SHUTDOWN) {
                printf("Caught sig\n");
                continue;
            }
            perror("accept connection");
            exit(EXIT_FAILURE);
        }
        count++;
        socket_buffer_push(&sockbuff, client_sock);
    }

    close(accept_sock);
    socket_exit_signal(&sockbuff);
    destroy_threads(threads, numWorkerThreads);
    socket_buffer_destroy(&sockbuff);
    poll_end(&poll, poll_stats);
    printf("Incoming connections: %d\n", count);
    printf("Terminated properly\n");

    return 0;

}

int get_args(int argc, char** argv, int* portNum, int* numWorkerThreads, int* bufferSize, char** poll_log, char** poll_stats) {

    if (argc != 6) {
        fprintf(stderr, "Usage: ./poller [portnum] [numWorkerthreads] [bufferSize] [poll-log] [poll-stats]\n");
        return -1;
    }

    if (!(*portNum = atoi(argv[1])) || *portNum < 0) {
        fprintf(stderr, "portNum must be a positive number\n");
        return -1;
    }

    if (!(*numWorkerThreads = atoi(argv[2])) || *numWorkerThreads <= 0) {
        fprintf(stderr, "numWorkerThreads must be a positive number\n");
        return -1;
    }

    if (!(*bufferSize = atoi(argv[3])) || *bufferSize <= 0) {
        fprintf(stderr, "bufferSize must be a positive number\n");
        return -1;
    }

    *poll_log = argv[4];
    *poll_stats = argv[5];

    return 0;
}
