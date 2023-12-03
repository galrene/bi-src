/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"

/* libves includes. */
#include "touchpad.h"
#include "displej.h"
#include "led.h"

#define KEY_UP     1
#define KEY_DOWN   3
#define KEY_LEFT   4
#define KEY_RIGHT  2
#define KEY_CENTER 5

#define TASK_PRIORITY_CEILING (configMAX_PRIORITIES-2)
#define TASK_PRIORITY_FLOOR   (tskIDLE_PRIORITY+1)

TaskHandle_t xUDTaskHandle;
TaskHandle_t xLRTaskHandle;
TaskHandle_t xEratHandle;
TaskHandle_t xIncrHandle;

void vKeypadInit ( void )
{
    touchpad_init();
}

/**
 * @brief "Sieve" task priority
 */
void vChangeLRTaskPriority ( void * pvParameters )
{
    uint32_t key = 0;
    // TaskHandle_t sieveTaskHandle = xTaskGetHandle( "Erat" );
    TaskHandle_t sieveTaskHandle = xEratHandle;
    if ( sieveTaskHandle == NULL)
        vDisplayPutString( "NULL_LR", 7 );
    char str[2];
    str[1] = '\0';

    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        UBaseType_t priority = uxTaskPriorityGet( sieveTaskHandle );
        str[0] = '0' + priority;
        vDisplayPutString( "P(", 2 );
        vDisplayPutString( str, 1 );
        vDisplayPutString( "->", 2 );
        switch (key)
        {
        case KEY_RIGHT:
            priority >= TASK_PRIORITY_CEILING ?
            vTaskPrioritySet( sieveTaskHandle, priority = TASK_PRIORITY_FLOOR )
            :
            vTaskPrioritySet( sieveTaskHandle, priority += 1 );
            break;
        case KEY_LEFT:
            priority == TASK_PRIORITY_FLOOR ?
            vTaskPrioritySet( sieveTaskHandle, priority = TASK_PRIORITY_CEILING )
            :
            vTaskPrioritySet( sieveTaskHandle, priority -= 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( str, 1 );
        vDisplayPutString( ")", 1 );
    }
}
/**
 * @brief "Increment" task priority
 */
void vChangeUDTaskPriority ( void * pvParameters )
{
    uint32_t key = 0;
    // TaskHandle_t incTaskHandle = xTaskGetHandle( "++" );
    TaskHandle_t incTaskHandle = xIncrHandle;
    if ( incTaskHandle == NULL)
        vDisplayPutString( "NULL_UD", 7 );
    char str[2];
    str[1] = '\0';

    while( 1 )
    {
        xTaskNotifyWait( 0, 0, &key, portMAX_DELAY );
        UBaseType_t priority = uxTaskPriorityGet( incTaskHandle );
        str[0] = '0' + priority;
        vDisplayPutString( "P(", 2 );
        vDisplayPutString( str, 1 );
        vDisplayPutString( "->", 2 );
        switch (key)
        {
        case KEY_UP:
            priority >= TASK_PRIORITY_CEILING ?
            vTaskPrioritySet( incTaskHandle, priority = TASK_PRIORITY_FLOOR )
            :
            vTaskPrioritySet( incTaskHandle, priority += 1 );
            break;
        case KEY_DOWN:
            priority == TASK_PRIORITY_FLOOR ?
            vTaskPrioritySet( incTaskHandle, priority = TASK_PRIORITY_CEILING )
            :
            vTaskPrioritySet( incTaskHandle, priority -= 1 );
            break;
        default:
            break;
        }
        str[0] = '0' + priority;
        vDisplayPutString( str, 1 );
        vDisplayPutString( ")", 1 );
    }
}

void vLEDBlinkTask ( void )
{
    led_toggle( LED_R );
    vTaskDelay( 500 / portTICK_PERIOD_MS );
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
    uint32_t key;
    // char str[2];
    // str[1] = '\0';
 
    TaskHandle_t incrementTaskControl = xUDTaskHandle;
    TaskHandle_t sieveTaskControl = xLRTaskHandle;
    if ( sieveTaskControl == NULL)
        vDisplayPutString( "NULL_sieve", 11 );
    if ( incrementTaskControl == NULL)
        vDisplayPutString( "NULL_increment", 9 );
    
    while( 1 )
    {
        key  = get_touchpad_key();
        if( key > 0 && key < 6 ){
            // str[0] = '0' + key;
            // vDisplayPutString( "K:", 2 );
            // vDisplayPutString( str, 1 );
            if ( key == KEY_UP || key == KEY_DOWN )
                xTaskNotify ( incrementTaskControl, key, eSetValueWithOverwrite );
            else if ( key == KEY_LEFT || key == KEY_RIGHT )
                xTaskNotify ( sieveTaskControl, key, eSetValueWithOverwrite );
            else if ( key == KEY_CENTER )
                xTaskCreate( ( TaskFunction_t ) vLEDBlinkTask,     
                             ( const char * ) "LED", 
                             configMINIMAL_STACK_SIZE, 
                             NULL,
                             TASK_PRIORITY_CEILING,
                             NULL );
        }
        
        vTaskDelay( 150 / portTICK_PERIOD_MS );
    }
    
    vTaskDelete( NULL );
}