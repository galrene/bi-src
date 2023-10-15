// USB_demo.c
// Dave.Nadler@Nadler.com 20120803
//
// FreeRTOS demo of USB device processing on PIC24F Starter Kit.
// Example simulates serial port:
// - receives data and displays lines on OLED display
// - sends bulk data simulating a dump sensor peripheral
//
// This code is based on 20120728 Microchip "Device - CDC - Basic Demo"
// Uses Microchip's USB stack "MCHPFSUSB"

#include "FreeRTOS.h" // prerequisite for lcd.h
#include "queue.h"    // prerequisite for lcd.h
#include "lcd.h" // FreeRTOS demo - xDisplayLCDmessage etc.
#include "task.h"


/********************************************************************
 FileName:      main.c (Microchip "Device - CDC - Basic Demo")
 Dependencies:  See INCLUDES section
 Processor:		PIC18, PIC24, and PIC32 USB Microcontrollers
 Hardware:		This demo is natively intended to be used on Microchip USB demo
 				boards supported by the MCHPFSUSB stack.  See release notes for
 				support matrix.  This demo can be modified for use on other hardware
 				platforms.
 Complier:  	Microchip C18 (for PIC18), C30 (for PIC24), C32 (for PIC32)
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the �Company�) for its PIC� Microcontroller is intended and
 supplied to you, the Company�s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Description
  ----  -----------------------------------------
  1.0   Initial release

  2.1   Updated for simplicity and to use common
        coding style

  2.6a  Added button debouncing using Start-of-Frame packets

  2.7   Updated demo to place the PIC24F devices into sleep when the
        USB is in suspend.  

  2.7b  Improvements to USBCBSendResume(), to make it easier to use.
********************************************************************/

/** INCLUDES *******************************************************/
#include "USB/usb.h"
#include "USB/usb_function_cdc.h"

#include "HardwareProfile.h"


#if !defined(PIC24F_STARTER_KIT)
  #error This code is tailored to PIC24F Starter Kit hardware **only**
#endif
#if !defined(__C30__)
  #error This code is for XC16 (formerly C30) compiler **only**
#endif
#if defined(USB_POLLING) || !defined(USB_INTERRUPT)
  #error This demo is for use with USB_INTERRUPT **only**
#endif


/** CONFIGURATION **************************************************/

/*
**     JTAGEN_OFF           JTAG port is disabled
**     ICS_PGx2             Emulator functions are shared with PGEC2/PGED2
**     FWDTEN_OFF           Watchdog Timer is disabled
**     GWRP_OFF             Writes to program memory are allowed
**     GCP_OFF              Code protection is disabled
*/
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2)

// Use the 12MHz clock provided by the PIC18F67J50 (debug interface CPU on 24F Starter Kit)
/*
**   POSCMOD_EC     Primary Oscillator: External Clock (no cyrstal, no PLL)
**   FNOSC_PRIPLL   Oscillator select: Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL)
**   PLL_96MHZ_ON   96MHz PLL Enabled
**   IESO_OFF       IESO mode (Two-speed start-up) disabled
**   PLLDIV_DIV3    USB 96 MHz PLL Prescaler: Oscillator input divided by 3 (12MHz input)
** Added to basic FreeRTOS configuration for USB demo
**   FCKSM_CSDCMD   Both Clock Switching and Fail-safe Clock Monitor are disabled
**   OSCIOFNC_ON    Primary Oscillator Output Function: OSCO functions as port I/O (RC15)
**   IOL1WAY_ON     IOLOCK One-Way Set Enable bit: Write RP Registers Once
*/
_CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_EC/* NO:POSCMOD_HS*/ & /*FNOSC_PRI No ???*/FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)


/** I N C L U D E S **********************************************************/

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "usb_config.h"
#include "USB/usb_device.h"
#include "USB/usb.h"

#include "HardwareProfile.h"

/** V A R I A B L E S ********************************************************/

char USB_In_Buffer[64];
char USB_Out_Buffer[64];

