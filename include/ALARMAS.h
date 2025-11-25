#ifndef ALARMAS_H
#define ALARMAS_H

#include <Arduino.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  SISTEMA DE ALARMAS Y SEGURIDAD
// =================================================================

void verificarAlarmas() {
    // Verificar temperaturas fuera de rango
    if (tempPromedio > tempMaxSeguridad) {
        activarAlarma(1); // Código 1: Temperatura alta
        return;
    }
    if (tempPromedio < tempMinSeguridad) {
        activarAlarma(2); // Código 2: Temperatura baja
        return;
    }
    
    // Verificar humedad fuera de rango
    if (humPromedio > humMaxSeguridad) {
        activarAlarma(3); // Código 3: Humedad alta (riesgo hongos)
        return;
    }
    if (humPromedio < humMinSeguridad) {
        activarAlarma(4); // Código 4: Humedad baja (deshidratación)
        return;
    }
    
    // Verificar fallo de sensores (todas las lecturas iguales o cero)
    if (temperaturas[0] == 0 && temperaturas[1] == 0 && temperaturas[2] == 0) {
        activarAlarma(5); // Código 5: Fallo de sensores
        return;
    }
    
    // Si todo está bien, desactivar alarma
    if (alarmaActiva) {
        desactivarAlarma();
    }
}

void activarAlarma(byte codigo) {
    if (alarmaActiva && codigoAlarma == codigo) return; // Ya está activa
    
    alarmaActiva = true;
    codigoAlarma = codigo;
    
    // Imprimir en serial
    Serial.print(F("*** ALARMA ACTIVADA - Codigo: "));
    Serial.print(codigo);
    Serial.print(F(" - "));
    
    switch(codigo) {
        case 1: Serial.println(F("TEMPERATURA ALTA")); break;
        case 2: Serial.println(F("TEMPERATURA BAJA")); break;
        case 3: Serial.println(F("HUMEDAD ALTA - Riesgo hongos")); break;
        case 4: Serial.println(F("HUMEDAD BAJA - Riesgo deshidratacion")); break;
        case 5: Serial.println(F("FALLO DE SENSORES")); break;
        default: Serial.println(F("DESCONOCIDA")); break;
    }
    
    // TODO: Descomentar cuando se conecte buzzer físicamente
    // pinMode(BUZZER_PIN, OUTPUT);
    // tone(BUZZER_PIN, 1000, 500); // Beep de 1000Hz por 500ms
}

void desactivarAlarma() {
    if (!alarmaActiva) return;
    
    Serial.println(F("Alarma desactivada - Condiciones normalizadas"));
    alarmaActiva = false;
    codigoAlarma = 0;
    
    // TODO: Descomentar cuando se conecte buzzer físicamente
    // noTone(BUZZER_PIN);
}

#endif // ALARMAS_H
