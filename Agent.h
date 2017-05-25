//
// Created by Karol Majka on 15/05/2017.
//

#ifndef MODELEXXL_AGENT_H
#define MODELEXXL_AGENT_H

#include <string>
#include <iostream>
#include "Lamport.h"

class Agent {
protected:
    int roomCurrent = -1;
    int roomCount = 0;
    Lamport *lamport;

    bool wannaCreateContest();
    int randomRoom();
    int randomTime();
    bool roomIsFree(int room);
    void sendInvite(Contest contest);
    void waitForAnswers();
    Contest waitForInvite();
    void takePart(Contest contest);
    void restAfterContest();

public:
    Agent(int size, int rank, int roomCount);
    void agentLoop();
};


#endif //MODELEXXL_AGENT_H
