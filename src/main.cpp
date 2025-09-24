#include <Arduino.h>        // Bibliothèque de base Arduino
#include <WiFi.h>           // Gestion du Wi-Fi sur ESP32
#include <WebServer.h>      // Serveur web HTTP
#include <painlessMesh.h>   // Réseau mesh ESP32
#include <LittleFS.h>       // Système de fichiers pour stocker l'historique

// ----------------------
// Paramètres réseau mesh
// ----------------------
const char* MESH_SSID = "monReseauMesh";   // Nom du réseau mesh
const char* MESH_PASS = "12345678";        // Mot de passe du mesh
const int MESH_PORT = 5555;                // Port de communication mesh

painlessMesh mesh;          // Objet pour gérer le réseau mesh
WebServer server(80);       // Serveur web qui écoute sur le port 80

// ----------------------
// Gestion des messages
// ----------------------
const int MAX_MSG = 20;     // Nombre maximum de messages stockés
String messages[MAX_MSG];    // Tableau circulaire pour stocker les messages
int msgIndex = 0;            // Index du prochain message à écrire

// ----------------------
// Lecture de l'historique depuis LittleFS
// LittleFS est le système de fichiers flash intégré à l’ESP32.
// En ouvrant le fichier avec File f = LittleFS.open("/messages.txt", "r");, 
// on peut relire les messages précédemment enregistrés pour les remettre dans le tableau de messages au démarrage.
// ----------------------
void loadHistory() {
  if(!LittleFS.begin(true)) return;        // Monte le système de fichiers LittleFS
  if(!LittleFS.exists("/messages.txt")) return; // Si aucun fichier, on quitte
  
// La méthode open() sert à ouvrir un fichier existant ou en créer un nouveau selon le mode choisi.
// Elle renvoie un objet File, qui représente le fichier et permet de lire, écrire ou manipuler les données.
  File f = LittleFS.open("/messages.txt", "r"); // Ouvre le fichier en lecture
  int idx = 0;                                 // Index pour remplir le tableau
  while(f.available() && idx < MAX_MSG){       // Tant que le fichier a des lignes et qu'on n'a pas dépassé MAX_MSG
    messages[idx++] = f.readStringUntil('\n'); // Lire chaque ligne et la stocker dans le tableau
  }
  f.close();                                   // Ferme le fichier
  msgIndex = idx % MAX_MSG;                    // Met à jour l'index pour le prochain message
}

// ----------------------
// Fonction appelée quand un message est reçu via mesh
// uint32_t est un type de donnée entier sur 32 bits la valeur ne peut être que positive ou nulle.
// Contrairement à un entier, il ne peut pas stocker de nombres négatifs.
// ----------------------
void receivedMsg(uint32_t from, String &msg){
  String m = String(from) + ": " + msg;       // Formate le message avec l'ID du noeud
  Serial.println(m);                           // Affiche dans le moniteur série
  messages[msgIndex] = m;                      // Stocke dans le tableau circulaire
  msgIndex = (msgIndex + 1) % MAX_MSG;        // Passe à l'index suivant (boucle si fin atteinte)
}

// ----------------------
// Génération de la page web
// ----------------------
void handleRoot(){
  // Début de la page HTML et CSS intégré
  String html = "<html><head><meta charset='UTF-8'><title>VIGIL</title>"
                "<style>"
                "body{font-family:Arial;background:#f4f4f4;margin:20px;}"
                ".msg{padding:5px;margin:5px;background:#e0e0e0;border-radius:5px;}"
                "input[type=text]{width:70%;padding:10px;}"
                "input[type=submit]{padding:10px 20px;}"
                "</style></head><body><h2>ESP32 Mesh Chat</h2>";

  // Affichage des messages dans l'ordre chronologique
  for(int i=0;i<MAX_MSG;i++){
    int idx = (msgIndex + i) % MAX_MSG;       // Calcul de l'index pour afficher les messages correctement
    if(messages[idx] != "") html += "<div class='msg'>" + messages[idx] + "</div>"; // Affiche le message
  }

  // Formulaire pour envoyer un message
  html += "<form method='POST' action='/send'>"
          "<input type='text' name='message' required>"
          "<input type='submit' value='Envoyer'>"
          "</form></body></html>";

  server.send(200,"text/html",html);          // Envoie la page au navigateur
}

// ----------------------
// Fonction appelée quand un message est envoyé via le formulaire
// ----------------------
void handleSend(){
  if(server.hasArg("message")){               // Vérifie si le champ "message" existe
    String msg = server.arg("message");       // Récupère le texte saisi
    mesh.sendBroadcast(msg);                  // Envoie le message à tous les ESP32 du réseau mesh
    receivedMsg(mesh.getNodeId(), msg);      // Ajoute le message localement
  }
  server.sendHeader("Location","/");          // Redirection vers la page principale
  server.send(303,"text/plain","");           // Code HTTP 303 pour la redirection
}

// ----------------------
// Setup : initialisation
// ----------------------
void setup(){
  Serial.begin(115200);                        // Démarre le moniteur série
  loadHistory();                               // Charge l'historique des messages

  mesh.init(MESH_SSID, MESH_PASS, MESH_PORT); // Initialise le réseau mesh
  mesh.onReceive(&receivedMsg);               // Déclare la fonction appelée pour chaque message reçu

  WiFi.softAP("ESP32-MeshChat","12345678");  // Crée un point d'accès Wi-Fi
  Serial.println("IP: "+WiFi.softAPIP().toString()); // Affiche l'IP

  server.on("/", handleRoot);                 // Page principale
  server.on("/send", handleSend);            // Route pour envoyer un message
  server.begin();                             // Démarre le serveur web
}

// ----------------------
// Loop : exécuté en continu
// ----------------------
void loop(){
  mesh.update();         // Actualise le réseau mesh
  server.handleClient(); // Gère les requêtes HTTP entrantes
}
