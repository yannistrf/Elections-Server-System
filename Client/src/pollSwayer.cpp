#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "util_cl.hpp"
#include "net_cl.hpp"

typedef struct {
    char* name;
    char* lastName;
    char* party;
} ThreadInfo;

struct sockaddr_in servaddr;

void* handler(void* ptr) {
    ThreadInfo* info = (ThreadInfo*) ptr;
    // printf("%ld: %s %s %s\n", pthread_self(), info->name, info->lastName, info->party);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) == 0)
        printf("success\n");

    size_t size = 256;
    char buff[size];

    bzero(buff, size);
    recv_msg(sock, buff);

    bzero(buff, size);
    strcpy(buff, info->name);
    strcat(buff, " ");
    strcat(buff, info->lastName);
    send_msg(sock, buff);

    bzero(buff, size);
    recv_msg(sock, buff);

    bzero(buff, size);
    send_msg(sock, info->party);

    bzero(buff, size);
    recv_msg(sock, buff);

    free(info->name);
    free(info->lastName);
    free(info->party);
    free(info);
}

int main(int argc, char** argv) {

    char* serverName;
    int portNum;
    char* inputFile;

    if (get_args(argc, argv, &serverName, &portNum, &inputFile) == -1) {
        exit(1);
    }

    struct addrinfo hints, *res;
    int error;
    memset(&hints, '\0', sizeof(hints));

    if (getaddrinfo(serverName, NULL, &hints, &res) != 0) {
        fprintf(stderr, "Could not resolve server name.\n");
        exit(EXIT_FAILURE);
    }
    // char ip[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &res->ai_addr->sa_data[2], ip, INET_ADDRSTRLEN);
    // printf("%s -> %s\n", argv[1], ip);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = res->ai_addr->sa_data[2];
    servaddr.sin_port = htons(portNum);

    // size_t size = 256;
    // char* buff = (char*) malloc(size * sizeof(char));

    // bzero(buff, 256);
    // recv_msg(sock, buff);
    // printf("Received: %s\n", buff);
    
    // printf("Send: ");
    // bzero(buff, 256);
    // getline(&buff, &size, stdin);
    // buff[strlen(buff)-1] = '\0';
    // send_msg(sock, buff);

    // bzero(buff, 256);
    // recv_msg(sock, buff);
    // printf("Received: %s\n", buff);
    
    // printf("Send: ");
    // bzero(buff, 256);
    // getline(&buff, &size, stdin);
    // buff[strlen(buff)-1] = '\0';
    // send_msg(sock, buff);

    // bzero(buff, 256);
    // recv_msg(sock, buff);
    // printf("Received: %s\n", buff);

    FILE* fp = fopen(inputFile, "r");
    char name[256];
    char lastName[256];
    char party[256];

    pthread_t* threads = NULL;
    int thread_count = 0;
    while (fscanf(fp, "%s %s %s\n", name, lastName, party) != EOF) {
        // printf("%s %s %s\n", name, lastName, party);
        ThreadInfo* info = (ThreadInfo*) malloc(sizeof(ThreadInfo));
        info->name = (char*) malloc(strlen(name) + 1);
        info->lastName = (char*) malloc(strlen(lastName) + 1);
        info->party = (char*) malloc(strlen(party) + 1);
        strcpy(info->name, name);
        strcpy(info->lastName, lastName);
        strcpy(info->party, party);
        
        thread_count++;
        threads = (pthread_t*) realloc(threads, thread_count * sizeof(pthread_t));
        pthread_create(&threads[thread_count-1], NULL, handler, (void*) info);
    }

    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    free(threads);
    fclose(fp);
    freeaddrinfo(res);
    return 0;
}