//
// Created by Sleyter Angulo on 6/2/26.
//

#include "../include/wrapper_mpi.h"

#include <stdio.h>

void init_parallelization_mpi(int* argc, char*** argv, int* rank, int* size)
{
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

void finalize_parallelization_mpi(void)
{
    MPI_Finalize();
}

void check_numbers_processors(int size, int required)
{
    if (size != required)
    {
        printf("This program requires exactly %d processes. You got %d\n", required, size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

void broadcast_numbers_processors(int* value, int source)
{
    MPI_Bcast(&value, 1, MPI_INT, source, MPI_COMM_WORLD);
}

void sync_processors(void)
{
    MPI_Barrier(MPI_COMM_WORLD);
}
