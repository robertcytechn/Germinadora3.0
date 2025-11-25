# 🎛️ Guía del Sistema de Menú con Encoder Rotatorio

## 📋 Descripción

Sistema de menús estilo Marlin Firmware con navegación mediante **encoder rotatorio** (perilla giratoria con botón integrado). Permite modificar todos los parámetros del sistema en tiempo real con una experiencia de usuario superior a los botones tradicionales.

## 🕹️ Hardware Requerido

### Encoder Rotatorio KY-040
- **Tipo:** Encoder incremental rotatorio con botón pulsador integrado
- **Ventajas:**
  - ✅ Navegación bidireccional rápida e intuitiva
  - ✅ Ajuste preciso de valores girando la perilla
  - ✅ Un solo componente (3 funciones en 1)
  - ✅ Menos pines ocupados que 3 botones separados
  - ✅ Sensación táctil superior y profesional
  - ✅ Muy económico (~$1-2 USD)

### Especificaciones Técnicas
- **Voltaje:** 5V
- **Salidas:** CLK (Clock), DT (Data), SW (Switch)
- **Tipo:** Encoder incremental (detecta dirección de giro)
- **Resolución:** Típicamente 20 pasos por revolución
- **Botón:** Pulsador normalmente abierto integrado
- **Durabilidad:** > 30,000 ciclos

## 🔌 Conexiones

### Diagrama de Cableado

```
Encoder KY-040          Arduino Mega 2560
--------------          -----------------
  CLK (Clock)    ───>      Pin 22
  DT (Data)      ───>      Pin 23
  SW (Switch)    ───>      Pin 24
  + (VCC)        ───>      5V
  GND            ───>      GND
```

### Esquema Visual

```
     ┌──────────────┐
     │  KY-040      │
     │   Encoder    │
     │              │
     │   ┌─────┐   │
     │   │  ●  │   │  ← Perilla giratoria
     │   └─────┘   │
     │              │
     │ CLK DT SW +G │
     └──┬──┬──┬──┬─┘
        │  │  │  │
        │  │  │  └──────────── GND
        │  │  └─────────────── 5V
        │  └────────────────── Pin 24 (SW)
        │  └────────────────── Pin 23 (DT)
        └───────────────────── Pin 22 (CLK)


Arduino Mega 2560
┌─────────────────┐
│                 │
│     DIGITAL     │
│                 │
│ Pin 22 ├─── CLK (Encoder)
│ Pin 23 ├─── DT  (Encoder)
│ Pin 24 ├─── SW  (Encoder)
│                 │
│     POWER       │
│                 │
│    5V  ├─── +   (Encoder)
│   GND  ├─── GND (Encoder)
│                 │
└─────────────────┘
```

### Detalles de Conexión

| Pin Encoder | Función | Pin Arduino | Tipo |
|-------------|---------|-------------|------|
| **CLK** | Señal Clock (A) | Pin 22 | Entrada Digital |
| **DT** | Señal Data (B) | Pin 23 | Entrada Digital |
| **SW** | Botón Switch | Pin 24 | Entrada Digital + Pull-up |
| **+** | Alimentación | 5V | Alimentación |
| **GND** | Tierra | GND | Tierra |

## 🔧 Instalación

### 1. Cableado Físico

1. **Desconectar** el Arduino de la alimentación
2. Conectar los 5 cables según el diagrama anterior
3. **Verificar** que no haya cortocircuitos entre VCC y GND
4. Reconectar alimentación

**⚠️ Importante:** 
- NO invertir VCC y GND (puede dañar el encoder)
- Usar cables cortos (< 20cm) para evitar ruido eléctrico
- Si usas cables largos, considera agregar condensadores de 0.1µF entre cada pin de señal y GND

### 2. Activación en el Código

En `main.cpp`, realizar los siguientes cambios:

**Paso 1:** Incluir la librería del encoder (línea ~22)
```cpp
#include <MENU_ENCODER.h>  // Sistema de menú con encoder rotatorio
```

**Paso 2:** Crear objeto del menú (línea ~36)
```cpp
MenuSystemEncoder menu(&display);
```

**Paso 3:** Inicializar en setup() (línea ~140)
```cpp
void setup() {
    // ... código existente ...
    
    // Inicializar sistema de menú con encoder
    menu.inicializar();
    
    Serial.println(F("Sistema iniciado - Presiona encoder 2s para menu"));
}
```

