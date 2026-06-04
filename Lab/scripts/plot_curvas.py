import matplotlib.pyplot as plt
import pandas as pd

try:
    # Lee los datos que C++ ya limpió por nosotros
    df = pd.read_csv('output/datos_limpios.csv')
    
    plt.figure(figsize=(10,6))
    plt.plot(df['Tiempo'], df['TempA'], label='Vaso A (Agua Caliente)', color='darkred', linewidth=1.5)
    plt.plot(df['Tiempo'], df['TempB'], label='Vaso B (Agua Fría)', color='darkblue', linewidth=1.5)
    
    plt.xlabel(r'Tiempo $t$ (s)', fontsize=12)
    plt.ylabel(r'Temperatura $T$ (°C)', fontsize=12)
    plt.title('Relajación Térmica Experimental - Datos Directos', fontsize=14)
    plt.legend(loc='upper right')
    plt.grid(True, linestyle='--', alpha=0.6)
    
    plt.savefig('output/Efecto_Lab.png', dpi=300, bbox_inches='tight')
    print("Exportado con éxito: output/Efecto_Lab.png")

except Exception as e:
    print(f"Error procesando gráfica experimental: {e}")