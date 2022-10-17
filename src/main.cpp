#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <iostream>
#include <string>  
using namespace std;
// Définition LED de controle des volets 
#define LED_OUTPUT_SHUTTER_INIT_A 23 // = position initialisée
#define LED_OUTPUT_SHUTTER_INIT_B 22
#define LED_OUTPUT_SHUTTER_INIT_C 21
#define LED_OUTPUT_SHUTTER_INIT_D 19

//Définition des pins sur l'esp32 pour les Lampes
#define LED_A 15
#define LED_B 2
#define LED_C 4
#define LED_D 5

//Définition des pins sur l'esp 32 pour les volets montée et descente
#define PIN_INPUT_SHUTTER_UP_A 34
#define PIN_INPUT_SHUTTER_DOWN_A 35
#define PIN_INPUT_SHUTTER_UP_B 32
#define PIN_INPUT_SHUTTER_DOWN_B 33
#define PIN_INPUT_SHUTTER_UP_C 25
#define PIN_INPUT_SHUTTER_DOWN_C 26
#define PIN_INPUT_SHUTTER_UP_D 27
#define PIN_INPUT_SHUTTER_DOWN_D 14

// Mettez à jour ces constantes pour votre projet
const char* ssid = "Temporaire_2.4G";
const char* password = "domotique";
const char* mqtt_server = "mqtt.4x.re";
#define mqtt_port 1883
#define MQTT_USER "itea"
#define MQTT_PASSWORD "*domoserv"
#define MQTT_SERIAL_PUBLISH_CH "/PROJETTER/maquetteknx/vr/position"
#define MQTT_SERIAL_RECEIVER_CH "/PROJETTER/maquetteknx/"
// Définition des chemins des topics MQTT
#define MQTT_SERIAL_PUBLISH_VR_A_POSITION "/PROJETTER/maquetteknx/etage0/chambre/vr_A/position"
#define MQTT_SERIAL_PUBLISH_VR_B_POSITION "/PROJETTER/maquetteknx/etage0/bureau/vr_B/position"
#define MQTT_SERIAL_PUBLISH_VR_C_POSITION "/PROJETTER/maquetteknx/etage1/salon/vr_C/position"
#define MQTT_SERIAL_PUBLISH_VR_D_POSITION "/PROJETTER/maquetteknx/etage1/bibliotheque/vr_D/position"
#define MQTT_SERIAL_PUBLISH_LED_A_ALLUMAGE "/PROJETTER/maquetteknx/etage0/chambre/LED_A/etat"
#define MQTT_SERIAL_PUBLISH_LED_B_ALLUMAGE "/PROJETTER/maquetteknx/etage0/bureau/LED_B/etat"
#define MQTT_SERIAL_PUBLISH_LED_C_ALLUMAGE "/PROJETTER/maquetteknx/etage1/salon/LED_C/etat"
#define MQTT_SERIAL_PUBLISH_LED_D_ALLUMAGE "/PROJETTER/maquetteknx/etage1/bibliotheque/LED_D/etat"

