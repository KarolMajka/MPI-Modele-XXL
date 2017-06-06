#include <iostream>
#include <mpi.h>
#include "Lamport.h"
#include "Agent.h"

#define M 10
#define S 9

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("pass argument to program\n");
        return 0;
    }
    int ret, numberOfRooms;
    ret = sscanf(argv[1], "%d", &numberOfRooms);
    if(ret != 1) {
        printf("pass integer argument to program\n");
        return 0;
    }
    MPI_Init(&argc, &argv);
    int size,rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Barrier(MPI_COMM_WORLD);
    Agent *agent = new Agent(size, rank, numberOfRooms);
    agent->agentLoop();

    MPI_Finalize();
    return 0;
}