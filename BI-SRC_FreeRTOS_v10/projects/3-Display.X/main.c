/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

/* Application includes. */
#include "oled.h"
#include "display.h"

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
    
    /* Create the task. */
    xTaskCreate( vDisplayPrintTask, ( const char * ) "Print A", configMINIMAL_STACK_SIZE, (void *) 'A', tskIDLE_PRIORITY + 1, NULL );
    xTaskCreate( vDisplayPrintTask, ( const char * ) "Print B", configMINIMAL_STACK_SIZE, (void *) 'B', tskIDLE_PRIORITY + 1, NULL );
    
    xTaskCreate( vDisplayGatekeeperTask, ( const char * ) "Manage Q", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
    
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
}

/*-----------------------------------------------------------*/
