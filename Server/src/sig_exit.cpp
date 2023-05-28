#include <signal.h>

extern int online;

void sig_handle(int signo) {
    
    online = 0;
}

// Sets the above signal handler for SIGINT. Returns 0
// on success, otherwise -1.
int set_sig_handler() {

    struct sigaction sigact = {0};
    sigact.sa_handler = sig_handle;
    sigfillset(&(sigact.sa_mask));

    return sigaction(SIGINT, &sigact, NULL);
}