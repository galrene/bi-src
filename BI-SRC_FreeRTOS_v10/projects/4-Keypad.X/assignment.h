#ifndef ASSINGMENT_H
#define	ASSINGMENT_H

extern TaskHandle_t xUDTaskHandle;
extern TaskHandle_t xLRTaskHandle;
extern TaskHandle_t xMTaskHandle;
extern TaskHandle_t xEratHandle;
extern TaskHandle_t xIncrHandle;

void vIncrement ( void );
void vTaskFindPrime ( void );
void vChangeLRTaskPriority ( void * pvParameters );
void vChangeUDTaskPriority ( void * pvParameters );
void vLEDBlinkTask ( void );

#endif	/* ASSINGMENT_H */

