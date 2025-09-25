#ifndef LED_H
#define LED_H

// Initialise le ruban LED
void ledInit();

// Fait défiler les couleurs (rouge/vert/bleu)
void ledBlinkRed();

// Effet Knight Rider (balayage rouge)
void ledGradient(int wait);

void ledOff();

// Alarme feu
void ledFireTruckSweep(int cycles, int wait);

// Alarme inondation
void ledFloodAlarm(int cycles, int wait);

#endif
