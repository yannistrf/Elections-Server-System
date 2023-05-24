#pragma once

#include <stdlib.h>
#include <pthread.h>
#include <queue>

typedef struct {
    
    std::queue<int> q;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int destroyed;

} SocketBuffer;

void socket_buffer_init(SocketBuffer* sb);
void socket_buffer_push(SocketBuffer* sb, int sock);
int socket_buffer_pop(SocketBuffer* sb);
void socket_exit_signal(SocketBuffer* sb);
void socket_buffer_destroy(SocketBuffer* sb);