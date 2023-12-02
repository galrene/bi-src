/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"

/* libves includes. */
#include "touchpad.h"
#include "displej.h"
#include "led.h"


typedef struct {
    TaskHandle_t xIncrementTaskHandle;
    TaskHandle_t xSieveTaskHandle;
} TTaskHandles_t;

#define KEY_UP     1
#define KEY_DOWN   3
#define KEY_LEFT   4
#define KEY_RIGHT  2
#define KEY_CENTER 5

#define TASK_PRIORITY_CEILING (configMAX_PRIORITIES-3)

void vKeypadInit ( void )
{
    touchpad_init();
}

void vKeypadCalibration ( void * pvParameters )
{
    
}
/**
 * @brief "Sieve" task priority
 */
void vChangeLRTaskPriority ( void * pvParameters )
{
    UBaseType_t key = 0;
    TaskHandle_t sieveTaskHandle = *(( TaskHandle_t * ) pvParameters);
    char str[2];
    str[1] = '\0';

    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        UBaseType_t priority = uxTaskPriorityGet( sieveTaskHandle );;
        switch (key)
        {
        case KEY_RIGHT:
            priority >= TASK_PRIORITY_CEILING ?
            vTaskPrioritySet( sieveTaskHandle, priority = 0 )
            :
            vTaskPrioritySet( sieveTaskHandle, priority += 1 );
            break;
        case KEY_LEFT:
            priority == 0 ?
            vTaskPrioritySet( sieveTaskHandle, priority = TASK_PRIORITY_CEILING )
            :
            vTaskPrioritySet( sieveTaskHandle, priority -= 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( "P:", 3 );
        vDisplayPutString( str, 2 );
    }
}
/**
 * @brief "Increment" task priority
 */
void vChangeUDTaskPriority ( void * pvParameters )
{
    UBaseType_t key = 0;
    TaskHandle_t incTaskHandle = *((TaskHandle_t *) pvParameters);
    char str[2];
    str[1] = '\0';

    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        vDisplayPutString( "Alive", 5 );
        UBaseType_t priority = uxTaskPriorityGet( incTaskHandle );;
        switch (key)
        {
        case KEY_UP:
            priority >= TASK_PRIORITY_CEILING ?
            vTaskPrioritySet( incTaskHandle, priority = 0 )
            :
            vTaskPrioritySet( incTaskHandle, priority += 1 );
            break;
        case KEY_DOWN:
            priority == 0 ?
            vTaskPrioritySet( incTaskHandle, priority = TASK_PRIORITY_CEILING )
            :
            vTaskPrioritySet( incTaskHandle, priority -= 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( "P:", 2 );
        vDisplayPutString( str, 1 );
    }
}

void vLEDBlinkTask ( void )
{
    vDisplayPutString( "LED", 3 );
    led_toggle( LED_R );
    vTaskDelay( 500 / portTICK_PERIOD_MS );
    vDisplayPutString( "   ", 3 );
    led_toggle( LED_R );
    vTaskDelete( NULL );
}

/**
 * Upravte vKeypadMonitorTask, kde každých 100 ms provádějte 
 * kontrolu stisknutých kláves a uvolňujte blokované tasky podle
 * stisknutých tlačítek.
 * 
 * Při zmáčknutí prostředního tlačítka vytvořte task, který rozsvítí LED, 500 ms počká, zhasne LED
 * a smaže sám sebe. Vyzkoušejte alespoň dvacetkrát. Pokud to nefunguje, zjistitě proč a proveďte
 * nezbytné změny.
 **/
void vKeypadMonitorTask ( void * pvParameters )
{
    BaseType_t key;
    char str[2];
    
    str[1] = '\0';
    // TTaskHandles_t * taskHandles = ( TTaskHandles_t * ) pvParameters;
    // TaskHandle_t incrementTaskControl = taskHandles->xIncrementTaskHandle;
    // TaskHandle_t sieveTaskControl = taskHandles->xSieveTaskHandle;
    TaskHandle_t incrementTaskControl = *((TaskHandle_t*)pvParameters);
    TaskHandle_t sieveTaskControl = NULL;
    while( 1 )
    {
        key  = get_touchpad_key();
        if( key > 0 && key < 6 ){
            str[0] = '0' + key;
            vDisplayPutString( "K:", 2 );
            vDisplayPutString( str, 1 );
            if ( key == KEY_UP || key == KEY_DOWN )
                xTaskNotify ( incrementTaskControl, key, eSetValueWithOverwrite );
            else if ( key == KEY_LEFT || key == KEY_RIGHT )
                xTaskNotify ( sieveTaskControl, key, eSetValueWithOverwrite );
            else if ( key == KEY_CENTER )
                xTaskCreate( vLEDBlinkTask,     
                             ( const char * ) "LED", 
                             configMINIMAL_STACK_SIZE, 
                             NULL,
                             TASK_PRIORITY_CEILING,
                             NULL );
        }
        
        vTaskDelay( 100 / portTICK_PERIOD_MS );
    }
    
    vTaskDelete( NULL );
}