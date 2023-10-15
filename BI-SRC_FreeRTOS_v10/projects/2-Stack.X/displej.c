#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include <stdio.h>

#include "display.h"
#include "displej.h"

StreamBufferHandle_t xStreamBuffer;
const size_t xStreamBufferSizeBytes = 50;
const size_t xTriggerLevel = 5;
const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );

void vDisplayInit( void )
{
    disp_init();
    disp_clear();

    xStreamBuffer = xStreamBufferCreate( xStreamBufferSizeBytes,
                                         xTriggerLevel );
    if( xStreamBuffer == NULL)
    {
        /* There was not enough heap memory space available to create the
        stream buffer. */
    }
}

void vDisplayGatekeeperTask( void * pvParameters )
{
    uint8_t ucRxData[ xStreamBufferSizeBytes ];
    size_t xReceivedBytes;
    size_t xIndex;

    while( 1 )
    {
        /* Receive up to another sizeof( ucRxData ) bytes from the stream buffer.
        Wait in the Blocked state (so not using any CPU processing time) for a
        maximum of 100ms for the full sizeof( ucRxData ) number of bytes to be
        available. */
        xReceivedBytes = xStreamBufferReceive( xStreamBuffer,
                                            ( void * ) ucRxData,
                                            sizeof( ucRxData ),
                                            xBlockTime );

        if( xReceivedBytes > 0 )
        {
            for( xIndex = 0; xIndex < xReceivedBytes; xIndex++ )
            {
                disp_char( ucRxData[ xIndex ] );
            }
        }
    }

    vTaskDelete( NULL );
}

void vDisplayPutString( const char *pcStringToSend, size_t xStringLength)
{
    xStreamBufferSend( xStreamBuffer,
                       ( void * ) pcStringToSend,
                       xStringLength, //strlen( pcStringToSend ),
                       portMAX_DELAY );
}