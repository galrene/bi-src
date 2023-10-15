// DRN adaptation to drive PIC24F starter board's SH1101A OLED display
// Dave.Nadler@Nadler.com 3-August-2012

/*
    FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.
    

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes. */
#include "lcd.h" // public interface to this module
#include "PIC24F_StarterKit_OLED.h"

/*
 * The LCD is written to by more than one task so is controlled by this
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the LCD directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 *
 * This task just writes messages on the OLED display (after scrolling the
 * display if required).
 */
static void vLCDTask( void *pvParameters );

/* 
 * Write a string of text to the LCD, scrolling to make room if needed.
 */
static unsigned portSHORT usRow = 0; // next row for prvLCDPutString output
static void prvLCDPutString(const portCHAR *pcString );

/* 
 * Clear the LCD. 
 */
// static void prvLCDClear( void );

void Demo_DelayMs( int ms ) {
  int tics = (int)(((long)configTICK_RATE_HZ*ms)/1000L) ; // slow, careful about computation underflow/overflow
  if(tics==0) tics=1;
  vTaskDelay(tics);
};

/*-----------------------------------------------------------*/

/* The length of the queue used to send messages to the LCD gatekeeper task. */
#define lcdQUEUE_SIZE       3

/* The queue used to send messages to the LCD task. */
xQueueHandle xLCDQueue;


/*-----------------------------------------------------------*/

xQueueHandle xStartLCDTask( void )
{
    /* Create the queue used by the LCD task.  Messages for display on the LCD
    are received via this queue. */
    xLCDQueue = xQueueCreate( lcdQUEUE_SIZE, sizeof( xLCDMessage ) );

    /* Start the task that will write to the LCD.  The LCD hardware is
    initialised from within the task itself so delays can be used. */
    xTaskCreate( vLCDTask, ( signed portCHAR * ) "LCD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    return xLCDQueue;
}
/*-----------------------------------------------------------*/

/* Send the message to the LCD gatekeeper for display. */
void xDisplayLCDmessage(xLCDMessage *xMessage) {
   xQueueSend( xLCDQueue, xMessage, portMAX_DELAY );
}
void xDisplayLCDmessageFromISR(xLCDMessage *xMessage) {
    portBASE_TYPE higherPriorityTaskAwoken;
    xQueueSendToBackFromISR( xLCDQueue, xMessage, &higherPriorityTaskAwoken );
    if(higherPriorityTaskAwoken) { /* could force task reschedule */ };
}

static void prvLCDPutString(const portCHAR *pcString )
{
    if(usRow >= 7) {
        OLED_Scroll_Up(); // no more room, scroll text up on screen
    } else {
        usRow++; // move down to next line
    }
    OLED_PutStringInvertOption(pcString, /*BYTE page*/ usRow, /*BYTE column*/ 0, /*BOOL invert*/ 0);
}
/*-----------------------------------------------------------*/

#if 0
  static void prvLCDClear( void )
  {
    usRow = 0;
    OLED_Clear();
  }
#endif
/*-----------------------------------------------------------*/


static void vLCDTask( void *pvParameters )
{
xLCDMessage xMessage;

    /* Initialise the hardware.  This uses delays so must not be called prior
    to the scheduler being started. */
    OLED_Init();  // replaces prvSetupLCD();

  #if 1    // Quick display primitive test sequence
    OLED_Set_Color(1); // kludgy global "color to draw"
    OLED_Draw_Box(/*SHORT left*/0, /*SHORT top*/0, /*SHORT right*/25, /*SHORT bottom*/25);
	Demo_DelayMs(500);
    OLED_Draw_Box(/*SHORT left*/30, /*SHORT top*/30, /*SHORT right*/75, /*SHORT bottom*/75);
	Demo_DelayMs(500);
	OLED_Draw_Line(/*SHORT x1*/ 44, /*SHORT y1*/4, /*SHORT x2*/104, /*SHORT y2*/60);
	Demo_DelayMs(500);
    OLED_Set_Color(0); // kludgy global "color to draw"
    OLED_Draw_Box(/*SHORT left*/0, /*SHORT top*/0, /*SHORT right*/10, /*SHORT bottom*/10);
    Demo_DelayMs(500);
    OLED_Set_Color(1); // kludgy global "color to draw"
    OLED_PutStringInvertOption("Hi There !", 2,0,1);
    OLED_PutStringInvertOption("Text output works !!", 0,0, 0);
    Demo_DelayMs(1000);
    OLED_Scroll_Up();
	usRow = 7;	// start text at bottom (leave diagnostics on screen; they will scroll off)
  #endif

    /* Welcome message. */
    prvLCDPutString("www.FreeRTOS.org");

    for( ;; )
    {
        /* Wait for a message to arrive that requires displaying. */
        while( xQueueReceive( xLCDQueue, &xMessage, portMAX_DELAY ) != pdPASS ) {
            vTaskDelay( 100 );
        };
        prvLCDPutString(xMessage.pcMessage );

        /* Delay the requested amount of time to ensure the text just written 
        to the LCD is not overwritten. */
        vTaskDelay( xMessage.xMinDisplayTime );     
    }
}
