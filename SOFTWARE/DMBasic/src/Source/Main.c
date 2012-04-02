/*****************************************************************************************************************************
Maximite

Main.c

Go to http://geoffg.net for updates, errata and helpfull notes
    
    Copyright 2011 Geoff Graham - http://geoffg.net
    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
    Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.  You should have received a copy of the GNU General Public License along with this program.
    If not, see <http://www.gnu.org/licenses/>.


This is the main source file for the Maximite project.
    
Development Environment
    To compile this you need:
     - Microchip MPLAB IDE V8.56 or later (www.microchip.com)
     - Microchip C32 C Compiler V1.11B Student (or Lite) version (www.microchip.com)
       IMPORTANT - This will not compile on versions earlier or later than 1.11B
       
If you are not using the MPLAB project file distributed with this source you must make sure that:
        In Configure -> Select Device...
              you have selected the correct processor type
    In Project -> Build Options... -> Project -> MPLAB PIC32 C Compiler
          you have defined the following symbol  PIC32MX795F512L_PIM
          you have defined either MAXIMITE or UBW32 to define the target platform
          and you have inserted -funsigned-char in the C compiler's command line (this will cause all chars to be unsigned)
    In Project -> Build Options... -> Project -> MPLAB PIC32 Linker
          you must set the Heap Size to 42000
          and the Min Stack Size to 6144
    In Project -> Build Options... -> Project -> Directories -> Include Search Files
          you must enter the following paths:
               .\Source
               .\Source\SDCard\Microchip\Include
               .\Source\USB
               .\Source\USB\Microchip\Include
    Finally, you must use the correct linker script to suit the target platform (Maximite or UBW32)

See the README.TXT for more information.

----------------------- PIC32 HARDWARE --------------------------
Interrupts.......................................................
PRIORITY     DESCRIPTION                 MAX SPEED
   7         Video horizontal sync       every 66uS
   6         Serial interface            every 17uS when open
   5         I2C interface			  	    ?
   4         USB polling		 every 100uS to 1mS
   3         PS2 keyboard clock          every 50 uS when sending
   2         Counting pins (11 to 14)    up to every 5uS
   1         MMBasic clocks and timers   every 1 mS
   
If you modify these priorities then you must also adjust the code of lower priority interrupts
to take into account that some thing like LATEbits.LATE4 = 1 is not an atomic operation and a 
higher interrupt might change PORTE while the macro us being executed.

Timers...........................................................
TIMER NBR    DESCRIPTION                 INTERRUPT
  core       Not Used                       no
   1		 USB polling                    yes
   2		 Sound							no
   3		 Video horizontal sync			yes
   4		 MMBasic clocks and timers		yes
   5		 Serial interface				yes
    
    
 ********************************************************************************************************************************/

#if !defined(MAXIMITE) && !defined(UBW32) && !defined(OLIMEX)
#error Must define either MAXIMITE or UBW32 in the project file
#endif
#if defined(MAXIMITE) && defined(UBW32)
#error Cannot define both MAXIMITE and UBW32 simultaneously
#endif

#include <p32xxxx.h>								// device specific defines
#include <plib.h>								// peripheral libraries
#include <stdlib.h>								// standard library functions
#include <string.h>                                                             // string functions

#define INCLUDE_FUNCTION_DEFINES

#include "Maximite.h"								// helpful defines
#ifdef MAXIMITE
#include "Configuration Bits.h"                                                 // config pragmas
#endif

#ifdef OLIMEX
#include "Configuration Bits.h"					// config pragmas
#endif

#include "IOPorts.h"								// helpful defines
#include "MMBasic/MMBasic.h"
#include "MMBasic/External.h"
#include "Video/Video.h"
#include "Keyboard/Keyboard.h"
#include "Timers/Timers.h"
#include "Serial/serial.h"
#include "Setup.h"

#ifdef OLIMEX
//#include "DuinoMite/Power.h"
#include "DuinoMite/RTC.h"
#include "DuinoMite/GameDuino.h"
#endif


