#ifndef LED_H
#define	LED_H

#define LED_R 1
#define LED_G 2
#define LED_B 4
#define LED_O 8

void vBlinkLED ( short led );
void vSemaphore ( void );
void vLightLED ( short led_id, short on );
void vInitLED ( void );

#endif	/* LED_H */

