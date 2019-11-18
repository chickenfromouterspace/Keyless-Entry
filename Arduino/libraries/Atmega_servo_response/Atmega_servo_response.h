#ifndef _ATMEGA_SERVO_RESPONSE_H
#define _ATMEGA_SERVO_RESPONSE_H

#include <SimpleTimer.h>
#include <Servo.h>
#include <millisDelay.h>

#define LED V1
#define LED 13                                        // Pin 13 is connected to the LED
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

void Auto_Lock(void);
void Unlock_Door(void);
void Wifisetup(void);
void Wifiloop(void);

#endif
