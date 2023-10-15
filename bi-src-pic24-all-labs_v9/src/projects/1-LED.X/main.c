/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

/* Application includes. */
#include "led.h"

#include <stdio.h>
/*-----------------------------------------------------------*/

/* Run on 16MHz */
_CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF \
     & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)

/* Hardware configuration function declaration. */
void vinitHW ( void );
/*-----------------------------------------------------------*/

/* Create the tasks and start the scheduler. */
int main ( void )
{
    /* Configure hardware. */
    vinitHW();
    /* Create the task. */
    BaseType_t xReturned;
    xReturned = xTaskCreate (
                    (TaskFunction_t) vSemaphore,
                    "LEDb",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    2,
                    NULL );
    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    return 0;
}
/*-----------------------------------------------------------*/

/* Hardware configuration function definition. */
void vinitHW ( void ) {
    vInitLED ();
}
/*-----------------------------------------------------------*/
