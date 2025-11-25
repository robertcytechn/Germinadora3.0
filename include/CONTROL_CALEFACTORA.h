#ifndef CONTROL_CALEFACTORA_H
#define CONTROL_CALEFACTORA_H

#include <Arduino.h>
#include <RTClib.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  CONTROL DE CALEFACCIÓN
// =================================================================

// Objeto RTC externo (declarado en main.cpp)
extern RTC_DS1307 reloj;

void controlCalefactora(){
    DateTime ahora = reloj.now();
    int horaActual = ahora.hour();
    
    // NUEVO: Desactivar completamente durante el ciclo nocturno
    // Solo opera durante el día (entre initDia y finDia)
    if (horaActual < initDia || horaActual >= finDia) {
        if (estatusResistencia) {
            estatusResistencia = false;
            digitalWrite(CALEFACTORA_PIN, LOW);
            Serial.println(F("Calefaccion OFF - Ciclo nocturno"));
        }
        return; // No hacer nada durante la noche
    }
    
    // Determinar setpoint según hora del día
    float target = (horaActual >= initDia && horaActual <= finDia) ? tempDia : tempNoche;

    // VERIFICACIÓN DE HUMEDAD ANTES DE CALENTAR (CRÍTICO para Drosera)
    // Si la humedad está por debajo del mínimo de seguridad, NO calentar
    // (el calor seca aún más el ambiente)
    if (humPromedio < humMinSeguridad) {
        // Apagar calefacción si está activa
        if (estatusResistencia) {
            estatusResistencia = false;
            digitalWrite(CALEFACTORA_PIN, LOW);
            Serial.println(F("ADVERTENCIA: Calefaccion desactivada - Humedad muy baja"));
        }
        return; // Salir de la función
    }

    // Lógica de Histéresis y Seguridad
    // ENCENDER si: Temp < (Objetivo - Histéresis) Y no hemos excedido tiempo de seguridad
    if (!estatusResistencia && tempPromedio <= (target - tempControl)) {
        estatusResistencia = true;
        inicioCalefaccion = millis();
        digitalWrite(CALEFACTORA_PIN, HIGH);
        Serial.print(F("Calefaccion ON - Target: ")); Serial.println(target);
    }
    
    // APAGAR si: Temp >= Objetivo O tiempo de seguridad excedido
    else if (estatusResistencia) {
        bool tiempoExcedido = (millis() - inicioCalefaccion >= MAX_TIEMPO_CALEFACCION);
        
        if (tempPromedio >= target || tiempoExcedido) {
            estatusResistencia = false;
            digitalWrite(CALEFACTORA_PIN, LOW);
            if (tiempoExcedido) {
                Serial.println(F("Calefaccion OFF - Tiempo maximo alcanzado"));
            }
        }
    }
}

#endif // CONTROL_CALEFACTORA_H
