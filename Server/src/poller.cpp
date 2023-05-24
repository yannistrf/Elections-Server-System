#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include "sig_exit.hpp"
#include "net.hpp"
#include "util_ser.hpp"
#include "socket_buffer.hpp"
#include "poll.hpp"
#include "threads.hpp"


int online = 1;

int main(int argc, char** argv) {

    int portNum;
    int numWorkerThreads;
    int bufferSize;
    char* poll_log;
    char* poll_stats;

    SocketBuffer sockbuff;
    Poll poll;

    set_sig_handler();

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

    socket_buffer_init(&sockbuff);
    poll_init(&poll, poll_log);
    
    ThreadInfo info;
    info.sockbuff = &sockbuff;
    info.poll = &poll;
    pthread_t* threads =  create_threads(numWorkerThreads, &info);
    printf("[THREADS INITIALISED]\n");

    while (online) {
        int client_sock;
        if ((client_sock = accept_conn(accept_sock)) < 0) {
            if (client_sock == SHUTDOWN) {
                printf("Caught sig\n");
                continue;
            }
            perror("accept connection");
        }        
        socket_buffer_push(&sockbuff, client_sock);
    }

    close(accept_sock);
    socket_exit_signal(&sockbuff);
    destroy_threads(threads, numWorkerThreads);
    socket_buffer_destroy(&sockbuff);
    poll_end(&poll, poll_stats);
    printf("Terminated properly\n");

    return 0;

}
