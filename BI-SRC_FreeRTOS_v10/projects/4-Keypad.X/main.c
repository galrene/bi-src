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

typedef struct {
    TaskHandle_t m_UDHandle;
    TaskHandle_t m_LRHandle;
} TTaskHandles_t;


/**
 * Skus si predavat task handles cez globalne premenne alebo ja uz neviem dopice.
 * 
 */

/* Create the tasks and start the scheduler. */
int main( void )
{
    /* Configure hardware. */
    prvSetupHardware();
    
    TTaskHandles_t TaskCtrlHandles = { .m_UDHandle = NULL,
                                       .m_LRHandle     = NULL };
    TaskHandle_t incrementTask = NULL;
    TaskHandle_t sieveTask     = NULL;

    TaskHandle_t UDTask        = NULL;

    /* Create the task. */
    xTaskCreate( vDisplayGatekeeperTask,
                 ( const char * ) "Disp",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 (configMAX_PRIORITIES-1),
                 NULL );
    xTaskCreate( vIncrement,     
                 ( const char * ) "++", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-2),
                 &incrementTask );
    // if ( xTaskCreate( vTaskFindPrime,     
    //              ( const char * ) "Erat", 
    //              500, 
    //              NULL,
    //              (configMAX_PRIORITIES-2),
    //              &sieveTask )
    //     != pdPASS )
    //     vDisplayPutString ( "task creation error", 19 );

    xTaskCreate( vChangeUDTaskPriority,     
                 ( const char * ) "IPrior", 
                 configMINIMAL_STACK_SIZE, 
                 (void *) &incrementTask,
                 (configMAX_PRIORITIES-1),
                 &UDTask );
    // xTaskCreate( vChangeLRTaskPriority,     
    //              ( const char * ) "SPrior", 
    //              configMINIMAL_STACK_SIZE, 
    //              (void *) &TaskCtrlHandles,
    //              (configMAX_PRIORITIES-1),
    //              (TaskHandle_t * ) &TaskCtrlHandles.xSieveTaskHandle );
    xTaskCreate( vKeypadMonitorTask,     
                 ( const char * ) "Keypad", 
                 3*configMINIMAL_STACK_SIZE, 
                 (void *) &UDTask,
                 (configMAX_PRIORITIES-1),
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