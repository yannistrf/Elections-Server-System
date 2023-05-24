#pragma once

#define SHUTDOWN -2
#define SOCKERR -1
#define BINDERR -2
#define LISTENERR -3

int accept_sock_init(int port);
int accept_conn(int accept_sock);
int send_msg(int sock, char* msg);
int recv_msg(int sock, char* buff);