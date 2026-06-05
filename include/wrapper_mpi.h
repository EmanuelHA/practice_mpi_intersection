//
// Created by Sleyter Angulo on 6/2/26.
//

#include <mpi.h>
#ifndef PRACTICE_CROSSING_MPI_WRAPPER_MPI_H
#define PRACTICE_CROSSING_MPI_WRAPPER_MPI_H

#define N_LANES 4
#define NUM_PROCESSOR 5
#define VEHICLES_PER_LINE 10

#define REQUEST_PERMISSION_TAG 1
#define PERMISSION_GRANTED_TAG 2
#define CROSSING_END_TAG 3
#define STATISTICS_TAG 4

typedef enum LANE_RANK
{
    COORDINATOR = 0,
    NORTH = 1,
    SOUTH = 2,
    EAST = 3,
    WEST = 4,
} lane_rank_t;


void init_parallelization_mpi(int *argc, char ***argv, int *rank, int *size);
void finalize_parallelization_mpi(void);

void check_numbers_processors(int size, int required);

void broadcast_numbers_processors(int *value, int source);

void sync_processors(void);


#endif //PRACTICE_CROSSING_MPI_WRAPPER_MPI_H
