//
// Created by Karol Majka on 15/05/2017.
//

#include "Agent.h"

Agent::Agent(int size, int rank, int roomCount) {
    this->lamport = new Lamport(size, rank);
    this->roomCount = roomCount;
    srand(time(NULL)+rank*size*1000);

}

void Agent::agentLoop() {
    while(1) {
        Contest contest;
        if (this->wannaCreateContest()) {
            int room = -1;
            do {
                room = this->randomRoom();
            } while (this->roomIsFree(room));
            int time = this->randomTime();
            contest.room = room;
            contest.time = time;

            this->sendInvite(contest;
            this->waitForAnswers();
        } else {
            contest = this->waitForInvite();
            roomCurrent = contest.room;
        }
        this->takePart(contest);
        roomCurrent = -1;
        this->restAfterContest();
    }
}

bool Agent::wannaCreateContest() {
    if (this->lamport->rank == 0) {
        return true;
    } else if (this->lamport->rank == 1) {
        return false;
    } else {
        return ((bool) rand() % 2);
    }
}

int Agent::randomRoom() {
    return rand() % this->roomCount;;
}

int Agent::randomTime() {
    return rand() % 25000 + 5000;
}

bool Agent::roomIsFree(int room) {
    //send broadcast and wait for answers
    return false;
}

void Agent::sendInvite(Contest contest) {
    //send broadcast
}

void Agent::waitForAnswers() {
    //wait for answers
}

Contest Agent::waitForInvite() {
    Contest contest;
    //wait for invite
    return contest;
}

void Agent::takePart(Contest contest) {
    //sleep contest.time
}

void Agent::restAfterContest() {
    //random sleep
}