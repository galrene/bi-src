#include "../Source/include/FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "oled.h"
#include "display.h"

#include <stdio.h>
#include "led.h"

#define    FCY    16000000UL
#include <libpic30.h>

xQueueHandle xDisplayQueue;
#define Q_MAX_ITEMS 6
#define Q_ITEM_SIZE 6

/**
 * vDisplayInit inicializuje OLED displej
 * a vytváří frontu pro výpis na displej
 */
void vDisplayInit ( void )
{
    vOLEDInit();
    xDisplayQueue = xQueueCreate ( Q_MAX_ITEMS,
                                   Q_ITEM_SIZE );
}
/**
 * vDisplayPrintTask je proces, jež v parametru přijímá textový řetězec,
 * pomocí vDisplayPutString jej vypíše na displey a pak čeká zpožďovací smyčkou
 * __delay_ms ( <pocet ms> )
 */
void vDisplayPrintTask ( void * pvParameters )
{
    while ( 1 ) {
        vDisplayPutString ( pvParameters );
        __delay_ms ( 500 );
    }
}
void vDisplayPrintTask2 ( void * pvParameters )
{
    while ( 1 ) {
        vDisplayPutString ( pvParameters );
        vTaskDelay ( 500 / portTICK_PERIOD_MS );
    }
}
/**
 * vDisplayGatekeeperTask je proces čekající na frontě pro výpis na displej
 * a obstarává samotný výpis pomocí funkce vOLEDPutString
 */
void vDisplayGatekeeperTask ( void * pvParameters )
{
    while ( 1 ) {
        char buffer[Q_ITEM_SIZE];
        BaseType_t qRet = xQueueReceive ( xDisplayQueue,
                                          (void * )buffer,
                                          portMAX_DELAY );
        if ( qRet == pdPASS )
            vOLEDPutString ( buffer );
    }
}
/**
 * vDisplayPutString je funkce, která v parametru přijímá řetězec
 * a vkládá jej do fronty pro výpis
 */
void vDisplayPutString ( const char * pcString )
{
    char buffer[Q_ITEM_SIZE];
    snprintf ( buffer, Q_ITEM_SIZE, "%s", pcString );
    xQueueSendToBack ( xDisplayQueue,
                       (void *) buffer, portMAX_DELAY );
}
