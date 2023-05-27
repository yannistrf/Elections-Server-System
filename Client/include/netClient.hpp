#pragma once

int create_conn(struct sockaddr_in* servaddr);
int send_msg(int sock, char* msg);
int recv_msg(int sock, char* buff);