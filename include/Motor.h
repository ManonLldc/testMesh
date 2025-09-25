// Motor.h
#ifndef MOTOR_H
#define MOTOR_H

#pragma once
#include <Arduino.h>

#define RELAY_PIN 25  // GPIO relié à IN du relais

void motorInit();
void motorOn();
void motorOff();

#endif