#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

const int Q = 27;
const double w[Q] = {
    8./27., 
    2./27., 2./27., 2./27., 2./27., 2./27., 2./27., 
    1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 1./54., 
    1./216., 1./216., 1./216., 1./216., 1./216., 1./216., 1./216., 1./216.
};

const int cx[Q] = {0, 1,-1, 0, 0, 0, 0, 1,-1, 1,-1, 1,-1, 1,-1, 0, 0, 0, 0, 1,-1, 1,-1, 1,-1, 1,-1};
const int cy[Q] = {0, 0, 0, 1,-1, 0, 0, 1, 1,-1,-1, 0, 0, 0, 0, 1,-1, 1,-1, 1, 1,-1,-1, 1, 1,-1,-1};
const int cz[Q] = {0, 0, 0, 0, 0, 1,-1, 0, 0, 0, 0, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1, 1, 1,-1,-1,-1,-1};
const int opp[Q] = {0, 2, 1, 4, 3, 6, 5, 10, 9, 8, 7, 14, 13, 12, 11, 18, 17, 16, 15, 26, 25, 24, 23, 22, 21, 20, 19};

extern "C" {

    void lbm_step(
        double* f, double* f_next, double* g, double* g_next,
        double* rho, double* ux, double* uy, double* uz, double* temp,
        int Nx, int Ny, int Nz, 
        double tau_f, double tau_g, 
        double beta1, double beta2, double g_grav, double T_ref,
        double k_term, double T_inf, double epsilon, double sigma,
        double C_evap, double C_clausius, double T_boil, double L_v_cp
    ) {
        int N = Nx * Ny * Nz;

        // --- PRECOMPUTACIÓN DE CONSTANTES FUERA DE LOS BUCLES ESPACIALES ---
        double omega_f = 1.0 / tau_f;
        double omega_g = 1.0 / tau_g;
        double eps_sigma = epsilon * sigma;
        double T_inf_4 = T_inf * T_inf * T_inf * T_inf;
        double inv_T_boil = 1.0 / T_boil;

        // La presión de saturación en el infinito es estática
        double P_sat_inf = std::exp(C_clausius * (inv_T_boil - 1.0 / T_inf));

        // --- BUCLE ÚNICO FUSIONADO (COLLIDE-AND-STREAM) ---
        for (int z = 0; z < Nz; ++z) {
            for (int y = 0; y < Ny; ++y) {
                for (int x = 0; x < Nx; ++x) {
                    int idx = z * Nx * Ny + y * Nx + x;

                    // 1. CÁLCULO MACROSCÓPICO
                    double loc_rho = 0.0, loc_ux = 0.0, loc_uy = 0.0, loc_uz = 0.0, loc_T = 0.0;
                    for (int i = 0; i < Q; ++i) {
                        int mem_idx = i * N + idx;
                        double fi = f[mem_idx]; // Lectura coalescida única
                        loc_rho += fi;
                        loc_ux  += fi * cx[i];
                        loc_uy  += fi * cy[i];
                        loc_uz  += fi * cz[i];
                        loc_T   += g[mem_idx];
                    }

                    if (loc_rho < 1e-6) loc_rho = 1e-6; 
                    double inv_rho = 1.0 / loc_rho; // División única

                    loc_ux *= inv_rho; loc_uy *= inv_rho; loc_uz *= inv_rho;

                    double deltaT = loc_T - T_ref;
                    double Fz = loc_rho * g_grav * (beta1 * deltaT + beta2 * deltaT * deltaT);
                    loc_uz += (Fz * 0.5 * inv_rho);

                    double u2 = loc_ux*loc_ux + loc_uy*loc_uy + loc_uz*loc_uz;
                    double max_u2 = 0.01; 
                    if (u2 > max_u2) {
                        double scale = std::sqrt(max_u2 / u2);
                        loc_ux *= scale; loc_uy *= scale; loc_uz *= scale;
                        u2 = max_u2; 
                    }

                    // Guardar macroscópicas locales
                    rho[idx] = loc_rho; ux[idx] = loc_ux; uy[idx] = loc_uy; uz[idx] = loc_uz; temp[idx] = loc_T;

                    // 2. COLISIÓN Y PROPAGACIÓN SIMULTÁNEA (Push)
                    for (int i = 0; i < Q; ++i) {
                        int mem_idx = i * N + idx;
                        double cu = cx[i]*loc_ux + cy[i]*loc_uy + cz[i]*loc_uz;

                        double feq = w[i] * loc_rho * (1.0 + 3.0*cu + 4.5*cu*cu - 1.5*u2);
                        double geq = w[i] * loc_T * (1.0 + 3.0*cu + 4.5*cu*cu - 1.5*u2);
                        double force_term = (1.0 - 0.5 * omega_f) * w[i] * 3.0 * Fz * cz[i];

                        // Estado post-colisión retenido en registros
                        double f_post = f[mem_idx] - omega_f * (f[mem_idx] - feq) + force_term;
                        double g_post = g[mem_idx] - omega_g * (g[mem_idx] - geq);

                        int next_x = x + cx[i];
                        int next_y = y + cy[i];
                        int next_z = z + cz[i];

                        // Condiciones de periodicidad laterales
                        if (next_x < 0) next_x = Nx - 1; else if (next_x >= Nx) next_x = 0;
                        if (next_y < 0) next_y = Ny - 1; else if (next_y >= Ny) next_y = 0;

                        if (next_z < 0) {
                            // FRONTERA INFERIOR: Newton-Raphson Optimizado
                            double T_wall = loc_T; 
                            const double tol = 1e-6;

                            for (int iter = 0; iter < 10; ++iter) {
                                double T_w2 = T_wall * T_wall;
                                double T_w3 = T_w2 * T_wall;
                                double F = k_term * (loc_T - T_wall) - eps_sigma * (T_w3 * T_wall - T_inf_4);
                                double dF = -k_term - 4.0 * eps_sigma * T_w3;
                                double delta = F / dF;
                                T_wall -= delta;

                                if (T_wall < T_inf) T_wall = T_inf; 
                                if (std::abs(delta) < tol) break;
                            }

                            int o_i = opp[i];
                            int mem_opp_curr = o_i * N + idx;

                            f_next[mem_opp_curr] = f_post; 
                            g_next[mem_opp_curr] = std::max(0.0, -g_post + 2.0 * w[o_i] * T_wall); 
                        } 
                        else if (next_z >= Nz) {
                            // FRONTERA SUPERIOR: Evaporación Optimizada
                            int o_i = opp[i];
                            int mem_opp_curr = o_i * N + idx;

                            double evap_fraction = 0.0;

                            if (loc_T > T_inf) {
                                double P_sat_fluid = std::exp(C_clausius * (inv_T_boil - 1.0 / loc_T));
                                evap_fraction = C_evap * std::max(0.0, P_sat_fluid - P_sat_inf);
                                if (evap_fraction > 1.0) evap_fraction = 1.0; 
                            }

                            double delta_m_local = f_post * evap_fraction;
                            f_next[mem_opp_curr] = f_post - delta_m_local;

                            double q_evap_T = (delta_m_local * L_v_cp) * inv_rho;
                            g_next[mem_opp_curr] = std::max(0.0, g_post - 2.0 * w[o_i] * q_evap_T);
                        } 
                        else {
                            // DOMINIO INTERNO: Streaming Directo
                            int idx_next = next_z * Nx * Ny + next_y * Nx + next_x;
                            f_next[i * N + idx_next] = f_post;
                            g_next[i * N + idx_next] = g_post;
                        }
                    }
                }
            }
        }
    }
}
