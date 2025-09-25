#include "WebApp.h"
#include "Messages.h"  // On a besoin des messages
#include <WiFi.h>
#include <LittleFS.h>

// Page principale HTML
void handleRoot() {
  String html = R"rawliteral(
<!doctype html>
<html lang='fr'>
<head>
<meta charset='utf-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<title>Bienvenue - Vigil</title>
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
<button class='buttonleft' onclick="window.location.href='/login'">Se connecter</button>
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
    mesh.sendBroadcast(msg);
  }

  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

// Initialisation du serveur
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.on("/messages", handleMessages);

  // Sert les fichiers CSS depuis LittleFS
  server.serveStatic("/css/", LittleFS, "/css/");

  server.begin();
}
