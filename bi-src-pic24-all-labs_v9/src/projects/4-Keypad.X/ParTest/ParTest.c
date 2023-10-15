// DRN adaptation for PIC24F Starter Kit board
// Dave.Nadler@Nadler.com 15-August-2012

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

/* Demo app includes. */
#include "partest.h"

#define ptOUTPUT 	0
#define ptALL_OFF	0

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/


// General hardware initialization is handled here for demo historic reasons...
#include "HardwareProfile.h" // Standard name for all Microchip projects

#if ! defined( __PIC24FJ256GB106__ )
  #error PIC24F Starter Kit board uses __PIC24FJ256GB106__ processor !
#endif
#if defined(USE_FRC)
  #error FRC oscillator is not accurate enough for USB
#endif
#if 0  // see USB_demo.c for configuration bit setup
  /*
  **     JTAGEN_OFF           JTAG port is disabled
  **     ICS_PGx2             Emulator functions are shared with PGEC2/PGED2
  **     FWDTEN_OFF           Watchdog Timer is disabled
  **     GWRP_OFF             Writes to program memory are allowed
  **     GCP_OFF              Code protection is disabled
  */
  _CONFIG1(JTAGEN_OFF & ICS_PGx2 & FWDTEN_OFF)        // JTAG off, watchdog timer off, ICS_PGx2 Emulator functions are shared with PGEC2/PGED2
  // Use the 12MHz clock provided by the PIC18F67J50 (debug interface chip on 24F Starter Kit board)
  _CONFIG2(IESO_OFF & PLL_96MHZ_ON & PLLDIV_DIV3 & FNOSC_PRIPLL & POSCMOD_HS)   // Primary HS OSC with PLL, USBPLL /3
#endif

void vParTestInitialise( void )
{
  #if 0
    /* The explorer 16 board has LED's on port A.  All bits are set as output
    so PORTA is read-modified-written directly. */
    TRISA = ptOUTPUT;
    PORTA = ptALL_OFF;
  #endif

    // ====================  PIC24F Starter Kit board  ====================
    // Pin 1-3 "PMPD" parallel interface - initialized in OLED code
    // Note:  Microchip Demo uses "PMA" ?? PWM for LED brightness ?
    // This board has one three-color LED.
    // Pin 4&5 tied together to drive BLUE LED, Pin 4 is RG6
    TRISGbits.TRISG6 = 0; // output to drive BLUE LED
    #define LED_BLUE_SET(_x)   LATGbits.LATG6 = (_x)? 0:1; // logical 1 for ON => output 0
    #define LED_BLUE_TOGGLE    LED_BLUE_SET(LATGbits.LATG6)
    LED_BLUE_SET(1);
    // Pin 6&8 tied together to drive GREEN LED. Pin 6 is RG8.
    TRISGbits.TRISG8 = 0; // output to drive GREEN LED
    #define LED_GREEN_SET(_x)   LATGbits.LATG8 = (_x)? 0:1; // logical 1 for ON => output 0
    #define LED_GREEN_TOGGLE    LED_GREEN_SET(LATGbits.LATG8)
    LED_GREEN_SET(0);
    // Pin 32&32 tied together to drive RED LED. Pin 32 is RF5.
    TRISFbits.TRISF5 = 0; // output to drive RED LED
    #define LED_RED_SET(_x)   LATFbits.LATF5 = (_x)? 0:1; // logical 1 for ON => output 0
    #define LED_RED_TOGGLE    LED_RED_SET(LATFbits.LATF5)
    LED_RED_SET(0);

    // FreeRTOS "minimal serial" driver uses UART2 but does not connect
    // it to any pins. Connect both TX *and* RX to pins to
    //     pin 49 VCPCON/RP24/CN50/RD1
    // which creates a hardware loopback (as desired for the test tasks)
    TRISDbits.TRISD1 = 0; // set pin as output
    _RP24R = 5; // RP24 outputs U2TX UART2 Transmit Serial port U2
                // Magic number "5" is from Table 10-3; no Microchip #define, aarrggg...
    _U2RXR = 24/*RP24*/ ;
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
    switch(uxLED&0x3) {
        default:
        case 0:   LED_BLUE_SET (xValue);  break;
        case 1:   LED_GREEN_SET(xValue);  break;
        case 2:   LED_RED_SET  (xValue);  break;
    };
#if 0
        unsigned portBASE_TYPE uxLEDBit;
	/* Which port A bit is being modified? */
	uxLEDBit = 1 << uxLED;

	if( xValue )
	{
		/* Turn the LED on. */
		portENTER_CRITICAL();
		{
			PORTA |= uxLEDBit;
		}
		portEXIT_CRITICAL();
	}
	else
	{
		/* Turn the LED off. */
		portENTER_CRITICAL();
		{
			PORTA &= ~uxLEDBit;
		}
		portEXIT_CRITICAL();
        }
#endif
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
    switch(uxLED&0x3) {
        default:
        case 0:   LED_BLUE_TOGGLE;  break;
        case 1:   LED_GREEN_TOGGLE; break;
        case 2:   LED_RED_TOGGLE;   break;
    };
#if 0
    unsigned portBASE_TYPE uxLEDBit;
	uxLEDBit = 1 << uxLED;
	portENTER_CRITICAL();
	{
		/* If the LED is already on - turn it off.  If the LED is already
		off, turn it on. */
		if( PORTA & uxLEDBit )
		{
			PORTA &= ~uxLEDBit;
		}
		else
		{
			PORTA |= uxLEDBit;
		}
	}
        portEXIT_CRITICAL();
#endif
}

