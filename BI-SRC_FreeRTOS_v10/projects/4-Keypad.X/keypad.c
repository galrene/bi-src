/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"

/* libves includes. */
#include "touchpad.h"
#include "displej.h"
#include "led.h"

#include "assignment.h"
#include "assignment_defines.h"

void vKeypadInit ( void )
{
    touchpad_init();
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
            else if ( key == KEY_CENTER ) {
                xTaskCreate( ( TaskFunction_t ) vLEDBlinkTask,     
                             ( const char * ) "LED", 
                             configMINIMAL_STACK_SIZE, 
                             NULL,
                             TASK_PRIORITY_CEILING,
                             &xMTaskHandle );
            }
                
                
        }
        
        vTaskDelay( 150 / portTICK_PERIOD_MS );
    }
    
    vTaskDelete( NULL );
}