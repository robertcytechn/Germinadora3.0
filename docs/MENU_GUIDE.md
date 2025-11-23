# 🎮 Guía del Sistema de Menú Interactivo

## 📋 Descripción

Sistema de menús estilo Marlin Firmware que permite modificar todos los parámetros del sistema en tiempo real sin necesidad de recompilar el código. Navegación mediante Joystick ARD-358.

## 🕹️ Hardware Requerido

### Joystick ARD-358
- **Tipo:** Joystick analógico de 2 ejes + botón
- **Conexiones:**
  - VCC → 5V
  - GND → GND
  - VRX (Eje X) → A0
  - VRY (Eje Y) → A1
  - SW (Botón) → Pin 22 (con pull-up interno)

### Características
- Rango analógico: 0-1023 (cada eje)
- Botón tipo switch (activo en LOW)
- Zona muerta configurable para evitar drift

## 🔧 Instalación

### 1. Conexión del Joystick

```
Joystick ARD-358        Arduino Mega
----------------        ------------
    VCC         ------>    5V
    GND         ------>    GND
    VRX (X)     ------>    A0
    VRY (Y)     ------>    A1
    SW (Botón)  ------>    Pin 22
```

### 2. Activación en el Código

En `main.cpp`, descomentar las siguientes líneas:

```cpp
// Línea ~10 - Include del menú
#include <MENU.h>

// Línea ~20 - Crear objeto del menú
MenuSystem menu(&display);

// Línea ~50 en setup() - Inicializar menú
menu.inicializar();

// Línea ~15 en loop() - Actualizar menú
menu.actualizar();

// Línea ~320 en mostrarPantalla() - Mostrar menú
if (menu.estaActivo()) {
    menu.mostrar();
    return;
}
```

### 3. Compilar y Cargar

```bash
pio run --target upload
```

## 🎯 Uso del Menú

### Activación del Menú

**Mantener presionado el botón del joystick por 2 segundos**

El sistema cambiará de la pantalla de monitoreo al menú principal.

### Navegación

| Acción | Control |
|--------|---------|
| **Mover arriba** | Mover joystick hacia arriba (Y < 400) |
| **Mover abajo** | Mover joystick hacia abajo (Y > 600) |
| **Seleccionar** | Presionar botón del joystick |
| **Volver** | Seleccionar opción "Volver" |
| **Salir del menú** | Seleccionar "Salir" en menú principal |

### Edición de Valores

1. Navegar hasta el parámetro deseado
2. Presionar botón para entrar en modo edición
3. Usar joystick arriba/abajo para modificar el valor
4. Presionar botón nuevamente para guardar

## 📚 Estructura de Menús

```
MENÚ PRINCIPAL
├── Temperatura
│   ├── Día: 25.0°C
│   ├── Noche: 15.0°C
│   ├── Histéresis: 1.0°C
│   ├── Máx Seguridad: 30.0°C
│   ├── Mín Seguridad: 10.0°C
│   └── Volver
│
├── Humedad
│   ├── Objetivo: 75%
│   ├── Histéresis: 5%
│   ├── Máx Seguridad: 90%
│   ├── Mín Seguridad: 55%
│   └── Volver
│
├── Iluminación
│   ├── Hora Inicio: 07:00
│   ├── Hora Fin: 21:00
│   ├── Duración Amanecer: 90 min
│   └── Volver
│
├── Ventilación
│   ├── T.Int ON: 3 min (solo lectura)
│   ├── T.Int OFF: 12 min (solo lectura)
│   ├── Configuración Ext (solo lectura)
│   └── Volver
│
├── Sistema
│   ├── Ver Info
│   ├── Reset Alarmas
│   ├── Ajustar RTC
│   ├── Guardar USB
│   └── Volver
│
└── Salir
```

## ⚙️ Parámetros Editables

### 🌡️ Temperatura

| Parámetro | Rango | Incremento | Descripción |
|-----------|-------|------------|-------------|
| Día | 15-35°C | 0.5°C | Temperatura objetivo diurna |
| Noche | 5-25°C | 0.5°C | Temperatura objetivo nocturna |
| Histéresis | 0.5-3°C | 0.1°C | Margen de control ±°C |
| Máx Seguridad | 25-40°C | 1.0°C | Alarma temperatura alta |
| Mín Seguridad | 0-15°C | 1.0°C | Alarma temperatura baja |

### 💧 Humedad

| Parámetro | Rango | Incremento | Descripción |
|-----------|-------|------------|-------------|
| Objetivo | 50-95% | 1% | Humedad objetivo |
| Histéresis | 2-10% | 1% | Margen de control ±% |
| Máx Seguridad | 80-99% | 1% | Alarma humedad alta (hongos) |
| Mín Seguridad | 40-70% | 1% | Alarma humedad baja |

### 💡 Iluminación

| Parámetro | Rango | Incremento | Descripción |
|-----------|-------|------------|-------------|
| Hora Inicio | 0-23 | 1h | Hora de amanecer (24h) |
| Hora Fin | 0-23 | 1h | Hora de anochecer (24h) |
| Duración Amanecer | 30-180 min | 5 min | Tiempo de transición gradual |

### 🌀 Ventilación

**Nota:** Los parámetros de ventilación están en constantes del código.
Para modificarlos, editar el archivo `VARS.h` y recompilar.

### 🔧 Sistema

- **Ver Info:** Muestra información del sistema (TODO)
- **Reset Alarmas:** Reinicia todas las alarmas activas
- **Ajustar RTC:** Ajusta fecha/hora del reloj (TODO)
- **Guardar USB:** Fuerza guardado inmediato de datos (TODO)

## 🎨 Interfaz Visual

### Pantalla del Menú

