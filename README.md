# 🌱 MegaGerminadora - Sistema de Control para Drosera Capensis

Sistema automatizado de germinación/incubación para plantas carnívoras **Drosera capensis** basado en Arduino Mega 2560.

## 📋 Descripción

Controlador inteligente que mantiene condiciones óptimas para la germinación y crecimiento de plantas carnívoras, específicamente optimizado para *Drosera capensis* (rosolí del Cabo).

### Características Principales

- ✅ Control automático de temperatura día/noche
- ✅ Monitoreo multi-punto de temperatura y humedad (3 sensores DHT11)
- ✅ Sistema de iluminación progresiva (amanecer/atardecer)
- ✅ Iluminación con espectro rojo para fotosíntesis
- ✅ Ventilación inteligente anti-hongos
- ✅ Sistema de alarmas de seguridad
- ✅ Código completamente no bloqueante (sin delays)
- 🔜 Control de humedad con nebulizador (preparado)
- 🔜 Logging de datos en USB cada 30 minutos (preparado)
- 🔜 Alarmas audibles con buzzer (preparado)

## 🌿 Parámetros para Drosera Capensis

| Parámetro | Valor Día | Valor Noche | Rango Seguridad |
|-----------|-----------|-------------|-----------------|
| **Temperatura** | 25°C | 15°C | 10-30°C |
| **Humedad** | 75% | 75% | 60-90% |
| **Iluminación** | 7:00 - 21:00 (14h) | - | - |
| **Ventilación** | Ciclos suaves | - | Anti-hongos |

## 🔌 Hardware Requerido

### Componentes Actuales
- Arduino Mega 2560
- 3x Sensores DHT11 (superior, inferior, puerta)
- Pantalla OLED SSD1306 (128x64, I2C)
- RTC DS1307 (reloj en tiempo real)
- Módulo de luces blancas (PWM)
- LEDs rojos para fotosíntesis
- Resistencia calefactora
- 2x Ventiladores (interno y externo, PWM)

### Componentes Futuros (Preparados en código)
- Nebulizador/Humidificador ultrasónico (pin 10)
- Buzzer para alarmas (pin 11)
- Módulo CH375B para USB Host (Serial1, pines 18/19)
- Pendrive/USB para logging de datos

## 📦 Librerías Necesarias

Las siguientes librerías se instalan automáticamente con PlatformIO:

```ini
- Adafruit Unified Sensor (^1.1.14)
- DHT sensor library (^1.4.6)
- Adafruit SSD1306 (^2.5.13)
- RTClib (^2.1.4)
- PID (^1.2.1)
- CH375-Arduino-master (para logging USB)
```

## 🔧 Configuración de Pines

```cpp
// SENSORES
DHT Superior:    Pin 2
DHT Inferior:    Pin 3
DHT Puerta:      Pin 4

// ACTUADORES PWM
Ventilador Ext:  Pin 5
Luces Blancas:   Pin 6

// ACTUADORES DIGITALES
Calefactora:     Pin 7
Ventilador Int:  Pin 8
LEDs Rojos:      Pin 9

// FUTUROS
Humidificador:   Pin 10 (comentado)
Buzzer:          Pin 11 (comentado)
CH375 INT:       Pin 12

// I2C (automático en Mega)
SDA:             Pin 20
SCL:             Pin 21

// SERIAL CH375B
RX1:             Pin 19
TX1:             Pin 18
```

## 🚀 Instalación

### 1. Clonar o descargar el proyecto

```bash
cd proyects
git clone <tu-repositorio>
cd MegaGerminadora
```

### 2. Abrir en PlatformIO (VS Code)

```
File > Open Folder > Seleccionar MegaGerminadora
```

### 3. Compilar y cargar

```bash
pio run --target upload
```

### 4. Ajustar RTC (SOLO UNA VEZ)

En `src/main.cpp`, descomentar la línea:
```cpp
reloj.adjust(DateTime(F(__DATE__), F(__TIME__)));
```

Cargar el código, esperar 5 segundos, comentar nuevamente la línea y volver a cargar.

## ⚙️ Configuración Personalizada

Editar el archivo `.pio/libdeps/megaatmega2560/Cyetchnologies/VARS.h`:

```cpp
// Temperaturas
float tempDia = 25.0;        // Temperatura diurna (°C)
float tempNoche = 15.0;      // Temperatura nocturna (°C)

// Humedad
float humObjetivo = 75.0;    // Humedad objetivo (%)

// Iluminación
int initDia = 7;             // Hora de inicio (24h)
int finDia = 21;             // Hora de fin (24h)
```

## 📊 Sistema de Monitoreo

### Pantalla OLED
Muestra en tiempo real:
- Temperatura promedio y máxima
- Humedad con indicadores (LOW/OK/HIGH)
- Estado de ventiladores
- Estado de iluminación
- Hora actual
- Indicador de alarmas

