#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void vInitLED ( void ) {
    // outputs
    TRISF &= ~0x0030;
    TRISG &= ~0x03C0;
    // open drain outputs
    ODCF |= 0x0030;
    ODCG |= 0x03C0;
}

void vLEDAllOff ( void ) {
    LATF |= 0x0030; // turn off red
    LATG |= 0x03C0; // turn off green and blue
}

void vLightLED ( short led_id, short on ) {
    switch ( led_id ) {
        case LED_R:
            if ( on ) {
                LATF &= ~0x0030; // light red
                LATG |= 0x03C0; // turn off green and blue
                break;
            }
            LATF |= 0x0030; // turn off red
            LATG |= 0x03C0; // turn off green and blue
            break;
        case LED_G:
            if ( on ) {
                LATG &= ~0x0300; // light green
                LATG |= 0x00C0;  // turn off blue
                LATF |= 0x0030;  // turn off red
                break;
            }
            LATF |= 0x0030; // turn off red
            LATG |= 0x03C0; // turn off green and blue
            break;
        case LED_B:
            if ( on ) {
                LATG &= ~0x00C0;  // light blue
                LATG |= 0x0300; // turn off green
                LATF |= 0x0030;  // turn off red
                break;
            }
            LATF |= 0x0030; // turn off red
            LATG |= 0x03C0; // turn off green and blue
            break;
        case LED_O:
            if ( on ) {
                LATF &= ~0x0030; // light red
                LATG &= ~0x0300; // light green
                LATG |= 0x00C0;  // turn off blue
                break;
            }
            LATF |= 0x0030; // turn off red
            LATG |= 0x03C0; // turn off green and blue            
            break;
        default:
            break;
    }
}

#define WAIT_TIME 1000 // ms

void vBlinkLED ( short led ) {
    vLightLED ( led, 1 );
    vTaskDelay ( WAIT_TIME / portTICK_PERIOD_MS);
    vLightLED ( led, 0 );
    vTaskDelay ( WAIT_TIME / portTICK_PERIOD_MS);
}

void vSemaphore ( void ) {
    while ( 1) {
        vBlinkLED ( LED_R );
        vBlinkLED ( LED_O );
        vBlinkLED ( LED_G );
        vBlinkLED ( LED_R );
        vBlinkLED ( LED_O );
        vBlinkLED ( LED_R );
    }
}