```
┌────────────────────┐
│ > TEMPERATURA      │ ← Header con título
├────────────────────┤
│ > Dia: 25.0C       │ ← Item seleccionado (invertido)
│   Noche: 15.0C     │ ← Items normales
│   Hister: 1.0C     │
│   Max: 30C         │
│   Min: 10C         │
└────────────────────┘
```

### Pantalla de Edición

```
┌────────────────────┐
│ EDITAR VALOR       │
├────────────────────┤
│ Temp Dia           │
│                    │
│    25.0 C          │ ← Valor grande y centrado
│                    │
│ Arriba/Abajo: +/-  │ ← Ayuda
│ Boton: Guardar     │
└────────────────────┘
```

## 💡 Características Avanzadas

### Zona Muerta (Deadzone)

El joystick tiene una zona muerta de ±100 unidades alrededor del centro (512) para evitar movimientos no deseados por drift.

```cpp
#define JOYSTICK_CENTER 512
#define JOYSTICK_DEADZONE 100
```

Si tu joystick tiene mucho drift, aumentar `JOYSTICK_DEADZONE` en `MENU.h`.

### Debounce del Botón

El botón tiene un debounce de 200ms para evitar múltiples lecturas.

```cpp
#define JOYSTICK_DEBOUNCE 200
```

### Throttle de Movimiento

La navegación tiene un throttle de 200ms entre movimientos para evitar desplazamientos muy rápidos.

### Scroll Automático

El menú muestra máximo 5 items a la vez. Si hay más items, el scroll es automático al navegar.

## 🔧 Calibración del Joystick

### Test de Valores

Agregar en `setup()` para ver los valores crudos:

```cpp
void setup() {
    // ... código existente ...
    
    // Test joystick (descomentar para calibrar)
    /*
    Serial.println("Test Joystick:");
    Serial.print("X centro: "); Serial.println(analogRead(A0));
    Serial.print("Y centro: "); Serial.println(analogRead(A1));
    delay(2000);
    
    Serial.println("Mover joystick en todas direcciones...");
    for (int i = 0; i < 50; i++) {
        Serial.print("X: "); Serial.print(analogRead(A0));
        Serial.print(" Y: "); Serial.println(analogRead(A1));
        delay(100);
    }
    */
}
```

### Ajustar Centro

Si el centro no es 512, modificar en `MENU.h`:

```cpp
#define JOYSTICK_CENTER 500  // Cambiar según tu joystick
```

## 📝 Persistencia de Datos

### Importante ⚠️

Los valores modificados en el menú **se guardan en RAM**. Al reiniciar el sistema, los valores vuelven a los definidos en `VARS.h`.

### Soluciones Futuras

1. **EEPROM:** Guardar valores en EEPROM y cargarlos al inicio
2. **SD Card:** Archivo de configuración `config.txt`
3. **USB:** Exportar/importar configuración via CH375B

## 🐛 Solución de Problemas

### Menú no se activa

- Verificar conexión del botón (pin 22)
- Verificar pull-up interno activado
- Mantener presionado 2 segundos completos
- Revisar Serial Monitor para mensajes

### Navegación errática

- Aumentar `JOYSTICK_DEADZONE` en `MENU.h`
- Verificar conexiones analógicas (A0, A1)
- Verificar alimentación estable (5V)
- Calibrar el centro del joystick

### Valores no se modifican

- Verificar que estás en modo edición (pantalla cambia)
- Verificar movimiento del joystick (mínimo ±100 del centro)
- Revisar rangos min/max del parámetro

### Pantalla parpadea

- Normal durante navegación rápida
- Reducir velocidad de movimiento del joystick
- El throttle de 200ms ayuda a esto

## 🔄 Flujo de Trabajo Típico

### Ejemplo: Cambiar temperatura día de 25°C a 27°C

1. **Activar menú:** Mantener botón presionado 2 segundos
2. **Ver pantalla:** "MENU PRINCIPAL" aparece
3. **Navegar:** Mover joystick abajo hasta "Temperatura" (invertido)
4. **Entrar:** Presionar botón
5. **Ver pantalla:** "TEMPERATURA" aparece
6. **Seleccionar:** "Dia: 25.0C" ya está seleccionado
7. **Editar:** Presionar botón
8. **Ver pantalla:** "EDITAR VALOR" con "25.0 C" grande
9. **Aumentar:** Mover joystick arriba 4 veces (25.0 → 27.0)
10. **Guardar:** Presionar botón
11. **Volver:** Navegar hasta "Volver" y presionar
12. **Salir:** Navegar hasta "Salir" en menú principal y presionar
13. **Listo:** Sistema vuelve a pantalla de monitoreo con nuevo valor

## 📚 Referencias de Código

### Archivos Relacionados

- `MENU.h` - Sistema completo de menús
- `main.cpp` - Integración del menú
- `VARS.h` - Variables editables
- `PINS.h` - Definición de pines

### Variables Globales Accedidas

El menú modifica directamente estas variables:

```cpp
// Temperatura
extern float tempDia;
extern float tempNoche;
extern float tempControl;
extern float tempMaxSeguridad;
extern float tempMinSeguridad;

// Humedad
extern float humObjetivo;
extern float humControl;
extern float humMaxSeguridad;
extern float humMinSeguridad;

// Iluminación
extern int initDia;
extern int finDia;
extern int duracionAmanecer;

// Sistema
extern bool alarmaActiva;
extern byte codigoAlarma;
```

## 🎓 Inspiración: Marlin Firmware

Este sistema está inspirado en el firmware Marlin de impresoras 3D, conocido por su interfaz intuitiva y robusta.

**Características heredadas:**
- Navegación jerárquica
- Editor de valores in-situ
- Indicadores visuales claros
- Diseño modular y expandible

---

**¡Disfruta de tu sistema de control totalmente configurable! 🎮🌱**
