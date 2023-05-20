#include <stdio.h>
#include <stdlib.h>

#include "util_cl.hpp"

int main(int argc, char** argv) {

    char* serverName;
    int portNum;
    char* inputFile;

    if (get_args(argc, argv, &serverName, &portNum, &inputFile) == -1) {
        exit(1);
    }

    return 0;
}