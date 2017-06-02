//
// Created by Karol Majka on 15/05/2017.
//

#ifndef MODELEXXL_AGENT_H
#define MODELEXXL_AGENT_H

#include <string>
#include <iostream>
#include "Lamport.h"

enum State {
    IDLE,
    BUSY,
    REST,
    WAITING_FOR_ANSWERS,
    WAITING_FOR_ROOM,

};

class Agent {
protected:
    int selectedRoom;
    int answerCount;
    int roomCount;

    time_t restTill;

    Contest currentContest;
    State state;
    Lamport *lamport;

    bool wannaCreateContest();
    int randomRoom();
    void askForRoom();
    int randomTime();
    bool wannaJoin(Message m);
    bool shouldBeFirst(Message m);
    void handleMsg();
    void doStuff();
    void handleRoom(Message m);
    void startContest(Contest c);
    void handleInvite(Message m);
    void handleStartContest(Message m);
    void handleAnswerInvite(Message m);
    void handleAnswerRoom(Message m);
    void answerInvite(Message m, bool answer);
    void sendInvite(Contest contest);

public:
    Agent(int size, int rank, int roomCount);
    void agentLoop();
};


#endif //MODELEXXL_AGENT_H
