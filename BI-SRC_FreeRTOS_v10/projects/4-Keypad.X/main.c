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
    TaskHandle_t xIncrementTaskHandle;
    TaskHandle_t xSieveTaskHandle;
} TTaskHandles_t;


/* Create the tasks and start the scheduler. */
int main( void )
{
    /* Configure hardware. */
    prvSetupHardware();
    
    TTaskHandles_t xTaskHandles = { .xIncrementTaskHandle = NULL,
                                    .xSieveTaskHandle     = NULL };

    TaskHandle_t changePriorTaskHandle;

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
                 (TaskHandle_t * ) &xTaskHandles.xIncrementTaskHandle );
    xTaskCreate( vSieveOfEratosthenes,     
                 ( const char * ) "Erat", 
                 configMINIMAL_STACK_SIZE, 
                 NULL,
                 (configMAX_PRIORITIES-2),
                 (TaskHandle_t * ) &xTaskHandles.xSieveTaskHandle );
    // send task handles as parameters to keypad monitor task
    xTaskCreate( vChangePriorityTask,     
                 ( const char * ) "Prior", 
                 configMINIMAL_STACK_SIZE, 
                 (void *) &xTaskHandles,
                 (configMAX_PRIORITIES-1),
                 &changePriorTaskHandle );
    xTaskCreate( vKeypadMonitorTask,     
                 ( const char * ) "Keypad", 
                 configMINIMAL_STACK_SIZE, 
                 (void *) &changePriorTaskHandle,
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
 * 
 * 1.) taskNotifyGive - nic moc
 * 2.) xTaskNotify - dovoluje poslat hodnotu
 *     - xTaskNotifyTake na prijmanie
 */
/**
 * Uloha s vytvaranim a zabijanim tasku nebude fungovat, kvoli heap_1
 * https://www.freertos.org/a00111.html
 * treba inu haldu.
*/