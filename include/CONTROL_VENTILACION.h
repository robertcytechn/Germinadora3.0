#ifndef CONTROL_VENTILACION_H
#define CONTROL_VENTILACION_H

#include <Arduino.h>
#include <RTClib.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  CONTROL DE VENTILACIÓN INTERNA
// =================================================================

void controlVentilacionInterna(){
    // REGLA DE ORO: Si hay calefacción, el ventilador interno DEBE estar encendido para mover el calor
    if (estatusResistencia) {
        digitalWrite(VENTINTER_PIN, HIGH);
        ventInternoActivo = true;
        ultimaVentInterno = millis(); // Reseteamos timer para que al apagar calor, siga un ciclo normal
    } 
    else {
        // Si no hay calefacción, ciclo de mantenimiento aire (2 min ON / 15 min OFF)
        unsigned long tiempoPasado = millis() - ultimaVentInterno;
        
        if (ventInternoActivo) {
            // Estamos ventilando, checar si toca apagar
            if (tiempoPasado >= T_VENT_INT_ON) {
                digitalWrite(VENTINTER_PIN, LOW);
                ventInternoActivo = false;
                ultimaVentInterno = millis();
            }
        } else {
            // Estamos en descanso, checar si toca encender
            if (tiempoPasado >= T_VENT_INT_OFF) {
                digitalWrite(VENTINTER_PIN, HIGH);
                ventInternoActivo = true;
                ultimaVentInterno = millis();
            }
        }
    }
}

// =================================================================
//  CONTROL DE VENTILACIÓN EXTERNA
// =================================================================

// Objeto RTC externo (declarado en main.cpp)
extern RTC_DS1307 reloj;

void controlVentilacionExterna(){
    DateTime ahora = reloj.now();
    int horaActual = ahora.hour();
    
    // NUEVO: Desactivar completamente durante el ciclo nocturno
    // Solo opera durante el día (entre initDia y finDia)
    if (horaActual < initDia || horaActual >= finDia) {
        // Apagar ventilación externa durante la noche
        if (potenciaVentiladorexterno != 0) {
            analogWrite(VENTILADOR_PIN, 0);
            potenciaVentiladorexterno = 0;
            estadoVentExt = 0;
            Serial.println(F("Ventilacion externa OFF - Ciclo nocturno"));
        }
        return; // No hacer nada durante la noche
    }
    
    // ===== CÓDIGO SOLO PARA CICLO DIURNO =====
    // Máquina de Estados para ciclo optimizado para alta humedad
    // Estado 0: Basal (15% suave), Estado 1: Ráfaga (60% moderado), Estado 2: Descanso (0%)
    
    unsigned long tiempoEnCiclo = millis() - inicioCicloExt;

    // Lógica de transición de estados
    if (estadoVentExt == 0 && tiempoEnCiclo >= T_EXT_BASAL) {
        estadoVentExt = 1; // Pasar a Ráfaga
        inicioCicloExt = millis();
    } 
    else if (estadoVentExt == 1 && tiempoEnCiclo >= T_EXT_RAFAGA) {
        estadoVentExt = 2; // Pasar a Descanso
        inicioCicloExt = millis();
    } 
    else if (estadoVentExt == 2 && tiempoEnCiclo >= T_EXT_DESCANSO) {
        estadoVentExt = 0; // Volver a Basal
        inicioCicloExt = millis();
    }

    // Asignación de potencia según estado
    if (estadoVentExt == 0) potenciaVentiladorexterno = PWM_EXT_BASAL;
    else if (estadoVentExt == 1) potenciaVentiladorexterno = PWM_EXT_RAFAGA;
    else potenciaVentiladorexterno = PWM_EXT_OFF;

    // OVERRIDE DE SEGURIDAD (solo durante el día - Emergencia de calor)
    // Si la temperatura supera por 2°C la meta, forzar ventilación máxima
    if (tempPromedio > tempDia + 2.0) {
        potenciaVentiladorexterno = 255; // Máxima potencia en emergencia
    }
    
    // OVERRIDE DE SEGURIDAD (solo durante el día - Humedad excesiva - riesgo de hongos)
    // Si la humedad supera el máximo, aumentar ventilación para secar
    if (humPromedio > humMaxSeguridad) {
        potenciaVentiladorexterno = 200; // Alta ventilación para reducir humedad
    }

    analogWrite(VENTILADOR_PIN, potenciaVentiladorexterno);
}

#endif // CONTROL_VENTILACION_H
