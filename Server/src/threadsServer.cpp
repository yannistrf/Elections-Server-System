#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include "threadsServer.hpp"
#include "netServer.hpp"

int set_sig_thread_mask();

int send_msg_wrapper(int sock, char* msg, char* buff) {
    
    bzero(buff, BUFFSIZE);
    strcpy(buff, msg);
    if (send_msg(sock, buff) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int recv_msg_wrapper(int sock, char* msg, char* buff) {

    bzero(buff, BUFFSIZE);
    if (recv_msg(sock, buff) == -1) {
        perror("recv");
        return -1;
    }

    strcpy(msg, buff);
    return 0;
}

// All worker threads will execute this function.
// In case of an error with the communication, terminate
// connection and wait for new connections.
void* client_handler(void* ptr) {
    
    // block SIGINT
    if (set_sig_thread_mask() != 0)
        fprintf(stderr, "Couldn't set signal mask for thread %ld. Continuing execution.\n", pthread_self());

    // Extract info from ThreadInfo
    ThreadInfo* info = (ThreadInfo*) ptr;
    SocketBuffer* sockbuff = info->sockbuff;
    Poll* poll = info->poll;

    while (1) {

        int sock;
        // Get connection from buffer, check for server shutdown
        if ((sock = socket_buffer_pop(sockbuff)) == -1)
            pthread_exit(NULL);

        char buff[BUFFSIZE];
        char name[BUFFSIZE/2];
        char party[BUFFSIZE/2];

        if (send_msg_wrapper(sock, (char*) "SEND NAME PLEASE", buff) == -1) {
            close(sock);
            continue;
        }
       
        if (recv_msg_wrapper(sock, name, buff) == -1) {
            close(sock);
            continue;
        }
       
        // Check if the voter has already voted
        if (poll_has_voted(poll, name)) {
            send_msg_wrapper(sock, (char*) "ALREADY VOTED", buff);
            close(sock);
            continue;  
        }

        if (send_msg_wrapper(sock, (char*) "SEND VOTE PLEASE", buff) == -1) {
            close(sock);
            continue;
        }        

        if (recv_msg_wrapper(sock, party, buff) == -1) {
            close(sock);
            continue;
        }

        // Record vote
        poll_vote(poll, name, party);

        // make "VOTE FOR XYZ RECORDED" message
        char final_msg[BUFFSIZE];
        strcpy(final_msg, "VOTE FOR ");
        strcat(final_msg, party);
        strcat(final_msg, " RECORDED");
        
        if (send_msg_wrapper(sock, final_msg, buff) == -1) {
            close(sock);
            continue;
        }

        close(sock);
    }

    pthread_exit(NULL);
}

// Creates threads, begins their execution and returns the array with their ids.
pthread_t* create_threads(int num_of_threads, ThreadInfo* info) {
    
    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * num_of_threads);
    for (int i = 0; i < num_of_threads; i++) {
        pthread_create(&threads[i], NULL, client_handler, (void*) info);
    }

    return threads;
}

// Waits for all of the threads
void destroy_threads(pthread_t* threads, int num_of_threads) {

    for (int i = 0; i < num_of_threads; i++)
        pthread_join(threads[i], NULL);
    
    free(threads);
}