#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <painlessMesh.h>
#include <LittleFS.h>

// ----------------------
// Paramètres réseau mesh
// ----------------------
const char* MESH_SSID = "monReseauMesh";
const char* MESH_PASS = "12345678";
const int MESH_PORT = 5555;

painlessMesh mesh;
WebServer server(80);

// ----------------------
// Gestion des messages
// ----------------------
const int MAX_MSG = 20;
String messages[MAX_MSG];
int msgIndex = 0;

// ----------------------
// Sauvegarde de l'historique
// ----------------------
void saveHistory() {
  File f = LittleFS.open("/messages.txt", "w");
  if (!f) return;
  for (int i = 0; i < MAX_MSG; i++) {
    if (messages[i] != "") {
      f.println(messages[i]);
    }
  }
  f.close();
}

// ----------------------
// Lecture de l'historique
// ----------------------
void loadHistory() {
  if (!LittleFS.begin(true)) return;
  if (!LittleFS.exists("/messages.txt")) return;

  File f = LittleFS.open("/messages.txt", "r");
  int idx = 0;
  while (f.available() && idx < MAX_MSG) {
    messages[idx++] = f.readStringUntil('\n');
  }
  f.close();
  msgIndex = idx % MAX_MSG;
}

// ----------------------
// Fonction appelée quand un message est reçu via mesh
// ----------------------
void receivedMsg(uint32_t from, String &msg) {
  String m = String(from) + ": " + msg;
  Serial.println(m);
  messages[msgIndex] = m;
  msgIndex = (msgIndex + 1) % MAX_MSG;
  saveHistory();
}

// ----------------------
// Génération de la page web
// ----------------------
void handleRoot(){
  // Début de la page HTML et CSS intégré
  String html = "<!doctype html>
                <html lang='fr'>

                  <head>
                    <meta charset='utf-8'>
                    <meta name='viewport' content='width=device-width, initial-scale=1'>
                    <title>Vigil</title>
                    <link rel='stylesheet' href='./css/reset.css'>
                    <link rel='stylesheet' href='./css/style.css'>
                  </head>"
                 "<body><header>
                      <button class='buttonleft'>Annonces</button>
                      <button class='buttonright'>Nous contacter</button>
                    </header>

                    <main>
                      <div class='containerul'>
                        <ul class='chatlist'>
                        </ul>
                      </div>";

  // Affichage des messages dans l'ordre chronologique
  for(int i=0;i<MAX_MSG;i++){
    int idx = (msgIndex + i) % MAX_MSG;       // Calcul de l'index pour afficher les messages correctement
    if(messages[idx] != "") html += "<div class='msg'>" + messages[idx] + "</div>"; // Affiche le message
  }

  // Formulaire pour envoyer un message
  html += "<form method='POST' action='/send'>"
          "<textarea placeholder='Saisissez votre demande' class='chatbox' name='chatbox' minlength='2'></textarea>"
          "<input class='submit-button' type='submit' value='Envoyez'>"
          "</form></main></body></html>";

  server.send(200, "text/html", html);
}

// ----------------------
// Gestion envoi message
// ----------------------
void handleSend() {
  if (server.hasArg("chatbox")) {
    String msg = server.arg("chatbox");
    mesh.sendBroadcast(msg);
    receivedMsg(mesh.getNodeId(), msg);
  }
  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

// ----------------------
// Setup
// ----------------------
void setup() {
  Serial.begin(115200);
  loadHistory();

  mesh.init(MESH_SSID, MESH_PASS, MESH_PORT);
  mesh.onReceive(&receivedMsg);

  WiFi.softAP("ESP32-MeshChat", "12345678");
  Serial.println("IP: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/send", handleSend);

  // Serve les fichiers CSS depuis LittleFS
  server.serveStatic("/css/", LittleFS, "/css/");

  server.begin();
}

// ----------------------
// Loop
// ----------------------
void loop() {
  mesh.update();
  server.handleClient();
}
