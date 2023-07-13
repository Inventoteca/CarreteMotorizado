/*
  CONTROLADOR DE BRAZO PARA COLGAR BICI
  Inventoteca, julio 2021

  El programa funciona en ESP32 o ESP8266
  Usa MQTT. Publica en el topic "brazo/status"
  Se suscribe al topic "brazo/control"

  Modificar para usar el ejemplo de reconexión sin bloquear

  Basic ESP8266 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.
  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

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
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE]; //buffer para mensaje de entrada ??
int value = 0; //valor que se envía

// Topics
char pubTopic[] = "brazo/status"; //aquí publica
char subTopic[] = "brazo/control"; //se suscribe

// Definir pines para los botones que controlan el mov del motor
#define BTN_UP   12
#define BTN_STOP 13
#define BTN_DOWN 14

// Definir pines de finales de carrera
#define LIMIT_UP   4
#define LIMIT_DOWN 5

// Definir pin de LED (opcional)
#define BUILTIN_LED 2

void setup_wifi() {
  // Modificar función para evitar bloqueo
  //delay(10);
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Modificar para evitar bloqueo
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  /*
    if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  */
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(pubTopic, "conectado");
      // ... and resubscribe
      client.subscribe(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // Configurar pines como entradas con pull-up
  // Botones
  pinMode(BTN_UP,   INPUT_PULLUP);
  pinMode(BTN_STOP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  // Finales de carrera
  pinMode(LIMIT_UP,   INPUT_PULLUP);
  pinMode(LIMIT_DOWN, INPUT_PULLUP);
  // LED opcional
  pinMode(BUILTIN_LED, OUTPUT);

  // Mensaje inicial
  Serial.begin(115200);
  Serial.println();
  Serial.println("CONTROLADOR DE BRAZO PARA COLGAR BICI");
  //Serial.println(ESP.getEfuseMac(), HEX);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%d", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
