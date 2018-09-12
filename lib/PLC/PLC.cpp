#include "Arduino.h"
#include "PLC.h"
//#include "SimpleModbusSlave.h"
/*PLC::PLC(int *InputPorts, int* OutputPorts, int* Counters, int InputSize, int OutputSize, int CounterSize){
    _InputPorts = InputPorts;
    _OutputPorts = OutputPorts;
    _Counters = Counters;
    _InputSize = InputSize;
    _OutputSize = OutputSize;
    _CounterSize = CounterSize;
    PLC::configurePins();
  }*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PLC::PLC(byte* InputPorts, byte* OutputPorts, byte InputSize, byte OutputSize, ModbusIP* modbus, byte mode){
      _InputPorts = InputPorts;
      _OutputPorts = OutputPorts;
      _mode = mode;
      //_Counters = Counters;
      _InputSize = InputSize;
      _OutputSize = OutputSize;
      //_CounterSize = CounterSize;
      //_modbusID = modbusID;
      _modbus = modbus;
    }
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void PLC::output(byte salida, byte value){
    if (_mode == ACTIVEHIGH){
      digitalWrite(_OutputPorts[salida] , value);
    }
    else if (_mode == ACTIVELOW){
      digitalWrite(_OutputPorts[salida], !value);
    }
    //_OutputStatus[salida]=value;
    if (value){
      bitSet(_outputRegister, salida);
      _modbus->Coil(salida, true);
    }
    else {
      bitClear(_outputRegister, salida);
      _modbus->Coil(salida, false);
    }
    //_modbus->Hreg(remoteOutputRegisterAddress, _outputRegister);
  }
////////////////////////////////////////////////////////////
//Método para leer uno de los valores de la tabla de entradas
////////////////////////////////////////////////////////////
byte PLC::input(byte entrada){
  if (_modbus->Hreg(statusRegisterAddress) == 0xFF00)
    return bitRead(_modbus->Hreg(remoteInputRegisterAddress), entrada);
  else
    return bitRead(_inputRegister, entrada);
}
////////////////////////////////////////////////////////////////////////////////////
//Método para actualizar la tabla de entradas antes de ejecutar el código principal
///////////////////////////////////////////////////////////////////////////////////
void PLC::inputStatusUpdate(){
  for(int i=0; i<_InputSize; i++){
    //_InputStatus[i]=digitalRead(_InputPorts[i]);
    if (digitalRead(_InputPorts[i])){
      bitSet(_inputRegister, i);
      _modbus->Ists(i, true);
    }
    else {
      bitClear(_inputRegister, i);
      _modbus->Ists(i, false);
    }
  }
  //modbus_update(_IOStatusRegister);
}

/*void PLC::outputStatusUpdate(){
  if (_mode == ACTIVEHIGH){
      for(int i=0; i<_OutputSize; i++)
      digitalWrite(_OutputPorts[i] , bitRead(_outputRegister, i));
  }
  else if (_mode == ACTIVELOW){
    for(int i=0; i<_OutputSize; i++)
    digitalWrite(_OutputPorts[i] , !bitRead(_outputRegister, i));
  }
  _OutputStatus[salida]=value;
}*/
//////////////////////////////////////////////////////////////////////////////////////////
//método para iniciaclizar los puertos de entrada y salida de acuerdo al modo de operación
//////////////////////////////////////////////////////////////////////////////////////////
void PLC::begin(){
  _modbus->addHreg(statusRegisterAddress, 0);//se crea el registro de Status
  _modbus->addHreg(remoteInputRegisterAddress, 0);//se crea el registro de entradas remotas
  _modbus->addHreg(remoteOutputRegisterAddress, 0);//se crea el registro de salidas remotas
  for (int i=0; i < _InputSize; i++){
    pinMode(_InputPorts[i], INPUT);
    _modbus->addIsts(i, false);
  }
  for (int i=0; i < _OutputSize; i++){
    pinMode(_OutputPorts[i], OUTPUT);//se configura el puerto como salisa
    _modbus->addCoil(i,false);//se crea el registro tipo coil para la salida
    if (_mode == ACTIVEHIGH){//si está configurado como salida activa en alto
      digitalWrite(_OutputPorts[i] , LOW);//se configura la salida en bajo
    }
    else if (_mode == ACTIVELOW){//si está configurado como salida activa en bajo
      digitalWrite(_OutputPorts[i], HIGH);//se configura la salida en alto
    }
  }
}
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
unsigned int PLC::getInputRegister(){
  return _inputRegister;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PLC::setInputRegister(unsigned int inputRegister){
  _inputRegister = inputRegister;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned int PLC::getOutputRegister(){
  return _outputRegister;
}
//Método para contadores
//byte PLC::counter(byte)

/*void PLC::comunicate(){
  int contador_caracteres = 0;
  String instruccion="";
  char nuevo_caracter;
  while(Serial.available()){
    nuevo_caracter = Serial.read();
    contador_caracteres++;
    if((nuevo_caracter != '\n' || nuevo_caracter != '\r') || contador_caracteres < BUFFER_LENGTH){
      instruccion.concat(nuevo_caracter);
    }
    else if(instruccion == "InputStatus?"){
        for (int i=0; i < _InputSize; i++){
          Serial.print(_InputStatus[i]);
          Serial.print("\r\n");
        }
      }
    }
  }*/
