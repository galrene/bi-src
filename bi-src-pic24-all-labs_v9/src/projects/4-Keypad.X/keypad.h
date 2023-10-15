#ifndef KEYPAD_H
#define	KEYPAD_H

#include "semphr.h"

extern SemaphoreHandle_t xKeypadUp;
extern SemaphoreHandle_t xKeypadDown;
extern SemaphoreHandle_t xKeypadLeft;
extern SemaphoreHandle_t xKeypadRight;
extern SemaphoreHandle_t xKeypadMiddle;

#define keypadUP_TRESHOLD       900
#define keypadDOWN_TRESHOLD     900
#define keypadLEFT_TRESHOLD     900
#define keypadRIGHT_TRESHOLD    900
#define keypadMIDDLE_TRESHOLD   900

void vKeypadInit ( void );
void vKeypadCalibration ( void * );
void vKeypadMonitor ( void * );
void vKeypadTest ( void * );

#endif	/* KEYPAD_H */

