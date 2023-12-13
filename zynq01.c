/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, orh a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"

/* Instances of the GPIO Driver */
XGpio gpio;

#define LED_CHANNEL 2
#define LED_MASK    0x01
#define DISP_CHANNEL 1

#define SA 1 << 0
#define SB 1 << 1
#define SC 1 << 2
#define SD 1 << 3
#define SE 1 << 4
#define SF 1 << 5
#define SG 1 << 6
#define DP 1 << 7
#define D0 1 << 8
#define D1 1 << 9
#define D2 1 << 10
#define D3 1 << 11


unsigned int digit_masks[] = {
    SA | SB | SC | SD | SE | SF,      // 0
    SB | SC,                          // 1
    SA | SB | SD | SE | SG,           // 2
    SA | SB | SC | SD | SG,           // 3
    SB | SC | SF | SG,                // 4
    SA | SC | SD | SF | SG,           // 5
    SA | SC | SD | SE | SF | SG,      // 6
    SA | SB | SC,                     // 7
    SA | SB | SC | SD | SE | SF | SG, // 8
    SA | SB | SC | SD | SF | SG,      // 9
    SA | SB | SC | SE | SF | SG,      // A
    SC | SD | SE | SF | SG,           // B
    SA | SD | SE | SF,                // C
    SB | SC | SD | SE | SG,           // D
    SA | SD | SE | SF | SG,           // E
    SA | SE | SF | SG                 // F
};

void init_peripherals() {
	XGpio_Initialize(&gpio, XPAR_AXI_GPIO_LED_DISP_DEVICE_ID);
	XGpio_SetDataDirection(&gpio, LED_CHANNEL,  0x00 );
	XGpio_SetDataDirection(&gpio, DISP_CHANNEL, ~LED_MASK );
}


void all_led_off() {
	XGpio_DiscreteClear(&gpio, LED_CHANNEL, 0xFF);
	print("LEDs turned off\n\r");

}

void cycle_leds (unsigned char * led_mask ) {
	XGpio_DiscreteWrite(&gpio, LED_CHANNEL, *led_mask);
	(*led_mask) <<= 1;
	if ( *led_mask == 0x00 ) // if shifted out of bounds, start from the beginning again
		*led_mask = 1;
}

void display( int num ) {
	XGpio_DiscreteWrite(&gpio, DISP_CHANNEL, digit_masks[num]);
}

void disp_nums ( int num0, int num1, int num2, int num3 ) {
	XGpio_DiscreteWrite(&gpio, DISP_CHANNEL, D3 | digit_masks[num0]);
	usleep(5000);
	XGpio_DiscreteWrite(&gpio, DISP_CHANNEL, D2 | digit_masks[num1]);
	usleep(5000);
* (b) that interact with a Xilinx device throug
	XGpio_DiscreteWrite(&gpio, DISP_CHANNEL, D1 | digit_masks[num2]);
	usleep(5000);
	XGpio_DiscreteWrite(&gpio, DISP_CHANNEL, D0 | digit_masks[num3]);
	usleep(5000);
}

/**
 *  TODO:
 *  Na 7-segmentovém displeji zobrazte čtyři libovolné hexa číslice (vytvořte pole se všemi konfiguracemi 0 až F)
 *
 *  V cyklu posouvejte jedničku v osmibitové proměnné a tu posílejte na LED diody, využijte funkce usleep()

 *	// pri testovani prerusenia preprogramovavat fpga (tlacitkom so stvorcekmi) pred testovanim noveho kodu - registre mozu byt v nespravnom stave
 *	// ID preruseni je vektor preruseni - vyhledate BTNS
 *  Pomocí přerušení od tlačítek, vypište do terminálu, které tlačítko bylo stisknuto
 *
 *  Při přepnutí libovolného přepínače vypište v terminálu hexa hodnotu stavu přepínačů
 */

/**
 * NOTES:
 * spravit citac ktory ked dosiahne hodnotu tak sa precyklia ledky, jediny delay bude mat 7-seg
 * inak by nevychadzali timingy pre 7-seg
 */
int main()
{
   init_platform();
   init_peripherals();
   all_led_off();
   unsigned char led_mask = 1;
   for ( int led_timer = 0; ; led_timer++ )
   {
	   disp_nums(0xD, 0xE, 0xA, 0xD);
	   if ( led_timer % 10 == 0 )
		   cycle_leds(&led_mask);
   }


   cleanup_platform();
   return 0;
}
