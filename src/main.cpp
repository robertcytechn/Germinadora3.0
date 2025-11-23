#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include <PINS.h>
#include <VARS.h>
// #include <CH375.h>  // Descomentar cuando se conecte el CH375B físicamente
// #include <MENU.h>   // Descomentar cuando se conecte el Joystick ARD-358 físicamente

// =================================================================
//  OBJETOS GLOBALES
// =================================================================
RTC_DS1307 reloj;
DHT dhtSuperior(DHT_SUPERIOR_PIN, DHT11);
DHT dhtInferior(DHT_INFERIOR_PIN, DHT11);
DHT dhtPuerta(DHT_PUERTA_PIN, DHT11);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_ADDR);

// CH375 usbHost(Serial1, CH375_INT_PIN); // Descomentar cuando se conecte el CH375B

// Sistema de Menú Interactivo (Descomentar cuando se conecte Joystick ARD-358)
// MenuSystem menu(&display);

// =================================================================
//  PROTOTIPOS DE FUNCIONES
// =================================================================
void testPerifericos();
void inicializarPines();
void displayTestMensaje(const String& texto, bool limpiar);

// Funciones de control principales
void leerTemperaturaHumedad();
void controlLuces();
void controlCalefactora();
void controlVentilacionInterna();
void controlVentilacionExterna();
void mostrarPantalla();

// Funciones de seguridad y monitoreo
void verificarAlarmas();
void activarAlarma(byte codigo);
void desactivarAlarma();

// Funciones para implementación futura
void controlHumedad();      // TODO: Implementar cuando se conecte humidificador
void guardarDatosUSB();     // TODO: Implementar cuando se conecte CH375B
void inicializarUSB();      // TODO: Implementar cuando se conecte CH375B

// =================================================================
//  SETUP Y LOOP PRINCIPAL
// =================================================================

void setup() {
    Wire.begin();
    Serial.begin(9600);
    
    // Inicializar RTC
    reloj.begin();
    // IMPORTANTE: Descomentar SOLO UNA VEZ para ajustar la hora del RTC
    // reloj.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
    // Inicializar pantalla OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("ERROR CRITICO: Fallo OLED. Sistema detenido."));
        for(;;); // Bucle infinito - detener sistema
    }

    // Ejecutar test de periféricos (incluye inicialización de pines)
    testPerifericos();

    // Inicializar temporizadores para el loop principal
    inicioCicloExt = millis();
    ultimoLogging = millis();
    
    // TODO: Descomentar cuando se conecte CH375B físicamente
    // inicializarUSB();
    
    // TODO: Descomentar cuando se conecte Joystick ARD-358 físicamente
    // menu.inicializar();
    
    Serial.println(F("Sistema iniciado correctamente."));
    Serial.println(F("Configuracion: Drosera capensis"));
    Serial.print(F("Temp Dia: ")); Serial.print(tempDia); Serial.println(F("C"));
    Serial.print(F("Temp Noche: ")); Serial.print(tempNoche); Serial.println(F("C"));
    Serial.print(F("Humedad Objetivo: ")); Serial.print(humObjetivo); Serial.println(F("%"));
    Serial.println(F(""));
    Serial.println(F("IMPORTANTE: Calefaccion y ventilacion externa"));
    Serial.println(F("se desactivan automaticamente durante la noche"));
}

// =================================================================
//  RUTINA DE TEST DE PERIFÉRICOS (CON RAMPA PWM MEJORADA)
// =================================================================

void inicializarPines() {
    pinMode(LUCES_PIN, OUTPUT);
    pinMode(CALEFACTORA_PIN, OUTPUT);
    pinMode(VENTINTER_PIN, OUTPUT);
    pinMode(VENTILADOR_PIN, OUTPUT);
    pinMode(LEDS_ROJOS_PIN, OUTPUT);

    // Inicializar salidas seguras
    digitalWrite(CALEFACTORA_PIN, LOW);
    digitalWrite(VENTINTER_PIN, LOW);
    digitalWrite(LEDS_ROJOS_PIN, LOW);
    analogWrite(LUCES_PIN, 0);
    analogWrite(VENTILADOR_PIN, 0);
}

