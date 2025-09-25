#include "Auth.h"
#include <mbedtls/sha256.h>  // SHA256

// ==========================
// Constantes et variables
// ==========================
const String ADMIN_USER = "admin";
const String ADMIN_PASS_HASH = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4";
bool isLoggedIn = false;

// ==========================
// Fonction SHA256
// ==========================
String sha256(const String &input) {
    byte hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, (const unsigned char*)input.c_str(), input.length());
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    String output;
    for (int i = 0; i < 32; i++) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        output += buf;
    }
    return output;
}

// ==========================
// Gestion du login
// ==========================
void handleLogin(WebServer &server) {
    if (server.method() == HTTP_POST) {
        String user = server.arg("user");
        String pass = server.arg("pass");

        if (user == ADMIN_USER && sha256(pass) == ADMIN_PASS_HASH) {
            isLoggedIn = true;
            server.sendHeader("Location", "/admin");
            server.send(303, "text/plain", "Redirecting...");
            return;
        } else {
            server.send(401, "text/html", "<p>Login ou mot de passe incorrect</p>");
            return;
        }
    }

    String html = "<form class='identificationTA' method='POST'>"
                  "Utilisateur: <input name='user'><br>"
                  "Mot de passe: <input type='password' name='pass'><br>"
                  "<input type='submit' value='Se connecter'>"
                  "</form>";
    server.send(200, "text/html", html);
}

// ==========================
// Page admin
// ==========================
void handleAdmin(WebServer &server) {
    if (!isLoggedIn) {
        server.sendHeader("Location", "/login");
        server.send(303, "text/plain", "Redirection vers login...");
        return;
    }

     String html = R"rawliteral(
<!doctype html>
<html lang='fr'>
<head>
<meta charset='utf-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<title>Connexion - Vigil</title>
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
<h2>Interface Admin</h2>
</header>
<main class='mainSign'>
<div class='identification'>
<p>Saisissez vos données</p>
<form method='POST' action='/send' class='identificationForm'>
<textarea placeholder='Votre adresse mail' class='identificationTA' name='Ident' minlength='6'></textarea>
<textarea placeholder='Votre mot de passe' class='identificationTA' name='Password' minlength='6'></textarea>
<input class='identificationButton' type='submit' value='S'inscrire'>
</form>
</div>
</main>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}
