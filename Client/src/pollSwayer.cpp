#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "netClient.hpp"
#include "threadsClient.hpp"

int get_args(int, char**, char**, int*, char**);

int main(int argc, char** argv) {

    char* serverName;
    int portNum;
    char* inputFile;

    // Extract command line arguments
    if (get_args(argc, argv, &serverName, &portNum, &inputFile) == -1)
        exit(EXIT_FAILURE);

    // Handle broken pipe
    struct sigaction sigact = {0};
    sigact.sa_handler = SIG_IGN;
    sigfillset(&(sigact.sa_mask));
    sigaction(SIGPIPE, &sigact, NULL);

    // Pass server info
    struct sockaddr_in servaddr;
    if (resolve_server_name(serverName, &servaddr) == -1) {
        fprintf(stderr, "Couldn't resolve server name.\n");
        exit(EXIT_FAILURE);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portNum);

    // Open votes file
    FILE* fp = fopen(inputFile, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char name[256];
    char lastName[256];
    char party[256];

    pthread_t* threads = NULL;
    int thread_count = 0;
    // Create a ThreadInfo for each thread and begin its execution
    while (fscanf(fp, "%s %s %s\n", name, lastName, party) != EOF) {
        ThreadInfo* info = create_info(name, lastName, party, &servaddr);        
        threads = add_thread(threads, &thread_count, info);
    }

    destroy_threads(threads, thread_count);
    fclose(fp);

    return 0;
}

int get_args(int argc, char** argv, char** serverName, int* portNum, char** inputFile) {

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