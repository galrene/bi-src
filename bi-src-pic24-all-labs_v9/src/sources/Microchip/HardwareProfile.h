/******************************************************************************

HardwareProfile.h - Define all hardware-specific information for a project.

This SPECIFIC file is for PIC24F Starter Kit board with PIC24FJ256GB106

******************************************************************************/

#ifndef _HARDWARE_PROFILE_H_
#define _HARDWARE_PROFILE_H_

// Some definitions are in separate file...
#define PIC24F_STARTER_KIT
#include "HardwareProfile - PIC24F Starter Kit.h" // Aarrrggg...

#define USE_GFX_PMP
#define USE_8BIT_PMP
//#define USE_FRC - incompatible with USB operation
//#define GO_SLOW - incompatible with USB host operation

/*********************************************************************
* Overview: Display controller selection.
*                                          
********************************************************************/
#define USE_GFX_DISPLAY_CONTROLLER_SH1101A
//#define USE_GFX_DISPLAY_CONTROLLER_SSD1303

/*********************************************************************
* Overview: Horizontal and vertical display resolution
*                  (from the glass datasheet).
*********************************************************************/
#define DISP_HOR_RESOLUTION             128
#define DISP_VER_RESOLUTION             64
/*********************************************************************
* Overview: Image orientation (can be 0, 90, 180, 270 degrees).
*********************************************************************/
#define DISP_ORIENTATION                0

#if defined( __C30__ )
    #ifdef USE_FRC
        #error Wrong processor or compiler or clock...
        #define GetSystemClock()            32000000UL
        #define GetPeripheralClock()        (GetSystemClock())
        #define GetInstructionClock()       (GetSystemClock() / 2)
        #define MILLISECONDS_PER_TICK       10
        #define TIMER_PRESCALER             TIMER_PRESCALER_8 //TIMER_PRESCALER_1
        #define TIMER_PERIOD                20000 //40000
    #else
        #ifdef GO_SLOW
            #error Wrong processor or compiler or clock...
            #define GetSystemClock()            8000000UL
            #define GetPeripheralClock()        (GetSystemClock())
            #define GetInstructionClock()       (GetSystemClock() / 2)
            #define MILLISECONDS_PER_TICK       10
            #define TIMER_PRESCALER             TIMER_PRESCALER_1
            #define TIMER_PERIOD                40000
        #else
            #define GetSystemClock()            32000000UL
            #define GetPeripheralClock()        (GetSystemClock())
            #define GetInstructionClock()       (GetSystemClock() / 2)
            #define MILLISECONDS_PER_TICK       10
            #define TIMER_PRESCALER             TIMER_PRESCALER_8   // 8MHz: TIMER_PRESCALER_1
            #define TIMER_PERIOD                20000                // 10ms=20000, 1ms=2000
        #endif
    #endif
#elif defined( __PIC32MX__)
    #error Wrong processor or compiler or clock...
    #define USB_A0_SILICON_WORK_AROUND
    //#define RUN_AT_48MHZ
    //#define RUN_AT_24MHZ
    #define RUN_AT_60MHZ
    // Clock values
    #define MILLISECONDS_PER_TICK       10                  // -0.000% error
    #define TIMER_PRESCALER             TIMER_PRESCALER_8   // At 60MHz
    #define TIMER_PERIOD                37500               // At 60MHz
#endif


//#define USE_USB_PLL

#if defined(__PIC32MX__)
    #error Wrong processor or compiler or clock...
    #if defined(RUN_AT_48MHZ)
        #define FCY_SPEED               48000000
        #define PERIPHERAL_BUS_SPEED    48000000
    #elif defined(RUN_AT_24MHZ)
        #define FCY_SPEED               24000000
        #define PERIPHERAL_BUS_SPEED    24000000
    #elif defined(RUN_AT_60MHZ)
        #define FCY_SPEED               60000000
        #define PERIPHERAL_BUS_SPEED    30000000    // Will be divided down
    #else
        #error Choose a speed
    #endif
#endif


// Define the baud rate constants
#if defined(__C30__)
    #define BAUDRATE2       57600UL
    #define BRG_DIV2        4
    #define BRGH2           1
#elif defined (__PIC32MX__)
    #error Wrong processor or compiler or clock...
    #define BAUDRATE2       115200ul
    #define BRG_DIV2        4
    #define BRGH2           1
#endif


// Define the RTCC default initialization.
#define RTCC_DEFAULT_DAY        20        // Date
#define RTCC_DEFAULT_MONTH       2        // Month
#define RTCC_DEFAULT_YEAR       12        // Year
#define RTCC_DEFAULT_WEEKDAY     1        // Day
#define RTCC_DEFAULT_HOUR       10        // hour
#define RTCC_DEFAULT_MINUTE     10        // Minute
#define RTCC_DEFAULT_SECOND     10        // Second

