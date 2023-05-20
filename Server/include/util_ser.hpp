#pragma once

int get_args(
                int argc,
                char** argv, int* portNum,
                int* numWorkerThreads, int* bufferSize,
                char** poll_log, char** poll_stats
            );
