//
// Created by Karol Majka on 15/05/2017.
//

#include <zconf.h>
#include "Agent.h"




Agent::Agent(int size, int rank, int roomCount) {
    this->lamport = new Lamport(size, rank);
    this->roomCount = roomCount;
    this->answerCount = size-1;
    this->selectedRoom = -1;
    this->restTill = 0;
    this->currentContest.id = -1;
    this->currentContest.room = -1;
    this->currentContest.time = -1;
    this->state = IDLE;
    this->lamportClockOnRequest = -1;
    this->localId = -1;
    srand(time(NULL)+rank*size*1000);
}



void Agent::agentLoop() {
    while(1) {
        handleMsg();
        doStuff();
        if (this->lamport->getTimestamp() > 100) {
            break;
        }
        sleep(1);
    }
}


void Agent::doStuff() {
    switch (this->state) {
        case WAITING_FOR_ANSWERS: {
            if (this->answerCount == 0) {
                this->state = BUSY;

                Contest c;
                c.id = this->currentContest.id;
                c.room = this->currentContest.room;
                c.time = time(0) + randomTime();
                this->currentContest.time = c.time;
                startContest(c);

            }
            break;
        }

        case WAITING_FOR_ROOM: {
            if (this->answerCount == 0) {
                this->state = WAITING_FOR_ANSWERS;
                this->answerCount = lamport->size - 1;

                this->currentContest.room = this->selectedRoom;
                this->selectedRoom = -1;

                Contest c;
                c.id = this->currentContest.id;
                c.room = this->currentContest.room;
                c.time = -1;

                sendInvite(c);
            } else if (this->answerCount < 1) {
                askForRoom();
            }
            break;
        }

        case BUSY: {
            if (this->currentContest.time != -1) {
                if (this->currentContest.time <= time(0)) {
                    this->state = REST;
                    this->currentContest.id = -1;
                    this->currentContest.time = -1;
                    this->currentContest.room = -1;
                    this->selectedRoom = -1;
                    this->answerCount = -1;
                    this->restTill = time(0) + randomTime();
                }
            }
            break;
        }

        case REST: {
            if (this->restTill <= time(0)) {
                this->state = IDLE;
            }
            break;
        }

        case IDLE: {
            if (this->wannaCreateContest()) {

                askForRoom();
            }
            break;
        }


    }
}

void Agent::askForRoom() {
    this->localId++;
    Message msg;
    Contest c;
    this->selectedRoom = this->randomRoom();
    c.room = this->selectedRoom;
    c.id = this->localId;
    this->currentContest.id = c.id;
    msg.contest = c;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    lamport->broadcast(msg, MessageTag(Room));

    printf("%d: \t %d --> broadcast - Room o pokoj %d, id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank, c.room, c.id);

    this->state = WAITING_FOR_ROOM;
    this->answerCount = lamport->size - 1;
    this->lamportClockOnRequest = this->lamport->getTimestamp();
}

void Agent::answerInvite(Message m, bool answer) {
    Message msg;
    msg.contest = m.contest;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    msg.answer = answer;
    lamport->sendMessage(m.processId, msg, MessageTag(AnswerInvite));

    printf("%d: \t %d --> wiadomosc do %d - AnswerInvite o pokoj %d, id: %d wartosc %d\n",
           this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.contest.room, m.contest.id, answer);
}

bool Agent::shouldBeFirst(Message m) {
    if(this->lamportClockOnRequest > m.lamportClock) {
        return true;
    } else if(this->lamportClockOnRequest == m.lamportClock && this->lamport->rank < m.processId) {
        return true;
    }
    return false;
}

void Agent::handleRoom(Message m) {
    // true - pokoj jest wolny

    Message msg;
    msg.contest = m.contest;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;

    if (msg.contest.room == this->currentContest.room) {
        msg.answer = false;
    } else if(msg.contest.room == this->selectedRoom) {
        msg.answer = shouldBeFirst(m);
    } else {
        msg.answer = true;
    }

    lamport->sendMessage(m.processId, msg, MessageTag(AnswerRoom));


    printf("%d: \t %d --> wiadomosc do %d - AnswerRoom o pokoj %d, id: %d wartosc %d\n",
           this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.contest.room, m.contest.id, msg.answer);
}


void Agent::handleInvite(Message m) {
    if (this->state != IDLE) {
        answerInvite(m, false);
    } else {
        answerInvite(m, wannaJoin(m));
    }
}

void Agent::handleAnswerInvite(Message m) {
    if (this->state == WAITING_FOR_ANSWERS && m.contest.id == this->currentContest.id) {
        this->answerCount--;
    }
}


void Agent::handleAnswerRoom(Message m) {
    if (this->state == WAITING_FOR_ROOM && m.contest.id == this->currentContest.id) {
        if (!m.answer) {
            this->answerCount = -1;
        }
        else {
            this->answerCount--;
        }

    }
}

void Agent::handleStartContest(Message m) {
    if(m.contest.id == this->currentContest.id) {
        this->state = BUSY;
        this->currentContest.time = m.contest.time;
    }

}

void Agent::handleMsg() {


    int flag;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if(flag) {
        Message m;
        MPI_Recv(&m, sizeof(Message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        this->lamport->setMax(m.lamportClock);

        switch (status.MPI_TAG) {
            case Invite: {
                printf("%d: \t %d <--  %d - Invite do pokoju %d, id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.contest.room, m.contest.id);
                handleInvite(m);
                break;
            }
            case Room: {
                printf("%d: \t %d <--  %d - Room o pokoj %d, id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.contest.room, m.contest.id);
                handleRoom(m);
                break;
            }
            case AnswerInvite: {
                printf("%d: \t %d <--  %d - AnswerInvite o odpowiedzi %d, id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.answer, m.contest.id);
                handleAnswerInvite(m);
                break;
            }
            case AnswerRoom: {
                printf("%d: \t %d <--  %d - AnswerRoom o odpowiedzi %d, id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.answer, m.contest.id);
                handleAnswerRoom(m);
                break;
            }
            case StartContest: {
                printf("%d: \t %d <--  %d - StartContest do pokoju %d, id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.contest.room, m.contest.id);
                handleStartContest(m);
                break;
            }
        }
    }
}

bool Agent::wannaCreateContest() {
    if (this->lamport->rank == 0) {
        return true;
    } else if (this->lamport->rank == 1) {
        return false;
    } else {
        return (rand() % 5) == 0;
    }
}

bool Agent::wannaJoin(Message m) {
    bool s = (bool) (rand() % 2);
    if (s) {
        this->currentContest = m.contest;
        this->state = BUSY;
    }
    return s;
};


int Agent::randomRoom() {
    //return 0;
    return rand() % this->roomCount;
}

int Agent::randomTime() {
    return rand() % 25 + 5;
}

void Agent::startContest(Contest contest) {
    Message m;
    m.contest = contest;
    m.lamportClock = this->lamport->getTimestamp();
    m.processId = this->lamport->rank;
    lamport->broadcast(m, MessageTag(StartContest));

    printf("%d: \t %d -->  broadcast - StartContest o pokoj %d, id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank, contest.room, contest.id);

}


void Agent::sendInvite(Contest contest) {
    Message m;
    m.contest = contest;
    m.lamportClock = this->lamport->getTimestamp();
    m.processId = this->lamport->rank;
    lamport->broadcast(m, MessageTag(Invite));

    printf("%d: \t %d -->  broadcast - Invite do pokoj %d, id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank, contest.room, contest.id);
}
