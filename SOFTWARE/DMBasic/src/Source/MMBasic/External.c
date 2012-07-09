/***********************************************************************************************************************
Maximite

External.c

Handles reading and writing to the digital and analog input/output pins ising the SETPIN and PIN commands

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/

#include <p32xxxx.h>								// device specific defines
#include <plib.h>									// peripheral libraries

#define INCLUDE_FUNCTION_DEFINES
#include "Maximite.h"
#include "../IOPorts.h"
#include "MMBasic.h"
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"
#include "../Timers/Timers.h"
#include "../Serial/Serial.h"
//	GS I2C Start
#include "I2C.h"
//	GS I2C End
#ifdef OLIMEX
	#include "Setup.h"
	#include "DuinoMite/RTC.h"
#endif

int ExtCurrentConfig[NBRPINS + 1];

int INT1Count, INT1Value;
int INT2Count, INT2Value;
int INT3Count, INT3Value;
int INT4Count, INT4Value;

int Average[6];  // average count for a/d KSD setpin 1,1,0 disables averaging and returns raw value



/*******************************************************************************************
External I/O related commands in MMBasic
========================================
These are the functions responsible for executing the ext I/O related  commands in MMBasic
They are supported by utility functions that are grouped at the end of this file

Each function is responsible for decoding a command
all function names are in the form cmd_xxxx() (for a basic command) or fun_xxxx() (for a
basic function) so, if you want to search for the function responsible for the LOCATE command
look for cmd_name

There are 4 items of information that are setup before the command is run.
All these are globals.

int cmdtoken	This is the token number of the command (some commands can handle multiple
				statement types and this helps them differentiate)

char *cmdline	This is the command line terminated with a zero char and trimmed of leading
				spaces.  It may exist anywhere in memory (or even ROM).

char *nextstmt	This is a pointer to the next statement to be executed.  The only thing a
				y=spi(1,2,3command can do with it is save it or change it to some other location.

int CurrentLineNbr  This is read only and is set to zero if the command is in immediate mode.

The only actions a command can do to change the program flow is to change nextstmt or
execute longjmp(mark, 1) if it wants to abort the program.

********************************************************************************************/



// this is invoked as a command (ie, pin(3) = 1)
// first get the argument then step over the closing bracket.  Search through the rest of the command line looking
// for the equals sign and step over it, evaluate the rest of the command and set the pin accordingly
void cmd_pin(void) {
	int pin, value;

	pin = getinteger(cmdline);
	if (ExtCurrentConfig[pin] >= EXT_COM_RESERVED)  error("Pin is allocated to a communications function");
	while (*cmdline && tokenfunction(*cmdline) != op_equal) cmdline++;
	if (!*cmdline) error("Invalid syntax");
	++cmdline;
	if (!*cmdline) error("Invalid syntax");
	value = getinteger(cmdline);
	if (!ExtSet(pin, value)) error("Invalid pin number");
}


// this is invoked as a function (ie, x = pin(3) )
void fun_pin(void) {
	// these two defines control the averaging of analog samples.  ANA_AVERAGE is the total number of samples to take
	// while ANA_DISCARD is the the number of highest value samples to discard and the same for the lowest samples
	// this leaves us with ANA_AVERAGE - ANA_DISCARD*2 samples which are averaged to give the final value
	#define ANA_AVERAGE		10
	#define ANA_DISCARD		2

	int pin, value, i, j, b[ANA_AVERAGE];
	float t;

	pin = getinteger(ep);
	if (pin < 0 || pin > NBRPINS) error("Invalid pin number");
	if (!ExtInp(pin, &value)) error("Pin not configured for input");
  if (ExtCurrentConfig[pin] == EXT_DIG_IN || ExtCurrentConfig[pin] == EXT_INT_HI || ExtCurrentConfig[pin] == EXT_INT_LO || ExtCurrentConfig[pin] == EXT_OC_OUT) {
		if(pin == 0) fret = (float)(value?0:1);		// invert if we are reading the pushbotton
		else fret = (float)value;					// all other inputs report as normal
	} else if (ExtCurrentConfig[pin] == EXT_ANA_IN) {
            if(Average[pin] !=0 || pin==21){
            // for analog we take ANA_AVERAGE readings and sort them into descending order in buffer b[].
                b[0] = value;							// this reading was taken earlier
		//dp("%d", b[0]);
                for (i = 1; i < ANA_AVERAGE; i++) {                             // so this loop uses one less
			ExtInp(pin, &b[i]);					// get the value
			//dp("%d", b[i]);
			for (j = i; j > 0; j--) {				// and sort into position
				if (b[j - 1] < b[j]) {
					t = b[j - 1];
					b[j - 1] = b[j];
					b[j] = t;
				} else break;
			}
		}
		//dp("%d  %d  %d  %d  %d  %d  %d  %d  %d  %d", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9]);
		// we then discard the top ANA_DISCARD samples and the bottom ANA_DISCARD samples and add up the remainder
		for (j = 0, i = ANA_DISCARD; i < ANA_AVERAGE - ANA_DISCARD; i++) j += b[i];
		// the total is averaged and scaled
                fret = ((float)j * 3.3) / (float)(1024 * (ANA_AVERAGE - ANA_DISCARD*2));
            }
            else {
                ExtInp(pin,&b[0]);
                fret=((float)b[0]);
            }
          } else if (ExtCurrentConfig[pin] == EXT_FREQ_IN || ExtCurrentConfig[pin] == EXT_PER_IN || ExtCurrentConfig[pin] == EXT_CNT_IN) {
		fret = (float)value;									// all other inputs report as normal
	} else error("Input mode not implemented");
}


