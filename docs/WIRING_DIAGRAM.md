# 🔌 Esquema de Conexiones - MegaGerminadora

## 📋 Lista de Componentes

### Microcontrolador
- 1x Arduino Mega 2560

### Sensores
- 3x DHT11 (Temperatura y Humedad)
- 1x RTC DS1307 con batería CR2032
- 1x Pantalla OLED SSD1306 128x64 I2C

### Actuadores Actuales
- 1x Tira LED blanca 12V (PWM)
- 1x Tira LED roja 12V
- 1x Resistencia calefactora 12V (o relay)
- 2x Ventiladores 12V (uno PWM, uno ON/OFF)

### Actuadores Futuros
- 1x Nebulizador ultrasónico 12V/24V
- 1x Buzzer activo 5V

### Control de Potencia
- 5x Módulos Relé 5V (o MOSFETs para PWM)
- 1x Fuente de alimentación 12V 5A mínimo
- 1x Fuente de alimentación 5V 2A para Arduino

### Comunicación
- 1x Módulo CH375B USB Host

### Varios
- Cables Dupont macho-hembra
- Resistencias 220Ω (para LEDs de estado)
- Resistencias 10kΩ (pull-up si es necesario)
- Tornillos y separadores
- Caja de proyecto

## 🔧 Diagrama de Conexiones

### Arduino Mega 2560 - Vista Superior

```
                    ╔═══════════════════════════════════════╗
                    ║  ARDUINO MEGA 2560                     ║
                    ╠════════════════════════════════════════╣
   DIGITAL PWM ~    ║                                        ║    DIGITAL
                    ║  [0] RX0 ←                    → 22 []  ║
                    ║  [1] TX0 ←                    → 23 []  ║
   DHT Superior  → [2] INT0                          24 []  ║
   DHT Inferior  → [3] INT1 PWM ~                    25 []  ║
   DHT Puerta    → [4]                               26 []  ║
   Ventil. Ext.  → [5] PWM ~                         27 []  ║
   Luces Blancas → [6] PWM ~                         28 []  ║
   Calefactora   → [7]                               29 []  ║
   Ventil. Int.  → [8]                               30 []  ║
   LEDs Rojos    → [9] PWM ~                         31 []  ║
   Humidificador → [10] PWM ~                        32 []  ║
   Buzzer        → [11] PWM ~                        33 []  ║
   CH375 INT     → [12]                              34 []  ║
                    ║  [13] LED                      → 35 []  ║
                    ║  [14] TX3                      → 36 []  ║
                    ║  [15] RX3                      → 37 []  ║
                    ║  [16] TX2                      → 38 []  ║
                    ║  [17] RX2                      → 39 []  ║
   CH375 TX      ← [18] TX1                          40 []  ║
   CH375 RX      ← [19] RX1                          41 []  ║
   SDA (I2C)     ↔ [20] SDA                          42 []  ║
   SCL (I2C)     ↔ [21] SCL                          43 []  ║
                    ║                                        ║
                    ║  [5V] [5V] [GND] [GND] [Vin]          ║
                    ╚═══════════════════════════════════════╝
```

## 📊 Tabla de Conexiones Detallada

### Sensores DHT11

| DHT11      | Pin  | Arduino Mega | Notas                    |
|------------|------|--------------|--------------------------|
| Superior   | VCC  | 5V           |                          |
|            | DATA | 2            | Resistor 10kΩ a 5V       |
|            | GND  | GND          |                          |
| Inferior   | VCC  | 5V           |                          |
|            | DATA | 3            | Resistor 10kΩ a 5V       |
|            | GND  | GND          |                          |
| Puerta     | VCC  | 5V           |                          |
|            | DATA | 4            | Resistor 10kΩ a 5V       |
|            | GND  | GND          |                          |

### Display OLED SSD1306 (I2C)

| OLED | Arduino Mega | Notas           |
|------|--------------|-----------------|
| VCC  | 5V (o 3.3V)  | Ver datasheet   |
| GND  | GND          |                 |
| SDA  | 20 (SDA)     | Bus I2C         |
| SCL  | 21 (SCL)     | Bus I2C         |

### RTC DS1307 (I2C)

| RTC  | Arduino Mega | Notas                  |
|------|--------------|------------------------|
| VCC  | 5V           |                        |
| GND  | GND          |                        |
| SDA  | 20 (SDA)     | Compartido con OLED    |
| SCL  | 21 (SCL)     | Compartido con OLED    |
| BAT  | CR2032       | Batería de respaldo    |

