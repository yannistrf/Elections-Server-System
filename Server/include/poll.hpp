#pragma once

#include <set>
#include <string>
#include <map>
#include <stdio.h>

typedef struct {
    std::set<std::string> peopleVoted;
    std::map<std::string, int> voteCounter;
    int poll_log_fd;
    FILE* log;
} Poll;

void poll_init(Poll* poll, char* poll_log);
int poll_has_voted(Poll* poll, char* name);
void poll_vote(Poll* poll, char* name, char* party);
void poll_end(Poll* poll, char* poll_stats);