# 💧 Guía de Implementación del Control de Humedad

## 🌿 Importancia para Drosera Capensis

Las plantas carnívoras Drosera capensis **requieren alta humedad** (70-85%) para:
- Producción de mucílago pegajoso en las hojas
- Evitar deshidratación de las trampas
- Crecimiento saludable de raíces
- Prevenir quemaduras por baja humedad

## 🔧 Hardware Recomendado

### Opción 1: Nebulizador Ultrasónico (Recomendado)

**Ventajas:**
- Partículas de agua muy finas
- No aumenta temperatura
- Bajo consumo energético
- Silencioso

**Modelo sugerido:** Nebulizador ultrasónico de 24V/12V con controlador ON/OFF

**Especificaciones:**
- Voltaje: 12V DC (o 24V según modelo)
- Consumo: <2A
- Capacidad: 300-500ml/h
- Frecuencia: 1.7MHz típico

### Opción 2: Humidificador de Evaporación

**Ventajas:**
- Más económico
- Sin mantenimiento de membranas
- Humedad más uniforme

**Desventajas:**
- Mayor consumo
- Más lento

### Opción 3: Sistema de Nebulización por Boquillas

**Ventajas:**
- Cobertura amplia
- Profesional

**Desventajas:**
- Requiere bomba de agua
- Mayor complejidad

## 🔌 Conexión Eléctrica

### Esquema con Relé

```
Arduino Pin 10 ----> Módulo Relé (señal)
5V Arduino     ----> Módulo Relé (VCC)
GND Arduino    ----> Módulo Relé (GND)

Relé COM       ----> Fuente 12V (+)
Relé NO        ----> Nebulizador (+)
Nebulizador (-) ----> Fuente 12V (-)
```

### Alternativa con MOSFET (para cargas <2A)

```
Arduino Pin 10 ----> MOSFET Gate (con R 220Ω)
MOSFET Source  ----> GND común
MOSFET Drain   ----> Nebulizador (-)
Nebulizador (+) ----> 12V
```

**MOSFET recomendado:** IRF520, IRLZ44N

## ⚙️ Configuración en el Código

### 1. Descomentar definición de pin en PINS.h

```cpp
#define HUMIDIFICADOR_PIN 10  // Quitar el comentario //
```

### 2. Inicializar pin en setup()

Ya está preparado en `inicializarPines()`, pero si necesitas:

```cpp
void inicializarPines() {
    // ... código existente ...
    pinMode(HUMIDIFICADOR_PIN, OUTPUT);
    digitalWrite(HUMIDIFICADOR_PIN, LOW);  // Iniciar apagado
}
```

### 3. Descomentar función en loop()

```cpp
void loop() {
    // ... código existente ...
    
    controlHumedad();  // Descomentar esta línea
    
    // ... resto del código ...
}
```

### 4. Descomentar implementación de controlHumedad()

En el archivo `main.cpp`, buscar la función `controlHumedad()` y eliminar los comentarios `/*` y `*/`.

## 🎛️ Ajuste de Parámetros

En el archivo `VARS.h`:

```cpp
// Configuración de humedad
float humObjetivo = 75.0;       // 70-85% ideal para Drosera
float humControl = 5.0;         // Margen ±5%
float humMaxSeguridad = 90.0;   // Máximo absoluto (hongos)
float humMinSeguridad = 60.0;   // Mínimo absoluto (deshidratación)
```

### Comportamiento

- **Humedad < 70%** (75 - 5): Activa nebulizador
- **Humedad >= 75%**: Apaga nebulizador
- **Humedad > 90%**: Apaga nebulizador + aumenta ventilación
- **Humedad < 60%**: Alarma + bloquea calefacción

## 🧪 Calibración Inicial

### Paso 1: Test sin plantas

1. Conectar hardware pero **sin plantas**
2. Descomentar `controlHumedad()`
3. Cargar código y monitorear Serial
4. Observar comportamiento durante 2 horas

### Paso 2: Ajustar sensibilidad

Si la humedad oscila mucho:

```cpp
// Aumentar margen
float humControl = 7.0;  // Era 5.0
```

Si tarda mucho en alcanzar objetivo:

```cpp
// Reducir margen
float humControl = 3.0;  // Era 5.0
```

### Paso 3: Verificar tiempos

Agregar temporización mínima para evitar ciclos cortos:

```cpp
void controlHumedad() {
    static unsigned long ultimoCambio = 0;
    static bool humidificadorActivo = false;
    
    // Tiempo mínimo entre cambios: 30 segundos
    const unsigned long TIEMPO_MIN_CICLO = 30000;
    
    if (millis() - ultimoCambio < TIEMPO_MIN_CICLO) return;
    
    // Control con histéresis
    if (!humidificadorActivo && humPromedio < (humObjetivo - humControl)) {
        digitalWrite(HUMIDIFICADOR_PIN, HIGH);
        humidificadorActivo = true;
        ultimoCambio = millis();
        Serial.println(F("Humidificador ON"));
    }
    else if (humidificadorActivo && humPromedio >= humObjetivo) {
        digitalWrite(HUMIDIFICADOR_PIN, LOW);
        humidificadorActivo = false;
        ultimoCambio = millis();
        Serial.println(F("Humidificador OFF"));
    }
    
    // Seguridad
    if (humPromedio > humMaxSeguridad) {
        digitalWrite(HUMIDIFICADOR_PIN, LOW);
        humidificadorActivo = false;
        Serial.println(F("Humidificador OFF - Humedad maxima"));
    }
}
```