void cmd_setpin(void) {
	int pin, value, line;

	getargs(&cmdline, 5, ",");
	if (argc%2 == 0 || argc == 0) error("Invalid syntax");
	pin = getinteger(argv[0]);
	if (pin < 0 || pin > NBRPINS) error("Invalid pin number");
	if (ExtCurrentConfig[pin] >= EXT_COM_RESERVED)  error("Pin is allocated to a communications function");
        Average[pin]=1; // set default average
        value = getinteger(argv[2]);
        if( argc==5 && value == EXT_ANA_IN ) Average[pin] = getinteger(argv[4]); //added KSD
        if (!ExtCfg(pin, value)) error( "Invalid configuration value" );
	if (value == EXT_INT_HI || value == EXT_INT_LO) {
		// we need to set up a software interrupt
		//if (pin == 0) error("Invalid pin number");
		if (argc != 5) error("Line number for the interrupt is not specified");
		line = getinteger(argv[4]);					// get the line number
		inttbl[pin].intp = findline(line, true);                        // and set
		ExtInp(pin, &inttbl[pin].last);					// save the current pin value for the first test
		inttbl[pin].lohi = (value == EXT_INT_HI);                       // and set trigger polarity
	} else if (argc > 3 && value !=EXT_ANA_IN) error("Line number is only used for interrupts");
}


/*******************************************************************************************
********************************************************************************************

Utility routines for the external I/O commands and functions in MMBasic

********************************************************************************************
********************************************************************************************/


