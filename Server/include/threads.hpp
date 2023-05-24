#pragma once

#include <pthread.h>

#include "socket_buffer.hpp"
#include "poll.hpp"

typedef struct {
    SocketBuffer* sockbuff;
    Poll* poll;
} ThreadInfo;

pthread_t* create_threads(int num_of_threads, ThreadInfo* info);
void destroy_threads(pthread_t* threads, int num_of_threads);
