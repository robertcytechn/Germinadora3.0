#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <DHT.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  LECTURA DE SENSORES DE TEMPERATURA Y HUMEDAD
// =================================================================

// Objetos DHT externos (declarados en main.cpp)
extern DHT dhtSuperior;
extern DHT dhtInferior;
extern DHT dhtPuerta;

void leerTemperaturaHumedad(){
    static unsigned long ultimoTiempoLecturaDHT = 0;
    if (millis() - ultimoTiempoLecturaDHT < TIEMPO_LECTURA_DHT) return;

    float t1 = dhtSuperior.readTemperature();
    float h1 = dhtSuperior.readHumidity();
    float t2 = dhtInferior.readTemperature();
    float h2 = dhtInferior.readHumidity();
    float t3 = dhtPuerta.readTemperature();
    float h3 = dhtPuerta.readHumidity();

    // Validación simple: si da NaN, usamos la última lectura válida o 0
    if (!isnan(t1)) temperaturas[0] = t1;
    if (!isnan(t2)) temperaturas[1] = t2;
    if (!isnan(t3)) temperaturas[2] = t3;
    
    if (!isnan(h1)) humedades[0] = h1;
    if (!isnan(h2)) humedades[1] = h2;
    if (!isnan(h3)) humedades[2] = h3;

    // Promedios
    tempPromedio = (temperaturas[0] + temperaturas[1] + temperaturas[2]) / 3.0;
    humPromedio = (humedades[0] + humedades[1] + humedades[2]) / 3.0;
    temperaturaMax = max(temperaturas[0], max(temperaturas[1], temperaturas[2]));
    
    ultimoTiempoLecturaDHT = millis();
}

#endif // SENSORES_H
