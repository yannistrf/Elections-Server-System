#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include "threads.hpp"
#include "net.hpp"


void* client_handler(void* ptr) {
    
    ThreadInfo* info = (ThreadInfo*) ptr;
    SocketBuffer* sockbuff = info->sockbuff;
    Poll* poll = info->poll;

    while (1) {
        int sock;
        if ((sock = socket_buffer_pop(sockbuff)) == -1)
            pthread_exit((void*) NULL);

        printf("Accepted connection: %d\n", sock);
        size_t size = 256;
        char buff[size];
        char name[size];
        char party[size];

        bzero(buff, size);
        strcpy(buff, "SEND NAME PLEASE");
        send_msg(sock, buff);

        bzero(buff, size);
        recv_msg(sock, buff);
        printf("Received name: %s\n", buff);
        strcpy(name, buff);
       
        if (poll_has_voted(poll, name)) {
            bzero(buff, size);
            strcpy(buff, "ALREADY VOTED");
            send_msg(sock, buff);
            close(sock);    
            continue;
        }
       
        bzero(buff, size);
        strcpy(buff, "SEND VOTE PLEASE");
        send_msg(sock, buff);
        
        bzero(buff, size);
        recv_msg(sock, buff);
        printf("Received vote: %s\n", buff);
        strcpy(party, buff);
        poll_vote(poll, name, party);

        bzero(buff, size);
        strcpy(buff, "VOTE FOR ");
        strcat(buff, party);
        strcat(buff, " RECORDED");
        send_msg(sock, buff);
        
        close(sock);
    }

    pthread_exit((void*) NULL);
}

pthread_t* create_threads(int num_of_threads, ThreadInfo* info) {
    

    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * num_of_threads);
    for (int i = 0; i < num_of_threads; i++) {
        pthread_create(&threads[i], NULL, client_handler, (void*) info);
    }

    return threads;
}

void destroy_threads(pthread_t* threads, int num_of_threads) {

    for (int i = 0; i < num_of_threads; i++)
        pthread_join(threads[i], NULL);
    
    free(threads);
}