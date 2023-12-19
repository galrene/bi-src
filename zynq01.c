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
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
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

/* Used for interrupt */
#include "xil_exception.h"
#include "xscugic.h"

/* Instances of the GPIO Driver */
XGpio sw_gpio;
XGpio but_gpio;
XGpio led_gpio;
XGpio disp_gpio;

#define LED_CHANNEL     2
#define DISP_CHANNEL    1
#define BUT_CHANNEL     1
#define SWITCH_CHANNEL  1

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
	// init leds
    XGpio_Initialize(&led_gpio, XPAR_AXI_GPIO_LED_DISP_DEVICE_ID);
	XGpio_SetDataDirection(&led_gpio, LED_CHANNEL,  0x00 );
    // init display
    XGpio_Initialize(&disp_gpio, XPAR_AXI_GPIO_LED_DISP_DEVICE_ID); // todo: might be wrong
	XGpio_SetDataDirection(&disp_gpio, DISP_CHANNEL, 0x00 );
    // init buttons
	XGpio_Initialize(&but_gpio, XPAR_AXI_GPIO_BTNS_8BITS_DEVICE_ID);
	XGpio_SetDataDirection(&but_gpio, BUT_CHANNEL, 0xFF );
    // init switches
	XGpio_Initialize(&sw_gpio, XPAR_AXI_GPIO_SWS_12BITS_DEVICE_ID);
	XGpio_SetDataDirection(&sw_gpio, SWITCH_CHANNEL, 0xFFF );
}


void all_led_off() {
	XGpio_DiscreteClear(&led_gpio, LED_CHANNEL, 0xFF);
	print("LEDs turned off\n\r");

}

void cycle_leds (unsigned char * led_mask ) {
	XGpio_DiscreteWrite(&led_gpio, LED_CHANNEL, *led_mask);
	(*led_mask) <<= 1;
	if ( *led_mask == 0x00 ) // if shifted out of bounds, start from the beginning again
		*led_mask = 1;
}

void display( int num ) {
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, digit_masks[num]);
}

void disp_nums ( int pos0, int pos1, int pos2, int pos3 ) {
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D3 | digit_masks[pos0]);
	usleep(5000);
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D2 | digit_masks[pos1]);
	usleep(5000);
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D1 | digit_masks[pos2]);
	usleep(5000);
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D0 | digit_masks[pos3]);
	usleep(5000);
}

void button_handler ( void ) {
	XGpio_InterruptClear(&but_gpio, BUT_CHANNEL);
	xil_printf("BUTTON interrupt\r\n");
	unsigned int readVal = XGpio_DiscreteRead(&but_gpio, BUT_CHANNEL);
	xil_printf("readVal: %X\r\n", readVal);
}

void switch_handler ( void ) {
	XGpio_InterruptClear(&sw_gpio, SWITCH_CHANNEL);
	xil_printf("SWITCH interrupt\r\n");
	unsigned int readVal = XGpio_DiscreteRead(&sw_gpio, SWITCH_CHANNEL);
	xil_printf("readVal: %X\r\n", readVal);
}

int init_interrupt ( void ) {
    XScuGic_Config * cfg = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
	if ( cfg == NULL ) return 0;
	xil_printf("Lookup success\r\n");

	XScuGic int_controller;
    if ( XScuGic_CfgInitialize(&int_controller, cfg, cfg->CpuBaseAddress)
         != XST_SUCCESS ) return 0;
    xil_printf("CfgInit success\r\n");

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler( XIL_EXCEPTION_ID_INT,
                                  (Xil_ExceptionHandler) XScuGic_InterruptHandler,
                                  (void *) &int_controller);

    // enable interrupt from buttons
	XScuGic_Connect( &int_controller, XPAR_FABRIC_AXI_GPIO_BTNS_8BITS_IP2INTC_IRPT_INTR,
                     (Xil_ExceptionHandler) button_handler, NULL);
	XScuGic_Enable(&int_controller, XPAR_FABRIC_AXI_GPIO_BTNS_8BITS_IP2INTC_IRPT_INTR);
	xil_printf("Enabled button interrupt\r\n");

	// enable interrupt from switches
	XScuGic_Connect( &int_controller, XPAR_FABRIC_AXI_GPIO_SWS_12BITS_IP2INTC_IRPT_INTR,
                     (Xil_ExceptionHandler) switch_handler, NULL);
	XScuGic_Enable(&int_controller, XPAR_FABRIC_AXI_GPIO_SWS_12BITS_IP2INTC_IRPT_INTR);
	xil_printf("Enabled switch interrupt\r\n");

	Xil_ExceptionEnable();
	xil_printf("Xil handler registered\r\n");

    XGpio_InterruptGlobalEnable(&sw_gpio); // na sw a butt xgpio instancie
	XGpio_InterruptGlobalEnable(&but_gpio); // na sw a butt xgpio instancie
	XGpio_InterruptEnable(&sw_gpio, SWITCH_CHANNEL); // channel 1 - both buttons and switches
	XGpio_InterruptEnable(&but_gpio, BUT_CHANNEL); // channel 1 - both buttons and switches
    xil_printf("Interrupt handling READY\r\n");
   	return 1;
}

/**
 *  TODO:
  *	// pri testovani prerusenia preprogramovavat fpga (tlacitkom so stvorcekmi) pred testovanim noveho kodu
       - registre mozu byt v nespravnom stave
 *	// ID preruseni je vektor preruseni - vyhledate BTNS
 *  Pomocí přerušení od tlačítek, vypište do terminálu, které tlačítko bylo stisknuto
 *
 *  Při přepnutí libovolného přepínače vypište v terminálu hexa hodnotu stavu přepínačů
 *
 *  xgpio instancia pre kazdu periferiu zvlast
 *
 */

int main()
{
   init_platform();
   init_peripherals();
   if ( ! init_interrupt() ) {
	   xil_printf("Unable to initialise interrupt\r\n");
	   cleanup_platform();
   }

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