// initialisation des variables position des volets
int shutter_positionA=50;// ON DECIDE QUE L'ETAT 0 C'EST L'ETAT BAS et volets initialisés à 50 %
int shutter_positionB=50;
int shutter_positionC=50;
int shutter_positionD=50; 
// initialisation des variables d'allumage
int led_allumageA;
int led_allumageB;
int led_allumageC;
int led_allumageD;
//Flag pour indiquer si le volet est bien initialisé
bool flag_allumageA=false;
bool flag_allumageB=false;
bool flag_allumageC=false;
bool flag_allumageD=false;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//----------------------------------------------------------------------------------------------------------------------
//--------------------------------------------DECLARATION DES FONCTIONS-------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecté à : ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connecté");
    Serial.println("IP addresse: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop jusqu'a ce soit connecté
  while (!client.connected()) {
    Serial.print("Attente d'une connection MQTT...");
    // Creer un ID Client
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attente de connexion
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Une fois connecté, publit une annonce
      client.publish("/PROJETTER/LEBRUN-GALLEY/icircuit/presence/ESP32/", "hello world");
      // ... et s'abonne pour lecture
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Nouvel essai dans 5 secondes ... ");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {  //Sert à écrire dans le serveur MQTT
    Serial.println("-------Nouveau message depuis le broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void publishSerialData(char *serialData){
  if (!client.connected()) { // Connection d'un nouveau client en cas de déconnection
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}

//Fonction pour publier un float sur un topic MQTT
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

  pinMode(LED_OUTPUT_SHUTTER_INIT_A, OUTPUT);
  pinMode(LED_OUTPUT_SHUTTER_INIT_B, OUTPUT);
  pinMode(LED_OUTPUT_SHUTTER_INIT_C, OUTPUT);
  pinMode(LED_OUTPUT_SHUTTER_INIT_D, OUTPUT);

  pinMode(PIN_INPUT_SHUTTER_UP_A, INPUT);
  pinMode(PIN_INPUT_SHUTTER_DOWN_A, INPUT);
  pinMode(PIN_INPUT_SHUTTER_UP_B, INPUT);
  pinMode(PIN_INPUT_SHUTTER_DOWN_B, INPUT);
  pinMode(PIN_INPUT_SHUTTER_UP_C, INPUT);
  pinMode(PIN_INPUT_SHUTTER_DOWN_C, INPUT);
  pinMode(PIN_INPUT_SHUTTER_UP_D, INPUT);
  pinMode(PIN_INPUT_SHUTTER_DOWN_D, INPUT);

  Serial.begin(9600);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

//----------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------LOOP-----------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void loop() {
  //**************************************************************************************************
  //******************************************** VOLET A *********************************************
  //**************************************************************************************************  
  int SHUTTER_STATE_UP_A = digitalRead(PIN_INPUT_SHUTTER_UP_A); // BOUTON KNX MONTEE - LECTURE BOUTON
  int SHUTTER_STATE_DOWN_A = digitalRead(PIN_INPUT_SHUTTER_DOWN_A); // BOUTON KNX DESCENTE - LECTURE BOUTON
  
   client.loop();//attente de paquets

//si la LED est flag est allumée, le volet est initialisé
  if ((shutter_positionA == 0)&&(flag_allumageA==false))
  {
    digitalWrite(LED_OUTPUT_SHUTTER_INIT_A,HIGH);
    flag_allumageA = true;
  }
// FIN DEMANDE

   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
   }

  //------------------------------------------------------ON CHECK LES BOUTONS-------------------------------------------------
  if (SHUTTER_STATE_UP_A == HIGH) // QUAND LE VOLET EST EN TRAIN DE MONTER
  {
    shutter_positionA++;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 1");
    // -- Pour les tests --digitalWrite(LED_OUTPUT_SHUTTER_UP_A,HIGH);
    if (shutter_positionA>=100)
      {
        shutter_positionA=100;
      }
    
  }else
  {
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,LOW);
  }
  
  if (SHUTTER_STATE_DOWN_A == HIGH) // QUAND LE VOLET EST EN TRAIN DE DESCENDRE
  {
    shutter_positionA--;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,HIGH);
      if (shutter_positionA<=0)
      {
        shutter_positionA=0;
      }
  }else
  {
   // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,LOW);
  }

  //**************************************************************************************************
  //******************************************** VOLET B *********************************************
  //**************************************************************************************************                 
  int SHUTTER_STATE_UP_B = digitalRead(PIN_INPUT_SHUTTER_UP_B); // BOUTON KNX MONTEE - LECTURE BOUTON
  int SHUTTER_STATE_DOWN_B = digitalRead(PIN_INPUT_SHUTTER_DOWN_B); // BOUTON KNX DESCENTE - LECTURE BOUTON
  
   client.loop();//attente de paquets

