#include <stdio.h>
#include "include/wrapper_mpi.h"

#define NORTH_LANE "NORTH"
#define EAST_LANE "EAST"
#define SOUTH_LANE "SOUTH"
#define WEST_LANE "WEST"

int main(int argc, char *argv[])
{
    int size, rank;
    init_parallelization_mpi(&argc, &argv, &rank, &size);

    check_numbers_processors(size, NUM_PROCESSOR);

    int vehicles_per_line = VEHICLES_PER_LINE;

    broadcast_numbers_processors(&vehicles_per_line, COORDINATOR);

    sync_processors();


    finalize_parallelization_mpi();
    return 0;
}
