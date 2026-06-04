#ifndef MODELO_MPEMBA_HPP
#define MODELO_MPEMBA_HPP
#include <vector>
#include "lector_datos.hpp"

// Parámetros de la descomposición espectral (Ec. 12 del paper)
struct ParametrosModelo {
    double Tb;
    double a2;
    double lambda2;
};

class ModeloMpemba {
public:
    ModeloMpemba(const DatosExperimento& datos);
    void ajustar_curvas();
    bool detectar_cruce(double& t_cruce, double& temp_cruce);
    ParametrosModelo paramA, paramB;
    const DatosExperimento& datos_;

private:
    ParametrosModelo ajustar_grid_search(const std::vector<double>& temp_exp);
};

#endif