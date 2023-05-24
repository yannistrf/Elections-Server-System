#include <stdio.h>
#include "poll.hpp"

void poll_init(Poll* poll, char* poll_log) {
    poll->log = fopen(poll_log, "w");
}

int poll_has_voted(Poll* poll, char* name) {
    if (poll->peopleVoted.find(name) == poll->peopleVoted.end())
        return 0;
    
    return 1;
}

void poll_vote(Poll* poll, char* name, char* party) {
    poll->peopleVoted.insert(name);
    fprintf(poll->log, "%s %s\n", name, party);

    if (poll->voteCounter.count(party)) {
        poll->voteCounter[party]++;
    }
    else {
        poll->voteCounter[party] = 1;
    }
}

void poll_end(Poll* poll, char* poll_stats) {
    FILE* stats = fopen(poll_stats, "w");
    
    for (auto it = poll->voteCounter.begin(); it != poll->voteCounter.end(); ++it) {
        fprintf(stats, "%s ->  %d\n", it->first.c_str(), it->second);
    } 
    
    fclose(poll->log);
    fclose(stats);
}