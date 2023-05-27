#include <stdio.h>
#include <stdlib.h>

#include "utilClient.hpp"


int get_args(   int argc,
                char** argv, char** serverName,
                int* portNum, char** inputFile
            ) {

    if (argc != 4) {
        fprintf(stderr, "Usage: ./pollSwayer [serverName] [portNum] [inputFile.txt]\n");
        return -1;
    }

    *serverName = argv[1];

    if (!(*portNum = atoi(argv[2])) || *portNum < 0) {
        fprintf(stderr, "portNum must be a positive number");
        return -1;
    }

    *inputFile = argv[3];

    return 0;
}