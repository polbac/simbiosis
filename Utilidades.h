#include <Arduino.h>
#include <Adafruit_TinyUSB_MIDI.h>
#include <DallasTemperature.h>


const int NUM_PISTAS = 1;

class Pista {
  private:
    Adafruit_TinyUSB_MIDI midi;
    uint8_t channelMidi;
    String nombre;
    uint8_t volumeCN = 1;
    uint8_t volumeMin;
    uint8_t volumeMax;

    uint8_t reaPitch = 2;
    uint8_t reaVerb = 3;
    uint8_t reaVerbate = 4;
    uint8_t flanger = 5;
    uint8_t distortion = 6;
    uint8_t reaEq = 7;
    uint8_t ringModulator = 8;
    uint8_t eq = 9;

    uint8_t eqMin;
    uint8_t eqMax;
    

    uint8_t cnRandom;
    bool esEq = false;

  public:
    Pista(
      String _nombre,
      uint8_t _channelMidi,
      uint8_t _volumeMin,
      uint8_t _volumeMax,
      uint8_t _eqMin,
      uint8_t _eqMax
    ) {
      nombre = _nombre;
      channelMidi = _channelMidi;
      volumeMin = _volumeMin;
      volumeMax = _volumeMax;
      eqMin = _eqMin;
      eqMax = _eqMax;
    }
    
    void learn(uint8_t cc) {
      if(cc == 0) {
        midi.sendControlChange(volumeCN, 50, channelMidi);
        Serial.println("Learn a volume " + String(nombre));
      }

      if(cc == 1) {
        midi.sendControlChange(reaVerb, 50, channelMidi);
        Serial.println("Learn a reaVerb " +  String(nombre));
      }

      if(cc == 2) {
        midi.sendControlChange(reaVerbate, 50, channelMidi);
        Serial.println("Learn a rea verbate " +  String(nombre));
      }

      if(cc == 3) {
        midi.sendControlChange(flanger, 50, channelMidi);
        Serial.println("Learn a flanger " +  String(nombre));
      }

      if(cc == 4) {
        midi.sendControlChange(distortion, 50, channelMidi);
        Serial.println("Learn a distortion " +  String(nombre));
      }

      if(cc == 5) {
        midi.sendControlChange(ringModulator, 50, channelMidi);
        Serial.println("Learn a ringModulator " +  String(nombre));
      }

      if(cc == 6) {
        midi.sendControlChange(eq, 50, channelMidi);
        Serial.println("Learn a EQ " + nombre);
      }

      if(cc == 7) {
        midi.sendControlChange(reaPitch, 50, channelMidi);
        Serial.println("Learn a reaPitch " + nombre);
      }
    }

    void reiniciar() {
      midi.sendControlChange(volumeCN, 0, channelMidi);
      midi.sendControlChange(reaVerb, 0, channelMidi);
      midi.sendControlChange(reaVerbate, 0, channelMidi);
      midi.sendControlChange(flanger, 0, channelMidi);
      midi.sendControlChange(ringModulator, 0, channelMidi);
      midi.sendControlChange(distortion, 0, channelMidi);
      midi.sendControlChange(eq, 0, channelMidi);
      midi.sendControlChange(reaPitch, 0, channelMidi);
    }

    void seleccionarEfectoRandom() {
      long rnd = random(1, 8);
      esEq = false;

      if(rnd == 1) {
        cnRandom = reaVerb;
      }

      if(rnd == 2) {
        cnRandom = reaVerbate;
      }

      if(rnd == 3) {
        cnRandom = flanger;
      }

      if(rnd == 4) {
        cnRandom = ringModulator;
      }

      if(rnd == 5) {
        cnRandom = distortion;
      }

      if(rnd == 6) {
        cnRandom = eq;
        esEq = true;
      }

      if(rnd == 7) {
        cnRandom = reaPitch;
      }

    }


    void moverPotes(float valor) {
      midi.sendControlChange(volumeCN, int(valor * 100) / 127, channelMidi);
      midi.sendControlChange(cnRandom, int(valor * 100) / 127, channelMidi);
    }
  
    
};

class AdministradorPistas {
  private:
    Pista* pistas[1];
    int bloquesDisponible = 0;
    int pistasDisponibles = 0;
  
