#include "Auth.h"              // Fichier d'en-tête avec les déclarations liées à l'authentification
#include "HtmlPages.h"         // Fichier qui contient les pages HTML sous forme de constantes (LOGIN_PAGE, ADMIN_PAGE)
#include <mbedtls/sha256.h>    // Librairie pour calculer des hash SHA-256

// ==========================
// Constantes et variables
// ==========================
const String ADMIN_USER = "admin@admin";  
// Nom d'utilisateur autorisé pour la connexion

const String ADMIN_PASS_HASH = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4";  
// Hash SHA-256 du mot de passe (ici c'est le hash de "1234")
// https://www.strongpasswordgenerator.org/fr/sha256-hash-generator/

bool isLoggedIn = false;  
// Indique si l'utilisateur est actuellement connecté (true) ou non (false)


// ==========================
// Fonction SHA256
// ==========================
String sha256(const String &input) {
    byte hash[32]; // Tableau pour stocker le hash (32 octets = 256 bits)

    mbedtls_sha256_context ctx;   // Contexte SHA-256 pour effectuer le calcul
    mbedtls_sha256_init(&ctx);    // Initialisation du contexte
    mbedtls_sha256_starts(&ctx, 0);  // Démarre le SHA-256 (0 = SHA-256 standard, pas SHA-224)
    mbedtls_sha256_update(&ctx, (const unsigned char*)input.c_str(), input.length());  
    // Ajoute les données (le mot de passe en clair) dans le calcul du hash
    mbedtls_sha256_finish(&ctx, hash);  
    // Termine le calcul et met le résultat dans "hash"
    mbedtls_sha256_free(&ctx);    
    // Libère les ressources allouées au contexte

    String output;  // Contiendra le hash en texte (hexadécimal lisible)
    for (int i = 0; i < 32; i++) {   // On parcourt les 32 octets du hash
        char buf[3];
        sprintf(buf, "%02x", hash[i]); // Convertit chaque octet en 2 caractères hexadécimaux
        output += buf;                 // Ajoute au résultat final
    }
    return output;  // Retourne le hash complet en hexadécimal (ex: "03ac6742...")
}


// ==========================
// Gestion du login
// ==========================
void handleLogin(WebServer &server) {
    if (server.method() == HTTP_POST) {  
        // Si la requête est un formulaire envoyé en POST
        String user = server.arg("user");  // Récupère le champ "user"
        String pass = server.arg("pass");  // Récupère le champ "pass"

        // Vérifie si l'utilisateur correspond à ADMIN_USER et si le hash du mot de passe est correct
        if (user == ADMIN_USER && sha256(pass) == ADMIN_PASS_HASH) {
            isLoggedIn = true;  // On marque l'utilisateur comme connecté
            server.sendHeader("Location", "/admin");  
            // On envoie une redirection vers /admin
            server.send(303, "text/plain", "Redirecting...");
            return;
        } else {
            // Si login incorrect → erreur 401 (Unauthorized)
            server.send(401, "text/html", "<p>Login ou mot de passe incorrect</p>");
            return;
        }
    }
    // Si ce n’est pas un POST → on affiche la page de connexion
    server.send(200, "text/html", LOGIN_PAGE);
}


// ==========================
// Page admin
// ==========================
void handleAdmin(WebServer &server) {
    if (!isLoggedIn) {  
        // Si l'utilisateur n'est pas connecté → redirection vers la page login
        server.sendHeader("Location", "/login");
        server.send(303, "text/plain", "Redirection vers login...");
        return;
    }
    // Si connecté → on affiche la page admin
    server.send(200, "text/html", ADMIN_PAGE);
}
