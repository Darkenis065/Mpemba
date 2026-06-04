#include <iostream>
#include <fstream>
#include "openmp_utils.hpp"
#include "lector_datos.hpp"
#include "modelo_mpemba.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== Computacion Cientifica - Marco Markoviano Mpemba ===\n";
    print_omp_info();

    try {
        // 1. C++ lee los datos crudos y los limpia en memoria
        auto datos = leer_datos("datos/datos.dat");

        // 2. NUEVO: Exportar los datos limpios y sincronizados para que Python no sufra
        std::ofstream out_raw("output/datos_limpios.csv");
        out_raw << "Tiempo,TempA,TempB\n";
        for(size_t i = 0; i < datos.tiempo.size(); ++i) {
            out_raw << datos.tiempo[i] << "," << datos.tempA[i] << "," << datos.tempB[i] << "\n";
        }
        std::cout << "[Main] Datos experimentales limpios exportados a output/datos_limpios.csv\n";

        // 3. Ejecutar el modelo matemático
        ModeloMpemba modelo(datos);
        modelo.ajustar_curvas();
        
        std::cout << "\n[Resultados Espectrales]\n";
        std::cout << "Curva A (Caliente): Tb=" << modelo.paramA.Tb << " a2=" << modelo.paramA.a2 << " L2=" << modelo.paramA.lambda2 << "\n";
        std::cout << "Curva B (Fria):     Tb=" << modelo.paramB.Tb << " a2=" << modelo.paramB.a2 << " L2=" << modelo.paramB.lambda2 << "\n";

        double t_cruce, temp_cruce;
        if (modelo.detectar_cruce(t_cruce, temp_cruce)) {
            std::cout << "\n>>> EFECTO MPEMBA DIRECTO CONFIRMADO <<<\n";
            std::cout << "Interseccion de trayectorias en t = " << t_cruce 
                      << " s, T = " << temp_cruce << " C.\n";
        } else {
            std::cout << "\n>>> NO SE DETECTA EFECTO MPEMBA <<<\n";
        }

        exportar_resultados("output/resultados_modelo.dat", modelo);

    } catch (const std::exception& e) {
        std::cerr << "Fallo en la ejecucion: " << e.what() << "\n";
        return 1;
    }

    return 0;
}