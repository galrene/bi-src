#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led.h"
#include "display.h"
#include "displej.h"
#include "keypad.h"
#include "assignment.h"

#include "stdio.h"

#define    FCY    16000000UL
#include <libpic30.h>

#define PRIME_LIMIT 1000

/**
 * Veškeré tasky jakožto i pomocné funkce si deklarujte v assignment.h a 
 * definujte v assignment.c.
 */
/**
 * Vytvořte task, který bude mít proměnnou typu BaseType_t,
 * jednou za vteřinu ji inkrementuje a vypíše její hodnotu na displej.
 * Použijte busy waiting.
 */
void vIncrement ( void ) {
    BaseType_t bUsefulVariable = 0;
    while ( 1 ) {
        char buffer[3];
        snprintf ( buffer, sizeof(buffer), "%d", bUsefulVariable++ );
        vDisplayPutString ( buffer, sizeof(buffer) );
        __delay_ms(1000);
    }
    
}


/**
 * Vytvořte task, který bude pomocí Eratosthenova síta hledat největší
 * prvočíslo menší než 1000 a přesně jednou za sekundu jej bude vypisovat
 * na displej (výpis nějak odlište od výpisu předchozího tasku).
 * Číslo hledejte pokaždé znovu.
 */

void vSieveOfEratosthenes(bool isPrime[], int limit) {
    for (int i = 2; i * i <= limit; ++i) {
        if (isPrime[i]) {
            for (int j = i * i; j <= limit; j += i)
                isPrime[j] = false;
        }
    }
}

int iFindLargestPrime() {
    bool isPrime[PRIME_LIMIT];
    // Initialize the array to true, assuming all numbers are prime initially
    for (int i = 0; i < PRIME_LIMIT; ++i)
        isPrime[i] = true;
    vSieveOfEratosthenes(isPrime, PRIME_LIMIT);
    // Find the largest prime number below the given limit
    for (int i = PRIME_LIMIT; i >= 2; --i) {
        if (isPrime[i])
            return i;
    }
    // No prime is found
    return -1;
}


/* Find largest prime under 1000 */
void vTaskFindPrime( void ) {
    while ( 1 ) {
        char buffer[5];
        int prime = iFindLargestPrime();
        snprintf ( buffer, sizeof(buffer), "%d", prime );
        vDisplayPutString ( buffer, sizeof(buffer) );
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
