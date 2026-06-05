#include "modelo_mpemba.hpp"
#include <cmath>
#include <iostream>
#include <omp.h>
#include <limits>
#include <stdexcept>

ModeloMpemba::ModeloMpemba(const DatosExperimento& datos) : datos_(datos) {
    if (datos_.tiempo.empty()) {
        throw std::invalid_argument("Error en ModeloMpemba: La estructura de datos experimental esta vacia.");
    }
}

ParametrosModelo ModeloMpemba::ajustar_grid_search(const std::vector<double>& temp_exp) {
    double best_mse = std::numeric_limits<double>::max();
    ParametrosModelo best_param = {0, 0, 0};
    
    // Rango físico amplio: T_base entre -2 y 5.5, amplitudes hasta 80°C, y decaimientos reales
    double Tb_min = -2.0, step_Tb = 0.5;
    double L2_min = -0.02, step_L2 = 0.0002; 
    double a2_min = 0.0, step_a2 = 1.0;      
    int num_t = static_cast<int>(datos_.tiempo.size());

    #pragma omp parallel
    {
        double local_best_mse = std::numeric_limits<double>::max();
        ParametrosModelo local_best_param = {0,0,0};

        #pragma omp for collapse(3) nowait
        for (int i = 0; i < 16; ++i) {        
            for (int j = 0; j < 100; ++j) {   
                for (int k = 0; k < 80; ++k) { 
                    double Tb = Tb_min + i * step_Tb;
                    double L2 = L2_min + j * step_L2;
                    double a2 = a2_min + k * step_a2;
                    
                    double mse = 0.0;
                    // Submuestreo rápido para el MSE
                    for (int t_idx = 0; t_idx < num_t; t_idx += 1) {
                        double t = datos_.tiempo[t_idx];
                        double t_model = Tb + a2 * std::exp(L2 * t);
                        double diff = temp_exp[t_idx] - t_model;
                        mse += diff * diff;
                    }

                    if (mse < local_best_mse) {
                        local_best_mse = mse;
                        local_best_param = {Tb, a2, L2};
                    }
                }
            }
        }
        
        #pragma omp critical
        {
            if (local_best_mse < best_mse) {
                best_mse = local_best_mse;
                best_param = local_best_param;
            }
        }
    }
    return best_param;
}

void ModeloMpemba::ajustar_curvas() {
    std::cout << "[Modelo] Extrayendo autovalores espectrales Vaso A (Caliente) con OpenMP...\n";
    paramA = ajustar_grid_search(datos_.tempA);
    std::cout << "[Modelo] Extrayendo autovalores espectrales Vaso B (Frio) con OpenMP...\n";
    paramB = ajustar_grid_search(datos_.tempB);
}

bool ModeloMpemba::detectar_cruce(double& t_cruce, double& temp_cruce) {
    if (datos_.tiempo.empty()) return false;

    double t_inicial = 0.0;
    for (double t = t_inicial; t < t_inicial + 50000.0; t += 1.0) {
        double tA = paramA.Tb + paramA.a2 * std::exp(paramA.lambda2 * t);
        double tB = paramB.Tb + paramB.a2 * std::exp(paramB.lambda2 * t);
        
        if (tA <= tB && t > 1.0) {
            t_cruce = t;
            temp_cruce = tA;
            return true;
        }
    }
    return false;
}