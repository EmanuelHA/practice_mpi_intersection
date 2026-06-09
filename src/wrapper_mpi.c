#include "../include/wrapper_mpi.h"

const char* LANE_NAMES[] = {
    "Coordinador",
    "Norte",
    "Sur",
    "Este",
    "Oeste"
};

void init_mpi_env(int* argc, char*** argv, int* rank, int* size) {
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

void finalize_mpi_env(void) {
    MPI_Finalize();
}

void validate_process_count(int size, int required) {
    if (size != required) {
        fprintf(stderr, "Error: se requieren EXACTAMENTE %d procesos. Ejecutando con %d procesos.\n", required, size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

void broadcast_integer(int* value, int source) {
    MPI_Bcast(value, 1, MPI_INT, source, MPI_COMM_WORLD);
}

void sync_processes(void) {
    MPI_Barrier(MPI_COMM_WORLD);
}

void print_sim_header(int size, int n_vehicules) {
    printf("======================================================\n");
    printf("  SIMULADOR DE INTERSECCION DE TRAFICO CON MPI\n");
    printf("  Procesos: %d | Carriles: %d | Vehiculos/carril: %d\n",
           size, N_LANES, n_vehicules);
    printf("======================================================\n\n");
    printf("[Coordinador] Parametros distribuidos. Iniciando...\n");
    fflush(stdout);
}