/** USB INCLUDES *******************************************************/
#include "./USB/Microchip/Include/USB/usb.h"
#include "./USB/usb_config.h"
#include "./USB/Microchip/Include/USB/usb_device.h"
#include "./USB/Microchip/Include/USB/usb_function_cdc.h"
#include "./USB/Microchip/Include/USB/usb_function_msd.h"
#include "./SDCard/Microchip/Include/MDD File System/SD-SPI.h"
#include "./USB/HardwareProfile.h"
#include "./USB/Microchip/Include/GenericTypeDefs.h"
#include "./USB/Microchip/Include/Compiler.h"


//extern void USBDeviceTasks(void);




/*****************************************************************************************************************************
Configuration defines
 ******************************************************************************************************************************/
#define USB_RX_BUFFER_SIZE	128
#define USB_TX_BUFFER_SIZE	64



/*****************************************************************************************************************************
Other defines
 ******************************************************************************************************************************/
#ifdef MAXIMITE
#define MES_SIGNON  "\rMaximite BASIC Version " VERSION "\r\n"\
						"Copyright " YEAR " Geoff Graham\r\n" 
#endif
#ifdef UBW32
#define MES_SIGNON  "\rUBW32 MMBasic Version " VERSION "\r\n"\
						"Copyright " YEAR " Geoff Graham\r\n" 
#endif
#ifdef OLIMEX
#define MES_SIGNON  "\rDMBasic Build Date: " __DATE__ " Time:" __TIME__ "\r\n"\
		    "www.olimex.com, Based On MMBasic By Geoff Graham\r\n"
#endif
#define MES_EXCEPTION   "\rException code %d at 0x%X\r\n"\
			"An internal error was trapped (sorry).\r\n"\
			"Are you using the PEEK or POKE commands?\r\n\n"


/*****************************************************************************************************************************
Declare all functions
 ******************************************************************************************************************************/
void InitEverything(void);
int CopyDataToKeystrokeBuffer(int numBytes);


/*****************************************************************************************************************************
Global memory locations
 ******************************************************************************************************************************/
extern int VA[]; // pointer to the image buffer

volatile char USB_RxBuf[USB_RX_BUFFER_SIZE];
volatile char USB_TxBuf[2][USB_TX_BUFFER_SIZE];
volatile int USB_NbrCharsInTxBuf;
volatile int USB_Current_TxBuf;
volatile BYTE MSDStatus;
int USBOn, VideoOn; // variables controlling the display of the output

volatile unsigned char InpQueue[INP_QUEUE_SIZE]; // INP_QUEUE_SIZE is defined in Maximite.h
volatile int InpQueueHead, InpQueueTail;

char PrintSignonToUSB;
int Autorun;

volatile int MMAbort = false;
volatile int DisableMMAbort = false;
int FileXfr = false; // true if we are transfering a file
int SupressVideo = false; //dont print to video
unsigned int __attribute__((section(".grg"))) _excep_dummy;
unsigned int __attribute__((section(".grg"))) _excep_code;
unsigned int __attribute__((section(".grg"))) _excep_addr;
#if defined(__C30__) || defined(__C32__)
//The LUN variable definition is critical to the MSD function driver.  This
//  array is a structure of function pointers that are the functions that
//  will take care of each of the physical media.  For each additional LUN
//  that is added to the system, an entry into this array needs to be added
//  so that the stack can know where to find the physical layer functions.
//  In this example the media initialization function is named
//  "MediaInitialize", the read capacity function is named "ReadCapacity",
//  etc.
LUN_FUNCTIONS LUN[MAX_LUN + 1] =
{
    {
        &MDD_SDSPI_MediaInitialize,
        &MDD_SDSPI_ReadCapacity,
        &MDD_SDSPI_ReadSectorSize,
        &MDD_SDSPI_MediaDetect,
        &MDD_SDSPI_SectorRead,
        &MDD_SDSPI_WriteProtectState,
        &MDD_SDSPI_SectorWrite
    }
};
#endif

/* Standard Response to INQUIRY command stored in ROM 	*/
const ROM InquiryResponse inq_resp = {
	0x00,		// peripheral device is connected, direct access block device
	0x80,           // removable
	0x04,	 	// version = 00=> does not conform to any standard, 4=> SPC-2
	0x02,		// response is in format specified by SPC-2
	0x20,		// n-4 = 36-4=32= 0x20
	0x00,		// sccs etc.
	0x00,		// bque=1 and cmdque=0,indicates simple queueing 00 is obsolete,
			// but as in case of other device, we are just using 00
	0x00,		// 00 obsolete, 0x80 for basic task queueing
	{'O','l','i','m','e','x'
    },
	// this is the T10 assigned Vendor ID
	{'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' '
    },
	{'0','0','0','1'
    }
};
/****************************************************************************************************************************
Main program
 *****************************************************************************************************************************/
