cmake_minimum_required(VERSION 4.2)
project(practice_crossing_mpi C)

set(CMAKE_C_STANDARD 23)

find_package(MPI REQUIRED)

add_executable(practice_crossing_mpi main.c
        src/vehicle.c
        include/vehicle.h
        src/wrapper_mpi.c
        include/wrapper_mpi.h
        src/lane.c
        include/lane.h
        src/coordinator.c
        include/coordinator.h)

target_include_directories(practice_crossing_mpi PRIVATE include)
target_link_libraries(practice_crossing_mpi PRIVATE MPI::MPI_C)
