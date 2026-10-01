#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

// ... paramètres WiFi et connexion identiques à la partie 2 ...
const char* url = "https://api.beebotte.com/v1/data/read/Projet/ledrouge?limit=1";
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const int PIN_LED = 23;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  
  WiFi.begin(ssid, password);
  Serial.println("Partie_3");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connexion au WiFi...");
  }

  Serial.println("Connecté au WiFi");
}

void loop() {
    WiFiClientSecure client;
    client.setInsecure(); // TP uniquement (pas de vérification du certificat)
    HTTPClient http;
    // TODO 1 : http.begin(client, url) puis ajouter l'en-tête X-Auth-Token
    http.begin(client, url);
    http.addHeader("X-Auth-Token", token);
    // TODO 2 : envoyer la requête GET et récupérer le code de retour
    int httpCode = http.GET();
    Serial.println(httpCode);
    // TODO 3 : si le code vaut 200, lire la réponse avec http.getString()
    // TODO 4 : analyser le JSON (ArduinoJson) et extraire doc[0]["data"]
    if (httpCode == 200) {
      String reponse = http.getString();
      DynamicJsonDocument doc(1024);

      Serial.println(reponse);
      DeserializationError error = deserializeJson(doc, reponse);

      if (error) {
        Serial.println("Failed to parse JSON");
      } else {
        boolean etat = doc[0]["data"];
        // TODO 5 : appliquer l'état à la LED avec digitalWrite()
        digitalWrite(23, etat);
      }
    } else {
    Serial.println("Failed to connect to the server");
  }
  // TODO 6 : http.end() puis attendre 5 secondes
  http.end();
  delay(1000);
}