BOOL stringPrinted;
volatile BOOL buttonPressed;
volatile BYTE buttonCount;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
static void InitializeSystem(void);
void ProcessIO(void);
void USBDeviceTasks(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void USBCBSendResume(void);
void BlinkUSBStatus(void);
void UserInit(void);

/** VECTOR REMAPPING ***********************************************/
#if defined(__C30__)
    #if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
        /*
         *	ISR JUMP TABLE
         *
         *	It is necessary to define jump table as a function because C30 will
         *	not store 24-bit wide values in program memory as variables.
         *
         *	This function should be stored at an address where the goto instructions 
         *	line up with the remapped vectors from the bootloader's linker script.
         *  
         *  For more information about how to remap the interrupt vectors,
         *  please refer to AN1157.  An example is provided below for the T2
         *  interrupt with a bootloader ending at address 0x1400
         */
//        void __attribute__ ((address(0x1404))) ISRTable(){
//        
//        	asm("reset"); //reset instruction to prevent runaway code
//        	asm("goto %0"::"i"(&_T2Interrupt));  //T2Interrupt's address
//        }
    #endif
#endif


// Initialization of USB requires a few retries at the top level.
// It will FAIL (and never recover) if this sequence is starved for
// time, for example by sticking breakpoints in the sequence below...
#ifdef INIT_DEBUG
  static int attachAttemptCount = 0;
  static int loopCount = 0;
  static int lastUSBDeviceState = 0;
#endif // #ifdef INIT_DEBUG
static void vUSBtask( void *pvParameters ) {
    while(1) {
        #ifdef INIT_DEBUG
            loopCount++;
        #endif // #ifdef INIT_DEBUG
        if(USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE)) {
            // It typically takes one or two times through this loop to
            // get here. Device state goes promptly to ATTACHED_STATE
            USBDeviceAttach();
            #ifdef INIT_DEBUG
                attachAttemptCount++;
                lastUSBDeviceState = USBDeviceState;
            #endif // #ifdef INIT_DEBUG
        };
        #ifdef INIT_DEBUG
            // In the background, ISRs are sequencing through the USB
            // initialization states. It typically takes (~260) loops
            // before reaching the CONFIGURED_STATE
            if(USBDeviceState >= CONFIGURED_STATE) {
                lastUSBDeviceState = USBDeviceState;
            };
        #endif // #ifdef INIT_DEBUG
        vTaskDelay( 1 );
        while(USBDeviceState >= CONFIGURED_STATE) {
          #if 0
            static char sendText[20];
            static int cnt = 0;
            sprintf(sendText, "Demo %4.4d *\r\n", cnt++);
          #else
            // Does USB stack work with >64 character send ?
            // Try 68 + CRLF (70 character xmit) - Works AOK...
            static char sendText[72] = {
                "1234567890" "1234567890" "1234567890" "1234567890" "1234567890" "1234567890" "12345678\r\n" };
          #endif
            if(USBUSARTIsTxTrfReady()) {
                putUSBUSART(sendText,strlen(sendText));
                CDCTxService(); // Kick USB state machine to start transmission
            };
            vTaskDelay( 1000 /* 1 sec for demo */ );
        }
    };
}
// Note: usb_device.h: #define USBGetDeviceState() USBDeviceState
// Note: HardwareProfile - PIC24F Starter Kit.h: #define USB_BUS_SENSE U1OTGSTATbits.SESVD
// So, no side effects from the tests in the initialization loop above;
// all USB state sequencing is handled solely at the ISR level.

void usb_demo_start(void)
{
    InitializeSystem();
    xTaskCreate( vUSBtask, ( signed portCHAR * ) "USB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

  #if 0
    while(1)
    {
        #if defined(USB_INTERRUPT)
            if(USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE))
            {
                USBDeviceAttach();
            }
        #endif

        #if defined(USB_POLLING)
            #error This demo is for use with USB_INTERRUPTS **only**
            // Check bus status and service USB interrupts.
            USBDeviceTasks();
            // Interrupt or polling method.  If using polling, must call
            // this function periodically.  This function will take care
            // of processing and responding to SETUP transactions 
            // (such as during the enumeration process when you first
            // plug in).  USB hosts require that USB devices should accept
            // and process SETUP packets in a timely fashion.  Therefore,
            // when using polling, this function should be called 
            // regularly (such as once every 1.8ms or faster** [see 
            // inline code comments in usb_device.c for explanation when
            // "or faster" applies])  In most cases, the USBDeviceTasks() 
            // function does not take very long to execute (ex: <100 
            // instruction cycles) before it returns.
        #endif

		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.
        ProcessIO();
    }//end while
  #endif
}//end main


