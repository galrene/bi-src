/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* libves includes. */
#include "led.h"

/* Application includes. */
#include "keypad.h"
#include "displej.h"
#include "assignment.h"

/*-----------------------------------------------------------*/

/* PIC24F config */
/* Run on 16MHz */
#pragma config JTAGEN=OFF, GCP=OFF, GWRP=OFF, FWDTEN=OFF, ICS=PGx2, \
               IESO=OFF, FCKSM=CSDCMD, OSCIOFNC=OFF, POSCMOD=HS, \
               FNOSC=PRIPLL, PLLDIV=DIV3, IOL1WAY=ON

/* Hardware configuration function declaration. */
static void prvSetupHardware ( void );

/*-----------------------------------------------------------*/
/* Create the tasks and start the scheduler. */
int main( void )
{
    /* Configure hardware. */
    prvSetupHardware();

    /* Create the tasks. */
    if ( xTaskCreate( ( TaskFunction_t ) vDisplayGatekeeperTask,
                 ( const char * ) "Disp",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 (configMAX_PRIORITIES-1),
                 NULL )
         != pdPASS )
        return 1;
    if ( xTaskCreate( ( TaskFunction_t ) vKeypadMonitorTask,     
                 ( const char * ) "Keypad", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-1),
                 NULL )
         != pdPASS )
        vDisplayPutString ( "task creation error", 19 );
    if ( xTaskCreate( ( TaskFunction_t ) vChangeLRTaskPriority,     
                 ( const char * ) "EraC", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-1),
                 &xLRTaskHandle )
         != pdPASS )
        vDisplayPutString ( "task creation error", 19 );
    if ( xTaskCreate( ( TaskFunction_t ) vChangeUDTaskPriority,     
                 ( const char * ) "++C", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-1),
                 &xUDTaskHandle )
         != pdPASS )
        vDisplayPutString ( "task creation error", 19 );
    if ( xTaskCreate( ( TaskFunction_t ) vTaskFindPrime,     
                 ( const char * ) "Erat", 
                 5*configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-3),
                 &xEratHandle )
        != pdPASS )
        vDisplayPutString ( "task creation error", 19 );
    if ( xTaskCreate( ( TaskFunction_t ) vIncrement,     
                 ( const char * ) "++", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-3),
                 &xIncrHandle )
         != pdPASS )
        vDisplayPutString ( "task creation error", 19 );
    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    return 0;
}
/*-----------------------------------------------------------*/

/* Hardware configuration function definition. */
static void prvSetupHardware ( void )
{
    vDisplayInit();
    vKeypadInit();
    led_init();
    led_all_off();
}

/*-----------------------------------------------------------*/

/**
 * @brief Zadanie: Zaleziac na stlacenom tlacitku vyvolaj udalost
 * ktora zavola task, ktory zmeni prioritu inemu
 * tasku.
 */
/**
 * Uloha s vytvaranim a zabijanim tasku nebude fungovat, kvoli heap_1
 * https://www.freertos.org/a00111.html
 * treba inu haldu.
*/
/**
 * @todo Problemy:
 * Nedorobene uvolnovanie haldy
 * Skarede stvorceky pri vDisplayPutString('\0')
 * Akonahle ma hladat prvocislo, tak sa zasekne
 * 
*/