#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <painlessMesh.h>

// On déclare le serveur et le mesh pour les utiliser dans WebApp.cpp
extern WebServer server;
extern painlessMesh mesh;

// Fonctions du module Web
void setupWebServer();
void handleRoot();
void handleMessages();
void handleSend();