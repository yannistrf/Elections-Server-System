#pragma once

// #include <stdlib.h>
#include <pthread.h>
#include <queue>

/*
    This module handles the storage and access
    of the socket descriptors taking into consideration
    the synchronization issues among the threads.
*/
typedef struct {
    
    std::queue<int> q;              // FIFO policy so use a queue
    long unsigned int max_size;     // max queue size
    pthread_mutex_t access_lock;    // mutex for the push-pop condition variable
    pthread_cond_t access_cond;     // push-pop condition variable, blocks if the buffer is empty
    pthread_mutex_t size_lock;      // mutex for the max size condition variable
    pthread_cond_t size_cond;       // max size condition variable, blocks if the buffer is full
    int closed;                     // indicates if we stoped accepting new connections

} SocketBuffer;

void socket_buffer_init(SocketBuffer* sb, int max_size);
void socket_buffer_accept_ready(SocketBuffer* sb);
void socket_buffer_push(SocketBuffer* sb, int sock);
int socket_buffer_pop(SocketBuffer* sb);
void socket_exit_signal(SocketBuffer* sb);
void socket_buffer_destroy(SocketBuffer* sb);