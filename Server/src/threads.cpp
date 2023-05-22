#include <stdlib.h>
#include "threads.hpp"
#include <stdio.h>

void create_threads(pthread_t* threads, int num_of_threads, void* (*func)(void*)) {
    
    for (int i = 0; i < num_of_threads; i++) {
        pthread_create(&threads[i], NULL, func, NULL);
    }
}

void destroy_threads(pthread_t* threads, int num_of_threads) {

    for (int i = 0; i < num_of_threads; i++)
        pthread_join(threads[i], NULL);
    
}