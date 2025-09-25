// Messages.h
#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>
#include <LittleFS.h>
#include <painlessMesh.h>

// Nombre max de messages
const int MAX_MSG = 20;

// Déclaration des variables externes (définies dans Messages.cpp)
extern String messages[MAX_MSG];
extern String messagesFrom[MAX_MSG];
extern int msgIndex;

// Fonctions exposées
void saveHistory();
void loadHistory();
void receivedMsg(uint32_t from, String &msg);

#endif