#include "led.h"
#include <Adafruit_NeoPixel.h>

#define PIN 13        // GPIO branché au DIN de ton ruban
#define NUMPIXELS 13  // nombre de LEDs

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void ledInit() {
    strip.begin();
    strip.show(); // éteint au départ
}

void ledOff() {
    strip.clear();
    strip.show();
}

void ledBlinkRed() {
    // Allume rouge
    strip.fill(strip.Color(255, 0, 0));
    strip.show();
    delay(500);

    // Éteint
    strip.clear();
    strip.show();
    delay(500);
}

void ledGradient(int wait) {
    for (int i = 0; i < NUMPIXELS - 2; i++) {
        strip.clear();

        // Rouge
        strip.setPixelColor(i, strip.Color(255, 0, 0));

        // Orange
        strip.setPixelColor(i + 1, strip.Color(255, 128, 0));

        // Jaune
        strip.setPixelColor(i + 2, strip.Color(255, 255, 0));

        strip.show();
        delay(wait);
    }

    // Retour pour effet aller-retour
    for (int i = NUMPIXELS - 3; i >= 0; i--) {
        strip.clear();

        strip.setPixelColor(i, strip.Color(255, 0, 0));       // Rouge
        strip.setPixelColor(i + 1, strip.Color(255, 128, 0)); // Orange
        strip.setPixelColor(i + 2, strip.Color(255, 255, 0)); // Jaune

        strip.show();
        delay(wait);
    }
}

// --- Alarme pompier : balayage rapide rouge/bleu ---
void ledFireTruckSweep(int cycles, int wait) {
    for (int c = 0; c < cycles; c++) {
        // Balayage rouge → bleu
        for (int i = 0; i < NUMPIXELS - 1; i++) { // -1 pour tenir compte de 2 LEDs rouges
            strip.clear();

            // Deux LEDs rouges
            strip.setPixelColor(i, strip.Color(255, 0, 0));
            strip.setPixelColor(i + 1, strip.Color(255, 0, 0));

            // Deux LEDs bleues juste derrière
            if (i > 0) strip.setPixelColor(i - 1, strip.Color(0, 0, 255));
            if (i > 1) strip.setPixelColor(i - 2, strip.Color(0, 0, 255));

            strip.show();
            delay(wait / 6); // 3x plus vite
        }

        // Balayage retour
        for (int i = NUMPIXELS - 2; i >= 1; i--) { // -2 pour 2 LEDs rouges
            strip.clear();

            // Deux LEDs rouges
            strip.setPixelColor(i, strip.Color(255, 0, 0));
            strip.setPixelColor(i + 1, strip.Color(255, 0, 0));

            // Deux LEDs bleues derrière
            if (i < NUMPIXELS - 2) strip.setPixelColor(i + 2, strip.Color(0, 0, 255));
            if (i < NUMPIXELS - 1) strip.setPixelColor(i + 3, strip.Color(0, 0, 255));

            strip.show();
            delay(wait / 6); // 3x plus vite
        }
    }

    ledOff();
}

// Alarme bleu/blanc pour inondation
void ledFloodAlarm(int cycles, int wait) {
    for (int c = 0; c < cycles; c++) {
        // Balayage bleu → blanc
        for (int i = 0; i < NUMPIXELS - 1; i++) {
            strip.clear();

            // Deux LEDs bleues
            strip.setPixelColor(i, strip.Color(0, 0, 255));
            strip.setPixelColor(i + 1, strip.Color(0, 0, 255));

            // Deux LEDs blanches juste derrière
            if (i > 0) strip.setPixelColor(i - 1, strip.Color(255, 255, 255));
            if (i > 1) strip.setPixelColor(i - 2, strip.Color(255, 255, 255));

            strip.show();
            delay(wait / 6);
        }

        // Balayage retour
        for (int i = NUMPIXELS - 2; i >= 1; i--) {
            strip.clear();

            strip.setPixelColor(i, strip.Color(0, 0, 255));
            strip.setPixelColor(i + 1, strip.Color(0, 0, 255));

            if (i < NUMPIXELS - 2) strip.setPixelColor(i + 2, strip.Color(255, 255, 255));
            if (i < NUMPIXELS - 1) strip.setPixelColor(i + 3, strip.Color(255, 255, 255));

            strip.show();
            delay(wait / 6);
        }
    }
    ledOff();
}
