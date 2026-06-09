#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/wrapper_mpi.h"
#include "../include/lane.h"
#include "../include/vehicle.h"

void run_lane(int rank, int n_vehicles) {
    char buffer[BUFF_LEN];
    snprintf(buffer, sizeof(buffer), "[%-5s] listo. Esperando autorizacion para cruzar.\n", LANE_NAMES[rank]);
    MPI_Send(buffer, (int)strlen(buffer) + 1, MPI_CHAR, COORDINATOR, REQUEST_PERMISSION, MPI_COMM_WORLD);

    // IMPORTANTE: Es necesario sincronizar luego del saludo para evitar solicitudes de cruce por parte de los procesos que saludaron ya que se
    // genera una RC entre la fase 1 y 2 que bota el programa debido a que el tag de saludo y solicitud de cruce es el mismo (REQUEST_PERMISSION) 
    sync_processes(); 

    srand((unsigned int)time(NULL) + (unsigned int)(rank * 97));
    
    double total_wait = 0.0;

    double waited_time = 0.0;

    // Fase 2: Generar y cruzar vehículos
    for (int16_t id = 1; id <= n_vehicles; id++) {
        int permission = 0;

        double chrono_start = MPI_Wtime();
        
        MPI_Send(&id, 1, MPI_INT16_T, COORDINATOR, REQUEST_PERMISSION, MPI_COMM_WORLD);
        MPI_Recv(&permission, 1, MPI_INT, COORDINATOR, PERMISSION_GRANTED, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double chrono_stop = MPI_Wtime();
        waited_time += (chrono_stop - chrono_start);

        if (permission != id) {
            fprintf(stderr, "[%s-%03" PRId16 "] Error: permiso inesperado recibido.\n", LANE_NAMES[rank], id);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        simulate_crossing();
        MPI_Send(&id, 1, MPI_INT16_T, COORDINATOR, CROSSING_END, MPI_COMM_WORLD);
    }

    int16_t end = -1;
    MPI_Send(&end, 1, MPI_INT16_T, COORDINATOR, REQUEST_PERMISSION, MPI_COMM_WORLD);

    uint32_t local_vehiculos = (uint32_t)n_vehicles;
    MPI_Reduce(&local_vehiculos, NULL, 1, MPI_UINT32_T, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
    MPI_Gather(&total_wait, 1, MPI_DOUBLE, NULL, 1, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
}