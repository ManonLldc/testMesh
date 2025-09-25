#ifndef AUTH_H
#define AUTH_H

#include <Arduino.h>
#include <WebServer.h>

// Constantes et variables
extern const String ADMIN_USER;
extern const String ADMIN_PASS_HASH;
extern bool isLoggedIn;

// Fonctions
String sha256(const String &input);
void handleLogin(WebServer &server);
void handleAdmin(WebServer &server);

#endif