int main(void) {

    // initial setup of the I/O ports
    AD1PCFG = 0xFFFF; // Default all pins to digital
    mJTAGPortEnable(0); // turn off jtag
    //    TRISE = -1;
    //    LATE = 0;
    //    ODCE = 0;
    //    CNCON = 0;
    //    CNEN = 0;
    //    CNPUE = 0;

    // setup the CPU
    SYSTEMConfigPerformance(CLOCKFREQ); // System config performance
#ifdef MAXIMITE
    mOSCSetPBDIV(OSC_PB_DIV_1); // fix the peripheral bus to the main clock speed
#endif
#ifdef OLIMEX
    mOSCSetPBDIV(OSC_PB_DIV_1); // fix the peripheral bus to the main clock speed
    mOSCEnableSOSC();

    RtccInit();            // init the RTCC
    while(RtccGetClkStat()!=RTCC_CLK_ON);       // wait for the SOSC to be actually running and RTCC to have its clock source
#endif
    INTEnableSystemMultiVectoredInt(); // allow vectored interrupts
#ifdef OLIMEX
    TRISBbits.TRISB13 = 0;
    ODCBbits.ODCB13 = 1;
    PORTBbits.RB13 = 0;
    TRISBbits.TRISB11 = 1; //make sure vga/composite select is input
#endif

    LoadSetup(); //load setup from flash
                 // init global variables
    USB_NbrCharsInTxBuf = 0;
    USB_Current_TxBuf = 0;
    InpQueueHead = InpQueueTail = 0;

    mSec(50); //delay to let power settle on mega

    if(S.UsbEnable) USBOn = true;
    VideoOn = S.VideoMode;

    initKeyboard(); // initilise and startup the keyboard routines
    initTimers(); // initilise and startup the timers
    initExtIO(); // Initialise the external analog and digital I/O
    if(S.GameDuino) GDInit();   //init gameduino

    mSec(50); //delay to let power settle on mega

    if (USBOn) {
        // initilise the USB input/output subsystems
        USBDeviceInit(); // Initialise USB module SFRs and firmware
          // setup timer 1 to generate a regular interrupt to process any USB activity
        PR1 = 1000 * ((BUSFREQ / 8) / 1000000) - 1; // nothing is connected to the USB (poll every 1 mSec)
        //PR1 = 100 * ((BUSFREQ / 2) / 1000000) - 1; // start polling at 100 uSec.  the interrupt will adjust this
        T1CON = 0x8010; // T1 on, prescaler 1:8
        mT1SetIntPriority(4); // medium priority
        mT1ClearIntFlag(); // clear interrupt flag
        mT1IntEnable(1); // enable interrupt
    }
        MMcls(); // clear the video buffer
        initFont(); // initialise the font table
    if (VideoOn ) {
        initVideo(); // start the video state machine
    }
    else
    {
            VRes = 432;
	    HRes = 480;
    }

    if (S.SerialCon == 1) {
        sprintf(inpbuf,"COM1:%d",S.BaudRate);
        SerialOpen(inpbuf, 1);
    }
    if (S.SerialCon == 2) {
        sprintf(inpbuf,"COM2:%d",S.BaudRate);
        SerialOpen(inpbuf, 1);
    }

    if (S.SerialCon == 3) {
        sprintf(inpbuf,"COM3:%d",S.BaudRate);
        SerialOpen(inpbuf, 1);
    }
    if (S.SerialCon == 4) {
        sprintf(inpbuf,"COM4:%d",S.BaudRate);
        SerialOpen(inpbuf, 1);
    }



        SoundPlay = 0; // start by not playing a sound
    P_SOUND_TRIS = P_OUTPUT; // set the sound pin as an output

    while (PauseTimer < 1000) MMInkey(); // let everything settle and consume any junk in the input buffer
    PrintSignonToUSB = true; // show that (so far) no one has used the keyboard

    // Initial message
    if (RCON & 0x0040) { // this will only happen if we recoverd from an untrapped exception
        char tmp[150]; // buffer for building the error message
        sprintf(tmp, MES_EXCEPTION, _excep_code, _excep_addr); // error msg
        MMPrintString(tmp); // and print it
        Autorun = false;
    } else {
        MMPrintString(MES_SIGNON); // print signon message
        MMPrintString("\r\n"); // extra line looks better
        Autorun = true;
    }

   MMBasicStart(); // run BASIC
}




