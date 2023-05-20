#include <stdio.h>
#include <stdlib.h>

#include "util_ser.hpp"


int get_args(   int argc,
                char** argv, int* portNum,
                int* numWorkerThreads, int* bufferSize,
                char** poll_log, char** poll_stats
            ) {

    if (argc != 6) {
        fprintf(stderr, "Usage: ./poller [portnum] [numWorkerthreads] [bufferSize] [poll-log] [poll-stats]\n");
        return -1;
    }

    if (!(*portNum = atoi(argv[1])) || *portNum < 0) {
        fprintf(stderr, "portNum must be a positive number\n");
        return -1;
    }

    if (!(*numWorkerThreads = atoi(argv[2])) || *numWorkerThreads <= 0) {
        fprintf(stderr, "numWorkerThreads must be a positive number\n");
        return -1;
    }

    if (!(*bufferSize = atoi(argv[3])) || *bufferSize <= 0) {
        fprintf(stderr, "bufferSize must be a positive number\n");
        return -1;
    }

    *poll_log = argv[4];
    *poll_stats = argv[5];

    return 0;
}