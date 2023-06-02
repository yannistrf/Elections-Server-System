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

    if (set_sig_handler() == -1) {
        fprintf(stderr, "Couldn't set the signal handler.\n");
        exit(EXIT_FAILURE);
    }

    if (get_args(argc, argv, &portNum, &numWorkerThreads, &bufferSize, &poll_log, &poll_stats) == -1)
        exit(EXIT_FAILURE);

    int accept_sock;
    if ((accept_sock = accept_sock_init(portNum)) == -1)
        exit(EXIT_FAILURE);

    
    SocketBuffer sockbuff;
    Poll poll;
    socket_buffer_init(&sockbuff, bufferSize);
    poll_init(&poll, poll_log);
    
    ThreadInfo info;
    info.sockbuff = &sockbuff;
    info.poll = &poll;
    pthread_t* threads = create_threads(numWorkerThreads, &info);
    printf("[THREADS INITIALISED]\n");
    printf("[SERVER LISTENING...]\n");
    
    int count = 0;
    while (online) {
        
        // Wait if the buffer is full
        socket_buffer_accept_ready(&sockbuff);
        // In case SIGINT was received while waiting
        if (!online) continue;

        int client_sock;
        if ((client_sock = accept_conn(accept_sock)) < 0) {
            
            // Check for Ctrl+C
            if (client_sock == SHUTDOWN) {
                printf("\n[INITIATING SHUTDOWN PROCESS...]\n");
                continue;
            }
            
            perror("accept");
            continue;
        }
        count++;
        // Put socket in buffer
        socket_buffer_push(&sockbuff, client_sock);
    }

    close(accept_sock);
    // Signal worker threads that there will be no more incoming connections
    socket_exit_signal(&sockbuff);
    // Wait for threads to finish
    destroy_threads(threads, numWorkerThreads);
    socket_buffer_destroy(&sockbuff);
    // End poll, write results
    poll_end(&poll, poll_stats);

    printf("Connected: %d\n", count);
    printf("[SERVER TERMINATED]\n");

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
