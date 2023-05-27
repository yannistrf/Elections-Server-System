#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
// #include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "utilClient.hpp"
#include "netClient.hpp"
#include "threadsClient.hpp"

struct sockaddr_in servaddr;

int main(int argc, char** argv) {

    char* serverName;
    int portNum;
    char* inputFile;

    if (get_args(argc, argv, &serverName, &portNum, &inputFile) == -1) {
        exit(1);
    }

    struct addrinfo hints, *res;
    memset(&hints, '\0', sizeof(hints));

    if (getaddrinfo(serverName, NULL, NULL, &res) != 0) {
        fprintf(stderr, "Could not resolve server name.\n");
        exit(EXIT_FAILURE);
    }
    // char ip[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &res->ai_addr->sa_data[2], ip, INET_ADDRSTRLEN);
    // printf("%s -> %s\n", argv[1], ip);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = res->ai_addr->sa_data[2];
    servaddr.sin_port = htons(portNum);


    FILE* fp = fopen(inputFile, "r");
    char name[256];
    char lastName[256];
    char party[256];

    pthread_t* threads = NULL;
    int thread_count = 0;
    while (fscanf(fp, "%s %s %s\n", name, lastName, party) != EOF) {
        ThreadInfo* info = create_info(name, lastName, party, &servaddr);        
        threads = add_thread(threads, &thread_count, info);
    }

    destroy_threads(threads, thread_count);
    fclose(fp);
    freeaddrinfo(res);
    return 0;
}