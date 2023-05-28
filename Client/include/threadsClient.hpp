#pragma once

#include <pthread.h>

/*
    The struct to get passed to 
    every thread, holds all the
    neccessary information
*/
typedef struct {
    char* fullName;
    char* party;
    struct sockaddr_in* servaddr;
} ThreadInfo;


ThreadInfo* create_info(char* name, char* lastName, char* party, struct sockaddr_in* servaddr);
pthread_t* add_thread(pthread_t* threads, int* thread_count, ThreadInfo* info);
void destroy_threads(pthread_t* threads, int thread_count);