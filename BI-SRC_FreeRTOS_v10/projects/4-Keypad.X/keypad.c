/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"

/* libves includes. */
#include "touchpad.h"
#include "displej.h"

/**
 * Pomocou xTaskNotify vieme pomocou Give zobudit task
 * kedze mame dve tlacitka pouzijeme 
 *  - bud notify index - vytvorenie viacerych notifikacii pre viacero taskov
 *      - pocet indexov sa setuje v configu
 *  - alebo pouzijeme xTaskNotify kde budeme setovat bity a podla toho
 *    rozhodovat ktore tlacitko bolo stlacene
 *    - alebo set nieco overwrite tiez moze byt useful
 */

typedef struct {
    TaskHandle_t xIncrementTaskHandle;
    TaskHandle_t xSieveTaskHandle;
} TTaskHandles_t;

void vKeypadInit ( void )
{
    touchpad_init();
}

void vKeypadCalibration ( void * pvParameters )
{
    
}

void vChangePriorityTask ( void * pvParameters )
{
    UBaseType_t key = 0;
    char str[2];
    
    TTaskHandles_t * taskHandlesPtr = ( TTaskHandles_t * ) pvParameters;
    
    TaskHandle_t xUDTaskHandle = taskHandlesPtr->xIncrementTaskHandle;
    TaskHandle_t xLRTaskHandle = taskHandlesPtr->xSieveTaskHandle;


    str[1] = '\0';
    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        UBaseType_t priority;
        switch (key)
        {
        case 1:
            // increment priority of "increment task"
            priority = uxTaskPriorityGet( xUDTaskHandle );
            if ( priority == configMAX_PRIORITIES - 1 )
                vTaskPrioritySet( xUDTaskHandle, 0 );
            else
                vTaskPrioritySet( xUDTaskHandle, priority + 1 );
            break;
        case 3:
            // decremet priority of "increment task"
            priority = uxTaskPriorityGet( xUDTaskHandle );
            vTaskPrioritySet( xUDTaskHandle, priority - 1 );
            break;
        case 5:
            // increment priority of "sieve task"
            priority = uxTaskPriorityGet( xLRTaskHandle );
            if ( priority == configMAX_PRIORITIES - 1 )
                vTaskPrioritySet( xLRTaskHandle, 0 );
            else
                vTaskPrioritySet( xLRTaskHandle, priority + 1 );
            break;
        case 4:
            // decrement priority of "sieve task"
            priority = uxTaskPriorityGet( xLRTaskHandle );
            vTaskPrioritySet( xLRTaskHandle, priority - 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( "P:", 3 );
        vDisplayPutString( str, 2 );
    }
    
    vTaskDelete( NULL );
}

/**
 * Upravte vKeypadMonitorTask, kde každých 100 ms provádějte 
 * kontrolu stisknutých kláves a uvolňujte blokované tasky podle
 * stisknutých tlačítek.
 **/
void vKeypadMonitorTask ( void * pvParameters )
{
    BaseType_t key;
    char str[2];
    
    str[1] = '\0';

    TaskHandle_t * changePriorityTask = ( TaskHandle_t * ) pvParameters;

    while( 1 )
    {
        key  = get_touchpad_key();
        if( key > 0 && key < 6 ){
            str[0] = '0' + key;
            vDisplayPutString( "K:", 3 );
            vDisplayPutString( str, 2 );
            xTaskNotify ( *changePriorityTask, key, eSetValueWithOverwrite );
        }
        
        vTaskDelay( 100 / portTICK_PERIOD_MS );
    }
    
    vTaskDelete( NULL );
}