#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "oled.h"
#include "display.h"

xQueueHandle xDisplayQueue;

void vDisplayInit ( void )
{

}

void vDisplayPrintTask ( void * pvParameters )
{
    portBASE_TYPE xI;
    for ( xI = 0; xI < 10; xI++ )
        vOLEDPutChar ( (char) pvParameters );

    vTaskDelete ( NULL );
}

void vDisplayGatekeeperTask ( void * pvParameters )
{

}

void vDisplayPutString ( const char * pcString )
{

}