#if defined(__PIC24F__)
    #include <p24fxxxx.h>
    // #include <uart2.h>
#elif defined(__PIC24H__)
    #error Wrong processor or compiler or clock...
    #include <p24hxxxx.h>
    // #include <uart2.h>
#else
    #include <p32xxxx.h>
    #include <plib.h>
    // #include <uart2.h>
#endif

/** TRIS ***********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0


/** CTMU Related****************************************************/
//  - See text in TouchSense.c for details -
//  - Only use 1 of the following 2 #define statements - 
#define TOUCHPAD_DIRECT_TOUCH   //No covering on the buttons
//#define TOUCHPAD_USE_OVERLAY      //Settings for acrylic overlay 

#define TOUCHPAD_LO_PASS            //Filtering for touchpads

#if defined TOUCHPAD_DIRECT_TOUCH 
    #if defined TOUCHPAD_USE_OVERLAY
        #error Cannot use both direct touch and overlay at the same time
    #endif
#endif

/** MDD File System Configuration *********************************************/
#define USE_USB_INTERFACE

// Determines processor type automatically
#ifdef __18CXX
    #error Wrong processor or compiler or clock...
    #define USE_PIC18
#elif defined __PIC24F__
    #define USE_16BIT
    #define USE_PIC24F
#elif defined __PIC24H__
    #error Wrong processor or compiler or clock...
    #define USE_16BIT
    #define USE_PIC24H
#elif defined __dsPIC30F__
    #error Wrong processor or compiler or clock...
    #define USE_16BIT
    #define USE_PIC30
#elif defined __dsPIC33F__
    #error Wrong processor or compiler or clock...
    #define USE_16BIT
    #define USE_PIC33
#elif defined __PIC32MX__
    #error Wrong processor or compiler or clock...
    #define USE_32BIT
    #define USE_PIC32
#else
    #error "Use PIC18, PIC24, dsPIC30, dsPIC33 or PIC32 processor"
#endif

/*********************************************************************
* BACKLIGHT CONTROL (these are dummy macros to satisfy common file gfxpmp.h)
*********************************************************************/
#define DisplayBacklightConfig()      
#define DisplayBacklightOn()        
#define DisplayBacklightOff()          

/*********************************************************************
* EPMP TIMING PARAMETERS
*********************************************************************/
#define PMP_DATA_SETUP_TIME         (0)    
#define PMP_DATA_WAIT_TIME          (45)  // minimum pulse width requirement of CS controlled RD/WR access in SH1101A is 100 ns
                                          // but since we have one cycle in setup and one cycle hold (minimum) we can estimate
                                          // the wait time. 
#define PMP_DATA_HOLD_TIME          (15)  // based on SH1101A data hold requirement  

#if 1
  /*********************************************************************
  * IOS FOR THE DISPLAY CONTROLLER (used in gfxpmp.h)
  *********************************************************************/
  // Definitions for reset pin
  #define DisplayResetConfig()        TRISDbits.TRISD2 = 0    
  #define DisplayResetEnable()        LATDbits.LATD2 = 0
  #define DisplayResetDisable()       LATDbits.LATD2 = 1
  // Definitions for RS pin
  #define DisplayCmdDataConfig()      TRISBbits.TRISB15 = 0;
  #define DisplaySetCommand()         LATBbits.LATB15 = 0
  #define DisplaySetData()            LATBbits.LATB15 = 1
  // Definitions for CS pin
  #define DisplayConfig()             TRISDbits.TRISD11 = 0             
  #define DisplayEnable()             LATDbits.LATD11 = 0
  #define DisplayDisable()            LATDbits.LATD11 = 1
#endif
// SH1101A controller definitions as used in FreeRTOS adaptation
// (DRN demo program July-2012)
  // Definitions for reset pin
  #define OLED_reset_config()         TRISDbits.TRISD2 = 0    
  #define OLED_reset_assert()         LATDbits.LATD2 = 0
  #define OLED_reset_deassert()       LATDbits.LATD2 = 1
  // Definitions for RS pin
  #define OLED_RS_config()            TRISBbits.TRISB15 = 0;
  #define OLED_SetCommand()           LATBbits.LATB15 = 0
  #define OLED_SetData()              LATBbits.LATB15 = 1
  // Definitions for CS pin
  #define OLED_CS_config()            TRISDbits.TRISD11 = 0             
  #define OLED_Select()               LATDbits.LATD11 = 0
  #define OLED_Deselect()             LATDbits.LATD11 = 1
  // Consolidated pin-configuration macro
  #define OLED_ConfigPins() { OLED_reset_config(); OLED_RS_config(); OLED_CS_config(); }




#endif
