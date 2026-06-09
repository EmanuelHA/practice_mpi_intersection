#define _DEFAULT_SOURCE // Necesario para usleep en estándares estrictos
#include <unistd.h>
#include <stdlib.h>
#include "../include/vehicle.h"

void simulate_crossing(void) {
    usleep(2000u + (unsigned int)(rand() % 4000));
}