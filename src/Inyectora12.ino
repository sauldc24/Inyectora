#include<PLC.h>
#include<LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>
#include <TON.h>
/*
TODOS LOS ELEMENTOS DEL PLC SE GUARDAN EN ARREGLOS QUE EL USUARIO DEBE DECLARAR
LOS ARREGLOS SIRVEN PARA CONSTRUIR UN OBJETO DE LA CLASE PLC
SE DEBE DECLARAR UN ARREGLO PARA CONFIGURAR LOS PUERTOS QUE SERVIRÁN DE SALIDAS
SE DEBE DECLARAR UN ARREGLO PARA CONFIGURAR LOS PUERTOS QUE SERVIRÁN DE ENTRADAS
SE DEBE DECLARAR UN ARREGLO PARA CONFIGURAR LOS CONTADORES
*/
//Se definen los nombres de las entrad as 0 a 5 DEL PLC
#define MICRO_PRENSA_IN 0
#define MICRO_PRENSA_OUT 1
#define MICRO_CARGA 2
#define MICRO_INYECCION 3
#define BOTON_INICIO 4
#define BOTON_AUTOMATICO 5
//Se definen los nombres de las salidas 0 A 5 DEL PLC
#define PRENSA_IN 0
#define PRENSA_OUT 1
#define INYECTAR 2
#define CARGAR 3
#define BOTADOR_IN 4
#define BOTADOR_OUT 5
//se definen los contadores y timers
#define registroBotadas 0
#define registroInyeccionesCompletas 1
#define registroTimerInyeccion 2
#define registroTimerCarga 3
#define registroTimerApertura 4
//

//definicion de arreglos
byte InputPorts[]={2,3,4,5,6,7};//pines de los puertos de entrada 1 al 6
byte OutputPorts[]={22,23,24,25,26,27};//pines de los puertos de salida 1 al 6
//bool myPLC.input(6]={};//arreglo para guardar el status de las entradas
//bool outputStatus[6]={};//arreglo para guardar el status de las salidas
//int contador_botador[3]={};
//Definición de banderas
int ciclo_botador_terminado = 0;
int inicio_semiautomatico = 0;
int inyeccion_iniciada = 0;
int carga_iniciada = 0;
int apertura_iniciada = 0;
//Definición de contadores y tiempos
int ciclos_botador = 2;
int tiempo_botador = 1000;
int tiempo_antes_botador = 1000;
int tiempo__antes_inyeccion = 1000;
int tiempo_despues_inyeccion = 1000;
int tiempo_antes_abrir = 1000;
ModbusIP mb;
PLC myPLC(InputPorts, OutputPorts, sizeof(InputPorts)/sizeof(byte), sizeof(OutputPorts)/sizeof(byte), &mb, ACTIVELOW);
///////////////////////////////////////////
//////////////////////////////////////////
void setup() {
// The media access control (ethernet hardware) address for the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// The IP address for the shield
byte ip[] = { 192, 168, 2, 120};
mb.config(mac, ip);
mb.addHreg(registroBotadas, 2);
mb.addHreg(registroInyeccionesCompletas, 0);
mb.addHreg(registroTimerInyeccion, 1000);
mb.addHreg(registroTimerCarga, 1000);
mb.addHreg(registroTimerApertura, 1000);
myPLC.begin();
}
///////////////////////////////////////////////////
//////////////////////////////////////////////////
void loop() {
mb.task();
/*if (mb.Hreg(statusRegisterAddress) == 0xFF00){
  myPLC.setInputRegister(mb.Hreg(inputRegisterAddress));
}
else{
myPLC.inputStatusUpdate();
mb.Hreg(inputRegisterAddress, myPLC.getInputRegister());
}*/
myPLC.inputStatusUpdate();
if (myPLC.input(BOTON_INICIO)){
  inicio_semiautomatico = 1;
}

if(myPLC.input(MICRO_PRENSA_OUT) && myPLC.input(MICRO_CARGA) && (myPLC.input(BOTON_AUTOMATICO) || inicio_semiautomatico) && ciclo_botador_terminado == 0){
  inyeccion_iniciada = 0;
  carga_iniciada = 0;
  apertura_iniciada = 0;
  for(unsigned int i = 0; i < mb.Hreg(registroBotadas); i++){
    myPLC.output(BOTADOR_OUT, LOW);
    myPLC.output(BOTADOR_IN, HIGH);
    delay(tiempo_botador);
    myPLC.output(BOTADOR_IN, LOW);
    myPLC.output(BOTADOR_OUT, HIGH);
    delay(tiempo_botador);
    ciclo_botador_terminado = 1;
    }
    myPLC.output(PRENSA_OUT, LOW);
    myPLC.output(PRENSA_IN, HIGH);
  }

//Rutina para asegurar apagado de bobina de inyección cuando se detecta el micro
if (myPLC.input(MICRO_INYECCION)){
  myPLC.output(INYECTAR, LOW);
}
//Rutina para asegurar apagado de bobina de carga cuando se detecta el micro
if(myPLC.input(MICRO_CARGA)){
  myPLC.output(CARGAR, LOW);
}
//estado de prensa cerrada manda a inyectar
if (myPLC.input(MICRO_PRENSA_IN) && myPLC.input(MICRO_CARGA) && (myPLC.input(BOTON_AUTOMATICO) || inicio_semiautomatico == 1) && inyeccion_iniciada == 0){
  inyeccion_iniciada = 1;
  delay(mb.Hreg(registroTimerInyeccion));
  myPLC.output(CARGAR, LOW);
  myPLC.output(INYECTAR, HIGH);
}

//estado de inyeccion finalizada manda a cargar
if(myPLC.input(MICRO_PRENSA_IN) && myPLC.input(MICRO_INYECCION) && (myPLC.input(BOTON_AUTOMATICO) || inicio_semiautomatico == 1) && carga_iniciada == 0){
  carga_iniciada = 1;
  delay(mb.Hreg(registroTimerCarga));
  myPLC.output(INYECTAR, LOW);
  myPLC.output(CARGAR, HIGH);
  mb.Hreg(registroInyeccionesCompletas, mb.Hreg(registroInyeccionesCompletas)+1);
  }
//estado de carga finalizada manda a abrir prensa
if (myPLC.input(MICRO_PRENSA_IN) && myPLC.input(MICRO_CARGA) && (myPLC.input(BOTON_AUTOMATICO) || inicio_semiautomatico == 1) && apertura_iniciada == 0 && carga_iniciada == 1){
  apertura_iniciada = 1;
  delay(mb.Hreg(registroTimerApertura));
  myPLC.output(PRENSA_IN, LOW);
  myPLC.output(PRENSA_OUT,HIGH);
  ciclo_botador_terminado = 0;
  inicio_semiautomatico = 0;
  }
  //mb.Hreg(outputRegisterAddress, myPLC.getOutputRegister());
}
