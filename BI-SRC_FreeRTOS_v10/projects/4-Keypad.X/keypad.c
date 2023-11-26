/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"

/* libves includes. */
#include "touchpad.h"
#include "displej.h"

TaskHandle_t xUDTaskHandle;
TaskHandle_t xLRTaskHandle;
TaskHandle_t xMTaskHandle;

/**
 * Pomocou xTaskNotify vieme pomocou Give zobudit task
 * kedze mame dve tlacitka pouzijeme 
 *  - bud notify index - vytvorenie viacerych notifikacii pre viacero taskov
 *      - pocet indexov sa setuje v configu
 *  - alebo pouzijeme xTaskNotify kde budeme setovat bity a podla toho
 *    rozhodovat ktore tlacitko bolo stlacene
 *    - alebo set nieco overwrite tiez moze byt useful
 */

void vKeypadInit ( void )
{
    touchpad_init();
}

void vKeypadCalibration ( void * pvParameters )
{
    
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
    
    str[1] = 0;
    while( 1 )
    {
        key  = get_touchpad_key();
        if( key > 0 && key < 6 ){
            str[0] = '0' + key;
            vDisplayPutString( str, 1 );
            if ( key == 1 || key == 3 )
                xTaskNotify ( xUDTaskHandle, key, eSetValueWithOverwrite );
            else if ( key == 2 || key == 4 )
                xTaskNotify ( xLRTaskHandle, key, eSetValueWithOverwrite );
            else if ( key == 5 )
                xTaskNotify ( xMTaskHandle, key, eSetValueWithOverwrite );
        }
        
        vTaskDelay( 100 / portTICK_PERIOD_MS );
    }
    
    vTaskDelete( NULL );
}