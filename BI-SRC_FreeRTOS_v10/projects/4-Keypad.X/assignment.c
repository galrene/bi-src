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

/**
 * Veškeré tasky jakožto i pomocné funkce si deklarujte v assignment.h a 
 * definujte v assignment.c.
 */

/**
 * Vytvořte task, který bude mít proměnnou typu BaseType_t,
 * jednou za vteřinu ji inkrementuje a vypíše její hodnotu na displej.
 * Použijte busy waiting.
 */
BaseType_t g_bUsefulVariable = 0;
void vIncrement ( void ) {
    while ( 1 ) {
        char buffer[3]; buffer[2] = '\0';
        BaseType_t printedCnt = snprintf ( buffer, sizeof(buffer), "%d", g_bUsefulVariable );
        g_bUsefulVariable++;
        vDisplayPutString ( "(" , 1 );
        vDisplayPutString ( buffer, printedCnt );
        vDisplayPutString ( ")", 1 );
        vTaskDelay ( 1000 / portTICK_PERIOD_MS );
    }
    
}


#define MAX_PRIME 1000 // Maximum prime number to find

BaseType_t sieve[MAX_PRIME]; // Sieve array to store prime numbers

void initSieve() {
    for (BaseType_t i = 0; i < MAX_PRIME; i++) {
        sieve[i] = 1; // Set all elements to prime (1)
    }
}

void markNonPrimes(BaseType_t prime) {
    // Mark multiples of prime as non-prime (0)
    for (BaseType_t i = 2; i * prime < MAX_PRIME; i++) {
        sieve[i * prime] = 0;
    }
}

void findPrimes() {
    // Mark multiples of 2 as non-prime
    markNonPrimes(2);

    for (BaseType_t i = 3; i < MAX_PRIME; i++) {
        if (sieve[i] == 1) {
            // Found a prime number
            // Mark its multiples as non-prime
            markNonPrimes(i);
        }
    }
}

BaseType_t bFindLargestPrime ( void ) {
    initSieve(); // Initialize the sieve array
    findPrimes(); // Find prime numbers
    // Find largest number in the sieve array
    for (BaseType_t i = MAX_PRIME; i >= 2; --i) {
        if (sieve[i] == 1)
            return i;
    }
    return -1;
}
/**
 * Vytvořte task, který bude pomocí Eratosthenova síta hledat největší
 * prvočíslo menší než 1000 a přesně jednou za sekundu jej bude vypisovat
 * na displej (výpis nějak odlište od výpisu předchozího tasku).
 * Číslo hledejte pokaždé znovu.
 */
void vTaskFindPrime( void ) {
    while ( 1 ) {
        char buffer[5] = {0};
        BaseType_t prime = bFindLargestPrime();
        BaseType_t printedCnt = snprintf ( buffer, sizeof(buffer), "%d", prime );
        vDisplayPutString ( "[", 1 );
        vDisplayPutString ( buffer, printedCnt );
        vDisplayPutString ( "]", 1 );
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}