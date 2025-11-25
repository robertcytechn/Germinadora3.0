#ifndef CONTROL_LUCES_H
#define CONTROL_LUCES_H

#include <Arduino.h>
#include <RTClib.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  CONTROL DE ILUMINACIÓN (Blanca + Roja)
// =================================================================

// Objeto RTC externo (declarado en main.cpp)
extern RTC_DS1307 reloj;

void controlLuces(){
    DateTime ahora = reloj.now();
    int minutosActuales = ahora.hour() * 60 + ahora.minute();
    int minutosInicio = initDia * 60;
    int minutosFin = finDia * 60;

    // 1. Control Luces Blancas (Amanecer/Atardecer suave)
    if (minutosActuales >= minutosInicio && minutosActuales <= minutosFin) {
        // Amanecer gradual
        if (minutosActuales < minutosInicio + duracionAmanecer) {
            potenciadeluces = map(minutosActuales, minutosInicio, minutosInicio + duracionAmanecer, 0, 255);
        }
        // Atardecer gradual
        else if (minutosActuales > minutosFin - duracionAmanecer) {
            potenciadeluces = map(minutosActuales, minutosFin - duracionAmanecer, minutosFin, 255, 0);
        }
        // Pleno día
        else {
            potenciadeluces = 255;
        }
    } else {
        potenciadeluces = 0; // Noche
    }
    analogWrite(LUCES_PIN, potenciadeluces);

    // 2. Control LEDs Rojos (Espectro rojo para fotosíntesis)
    // Se encienden 1 hora después del inicio y se apagan 1 hora antes del fin
    // Esto da un periodo de luz roja más corto pero intenso
    if (minutosActuales >= (minutosInicio + 60) && minutosActuales <= (minutosFin - 60)) {
        digitalWrite(LEDS_ROJOS_PIN, HIGH);
        lucesRojasOn = true;
    } else {
        digitalWrite(LEDS_ROJOS_PIN, LOW);
        lucesRojasOn = false;
    }
}

#endif // CONTROL_LUCES_H
