#include <stdlib.h>
#include <signal.h>

extern int online;

void sig_handle(int signo) {
    
    online = 0;
}

// Sets the above signal handler for SIGINT.
// Also ignores SIGPIPE in order to handle it later.
// Returns 0 on success, otherwise -1.
int set_sig_handler() {

    struct sigaction sigact = {0};
    sigfillset(&(sigact.sa_mask));

    sigact.sa_handler = sig_handle;
    if (sigaction(SIGINT, &sigact, NULL) == -1)
        return -1;
    
    sigact.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sigact, NULL) == -1)
        return -1;
    
    return 0;
}

// Whichever thread calls this won't be able to catch Ctrl+C
// Gets called from all of the worker threads
int set_sig_thread_mask() {

    sigset_t mask;
	sigemptyset(&mask); 
    sigaddset(&mask, SIGINT); 
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0)
        return -1;
    
    return 0;
}