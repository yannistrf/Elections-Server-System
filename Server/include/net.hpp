#pragma once

int accept_sock_init(int port);
int accept_conn(int accept_sock);
int send_msg(int sock, char* msg);
int recv_msg(int sock, char* buff);