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
    xTaskCreate( ( TaskFunction_t ) vDisplayGatekeeperTask,
                 ( const char * ) "Disp",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 (configMAX_PRIORITIES-1),
                 NULL );
    xTaskCreate( ( TaskFunction_t ) vKeypadMonitorTask,     
                 ( const char * ) "Keypad", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-1),
                 NULL );
    xTaskCreate( ( TaskFunction_t ) vChangeLRTaskPriority,     
                 ( const char * ) "EraC", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-1),
                 &xLRTaskHandle );
    xTaskCreate( ( TaskFunction_t ) vChangeUDTaskPriority,     
                 ( const char * ) "++C", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-1),
                 &xUDTaskHandle );
    xTaskCreate( ( TaskFunction_t ) vTaskFindPrime,     
                 ( const char * ) "Erat", 
                 6*configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-3),
                 &xEratHandle );
    xTaskCreate( ( TaskFunction_t ) vIncrement,     
                 ( const char * ) "++", 
                 2*configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-3),
                 &xIncrHandle );
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
 * Problems:
 * 
 * Questions:
 * Preco ked pouzijem v incremente vtaskdelay namiesto blokoujuceho
 * delayu tak potrebuje viacej stacku?
 * 
 * Notes:
 * Problem dochadzajucej haldy vyrieseny nepouzivanim busy waitingu,
 * cize sa stiha volat idle task ktory uvolnuje TCB po vTaskDelete(NULL)
 * 
 * https://forums.freertos.org/t/free-heap-memory-after-deleting-task/14627
 * 
*/