**Paso 4:** Actualizar en loop() (línea ~160)
```cpp
void loop() {
    // ... lectura de sensores ...
    
    // Actualizar menú (procesa encoder y botón)
    menu.actualizar();
    
    // ... resto del código ...
}
```

**Paso 5:** Mostrar en pantalla (dentro de `mostrarPantalla()`)
```cpp
void mostrarPantalla() {
    // Si el menú está activo, solo mostrar el menú
    if (menu.estaActivo()) {
        menu.mostrar();
        return;
    }
    
    // ... código normal de pantalla de monitoreo ...
}
```

### 3. Compilar y Cargar

```bash
platformio run --target upload
```

## 🎯 Uso del Sistema

### Activación del Menú

**Mantener presionado el botón del encoder por 2 segundos**

El sistema cambiará automáticamente de la pantalla de monitoreo al menú principal.

### Navegación con Encoder

| Acción | Control |
|--------|---------|
| **Navegar arriba** | Girar encoder antihorario (izquierda) ↺ |
| **Navegar abajo** | Girar encoder horario (derecha) ↻ |
| **Seleccionar** | Presionar botón del encoder (click) |
| **Confirmar cambio** | Presionar botón del encoder en modo edición |
| **Salir del menú** | Seleccionar "< Salir" en menú principal |

### Edición de Valores

Cuando entras en modo edición de un parámetro:

1. **Gira el encoder** para ajustar el valor
   - Giro horario ↻ = Incrementar
   - Giro antihorario ↺ = Decrementar
   - Ajuste automático según el tipo de parámetro:
     - Temperatura: ±0.5°C por paso
     - Humedad: ±1% por paso
     - Tiempo: ±1 unidad por paso

2. **Presiona el botón** para confirmar y volver al menú

### Características Avanzadas

#### Auto-scroll Inteligente
- El menú hace scroll automático cuando hay más de 5 items
- El cursor se mantiene visible en todo momento

#### Límites de Valores
- Todos los parámetros tienen límites min/max configurados
- No puedes establecer valores fuera de rangos seguros
- Ejemplos:
  - Temperatura día: 10-35°C
  - Humedad objetivo: 50-95%
  - Hora inicio día: 0-23h

#### Detección de Dirección
- El sistema detecta la dirección de giro mediante interrupciones
- Respuesta instantánea sin lag perceptible
- Anti-rebote por hardware y software

## 📊 Estructura del Menú

```
MENÚ PRINCIPAL
├── Temperatura
│   ├── Día: XX.X°C
│   ├── Noche: XX.X°C
│   ├── Control: ±X.X°C
│   ├── Max Seguridad: XX.X°C
│   ├── Min Seguridad: XX.X°C
│   └── < Volver
│
├── Humedad
│   ├── Objetivo: XX%
│   ├── Control: ±X%
│   ├── Max Seguridad: XX%
│   ├── Min Seguridad: XX%
│   └── < Volver
│
├── Iluminación
│   ├── Inicio día: XXh
│   ├── Fin día: XXh
│   ├── Duración amanecer: XXmin
│   └── < Volver
│
├── Ventilación
│   ├── [Configuración futura]
│   └── < Volver
│
├── Sistema
│   ├── Info Sistema
│   ├── Reset Alarmas
│   ├── Ajustar RTC
│   ├── Guardar USB
│   └── < Volver
│
└── < Salir
```

## 🔬 Funcionamiento Técnico

### Sistema de Lectura

El encoder usa **lectura por polling** ya que los pines 22-24 no tienen interrupciones dedicadas:

```cpp
// Lectura directa en actualizar()
void MenuSystemEncoder::actualizarEncoder() {
    byte clk = digitalRead(ENCODER_CLK_PIN);
    byte dt = digitalRead(ENCODER_DT_PIN);
    // Detectar cambios y dirección
}
```

**Características:**
- Lectura en cada ciclo del loop()
- Suficiente para navegación de menú
- Sin dependencia de interrupciones

### Decodificación de Cuadratura

El encoder genera señales en cuadratura (90° desfasadas):

```
Giro Horario (CW):        Giro Antihorario (CCW):
CLK  ─┐ ┌─┐ ┌─            CLK  ─┐ ┌─┐ ┌─
      └─┘ └─┘                   └─┘ └─┘
DT   ──┐ ┌─┐ ┌             DT   ┐ ┌─┐ ┌─┐
       └─┘ └─┘                 └─┘ └─┘

Cuando CLK baja:              Cuando CLK baja:
DT ≠ CLK → Incremento         DT = CLK → Decremento
```

### Anti-rebote

