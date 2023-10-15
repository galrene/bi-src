// Microchip supplied this file for some of the demos.
// Unfortunately:
// - it does not cover all of the hardware for the PIC24F Starter Kit
// - it has some bizarre issues documented below
// DRN Dave.Nadler@Nadler.com 20120803

/********************************************************************
 FileName:      HardwareProfile - PIC24F Starter Kit.h
 Dependencies:  See INCLUDES section
 Processor:     PIC24FJ256GB106
 Hardware:      PIC24F Starter Kit
 Compiler:      Microchip C30
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   11/19/2004   Initial release
  2.1   02/26/2007   Updated for simplicity and to use common
                     coding style
  2.3   09/15/2008   Broke out each hardware platform into its own
                     "HardwareProfile - xxx.h" file
********************************************************************/

#ifndef HARDWARE_PROFILE_PIC24F_STARTER_KIT_H
#define HARDWARE_PROFILE_PIC24F_STARTER_KIT_H

    // DRN: The USB definitions below are INCOMPLETE.
    // DRN: Please see USB_config.h

    /*******************************************************************/
    /******** USB stack hardware selection options *********************/
    /*******************************************************************/
    //This section is the set of definitions required by the MCHPFSUSB
    //  framework.  These definitions tell the firmware what mode it is
    //  running in, and where it can find the results to some information
    //  that the stack needs.
    //These definitions are required by every application developed with
    //  this revision of the MCHPFSUSB framework.  Please review each
    //  option carefully and determine which options are desired/required
    //  for your application.

    // DRN: USB power-sense is not useful in USB-powered applications like
    // DRN: PIC24F Starter Kit board or HSB board 

    //#define USE_SELF_POWER_SENSE_IO
    #define tris_self_power     TRISAbits.TRISA2    // Input
    #define self_power          1

    //#define USE_USB_BUS_SENSE_IO
    #define tris_usb_bus_sense  U1OTGSTATbits.SESVD  //TRISBbits.TRISB5    // Input
    #define USB_BUS_SENSE       U1OTGSTATbits.SESVD

    // DRN: Bootloader was uncommented but not supported as delivered
    // DRN: in Microchip example - comment it out !
    //Uncomment this to make the output HEX of this project 
    //   to be able to be bootloaded using the HID bootloader
    // DRN commented out: #define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER	

    //If the application is going to be used with the HID bootloader
    //  then this will provide a function for the application to 
    //  enter the bootloader from the application (optional)
    #if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
        #define EnterBootloader() __asm__("goto 0x400")
    #endif   

    /*******************************************************************/
    /*******************************************************************/
    /*******************************************************************/
    /******** Application specific definitions *************************/
    /*******************************************************************/
    /*******************************************************************/
    /*******************************************************************/

    /** Board definition ***********************************************/
    //These defintions will tell the main() function which board is
    //  currently selected.  This will allow the application to add
    //  the correct configuration bits as wells use the correct
    //  initialization functions for the board.  These defitions are only
    //  required in the stack provided demos.  They are not required in
    //  final application design.
    #define DEMO_BOARD PIC24F_STARTER_KIT
    #define PIC24F_STARTER_KIT
    // DRN commented out: #define CLOCK_FREQ 32000000

    // DRN: LED driver pins are wired together in pairs on PIC24F
    // DRN: Starter Kit.
    // DRN: RF4-RF5 Red, RG6-RG7 Blue, RG8-RG9 Green
    // DRN: Failing to drive them identically could damage the hardware... 
    // DRN: The 'unusual' coding below changes the port between input
    // DRN: and output, rather than setting the ports to output then
    // DRN: setting the LATCH bits as normally coded... WTF ???
    /** LED ************************************************************/
    #define mInitAllLEDs()      LATG &= 0xFE1F; TRISG &= 0xFE1F; LATF &= 0xFFCF; TRISF &= 0xFFCF; //G6-7,8-9 and F4-5

#if 0 // suppress wacky Microchip coding
    #define mGetLED_1()         (TRISG & ~0x0180?1:0)
    #define mGetLED_2()         (TRISG & ~0x0060?1:0)
    #define mGetLED_3()         (TRISF & ~0x0030?1:0)
    #define mGetLED_4()              

    #define mLED_1_On()         TRISG |= 0x0180; 
    #define mLED_2_On()         TRISG |= 0x0060; 
    #define mLED_3_On()         TRISF |= 0x0030; 
    #define mLED_4_On()          

    #define mLED_1_Off()        TRISG &= ~0x0180; 
    #define mLED_2_Off()        TRISG &= ~0x0060;  
    #define mLED_3_Off()        TRISF &= ~0x0030;  
    #define mLED_4_Off()         
    
    #define mLED_1_Toggle()     TRISG ^= 0x0180; 
    #define mLED_2_Toggle()     TRISG ^= 0x0060;  
    #define mLED_3_Toggle()     TRISF ^= 0x0030; 
    #define mLED_4_Toggle()      

#else // replace wacky Microchip coding
    #define mGetLED_1()         (LATG & ~0x0180?1:0)
    #define mGetLED_2()         (LATG & ~0x0060?1:0)
    #define mGetLED_3()         (LATF & ~0x0030?1:0)
    #define mGetLED_4()              

    #define mLED_1_On()         LATG |= 0x0180; 
    #define mLED_2_On()         LATG |= 0x0060; 
    #define mLED_3_On()         LATF |= 0x0030; 
    #define mLED_4_On()          

    #define mLED_1_Off()        LATG &= ~0x0180; 
    #define mLED_2_Off()        LATG &= ~0x0060;  
    #define mLED_3_Off()        LATF &= ~0x0030;  
    #define mLED_4_Off()         
    
    #define mLED_1_Toggle()     LATG ^= 0x0180; 
    #define mLED_2_Toggle()     LATG ^= 0x0060;  
    #define mLED_3_Toggle()     LATF ^= 0x0030; 
    #define mLED_4_Toggle()      
#endif

    // DRN: The PIC24F Starter Kit has NO SWITCHES and pins RD6 and RD7
    // DRN: are NOT CONNECTED to anything - this is a Microchip kludge
    // DRN: to get demo application to build...
    /** SWITCH *********************************************************/
    #define mInitSwitch2()      TRISDbits.TRISD6=1;
    #define mInitSwitch3()      TRISDbits.TRISD7=1;
    #define mInitAllSwitches()  mInitSwitch2();mInitSwitch3();
    #define sw2                 PORTDbits.RD6
    #define sw3                 PORTDbits.RD7

    /** I/O pin definitions ********************************************/
    #define INPUT_PIN 1
    #define OUTPUT_PIN 0

#endif  //HARDWARE_PROFILE_PIC24F_STARTER_KIT_H
