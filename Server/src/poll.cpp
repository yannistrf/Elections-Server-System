#include <stdio.h>

#include "poll.hpp"

void poll_init(Poll* poll, char* poll_log) {

    pthread_mutex_init(&poll->lock, NULL);
    poll->log = fopen(poll_log, "w");
}

// Returns 1 if the voter has voted before, 0 otherwise
int poll_has_voted(Poll* poll, char* name) {

    // Critical segment
    pthread_mutex_lock(&poll->lock);
    
    // check if the voter has voted before
    int has_voted = !(poll->peopleVoted.find(name) == poll->peopleVoted.end());
    
    pthread_mutex_unlock(&poll->lock);

    return has_voted;
}

// Stores the voter-party pair and increments the vote count for the party
void poll_vote(Poll* poll, char* name, char* party) {

    // Critical segment
    pthread_mutex_lock(&poll->lock);
    
    poll->peopleVoted.insert(name);
    fprintf(poll->log, "%s %s\n", name, party);
    fflush(poll->log);

    if (poll->voteCounter.count(party))
        poll->voteCounter[party]++;
    else    // if it's the first vote of the party
        poll->voteCounter[party] = 1;

    pthread_mutex_unlock(&poll->lock);
}

void poll_end(Poll* poll, char* poll_stats) {

    FILE* stats = fopen(poll_stats, "w");
    std::string first_party;
    int votes = 0;

    // write the results from the elections to the file
    // also search for the party with the most votes
    for (auto it = poll->voteCounter.begin(); it != poll->voteCounter.end(); ++it) {
        
        fprintf(stats, "%s %d\n", it->first.c_str(), it->second);
        
        if (it->second > votes) {
            first_party = it->first.c_str();
            votes = it->second;
        }
    } 
    
    printf("\nparticipants: %ld\n", poll->peopleVoted.size());
    // print the winner of the elections
    printf("\n*** WINNER ***\n");
    printf("%s %d\n\n", first_party.c_str(), votes);
    
    fclose(poll->log);
    fclose(stats);
    pthread_mutex_destroy(&poll->lock);
}