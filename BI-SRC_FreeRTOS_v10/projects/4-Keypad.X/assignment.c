#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led.h"
#include "display.h"
#include "keypad.h"
#include "assignment.h"

#include "stdio.h"
#include <stdbool.h>

#define    FCY    16000000UL
#include <libpic30.h>


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
        snprintf ( buffer, sizeof(buffer), "%d", bUsefulVariable );
        vDisplayPutString ( buffer, sizeof(buffer) )
        
        bUsefulVariable++;
        __delay_ms(1000);
    }    
    
}


/**
 * Vytvořte task, který bude pomocí Eratosthenova síta hledat největší
 * prvočíslo menší než 1000 a přesně jednou za sekundu jej bude vypisovat
 * na displej (výpis nějak odlište od výpisu předchozího tasku).
 * Číslo hledejte pokaždé znovu.
 */

void sieveOfEratosthenes(bool isPrime[], int limit) {
    for (int i = 2; i * i <= limit; ++i) {
        if (isPrime[i]) {
            for (int j = i * i; j <= limit; j += i)
                isPrime[j] = false;
        }
    }
}

int findLargestPrime() {
    bool isPrime[LIMIT];
    // Initialize the array to true, assuming all numbers are prime initially
    for (int i = 0; i < LIMIT; ++i)
        isPrime[i] = true;
    sieveOfEratosthenes(isPrime, LIMIT);
    // Find the largest prime number below the given limit
    for (int i = LIMIT; i >= 2; --i) {
        if (isPrime[i])
            return i;
    }
    // No prime is found
    return -1;
}

#define LIMIT 1000
/* Find largest prime under 1000 */
void vTaskFindPrime() {
    while ( 1 ) {
        char buffer[4];
        int prime = findLargestPrime();
        snprintf ( buffer, sizeof(buffer), "%d", prime );
        vDisplayPutString ( buffer, sizeof(buffer) );
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
