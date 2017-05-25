#include <iostream>
#include <mpi.h>
#include "Lamport.h"
#include "Agent.h"

#define M 10
#define S 3

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int size,rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Agent *agent = new Agent(size, rank, S);
    agent->agentLoop();

    MPI_Finalize();
    return 0;
}