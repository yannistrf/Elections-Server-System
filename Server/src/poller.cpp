#include <iostream>

using namespace std;

int get_args(
                int argc,
                char** argv, int* portNum,
                int* numWorkerThreads, int* bufferSize,
                char** poll_log, char** poll_stats
            );

int main(int argc, char** argv) {

    int portNum;
    int numWorkerThreads;
    int bufferSize;
    char* poll_log;
    char* poll_stats;

    if (get_args(argc, argv, &portNum, &numWorkerThreads, &bufferSize, &poll_log, &poll_stats))
        exit(-1);
    

}

int get_args(   int argc,
                char** argv, int* portNum,
                int* numWorkerThreads, int* bufferSize,
                char** poll_log, char** poll_stats
            ) {

    if (argc != 6) {
        cerr << "Usage: ./poller [portnum] [numWorkerthreads] [bufferSize] [poll-log] [poll-stats]" << endl;
        return -1;
    }

    if (!(*portNum = atoi(argv[1])) || *portNum < 0) {
        cerr << "portNum must be a positive number" << endl;
        return -1;
    }

    if (!(*numWorkerThreads = atoi(argv[2])) || *numWorkerThreads <= 0) {
        cerr << "numWorkerThreads must be a positive number" << endl;
        return -1;
    }

    if (!(*bufferSize = atoi(argv[3])) || *bufferSize <= 0) {
        cerr << "bufferSize must be a positive number" << endl;
        return -1;
    }

    *poll_log = argv[4];
    *poll_stats = argv[5];

    return 0;
}