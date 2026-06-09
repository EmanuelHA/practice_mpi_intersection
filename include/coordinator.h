#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "wrapper_mpi.h"

#define MAX_QUEUE_SIZE (N_LANES * VEHICLES_PER_LANE)

typedef struct {
    int8_t lane;
    int16_t id;
} request;

typedef struct {
    request data[MAX_QUEUE_SIZE];
    int start;
    int size;
} request_queue;

void enqueue(request_queue *queue, request request);
request dequeue(request_queue *queue);
bool is_queue_empty(const request_queue *queue);
bool is_queue_full(const request_queue *queue);

void run_coordinator(int size, int n_vehicules);

#endif // COORDINATOR_H