### Módulo CH375B USB Host (Serial)

| CH375B | Arduino Mega | Notas                    |
|--------|--------------|--------------------------|
| VCC    | 5V           |                          |
| GND    | GND          |                          |
| TXD    | 19 (RX1)     | Serial1                  |
| RXD    | 18 (TX1)     | Serial1                  |
| INT#   | 12           | Interrupción             |

## ⚡ Conexiones de Potencia (Actuadores)

### Esquema General con Relés

```
         Arduino Mega Pin
                │
                ↓
         ┌──────────────┐
         │ Módulo Relé  │
         │  5V    GND   │
         │   ↑     ↑    │
         └───┼─────┼────┘
             │     │
          Arduino 5V  Arduino GND
         
         Relé COM ←─── 12V+ (Fuente)
         Relé NO  ──→  Actuador 12V+
         Actuador GND → 12V- (Fuente)
```

### Actuadores - Conexión Detallada

#### 1. Luces Blancas (PWM)

```
Pin 6 PWM → Base NPN (o Gate MOSFET) ┐
                                      ↓
          12V+ → LED Strip + → Transistor → LED Strip - → 12V-
                               (IRF520)
```

**Alternativa con Relé PWM:**
```
Pin 6 PWM → Módulo Dimmer PWM → LED Strip
```

#### 2. Ventilador Externo (PWM)

```
Pin 5 PWM → Gate MOSFET (IRF520) ┐
                                  ↓
          12V+ → Ventilador + → Drain → Source → GND
                                          ↓
                                     Diodo flyback
                                      (1N4007)
```

#### 3. Calefactora (ON/OFF)

```
Pin 7 → Relé Señal
        Relé COM ← 12V+
        Relé NO → Resistencia + → Resistencia - → 12V-
```

**IMPORTANTE:** Usar relé de mínimo 10A si la resistencia consume >5A

#### 4. Ventilador Interno (ON/OFF)

```
Pin 8 → Relé Señal
        Relé COM ← 12V+
        Relé NO → Ventilador + → Ventilador - → 12V-
                                        ↓
                                  Diodo 1N4007
```

#### 5. LEDs Rojos (ON/OFF)

```
Pin 9 → Relé Señal (o directo con resistencia si es 5V)
        Relé COM ← 12V+
        Relé NO → LED Strip + → LED Strip - → 12V-
```

#### 6. Humidificador (ON/OFF) - Futuro

```
Pin 10 → Relé Señal
         Relé COM ← 12V+ (o 24V según modelo)
         Relé NO → Nebulizador + → Nebulizador - → 12V-
```

#### 7. Buzzer (ON/OFF) - Futuro

```
Pin 11 → Buzzer + (Buzzer activo 5V)
         Buzzer - → GND
```

## 🔋 Alimentación

### Esquema de Fuentes

```
220V AC ─→ Fuente 12V 5A ──┬─→ Actuadores 12V
          (Switching)       │
                           └─→ Regulador 7805 ─→ 5V Sensores
                                                  (Alternativo)

220V AC ─→ Fuente 5V 2A ────→ Arduino Mega (Vin o USB)
          (Adaptador USB)
```

### Distribución de Corriente

| Componente          | Voltaje | Corriente | Fuente    |
|---------------------|---------|-----------|-----------|
| Arduino Mega        | 5V      | 500mA     | USB/Vin   |
| DHT11 (x3)          | 5V      | 5mA       | Arduino   |
| OLED                | 5V      | 20mA      | Arduino   |
| RTC DS1307          | 5V      | 1.5mA     | Arduino   |
| CH375B              | 5V      | 50mA      | Arduino   |
| Luces blancas       | 12V     | 1000mA    | Ext 12V   |
| LEDs rojos          | 12V     | 500mA     | Ext 12V   |
| Ventilador ext      | 12V     | 300mA     | Ext 12V   |
| Ventilador int      | 12V     | 200mA     | Ext 12V   |
| Calefactora         | 12V     | 2000mA    | Ext 12V   |
| Humidificador       | 12V     | 500mA     | Ext 12V   |
| **TOTAL 5V**        |         | **~580mA**| 2A OK     |
| **TOTAL 12V**       |         | **~4.5A** | 5A OK     |

## 🛡️ Protecciones Recomendadas

### 1. Diodos de Protección

Para todos los actuadores inductivos (ventiladores, relés):

