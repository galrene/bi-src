#ifndef KEYPAD_H
#define	KEYPAD_H

extern TaskHandle_t xUDTaskHandle;
extern TaskHandle_t xLRTaskHandle;
extern TaskHandle_t xMTaskHandle;

void vKeypadInit ( void );
void vKeypadCalibration ( void * );
void vKeypadMonitorTask ( void * );

#endif	/* KEYPAD_H */

