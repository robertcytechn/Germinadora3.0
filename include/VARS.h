#ifndef VARS_H
#define VARS_H

#include <Arduino.h>

// =================================================================
//  DECLARACIONES DE VARIABLES GLOBALES
// =================================================================
// IMPORTANTE: Las definiciones están en main.cpp

// --- CONFIGURACIÓN DE TEMPERATURA (Optimizado para Drosera capensis) ---
extern float tempDia;           // 24-28°C óptimo para Drosera capensis
extern float tempNoche;         // 12-18°C ideal para descanso nocturno
extern float tempControl;        // Histeresis (Margen de error)
extern float tempMaxSeguridad;  // Temperatura máxima de seguridad
extern float tempMinSeguridad;  // Temperatura mínima de seguridad

// --- CONFIGURACIÓN DE HUMEDAD (CRÍTICO para plantas carnívoras) ---
extern float humObjetivo;       // 70-85% ideal para Drosera capensis
extern float humControl;         // Margen de control ±5%
extern float humMaxSeguridad;   // Humedad máxima (riesgo de hongos)
extern float humMinSeguridad;   // Humedad mínima (riesgo de deshidratación)

// --- CONFIGURACIÓN DE ILUMINACIÓN ---
extern int initDia;                // Hora amanecer (7 AM)
extern int finDia;                // Hora anochecer (9 PM) - 14h luz total
extern int duracionAmanecer;      // 1.5 horas de transición gradual

// --- TIEMPOS (Todo en milisegundos para usar directo con millis) ---
extern const unsigned long TIEMPO_LECTURA_DHT;         // Leer sensores cada 2 segundos
extern const unsigned long TIEMPO_ACTUALIZACION_PANTALLA; // Actualizar pantalla cada 1 segundo
extern const unsigned long TIEMPO_LOGGING;    // Guardar datos cada 30 minutos

// Tiempos Ventilación Interna (Optimizado para Drosera - evitar hongos)
extern const unsigned long T_VENT_INT_ON;   // 3 min encendido (aumentado para mejor circulación)
extern const unsigned long T_VENT_INT_OFF; // 12 min apagado (reducido para evitar condensación)

// Tiempos Ventilación Externa (Ciclo suave para no deshidratar)
extern const unsigned long T_EXT_BASAL;    // 50 min al 15% (muy suave)
extern const unsigned long T_EXT_RAFAGA;    // 5 min al 60% (moderado, no 100%)
extern const unsigned long T_EXT_DESCANSO;  // 5 min apagado (reducido)

// PWM para Ventilador Externo (Optimizado para alta humedad)
extern const int PWM_EXT_BASAL;      // ~15% de potencia (muy suave, circulación mínima)
extern const int PWM_EXT_RAFAGA;    // ~60% potencia (moderado, no máximo)
extern const int PWM_EXT_OFF;

// --- VARIABLES DE ESTADO (No tocar, uso interno) ---
extern float temperaturas[3];
extern float humedades[3];
extern float tempPromedio;
extern float humPromedio;
extern float temperaturaMax;

extern int potenciadeluces;
extern int potenciaVentiladorexterno;
extern bool lucesRojasOn;      // Estado leds rojos
extern bool estatusResistencia;// Estado calefactor

// Temporizadores internos
extern unsigned long ultimaVentInterno;
extern bool ventInternoActivo;

// Máquina de estados Ventilación Externa
extern unsigned long inicioCicloExt;
extern int estadoVentExt; // 0: Basal, 1: Ráfaga, 2: Descanso

// Seguridad Calefacción
extern unsigned long inicioCalefaccion;
extern const unsigned long MAX_TIEMPO_CALEFACCION; // Seguridad: Max 10 min seguidos (aumentado)

// --- SISTEMA DE ALARMAS (Para implementación futura con buzzer) ---
extern bool alarmaActiva;
extern byte codigoAlarma; // 0=OK, 1=TempAlta, 2=TempBaja, 3=HumAlta, 4=HumBaja, 5=SensorFallo

// --- SISTEMA DE LOGGING USB (CH375B) ---
extern unsigned long ultimoLogging;
extern bool usbConectado;
extern unsigned long registrosGuardados;

#endif // VARS_H