```
            Carga
             ││
         ┌───┘└───┐
         │  ┌─┐   │
      +  │  │↓│   │  - 
    ─────┘  └─┘   └─────
           1N4007
```

### 2. Fusibles

- Fuente 12V: Fusible 6A
- Fuente 5V: Fusible 2.5A
- Calefactora: Fusible individual 3A

### 3. Reguladores de Voltaje (Opcional)

Si alimentas sensores desde 12V:

```
12V → 7805 → 5V para sensores
      │
    100µF  →  GND
    (in)
      │
    10µF   →  GND
    (out)
```

## 📦 Layout Físico Sugerido

```
┌─────────────────────────────────────────────────────┐
│  CAJA DE CONTROL                                    │
│                                                     │
│  ┌──────────┐    ┌──────────┐   ┌────────┐        │
│  │ Arduino  │    │  OLED    │   │ Relés  │        │
│  │  Mega    │    │ Display  │   │ x5-6   │        │
│  └──────────┘    └──────────┘   └────────┘        │
│                                                     │
│  ┌──────────┐    ┌──────────┐   ┌────────┐        │
│  │ Fuente   │    │ Fuente   │   │ Bornes │        │
│  │  12V     │    │   5V     │   │ Salida │        │
│  └──────────┘    └──────────┘   └────────┘        │
│                                                     │
│  Cables saliendo por abajo a germinador            │
└─────────────────────────────────────────────────────┘
```

## 🔍 Verificación de Conexiones

### Checklist antes de Energizar

- [ ] Todas las conexiones GND comunes
- [ ] No hay cortocircuitos entre pines
- [ ] Polaridad correcta en todos los componentes
- [ ] Diodos flyback en ventiladores
- [ ] Relés correctamente conectados (COM, NO, NC)
- [ ] MOSFETs con resistencia gate (220Ω)
- [ ] Fuentes con voltaje correcto (medir con multímetro)
- [ ] Arduino NO conectado durante pruebas de 12V
- [ ] Cables de sección adecuada (min 0.5mm² para 12V)

### Test Paso a Paso

1. **Test de fuentes** (sin Arduino conectado)
   - Medir 5V sin carga
   - Medir 12V sin carga

2. **Test de Arduino solo**
   - Conectar 5V
   - Verificar LED power
   - Cargar sketch de prueba (Blink)

3. **Test de sensores**
   - Conectar DHT11 uno por uno
   - Verificar lecturas en Serial

4. **Test de displays**
   - Conectar OLED
   - Verificar I2C scanner

5. **Test de actuadores** (uno por uno)
   - Conectar fuente 12V
   - Activar desde código
   - Medir voltaje en salida

## 📐 Esquema Eléctrico Completo (ASCII)

```
                       ARDUINO MEGA 2560
                      ┌─────────────────┐
                      │                 │
    DHT Superior  ────┤ 2           5V  ├──── 5V Bus
    DHT Inferior  ────┤ 3          GND  ├──── GND Bus
    DHT Puerta    ────┤ 4          20   ├────┐
    Vent Ext PWM  ────┤ 5          21   ├──┐ │
    Luces PWM     ────┤ 6          19   ├┐ │ │ I2C
    Calefactora   ────┤ 7          18   ││ │ │
    Vent Int      ────┤ 8               ││ │ │
    LEDs Rojos    ────┤ 9               ││ │ │
    Humidificador ────┤10               ││ │ │
    Buzzer        ────┤11               ││ │ │
    CH375 INT     ────┤12               ││ │ │
                      └─────────────────┘│ │ │
                                         │ │ │
                        CH375B ──────────┴─┘ │
                      ┌──────────┐           │
                      │ VCC  GND │           │
                      │ TXD  RXD │           │
                      │ INT      │           │
                      └──────────┘           │
                                             │
                   OLED + RTC ───────────────┴──
                ┌─────────────────┐
                │ VCC  GND SDA SCL│
                └─────────────────┘
```

## 🔗 Referencias

- [Arduino Mega Pinout](https://www.arduino.cc/en/Hacking/PinMapping2560)
- [MOSFET como Switch](https://www.electronics-tutorials.ws/transistor/tran_7.html)
- [Relay Module Guide](https://www.instructables.com/How-to-use-a-relay/)
- [I2C Bus](https://learn.sparkfun.com/tutorials/i2c)

---

**Verifica dos veces, conecta una vez. ⚡🔌**