/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
    AD1PCFGL = 0xFFFF; // Configure all potential Analog Pins to digital mode

    // The USB specifications require that USB peripheral devices must never source
    // current onto the Vbus pin.  Additionally, USB peripherals should not source
    // current on D+ or D- when the host/hub is not actively powering the Vbus line.
    // When designing a self powered (as opposed to bus powered) USB peripheral
    // device, the firmware should make sure not to turn on the USB module and D+
    // or D- pull up resistor unless Vbus is actively powered.  Therefore, the
    // firmware needs some means to detect when Vbus is being powered by the host.
    // A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
    // can be used to detect when Vbus is high (host actively powering), or low
    // (host is shut down or otherwise not supplying power).  The USB firmware
    // can then periodically poll this I/O pin to know when it is okay to turn on
    // the USB module/D+/D- pull up resistor.  When designing a purely bus powered
    // peripheral device, it is not possible to source current on D+ or D- when the
    // host is not actively providing power on Vbus. Therefore, implementing this
    // bus sense feature is optional.  This firmware can be made to use this bus
    // sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
    // HardwareProfile.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
      tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif

    // If the host PC sends a GetStatus (device) request, the firmware must respond
    // and let the host know if the USB peripheral device is currently bus powered
    // or self powered.  See chapter 9 in the official USB specifications for details
    // regarding this request.  If the peripheral device is capable of being both
    // self and bus powered, it should not return a hard coded value for this request.
    // Instead, firmware should check if it is currently self or bus powered, and
    // respond accordingly.  If the hardware has been configured like demonstrated
    // on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
    // currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
    // is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
    // has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
    // to it in HardwareProfile.h.
    #if defined(USE_SELF_POWER_SENSE_IO)
      tris_self_power = INPUT_PIN;	// See HardwareProfile.h
    #endif

// Yuck:    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem



/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{
    //Initialize all of the debouncing variables
    buttonCount = 0;
    buttonPressed = FALSE;
    stringPrinted = TRUE;

    //Initialize all of the LED pins
	mInitAllLEDs();

    //Initialize the pushbuttons
    mInitAllSwitches();
}//end UserInit

/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void ProcessIO(void)
{   
    BYTE numBytesRead;

    //Blink the LEDs according to the USB device status
    BlinkUSBStatus();
    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

    if(buttonPressed)
    {
        if(stringPrinted == FALSE)
        {
            if(mUSBUSARTIsTxTrfReady())
            {
                putrsUSBUSART("Button Pressed -- \r\n");
                stringPrinted = TRUE;
            }
        }
    }
    else
    {
        stringPrinted = FALSE;
    }

    if(USBUSARTIsTxTrfReady())
    {
		numBytesRead = getsUSBUSART(USB_Out_Buffer,64);
		if(numBytesRead != 0)
		{
			BYTE i;
	        
			for(i=0;i<numBytesRead;i++)
			{
				switch(USB_Out_Buffer[i])
				{
					case 0x0A:
					case 0x0D:
						USB_In_Buffer[i] = USB_Out_Buffer[i];
						break;
					default:
						USB_In_Buffer[i] = USB_Out_Buffer[i] + 1;
						break;
				}

			}

			putUSBUSART(USB_In_Buffer,numBytesRead);
		}
	}

    CDCTxService();
}		//end ProcessIO

/********************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs 
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
void BlinkUSBStatus(void)
{
    static WORD led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

    if(USBSuspendControl == 1)
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            if(mGetLED_1())
            {
                mLED_2_On();
            }
            else
            {
                mLED_2_Off();
            }
        }//end if
    }
    else
    {
        if(USBDeviceState == DETACHED_STATE)
        {
            mLED_Both_Off();
        }
        else if(USBDeviceState == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(USBDeviceState == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(USBDeviceState == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }//end if
        }
        else if(USBDeviceState == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                if(mGetLED_1())
                {
                    mLED_2_Off();
                }
                else
                {
                    mLED_2_On();
                }
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus




// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:
	
	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is 
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause 
	//things to not work as intended.	
	

    #if defined(__C30__)
        USBSleepOnSuspend();
    #endif
}

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// a few milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).
}

#ifdef USB_ENABLE_SOF_HANDLER
  /********************************************************************
   * Function:        void USBCB_SOF_Handler(void)
   *
   * PreCondition:    None
   *
   * Input:           None
   *
   * Output:          None
   *
   * Side Effects:    None
   *
   * Overview:        The USB host sends out a SOF packet to full-speed
   *                  devices every 1 ms. This interrupt may be useful
   *                  for isochronous pipes. End designers should
   *                  implement callback routine as necessary.
   *
   * Note:            None
   *******************************************************************/
  void USBCB_SOF_Handler(void)
  {
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
  }
