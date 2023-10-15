/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Application includes. */
#include "displej.h"
#include "stack.h"
#include "led.h"

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

/* Create the tasks and start the scheduler. */
int main( void )
{
    /* Configure hardware. */
    prvSetupHardware();
    
    /* Create the task. */
    xTaskCreate( vStackTask1, ( const char * ) "ST 1", 100, NULL, tskIDLE_PRIORITY + 1, NULL );
    xTaskCreate( vStackTask2, ( const char * ) "ST 2", 100, NULL, tskIDLE_PRIORITY + 1, NULL );
//    xTaskCreate( vDisplayGatekeeperTask, ( const char * ) "DGKT", 1000, NULL, tskIDLE_PRIORITY + 2, NULL );
    
    /* Start the scheduler. */
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
static void prvSetupHardware ( void )
{
    vInitLED ();
//    vDisplayInit();
}

/*-----------------------------------------------------------*/
