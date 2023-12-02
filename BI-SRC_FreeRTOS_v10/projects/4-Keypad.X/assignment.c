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

#define PRIME_LIMIT 100

/**
 * Veškeré tasky jakožto i pomocné funkce si deklarujte v assignment.h a 
 * definujte v assignment.c.
 */

/**
 * Vytvořte task, který bude pomocí Eratosthenova síta hledat největší
 * prvočíslo menší než 1000 a přesně jednou za sekundu jej bude vypisovat
 * na displej (výpis nějak odlište od výpisu předchozího tasku).
 * Číslo hledejte pokaždé znovu.
 */

void vSieveOfEratosthenes(uint8_t isPrime[], int limit) {
    for (int i = 2; i * i <= limit; ++i) {
        if (isPrime[i]) {
            for (int j = i * i; j <= limit; j += i)
                isPrime[j] = 0;
        }
    }
}

int iFindLargestPrime() {
    uint8_t isPrime[PRIME_LIMIT];
    // Initialize the array to true, assuming all numbers are prime initially
    for (int i = 0; i < PRIME_LIMIT; ++i)
        isPrime[i] = 1;
    vSieveOfEratosthenes(isPrime, PRIME_LIMIT);
    // Find the largest prime number below the given limit
    for (int i = PRIME_LIMIT; i >= 2; --i) {
        if (isPrime[i])
            return i;
    }
    // No prime is found
    return -1;
}


void vTaskFindPrime( void ) {
    while ( 1 ) {
        vDisplayPutString ( "Sieve", 5 );
        char buffer[5] = {0};
        BaseType_t prime = iFindLargestPrime();
        snprintf ( buffer, sizeof(buffer), "%x", prime );
        vDisplayPutString ( buffer, sizeof(buffer) );
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}

/**
 * Vytvořte task, který bude mít proměnnou typu BaseType_t,
 * jednou za vteřinu ji inkrementuje a vypíše její hodnotu na displej.
 * Použijte busy waiting.
 */
BaseType_t g_bUsefulVariable = 0;
void vIncrement ( void ) {
    while ( 1 ) {
        char buffer[3]; buffer[2] = 0;
        snprintf ( buffer, sizeof(buffer), "%d", g_bUsefulVariable++ );
        // vDisplayPutString ( buffer, sizeof(buffer)-1 );
        __delay_ms(1000);
    }
    
}