  public:
    void agregarPista(Pista* _pista) {
      pistas[bloquesDisponible] = _pista;

      bloquesDisponible += 1;
      pistasDisponibles += 1;
    }

    void devolverPista(Pista* pista) {
      
    }

    Pista obtenerPista() {

    }
};

class GrupoSensorDelta {
  private:
    String name;
    float sensores[5];
    float temperatura;
    float ultimaTemperatura;
    float porcentaje = 0;
    float VELOCIDAD = 0.1;
    
    Pista* pista;


  public:
    GrupoSensorDelta(String _name) {
      name = _name;
      pista = nullptr; 
    }

    void asignarPista(Pista* _pista) {
      pista = _pista;
      pista->reiniciar();
      pista->seleccionarEfectoRandom();
    }
  
    void loop(float _sensores[5]) {
      float temperaturaSensada = 0;

      for(byte i = 0; i < 5; i++) {
        if(_sensores[i] > temperaturaSensada){
          temperaturaSensada = _sensores[i];
        }
      }

      float incremento;

      if(temperaturaSensada > ultimaTemperatura){
        incremento = VELOCIDAD;
      } else {
        incremento = -VELOCIDAD;
      }

      porcentaje += incremento;

      if(porcentaje < 0){
        porcentaje = 0;
      }

      if(porcentaje > 100){
        porcentaje = 100;
      }

      pista->moverPotes(porcentaje);

      ultimaTemperatura = temperaturaSensada;
    }


    
};


// viejo
class GrupoSensor {
  private:
    String name;
    String trans; 
    int encendido;
    int cc;
    float sensores[5];
    float temperaturaMaxima = 1.0;
    float temperaturaMinima = 999.0;

    float valorFinal;
    Pista* pista;

    const unsigned long TIEMPO_RESET = 1000; // 30 minutos (*60)
    float minimoEnVentana = 999.0;
    float maximoEnVentana = 1.0;
    unsigned long horaUltimoReseteo = millis();

  public:
    GrupoSensor(String _name) {
      name = _name;
      pista = nullptr; 
    }

    void asignarPista(Pista* _pista) {
      pista = _pista;
      pista->reiniciar();
      pista->seleccionarEfectoRandom();
    }
  
    void loop(float _sensores[5]) {
      float valorMaximoTomado = 0;

      for(byte i = 0; i < 5; i++) {
        if(_sensores[i] > valorMaximoTomado){
          valorMaximoTomado = _sensores[i];
        }
      }

      valorFinal = valorMaximoTomado;

      calcularValoresMaximosMinimos(valorFinal);

      float porcentaje = obtenerPorcentaje();

      //Serial.println("Temp: " + String(valorFinal));
      Serial.println("Min: " + String(temperaturaMinima) + " / Max:" + String(temperaturaMaxima) + " / Porcentaje: " + String(porcentaje) + "(" + String(valorFinal) + ")");
      
      
      pista->moverPotes(porcentaje);
      
    }

    void calcularValoresMaximosMinimos(float v){
      // minimo
      if(v < minimoEnVentana){
        minimoEnVentana = v;
      }

      if(minimoEnVentana < temperaturaMinima){
        temperaturaMinima = minimoEnVentana;
      }

      // maximo
      if(v > maximoEnVentana){
        maximoEnVentana = v;
      }

      if(maximoEnVentana > temperaturaMaxima){
        temperaturaMaxima = maximoEnVentana;
      }

      unsigned long ahora = millis();
      unsigned long tiempoTranscurrido = ahora - horaUltimoReseteo;
      
      //if(tiempoTranscurrido > TIEMPO_RESET){
      //  Serial.println("RESET");
      //  temperaturaMaxima = maximoEnVentana;
      //  temperaturaMinima = minimoEnVentana;

        //minimoEnVentana = 999.99;
        //maximoEnVentana = 1.00;

        //horaUltimoReseteo = ahora;
      //}


    }

    float obtenerPorcentaje() {
      if (temperaturaMaxima-temperaturaMinima < 1.00){
        return 0;
      }

      return (valorFinal - temperaturaMinima) * 100.0 / (temperaturaMaxima - temperaturaMinima); 
    }
    
};
