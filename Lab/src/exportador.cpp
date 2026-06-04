#include "exportador.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

void exportar_resultados(const std::string& ruta, const ModeloMpemba& modelo) {
    std::ofstream out(ruta);
    out << "t,tempA_mod,tempB_mod\n";
    
    int steps = 1000;
    double max_t = modelo.datos_.tiempo.back() * 1.5; 
    double dt = max_t / steps;

    for (int i = 0; i < steps; ++i) {
        double t = i * dt;
        double tA = modelo.paramA.Tb + modelo.paramA.a2 * std::exp(modelo.paramA.lambda2 * t);
        double tB = modelo.paramB.Tb + modelo.paramB.a2 * std::exp(modelo.paramB.lambda2 * t);
        out << t << "," << tA << "," << tB << "\n";
    }
    std::cout << "[Exportador] Extrapolacion guardada en " << ruta << "\n";
}