/****************************************************************************************************************************
USB related functions
 *****************************************************************************************************************************/
/******************************************************************************
 * Function:        void mySetLineCodingHandler(void)
 *
 * PreCondition:    USB_CDC_SET_LINE_CODING_HANDLER is defined
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function gets called when a SetLineCoding command
 *                  is sent on the bus.  This function will evaluate the request
 *                  and determine if the application should update the baudrate
 *                  or not.
 *
 * Note:
 *
 *****************************************************************************/
#if defined(USB_CDC_SET_LINE_CODING_HANDLER)
void mySetLineCodingHandler(void)
{
    //If the request is not in a valid range
    if(cdc_notice.GetLineCoding.dwDTERate.Val > 115200)
    {
        //NOTE: There are two ways that an unsupported baud rate could be
        //handled.  The first is just to ignore the request and don't change
        //the values.  That is what is currently implemented in this function.
        //The second possible method is to stall the STATUS stage of the request.
        //STALLing the STATUS stage will cause an exception to be thrown in the
        //requesting application.  Some programs, like HyperTerminal, handle the
        //exception properly and give a pop-up box indicating that the request
        //settings are not valid.  Any application that does not handle the
        //exception correctly will likely crash when this requiest fails.  For
        //the sake of example the code required to STALL the status stage of the
        //request is provided below.  It has been left out so that this demo
        //does not cause applications without the required exception handling
        //to crash.
        //---------------------------------------
        //USBStallEndpoint(0,1);
    }
    else
    {
        //DWORD_VAL dwBaud;

        //Update the baudrate info in the CDC driver
        CDCSetBaudRate(cdc_notice.GetLineCoding.dwDTERate.Val);

        //Update the baudrate of the UART
//        #if defined(__18CXX)
//            dwBaud.Val = (GetSystemClock()/4)/line_coding.dwDTERate.Val-1;
//            SPBRG = dwBaud.v[0];
//            SPBRGH = dwBaud.v[1];
//        #elif defined(__C30__)
//            dwBaud.Val = (((GetPeripheralClock()/2)+(BRG_DIV2/2*line_coding.dwDTERate.Val))/BRG_DIV2/line_coding.dwDTERate.Val-1);
//            U2BRG = dwBaud.Val;
//        #elif defined(__C32__)
//            U2BRG = ((GetPeripheralClock()+(BRG_DIV2/2*line_coding.dwDTERate.Val))/BRG_DIV2/line_coding.dwDTERate.Val-1);
//            //U2MODE = 0;
//            U2MODEbits.BRGH = BRGH2;
//            //U2STA = 0;
//        #endif
    }
}
#endif
/******************************************************************************************
Timer 1 interrupt.
Used to send and get data to or from the USB interface.
 *******************************************************************************************/
