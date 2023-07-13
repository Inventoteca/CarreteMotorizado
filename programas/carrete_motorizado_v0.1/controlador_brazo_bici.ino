/*
  CONTROLADOR DE BRAZO PARA COLGAR BICI
  Inventoteca, julio 2021
 
  El programa funciona en ESP32 o ESP8266
  Usa MQTT. Publica en el topic "brazo/status"
  Se suscribe al topic "brazo/control"
 
 Reconnecting MQTT example - non-blocking

 This sketch demonstrates how to keep the client connected
 using a non-blocking reconnect function. If the client loses
 its connection, it attempts to reconnect every 5 seconds
 without blocking the main loop.

*/

//#include <SPI.h>
//#include <Ethernet.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Datos de la red
const char* ssid = "Inventoteca_2G";
const char* password = "science_7425";
const char* mqtt_server = "broker.mqtt-dashboard.com";

// Variables para MQTT
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0; //millis del último mensaje
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE]; //buffer para mensaje de entrada ??
//int value = 0; //valor que se envía
//EthernetClient ethClient;
//PubSubClient client(ethClient);

// Definir pines para los botones que controlan el mov del motor
#define BTN_UP   12
#define BTN_STOP 13
#define BTN_DOWN 14

// Definir pines de finales de carrera
#define LIMIT_UP   4
#define LIMIT_DOWN 5

// Update these with values suitable for your hardware/network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress server(172, 16, 0, 2);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

long lastReconnectAttempt = 0;

void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  // Imprime datos de conexión
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

boolean reconnect() {
  // Crear id único
  String clientId = String("ESP") + String((uint32_t)ESP.getEfuseMac(), HEX);
  
  Serial.print("Attempting MQTT connection...");
  if (client.connect(clientId.c_str())) {
    // Once connected, publish an announcement...
    Serial.println("connected");
    client.publish("brazo/status", "conectado");
    // ... and resubscribe
    client.subscribe("brazo/control");
  }
  return client.connected();
}

void setup()
{
  // Configurar pines como entradas con pull-up
  // Botones
  pinMode(BTN_UP,   INPUT_PULLUP);
  pinMode(BTN_STOP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  // Finales de carrera
  pinMode(LIMIT_UP,   INPUT_PULLUP);
  pinMode(LIMIT_DOWN, INPUT_PULLUP);

  // Mensaje inicial
  Serial.begin(115200);
  Serial.println();
  Serial.println("CONTROLADOR DE BRAZO PARA COLGAR BICI");
  Serial.println(ESP.getEfuseMac(), HEX);

  // Conectar a la red WiFi
  setup_wifi();

  // Establecer server
  client.setServer(server, 1883);

  // Función que se ejecuta cuando llega un mensaje
  client.setCallback(callback);

  //Ethernet.begin(mac, ip);
  //delay(1500);
  lastReconnectAttempt = 0;
}


void loop()
{
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected

    client.loop();
  }

  // Leer botones

}
