#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <painlessMesh.h>
#include <LittleFS.h>
#include "Config.h"     // <- paramètres réseau ici
#include "Messages.h"   // <- gestion des messages
#include "WebApp.h"     // <- serveur web
#include"Motor.h"

// Définition de la broche du relais
#define RELAY_PIN 25

// Objets globaux (partagés avec WebApp)
painlessMesh mesh;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Initialisation du relais
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // moteur éteint au démarrage

  // Exemple de route Web pour allumer/éteindre le moteur
  server.on("/on", []() {
    digitalWrite(RELAY_PIN, HIGH);
    server.send(200, "text/plain", "Moteur ON");
  });

  server.on("/off", []() {
    digitalWrite(RELAY_PIN, LOW);
    server.send(200, "text/plain", "Moteur OFF");
  });

  loadHistory();  // Recharge l’historique sauvegardé

  // Initialisation du mesh
  mesh.init(MESH_SSID, MESH_PASS, MESH_PORT);
  mesh.onReceive(&receivedMsg);

  // Création du point d'accès WiFi
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  Serial.println("IP: " + WiFi.softAPIP().toString());

  // Configuration du serveur web
  setupWebServer();
}

void loop() {
  mesh.update();        // Mise à jour du réseau Mesh
  server.handleClient(); // Gestion des requêtes HTTP
  // Petit test automatique ON/OFF toutes les 3 secondes
  digitalWrite(RELAY_PIN, HIGH); 
  delay(3000);
  digitalWrite(RELAY_PIN, LOW);
  delay(3000);
}