//DEMANDE A LUSER DE VOIR LA LED ALLUMER POUR UNE COORDINATION ENTRE LA MAQUETTE ET HOMEASSISTANT
  if ((shutter_positionB == 0)&&(flag_allumageB==false))
  {
    digitalWrite(LED_OUTPUT_SHUTTER_INIT_B,HIGH);
    flag_allumageB = true;
  }
// FIN DEMANDE

   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
   }

  //------------------------------------------------------ON CHECK LES BOUTONS-------------------------------------------------
  if (SHUTTER_STATE_UP_B == HIGH) // QUAND LE VOLET EST EN TRAIN DE MONTER
  {
    shutter_positionB++;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,HIGH);
    if (shutter_positionB>=100)
      {
        shutter_positionB=100;
      }
    
  }else
  {
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,LOW);
  }
  
  if (SHUTTER_STATE_DOWN_B == HIGH) // QUAND LE VOLET EST EN TRAIN DE DESCENDRE
  {
    shutter_positionB--;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,HIGH);
      if (shutter_positionB<=0)
      {
        shutter_positionB=0;
      }
  }else
  {
   // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,LOW);
  }
  //**************************************************************************************************
  //******************************************** VOLET C *********************************************
  //**************************************************************************************************                 
  int SHUTTER_STATE_UP_C = digitalRead(PIN_INPUT_SHUTTER_UP_C); // BOUTON KNX MONTEE - LECTURE BOUTON
  int SHUTTER_STATE_DOWN_C = digitalRead(PIN_INPUT_SHUTTER_DOWN_C); // BOUTON KNX DESCENTE - LECTURE BOUTON
  
   client.loop();//attente de paquets

//DEMANDE A LUSER DE VOIR LA LED ALLUMER POUR UNE COORDINATION ENTRE LA MAQUETTE ET HOMEASSISTANT
  if ((shutter_positionC == 0)&&(flag_allumageC==false))
  {
    digitalWrite(LED_OUTPUT_SHUTTER_INIT_C,HIGH);
    flag_allumageC = true;
  }
// FIN DEMANDE

   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
   }

  //------------------------------------------------------ON CHECK LES BOUTONS-------------------------------------------------
  if (SHUTTER_STATE_UP_C == HIGH) // QUAND LE VOLET EST EN TRAIN DE MONTER
  {
    shutter_positionC++;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,HIGH);
    if (shutter_positionC>=100)
      {
        shutter_positionC=100;
      }
    
  }else
  {
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,LOW);
  }
  
  if (SHUTTER_STATE_DOWN_C == HIGH) // QUAND LE VOLET EST EN TRAIN DE DESCENDRE
  {
    shutter_positionC--;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,HIGH);
      if (shutter_positionC<=0)
      {
        shutter_positionC=0;
      }
  }else
  {
   // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,LOW);
  }
  //**************************************************************************************************
  //******************************************** VOLET D *********************************************
  //**************************************************************************************************                 
  int SHUTTER_STATE_UP_D = digitalRead(PIN_INPUT_SHUTTER_UP_D); // BOUTON KNX MONTEE - LECTURE BOUTON
  int SHUTTER_STATE_DOWN_D = digitalRead(PIN_INPUT_SHUTTER_DOWN_D); // BOUTON KNX DESCENTE - LECTURE BOUTON
  
   client.loop();//attente de paquets

//DEMANDE A L'USER DE VOIR LA LED ALLUMER POUR UNE COORDINATION ENTRE LA MAQUETTE ET HOMEASSISTANT
  if ((shutter_positionD == 0)&&(flag_allumageD==false))
  {
    digitalWrite(LED_OUTPUT_SHUTTER_INIT_D,HIGH);
    flag_allumageD = true;
  }
