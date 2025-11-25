#ifndef CONTROL_HUMEDAD_H
#define CONTROL_HUMEDAD_H

#include <Arduino.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  CONTROL DE HUMIDIFICACIÓN
// =================================================================

// TODO: Implementar cuando se conecte humidificador físicamente
void controlHumedad() {
    /*
    // Control de humedad con histéresis
    // ENCENDER si: Humedad < (Objetivo - Control)
    if (humPromedio < (humObjetivo - humControl)) {
        digitalWrite(HUMIDIFICADOR_PIN, HIGH);
        Serial.println(F("Humidificador ON"));
    }
    // APAGAR si: Humedad >= Objetivo
    else if (humPromedio >= humObjetivo) {
        digitalWrite(HUMIDIFICADOR_PIN, LOW);
        Serial.println(F("Humidificador OFF"));
    }
    
    // Seguridad: apagar si humedad muy alta
    if (humPromedio > humMaxSeguridad) {
        digitalWrite(HUMIDIFICADOR_PIN, LOW);
        Serial.println(F("Humidificador OFF - Humedad maxima alcanzada"));
    }
    */
}

#endif // CONTROL_HUMEDAD_H
