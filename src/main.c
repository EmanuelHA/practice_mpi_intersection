#include <stdlib.h>
#include "../include/wrapper_mpi.h"
#include "../include/coordinator.h"
#include "../include/lane.h"

int main(int argc, char *argv[])
{
    int size, rank;
    init_mpi_env(&argc, &argv, &rank, &size);

    validate_process_count(size, NUM_PROCESSES);

    int vehicles_per_lane = VEHICLES_PER_LANE;
    broadcast_integer(&vehicles_per_lane, COORDINATOR);

    sync_processes();

    if (rank == COORDINATOR) {
        run_coordinator(size, vehicles_per_lane);
    } else {
        run_lane(rank, vehicles_per_lane);
    }

    finalize_mpi_env();
    return EXIT_SUCCESS;
}