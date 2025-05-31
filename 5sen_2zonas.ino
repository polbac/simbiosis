#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_A 16
#define ONE_WIRE_BUS_B 15

OneWire oneWireA(ONE_WIRE_BUS_A);
OneWire oneWireB(ONE_WIRE_BUS_B);

DallasTemperature sensorsA(&oneWireA);
DallasTemperature sensorsB(&oneWireB);

DeviceAddress sensorA[5];
DeviceAddress sensorB[5];

void setup() {
  Serial.begin(115200);
  sensorsA.begin();
  sensorsB.begin();

  sensorsA.setResolution(12);
  sensorsB.setResolution(12);

  sensorsA.setWaitForConversion(false);
  sensorsB.setWaitForConversion(false);

  // Detectar sensores grupo A
  for (int i = 0; i < 5; i++) {
    if (!sensorsA.getAddress(sensorA[i], i)) {
      Serial.print("Sensor A");
      Serial.print(i + 1);
      Serial.println(" no encontrado");
    } else {
      sensorsA.setResolution(sensorA[i], 12);
    }
  }

  // Detectar sensores grupo B
  for (int i = 0; i < 5; i++) {
    if (!sensorsB.getAddress(sensorB[i], i)) {
      Serial.print("Sensor B");
      Serial.print(i + 1);
      Serial.println(" no encontrado");
    } else {
      sensorsB.setResolution(sensorB[i], 12);
    }
  }
}

void loop() {
  unsigned long startTotal = millis();

  // 1. Enviar orden de conversión a ambos buses
  sensorsA.requestTemperatures();
  sensorsB.requestTemperatures();

  // 2. Esperar manualmente (750 ms para 12 bits)
  //delay(600);

  // 3. Leer temperaturas
  float tempsA[5];
  float tempsB[5];
  unsigned long timesA[5];
  unsigned long timesB[5];

  for (int i = 0; i < 5; i++) {
    unsigned long t = millis();
    tempsA[i] = sensorsA.getTempC(sensorA[i]);
    timesA[i] = millis() - t;
  }

  for (int i = 0; i < 5; i++) {
    unsigned long t = millis();
    tempsB[i] = sensorsB.getTempC(sensorB[i]);
    timesB[i] = millis() - t;
  }

  unsigned long totalDuration = millis() - startTotal;

  // Grupo A
  Serial.print("Grupo A - ");
  for (int i = 0; i < 5; i++) {
    Serial.print("T");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(tempsA[i]);
    Serial.print(" °C");
    if (i < 4) Serial.print(", ");
  }
  Serial.println();

  Serial.print("Tiempos A: ");
  for (int i = 0; i < 5; i++) {
    Serial.print("T");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(timesA[i]);
    Serial.print(" ms");
    if (i < 4) Serial.print(", ");
  }
  Serial.println();

  // Grupo B
  Serial.print("Grupo B - ");
  for (int i = 0; i < 5; i++) {
    Serial.print("T");
    Serial.print(i + 6);
    Serial.print(": ");
    Serial.print(tempsB[i]);
    Serial.print(" °C");
    if (i < 4) Serial.print(", ");
  }
  Serial.println();

  Serial.print("Tiempos B: ");
  for (int i = 0; i < 5; i++) {
    Serial.print("T");
    Serial.print(i + 6);
    Serial.print(": ");
    Serial.print(timesB[i]);
    Serial.print(" ms");
    if (i < 4) Serial.print(", ");
  }
  Serial.print(" || Total: ");
  Serial.print(totalDuration);
  Serial.println(" ms");

  Serial.println();
}