**Por hardware:**
- Muchos encoders KY-040 incluyen condensadores de 0.1µF

**Por software:**
- Debounce de 5ms en la ISR
- Filtrado de pulsos espurios

## 🔍 Solución de Problemas

### El encoder cuenta doble

**Causa:** Ruido eléctrico o rebote
**Solución:** 
1. Reducir longitud de cables
2. Agregar condensadores de 0.1µF entre CLK/GND y DT/GND
3. Aumentar `ENCODER_DEBOUNCE` a 10ms en MENU_ENCODER.h

### Gira en dirección opuesta

**Causa:** Cables CLK y DT invertidos
**Solución:** Intercambiar las conexiones de los pines 18 y 19

### El botón no responde

**Causa:** Cable SW suelto o mal contacto
**Solución:**
1. Verificar soldadura del pin SW
2. Verificar conexión al pin 20
3. Comprobar continuidad con multímetro

### No detecta algunos giros

**Causa:** Interrupciones deshabilitadas o ISR bloqueada
**Solución:**
1. No usar `delay()` dentro de ISR
2. Verificar que `interrupts()` esté habilitado
3. Revisar que no haya bucles largos en el código

### El menú no se activa con presión larga

**Causa:** Tiempo de debounce muy alto
**Solución:**
1. Verificar `BTN_LONG_PRESS` = 2000ms (2 segundos)
2. Mantener presionado completamente (no soltar)
3. Revisar pull-up del pin SW

## 📈 Comparación: Encoder vs Botones

| Característica | Encoder KY-040 | 3 Botones |
|----------------|----------------|-----------|
| **Componentes** | 1 | 3 |
| **Pines usados** | 3 | 3 |
| **Costo total** | ~$1.50 | ~$0.75 |
| **Navegación** | Bidireccional continua | Unidireccional |
| **Velocidad** | Muy rápida | Lenta |
| **Precisión ajuste** | Excelente | Buena |
| **Experiencia usuario** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Durabilidad** | > 30k ciclos | > 100k pulsos |
| **Complejidad código** | Media | Baja |
| **Respuesta** | Instantánea (ISR) | Polling |

## 🎨 Personalización

### Cambiar velocidad de ajuste

En `MENU_ENCODER.h`, modificar los valores de `incremento`:

```cpp
void MenuSystemEncoder::editarTempDia() {
    // ...
    incremento = 0.5;  // Cambiar a 0.1 para ajuste más fino
    // ...
}
```

### Cambiar tiempo de presión larga

En `MENU_ENCODER.h`:

```cpp
#define BTN_LONG_PRESS 2000  // Cambiar a 1000 para 1 segundo
```

### Cambiar pines del encoder

Si necesitas usar otros pines, modifica en `PINS.h`:

```cpp
#define ENCODER_CLK_PIN XX  // Cualquier pin digital
#define ENCODER_DT_PIN YY   // Cualquier pin digital
#define ENCODER_SW_PIN ZZ   // Cualquier pin digital
```

**⚠️ Nota:** Los pines 22-24 actuales no tienen interrupciones hardware.
Para mejor rendimiento, considera usar pines con INT: 2, 3, 18, 19, 20, 21

## 📚 Referencias

- [Datasheet KY-040](https://components101.com/modules/KY-04-rotary-encoder-pinout-features-datasheet-working-application-alternative)
- [Rotary Encoder Theory](https://www.arduino.cc/reference/en/libraries/encoder/)
- [Quadrature Encoding](https://en.wikipedia.org/wiki/Incremental_encoder)

## ✅ Checklist de Instalación

- [ ] Encoder KY-040 conectado según diagrama
- [ ] Verificado que no hay cortocircuitos
- [ ] `#include <MENU_ENCODER.h>` agregado en main.cpp
- [ ] Objeto `MenuSystemEncoder menu(&display);` creado
- [ ] `menu.inicializar();` llamado en setup()
- [ ] `menu.actualizar();` llamado en loop()
- [ ] `if (menu.estaActivo()) menu.mostrar();` en pantalla
- [ ] Código compilado sin errores
- [ ] Cargado al Arduino Mega
- [ ] Probado activación con presión larga (2s)
- [ ] Probado giro horario/antihorario
- [ ] Probado edición de valores

## 🎉 ¡Listo!

Ahora tienes un sistema de menú profesional con encoder rotatorio. Disfruta de la navegación fluida y rápida para ajustar todos los parámetros de tu germinadora.

**Presiona el encoder 2 segundos para comenzar** 🎛️