## 📊 Monitoreo

### En la Pantalla OLED

Ya implementado - la pantalla muestra:
- `H: 75% OK` - Humedad en rango
- `H: 58% LOW!` - Humedad baja
- `H: 92% HIGH!` - Humedad alta

### En Serial Monitor

```
Humidificador ON
H: 68.5% -> Objetivo: 75.0%
...
H: 75.2% -> Humidificador OFF
```

## ⚠️ Sistemas de Seguridad

### 1. Límite de Tiempo Continuo

Evitar que el nebulizador funcione indefinidamente:

```cpp
const unsigned long MAX_TIEMPO_HUMIDIFICADOR = 15 * 60000UL;  // 15 min
unsigned long inicioHumidificacion = 0;

if (humidificadorActivo) {
    if (millis() - inicioHumidificacion > MAX_TIEMPO_HUMIDIFICADOR) {
        digitalWrite(HUMIDIFICADOR_PIN, LOW);
        humidificadorActivo = false;
        activarAlarma(6);  // Nueva alarma: humidificador atascado
    }
}
```

### 2. Coordinación con Ventilación

El código ya lo hace - si humedad > 90%:
- Apaga humidificador
- Aumenta ventilación externa a 200 PWM

### 3. Prevención de Condensación

Si hay calefacción activa + alta humedad = condensación

```cpp
// En controlHumedad(), añadir:
if (estatusResistencia && humPromedio > 80.0) {
    // No humidificar si hay calefacción y ya hay mucha humedad
    digitalWrite(HUMIDIFICADOR_PIN, LOW);
    return;
}
```

## 🧼 Mantenimiento del Nebulizador

### Limpieza Semanal

1. Apagar sistema
2. Vaciar depósito de agua
3. Limpiar membrana con vinagre blanco (10 min)
4. Enjuagar con agua destilada
5. Secar completamente

### Limpieza Mensual

1. Desmontar completamente
2. Limpiar con solución citrato (anti-cal)
3. Verificar membrana (cambiar si agrietada)
4. Limpiar sensores de nivel
5. Rearmar y probar

### Agua Recomendada

- **Óptimo:** Agua destilada o desionizada
- **Aceptable:** Agua de lluvia filtrada
- **NO usar:** Agua de grifo (acumula cal en la membrana)

## 📐 Dimensionamiento

### Cálculo de Capacidad

Para un germinador de volumen V (litros):

```
Capacidad mínima = V × 0.05 litros/hora

Ejemplo: Germinador de 100L
Capacidad = 100 × 0.05 = 5L/h (exagerado)
Usar: 300-500 ml/h es suficiente
```

### Ubicación del Nebulizador

- **Dentro del germinador:** Mejor distribución, más humedad
- **Fuera con ducto:** Más fácil mantenimiento, menos humedad directa
- **Altura:** A media altura, la niebla sube naturalmente

## 🔬 Sensores de Humedad Adicionales (Opcional)

### Sensor Capacitivo de Suelo

Para medir humedad del sustrato:

```cpp
#define SENSOR_SUELO_PIN A0

int humedadSuelo = analogRead(SENSOR_SUELO_PIN);
// Valor típico: 0-1023 (seco a húmedo)
```

### Higrómetro de Precisión (DHT22)

Más preciso que DHT11:

```cpp
// Cambiar en main.cpp:
DHT dhtSuperior(DHT_SUPERIOR_PIN, DHT22);  // Era DHT11
```

**Mejora:** ±2% humedad (vs ±5% del DHT11)

## 🐛 Troubleshooting

### Humedad no sube

- Verificar que hay agua en el depósito
- Comprobar que el nebulizador funciona (debe verse niebla)
- Aumentar capacidad del nebulizador
- Verificar que no hay fugas de aire en el germinador
- Reducir ventilación externa

### Humedad sube demasiado

- Reducir capacidad del nebulizador
- Aumentar ventilación
- Aumentar `humControl` en VARS.h
- Verificar sensores DHT11 (pueden estar mojados)

### Oscilaciones grandes

- Aumentar `humControl` (histéresis mayor)
- Aumentar `TIEMPO_MIN_CICLO`
- Verificar ubicación de sensores (lejos del nebulizador directo)

### Condensación en paredes

- Normal si humedad > 85%
- Aumentar ventilación interna
- Reducir `humObjetivo` a 72-73%
- Mejorar aislamiento térmico

## 📚 Referencias

- [Ultrasonic Mist Maker Guide](https://www.instructables.com/Ultrasonic-Mist-Maker-Guide/)
- [Carnivorous Plants Humidity Requirements](https://www.carnivorousplants.org/)
- [DHT Sensor Accuracy](https://learn.adafruit.com/dht)

---

**¡Con el control de humedad tu Drosera capensis crecerá perfectamente! 💧🌱**
