#pragma once

#include <set>
#include <string>
#include <map>
#include <stdio.h>

/*
    This module is all about storing the results from
    the election process taking into consideration
    the synchronization issues among the threads.
*/
typedef struct {
    
    std::set<std::string> peopleVoted;          // every voter must be unique so use a set
    std::map<std::string, int> voteCounter;     // a map that holds the information party-votes
    pthread_mutex_t lock;                       // a mutex to handle the race conditions for the set and map
    FILE* log;                                  // the file that we write all of the votes

} Poll;

void poll_init(Poll* poll, char* poll_log);
int poll_has_voted(Poll* poll, char* name);
void poll_vote(Poll* poll, char* name, char* party);
void poll_end(Poll* poll, char* poll_stats);