void __ISR(_TIMER_1_VECTOR, ipl4) T1Interrupt(void) {
    int i, numBytesRead;

    if(S.UsbEnable ==1){
    if (U1OTGSTAT & 1) { // is there 5V on the USB?
        USBDeviceTasks(); // do any USB work
        if (USBGetDeviceState() == DETACHED_STATE) // 5V on the USB but nothing happening
            PR1 = 500 * ((BUSFREQ / 8) / 1000000) - 1; // probably using USB power only (poll every 500 uSec)
        else if (USBGetDeviceState() != CONFIGURED_STATE) // we are enumerating with the host
            PR1 = 75 * ((BUSFREQ / 8) / 1000000) - 1; // maximum speed while we are enumerating (poll every 75 uSec)
        else { // we must have finished enumerating
            // at this point we are connected and have enumerated - we can now send and receive data
            PR1 = 250 * ((BUSFREQ / 8) / 1000000) - 1; // slow speed is only needed (poll every 250 uSec)

            numBytesRead = getsUSBUSART(USB_RxBuf, USB_RX_BUFFER_SIZE); // check for data to be read
            for (i = 0; i < numBytesRead; i++) { // if we have some data, copy it into the keyboard buffer
                InpQueue[InpQueueHead] = USB_RxBuf[i]; // add the byte in the keystroke buffer
                InpQueueHead = (InpQueueHead + 1) % INP_QUEUE_SIZE; // increment the head of the queue
                if (USB_RxBuf[i] == 3 && !FileXfr) // check for CTRL-C
                    MMAbort = true; // and if so tell BASIC to stop running
                if (PrintSignonToUSB) {
                    strncpy(USB_TxBuf[USB_Current_TxBuf], MES_SIGNON, USB_TX_BUFFER_SIZE); // if first time send signon msg
                    USB_NbrCharsInTxBuf = strlen(USB_TxBuf[USB_Current_TxBuf]);
                    PrintSignonToUSB = false;
                }
            }

            if (USB_NbrCharsInTxBuf && mUSBUSARTIsTxTrfReady()) { // next, check for data to be sent
                putUSBUSART(USB_TxBuf[USB_Current_TxBuf], USB_NbrCharsInTxBuf); // and send it
                USB_Current_TxBuf = !USB_Current_TxBuf;
                USB_NbrCharsInTxBuf = 0;
            }
            CDCTxService(); // send anything that needed sending
        }
    } else
        PR1 = 1000 * ((BUSFREQ / 8) / 1000000) - 1; // nothing is connected to the USB (poll every 1 mSec)
    }
    if(S.UsbEnable ==2){
    }
    MSDStatus=MSDTasks();
    mT1ClearIntFlag(); // Clear the interrupt flag
            
}

/******************************************************************************************
BOOL USER_USB_CALLBACK_EVENT_HANDLER
This function is called from the USB stack to notify a user application that a USB event
occured.  This callback is in interrupt context when the USB_INTERRUPT option is selected.

Args:  event - the type of event 
 *pdata - pointer to the event data
       size - size of the event data
       
This function was derived from the demo CDC program provided by Microchip
 *******************************************************************************************/
/*
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size) {
    switch (event) {
        case EVENT_CONFIGURED:
            CDCInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            break;
        case EVENT_EP0_REQUEST:
            USBCheckCDCRequest();
            break;
        case EVENT_SOF:
            break;
        case EVENT_SUSPEND:
            break;
        case EVENT_RESUME:
            break;
        case EVENT_BUS_ERROR:
            break;
        case EVENT_TRANSFER:
            Nop();
            break;
        default:
            break;
    }
    return TRUE;
}
*/

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
    #if 0
        U1EIR = 0xFFFF;
        U1IR = 0xFFFF;
        U1OTGIR = 0xFFFF;
        IFS5bits.USB1IF = 0;
        IEC5bits.USB1IE = 1;
        U1OTGIEbits.ACTVIE = 1;
        U1OTGIRbits.ACTVIF = 1;
        Sleep();
    #endif
    #endif
}


/******************************************************************************
 * Function:        void _USB1Interrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the USB interrupt bit is set
 *					In this example the interrupt is only used when the device
 *					goes to sleep when it receives a USB suspend command
 *
 * Note:            None
 *****************************************************************************/
#if 0
void __attribute__ ((interrupt)) _USB1Interrupt(void)
{
    #if !defined(self_powered)
        if(U1OTGIRbits.ACTVIF)
        {
            IEC5bits.USB1IE = 0;
            U1OTGIEbits.ACTVIE = 0;
            IFS5bits.USB1IF = 0;

            //USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
            USBClearInterruptFlag(USBIdleIFReg,USBIdleIFBitNum);
            //USBSuspendControl = 0;
        }
    #endif
}
#endif

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
    USBCheckMSDRequest();
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
    #if (MSD_DATA_IN_EP == MSD_DATA_OUT_EP)
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    #else
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
        USBEnableEndpoint(MSD_DATA_OUT_EP,USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    #endif

    USBMSDInit();
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
    switch(event)
    {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
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
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT
            //      endpoints).
            break;
        default:
            break;
    }
    return TRUE;
}

