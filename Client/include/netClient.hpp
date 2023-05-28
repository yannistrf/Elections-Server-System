#pragma once

#include <netdb.h>

#define BUFFSIZE 1024

int resolve_server_name(char* serverName, struct sockaddr_in* servaddr);
int create_conn(struct sockaddr_in* servaddr);
int send_msg(int sock, char* msg);
int recv_msg(int sock, char* buff);