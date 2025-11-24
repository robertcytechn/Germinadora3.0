# Guía de Cableado de Botones para el Menú

## Hardware Necesario

- 3 botones pulsadores (normalmente abiertos)
- Arduino Mega 2560
- Cable jumper
- **NO SE NECESITAN resistencias pull-up** (se usan las internas del Arduino)

## Esquema de Conexión

### Botón ARRIBA (Pin 24)
```
Botón ARRIBA
├─ Terminal 1 → Pin 24 (Arduino Mega)
└─ Terminal 2 → GND (Arduino Mega)
```

### Botón ABAJO (Pin 25)
```
Botón ABAJO
├─ Terminal 1 → Pin 25 (Arduino Mega)
└─ Terminal 2 → GND (Arduino Mega)
```

### Botón ENTER (Pin 26)
```
Botón ENTER
├─ Terminal 1 → Pin 26 (Arduino Mega)
└─ Terminal 2 → GND (Arduino Mega)
```

## Configuración del Software

Los pines están configurados en `MENU.h` con resistencias pull-up internas activadas:

```cpp
// Pines de botones
const int BTN_ARRIBA_PIN = 24;  // Botón ARRIBA
const int BTN_ABAJO_PIN = 25;   // Botón ABAJO
const int BTN_ENTER_PIN = 26;   // Botón ENTER/SELECT
```

En la función `inicializar()`:
```cpp
pinMode(BTN_ARRIBA_PIN, INPUT_PULLUP);
pinMode(BTN_ABAJO_PIN, INPUT_PULLUP);
pinMode(BTN_ENTER_PIN, INPUT_PULLUP);
```

## Funcionamiento

### Lógica de Lectura
- **Estado REPOSO**: Pin en HIGH (gracias al pull-up interno)
- **Botón PRESIONADO**: Pin en LOW (conectado a GND)

### Tiempos de Debounce y Repetición
```cpp
const unsigned long BTN_DEBOUNCE = 50;        // 50ms anti-rebote
const unsigned long BTN_REPEAT_DELAY = 500;   // 500ms antes de repetición
const unsigned long BTN_REPEAT_RATE = 100;    // 100ms entre repeticiones
```

### Activación del Menú
**Mantener presionado el botón ENTER por 2 segundos** para activar el menú principal.

### Navegación
- **ARRIBA**: Moverse hacia arriba en el menú o incrementar valor
- **ABAJO**: Moverse hacia abajo en el menú o decrementar valor
- **ENTER**: Seleccionar opción o salir del modo edición

### Edición de Valores
Cuando estás editando un valor (temperatura, humedad, etc.):
- **Presión corta ARRIBA/ABAJO**: Cambio único
- **Mantener presionado ARRIBA/ABAJO**: Repetición automática después de 500ms
  - Los valores cambian cada 100ms mientras mantienes presionado

## Diagrama ASCII Completo

```
                    ARDUINO MEGA 2560
                    ┌────────────────┐
                    │                │
   ARRIBA ─┐        │         Pin 24 ├─── BTN ARRIBA
           └── GND ─┤ GND            │
                    │                │
   ABAJO ──┐        │         Pin 25 ├─── BTN ABAJO
           └── GND ─┤ GND            │
                    │                │
   ENTER ──┐        │         Pin 26 ├─── BTN ENTER
           └── GND ─┤ GND            │
                    │                │
                    └────────────────┘

Nota: Cada botón conecta su pin al GND cuando se presiona
```

## Comparación con Sistema Anterior (Joystick)

| Característica | Joystick ARD-358 | Botones |
|---------------|------------------|---------|
| Pines usados | A0, A1, Pin 22 | Pin 24, 25, 26 |
| Tipo de lectura | Analógica (0-1023) | Digital (HIGH/LOW) |
| Resistencias externas | Incluidas en módulo | No necesarias |
| Costo aproximado | $3-5 USD | $0.30-1 USD |
| Complejidad | Media | Baja |
| Zona muerta (deadzone) | Sí (software) | No necesaria |
| Debounce | Sí | Sí |

## Ventajas del Sistema de Botones

1. **Simplicidad**: Solo lectura digital, no analógica
2. **Costo**: Más económico que un joystick
3. **Disponibilidad**: Fácil de encontrar en cualquier tienda
4. **Confiabilidad**: Menos propenso a deriva o ruido
5. **Tamaño**: Ocupa menos espacio en el panel
6. **Mantenimiento**: Más fácil de reemplazar individualmente

## Solución de Problemas

### Problema: El menú se activa solo
**Causa**: Botón con mala conexión o rebotes excesivos  
**Solución**: Verificar conexiones, aumentar `BTN_DEBOUNCE` a 100ms

### Problema: Los botones no responden
**Causa**: Conexión invertida o pin incorrecto  
**Solución**: 
- Verificar que un lado del botón esté a GND
- Revisar que los pines sean 24, 25, 26
- Verificar que INPUT_PULLUP esté configurado

### Problema: La repetición es muy rápida/lenta
**Solución**: Ajustar `BTN_REPEAT_RATE`:
- Más rápido: Reducir a 50ms
- Más lento: Aumentar a 200ms

### Problema: Demora antes de la repetición muy larga
**Solución**: Ajustar `BTN_REPEAT_DELAY`:
- Repetición más rápida: Reducir a 300ms
- Más conservador: Aumentar a 800ms

## Código de Prueba

Para verificar que los botones funcionan correctamente, puedes usar este código de prueba:

```cpp
void setup() {
    Serial.begin(9600);
    pinMode(24, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
}

void loop() {
    Serial.print("ARRIBA: ");
    Serial.print(digitalRead(24) == LOW ? "PRESS" : "----");
    Serial.print(" | ABAJO: ");
    Serial.print(digitalRead(25) == LOW ? "PRESS" : "----");
    Serial.print(" | ENTER: ");
    Serial.println(digitalRead(26) == LOW ? "PRESS" : "----");
    delay(100);
}
```

Deberías ver "PRESS" cuando presionas cada botón.

## Referencias

- Documentación Arduino INPUT_PULLUP: https://www.arduino.cc/reference/en/language/variables/constants/constants/
- Debouncing en Arduino: https://docs.arduino.cc/built-in-examples/digital/Debounce/