/****************************************************************************************************************************
Keyboard/USB input functions
 *****************************************************************************************************************************/

int MMInkeyTask(void) {
    unsigned int c = -1; // default no character
    if (InpQueueHead != InpQueueTail) { // is there anything in the keyboard queue?
        c = InpQueue[InpQueueTail]; // if so, get it
        InpQueueTail = (InpQueueTail + 1) % INP_QUEUE_SIZE; // and remove from the queue
    } else if (SerialConsole) { // if there is a serial console
        c = SerialGetchar(SerialConsole); // get the char from the serial port (returns -1 if nothing)
    }
    if (MMAbort) longjmp(mark, 1); // jump back to the input prompt
    return c;
}


/*****************************************************************************************
The vt100 escape code sequences
===============================
3 char codes			Arrow Up	esc [ A
                                                Arrow Down	esc [ B
                                                Arrow Right	esc [ C
                                                Arrow Left	esc [ D
						
4 char codes			Insert		esc [ 1 ~
                                                Home		esc [ 2 ~
                                                End			esc [ 4 ~
                                                Page Up		esc [ 5 ~
                                                Page Down	esc [ 6 ~
						
5 char codes                                    F1			esc [ 1 1 ~
                                                F2			esc [ 1 2 ~
                                                F3			esc [ 1 3 ~
                                                F4			esc [ 1 4 ~
                                                F5			esc [ 1 5 ~			note the
                                                F6			esc [ 1 7 ~			disconnect
                                                F7			esc [ 1 8 ~
                                                F8			esc [ 1 9 ~
                                                F9			esc [ 2 0 ~
                                                F10			esc [ 2 2 ~			note the
                                                F11			esc [ 2 3 ~			disconnect
                                                F12			esc [ 2 4 ~

 *****************************************************************************************/

// check if there is a keystroke waiting in the buffer and, if so, return with the char
// returns -1 if no char waiting
// the main work is to check for vt100 escape code sequences and map to Maximite codes

int MMInkey(void) {
    unsigned int c = -1; // default no character
    unsigned int tc = -1; // default no character
    unsigned int ttc = -1; // default no character
    static unsigned int c1 = -1;
    static unsigned int c2 = -1;
    static unsigned int c3 = -1;
    static unsigned int c4 = -1;

    if (c1 != -1) { // check if there are discarded chars from a previous sequence
        c = c1;
        c1 = c2;
        c2 = c3;
        c3 = c4;
        c4 = -1; // shuffle the queue down
        return c; // and return the head of the queue
    }

    c = MMInkeyTask(); // do discarded chars so get the char
    if (c == 0x1b && !FileXfr) {
        InkeyTimer = 0; // start the timer
        while (InkeyTimer < 20); // delay 20mS to allow the next char to arrive
        c = MMInkeyTask(); // get the second char
        if (c != '[') {
            c1 = c;
            return 0x1b;
        } // must be a square bracket
        while (InkeyTimer < 30); // delay some more to allow the final chars to arrive, even at 1200 baud
        c = MMInkeyTask(); // get the third char
        if (c == 'A') return UP; // the arrow keys are three chars
        if (c == 'B') return DOWN;
        if (c == 'C') return RIGHT;
        if (c == 'D') return LEFT;
        if (c < '1' && c > '6') {
            c1 = '[';
            c2 = c;
            return 0x1b;
        } // the 3rd char must be in this range
        tc = MMInkeyTask();
        if (tc == '~') { // all 4 char codes must be terminated with ~
            if (c == '1') return HOME;
            if (c == '2') return INSERT;
            if (c == '3') return DEL;
            if (c == '4') return END;
            if (c == '5') return PUP;
            if (c == '6') return PDOWN;
            c1 = '[';
            c2 = c;
            c3 = tc;
            return 0x1b; // not a valid 4 char code
        }
        ttc = MMInkeyTask(); // get the 5th char
        if (ttc == '~') { // must be a ~
            if (c == '1') {
                if (tc >= '1' && tc <= '5') return F1 + (tc - '1'); // F1 to F5
                if (tc >= '7' && tc <= '9') return F6 + (tc - '7'); // F6 to F8
            }
            if (c == '2') {
                if (tc == '0' || tc == '1') return F9 + (tc - '0'); // F9 and F10
                if (tc == '3' || tc == '4') return F11 + (tc - '3'); // F11 and F12
            }
        }
        // nothing worked so bomb out
        c1 = '[';
        c2 = c;
        c3 = tc;
        c4 = ttc;
        return 0x1b;
    }
    return c;
}


