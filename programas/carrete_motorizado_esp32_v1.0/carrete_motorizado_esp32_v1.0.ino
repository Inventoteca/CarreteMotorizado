/* CARRETE MOTORIZADO
  Controlador del carrete motorizado
  Usa bluetooth y control de Nintendo.
  https://github.com/dmadison/NintendoExtensionCtrl
  Basado en ejemplo SerialToSerialBT (ESP32)
*/

// Definir pines
#define REL1 26 //relevador 1 (controla motor)
#define REL2 27 //relevador 2 (controla motor)
#define SW1 16 //switch 1 (detectar límite de movimiento ARRIBA)
#define SW2 17 //switch 2 (detectar límite de movimiento ABAJO opcional)

// Librería bluetooh
#include "BluetoothSerial.h"
//puede ser módulo bluetooth externo
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

// Comandos para subir y bajar motor
const char SUBIR = 'S';
const char BAJAR = 'B';
const char PARAR = 'P';
//También son estados
char estado = PARAR;

// Librería control de Nintendo
#include <NintendoExtensionCtrl.h>
ClassicController nes; //objeto

// Temporizador
const int period = 1000;
unsigned long time_now = 0;
bool espera = false;

void setup() {
  // Configurar pines
  pinMode(REL1, OUTPUT);
  pinMode(REL2, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  parar(); //relevadores no activados

  // Iniciar comunicación serial y BT
  Serial.begin(115200);
  SerialBT.begin("Carrete"); //Bluetooth device name
  Serial.println("Bluetooth activado");

  // Iniciar comunicación con el control de NES
  nes.begin(); //iniciar comunicación con control
  while (!nes.connect()) {
    Serial.println("Receptor de control no detectado");
    delay(1000);
  }
}

void loop() {
  char com1 = 'x'; //comando BT
  char com2 = 'x'; //comando NES

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  // Leer comando BT
  if (SerialBT.available()) {
    com1 = SerialBT.read();
    Serial.print("Bluetooth ");
    Serial.println(com1);
  }

  // Leer comando control NES
  if (nes.update()) {

    if (nes.dpadUp()) {
      com2 = 'S'; //subir
      Serial.print("NES ");
      Serial.println(com2);
    }
    else if (nes.dpadDown()) {
      com2 = 'B'; //bajar
      Serial.print("NES ");
      Serial.println(com2);
    }
    else if (nes.buttonA() or nes.buttonB()) {
      com2 = 'P'; //parar
      Serial.print("NES ");
      Serial.println(com2);
    }
  }

  if (estado == PARAR) {
    parar();
    if (espera) { //
      if ((unsigned long)(millis() - time_now) > period) {
        //time_now = millis();
        espera = false;
        //Serial.println("Hello");
      }
    }
    else {
      if (com1 == 'S' or com2 == 'S') estado = SUBIR;
      else if (com1 == 'B' or com2 == 'B') estado = BAJAR;
    }
  }
  else if (estado == SUBIR) {
    subir();
    if (com1 == 'P' or com2 == 'P' or com1 == 'B' or com2 == 'B' or digitalRead(SW1) == 0) {
      estado = PARAR;
      time_now = millis();
      espera = true;
    }
  }
  else if (estado == BAJAR) {
    bajar();
    if (com1 == 'P' or com2 == 'P' or com1 == 'S' or com2 == 'S') {
      estado = PARAR;
      time_now = millis();
      espera = true;
    }
  }
}

void parar() {
  digitalWrite(REL1, HIGH);
  digitalWrite(REL2, HIGH);
}

void subir() {
  digitalWrite(REL1, HIGH);
  digitalWrite(REL2, LOW);
}

void bajar() {
  digitalWrite(REL1, LOW);
  digitalWrite(REL2, HIGH);
}

// Pines disponibles en la placa hexagonal (GPIO)
/*
  #define SERVO_L_5V  26 //pin 11
  #define SERVO_L_3V3 27 //pin 12
  #define SERVO_R_5V  17 //pin 28
  #define SERVO_R_3V3 16 //pin 27

  #define LED 23 //pin 37

  #define SDA 21 //pin 33
  #define SCL 22 //pin 36

  #define EXT1 15 //pin 23, sin función predefinida
  #define EXT2 2 //pin 24, sin función predefinida

  #define MOT_L1 12 //pin 14
  #define MOT_L2 13 //pin 16
  #define MOT_R1 18 //pin 30
  #define MOT_R2 19 //pin 31
*/
// Pines no configurables
//botón RST en pin 3 (EN)
//botón PROG en pin 25 (GPIO0)
//RX pin 35 (GPIO1)
//TX pin 34 (GPIO3)
