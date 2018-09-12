
/*TIMER ON DELAY LIBRARY
CREATED BY SAUL DIAZ*/
#ifndef TON_H
#define TON_H
#include "Arduino.h"
class TON{
public:
  /*constructor de la clase. requiere una entrada que inicia el retardo,
  una entrada de reset y el tiempo de retardo.*/
  TON(bool* comando, bool* reset, unsigned long tiempo);
  void update();
  /*método para modificar el tiempo de espera del timer*/
  //void setTime(unsigned long);
  /*método para obtener el status de la salida*/
  bool output();
private:
  bool* _punteroComando;
  bool* _punteroReset;
  bool _estadoActual = false;
  bool _salida = false;
  unsigned long _tiempoInicial;
  unsigned long _tiempoTranscurrido = 0;
  unsigned long _retardo = 0;
};
#endif
