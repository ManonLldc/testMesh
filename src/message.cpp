#include <Arduino.h>
#include <LittleFS.h>
#include "Messages.h"

// Constantes
#define MAX_MSG 20
#define FILENAME "/messages.txt"

// Variables de stockage des messages
String messages[MAX_MSG];
String messagesFrom[MAX_MSG];
int msgIndex = 0;

// Fonction pour sauvegarder l'historique des messages dans LittleFS
void saveHistory() {
  Serial.println("Sauvegarde de l'historique...");
  File file = LittleFS.open(FILENAME, "w");
  if (!file) {
    Serial.println("Erreur lors de l'ouverture du fichier pour écriture");
    return;
  }

  for (int i = 0; i < MAX_MSG; i++) {
    if (!messages[i].isEmpty()) {
      file.println(messagesFrom[i]);
      file.println(messages[i]);
    }
  }
  file.close();
  Serial.println("Historique sauvegardé.");
}

// Fonction pour charger l'historique des messages depuis LittleFS
void loadHistory() {
  Serial.println("Chargement de l'historique...");
  if (!LittleFS.exists(FILENAME)) {
    Serial.println("Fichier d'historique inexistant.");
    return;
  }

  File file = LittleFS.open(FILENAME, "r");
  if (!file) {
    Serial.println("Erreur lors de l'ouverture du fichier pour lecture");
    return;
  }

  msgIndex = 0;
  while (file.available() && msgIndex < MAX_MSG) {
    messagesFrom[msgIndex] = file.readStringUntil('\n');
    messagesFrom[msgIndex].trim();
    messages[msgIndex] = file.readStringUntil('\n');
    messages[msgIndex].trim();
    msgIndex++;
  }
  file.close();
  Serial.print("Historique chargé avec ");
  Serial.print(msgIndex);
  Serial.println(" messages.");
}

// Fonction appelée lors de la réception d'un message
void receivedMsg(uint32_t from, String &msg) {
  Serial.printf("Message reçu de %u : %s\n", from, msg.c_str());

  // Ajouter le message à l'historique
  messagesFrom[msgIndex] = String(from);
  messages[msgIndex] = msg;
  msgIndex++;
  if (msgIndex >= MAX_MSG) {
    msgIndex = 0; // Revenir au début du tableau si on atteint la limite
  }

  saveHistory(); // Sauvegarder l'historique après chaque nouveau message
}