#include <stdio.h>
#include <signal.h>

#include "sig_exit.hpp"

extern int online;

void sig_handle(int signo) {
    printf("\n[SERVER SHUTDOWN]\n");
    online = 0;
}

int set_sig_handler() {

    struct sigaction sigact = {0};
    sigact.sa_handler = sig_handle;
    sigfillset(&(sigact.sa_mask));

    return sigaction(SIGINT, &sigact, NULL);
}