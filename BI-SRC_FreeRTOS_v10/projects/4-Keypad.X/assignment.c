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

void vSieveOfEratosthenes(uint8_t isPrime[], BaseType_t limit) {
    for (BaseType_t i = 2; i * i <= limit; ++i) {
        if (isPrime[i]) {
            for (BaseType_t j = i * i; j <= limit; j += i)
                isPrime[j] = 0;
        }
    }
}

BaseType_t iFindLargestPrime ( void ) {
    vDisplayPutString ( "I", 1 );
    uint8_t isPrime[PRIME_LIMIT];
    vDisplayPutString ( "E", 1 );
    // Initialize the array to true, assuming all numbers are prime initially
    for (BaseType_t i = 0; i < PRIME_LIMIT; ++i)
        isPrime[i] = 1;
    vSieveOfEratosthenes(isPrime, PRIME_LIMIT);
    // Find the largest prime number below the given limit
    for (BaseType_t i = PRIME_LIMIT; i >= 2; --i) {
        if (isPrime[i])
            return i;
    }
    // No prime is found
    return -1;
}


void vTaskFindPrime2( void ) {
    while ( 1 ) {
        vDisplayPutString ( "S", 1 );
        char buffer[5] = {0};
        // BaseType_t prime = iFindLargestPrime();
        BaseType_t prime = 7;
        snprintf ( buffer, sizeof(buffer), "%d", prime );
        vDisplayPutString ( buffer, sizeof(buffer)-1 );
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
        char buffer[3]; buffer[2] = '\0';
        snprintf ( buffer, sizeof(buffer), "%d", g_bUsefulVariable );
        g_bUsefulVariable++;
        vDisplayPutString ( buffer, sizeof(buffer)-1 );
        __delay_ms(1000);
    }
    
}


#define MAX_PRIME 1000 // Maximum prime number to find

uint8_t sieve[MAX_PRIME]; // Sieve array to store prime numbers

void initSieve() {
    for (uint8_t i = 0; i < MAX_PRIME; i++) {
        sieve[i] = 1; // Set all elements to prime (1)
    }
}

void markNonPrimes(uint8_t prime) {
    // Mark multiples of prime as non-prime (0)
    for (uint8_t i = 2; i * prime < MAX_PRIME; i++) {
        sieve[i * prime] = 0;
    }
}

void findPrimes() {
    // Mark multiples of 2 as non-prime
    markNonPrimes(2);

    for (uint8_t i = 3; i < MAX_PRIME; i++) {
        if (sieve[i] == 1) {
            // Found a prime number
            // Mark its multiples as non-prime
            markNonPrimes(i);
        }
    }
}


BaseType_t bFindLargestPrime ( void ) {
    vDisplayPutString ( "Init", 4 );
    initSieve(); // Initialize the sieve array
    vDisplayPutString ( "Sieve", 5 );
    findPrimes(); // Find prime numbers
    // Find largest number in the sieve array
    vDisplayPutString ( "Find", 4 );
    for (uint8_t i = MAX_PRIME; i >= 2; --i) {
        if (sieve[i] == 1)
            return i;
    }
}

void vTaskFindPrime( void ) {
    while ( 1 ) {
        vDisplayPutString ( "S", 1 );
        char buffer[5] = {0};
        BaseType_t prime = iFindLargestPrime();
        // BaseType_t prime = 99;
        snprintf ( buffer, sizeof(buffer), "%d", prime );
        vDisplayPutString ( buffer, sizeof(buffer)-1 );
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}