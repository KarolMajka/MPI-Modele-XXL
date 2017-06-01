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
    AnswerRoom
};

struct Contest {
    int room;
    int time;
};

struct Message {
    Contest contest;
    int processID;
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
    void sendMessage(Message message, MessageTag tag);
    void increment();
    int getTimestamp();

protected:
    void sendMessage(int to, Message message, MessageTag tag);

};


#endif //MODELEXXL_LAMPORT_H
