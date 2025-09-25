#ifndef HTMLPAGES_H
#define HTMLPAGES_H

// Page de login
const char LOGIN_PAGE[] PROGMEM = R"rawliteral(
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
<button class='buttonleft'>Plateforme</button>
<button class='buttonright'>d'Authentification</button>
</header>
<main class='mainSign'>
<div class='identification'>
<p>Veuillez vous identifier</p>
<form method='POST' action='/login' class='identificationForm'>
<input type='text' placeholder='Votre adresse mail' class='identificationTA' name='user' minlength='4'>
<input type='password' placeholder='Votre mot de passe' class='identificationTA' name='pass' minlength='4'>
<input class='identificationButton' type='submit' value='Se connecter'>
</form>
</div>
</main>
</body>
</html>
)rawliteral";

// Page admin
const char ADMIN_PAGE[] PROGMEM = R"rawliteral(
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
<button class='buttonleft'>Annonces</button>
<button class='buttonright'>Se connecter</button>
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

#endif
