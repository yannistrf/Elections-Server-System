#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>


#include "threadsClient.hpp"
#include "netClient.hpp"

ThreadInfo* create_info(char* name, char* lastName, char* party, struct sockaddr_in* servaddr) {
    
    ThreadInfo* info = (ThreadInfo*) malloc(sizeof(ThreadInfo));
    info->fullName = (char*) malloc(strlen(name) + strlen(lastName) + 2);
    info->party = (char*) malloc(strlen(party) + 1);
    info->servaddr = servaddr;

    strcpy(info->fullName, name);
    strcat(info->fullName, " ");
    strcat(info->fullName, lastName);
    strcpy(info->party, party);

    return info;
}

void destroy_info(ThreadInfo* info) {
    
    free(info->fullName);
    free(info->party);
    free(info);
}

void* handler(void* ptr) {
    
    ThreadInfo* info = (ThreadInfo*) ptr;

    int sock;
    if ((sock = create_conn(info->servaddr)) == -1) {
        fprintf(stderr, "Couldn't connect to server.\n");
        pthread_exit((void*) NULL);
    }

    size_t size = 256;
    char buff[size];

    bzero(buff, size);
    recv_msg(sock, buff);

    bzero(buff, size);
    strcpy(buff, info->fullName);
    send_msg(sock, buff);

    bzero(buff, size);
    recv_msg(sock, buff);
   
    if (strcmp(buff, "ALREADY VOTED")) {
        bzero(buff, size);
        send_msg(sock, info->party);
        bzero(buff, size);
        recv_msg(sock, buff);
    }

    destroy_info(info);
    close(sock);
    pthread_exit((void*) NULL);
}


pthread_t* add_thread(pthread_t* threads, int* thread_count, ThreadInfo* info) {
    
    (*thread_count)++;
    threads = (pthread_t*) realloc(threads, (*thread_count) * sizeof(pthread_t));
    pthread_create(&threads[*thread_count-1], NULL, handler, (void*) info);
    
    return threads;
}

void destroy_threads(pthread_t* threads, int thread_count) {
    
    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    free(threads);
}