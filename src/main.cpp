#include <Arduino.h>       // Bibliothèque de base Arduino
#include <WiFi.h>          // Gestion du Wi-Fi sur l'ESP32
#include <WebServer.h>     // Serveur web HTTP
#include <painlessMesh.h>  // Réseau mesh ESP32

// =========================
// Paramètres du réseau mesh
// =========================
const char* MESH_PREFIX   = "monReseauMesh"; // Nom du réseau mesh
const char* MESH_PASSWORD = "12345678";      // Mot de passe du mesh
const int   MESH_PORT     = 5555;            // Port du mesh

painlessMesh mesh;          
WebServer server(80);       

// =========================
// Gestion des messages
// =========================
const int MAX_MESSAGES = 20;     // Nombre max de messages stockés
String messages[MAX_MESSAGES];   // Tableau circulaire des messages
int messageIndex = 0;            // Position du prochain message

// =========================
// Callback : message reçu
// =========================
void receivedCallback(uint32_t from, String &msg) {
  String formattedMsg = "ESP32 " + String(from) + ": " + msg;
  Serial.println(formattedMsg);

  messages[messageIndex] = formattedMsg;
  messageIndex = (messageIndex + 1) % MAX_MESSAGES;
}

// =========================
// Génération de la page web
// =========================
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset='UTF-8'>
      <title>ESP32 Mesh Chat</title>
      // <link rel='stylesheet' href='/ui/css/reset.css'>
      // <link rel='stylesheet' href='/ui/css/style.css'>
    </head>
    <body>
      <h2>ESP32 Mesh Chat</h2>
      <form action='/send' method='POST'>
        <input type='text' id='msgInput' name='message' placeholder='Écris ton message' autofocus>
        <input type='submit' value='Envoyer'>
      </form>
      <ul>
  )rawliteral";

  for(int i = 0; i < MAX_MESSAGES; i++) {
    if(messages[i] != "") {
      html += "<li>" + messages[i] + "</li>";
    }
  }

  html += R"rawliteral(
      </ul>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// =========================
// Envoi d’un message
// =========================
void handleSend() {
  if(server.hasArg("message")) {
    String msg = server.arg("message");
    mesh.sendBroadcast(msg);
    receivedCallback(mesh.getNodeId(), msg);
  }
  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

// =========================
// Feuille de style CSS
// =========================
void handleCSS() {
  String css = R"rawliteral(
    body { font-family: Arial, sans-serif; background:#f4f4f4; color:#333; margin:20px; }
  )rawliteral";

  server.send(200, "text/css", css);
}

// =========================
// Setup
// =========================
void setup() {
  Serial.begin(115200);
  Serial.println("Démarrage ESP32 Mesh Chat...");

  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  WiFi.softAP("ESP32-MeshChat", "12345678");
  Serial.println("IP du point d'accès : " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.on("/style.css", handleCSS);
  server.begin();

  Serial.println("Serveur web démarré");
}

// =========================
// Loop
// =========================
void loop() {
  mesh.update();
  server.handleClient();
}
