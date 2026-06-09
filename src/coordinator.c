#include <stdlib.h>
#include "../include/coordinator.h"

bool is_queue_empty(const request_queue *queue) {
    return queue->size == 0;
}

bool is_queue_full(const request_queue *queue) {
    return queue->size == MAX_QUEUE_SIZE;
}

void enqueue(request_queue *queue, request request) {
    if (is_queue_full(queue)) {
        fprintf(stderr, "[Coordinador] Error: cola llena.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int pos = (queue->start + queue->size) % MAX_QUEUE_SIZE;
    queue->data[pos] = request;
    queue->size++;
}

request dequeue(request_queue *queue) {
    request req = queue->data[queue->start];
    queue->start = (queue->start + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return req;
}

static void print_vehicle(const request *req) {
    printf("[%s-%03" PRId16 "]", LANE_NAMES[req->lane], req->id);
}

static void grant_permission(const request *req, bool from_queue) {
    int permisson = req->id;
    MPI_Send(&permisson, 1, MPI_INT, req->lane, PERMISSION_GRANTED, MPI_COMM_WORLD);

    printf("[Coordinador] ");
    print_vehicle(req);
    if (from_queue) {
        printf(" autorizado (era el siguiente en cola). Cruce OCUPADO.\n");
    } else {
        printf(" autorizado. Cruce OCUPADO.\n");
    }
    fflush(stdout);
}

void run_coordinator(int size, int n_vehicules) {
    // Fase 1: Recibir saludos iniciales
    printf("[%s] Sistema iniciado. N_VEHICLES=%d distribuido a %d carriles.\n", LANE_NAMES[COORDINATOR], n_vehicules, N_LANES);
    for (int i = 0; i < N_LANES; i++) {
        char buffer[BUFF_LEN];
        MPI_Recv(buffer, BUFF_LEN, MPI_CHAR, MPI_ANY_SOURCE, REQUEST_PERMISSION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%s", buffer);
    }
    printf("\n");
    print_sim_header(size, n_vehicules);

    // IMPORTANTE: Es necesario sincronizar luego del saludo para evitar solicitudes de cruce por parte de los procesos que saludaron ya que se
    // genera una RC entre la fase 1 y 2 que bota el programa debido a que el tag de saludo y solicitud de cruce es el mismo (REQUEST_PERMISSION) 
    sync_processes();

    // Fase 2: Variables de control de cruce
    double start_sim_time = MPI_Wtime();

    request_queue queue = {0};
    request active = {0};
    bool is_crossing_busy = false;
    bool lane_finished[NUM_PROCESSES] = {false};
    int lanes_completed = 0;

    while (lanes_completed < N_LANES || is_crossing_busy || !is_queue_empty(&queue)) {
        // Si el cruce se libera y hay carros esperando, desencolar
        if (!is_crossing_busy && !is_queue_empty(&queue)) {
            active = dequeue(&queue);
            is_crossing_busy = true;
            grant_permission(&active, true);
            continue;
        }

        int16_t id = 0;
        MPI_Status status;
        // Si ocupado, escuchar solo CROSSING_END, si libre escuchar REQUEST_PERMISSION
        int expected_tag = is_crossing_busy ? CROSSING_END : REQUEST_PERMISSION;

        MPI_Recv(&id, 1, MPI_INT16_T, MPI_ANY_SOURCE, expected_tag, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == REQUEST_PERMISSION) {
            int8_t lane = (int8_t)status.MPI_SOURCE;

            if (id == -1) {
                if (!lane_finished[lane]) {
                    lane_finished[lane] = true;
                    lanes_completed++;
                }
                continue;
            }

            request req = {lane, id};
            print_vehicle(&req);
            printf(" -> solicita cruce");

            if (is_crossing_busy || !is_queue_empty(&queue)) {
                enqueue(&queue, req);
                printf(" (en cola: %d esperando)\n", queue.size);
            } else {
                printf("\n");
                active = req;
                is_crossing_busy = true;
                grant_permission(&active, false);
            }
            fflush(stdout);

        } else if (status.MPI_TAG == CROSSING_END) {
            request done = {(int8_t)status.MPI_SOURCE, id};
            print_vehicle(&done);
            printf(" \xE2\x9C\x93 cruce completado. Cruce LIBRE.\n");
            fflush(stdout);
            is_crossing_busy = false;
        }
    }

    double end_sim_time = MPI_Wtime(); // FASE 3: Termina el tiempo global

    // Fase 3: Estadísticas Colectivas (MPI_Reduce, MPI_Gather)
    uint32_t crossed_total = 0;
    // Coordinador no tiene vehículos, envía 0 al sumador global
    uint32_t zero_vehicles = 0; 
    MPI_Reduce(&zero_vehicles, &crossed_total, 1, MPI_UINT32_T, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

    // Arreglo para guardar el tiempo que esperó cada carril (0 a 4)(rank 0 no cuenta)
    double wait_times[NUM_PROCESSES] = {0};
    double lane_wait = 0.0;
    MPI_Gather(&lane_wait, 1, MPI_DOUBLE, wait_times, 1, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    // --- IMPRESIÓN DEL REPORTE FINAL ---
    printf("\n------- REPORTE FINAL -------\n");
    printf("Vehículos cruzados (total): %" PRIu32 "/%d\n", crossed_total, (n_vehicules * N_LANES));
    printf("Accidentes:                 0 - garantizado por arquitectura\n");
    printf("Tiempo de simulacion:       %.3f s (MPI_Wtime)\n\n", (end_sim_time - start_sim_time));
    
    printf("Estadisticas por carril (MPI_Gather):\n");
    printf("%-10s %-10s %-15s\n", "Carril", "Vehiculos", "Espera acum. (s)");
    
    double max_wait = -1.0;
    double min_wait = 999999.0;

    for (int i = 1; i <= N_LANES; i++) {
        if (wait_times[i] > max_wait) max_wait = wait_times[i];
        if (wait_times[i] < min_wait) min_wait = wait_times[i];
    }

    for (int i = 1; i <= N_LANES; i++) {
        printf("%-10s %-10d %.3f", LANE_NAMES[i], n_vehicules, wait_times[i]);
        if (wait_times[i] == max_wait) printf("  <- mayor espera");
        if (wait_times[i] == min_wait) printf("  <- menor espera");
        printf("\n");
    }

    printf("\nFunciones MPI usadas: MPI_Send, MPI_Recv, MPI_Bcast, MPI_Barrier, MPI_Reduce, MPI_Gather, MPI_Wtime\n");
}