#endif // #ifdef USB_ENABLE_SOF_HANDLER

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckCDCRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    CDCInitEP();
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function will only be able to wake up the host if
 *                  all of the below are true:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior, 
 *                  as a USB device that has not been armed to perform remote 
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *                  
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are 
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex: 
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup. 
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in 
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager 
    //properties page for the USB device, power management tab, the 
    //"Allow this device to bring the computer out of standby." checkbox 
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();
            
            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again, 
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            }while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/*******************************************************************
 * Function:        void USBCBEP0DataReceived(void)
 *
 * PreCondition:    ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 *                  defined already (in usb_config.h)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called whenever a EP0 data
 *                  packet is received.  This gives the user (and
 *                  thus the various class examples a way to get
 *                  data that is received via the control endpoint.
 *                  This function needs to be used in conjunction
 *                  with the USBCBCheckOtherReq() function since 
 *                  the USBCBCheckOtherReq() function is the apps
 *                  method for getting the initial control transfer
 *                  before the data arrives.
 *
 * Note:            None
 *******************************************************************/
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void)
{
}
#endif

// DRN demonstration code follows...
// ISR-context routine to capture input and echo to LCD display queue
static void check_transfer(void) {
    static char rxFromHost[100];
    static int rxCharCnt = 0; // characters already in buffer
    static char LCDbuffer[ 22 ]; // LCD text buffer 21 characters plus terminator
    // Message to send on the queue to the LCD task.  The first
    // parameter is the minimum time (in ticks) that the message should
    // be left on the LCD without being overwritten.  The second
    // parameter is a pointer to the message to display itself.
    xLCDMessage xMessage = { 0, LCDbuffer };
    char *p0 = &rxFromHost[rxCharCnt];
    // Note: no check for full buffer (can block further processing)
    int l = getsUSBUSART(p0, sizeof(rxFromHost)-rxCharCnt);
    if(l) {
        rxCharCnt += l;
        // scan for CR or LF
        while(l--) {
            char c = *p0++;
            if(c!=0x0A && c!=0x0D) continue;
            // send characters up to the CR or LF
            rxCharCnt -= l; rxCharCnt--;
            if(rxCharCnt >= sizeof(LCDbuffer)) rxCharCnt = sizeof(LCDbuffer)-1;
            rxFromHost[rxCharCnt] = 0; // terminate the string
            memcpy(LCDbuffer, rxFromHost, rxCharCnt+1);
            xDisplayLCDmessageFromISR(&xMessage);
            // reset buffer (drop any characters already buffered)
            rxCharCnt = 0;
            // Note: No clear of USB-stack buffer (could cause problems)...
            break;
        };
    };
    CDCTxService(); // Send queued data...
}

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch( (INT)event )
    {
        case EVENT_TRANSFER:
            if( (USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl==1) )break;
            // Add application-specific callback function here if desired.
            check_transfer();
            break;
        #ifdef USB_ENABLE_SOF_HANDLER
          case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        #endif // #ifdef USB_ENABLE_SOF_HANDLER
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            // Add application-specific callback here if desired.
            // The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            // FEATURE (endpoint halt) request on an application endpoint which was 
            // previously armed (UOWN was = 1).  Here would be a good place to:
            // 1.  Determine which endpoint the transaction that just got terminated was 
            //     on, by checking the handle value in the *pdata.
            // 2.  Re-arm the endpoint if desired (typically would be the case for OUT 
            //     endpoints).
            break;
        default:
            break;
    }      
    return TRUE; 
}
