#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include <PINS.h>
#include <VARS.h>

// =================================================================
//  MÓDULOS DE CONTROL
// =================================================================
#include <SENSORES.h>
#include <CONTROL_LUCES.h>
#include <CONTROL_CALEFACTORA.h>
#include <CONTROL_VENTILACION.h>
#include <CONTROL_HUMEDAD.h>
#include <ALARMAS.h>
#include <PANTALLA.h>
#include <USB_LOGGING.h>

// #include <CH375.h>  // Descomentar cuando se conecte el CH375B físicamente
// #include <MENU.h>   // Descomentar cuando se conecte botones de navegación físicamente

// =================================================================
//  OBJETOS GLOBALES
// =================================================================
RTC_DS1307 reloj;
DHT dhtSuperior(DHT_SUPERIOR_PIN, DHT11);
DHT dhtInferior(DHT_INFERIOR_PIN, DHT11);
DHT dhtPuerta(DHT_PUERTA_PIN, DHT11);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_ADDR);

// CH375 usbHost(Serial1, CH375_INT_PIN); // Descomentar cuando se conecte el CH375B

// Sistema de Menú Interactivo (Descomentar cuando se conecten botones de navegación)
// MenuSystem menu(&display);

// =================================================================
//  PROTOTIPOS DE FUNCIONES
// =================================================================
void testPerifericos();
void inicializarPines();
void displayTestMensaje(const String& texto, bool limpiar);

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

    displayTestMensaje("---TEST FINALIZADO---", true);
    delay(2000);

    // Limpiar pantalla para el programa principal
    display.clearDisplay();
    display.display();
}

// =================================================================
//  LOOP PRINCIPAL
// =================================================================

void loop() {
    // Loop principal - SIN DELAYS (totalmente no bloqueante)
    
    // TODO: Descomentar cuando se conecten botones de navegación
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