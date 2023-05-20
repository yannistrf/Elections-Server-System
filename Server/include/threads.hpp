#include <pthread.h>

void create_threads(pthread_t* threads, int num_of_threads, void* (*func)(void*));
void destroy_threads(pthread_t* threads, int num_of_threads);
