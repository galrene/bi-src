#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

short prvFoo ( long lA )
{
    short sI;
    unsigned char ucArr [20];
    for ( sI = 0; sI < 20; sI++ )
        ucArr [ sI ] = 42;
    return 42;
}

unsigned char prvBar ( short sB )
{
    short * psP;
    short sA;
    sA = sB;
    psP = &sA;
    return *psP;
}

void vStackTask1 ( void * pvParameters )
{
    unsigned short usI;
    srand('a');
    while ( 1 )
    {
        for ( usI = 0; usI < 10; usI++ )
        {
            if ( rand() % 2 )
            //if ( usI % 2 )
                prvFoo ( 1 );
            else
                prvBar ( 2 );
        }
    }
    vTaskDelete ( NULL );
}

unsigned short prvBinomial ( unsigned short usN, unsigned short usK )
{
    if ( usK == 0 || usK == usN )
        return 1;
    return prvBinomial ( usN - 1, usK - 1 ) + prvBinomial ( usN - 1, usK );
}

void vStackTask2 ( void * pvParameters )
{
    while ( 1 )
    {
        prvBinomial ( 10, 3 );
    }
    vTaskDelete ( NULL );
}

