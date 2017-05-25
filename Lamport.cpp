//
// Created by Karol Majka on 15/05/2017.
//

#include "Lamport.h"


Lamport::Lamport(int size, int rank) {
    this->size = size;
    this->rank = rank;
}

void Lamport::sendMessage(Message message, MessageTag tag) {
    for(int i = 0; i < size; i++) {
        if (i != rank) {
            this->sendMessage(i, message, tag);
        }
    }
}

void Lamport::increment() {
    this->timestamp++;
}
int Lamport::getTimestamp() {
    return this->timestamp;
}

void Lamport::sendMessage(int to, Message message, MessageTag tag) {
    MPI_Send(&message, sizeof(message), MPI_BYTE, to, tag, MPI_COMM_WORLD);
}