// get a keystroke.  Will wait forever for input
// if the char is a cr then replace it with a newline (lf)

int MMgetchar(void) {
    int c;

    do {
        ShowCursor(true);
        c = MMInkey();
        if (c == '\r') c = '\n';
    } while (c == -1);
    ShowCursor(false);
    return c;
}





/****************************************************************************************************************************
Video/USB output functions
 *****************************************************************************************************************************/
// put a character out to the video screen, the serial console and the USB interface

char MMputchar(char c) {
//    if (!FileXfr && VideoOn) VideoPutc(c); // draw the char on the video screen
    if (!FileXfr && !SupressVideo) VideoPutc(c); // draw the char on the video screen
    if (SerialConsole) SerialPutchar(SerialConsole, c); // send it to the serial console if enabled
    if (!(SerialConsole && FileXfr) && USBOn) USBPutchar(c); // send it to the USB
    return c;
}

void USBPutchar(char c) {
    static int DelayCnt = 0;

    if ((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1)) {// check USB status
        USB_NbrCharsInTxBuf = 0;
        DelayCnt = 0;
        return; // and skip if the USB is not connected
    }

    // if the buffer is full delay for a maximum of 5mS (at level 2 optimisation)
    // this will only delay once on buffer full and the delay will only be re enabled when something is sent
    while ((USB_NbrCharsInTxBuf >= USB_TX_BUFFER_SIZE) && DelayCnt < 57000) DelayCnt++;

    if (USB_NbrCharsInTxBuf < USB_TX_BUFFER_SIZE) { // skip if the buffer is still full (not being drained)
        mT1IntEnable(0); // Disable Timer1 Interrupt
        USB_TxBuf[USB_Current_TxBuf][USB_NbrCharsInTxBuf++] = c; // Place char into the buffer
        mT1IntEnable(1); // Enable Timer3 Interrupt
        DelayCnt = 0;
    }
}


// print a string to the video and USB interface

void MMPrintString(char* s) {
    while (*s) MMputchar(*s++);
}




/****************************************************************************************************************************
Utility functions
 *****************************************************************************************************************************/


// trap an unhandled error (exception)

/* list of exception codes
        0 = EXCEP_IRQ,				// interrupt
        4 = EXCEP_AdEL,				// address error exception (load or ifetch)
        5 = EXCEP_AdES,				// address error exception (store)
        6 = EXCEP_IBE,				// bus error (ifetch)
        7 = EXCEP_DBE,				// bus error (load/store)
        8 = EXCEP_Sys,				// syscall
        9 = EXCEP_Bp,				// breakpoint
        10 = EXCEP_RI,				// reserved instruction
        11 = EXCEP_CpU,				// coprocessor unusable
        12 = EXCEP_Overflow,		// arithmetic overflow
        13 = EXCEP_Trap,			// trap (possible divide by zero)
        14 = EXCEP_IS1 = 16,		// implementation specfic 1
        15 = EXCEP_CEU,				// CorExtend Unuseable
        16 = EXCEP_C2E				// coprocessor 2
 */

/* exception handler
   this function overrides the normal _weak_ generic handler and uses two variables (_excep_code and _excep_addr) to save the cause 
   and address of the exception.  These are saved in a special section of memory which is not initialised by the startup code.  
   For details see the following:
                        http://www.microchip.com/forums/tm.aspx?m=434737&mpage=1&key=%F1%AA%88%B1
                        http://www.microchip.com/forums/tm.aspx?m=458360&high=persist
   			
    This technique requires the following to be inserted in the linker script just after the bss section and before the heap
    section:
         .grg :
                   {
 *(.grg)
                   }

        Then, in the global section of the program define the following variables:
                unsigned int __attribute__((section(".grg"))) _excep_dummy;
                unsigned int __attribute__((section(".grg"))) _excep_code;
                unsigned int __attribute__((section(".grg"))) _excep_addr;
		
        For some reason the startup routines still zero the first integer in this section and that is the reason for the dummy
        variable.  However _excep_code and _excep_addr survive the restart and their values can be displayed to give a clue as
        to why and where the exception occured.
 */

