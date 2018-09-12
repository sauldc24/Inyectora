#include "Arduino.h"
#include "TON.h"

TON::TON(bool* comando, bool* reset, unsigned long retardo){
  _punteroComando = comando;
  _punteroReset = reset;
  _retardo = retardo;
}

void TON::update(){
if (_estadoActual == false && *_punteroComando == true && (not *_punteroReset)){
  _tiempoTranscurrido = 0;
  _tiempoInicial = millis();
  _estadoActual = true;
}
if (_estadoActual && ! *_punteroReset){
  _tiempoTranscurrido = millis() - _tiempoInicial;
  if (_tiempoTranscurrido >= _retardo){
    _salida = true;
  }
}
else {
  _salida = false;
  _estadoActual = false;
}
}

bool TON::output(){
  return _salida;
}
