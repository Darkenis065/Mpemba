#include "openmp_utils.hpp"
#include <omp.h>
#include <iostream>

void print_omp_info() {
    int max_threads = omp_get_max_threads();
    std::cout << "[OpenMP] Detectados automáticamente " << max_threads << " hilos de CPU.\n";
}

int get_max_threads() {
    return omp_get_max_threads();
}