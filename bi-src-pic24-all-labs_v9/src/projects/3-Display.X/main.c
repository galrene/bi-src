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
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF \
     & FWDTEN_OFF & ICS_PGx2)
_CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF \
     & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)

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
    vOLEDInit();
}

/*-----------------------------------------------------------*/
