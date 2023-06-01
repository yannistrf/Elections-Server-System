#pragma once

#include <pthread.h>

#include "socket_buffer.hpp"
#include "poll.hpp"

/*
    The struct to get passed to 
    every thread, holds all the
    neccessary structs for being
    able to serve a client.
*/
typedef struct {
    
    SocketBuffer* sockbuff;
    Poll* poll;

} ThreadInfo;

pthread_t* create_threads(int num_of_threads, ThreadInfo* info);
void destroy_threads(pthread_t* threads, int num_of_threads);
