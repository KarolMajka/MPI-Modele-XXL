//
// Created by Karol Majka on 15/05/2017.
//

#ifndef MODELEXXL_LAMPORT_H
#define MODELEXXL_LAMPORT_H
#include <string>
#include <iostream>
#include <mpi.h>
using std::string;

enum MessageTag {
    Invite,
    AnswerInvite,
    Room,
    AnswerRoom,
    StartContest
};

struct Contest {
    int room;
    time_t time;
};

struct Message {
    Contest contest;
    int processId;
    int lamportClock;
    bool answer;
};

class Demand {
public:
    int id;
    int clock;
    Demand(int i, int c):id(i),clock(c){}
};

class Lamport {

protected:
    int timestamp = 0;

public:
    int size;
    int rank;
    Lamport(int size, int rank);
    void broadcast(Message message, MessageTag tag);
    void increment();
    int getTimestamp();
    void sendMessage(int to, Message message, MessageTag tag);


protected:

};


#endif //MODELEXXL_LAMPORT_H
