#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const int PIN_LED = 23;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connexion au WiFi...");
  }

  Serial.println("Connecté au WiFi");
}

void loop() {
  digitalWrite(PIN_LED, HIGH);
}