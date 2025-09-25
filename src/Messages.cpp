// Messages.cpp
#include "Messages.h"

// ==========================
// Variables globales
// ==========================

// Tableau pour stocker les messages
String messages[MAX_MSG];

// Tableau pour stocker l'expéditeur de chaque message (par ex. MAC)
String messagesFrom[MAX_MSG];

// Index pour le prochain message (circular buffer)
int msgIndex = 0;

// ==========================
// Sauvegarde de l'historique dans LittleFS
// ==========================
void saveHistory() {
    // On ouvre le fichier messages.txt en écriture (écrase l'ancien contenu)
    File f = LittleFS.open("/messages.txt", "w");
    // On ouvre le fichier messagesFrom.txt pour les expéditeurs
    File fmac = LittleFS.open("/messagesFrom.txt", "w");

    // Si un des fichiers n'a pas pu s'ouvrir, on quitte
    if (!f || !fmac) return;

    // Parcours de tous les messages
    for (int i = 0; i < MAX_MSG; i++) {
        // On ne sauvegarde que les cases non vides
        if (messages[i] != "") {
            f.println(messages[i]);       // Écrit le message
            fmac.println(messagesFrom[i]); // Écrit l'expéditeur correspondant
        }
    }

    // Fermeture des fichiers pour sauvegarder les données
    f.close();
    fmac.close();
}

// ==========================
// Chargement de l'historique depuis LittleFS
// ==========================
void loadHistory() {
    // Initialise LittleFS (si déjà fait, pas de problème)
    if (!LittleFS.begin(true)) return;

    // Vérifie que les fichiers existent
    if (!LittleFS.exists("/messages.txt") || !LittleFS.exists("/messagesFrom.txt")) return;

    // Ouverture des fichiers en lecture
    File f = LittleFS.open("/messages.txt", "r");
    File fmac = LittleFS.open("/messagesFrom.txt", "r");

    int idx = 0; // Compteur pour remplir nos tableaux

    // Tant qu'il y a des lignes à lire et qu'on n'a pas dépassé MAX_MSG
    while (f.available() && fmac.available() && idx < MAX_MSG) {
        messages[idx] = f.readStringUntil('\n');       // Lit un message
        messagesFrom[idx] = fmac.readStringUntil('\n'); // Lit l'expéditeur correspondant
        idx++; // Passe à la case suivante
    }

    // Fermeture des fichiers
    f.close();
    fmac.close();

    // Met à jour msgIndex pour le prochain message (circular buffer)
    msgIndex = idx % MAX_MSG;
}

// ==========================
// Réception d'un message via le réseau mesh
// ==========================
void receivedMsg(uint32_t from, String &msg) {
    // Convertit l'identifiant de l'expéditeur en hexadécimal (MAC ou ID)
    String mac = String(from, HEX);

    // Stocke le message et l'expéditeur dans le tableau à la position msgIndex
    messages[msgIndex] = msg;
    messagesFrom[msgIndex] = mac;

    // Avance l'index circulaire (retour au début si MAX_MSG atteint)
    msgIndex = (msgIndex + 1) % MAX_MSG;

    // Sauvegarde immédiatement l'historique
    saveHistory();

    // Affiche le message reçu sur le moniteur série pour debug
    Serial.println(mac + ": " + msg);
}
