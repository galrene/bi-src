#ifndef KEYPAD_H
#define	KEYPAD_H

extern TaskHandle_t xUDTaskHandle;
extern TaskHandle_t xLRTaskHandle;
extern TaskHandle_t xMTaskHandle;
extern TaskHandle_t xEratHandle;
extern TaskHandle_t xIncrHandle;

void vKeypadInit ( void );
void vKeypadCalibration ( void * );
void vKeypadMonitorTask ( void * pvParameters );
void vChangeLRTaskPriority ( void * pvParameters );
void vChangeUDTaskPriority ( void * pvParameters );

#endif	/* KEYPAD_H */

