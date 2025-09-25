#include "WebApp.h"
#include "Messages.h"  // On a besoin des messages
#include <WiFi.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <WebServer.h>

// Déclaration externe de la fonction receivedCallback
extern void receivedCallback(uint32_t from, String &msg);

// Page principale HTML
void handleRoot() {
  String html = R"rawliteral(
<!doctype html>
<html lang='fr'>
<head>
<meta charset='utf-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<title>Vigil</title>
<link rel="stylesheet" href="/css/reset.css">
<link rel="stylesheet" href="/css/style.css">
<script>
function reloadMessages(){
  fetch('/messages')
    .then(response => response.text())
    .then(data => {
      document.querySelector('.chatlist').innerHTML = data;
    });
}
setInterval(reloadMessages, 1000);
window.onload = reloadMessages;
</script>
</head>
<body>
<header>
<button class='buttonleft'>Annonces</button>
<button class='buttonright'>Nous contacter</button>
</header>
<main>
<div class='containerul'>
<ul class='chatlist'></ul>
</div>
<form method='POST' action='/send'>
<textarea placeholder='Saisissez votre demande' class='chatbox' name='chatbox' minlength='2'></textarea>
<input class='submit-button' type='submit' value='Envoyez'>
</form>
</main>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Endpoint AJAX : affichage des messages
void handleMessages() {
  String html = "";

  for (int i = 0; i < MAX_MSG; i++) {
    int idx = (msgIndex + i) % MAX_MSG;
    if (messages[idx] != "") {
      String classe = (messagesFrom[idx] == WiFi.macAddress()) ? "lichatreception" : "lichatlist";
      html += "<li class='" + classe + "'><strong>" + messagesFrom[idx] + ":</strong> " + messages[idx] + "</li>";
    }
  }

  server.send(200, "text/html", html);
}

// Formulaire d’envoi
void handleSend() {
  if (server.hasArg("chatbox")) {
    String msg = server.arg("chatbox");
    String mac = WiFi.macAddress();

    messages[msgIndex] = msg;
    messagesFrom[msgIndex] = mac;
    msgIndex = (msgIndex + 1) % MAX_MSG;

    saveHistory();
    
    // Afficher le message sur l'écran LCD local
    receivedCallback(strtoul(mac.c_str(), NULL, 16), msg);
    
    // Envoyer le message aux autres nœuds
    mesh.sendBroadcast(msg);
  }

  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

// Gestionnaire pour les fichiers non trouvés
void handleNotFound() {
  String message = "Fichier non trouvé\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// Gestionnaire pour servir les fichiers statiques
bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";
  String contentType = "text/plain";
  if (path.endsWith(".css")) contentType = "text/css";
  if (path.endsWith(".js")) contentType = "application/javascript";
  if (path.endsWith(".html")) contentType = "text/html";
  
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

// Initialisation du serveur
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.on("/messages", handleMessages);

  // Gestionnaire pour les fichiers statiques
  server.on("/css/reset.css", HTTP_GET, []() {
    if (!handleFileRead("/css/reset.css")) {
      server.send(404, "text/plain", "Fichier CSS non trouvé");
    }
  });
  
  server.on("/css/style.css", HTTP_GET, []() {
    if (!handleFileRead("/css/style.css")) {
      server.send(404, "text/plain", "Fichier CSS non trouvé");
    }
  });

  server.onNotFound(handleNotFound);
  server.begin();
  
  Serial.println("Serveur HTTP démarré");
}
