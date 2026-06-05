#include "lector_datos.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

DatosExperimento leer_datos(const std::string& ruta_archivo) {
    DatosExperimento datos;
    std::ifstream archivo(ruta_archivo);
    if (!archivo.is_open()) {
        throw std::runtime_error("Error critico: No se pudo abrir: " + ruta_archivo);
    }

    std::string linea;
    bool primera_linea = true;

    while (std::getline(archivo, linea)) {
        // Eliminar salto de linea oculto de Windows (\r)
        linea.erase(std::remove(linea.begin(), linea.end(), '\r'), linea.end());
        
        if (linea.empty()) continue;

        // Ignorar la cabecera exacta de tu archivo o cualquier comentario
        if (primera_linea || linea.find("Paso") != std::string::npos || linea.find("Tiempo") != std::string::npos || linea[0] == '#') {
            primera_linea = false;
            continue;
        }

        // CRUCIAL: Convertir el formato decimal de coma (,) a punto (.) para que C++ lo entienda
        std::replace(linea.begin(), linea.end(), ',', '.');

        std::stringstream ss(linea);
        double t, ta, ma, tb, mb;
        
        // Ahora la lectura cientifica leera exactamente 63.380492...
        if (ss >> t >> ta >> ma >> tb >> mb) {
            datos.tiempo.push_back(t);
            datos.tempA.push_back(ta);
            datos.tempB.push_back(tb);
        }
    }

    if (datos.tiempo.empty()) {
        throw std::runtime_error("Fallo la lectura numerica.");
    }

    std::cout << "[Lector] Parseo exitoso. " << datos.tiempo.size() << " lecturas extraidas con precision decimal.\n";
    return datos;
}