#ifndef PANTALLA_H
#define PANTALLA_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <VARS.h>
#include <PINS.h>

// =================================================================
//  VISUALIZACIÓN EN PANTALLA OLED
// =================================================================

// Objetos externos (declarados en main.cpp)
extern Adafruit_SSD1306 display;
extern RTC_DS1307 reloj;

void mostrarPantalla(){
    static unsigned long ultimaActualizacion = 0;
    
    // TODO: Descomentar cuando se conecte Joystick ARD-358
    // Si el menú está activo, no mostrar la pantalla principal
    // if (menu.estaActivo()) {
    //     menu.mostrar();
    //     return;
    // }
    
    // Actualizar pantalla solo cada TIEMPO_ACTUALIZACION_PANTALLA (no bloqueante)
    if (millis() - ultimaActualizacion < TIEMPO_ACTUALIZACION_PANTALLA) return;
    ultimaActualizacion = millis();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    
    // Fila 1: Temperaturas
    display.print("T:");
    display.print(tempPromedio, 1); 
    display.print(" M:");
    display.print(temperaturaMax, 1);
    if (estatusResistencia) display.print(" HEAT");
    display.println();

    // Fila 2: Humedad (CRÍTICO para Drosera)
    display.print("H:");
    display.print(humPromedio, 0);
    display.print("%");
    // Indicador visual de estado de humedad
    if (humPromedio < humMinSeguridad) display.print(" LOW!");
    else if (humPromedio > humMaxSeguridad) display.print(" HIGH!");
    else display.print(" OK");
    display.println();

    // Fila 3: Estados Ventilación
    display.print("V.Ext:");
    if(potenciaVentiladorexterno == 255) display.print("MAX");
    else if(potenciaVentiladorexterno >= 150) display.print("MED");
    else if(potenciaVentiladorexterno > 0) display.print("LOW");
    else display.print("OFF");
    
    display.print(" V.Int:");
    display.println(digitalRead(VENTINTER_PIN) ? "ON" : "OFF");

    // Fila 4: Luces
    display.print("Luz:");
    display.print(map(potenciadeluces, 0, 255, 0, 100));
    display.print("% R:");
    display.println(lucesRojasOn ? "ON" : "OFF");

    // Fila 5: Hora
    DateTime now = reloj.now();
    display.print(now.hour());
    display.print(":");
    if(now.minute()<10) display.print("0");
    display.print(now.minute());
    display.print(":");
    if(now.second()<10) display.print("0");
    display.print(now.second());
    display.println();

    // Fila 6: Setpoints y alarma
    display.print("D:");
    display.print(tempDia, 0);
    display.print(" N:");
    display.print(tempNoche, 0);
    if (alarmaActiva) {
        display.print(" ALM!");
    }

    display.display();
}

#endif // PANTALLA_H
