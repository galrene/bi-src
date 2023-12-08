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

#include "assignment_defines.h"
/**
 * Veškeré tasky jakožto i pomocné funkce si deklarujte v assignment.h a 
 * definujte v assignment.c.
 */
TaskHandle_t xUDTaskHandle;
TaskHandle_t xLRTaskHandle;
TaskHandle_t xMTaskHandle;
TaskHandle_t xEratHandle;
TaskHandle_t xIncrHandle;

/**
 * Vytvořte task, který bude mít proměnnou typu BaseType_t,
 * jednou za vteřinu ji inkrementuje a vypíše její hodnotu na displej.
 * Použijte busy waiting.
 */
BaseType_t g_bUsefulVariable = 0;
void vIncrement ( void ) {
    while ( 1 ) {
        char buffer[6] = {0};
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

/**
 * @brief "Sieve" task priority
 */
void vChangeLRTaskPriority ( void * pvParameters )
{
    BaseType_t key = 0;
    if ( xEratHandle == NULL)
        vDisplayPutString( "NULL_LR", 7 );
    char str[2];
    str[1] = '\0';

    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        UBaseType_t priority = uxTaskPriorityGet( xEratHandle );
        str[0] = '0' + priority;
        vDisplayPutString( "P(", 2 );
        vDisplayPutString( str, 1 );
        vDisplayPutString( "->", 2 );
        switch (key)
        {
        case KEY_RIGHT:
            priority >= TASK_PRIORITY_CEILING ?
            vTaskPrioritySet( xEratHandle, priority = TASK_PRIORITY_FLOOR )
            :
            vTaskPrioritySet( xEratHandle, priority += 1 );
            break;
        case KEY_LEFT:
            priority <= TASK_PRIORITY_FLOOR ?
            vTaskPrioritySet( xEratHandle, priority = TASK_PRIORITY_CEILING )
            :
            vTaskPrioritySet( xEratHandle, priority -= 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( str, 1 );
        vDisplayPutString( ")", 1 );
    }
}
/**
 * @brief "Increment" task priority
 */
void vChangeUDTaskPriority ( void * pvParameters )
{
    BaseType_t key = 0;
    if ( xIncrHandle == NULL)
        vDisplayPutString( "NULL_UD", 7 );
    char str[2];
    str[1] = '\0';

    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        UBaseType_t priority = uxTaskPriorityGet( xIncrHandle );
        str[0] = '0' + priority;
        vDisplayPutString( "P(", 2 );
        vDisplayPutString( str, 1 );
        vDisplayPutString( "->", 2 );
        switch (key)
        {
        case KEY_UP:
            priority >= TASK_PRIORITY_CEILING ?
            vTaskPrioritySet( xIncrHandle, priority = TASK_PRIORITY_FLOOR )
            :
            vTaskPrioritySet( xIncrHandle, priority += 1 );
            break;
        case KEY_DOWN:
            priority <= TASK_PRIORITY_FLOOR ?
            vTaskPrioritySet( xIncrHandle, priority = TASK_PRIORITY_CEILING )
            :
            vTaskPrioritySet( xIncrHandle, priority -= 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( str, 1 );
        vDisplayPutString( ")", 1 );
    }
}

void vLEDBlinkTask ( void )
{
    led_toggle( LED_R );
    vTaskDelay( 500 / portTICK_PERIOD_MS );
    led_toggle( LED_R );
    vTaskDelete ( NULL );
}