### Monitor Serial (9600 baud)
- Mensajes de inicio del sistema
- Activación/desactivación de actuadores
- Alertas y alarmas
- Debug de funcionamiento

## 🚨 Sistema de Alarmas

El sistema detecta y reporta automáticamente:

| Código | Descripción | Acción |
|--------|-------------|--------|
| 1 | Temperatura muy alta (>30°C) | Ventilación máxima |
| 2 | Temperatura muy baja (<10°C) | Calefacción urgente |
| 3 | Humedad muy alta (>90%) | Ventilación anti-hongos |
| 4 | Humedad muy baja (<60%) | Bloquea calefacción |
| 5 | Fallo de sensores | Modo seguro |

## 🔐 Sistemas de Seguridad

### Calefacción
- ✅ Tiempo máximo continuo: 10 minutos
- ✅ Bloqueo si humedad < 60%
- ✅ Control por histéresis (±1°C)

### Ventilación
- ✅ Override en emergencia térmica (+2°C sobre objetivo)
- ✅ Override en humedad excesiva (>90%)
- ✅ Ventilación obligatoria durante calefacción

### Sensores
- ✅ Validación de lecturas (detección NaN)
- ✅ Mantiene última lectura válida
- ✅ Alarma si todos los sensores fallan

## 📝 Funciones Preparadas para Implementación

### Control de Humedad
```cpp
// En loop(), descomentar cuando conectes el humidificador:
controlHumedad();  // Pin 10 configurado
```

### Logging USB
```cpp
// En setup(), descomentar cuando conectes CH375B:
inicializarUSB();

// En loop():
guardarDatosUSB();  // Guarda CSV cada 30 min
```

### Alarma Audible
```cpp
// En PINS.h, descomentar:
#define BUZZER_PIN 11

// La función activarAlarma() ya tiene el código preparado
```

## 📈 Formato de Datos USB

Los datos se guardan en formato CSV con las siguientes columnas:

```csv
Fecha,Hora,TempProm,HumProm,T1,T2,T3,H1,H2,H3,Luz,Calef,Alarma,CodAlarma
2025-11-22,14:30:00,24.5,75.2,24.3,24.8,24.4,76.1,74.5,75.0,255,0,0,0
```

## 🛠️ Mantenimiento

### Calibración de Sensores DHT11
Los DHT11 tienen precisión ±2°C y ±5% humedad. Para mejor precisión considerar DHT22.

### Limpieza
- Limpiar sensores cada 2 semanas
- Verificar ventiladores (polvo/obstrucciones)
- Limpiar pantalla OLED con paño suave

### Batería RTC
Cambiar la batería CR2032 del RTC cada 2-3 años.

## 🐛 Solución de Problemas

### Pantalla OLED no enciende
- Verificar conexiones I2C (SDA/SCL)
- Comprobar dirección I2C (0x3C o 0x3D)
- Verificar alimentación 3.3V/5V según modelo

### RTC pierde la hora
- Cambiar batería CR2032
- Verificar conexiones I2C
- Re-ajustar hora (ver sección Instalación)

### Sensores DHT11 dan NaN
- Esperar 2 segundos entre lecturas
- Verificar alimentación (3.3V o 5V según modelo)
- Cable de datos con resistencia pull-up 10kΩ
- Distancia máxima 20 metros

### Ventiladores no arrancan
- Aumentar `PWM_EXT_BASAL` en VARS.h
- Verificar voltaje de alimentación
- Usar transistor/MOSFET para corrientes >40mA

## 📚 Recursos Adicionales

### Drosera Capensis - Cuidados
- [ICPS - Carnivorous Plant Database](http://www.carnivorousplants.org/)
- [Drosera Care Guide](https://www.carnivorousplants.co.uk/resources/drosera-care-sheet/)

### Documentación Técnica
- [Arduino Mega Pinout](https://www.arduino.cc/en/Hacking/PinMapping2560)
- [DHT11 Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
- [CH375B USB Host Guide](https://github.com/djuseeq/Ch376msc)

## 📄 Licencia

Este proyecto es de código abierto. Úsalo y modifícalo libremente.

## 👨‍💻 Autor

Desarrollado para el cultivo óptimo de plantas carnívoras.

## 🔄 Changelog

### v2.0 (Nov 2025)
- ✅ Optimizado para Drosera capensis
- ✅ Eliminados todos los delays (código no bloqueante)
- ✅ Agregada verificación de humedad antes de calefacción
- ✅ Sistema de alarmas implementado
- ✅ Preparado para logging USB con CH375B
- ✅ Preparado para control de humedad
- ✅ Ventilación anti-hongos mejorada
- ✅ Documentación completa

### v1.0 (Original)
- Control básico de temperatura
- Iluminación programable
- Ventilación simple

---

**¡Feliz cultivo de plantas carnívoras! 🌱🦟**
