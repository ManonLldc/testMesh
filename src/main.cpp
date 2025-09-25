#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <painlessMesh.h>
#include <LittleFS.h>
#include <LiquidCrystal_I2C.h> // Ajout de la librairie pour l'écran LCD I2C
#include <Wire.h>             // Ajout de la librairie pour la communication I2C
#include "Config.h"           // <- paramètres réseau ici
#include "Messages.h"         // <- gestion des messages
#include "WebApp.h"           // <- serveur web

// Configuration LCD I2C
#define LCD_ADDR 0x27 // Adresse I2C du LCD (peut être 0x3F selon le module)
#define LCD_COLS 16   // Nombre de colonnes
#define LCD_ROWS 2    // Nombre de lignes

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS); // Création de l'objet LCD

// Variables pour gérer l'affichage des messages
String currentMessage = "";
String currentSender = "";
unsigned long lastMessageTime = 0;
bool newMessageReceived = false;

// Objets globaux (partagés avec WebApp)
painlessMesh mesh;
WebServer server(80);

// Fonction pour afficher un message sur le LCD
void displayMessageOnLCD(uint32_t from, String msg) {
  lcd.clear();

  // Première ligne : ID de l'expéditeur
  lcd.setCursor(0, 0);
  lcd.print("From: ");
  lcd.print(from);

  // Deuxième ligne : message (tronqué si trop long)
  lcd.setCursor(0, 1);
  if (msg.length() > LCD_COLS) {
    lcd.print(msg.substring(0, LCD_COLS));
  } else {
    lcd.print(msg);
  }
}

// Fonction pour faire défiler un message long au besoin
void scrollMessage(String msg) {
  if (msg.length() > LCD_COLS) {
    static int scrollPosition = 0;
    static unsigned long lastScrollTime = 0;

    if (millis() - lastScrollTime > 500) { // Défile toutes les 500ms
      lcd.setCursor(0, 1);
      lcd.print("                "); // Efface la ligne
      lcd.setCursor(0, 1);

      String displayText = msg.substring(scrollPosition, scrollPosition + LCD_COLS);
      if (displayText.length() < LCD_COLS) {
        displayText += " " + msg.substring(0, LCD_COLS - displayText.length());
      }
      lcd.print(displayText);

      scrollPosition++;
      if (scrollPosition >= msg.length()) {
        scrollPosition = 0;
      }
      lastScrollTime = millis();
    }
  }
}

// Fonction appelée à chaque fois qu'un message est reçu
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Message reçu de %u : %s\n", from, msg.c_str());

  // Sauvegarder le message pour l'affichage LCD
  currentMessage = msg;
  currentSender = String(from);
  lastMessageTime = millis();
  newMessageReceived = true;

  // Afficher immédiatement sur le LCD
  displayMessageOnLCD(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);

  // Afficher la nouvelle connexion sur LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("New Node:");
  lcd.setCursor(0, 1);
  lcd.print(nodeId);
  delay(2000); // Affiche pendant 2 secondes
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void setup() {
  Serial.begin(115200);

  // Initialisation du système de fichiers
  if(!LittleFS.begin(true)) {
    Serial.println("Erreur lors du montage de LittleFS");
    return;
  }
  Serial.println("LittleFS monté avec succès");

  // Initialisation de l'écran LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mesh Network:");
  lcd.print(MESH_PORT);
  lcd.print("\n");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mesh Ready");
  lcd.setCursor(0, 1);
  lcd.print("ID: ");
  lcd.print(mesh.getNodeId());

  loadHistory(); // Recharge l’historique sauvegardé

  // Initialisation du mesh
  mesh.init(MESH_SSID, MESH_PASS, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  // Création du point d'accès WiFi
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  Serial.println("IP: " + WiFi.softAPIP().toString());

  // Configuration du serveur web
  setupWebServer();

  // Envoi d'un message de test au démarrage pour que les autres nodes le reçoivent
  mesh.sendBroadcast("ESP_lcd online ");
}

void loop() {
  mesh.update();        // Mise à jour du réseau Mesh
  server.handleClient(); // Gestion des requêtes HTTP

  // Gestion du défilement des messages longs
  if (newMessageReceived && currentMessage.length() > LCD_COLS) {
    scrollMessage(currentMessage);
  }

  // Effacer le message après 10 secondes d'inactivité
  if (newMessageReceived && (millis() - lastMessageTime > 10000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting for");
    lcd.setCursor(0, 1);
    lcd.print("messages...");
    newMessageReceived = false;
  }
}