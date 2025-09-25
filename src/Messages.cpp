// Messages.cpp
#include "Messages.h"

// Définition des variables globales
String messages[MAX_MSG];
String messagesFrom[MAX_MSG];
int msgIndex = 0;

// Sauvegarde des messages dans LittleFS
void saveHistory() {
  File f = LittleFS.open("/messages.txt", "w");
  File fmac = LittleFS.open("/messagesFrom.txt", "w");

  if (!f || !fmac) return;

  for (int i = 0; i < MAX_MSG; i++) {
    if (messages[i] != "") {
      f.println(messages[i]);
      fmac.println(messagesFrom[i]);
    }
  }

  f.close();
  fmac.close();
}

// Chargement des messages depuis LittleFS
void loadHistory() {
  if (!LittleFS.begin(true)) return;
  if (!LittleFS.exists("/messages.txt") || !LittleFS.exists("/messagesFrom.txt")) return;

  File f = LittleFS.open("/messages.txt", "r");
  File fmac = LittleFS.open("/messagesFrom.txt", "r");

  int idx = 0;
  while (f.available() && fmac.available() && idx < MAX_MSG) {
    messages[idx] = f.readStringUntil('\n');
    messagesFrom[idx] = fmac.readStringUntil('\n');
    idx++;
  }

  f.close();
  fmac.close();

  msgIndex = idx % MAX_MSG;
}

// Réception d’un message via mesh
void receivedMsg(uint32_t from, String &msg) {
  String mac = String(from, HEX);

  messages[msgIndex] = msg;
  messagesFrom[msgIndex] = mac;

  msgIndex = (msgIndex + 1) % MAX_MSG;
  saveHistory();

  Serial.println(mac + ": " + msg);
}
