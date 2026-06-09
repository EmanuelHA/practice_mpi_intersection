#ifndef WRAPPER_MPI_H
#define WRAPPER_MPI_H

#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define N_LANES             4
#define NUM_PROCESSES       5
#define VEHICLES_PER_LANE   10
#define BUFF_LEN            256

typedef enum LANE_RANKS {
    COORDINATOR,
    NORTH_LANE,
    SOUTH_LANE,
    EAST_LANE,
    WEST_LANE
} LANE;

typedef enum TAGS {
    REQUEST_PERMISSION = 1,
    PERMISSION_GRANTED = 2,
    CROSSING_END       = 3,
    STATISTICS         = 4
} TAG;

extern const char* LANE_NAMES[];

void init_mpi_env(int *argc, char ***argv, int *rank, int *size);
void finalize_mpi_env(void);
void validate_process_count(int size, int required);
void broadcast_integer(int *value, int source);
void sync_processes(void);
void print_sim_header(int size, int n_vehicules);

#endif // WRAPPER_MPI_H