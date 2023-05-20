#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <queue>
#include <signal.h>

#include "net.hpp"
#include "util_ser.hpp"
#include "threads.hpp"
#include "socket_buffer.hpp"

using namespace std;


SocketBuffer sockbuff;

void sig_handle(int signo) {
    printf("Caught signal: %d\n", signo);
    exit(EXIT_SUCCESS);
}

void* hello(void* x) {
    // while (1) {
        int sock = socket_buffer_pop(&sockbuff);
        printf("Got sock %d\n", sock);
    // }
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

    // printf("connected\n");

    // size_t size = 256;
    // char* buff = (char*) malloc(size * sizeof(char));

    // while (1) {
    //     bzero(buff, 256);
    //     recv_msg(client_sock, buff);
    //     printf("Received: %s\n", buff);
    //     bzero(buff, 256);
    //     printf("Send: ");
    //     getline(&buff, &size, stdin);
    //     send_msg(client_sock, buff);
    // }

    socket_buffer_init(&sockbuff);

    pthread_t* threads = (pthread_t*) malloc(numWorkerThreads * sizeof(pthread_t));
    create_threads(threads, 3, hello);

    int k = 0;
    while (k < 3) {
        int client_sock;
        if ((client_sock = accept_conn(accept_sock)) == -1) {
            exit(1);
        }
        socket_buffer_push(&sockbuff, client_sock);
        k++;
    }

    destroy_threads(threads, 3);
    printf("Finished\n");
}