// FIN DEMANDE

   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
   }

  //------------------------------------------------------ON CHECK LES BOUTONS-------------------------------------------------
  if (SHUTTER_STATE_UP_D == HIGH) // QUAND LE VOLET EST EN TRAIN DE MONTER
  {
    shutter_positionD++;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,HIGH);
    if (shutter_positionD>=100)
      {
        shutter_positionD=100;
      }
    
  }else
  {
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-montee", "VR_MONTEE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_UP,LOW);
  }
  
  if (SHUTTER_STATE_DOWN_D == HIGH) // QUAND LE VOLET EST EN TRAIN DE DESCENDRE
  {
    shutter_positionD--;
    // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 1");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,HIGH);
      if (shutter_positionD<=0)
      {
        shutter_positionD=0;
      }
  }else
  {
   // -- POUR LES TESTS -- client.publish("/PROJETTER/LEBRUN-GALLEY/test/vr-descente", "VR_DESCENTE = 0");
    // -- POUR LES TESTS --digitalWrite(LED_OUTPUT_SHUTTER_DOWN,LOW);
  }

  //**************************************************************************************************
  //******************************************** LAMPE A *********************************************
  //**************************************************************************************************
  int LED_ON_A = digitalRead(LED_A); // BOUTON KNX LED ON - LECTURE BOUTON
    
   client.loop();//attente de paquets

  if ((LED_ON_A == HIGH))
  {
    led_allumageA = 1;
  }
  else led_allumageA = 0;

  //**************************************************************************************************
  //******************************************** LAMPE B *********************************************
  //**************************************************************************************************
  int LED_ON_B = digitalRead(LED_B); // BOUTON KNX LED ON - LECTURE BOUTON
    
   client.loop();//attente de paquets

  if ((LED_ON_B == HIGH))
  {
    led_allumageB = 1;
  }
  else led_allumageB = 0;

  //**************************************************************************************************
  //******************************************** LAMPE C *********************************************
  //**************************************************************************************************
  int LED_ON_C = digitalRead(LED_C); // BOUTON KNX LED ON - LECTURE BOUTON
    
   client.loop();//attente de paquets

  if ((LED_ON_C == HIGH))
  {
    led_allumageC = 1;
  }
  else led_allumageC = 0;

  //**************************************************************************************************
  //******************************************** LAMPE D *********************************************
  //**************************************************************************************************
  int LED_ON_D = digitalRead(LED_D); // BOUTON KNX LED ON - LECTURE BOUTON
    
   client.loop();//attente de paquets

  if ((LED_ON_D == HIGH))
  {
    led_allumageD = 1;
  }
  else led_allumageD = 0;

  //---------------------------------------------------FIN ON CHECK LES BOUTONS-------------------------------------------------
  // ---------------------------------------------------------------------------------------------------------------------------
  // AFFICHAGE DES POSITIONS VOLETS ET ETATS LED
  Serial.println(shutter_positionA);
  mqtt_publish(MQTT_SERIAL_PUBLISH_VR_A_POSITION, shutter_positionA);
  Serial.println(shutter_positionB);
  mqtt_publish(MQTT_SERIAL_PUBLISH_VR_B_POSITION, shutter_positionB);
  Serial.println(shutter_positionC);
  mqtt_publish(MQTT_SERIAL_PUBLISH_VR_C_POSITION, shutter_positionC);
  Serial.println(shutter_positionD);
  mqtt_publish(MQTT_SERIAL_PUBLISH_VR_D_POSITION, shutter_positionD);
  Serial.println(led_allumageA);
  mqtt_publish(MQTT_SERIAL_PUBLISH_LED_A_ALLUMAGE, led_allumageA);
  Serial.println(led_allumageB);
  mqtt_publish(MQTT_SERIAL_PUBLISH_LED_B_ALLUMAGE, led_allumageB);
  Serial.println(led_allumageC);
  mqtt_publish(MQTT_SERIAL_PUBLISH_LED_C_ALLUMAGE, led_allumageC);
  Serial.println(led_allumageD);
  mqtt_publish(MQTT_SERIAL_PUBLISH_LED_D_ALLUMAGE, led_allumageD);
  
  delay(110);
 }