void ClearExternalIO(void) {
	int i;

//	GS I2C Start
	i2c_disable();
	i2c_slave_disable();
//	GS I2C End

	if (SerialConsole != 1) SerialClose(1);
	if (SerialConsole != 2) SerialClose(2);
	if (SerialConsole != 3) SerialClose(3);
	if (SerialConsole != 4) SerialClose(4);

	// stop the sound
	SoundPlay = 0;
	CloseTimer2();
#ifdef OLIMEX
	CloseOC1();
#else
	CloseOC2();
#endif
	inttbl[NBRPINS + 2].intp = NULL;					// disable the tick interrupt
	for (i = 1; i < NBRPINS + 1; i++) {
		inttbl[i].intp = NULL;						// disable all interrupts
#ifdef OLIMEX
    #ifdef  OLIMEX_DUINOMITE_EMEGA
		if (ExtCurrentConfig[i] != EXT_CONSOLE_RESERVED && i != 35 ) {	// don't reset the serial console
    #else
		if (ExtCurrentConfig[i] != EXT_CONSOLE_RESERVED && i != 21 ) {	// don't reset the serial console
    #endif
#else
		if (ExtCurrentConfig[i] != EXT_CONSOLE_RESERVED) {              // don't reset the serial console
#endif
			ExtCfg(i, EXT_NOT_CONFIG);                              // all set to unconfigured
			ExtSet(i, 0);						// all outputs (when set) default to low
		}
	}
        InterruptReturn = NULL;
}


/****************************************************************************************************************************
Initialise the I/O pins
*****************************************************************************************************************************/
void initExtIO(void) {
	int i;

	for (i = 1; i < NBRPINS + 1; i++) {
		ExtCfg(i, EXT_NOT_CONFIG);                                      // all set to unconfigured
		ExtSet(i, 0);							// all outputs (when set) default to low
	}
#ifndef OLIMEX
 	CNCONbits.ON = 1;       						// turn on Change Notification module
 	CNPUEbits.CNPUE1 = 1;							// turn on the pullup for pin C13 also called CN1
#endif
	ExtCurrentConfig[0] = EXT_DIG_IN;                                       // and show that we can read from it
	P_LED_TRIS = P_OUTPUT; 							// make the LED pin an output
	ExtSet(0, 0);								// and turn it off
#ifdef OLIMEX
    #ifdef OLIMEX_DUINOMITE_EMEGA
	ExtCurrentConfig[35] = EXT_ANA_IN;
    #else
	ExtCurrentConfig[21] = EXT_ANA_IN;
    #endif
#endif

	// setup the analog (ADC) function
	AD1CON1 = 0x00E0;       						// automatic conversion after sampling
 	AD1CSSL = 0;       							// no scanning required
	AD1CON2 = 0;       							// use MUXA, use AVdd   &   AVss as Vref+/-
	AD1CON3 = 0x203; //0x1F3F;  							// Tsamp = 32 x Tad;
	AD1CON1bits.ADON = 1; 							// turn on the ADC

}


/****************************************************************************************************************************
Configure an I/O pin
Returns true if all is OK
*****************************************************************************************************************************/
int ExtCfg(int pin, int cfg) {
	int tris, ana, oc;

	if (pin < 0 || pin > NBRPINS) return false;						// initial sanity check

	// make sure that interrupts are disabled in case we are changing from an interrupt input
#ifdef OLIMEX
	if (pin == 5) ConfigINT2(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
	if (pin == 6) ConfigINT3(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
        #ifdef  OLIMEX_DUINOMITE_EMEGA
        #else
                if (pin == 7) ConfigINT4(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
        #endif
#else
	if (pin == 11) ConfigINT1(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
	if (pin == 12) ConfigINT2(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
	if (pin == 13) ConfigINT3(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
	if (pin == 14) ConfigINT4(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_DISABLE);
#endif

	inttbl[pin].intp = NULL;		// also disable a software interrupt on this pin

	switch (cfg) {
		case EXT_NOT_CONFIG:
	#ifdef OLIMEX
			if (pin == 5) {P_E5_TRIS = 1; P_E5_OC = 1;}
			if (pin == 6) {P_E6_TRIS = 1; P_E6_OC = 1;}
			if (pin == 11) {P_E11_TRIS = 1; P_E11_OC = 1;}
			if (pin == 12) {P_E12_TRIS = 1; P_E12_OC = 1;}
	#endif
			tris = 1; ana = 1; oc = 1;
			break;

		case EXT_ANA_IN:
	#ifdef OLIMEX
			if (pin > 6 && pin < 19) return false;
			if (pin == 5) {P_E5_TRIS = 1; P_E5_OC = 1;}
			if (pin == 6) {P_E6_TRIS = 1; P_E6_OC = 1;}
	#else
			if (pin > 10) return false;
	#endif
			tris = 1; ana = 0; oc = 1;
			break;

		case EXT_FREQ_IN:											// same as counting, so fall through
		case EXT_PER_IN:											// same as counting, so fall through
		case EXT_CNT_IN:
	#ifdef OLIMEX
	#else
			if (pin == 11) {
				INT1Count = INT1Value = 0;
				ConfigINT1(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_ENABLE);
				tris = 1; ana = 1; oc = 1;
 			}
	#endif
	#ifdef OLIMEX
			if (pin == 5) {
				P_E5_TRIS = 1;
				P_E5_OC = 1;
	#else
			if (pin == 12) {
	#endif
				INT2Count = INT2Value = 0;
				ConfigINT2(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_ENABLE);
				tris = 1; ana = 1; oc = 1;
				break;
			}
	#ifdef OLIMEX
			if (pin == 6) {
				P_E6_TRIS = 1;
				P_E6_OC = 1;
	#else
			if (pin == 13) {
	#endif
				INT3Count = INT3Value = 0;
				ConfigINT3(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_ENABLE);
				tris = 1; ana = 1; oc = 1;
				break;
			}
#ifdef  OLIMEX_DUINOMITE_EMEGA
                    break;
#else
	#ifdef OLIMEX
			if (pin == 7) {
	#else
			if (pin == 14) {
	#endif
				INT4Count = INT4Value = 0;
				ConfigINT4(EXT_INT_PRI_2 | RISING_EDGE_INT | EXT_INT_ENABLE);
				tris = 1; ana = 1; oc = 1;
				break;
			}
			return false;							// not an interrupt enabled pin
#endif

		case EXT_INT_LO:											// same as digital input, so fall through
		case EXT_INT_HI:											// same as digital input, so fall through
		case EXT_DIG_IN:
	#ifdef OLIMEX
                        if (pin == 5) {P_E5_TRIS = 1; P_E5_OC = 1;}
			if (pin == 6) {P_E6_TRIS = 1; P_E6_OC = 1;}
			if (pin == 11) {P_E11_TRIS = 1; P_E11_OC = 1;}
			if (pin == 12) {P_E12_TRIS = 1; P_E12_OC = 1;}
	#endif
			tris = 1; ana = 1; oc = 1;
			break;

		case EXT_DIG_OUT:
	#ifdef OLIMEX
			if (pin == 11) {P_E11_TRIS = 1; P_E11_OC = 1;} //return false;
			if (pin == 5) {P_E5_TRIS = 1; P_E5_OC = 1;}
			if (pin == 6) {P_E6_TRIS = 1; P_E6_OC = 1;}
			if (pin == 12) {P_E12_TRIS = 1; P_E12_OC = 1;}
	#endif
			tris = 0; ana = 1; oc = 0;
			break;

		case EXT_OC_OUT:
	#ifdef OLIMEX
			if (pin == 11) {P_E11_TRIS = 1; P_E11_OC = 1;} //return false;
			if (pin == 5) {P_E5_TRIS = 1; P_E5_OC = 1;}
			if (pin == 6) {P_E6_TRIS = 1; P_E6_OC = 1;}
			if (pin == 12) {P_E12_TRIS = 1; P_E12_OC = 1;}
	#else
			if (pin < 11) return false;
	#endif
			tris = 0; ana = 1; oc = 1;
			break;

		case EXT_COM_RESERVED:
		case EXT_CONSOLE_RESERVED:
			ExtCurrentConfig[pin] = cfg;		// don't do anything except set the config type
		#ifdef OLIMEX
			if (pin == 5) {P_E5_TRIS = 1; P_E5_OC = 1; P_E5_ANALOG = 1;}
			if (pin == 6) {P_E6_TRIS = 1; P_E6_OC = 1; P_E6_ANALOG = 1;}
			if (pin == 11) {P_E11_TRIS = 1; P_E11_OC = 1;}
			if (pin == 12) {P_E12_TRIS = 1; P_E12_OC = 1;}
		#endif
			return true;

		default:
			return false;
	}

	ExtCurrentConfig[pin] = cfg;

	// set the TRIS and analog characteristics
	switch (pin) {
		case 1:  P_E1_TRIS = tris;   P_E1_OC = oc;   P_E1_ANALOG = ana;		break;
		case 2:  P_E2_TRIS = tris;   P_E2_OC = oc;   P_E2_ANALOG = ana;		break;
		case 3:  P_E3_TRIS = tris;   P_E3_OC = oc;   P_E3_ANALOG = ana;		break;
		case 4:  P_E4_TRIS = tris;   P_E4_OC = oc;   P_E4_ANALOG = ana;		break;
		case 5:  P_E5_TRIS = tris;   P_E5_OC = oc;   P_E5_ANALOG = ana;		break;
		case 6:  P_E6_TRIS = tris;   P_E6_OC = oc;   P_E6_ANALOG = ana;		break;
	#ifdef OLIMEX
            #ifdef  OLIMEX_DUINOMITE_EMEGA
		case 7:  P_E7_TRIS = tris;   P_E7_OC = oc;   P_E7_ANALOG = ana;		break;
		case 8:  P_E8_TRIS = tris;   P_E8_OC = oc;   break;
		case 9:  P_E9_TRIS = tris;   P_E9_OC = oc;   break;
		case 10: P_E10_TRIS = tris;  P_E10_OC = oc;  break;
            #else
		case 7:  P_E7_TRIS = tris;   P_E7_OC = oc;	              break;
 		case 8:  if (!S.SDEnable) { P_E8_TRIS = tris; P_E8_OC = oc;}  break;
		case 9:  if (!S.SDEnable) { P_E9_TRIS = tris; P_E9_OC = oc;}  break;
		case 10: if (!S.SDEnable) { P_E10_TRIS = tris; P_E10_OC = oc;} break;
            #endif
	#else
		case 7:  P_E7_TRIS = tris;   P_E7_OC = oc;   P_E7_ANALOG = ana;		break;
		case 8:  P_E8_TRIS = tris;   P_E8_OC = oc;   P_E8_ANALOG = ana;		break;
		case 9:  P_E9_TRIS = tris;   P_E9_OC = oc;   P_E9_ANALOG = ana;		break;
		case 10: P_E10_TRIS = tris;  P_E10_OC = oc;  P_E10_ANALOG = ana;	break;
	#endif
		case 11: P_E11_TRIS = tris;  P_E11_OC = oc;			break;
		case 12: P_E12_TRIS = tris;  P_E12_OC = oc;			break;
		case 13: P_E13_TRIS = tris;  P_E13_OC = oc;			break;
		case 14: P_E14_TRIS = tris;  P_E14_OC = oc;			break;
		case 15: P_E15_TRIS = tris;  P_E15_OC = oc;			break;
		case 16: P_E16_TRIS = tris;  P_E16_OC = oc;			break;
		case 17: P_E17_TRIS = tris;  P_E17_OC = oc;			break;
		case 18: P_E18_TRIS = tris;  P_E18_OC = oc;			break;
	#ifdef OLIMEX
            // SPP +
            #ifdef	OLIMEX_DUINOMITE_EMEGA		// edit for DuinoMite eMega
		case 19: P_E19_TRIS = tris;  P_E19_OC = oc;			break;
		case 20: P_E20_TRIS = tris;  P_E20_OC = oc;                     break;
                case 21: P_E21_TRIS = tris;  P_E21_OC = oc;			break;
		case 22: P_E22_TRIS = tris;  P_E22_OC = oc;			break;
		case 23: P_E23_TRIS = tris;  P_E23_OC = oc;			break;
		case 24: P_E24_TRIS = tris;  P_E24_OC = oc;			break;
		case 25: P_E25_TRIS = tris;  P_E25_OC = oc;			break;
		case 26: P_E26_TRIS = tris;  P_E26_OC = oc;			break;
		case 27: P_E27_TRIS = tris;  P_E27_OC = oc;			break;
		case 28: P_E28_TRIS = tris;  P_E28_OC = oc;			break;
		case 29: P_E29_TRIS = tris;  P_E29_OC = oc;			break;
		case 30: P_E30_TRIS = tris;  P_E30_OC = oc;			break;
		case 31: P_E31_TRIS = tris;  P_E31_OC = oc; P_E31_ANALOG = ana;	break;
		case 32: P_E32_TRIS = tris;  P_E32_OC = oc; P_E31_ANALOG = ana;	break;
		case 33: P_E33_TRIS = tris;  P_E33_OC = oc;			break;
		case 34: P_E34_TRIS = tris;  P_E34_OC = oc;			break;
		case 35: P_E35_TRIS = tris;  P_E35_OC = oc; P_E31_ANALOG = ana; break;
		case 36: P_E36_TRIS = tris;  P_E36_OC = oc;			break;
		case 37: P_E37_TRIS = tris;  P_E37_OC = oc;			break;
		case 38: P_E38_TRIS = tris;  P_E38_OC = oc;			break;
		case 39: P_E39_TRIS = tris;  P_E39_OC = oc;			break;
            #else	// original by Geoff Graham for DuinoMite Mega
		case 19: //if (!S.VideoMode) {
                    P_E19_TRIS = tris; P_E19_OC = oc; P_E19_ANALOG = ana;//}
                break;
		case 20: if (!S.VideoMode || P_VGA_COMP) {P_E20_TRIS = tris; P_E20_OC = oc; P_E20_ANALOG = ana;} break;
		case 21: P_E21_TRIS = tris;  P_E21_OC = oc;  P_E21_ANALOG = ana;		break;
                case 22: P_E22_TRIS = tris;  P_E22_OC = oc; break;
                case 23: P_E23_TRIS = tris;  P_E23_OC = oc; break;
            #endif
            // SPP -
	#else
		case 19: P_E19_TRIS = tris;  P_E19_OC = oc;			break;
		case 20: P_E20_TRIS = tris;  P_E20_OC = oc;			break;
	#endif
	#ifdef UBW32
                case 21: P_E21_TRIS = tris;  P_E21_OC = oc;			break;
		case 22: P_E22_TRIS = tris;  P_E22_OC = oc;			break;
		case 23: P_E23_TRIS = tris;  P_E23_OC = oc;			break;
		case 24: P_E24_TRIS = tris;  P_E24_OC = oc;			break;
		case 25: P_E25_TRIS = tris;  P_E25_OC = oc;			break;
		case 26: P_E26_TRIS = tris;  P_E26_OC = oc;			break;
		case 27: P_E27_TRIS = tris;  P_E27_OC = oc;			break;
		case 28: P_E28_TRIS = tris;  P_E28_OC = oc;			break;
		case 29: P_E29_TRIS = tris;  P_E29_OC = oc;			break;
		case 30: P_E30_TRIS = tris;  P_E30_OC = oc;			break;
		case 31: P_E31_TRIS = tris;  P_E31_OC = oc;			break;
		case 32: P_E32_TRIS = tris;  P_E32_OC = oc;			break;
		case 33: P_E33_TRIS = tris;  P_E33_OC = oc;			break;
		case 34: P_E34_TRIS = tris;  P_E34_OC = oc;			break;
		case 35: P_E35_TRIS = tris;  P_E35_OC = oc;			break;
		case 36: P_E36_TRIS = tris;  P_E36_OC = oc;			break;
		case 37: P_E37_TRIS = tris;  P_E37_OC = oc;			break;
		case 38: P_E38_TRIS = tris;  P_E38_OC = oc;			break;
		case 39: P_E39_TRIS = tris;  P_E39_OC = oc;			break;
		case 40: P_E40_TRIS = tris;  P_E40_OC = oc;			break;
		case 41: P_E41_TRIS = tris;  P_E41_OC = oc;			break;
		case 42: P_E42_TRIS = tris;  P_E42_OC = oc;			break;
		case 43: P_E43_TRIS = tris;  P_E43_OC = oc;			break;
		case 44: P_E44_TRIS = tris;  P_E44_OC = oc;			break;
		case 45: P_E45_TRIS = tris;  P_E45_OC = oc;			break;
		case 46: P_E46_TRIS = tris;  P_E46_OC = oc;			break;
		case 47: P_E47_TRIS = tris;  P_E47_OC = oc;			break;
		case 48: P_E48_TRIS = tris;  P_E48_OC = oc;			break;
		case 49: P_E49_TRIS = tris;  P_E49_OC = oc;			break;
		case 50: P_E50_TRIS = tris;  P_E50_OC = oc;			break;
	#endif
	}

	if (cfg == EXT_NOT_CONFIG) ExtSet(pin, 0);						// set the default output to low
	return true;
}


/****************************************************************************************************************************
Set the output of a digital I/O pin
Returns true if all is OK
*****************************************************************************************************************************/
int ExtSet(int pin, int val){
	val = (val != 0);							// non zero is on
	INTDisableInterrupts();				// setting an output bit is NOT atomic and a bit set operation
																// in an interrupt could result in this set corrupting the output
	switch (pin) {
	#ifdef UBW32
		case 0:  P_LED_OUT = !val;  break;		// this is the LED - the UBW32 wired them upside down !!
	#else
		case 0:  P_LED_OUT = val;  break;			// this is the LED
	#endif
		case 1:  P_E1_OUT = val;   break;
		case 2:  P_E2_OUT = val;   break;
		case 3:  P_E3_OUT = val;   break;
		case 4:	 P_E4_OUT = val;   break;
		case 5:	 P_E5_OUT = val;   break;
		case 6:	 P_E6_OUT = val;   break;
		case 7:	 P_E7_OUT = val;   break;
	#ifdef OLIMEX
            #ifdef  OLIMEX_DUINOMITE_EMEGA
		case 8:	 P_E8_OUT = val;   break;
		case 9:	 P_E9_OUT = val;   break;
		case 10: P_E10_OUT = val;  break;
            #else
		case 8:	 if (!S.SDEnable) P_E8_OUT = val;   break;
		case 9:	 if (!S.SDEnable) P_E9_OUT = val;   break;
		case 10: if (!S.SDEnable) P_E10_OUT = val;  break;
            #endif
	#else
		case 8:	 P_E8_OUT = val;   break;
		case 9:	 P_E9_OUT = val;   break;
		case 10: P_E10_OUT = val;  break;
	#endif
		case 11: P_E11_OUT = val;  break;
		case 12: P_E12_OUT = val;  break;
		case 13: P_E13_OUT = val;  break;
		case 14: P_E14_OUT = val;  break;
		case 15: P_E15_OUT = val;  break;
		case 16: P_E16_OUT = val;  break;
		case 17: P_E17_OUT = val;  break;
		case 18: P_E18_OUT = val;  break;
	#ifdef OLIMEX
            #ifdef  OLIMEX_DUINOMITE_EMEGA
                case 19: P_E19_OUT = val; break;
                case 20: P_E20_OUT = val; break;
            #else
		case 19: P_E19_OUT = val;  break;
		case 20: if (!S.VideoMode || P_VGA_COMP) P_E20_OUT = val;  break;
                case 22: P_E22_OUT = val ; break;
                case 23: P_E23_OUT = val ; break;
            #endif
        #else
		case 19: P_E19_OUT = val;  break;
		case 20: P_E20_OUT = val;  break;
	#endif
        #if defined UBW32 || defined OLIMEX_DUINOMITE_EMEGA
		case 21: P_E21_OUT = val;	break;
		case 22: P_E22_OUT = val;	break;
		case 23: P_E23_OUT = val;	break;
		case 24: P_E24_OUT = val;	break;
		case 25: P_E25_OUT = val;	break;
		case 26: P_E26_OUT = val;	break;
		case 27: P_E27_OUT = val;	break;
		case 28: P_E28_OUT = val;	break;
		case 29: P_E29_OUT = val;	break;
		case 30: P_E30_OUT = val;	break;
		case 31: P_E31_OUT = val;	break;
		case 32: P_E32_OUT = val;	break;
		case 33: P_E33_OUT = val;	break;
		case 34: P_E34_OUT = val;	break;
		case 35: P_E35_OUT = val;	break;
		case 36: P_E36_OUT = val;	break;
		case 37: P_E37_OUT = val;	break;
		case 38: P_E38_OUT = val;	break;
		case 39: P_E39_OUT = val;	break;
        #endif
        #ifdef UBW32
		case 40: P_E40_OUT = val;	break;
		case 41: P_E41_OUT = val;	break;
		case 42: P_E42_OUT = val;	break;
		case 43: P_E43_OUT = val;	break;
		case 44: P_E44_OUT = val;	break;
		case 45: P_E45_OUT = val;	break;
		case 46: P_E46_OUT = val;	break;
		case 47: P_E47_OUT = val;	break;
		case 48: P_E48_OUT = val;	break;
		case 49: P_E49_OUT = val;	break;
		case 50: P_E50_OUT = val;	break;
	#endif
		default:
			INTEnableInterrupts();
			return false;
	}
	INTEnableInterrupts();
	return true;
}


/****************************************************************************************************************************
Get the value of an I/O pin and returns it in &val
For digital returns 0 if low or 1 if high
For analog returns the reading as a 10 bit number with 0b1111111111 = 3.3V
Returns true if all is OK
*****************************************************************************************************************************/
int ExtInp(int pin, int *val){
	int r = 0;

    if(pin < 0 || pin > NBRPINS) return false;					// sanity check

	// read from a digital input
	if (ExtCurrentConfig[pin] == EXT_DIG_IN || ExtCurrentConfig[pin] == EXT_INT_HI || ExtCurrentConfig[pin] == EXT_INT_LO || ExtCurrentConfig[pin] == EXT_OC_OUT) {
		switch (pin) {
			case 0:  r = P_BUTTON_IN;  break;			// this is the push button
			case 1:  r = P_E1_IN;  break;
			case 2:  r = P_E2_IN;  break;
			case 3:  r = P_E3_IN;  break;
			case 4:	 r = P_E4_IN;  break;
			case 5:	 r = P_E5_IN;  break;
			case 6:	 r = P_E6_IN;  break;
			case 7:	 r = P_E7_IN;  break;
		#ifdef OLIMEX
                    #ifdef  OLIMEX_DUINOMITE_EMEGA
			case 8:	 r = P_E8_IN;  break;
			case 9:	 r = P_E9_IN;  break;
			case 10: r = P_E10_IN; break;
                    #else
			case 8:	 if (!S.SDEnable) r = P_E8_IN;  break;
			case 9:	 if (!S.SDEnable) r = P_E9_IN;  break;
			case 10: if (!S.SDEnable) r = P_E10_IN; break;
                    #endif
		#else
			case 8:	 r = P_E8_IN;  break;
			case 9:	 r = P_E9_IN;  break;
			case 10: r = P_E10_IN; break;
		#endif
			case 11: r = P_E11_IN; break;
			case 12: r = P_E12_IN; break;
			case 13: r = P_E13_IN; break;
			case 14: r = P_E14_IN; break;
			case 15: r = P_E15_IN; break;
			case 16: r = P_E16_IN; break;
			case 17: r = P_E17_IN; break;
			case 18: r = P_E18_IN; break;
		#ifdef OLIMEX
                    #ifdef  OLIMEX_DUINOMITE_EMEGA
			case 19: r = P_E19_IN; break;
			case 20: r = P_E20_IN; break;
                    #else
			case 19: r = P_E19_IN; break;
			case 20: if (!S.VideoMode || P_VGA_COMP) r = P_E20_IN; break;
                        case 22: r = P_E22_IN; break;
                        case 23: r = P_E23_IN; break;
                    #endif
                #else
			case 19: r = P_E19_IN; break;
			case 20: r = P_E20_IN; break;
		#endif
                #if defined UBW32 || defined OLIMEX_DUINOMITE_EMEGA
			case 21: r = P_E21_IN;	break;
			case 22: r = P_E22_IN;	break;
			case 23: r = P_E23_IN;	break;
			case 24: r = P_E24_IN;	break;
			case 25: r = P_E25_IN;	break;
			case 26: r = P_E26_IN;	break;
			case 27: r = P_E27_IN;	break;
			case 28: r = P_E28_IN;	break;
			case 29: r = P_E29_IN;	break;
			case 30: r = P_E30_IN;	break;
			case 31: r = P_E31_IN;	break;
			case 32: r = P_E32_IN;	break;
			case 33: r = P_E33_IN;	break;
			case 34: r = P_E34_IN;	break;
			case 35: r = P_E35_IN;	break;
			case 36: r = P_E36_IN;	break;
			case 37: r = P_E37_IN;	break;
			case 38: r = P_E38_IN;	break;
			case 39: r = P_E39_IN;	break;
                #endif
		#ifdef UBW32
			case 40: r = P_E40_IN;	break;
			case 41: r = P_E41_IN;	break;
			case 42: r = P_E42_IN;	break;
			case 43: r = P_E43_IN;	break;
			case 44: r = P_E44_IN;	break;
			case 45: r = P_E45_IN;	break;
			case 46: r = P_E46_IN;	break;
			case 47: r = P_E47_IN;	break;
			case 48: r = P_E48_IN;	break;
			case 49: r = P_E49_IN;	break;
			case 50: r = P_E50_IN;	break;
		#endif
		}
		*val = (r?1:0);												// zero for low, 1 for high
		return true;
	}

	// read from an analog input
	if (ExtCurrentConfig[pin] == EXT_ANA_IN) {
		switch (pin) {       										// select input channel
			case 1:  AD1CHSbits.CH0SA = P_E1_ACHAN;  break;
			case 2:  AD1CHSbits.CH0SA = P_E2_ACHAN;  break;
			case 3:  AD1CHSbits.CH0SA = P_E3_ACHAN;  break;
			case 4:  AD1CHSbits.CH0SA = P_E4_ACHAN;  break;
			case 5:  AD1CHSbits.CH0SA = P_E5_ACHAN;  break;
			case 6:  AD1CHSbits.CH0SA = P_E6_ACHAN;  break;
#ifdef  OLIMEX
		// SPP +
		#ifdef	OLIMEX_DUINOMITE_EMEGA	// edit for DuinoMite eMega
                        case 35: AD1CHSbits.CH0SA = P_E35_ACHAN; break;
		#else		// original by Geoff Graham for DuinoMite Mega
			case 19: if (!S.VideoMode) AD1CHSbits.CH0SA = P_E19_ACHAN;  break;
			case 20: if (!S.VideoMode) AD1CHSbits.CH0SA = P_E20_ACHAN;  break;
			case 21: AD1CHSbits.CH0SA = P_E21_ACHAN;  break;
		#endif
		// SPP -
#else
			case 7:  AD1CHSbits.CH0SA = P_E7_ACHAN;  break;
			case 8:  AD1CHSbits.CH0SA = P_E8_ACHAN;  break;
			case 9:  AD1CHSbits.CH0SA = P_E9_ACHAN;  break;
			case 10: AD1CHSbits.CH0SA = P_E10_ACHAN; break;
#endif
		}
#ifdef OLIMEX
    #ifdef  OLIMEX_DUINOMITE_EMEGA
    #else
		if ((pin == 19 || pin == 20) && S.VideoMode) {
			*val = 0;
			return true;
		}
    #endif
#endif
		AD1CON1bits.SAMP = 1;       						// start sampling
		while (!AD1CON1bits.DONE && !MMAbort);  // wait conversion complete
		*val = ADC1BUF0;												// and get the result
#ifdef OLIMEX
    #ifdef  OLIMEX_DUINOMITE_EMEGA
		if (pin == 35) *val *= 3.13;
    #else
		if (pin == 21) *val *= 3.13;
    #endif
#endif
		return true;
	}

	// read from a frequency/period input
	if (ExtCurrentConfig[pin] == EXT_FREQ_IN || ExtCurrentConfig[pin] == EXT_PER_IN) {
		switch (pin) {       										// select input channel
#ifdef OLIMEX
			case 5:  *val = INT2Value; break;
			case 6:  *val = INT3Value; break;
			case 7:  *val = INT4Value; break;
#else
			case 11:  *val = INT1Value; break;
			case 12:  *val = INT2Value; break;
			case 13:  *val = INT3Value; break;
			case 14:  *val = INT4Value; break;
#endif
		}
		return true;
	}

	// read from a counter input
	if (ExtCurrentConfig[pin] == EXT_CNT_IN) {
		switch (pin) {       										// select input channel
#ifdef OLIMEX
			case 5:  *val = INT2Count; break;
			case 6:  *val = INT3Count; break;
			case 7:  *val = INT4Count; break;
#else
			case 11:  *val = INT1Count; break;
			case 12:  *val = INT2Count; break;
			case 13:  *val = INT3Count; break;
			case 14:  *val = INT4Count; break;
#endif
		}
		return true;
	}

	return false;													// not configured as an input
}


#ifdef OLIMEX
// perform the counting functions for pin BUT
void __ISR( _EXTERNAL_1_VECTOR , ipl2) INT1Interrupt( void) {
	if (ExtCurrentConfig[0] == EXT_PER_IN) {
		INT1Value = INT1Count;
		INT1Count = 0;
	} else INT1Count++;
	SleepMMVal = SleepButInt;
	mINT1ClearIntFlag();    										// Clear the interrupt flag
	return;
}


// perform the counting functions for pin 5
void __ISR( _EXTERNAL_2_VECTOR , ipl2) INT2Interrupt( void) {
	if (ExtCurrentConfig[5] == EXT_PER_IN) {
		INT2Value = INT2Count;
		INT2Count = 0;
	} else INT2Count++;
	mINT2ClearIntFlag();    										// Clear the interrupt flag
	return;
}


// perform the counting functions for pin 6
void __ISR( _EXTERNAL_3_VECTOR , ipl2) INT3Interrupt( void) {
	if (ExtCurrentConfig[6] == EXT_PER_IN) {
		INT3Value = INT3Count;
		INT3Count = 0;
	} else INT3Count++;
	mINT3ClearIntFlag();    										// Clear the interrupt flag
	return;
}


// perform the counting functions for pin 7
void __ISR( _EXTERNAL_4_VECTOR , ipl2) INT4Interrupt( void) {
	if (ExtCurrentConfig[7] == EXT_PER_IN) {
		INT4Value = INT4Count;
		INT4Count = 0;
	} else INT4Count++;
	SleepMMVal = SleepExtInt;
	mINT4ClearIntFlag();    										// Clear the interrupt flag
	return;
}
#else
// perform the counting functions for pin 11
void __ISR( _EXTERNAL_1_VECTOR , ipl2) INT1Interrupt( void) {
	if (ExtCurrentConfig[11] == EXT_PER_IN) {
		INT1Value = INT1Count;
		INT1Count = 0;
	} else INT1Count++;
	mINT1ClearIntFlag();    										// Clear the interrupt flag
	return;
}


// perform the counting functions for pin 12
void __ISR( _EXTERNAL_2_VECTOR , ipl2) INT2Interrupt( void) {
	if (ExtCurrentConfig[12] == EXT_PER_IN) {
		INT2Value = INT2Count;
		INT2Count = 0;
	} else INT2Count++;
	mINT2ClearIntFlag();    										// Clear the interrupt flag
	return;
}


// perform the counting functions for pin 13
void __ISR( _EXTERNAL_3_VECTOR , ipl2) INT3Interrupt( void) {
	if (ExtCurrentConfig[13] == EXT_PER_IN) {
		INT3Value = INT3Count;
		INT3Count = 0;
	} else INT3Count++;
	mINT3ClearIntFlag();    										// Clear the interrupt flag
	return;
}


// perform the counting functions for pin 14
void __ISR( _EXTERNAL_4_VECTOR , ipl2) INT4Interrupt( void) {
	if (ExtCurrentConfig[14] == EXT_PER_IN) {
		INT4Value = INT4Count;
		INT4Count = 0;
	} else INT4Count++;
	mINT4ClearIntFlag();    										// Clear the interrupt flag
	return;
}
#endif
