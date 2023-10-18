/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Application includes. */
#include "displej.h"
#include "stack.h"
#include "led.h"

#include <stdio.h>

/*-----------------------------------------------------------*/

/* PIC24F config */
/* Run on 16MHz */
#pragma config JTAGEN=OFF, GCP=OFF, GWRP=OFF, FWDTEN=OFF, ICS=PGx2, \
               IESO=OFF, FCKSM=CSDCMD, OSCIOFNC=OFF, POSCMOD=HS, \
               FNOSC=PRIPLL, PLLDIV=DIV3, IOL1WAY=ON

/* Hardware configuration function declaration. */
static void prvSetupHardware ( void );
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName );

/*-----------------------------------------------------------*/
/**
 * Task1
 *  analyticky <=40B; nahodily pristup 57B
 * Task2
 *  analyticky 76B, nahodily pristup 76B
 */


/* Create the tasks and start the scheduler. */
int main( void )
{
    /* Configure hardware. */
    prvSetupHardware();
    TaskHandle_t task1Handle = NULL;
    TaskHandle_t task2Handle = NULL;
    /* Create the task. */
    xTaskCreate( vStackTask1, ( const char * ) "ST 1", 100, NULL, tskIDLE_PRIORITY + 1, &task1Handle ); 
    xTaskCreate( vStackTask2, ( const char * ) "ST 2", 100, NULL, tskIDLE_PRIORITY + 1, &task2Handle );
    xTaskCreate( vDisplayGatekeeperTask, ( const char * ) "DGKT", 1000, NULL, tskIDLE_PRIORITY + 2, NULL );
    
    /* Start the scheduler. */

    UBaseType_t wm1 = uxTaskGetStackHighWaterMark ( task1Handle );
    UBaseType_t wm2 = uxTaskGetStackHighWaterMark ( task2Handle );
    
    char buffer [12];
    size_t strLen = snprintf ( buffer, sizeof(buffer), "T1:%d T2:%d", wm1, wm2 );
    vDisplayPutString ( buffer, strLen );

    vTaskStartScheduler();
    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    return 0;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName ) {
    vLightLED ( LED_R, 1 );
    while ( 1 );
}


/* Hardware configuration function definition. */
static void prvSetupHardware ( void ) {
    vInitLED ();
    vDisplayInit();
}

/*-----------------------------------------------------------*/
