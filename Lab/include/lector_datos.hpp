#ifndef LECTOR_DATOS_HPP
#define LECTOR_DATOS_HPP
#include <vector>
#include <string>

struct DatosExperimento {
    std::vector<double> tiempo;
    std::vector<double> tempA;
    std::vector<double> tempB;
};

DatosExperimento leer_datos(const std::string& ruta_archivo);

#endif