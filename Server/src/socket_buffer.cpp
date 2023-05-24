#include "socket_buffer.hpp"


void socket_buffer_init(SocketBuffer* sb) {
    pthread_mutex_init(&sb->lock, NULL);
    pthread_cond_init(&sb->cond, NULL);
    sb->destroyed = 0;
}

void socket_buffer_push(SocketBuffer* sb, int sock) {
    pthread_mutex_lock(&sb->lock);
    sb->q.push(sock);
    pthread_cond_signal(&sb->cond);
    pthread_mutex_unlock(&sb->lock);
}


int socket_buffer_pop(SocketBuffer* sb) {
    int sock;
    
    pthread_mutex_lock(&sb->lock);
    while (sb->q.empty() && !sb->destroyed) {
        pthread_cond_wait(&sb->cond, &sb->lock);
    }

    if (sb->q.empty()) {
        pthread_mutex_unlock(&sb->lock);
        return -1;
    }

    sock = sb->q.front();
    sb->q.pop();
    pthread_mutex_unlock(&sb->lock);

    return sock;
}

void socket_exit_signal(SocketBuffer* sb) {
    sb->destroyed = 1;
    pthread_cond_broadcast(&sb->cond);
}


void socket_buffer_destroy(SocketBuffer* sb) {
    pthread_mutex_destroy(&sb->lock);
    pthread_cond_destroy(&sb->cond);
}