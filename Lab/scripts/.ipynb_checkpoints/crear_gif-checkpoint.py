import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from animacion_moleculas import update_particles

def create_gif(filename, t_start, t_end, color, title_prefix):
    num_particles = 100
    pos = np.random.rand(num_particles, 2) * 100
    vel = (np.random.rand(num_particles, 2) - 0.5) * np.sqrt(t_start)
    
    fig, ax = plt.subplots(figsize=(5,5))
    ax.set_xlim(0, 100); ax.set_ylim(0, 100); ax.axis('off')
    scatter = ax.scatter(pos[:,0], pos[:,1], c=color, s=20, alpha=0.8)
    
    def animate(frame):
        nonlocal pos, vel
        temp = t_start - (t_start - t_end) * (frame / 60.0)
        temp_prev = t_start - (t_start - t_end) * (max(0, frame-1) / 60.0)
        factor = np.sqrt(max(temp, 0.1) / max(temp_prev, 0.1))
        
        pos, vel = update_particles(pos, vel, factor)
        scatter.set_offsets(pos)
        ax.set_title(rf'{title_prefix} $E_k \propto T$ = {temp:.1f} °C')
        return scatter,

    anim = animation.FuncAnimation(fig, animate, frames=60, interval=60, blit=True)
    anim.save(filename, writer='pillow')
    plt.close()
    print(f"Exportado: {filename}")

def create_comparative_gif(filename):
    num_p = 100
    pA = np.random.rand(num_p, 2) * 100
    vA = (np.random.rand(num_p, 2) - 0.5) * np.sqrt(63.0)
    pB = np.random.rand(num_p, 2) * 100
    vB = (np.random.rand(num_p, 2) - 0.5) * np.sqrt(18.0)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))
    ax1.set_xlim(0, 100); ax1.set_ylim(0, 100); ax1.axis('off')
    ax2.set_xlim(0, 100); ax2.set_ylim(0, 100); ax2.axis('off')
    scA = ax1.scatter(pA[:,0], pA[:,1], c='darkred', s=20)
    scB = ax2.scatter(pB[:,0], pB[:,1], c='darkblue', s=20)
    
    def animate(frame):
        nonlocal pA, vA, pB, vB
        tA = 63.0 - (63.0 - 3.1) * (frame / 60.0)
        tB = 18.0 - (18.0 - 0.6) * (frame / 60.0)
        
        tA_p = 63.0 - (63.0 - 3.1) * (max(0, frame-1)/60.0)
        tB_p = 18.0 - (18.0 - 0.6) * (max(0, frame-1)/60.0)
        
        fA = np.sqrt(max(tA, 0.1)/max(tA_p, 0.1))
        fB = np.sqrt(max(tB, 0.1)/max(tB_p, 0.1))
            
        pA, vA = update_particles(pA, vA, fA)
        pB, vB = update_particles(pB, vB, fB)
        scA.set_offsets(pA)
        scB.set_offsets(pB)
        ax1.set_title(rf'Vaso A: {tA:.1f} °C')
        ax2.set_title(rf'Vaso B: {tB:.1f} °C')
        return scA, scB

    anim = animation.FuncAnimation(fig, animate, frames=60, interval=60, blit=True)
    anim.save(filename, writer='pillow')
    plt.close()
    print(f"Exportado: {filename}")

if __name__ == '__main__':
    create_gif('output/simulacion_A.gif', 63.0, 3.1, 'darkred', 'Vaso A')
    create_gif('output/simulacion_B.gif', 18.0, 0.6, 'darkblue', 'Vaso B')
    create_comparative_gif('output/simulacion_comparativa.gif')