#include <Arduino.h>       // Librairie de base Arduino : Serial, delay, millis, etc.
#include <painlessMesh.h>  // Librairie pour créer un réseau mesh avec ESP32
#include <LiquidCrystal_I2C.h> // Librairie pour l'écran LCD I2C
#include <Wire.h> // Librairie pour la communication I2C

// Paramètres du réseau mesh
#define MESH_PREFIX   "monReseauMesh"   // Nom du réseau Wi-Fi mesh
#define MESH_PASSWORD "12345678"        // Mot de passe du réseau mesh
#define MESH_PORT     5555              // Port utilisé pour la communication mesh

// Configuration LCD I2C
#define LCD_ADDR 0x27  // Adresse I2C du LCD (peut être 0x3F selon le module)
#define LCD_COLS 16    // Nombre de colonnes
#define LCD_ROWS 2     // Nombre de lignes

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS); // Création de l'objet LCD

// Variables pour gérer l'affichage des messages
String currentMessage = "";
String currentSender = "";
unsigned long lastMessageTime = 0;
bool newMessageReceived = false;

// Fonction pour afficher un message sur le LCD
void displayMessageOnLCD(uint32_t from, String msg) {
  lcd.clear();
  
  // Première ligne : ID de l'expéditeur
  lcd.setCursor(0, 0);
  lcd.print("From: ");
  lcd.print(from);
  
  // Deuxième ligne : message (tronqué  si trop long)
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




painlessMesh mesh;  // Création de l'objet mesh qui gère tout le réseau

// Fonction appelée à chaque fois qu'un message est reçu
// 'from' : ID du node qui a envoyé le message
// 'msg'  : contenu du message reçu
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Message reçu de %u : %s\n", from, msg.c_str()); // Affiche le message reçu sur le moniteur série
}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
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
  Serial.begin(115200);  // Démarrage du port série à 115200 bauds
  delay(1000);            // Petit délai pour que le Serial soit prêt
  
  
  // Initialisation de l'écran LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mesh Network:%u\n", MESH_PORT);
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

 delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mesh Ready");
  lcd.setCursor(0, 1);
  lcd.print("ID: ");
  lcd.print(mesh.getNodeId());


  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);


  // Initialisation du réseau mesh
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT); // Configure le réseau avec le nom, mot de passe et port
  mesh.onReceive(&receivedCallback);               // Associe la fonction de réception des messages

  // Envoi d'un message de test au démarrage pour que les autres nodes le reçoivent
  mesh.sendBroadcast("ESP_rouge online ");
}

void loop() {
  mesh.update();  // Nécessaire : permet au mesh de traiter les messages entrants et sortants
  
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



/* 
  // Exemple : envoyer un message toutes les 5 secondes
  static unsigned long lastSend = 0;          // Variable pour mémoriser le dernier envoi
  if (millis() - lastSend > 5000) {           // Vérifie si 5 secondes se sont écoulées
    lastSend = millis();                       // Met à jour le temps du dernier envoi
    mesh.sendBroadcast("ESP_rouge!"); // Envoie le message à tous les nodes du réseau
  }
}
 */
