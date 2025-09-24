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