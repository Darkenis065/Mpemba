import matplotlib.pyplot as plt
import pandas as pd

try:
    # Cargar datos limpios y modelados
    df_exp = pd.read_csv('output/datos_limpios.csv')
    df_mod = pd.read_csv('output/resultados_modelo.dat')
    
    plt.figure(figsize=(10,6))
    
    # Datos experimentales (zancada de 100 para no saturar la imagen)
    plt.scatter(df_exp['Tiempo'][::100], df_exp['TempA'][::100], color='lightcoral', s=8, alpha=0.5, label='Datos Vaso A')
    plt.scatter(df_exp['Tiempo'][::100], df_exp['TempB'][::100], color='cornflowerblue', s=8, alpha=0.5, label='Datos Vaso B')
    
    # Modelos ajustados
    plt.plot(df_mod['t'], df_mod['tempA_mod'], label='Modelo Espectral A', color='darkred', linewidth=2.5)
    plt.plot(df_mod['t'], df_mod['tempB_mod'], label='Modelo Espectral B', color='darkblue', linewidth=2.5)
    
    # Detección visual del cruce
    diff = df_mod['tempA_mod'] - df_mod['tempB_mod']
    mask = (diff <= 0.0) & (df_mod['t'] > 1.0)
    
    if mask.any():
        idx = mask.idxmax()
        plt.scatter(df_mod['t'].iloc[idx], df_mod['tempA_mod'].iloc[idx], color='black', zorder=5, marker='x', s=120)
        plt.annotate(f'Cruce Mpemba: {df_mod["t"].iloc[idx]:.1f} s', 
                     xy=(df_mod['t'].iloc[idx], df_mod['tempA_mod'].iloc[idx]),
                     xytext=(df_mod['t'].iloc[idx] * 0.7, df_mod['tempA_mod'].iloc[idx] + 10),
                     arrowprops=dict(facecolor='black', arrowstyle='->'))

    plt.xlabel(r'Tiempo $t$ (s)', fontsize=12)
    plt.ylabel(r'Temperatura $T$ (°C)', fontsize=12)
    plt.title('Extrapolación y Cruce Asintótico (Marco Markoviano)', fontsize=14)
    plt.legend(loc='upper right')
    plt.grid(True, linestyle='--', alpha=0.6)
    
    plt.savefig('output/Mpemba_Modelado.png', dpi=300, bbox_inches='tight')
    print("Exportado con éxito: output/Mpemba_Modelado.png")

except Exception as e:
    print(f"Error procesando gráfica extrapolada: {e}")