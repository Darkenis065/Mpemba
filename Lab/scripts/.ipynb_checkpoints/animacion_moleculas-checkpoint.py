import numpy as np

def update_particles(positions, velocities, temp_factor, dt=0.5):
    """ Físicamente modelado: Ek = 3/2 kB T -> |v| proporcional a sqrt(T) """
    v_actual = velocities * temp_factor
    positions += v_actual * dt
    # Rebotes elásticos simples
    for i in range(2):
        mask_min = positions[:, i] < 0
        positions[mask_min, i] *= -1
        velocities[mask_min, i] *= -1
        mask_max = positions[:, i] > 100
        positions[mask_max, i] = 200 - positions[mask_max, i]
        velocities[mask_max, i] *= -1
    return positions, velocities