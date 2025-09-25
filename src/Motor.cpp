#include "Motor.h"
#include <Arduino.h>

void motorInit() {
     Serial.println("coucou");
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // moteur éteint au départ
}

void motorOn() {
    Serial.println("HIgh");
    digitalWrite(RELAY_PIN, HIGH); // active le relais, moteur tourne
}

void motorOff() {
    Serial.println("low");
    digitalWrite(RELAY_PIN, LOW);  // désactive le relais, moteur s'arrête
}
