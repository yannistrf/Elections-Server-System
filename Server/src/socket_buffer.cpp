#include "socket_buffer.hpp"
#include <stdio.h>

void socket_buffer_init(SocketBuffer* sb, int max_size) {

    sb->max_size = max_size;
    pthread_mutex_init(&sb->access_lock, NULL);
    pthread_cond_init(&sb->access_cond, NULL);
    pthread_mutex_init(&sb->size_lock, NULL);
    pthread_cond_init(&sb->size_cond, NULL);
    sb->closed = 0;
}

// When the main thread returns from this function,
// it is guaranteed that the buffer is not full
void socket_buffer_accept_ready(SocketBuffer* sb) {

    pthread_mutex_lock(&sb->size_lock);
    // if buffer is full go to sleep
    while (sb->q.size() == sb->max_size) {
        printf("Master going to sleep\n");
        pthread_cond_wait(&sb->size_cond, &sb->size_lock);
    }

    pthread_mutex_unlock(&sb->size_lock);
}

// Allows main thread to insert a socket in the buffer
void socket_buffer_push(SocketBuffer* sb, int sock) {

    printf("trying to push %d\n", sock);
    // Critical segment
    pthread_mutex_lock(&sb->access_lock);

    sb->q.push(sock);
    printf("push sock %d\n", sock);
    // printf("size = %ld\n", sb->q.size());
    // wake up a child thread to handle the connection 
    pthread_cond_signal(&sb->access_cond);
    
    pthread_mutex_unlock(&sb->access_lock);
}

// Returns the next socket to the child threads
int socket_buffer_pop(SocketBuffer* sb) {
    int sock;
    
    pthread_mutex_lock(&sb->access_lock);
    // if buffer is empty and we are still accepting
    // connections go to sleep
    while (sb->q.empty() && !sb->closed) {
        printf("going to sleep %ld\n", pthread_self());
        pthread_cond_wait(&sb->access_cond, &sb->access_lock);
    }

    // will get in here only if the socket buffer
    // got closed, aka received SIGINT
    if (sb->q.empty()) {
        pthread_mutex_unlock(&sb->access_lock);
        return -1;
    }

    sock = sb->q.front();
    sb->q.pop();
    printf("poped %d %ld\n", sock, pthread_self());

    // if the buffer was full, notify the main thread to accept
    // more connections, need to lock to avoid undefined behavior
    if (sb->q.size() == sb->max_size - 1) {
        pthread_mutex_lock(&sb->size_lock);
        pthread_cond_signal(&sb->size_cond);
        pthread_mutex_unlock(&sb->size_lock);
    }

    pthread_mutex_unlock(&sb->access_lock);

    return sock;
}

// Need to signal the threads that we no longer accept connections
// so that after they take care of the incoming connections they
// can exit. Need to lock to avoid undefined behavior.
void socket_exit_signal(SocketBuffer* sb) {

    pthread_mutex_lock(&sb->access_lock);
    sb->closed = 1;
    pthread_cond_broadcast(&sb->access_cond);
    pthread_mutex_unlock(&sb->access_lock);
}


void socket_buffer_destroy(SocketBuffer* sb) {

    pthread_mutex_destroy(&sb->access_lock);
    pthread_cond_destroy(&sb->access_cond);
}