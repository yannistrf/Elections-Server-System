#pragma once

#include <pthread.h>
// #include <arpa/inet.h>

typedef struct {
    char* fullName;
    char* party;
    struct sockaddr_in* servaddr;
} ThreadInfo;


ThreadInfo* create_info(char* name, char* lastName, char* party, struct sockaddr_in* servaddr);
void destroy_info(ThreadInfo* info);
pthread_t* add_thread(pthread_t* threads, int* thread_count, ThreadInfo* info);
void destroy_threads(pthread_t* threads, int thread_count);