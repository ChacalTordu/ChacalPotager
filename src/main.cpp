#include <WiFi.h>
#include <PubSubClient.h>
#include <iostream>
#include <string>  
using namespace std;

//definie those variables for your project
const char* ssid = "YOUR_SSID";
const char* password = "YOUR PSSWD";
const char* mqtt_server = "YOUR_MQTT_SERVER_ADRESS";
#define mqtt_port 1883 //1883 is the default port used for mqtt
#define MQTT_USER "YOUR_MQTT_USER"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"
#define MQTT_SERIAL_PUBLISH_CH "/Potager/"
//#define MQTT_SERIAL_RECEIVER_CH "" // FOR THE MOMENT WE DONT RECEIVE ANY DATA

// Définition of sensors MQTT topics path
#define MQTT_SERIAL_PUBLISH_SENSOR1 "/Potager/parcelle1/capteur-humidite1"
#define MQTT_SERIAL_PUBLISH_SENSOR2 "/Potager/parcelle1/capteur-humidite2"
#define MQTT_SERIAL_PUBLISH_SENSOR3 "/Potager/parcelle1/capteur-humidite3"
#define MQTT_SERIAL_PUBLISH_SENSOR4 "/Potager/parcelle1/capteur-humidite4"

//ENTER THE PINS USED BY YOUR HUMIDITY SENSORS
#define HUMIDITY_SENSOR1 36
#define HUMIDITY_SENSOR2 39
#define HUMIDITY_SENSOR3 34
#define HUMIDITY_SENSOR4 35

// initialiazing variables sensor values on 50
int humidity_sensor1=50;
int humidity_sensor2=50;
int humidity_sensor3=50;
int humidity_sensor4=50; 

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//----------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------FONCTIONS------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

//SETTING UP THE WIFI
void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connected to : ");
    Serial.println(ssid);
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

//RECONNECT TO THE WIFI IN CASE OF CONNEXION FAILED
void reconnect() {
  // Loop until we are connected
  while (!client.connected()) {
    Serial.print("Waiting for MQTT connexion ...");
    // Creating an ID Client
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    //Waiting for connexion
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish
      client.publish(MQTT_SERIAL_PUBLISH_CH, "hello mqtt");
      // ... et subscribe to read
      //client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("New try in 5 secondes ... ");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//Use for writing on the mqtt server
void callback(char* topic, byte *payload, unsigned int length) { 
    Serial.println("-------New message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void publishSerialData(char *serialData){
  if (!client.connected()) { // Connection in case of deco, to a new client
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}

//Fonction for publishing a float MQTT topic
void mqtt_publish(String topic, int t){
  char top[topic.length()+1];
  topic.toCharArray(top,topic.length()+1);
  char t_char[50];
  String t_str = String(t);
  t_str.toCharArray(t_char, t_str.length() + 1);
  client.publish(top,t_char);
}

//----------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------SETUP----------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void setup() {

  pinMode(HUMIDITY_SENSOR1, INPUT);
  pinMode(HUMIDITY_SENSOR2, INPUT);
  pinMode(HUMIDITY_SENSOR3, INPUT);
  pinMode(HUMIDITY_SENSOR4, INPUT);

  Serial.begin(9600);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

//----------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------LOOP-----------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void loop() {

  humidity_sensor1 = digitalRead(HUMIDITY_SENSOR1);
  humidity_sensor2 = digitalRead(HUMIDITY_SENSOR2); 
  humidity_sensor3 = digitalRead(HUMIDITY_SENSOR3);
  humidity_sensor1 = digitalRead(HUMIDITY_SENSOR4); 

  client.loop();

  if (Serial.available() > 0) {
    char mun[501];
    memset(mun,0, 501);
    Serial.readBytesUntil( '\n',mun,500);
    publishSerialData(mun);
  }

  // AFFICHAGE DES POSITIONS VOLETS ET ETATS LED
  Serial.println(humidity_sensor1);
  mqtt_publish(MQTT_SERIAL_PUBLISH_SENSOR1, humidity_sensor1);
  Serial.println(humidity_sensor2);
  mqtt_publish(MQTT_SERIAL_PUBLISH_SENSOR2, humidity_sensor2);
  Serial.println(humidity_sensor3);
  mqtt_publish(MQTT_SERIAL_PUBLISH_SENSOR3, humidity_sensor3);
  Serial.println(humidity_sensor4);
  mqtt_publish(MQTT_SERIAL_PUBLISH_SENSOR4, humidity_sensor4);
 }