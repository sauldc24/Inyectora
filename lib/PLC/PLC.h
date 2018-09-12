#ifndef PLC_h
#define PLC_H
#include "Arduino.h"
#include <Modbus.h>
#include <ModbusIP.h>
//definicion de direcciones de registros
#define statusRegisterAddress 100//se necesita escribir un valor de 0xFF00 para controlar las entradas de manera remota
#define remoteInputRegisterAddress 101
#define remoteOutputRegisterAddress 102
////////////////////////////////////////////////
class PLC{
public:
  #define ACTIVEHIGH 1
  #define ACTIVELOW 0
  #define BUFFER_LENGTH 50
  //PLC(int*, int*, int* , int , int , int);
  PLC(byte*, byte*, byte , byte, ModbusIP*, byte);
  void output(byte, byte);
  byte input(byte);
  void inputStatusUpdate();
  void begin();
  unsigned int getInputRegister();
  void setInputRegister(unsigned int);
  unsigned int getOutputRegister();
  void comunicate();
  byte counter(byte);
private:
  byte* _InputPorts;
  //byte _InputStatus[10];//se utiliza 10 para separar memoria suficiente para un máximo de 10 entradas
  unsigned int _inputRegister = 0;
  byte* _OutputPorts;
  //byte _OutputStatus[10];//se utiliza 10 para separar memoria suficiente para un máximo de 10 salida
  unsigned int _outputRegister = 0;
  byte _mode;
  byte _Counters[10];//se utiliza 10 para separar memoria suficiente para un máximo de 10 salidas
  byte _InputSize;
  byte _OutputSize;
  byte _CounterSize;
  byte _modbusID;
  ModbusIP* _modbus;
};
#endif