void displayTestMensaje(const String& texto, bool limpiar = false) {
    if (limpiar) {
        display.clearDisplay();
        display.setCursor(0, 0);
    }
    display.println(texto);
    display.display();
    Serial.println(texto); // También por puerto serie
}

void testPerifericos() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    displayTestMensaje("--- TEST INICIADO ---");
    delay(1000);

    // 1. Inicializar pines a un estado seguro
    inicializarPines();
    displayTestMensaje("Pines inicializados.");
    delay(1000);

    // 2. Testear e Inicializar Reloj RTC
    displayTestMensaje("Probando RTC...", true);
    if (reloj.begin()) {
        displayTestMensaje("RTC: OK");
        // Descomenta la siguiente línea UNA SOLA VEZ para ajustar la hora.
        // reloj.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        displayTestMensaje("RTC: FALLO");
    }
    delay(1500);

    // 3. Testear e Inicializar Sensores DHT
    displayTestMensaje("Probando DHT Superior...", true);
    dhtSuperior.begin();
    delay(250); // Pequeña pausa para que el sensor estabilice
    if (isnan(dhtSuperior.readTemperature())) {
        displayTestMensaje("DHT Superior: FALLO");
    } else {
        displayTestMensaje("DHT Superior: OK");
    }
    delay(1500);

    displayTestMensaje("Probando DHT Inferior...", true);
    dhtInferior.begin();
    delay(250);
    if (isnan(dhtInferior.readTemperature())) {
        displayTestMensaje("DHT Inferior: FALLO");
    } else {
        displayTestMensaje("DHT Inferior: OK");
    }
    delay(1500);

    displayTestMensaje("Probando DHT Puerta...", true);
    dhtPuerta.begin();
    delay(250);
    if (isnan(dhtPuerta.readTemperature())) {
        displayTestMensaje("DHT Puerta: FALLO");
    } else {
        displayTestMensaje("DHT Puerta: OK");
    }
    delay(2000);

    // 4. Test Visual de Actuadores (con rampa PWM mejorada)
    const int pinesActuadores[] = {LUCES_PIN, VENTILADOR_PIN, CALEFACTORA_PIN, VENTINTER_PIN, LEDS_ROJOS_PIN};
    const String nombresActuadores[] = {"Luces", "Vent. Ext", "Calefactor", "Vent. Int", "LEDs Rojos"};
    const bool esPwm[] = {true, true, false, false, false}; // Indica si el pin es PWM

    for (int i = 0; i < 5; i++) {
        
        if (esPwm[i]) {
            // --- TEST DE RAMPA PWM ---
            // Rampa de subida (10 segundos)
            displayTestMensaje("Probando " + nombresActuadores[i] + "\n0% -> 100%", true);
            for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
                analogWrite(pinesActuadores[i], dutyCycle);
                delay(39); // 10000ms / 256 pasos ~= 39ms
            }
            delay(1000); // Mantener al máximo por 1 segundo

            // Rampa de bajada (10 segundos)
            displayTestMensaje("Probando " + nombresActuadores[i] + "\n100% -> 0%", true);
            for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
                analogWrite(pinesActuadores[i], dutyCycle);
                delay(39);
            }
            analogWrite(pinesActuadores[i], 0); // Asegurar que queda apagado
            delay(1000);

        } else {
            // --- TEST DIGITAL ON/OFF ---
            displayTestMensaje("Probando: " + nombresActuadores[i], true);
            digitalWrite(pinesActuadores[i], HIGH);
            delay(2000); // Mantenemos 2 seg para verificar
            digitalWrite(pinesActuadores[i], LOW);
            delay(500);
        }
    }

    displayTestMensaje("--- TEST FINALIZADO ---", true);
    delay(2000);

    // Limpiar pantalla para el programa principal
    display.clearDisplay();
    display.display();
}


