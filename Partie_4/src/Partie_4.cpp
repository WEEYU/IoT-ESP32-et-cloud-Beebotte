#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"   // const char* token = "token_...";  (non versionné)

const char* ssid     = "Wokwi-GUEST";
const char* password = "";
const int   PIN_LED  = 23;
const int   PIN_BTN  = 18;   // extension : bouton poussoir vers GND

const char* mqttServer = "mqtt.beebotte.com";
const int   mqttPort   = 1883;
const char* topic      = "Projet/ledrouge";   // P majuscule : Beebotte est sensible à la casse

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

bool ledState = false;

void callback(char* t, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur ");
  Serial.print(t);
  Serial.print(" : ");
  Serial.write(payload, length);
  Serial.println();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print("Erreur JSON : ");
    Serial.println(error.c_str());
    return;
  }

  ledState = doc["data"];
  digitalWrite(PIN_LED, ledState ? HIGH : LOW);
  Serial.print("LED = ");
  Serial.println(ledState ? "ON" : "OFF");
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Connexion MQTT...");
    String clientId = "esp32-led-" + String(random(0xffff), HEX);  // unique

    // Token récent (token_...) utilisé directement, sans mot de passe
    if (mqttClient.connect(clientId.c_str(), token, NULL)) {
      Serial.println(" connecté au broker");
      if (mqttClient.subscribe(topic)) {
        Serial.print("Abonné à ");
        Serial.println(topic);
      } else {
        Serial.println("Abonnement refusé");
      }
    } else {
      Serial.print(" échec, state = ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  Serial.println("Partie_4 v3");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("Connecté au WiFi");

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
  mqttClient.setSocketTimeout(15);   // le broker met parfois du temps à répondre
  mqttClient.setKeepAlive(30);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Extension : le bouton inverse l'état en publiant sur le topic
  static bool lastBtn = HIGH;
  bool btn = digitalRead(PIN_BTN);
  if (lastBtn == HIGH && btn == LOW) {
    String msg = String("{\"data\":") + (ledState ? "false" : "true") + ",\"write\":true}";
    mqttClient.publish(topic, msg.c_str());
    Serial.print("Publié : ");
    Serial.println(msg);
    delay(200);   // anti-rebond
  }
  lastBtn = btn;
}