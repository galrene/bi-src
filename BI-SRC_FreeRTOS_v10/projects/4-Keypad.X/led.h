#ifndef LED_H
#define	LED_H

#include "FreeRTOS.h"

#define ledRED      0
#define ledGREEN    1
#define ledBLUE     2
#define ledON       1
#define ledOFF      0

void vLEDInit ( void );
void vLEDSet ( unsigned portBASE_TYPE uxLED, unsigned portBASE_TYPE uxOn );
void vLEDToggle ( unsigned portBASE_TYPE uxLED );
void vLEDFlashTask ( void * );

#endif	/* LED_H */

