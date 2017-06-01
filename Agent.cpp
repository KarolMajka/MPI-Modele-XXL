//
// Created by Karol Majka on 15/05/2017.
//

#include "Agent.h"
#include "Lamport.h"




Agent::Agent(int size, int rank, int roomCount) {
    this->lamport = new Lamport(size, rank);
    this->roomCount = roomCount;
    this->currentContest.room = NULL;
    this->currentContest.time = NULL;
    srand(time(NULL)+rank*size*1000);
}



void Agent::agentLoop() {
    while(1) {
        handleMsg();
        doStuff();
    }
}


void Agent::doStuff() {
    switch (this->state) {
        case WAITING_FOR_ANSWERS: {
            if (this->roomCount == 0) {
                this->state = BUSY;

                Contest c;
                c.room = this->currentRoom;
                c.time = time(0) + randomTime();

                startContest(c);

            }
        }

        case WAITING_FOR_ROOM: {
            if (this->roomCount == 0) {
                this->state = WAITING_FOR_ANSWERS;
                this->roomCount = lamport->size - 1;

                this->currentRoom = this->selectedRoom;
                this->selectedRoom = -1;

                Contest c;
                c.room = this->currentRoom;
                c.time = -1;

                sendInvite(c);
            } else if (this->roomCount < 0) {
                askForRoom();
            }
            break;
        }

        case BUSY: {
            if (this->currentContest.time) {
                if (this->currentContest.time <= time(0)) {
                    this->state = REST;
                    this->currentContest.time = NULL;
                    this->currentContest.room = NULL;
                    this->currentRoom = -1;
                    this->selectedRoom = -1;
                    this->roomCount = -1;
                    this->restTill = time(0) + randomTime();
                }
            }
            break;
        }

        case REST: {
            if (this->restTill <= time(0)) {
                this->state = IDLE;
            }
        }

        case IDLE: {
            if (this->wannaCreateContest()) {

                askForRoom();
            }
        }


    }
}

void Agent::askForRoom() {
    Message msg;
    Contest c;

    this->selectedRoom = this->randomRoom();
    c.room = this->selectedRoom;
    msg.contest = c;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;

    lamport->broadcast(msg, MessageTag(Room));

    this->state = WAITING_FOR_ROOM;
    this->roomCount = lamport->size - 1;
}

void Agent::answerInvite(Message m, bool answer) {
    Message msg;
    msg.contest = m.contest;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    msg.answer = answer;
    lamport->sendMessage(m.processId, msg, MessageTag(AnswerInvite));
}

void Agent::answerRoom(Message m) {
    // true - pokoj jest wolny

    Message msg;
    msg.contest = m.contest;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    msg.answer = msg.contest.room != this->currentRoom;
    lamport->sendMessage(m.processId, msg, MessageTag(AnswerInvite));
}


void Agent::handleInvite(Message m) {
    if (this->state != IDLE) {
        answerInvite(m, false);
    } else {
        answerInvite(m, wannaJoin(m));
    }
}

void Agent::handleAnswerInvite(Message m) {
    if (this->state == WAITING_FOR_ANSWERS) {
        this->roomCount--;
    }
}


void Agent::handleAnswerRoom(Message m) {
    if (this->state == WAITING_FOR_ROOM) {
        if (!m.answer) {
            this->roomCount = -1;
        }
        else {
            this->roomCount--;
        }

    }
}

void Agent::handleStartContest(Message m) {
    this->state = BUSY;

}

void Agent::handleMsg() {
    int flag;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

    if (flag) {
        Message m;
        MPI_Status status;
        MPI_Recv(&m, sizeof(Message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (status.MPI_TAG) {
            case Invite: {
                handleInvite(m);
                break;
            }
            case Room: {
                answerRoom(m);
                break;
            }
            case AnswerInvite: {
                handleAnswerInvite(m);
                break;
            }
            case AnswerRoom: {
                handleAnswerRoom(m);
                break;
            }
            case StartContest: {
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
        return (bool) (rand() % 2);
    }
}

bool Agent::wannaJoin(Message m) {
    bool s = (bool) (rand() % 2);
    this->currentContest = m.contest;
    if (s) state = BUSY;
    return s;
};


int Agent::randomRoom() {
    return rand() % this->roomCount;
}

int Agent::randomTime() {
    return rand() % 25000 + 5000;
}

bool Agent::roomIsFree(int room) {
    //send broadcast and wait for answers
    return false;
}


void Agent::startContest(Contest contest) {
    Message m;
    m.contest = contest;
    m.lamportClock = this->lamport->getTimestamp();
    m.processId = this->lamport->rank;
    lamport->broadcast(m, MessageTag(StartContest));
}


void Agent::sendInvite(Contest contest) {
    Message m;
    m.contest = contest;
    m.lamportClock = this->lamport->getTimestamp();
    m.processId = this->lamport->rank;
    lamport->broadcast(m, MessageTag(Invite));
}

void Agent::waitForAnswers() {
    int received = 0;
    Message m;
    MPI_Recv(&m, sizeof(Message), MPI_BYTE, MPI_ANY_SOURCE, MessageTag(AnswerInvite),
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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