void _general_exception_handler(void) {
#ifdef MAXIMITE
    P_LED_OUT = 0; // this is the LED
#endif
#ifdef UBW32
    P_LED_OUT = 1; // this is the LED - the UBW32 wired them upside down !!
#endif
#ifdef OLIMEX
    P_LED_OUT = 0; // this is the LED
#endif
    asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
    asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));

    _excep_code = (_excep_code & 0x0000007C) >> 2;

    debughalt(); // Break here when compiling under a Debug build configuration
    SoftReset(); // this will restart the processor – only works when not in debug
}



#ifdef __DEBUG

void dump(char *p, int nbr) {
    char buf1[60], buf2[30], *b1, *b2;
    b1 = buf1;
    b2 = buf2;
    b1 += sprintf(b1, "%8x: ", (unsigned int) p);
    while (nbr > 0) {
        b1 += sprintf(b1, "%02x ", *p);
        b2 += sprintf(b2, "%c", (*p >= ' ' && *p < 0x7f) ? *p : ' ');
        p++;
        nbr--;
        if ((unsigned int) p % 16 == 0) {
            sprintf(inpbuf, "%s   %s", buf1, buf2);
            MMPrintString(inpbuf);
            b1 = buf1;
            b2 = buf2;
            b1 += sprintf(b1, "\r\n%8x: ", (unsigned int) p);
        }
    }
    if (b2 != buf2) {
        sprintf(inpbuf, "%s   %s", buf1, buf2);
        MMPrintString(inpbuf);
    }
    MMPrintString("\r\n");
}
#endif



#ifdef PROFILE
int p_enabled = false;
unsigned int *p_array;

void StartProfiling(void) {
    int i;

    p_array = (unsigned int *) getmemory(((P_END_ADDR - P_START_ADDR) / P_GRANUALITY) * 4);
    for (i = 0; i < (P_END_ADDR - P_START_ADDR) / P_GRANUALITY; i++) p_array[i] = 0;
    p_enabled = true;

    PR5 = 10 * ((BUSFREQ) / 1000000) - 1; // ticks at 10us
    T5CON = 0x8000; // T4 on, prescaler 1:1
    mT5SetIntPriority(6); // a reasonably high priority
    mT5ClearIntFlag(); // clear interrupt flag
    mT5IntEnable(1); // enable interrupt
}

void __ISR(_TIMER_5_VECTOR, ipl6) T5Interrupt(void) {
    unsigned int pc;
    register unsigned int *msp asm("sp");

    // note the magic number 44 is found in the disassembly of this interrupt, look for the lines:
    //   lw          k0,44(sp)
    //      - - - - - - -
    //   mtc0        k0,EPC
    // the magic number is the 44 (or whatever)
    pc = msp[44 / 4];
    pc -= P_START_ADDR;
    pc /= P_GRANUALITY;
    if (pc < (P_END_ADDR - P_START_ADDR) / P_GRANUALITY) p_array[pc]++;
    mT5ClearIntFlag();
}

void StopProfiling(void) {
    int filenbr, i;
    char *p;
    char fn[] = "B:PROFILE.XLS";

    if (!p_enabled) return;
    p_enabled = false;
    mT5IntEnable(0);
    T5CON = 0;

    if ((filenbr = FindFreeFileNbr()) == 0) error("Too many files open");
    MMfopen(fn, "w", filenbr);
    for (i = 0; i < (P_END_ADDR - P_START_ADDR) / P_GRANUALITY; i++) {
        sprintf(inpbuf, "0x%x\t%u\r\n", (i * P_GRANUALITY) + P_START_ADDR, p_array[i]);
        p = inpbuf;
        while (*p) MMfputc(*p++, filenbr);
    }
    MMfclose(filenbr);

    free(p_array);
    HeapUsed -= ((P_END_ADDR - P_START_ADDR) / P_GRANUALITY) * 4;
}

#endif

