#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <painlessMesh.h>
#include <LittleFS.h>
#include "Config.h"     // <- paramètres réseau ici
#include "Messages.h"   // <- gestion des messages
#include "WebApp.h"     // <- serveur web
#include"Motor.h"

// Objets globaux (partagés avec WebApp)
painlessMesh mesh;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  motorInit(); // initialise le relais/moteur

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
  motorOn();      // allume le moteur
  delay(3000);    // le moteur tourne 3 secondes
  motorOff();     // arrêt moteur
}
