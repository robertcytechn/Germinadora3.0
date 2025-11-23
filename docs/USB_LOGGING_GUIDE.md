# 📝 Guía de Implementación del Logging USB con CH375B

## 🔌 Conexiones del CH375B

### Pines del Módulo CH375B a Arduino Mega

```
CH375B          Arduino Mega
------          ------------
VCC       ----> 5V
GND       ----> GND
TXD       ----> RX1 (Pin 19)
RXD       ----> TX1 (Pin 18)
INT#      ----> Pin 12 (CH375_INT_PIN)
```

## ⚙️ Configuración en el Código

### 1. Descomentar includes en main.cpp

```cpp
#include <CH375.h>  // Línea 10
```

### 2. Descomentar objeto CH375

```cpp
CH375 usbHost(Serial1, CH375_INT_PIN);  // Después de crear el display
```

### 3. Descomentar llamadas en setup()

```cpp
void setup() {
    // ... código existente ...
    
    inicializarUSB();  // Agregar al final del setup
}
```

### 4. Descomentar llamadas en loop()

```cpp
void loop() {
    // ... código existente ...
    
    guardarDatosUSB();  // Agregar antes del final
}
```

### 5. Descomentar implementación de funciones

En las funciones `inicializarUSB()` y `guardarDatosUSB()`, eliminar los comentarios `/*` y `*/`.

## 📊 Formato de Archivo CSV

El sistema creará un archivo `DROSERA_LOG.CSV` en la raíz del USB con el siguiente formato:

```csv
Fecha,Hora,TempProm,HumProm,T1,T2,T3,H1,H2,H3,Luz,Calef,Alarma,CodAlarma
2025-11-22,07:00:00,23.5,74.8,23.2,23.8,23.5,75.1,74.2,75.1,0,0,0,0
2025-11-22,07:30:00,24.1,75.2,23.9,24.3,24.1,76.0,74.5,75.1,128,1,0,0
2025-11-22,08:00:00,25.0,75.5,24.8,25.2,25.0,76.5,74.8,75.2,255,0,0,0
```

### Descripción de Columnas

| Columna | Descripción | Ejemplo |
|---------|-------------|---------|
| Fecha | Año-Mes-Día | 2025-11-22 |
| Hora | Hora:Minuto:Segundo | 14:30:00 |
| TempProm | Temperatura promedio (°C) | 24.5 |
| HumProm | Humedad promedio (%) | 75.2 |
| T1 | Temperatura sensor superior | 24.3 |
| T2 | Temperatura sensor inferior | 24.8 |
| T3 | Temperatura sensor puerta | 24.4 |
| H1 | Humedad sensor superior | 76.1 |
| H2 | Humedad sensor inferior | 74.5 |
| H3 | Humedad sensor puerta | 75.0 |
| Luz | PWM luces (0-255) | 255 |
| Calef | Calefactor activo (0/1) | 0 |
| Alarma | Alarma activa (0/1) | 0 |
| CodAlarma | Código de alarma (0-5) | 0 |

## 📈 Análisis de Datos

### Importar en Excel

1. Abrir Excel
2. Datos > Desde texto/CSV
3. Seleccionar `DROSERA_LOG.CSV`
4. Delimitador: Coma
5. Crear gráficos de líneas para temperatura y humedad

### Importar en Python (pandas)

```python
import pandas as pd
import matplotlib.pyplot as plt

# Leer datos
df = pd.read_csv('DROSERA_LOG.CSV')
df['Datetime'] = pd.to_datetime(df['Fecha'] + ' ' + df['Hora'])

# Graficar temperatura
plt.figure(figsize=(12, 6))
plt.subplot(2, 1, 1)
plt.plot(df['Datetime'], df['TempProm'], label='Temperatura Promedio')
plt.plot(df['Datetime'], df['T1'], alpha=0.3, label='Sensor 1')
plt.plot(df['Datetime'], df['T2'], alpha=0.3, label='Sensor 2')
plt.plot(df['Datetime'], df['T3'], alpha=0.3, label='Sensor 3')
plt.axhline(y=25, color='r', linestyle='--', label='Objetivo Día')
plt.axhline(y=15, color='b', linestyle='--', label='Objetivo Noche')
plt.ylabel('Temperatura (°C)')
plt.legend()
plt.grid(True)

# Graficar humedad
plt.subplot(2, 1, 2)
plt.plot(df['Datetime'], df['HumProm'], label='Humedad Promedio', color='green')
plt.axhline(y=75, color='g', linestyle='--', label='Objetivo')
plt.axhline(y=90, color='r', linestyle='--', label='Máximo Seguridad')
plt.axhline(y=60, color='orange', linestyle='--', label='Mínimo Seguridad')
plt.xlabel('Fecha y Hora')
plt.ylabel('Humedad (%)')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.savefig('grafico_drosera.png')
plt.show()
```

## 🔧 Troubleshooting

### USB no detectado

```cpp
// Agregar debug en inicializarUSB()
Serial.println("Verificando conexion USB...");
delay(1000);
// Verificar INT pin
Serial.print("INT pin estado: ");
Serial.println(digitalRead(CH375_INT_PIN));
```

### Datos no se guardan

- Verificar que el USB tiene formato FAT32
- Verificar que hay espacio disponible
- Comprobar que `usbConectado` es `true`
- Revisar Serial Monitor para mensajes de error

### Archivo corrupto

- Usar USB de buena calidad (evitar clones)
- Agregar checksum en cada línea
- Crear backup periódico

## 💾 Capacidad de Almacenamiento

### Cálculo de espacio

Cada registro ocupa aproximadamente 100 bytes.

Con logging cada 30 minutos:
- 48 registros/día = 4.8 KB/día
- 30 días = 144 KB/mes
- 1 año = 1.7 MB/año

**Un USB de 1GB puede almacenar ~585 años de datos** 😄

## 🔄 Mantenimiento

### Backup Mensual

1. Extraer USB del sistema (apagar primero)
2. Copiar `DROSERA_LOG.CSV` a PC
3. Renombrar con fecha: `DROSERA_LOG_2025_11.CSV`
4. Opcional: Borrar archivo original para empezar de cero
5. Volver a insertar USB

### Formato USB

Usar **FAT32** para máxima compatibilidad:

```bash
# Windows (PowerShell como Admin)
format E: /FS:FAT32 /Q

# Linux
sudo mkfs.vfat -F 32 /dev/sdX1

# macOS
sudo diskutil eraseDisk FAT32 DROSERA_LOG /dev/diskX
```

## 📋 Checklist de Instalación

- [ ] CH375B conectado correctamente (VCC, GND, TX, RX, INT)
- [ ] Serial1 libre (no usar pines 18/19 para otra cosa)
- [ ] Includes descomentados en main.cpp
- [ ] Objeto usbHost creado
- [ ] inicializarUSB() llamado en setup()
- [ ] guardarDatosUSB() llamado en loop()
- [ ] USB formateado en FAT32
- [ ] USB con al menos 1MB libre
- [ ] Compilar y verificar sin errores
- [ ] Monitorear Serial para confirmar detección USB

## 📚 Referencias

- [CH375 Datasheet (Inglés)](http://www.electrodragon.com/w/images/1/18/CH375DS1.PDF)
- [Librería CH375-Arduino](https://github.com/arielnh56/CH375-Arduino)
- [FAT32 Specification](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html)

---

**Nota:** El CH375B es un chip complejo. Si tienes problemas, empieza con el ejemplo básico de la librería (`chipTest`) antes de implementar el logging completo.
