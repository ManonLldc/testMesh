#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <painlessMesh.h>
#include <LittleFS.h>

#include "Config.h"
#include "Messages.h"
#include "WebApp.h"
#include "Auth.h"  // <-- Inclut Auth.h

painlessMesh mesh;
WebServer server(80);

void setup() {
    Serial.begin(115200);

    loadHistory();

    mesh.init(MESH_SSID, MESH_PASS, MESH_PORT);
    mesh.onReceive(&receivedMsg);

    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
    Serial.println("IP: " + WiFi.softAPIP().toString());

    setupWebServer();

    // Routes login/admin
    server.on("/login", HTTP_GET, []() { handleLogin(server); });
    server.on("/login", HTTP_POST, []() { handleLogin(server); });
    server.on("/admin", HTTP_GET, []() { handleAdmin(server); });

    server.begin();
    Serial.println("Serveur HTTP démarré");
}

void loop() {
    mesh.update();
    server.handleClient();
}
