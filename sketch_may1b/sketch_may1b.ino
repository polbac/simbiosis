#include <OneWire.h>
#include <DallasTemperature.h>
#include "Adafruit_TinyUSB.h"
#include <Adafruit_TinyUSB_MIDI.h>


Adafruit_TinyUSB_MIDI MIDI;

// Pines para cada zona
#define ZONA1_PIN 16  // 3 sensores
#define ZONA2_PIN 15  // 2 sensores

// OneWire y Dallas para cada zona
OneWire oneWire1(ZONA1_PIN);
DallasTemperature sensors1(&oneWire1);


OneWire oneWire2(ZONA2_PIN);
DallasTemperature sensors2(&oneWire2);

// Direcciones de sensores
DeviceAddress sensor1_addrs[3];
DeviceAddress sensor2_addrs[2];

void printAddress(DeviceAddress addr) {
  for (uint8_t i = 0; i < 8; i++) {
    if (addr[i] < 16) Serial.print("0");
    Serial.print(addr[i], HEX);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  MIDI.begin();

  Serial.println("midi inicializado");
  sensors1.setResolution(9);
  sensors1.begin();
  sensors2.setResolution(9);
  sensors2.begin();

  Serial.println("Buscando sensores...");

  // ZONA 1
  if (sensors1.getDeviceCount() < 3) {
    Serial.println("Error: Zona 1 necesita 3 sensores.");
    while (true);
  }

  for (int i = 0; i < 3; i++) {
    if (!sensors1.getAddress(sensor1_addrs[i], i)) {
      Serial.print("No se pudo obtener dirección del sensor ");
      Serial.print(i);
      Serial.println(" en zona 1.");
      while (true);
    }
  }

  // ZONA 2
  if (sensors2.getDeviceCount() < 2) {
    Serial.println("Error: Zona 2 necesita 2 sensores.");
    while (true);
  }

  for (int i = 0; i < 2; i++) {
    if (!sensors2.getAddress(sensor2_addrs[i], i)) {
      Serial.print("No se pudo obtener dirección del sensor ");
      Serial.print(i);
      Serial.println(" en zona 2.");
      while (true);
    }
  }

  Serial.println("Sensores detectados correctamente.");
}

void loop() {
  unsigned long t0 = millis();
  Serial.println("=== Nueva lectura ===");

  // Iniciar todas las conversiones al mismo tiempo
  sensors1.requestTemperatures();
  sensors2.requestTemperatures();

  // ZONA 1
  for (int i = 0; i < 3; i++) {
    unsigned long start = millis();
    float temp = sensors1.getTempC(sensor1_addrs[i]);
    unsigned long end = millis();

    Serial.print("Zona 1 - Sensor ");
    Serial.print(i);
    Serial.print(" [");
    printAddress(sensor1_addrs[i]);
    Serial.print("] @ ");
    Serial.print(end - start);
    Serial.print(" ms: ");
    Serial.print(temp);
    Serial.println(" ºC");
  }

  // ZONA 2
  for (int i = 0; i < 2; i++) {
    unsigned long start = millis();
    float temp = sensors2.getTempC(sensor2_addrs[i]);
    unsigned long end = millis();

    Serial.print("Zona 2 - Sensor ");
    Serial.print(i);
    Serial.print(" [");
    printAddress(sensor2_addrs[i]);
    Serial.print("] @ ");
    Serial.print(end - start);
    Serial.print(" ms: ");
    Serial.print(temp);
    Serial.println(" ºC");
  }

  unsigned long totalTime = millis() - t0;
  Serial.print("⏱ Tiempo total de lectura: ");
  Serial.print(totalTime);
  Serial.println(" ms\n");

   // Send Control Change messages
  for (int i = 1; i < 128; i++) {
    MIDI.sendControlChange(10, i, 10);  // cc number, cc value, channel
    MIDI.sendNoteOn(i, 127, 1);  // note, velocity, channel
    
  }

  Serial.print("envio midi ok");


  delay(2000);  // Espera entre lecturas
}
