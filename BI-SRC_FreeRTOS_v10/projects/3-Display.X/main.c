/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

/* Application includes. */
#include "oled.h"
#include "display.h"
#include "led.h"
/*-----------------------------------------------------------*/

/* PIC24F config */
/* Run on 16MHz */
#pragma config JTAGEN=OFF, GCP=OFF, GWRP=OFF, FWDTEN=OFF, ICS=PGx2, \
               IESO=OFF, FCKSM=CSDCMD, OSCIOFNC=OFF, POSCMOD=HS, \
               FNOSC=PRIPLL, PLLDIV=DIV3, IOL1WAY=ON

/* Hardware configuration function declaration. */
static void prvSetupHardware ( void );

/*-----------------------------------------------------------*/
void vPrintIdleVarTask( void *pvParameters );
BaseType_t veryUsefulVariable;

void vApplicationIdleHook( void ) {
    veryUsefulVariable++;
}

/*-----------------------------------------------------------*/

/* Create the tasks and start the scheduler. */
int main( void )
{
    /* Configure hardware. */
    prvSetupHardware();
    
    /* Create the task. */
    xTaskCreate( vDisplayGatekeeperTask,
                ( const char * ) "Manage Q",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 3,
                NULL );

    xTaskCreate( vDisplayPrintTask2,
                 ( const char * ) "Greet 1",
                 2*configMINIMAL_STACK_SIZE,
                 (void *) "A",
                 tskIDLE_PRIORITY + 2,
                 NULL );
    xTaskCreate( vDisplayPrintTask2,
                 ( const char * ) "Greet 2",
                 2*configMINIMAL_STACK_SIZE,
                 (void *) "B",
                 tskIDLE_PRIORITY + 2,
                 NULL );
    xTaskCreate( vPrintIdleVarTask,
                 ( const char * ) "PIdle",
                 2*configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 3,
                 NULL );

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

void vPrintIdleVarTask( void *pvParameters )
{
    
    for( ;; )
    {
        char buffer[6] = {0};
        snprintf( buffer, sizeof(buffer), "%d", veryUsefulVariable );
        vDisplayPutString( buffer );
        vTaskDelay( 10000 / portTICK_PERIOD_MS );
    }
}