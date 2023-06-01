#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "threadsClient.hpp"
#include "netClient.hpp"

ThreadInfo* create_info(char* name, char* lastName, char* party, struct sockaddr_in* servaddr) {

    // Allocate space    
    ThreadInfo* info = (ThreadInfo*) malloc(sizeof(ThreadInfo));
    info->fullName = (char*) malloc(strlen(name) + strlen(lastName) + 2);
    info->party = (char*) malloc(strlen(party) + 1);
    info->servaddr = servaddr;

    // Concatenate name and lastName
    strcpy(info->fullName, name);
    strcat(info->fullName, " ");
    strcat(info->fullName, lastName);
    strcpy(info->party, party);

    return info;
}

// Frees all the space occupied from ThreadInfo.
void destroy_info(ThreadInfo* info) {
    
    free(info->fullName);
    free(info->party);
    free(info);
}

// Releases thread's recources.
void exit_thread(ThreadInfo* info, int sock) {
    
    destroy_info(info);
    close(sock);
}

/**************************************************************************/
/*********************** MESSAGE HANDLING FUNCTIONS ***********************/

// All of the below return -1 if something unexpected happens

int recv_send_name_msg(int sock, char* buff) {
    
    bzero(buff, BUFFSIZE);
    if (recv_msg(sock, buff) == -1) {
        perror("recv");
        return -1;
    }

    if (strcmp(buff, "SEND NAME PLEASE")) {
        fprintf(stderr, "Unknown message received, exiting...\n");
        return -1;
    }

    return 0;
}

int send_fullname(int sock, char* fullname, char* buff) {
    
    bzero(buff, BUFFSIZE);
    strcpy(buff, fullname);

    if (send_msg(sock, buff) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int recv_send_vote_msg(int sock, char* buff) {

    bzero(buff, BUFFSIZE);
    if (recv_msg(sock, buff) == -1) {
        perror("recv");
        return -1;
    }

    if (!strcmp(buff, "ALREADY VOTED")) {
        return -1;
    }

    if (strcmp(buff, "SEND VOTE PLEASE")) {
        fprintf(stderr, "Unknown message received, exiting...\n");
        return -1;
    }

    return 0;
}

int send_vote(int sock, char* vote, char* buff) {
    
    bzero(buff, BUFFSIZE);
    strcpy(buff, vote);

    if (send_msg(sock, buff) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int recv_vote_recorded_msg(int sock, char* party, char* buff) {

    bzero(buff, BUFFSIZE);
    if (recv_msg(sock, buff) == -1) {
        perror("recv");
        return -1;
    }

    char exp_msg[BUFFSIZE];
    bzero(exp_msg, BUFFSIZE);
    strcpy(exp_msg, "VOTE FOR ");
    strcat(exp_msg, party);
    strcat(exp_msg, " RECORDED");

    if (strcmp(buff, exp_msg)) {
        fprintf(stderr, "Unknown message received, exiting...\n");
        return -1;
    }

    return 0;
}

/**************************************************************************/
/**************************************************************************/

// The thread function.
void* handler(void* ptr) {

    ThreadInfo* info = (ThreadInfo*) ptr;

    // Try to connect to the server
    int sock;
    if ((sock = create_conn(info->servaddr)) == -1) {
        fprintf(stderr, "Couldn't connect to server.\n");
        destroy_info(info);
        pthread_exit(NULL);
    }

    char buff[BUFFSIZE];

    if (recv_send_name_msg(sock, buff) == -1) {
        exit_thread(info, sock);
        pthread_exit(NULL);
    }

    if (send_fullname(sock, info->fullName, buff) == -1) {
        exit_thread(info, sock);
        pthread_exit(NULL);
    }

    if (recv_send_vote_msg(sock, buff) == -1) {
        exit_thread(info, sock);
        pthread_exit(NULL);
    }

    if (send_vote(sock, info->party, buff) == -1) {
        exit_thread(info, sock);
        pthread_exit(NULL);
    }
   
    if (recv_vote_recorded_msg(sock, info->party, buff) == -1) {
        exit_thread(info, sock);
        pthread_exit(NULL);
    }

    exit_thread(info, sock);
    pthread_exit(NULL);
}

// Adds a thread to the thread array and begins its execution.
pthread_t* add_thread(pthread_t* threads, int* thread_count, ThreadInfo* info) {
    
    (*thread_count)++;
    threads = (pthread_t*) realloc(threads, (*thread_count) * sizeof(pthread_t));
    pthread_create(&threads[*thread_count-1], NULL, handler, (void*) info);
    
    return threads;
}

// Collects all of the threads and frees thread array.
void destroy_threads(pthread_t* threads, int thread_count) {
    
    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    free(threads);
}