// =================================================================
//  FUNCIONES DE CONTROL DEL SISTEMA
// =================================================================

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

    // OVERRIDE DE SEGURIDAD (Emergencia de calor)
    // Si la temperatura supera por 2°C la meta, forzar ventilación máxima
    float target = (reloj.now().hour() >= initDia && reloj.now().hour() <= finDia) ? tempDia : tempNoche;
    if (tempPromedio > target + 2.0) {
        potenciaVentiladorexterno = 255; // Máxima potencia en emergencia
    }
    
    // OVERRIDE DE SEGURIDAD (Humedad excesiva - riesgo de hongos)
    // Si la humedad supera el máximo, aumentar ventilación para secar
    if (humPromedio > humMaxSeguridad) {
        potenciaVentiladorexterno = 200; // Alta ventilación para reducir humedad
    }

    analogWrite(VENTILADOR_PIN, potenciaVentiladorexterno);
}

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
    display.print(" Max:");
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

// =================================================================
//  FUNCIONES DE SEGURIDAD Y ALARMAS
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

// =================================================================
//  FUNCIONES PARA IMPLEMENTACIÓN FUTURA
// =================================================================

// TODO: Implementar cuando se conecte humidificador/nebulizador
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

// TODO: Implementar cuando se conecte CH375B físicamente
void inicializarUSB() {
    /*
    Serial1.begin(9600); // Inicializar Serial1 para CH375B
    pinMode(CH375_INT_PIN, INPUT);
    
    Serial.println(F("Inicializando CH375B..."));
    
    // Inicializar el CH375 en modo host
    if (usbHost.init()) {
        Serial.println(F("CH375B inicializado correctamente"));
        usbConectado = false;
    } else {
        Serial.println(F("Error al inicializar CH375B"));
    }
    */
}

// TODO: Implementar cuando se conecte CH375B físicamente
void guardarDatosUSB() {
    /*
    static unsigned long ultimoGuardado = 0;
    
    // Guardar datos cada TIEMPO_LOGGING (30 minutos)
    if (millis() - ultimoGuardado < TIEMPO_LOGGING) return;
    
    // Verificar si hay USB conectado
    if (!usbConectado) {
        Serial.println(F("Intentando detectar USB..."));
        // Aquí iría la lógica de detección USB con CH375
        // if (usbHost.detectDevice()) usbConectado = true;
        return;
    }
    
    // Crear string con datos en formato CSV
    DateTime now = reloj.now();
    String datos = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + ",";
    datos += String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ",";
    datos += String(tempPromedio, 2) + ",";
    datos += String(humPromedio, 2) + ",";
    datos += String(temperaturas[0], 2) + ",";
    datos += String(temperaturas[1], 2) + ",";
    datos += String(temperaturas[2], 2) + ",";
    datos += String(humedades[0], 2) + ",";
    datos += String(humedades[1], 2) + ",";
    datos += String(humedades[2], 2) + ",";
    datos += String(potenciadeluces) + ",";
    datos += String(estatusResistencia) + ",";
    datos += String(alarmaActiva) + ",";
    datos += String(codigoAlarma);
    datos += "\n";
    
    // Aquí iría la lógica para escribir en el USB usando CH375
    // usbHost.write(datos.c_str());
    
    registrosGuardados++;
    ultimoGuardado = millis();
    
    Serial.print(F("Datos guardados en USB. Registro #"));
    Serial.println(registrosGuardados);
    */
}

void loop() {
    // Loop principal - SIN DELAYS (totalmente no bloqueante)
    
    // TODO: Descomentar cuando se conecte Joystick ARD-358
    // Actualizar sistema de menú (detecta activación y navegación)
    // menu.actualizar();
    
    leerTemperaturaHumedad();      // Actualiza solo si ha pasado el tiempo definido
    controlLuces();                 // Control de iluminación diurna/nocturna
    controlCalefactora();           // Control térmico con verificación de humedad (SOLO DÍA)
    controlVentilacionInterna();    // Ventilación interna con ciclos
    controlVentilacionExterna();    // Ventilación externa con estados (SOLO DÍA)
    mostrarPantalla();              // Actualiza solo cada segundo (o muestra menú)
    verificarAlarmas();             // Sistema de seguridad y monitoreo
    
    // TODO: Descomentar cuando se conecten los dispositivos físicos
    // controlHumedad();            // Control de humidificador/nebulizador
    // guardarDatosUSB();           // Logging cada 30 minutos en USB
    
    // Sin delay - el loop se ejecuta continuamente
    // Cada función controla su propia temporización con millis()
}