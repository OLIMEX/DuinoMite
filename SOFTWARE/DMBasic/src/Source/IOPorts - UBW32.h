/***********************************************************************************************************************
MMBasic

IOPorts.h

Include file that defines the IOPins for the PIC32 chip in MMBasic.
This file needs to be completed and MMBasic rewritten to use the defines in this file (a ToDo job).
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

//#include <p32xxxx.h>
//#include <plib.h>

/*
Note that we never use TRIS to set a pin as an input because that is the default after powerup
*/

// General defines
#define P_INPUT						1						// for setting the TRIS on I/O bits
#define P_OUTPUT					0
#define P_ON							1
#define P_OFF							0


// Defines for the external I/O pins
#define P_BUTTON_IN				PORTEbits.RE7				// this is the program push button
#define P_LED_OUT					LATEbits.LATE0			// this is the LED
#define P_LED_TRIS				TRISEbits.TRISE0

#define P_E1_TRIS					TRISBbits.TRISB4		// MMBasic ext i/o pin 1
#define P_E1_ANALOG				AD1PCFGbits.PCFG4
#define P_E1_ACHAN				4
#define P_E1_OC	    			ODCBbits.ODCB4
#define P_E1_OUT					LATBbits.LATB4
#define P_E1_IN						PORTBbits.RB4
#define P_E1_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB4_MASK else TRISBCLR = _TRISB_TRISB4_MASK;}
#define P_E1_TRISINP			{TRISBSET = _TRISB_TRISB4_MASK;}
#define P_E1_TRISOUT			{TRISBCLR = _TRISB_TRISB4_MASK;}
#define P_E1_WRITESET(m)	{if(m) LATBSET = _LATB_LATB4_MASK else LATBCLR = _LATB_LATB4_MASK;}
#define P_E1_WRITE1				{LATBSET = _LATB_LATB4_MASK;}
#define P_E1_WRITE0				{LATBCLR = _LATB_LATB4_MASK;}
#define P_E1_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB4_MASK else ODCBCLR = _ODCB_ODCB4_MASK;}
#define P_E1_ODCON				{ODCBSET = _ODCB_ODCB4_MASK;}
#define P_E1_ODCOFF				{ODCBCLR = _ODCB_ODCB4_MASK;}
#define P_E1_READ					((PORTB & _PORTB_RB4_MASK) ? 1 : 0)

#define P_E2_TRIS					TRISBbits.TRISB3		// MMBasic ext i/o pin 2
#define P_E2_ANALOG				AD1PCFGbits.PCFG3
#define P_E2_ACHAN				3
#define P_E2_OC	    			ODCBbits.ODCB3
#define P_E2_OUT					LATBbits.LATB3
#define P_E2_IN						PORTBbits.RB3
#define P_E2_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB3_MASK else TRISBCLR = _TRISB_TRISB3_MASK;}
#define P_E2_TRISINP			{TRISBSET = _TRISB_TRISB3_MASK;}
#define P_E2_TRISOUT			{TRISBCLR = _TRISB_TRISB3_MASK;}
#define P_E2_WRITESET(m)	{if(m) LATBSET = _LATB_LATB3_MASK else LATBCLR = _LATB_LATB3_MASK;}
#define P_E2_WRITE1				{LATBSET = _LATB_LATB3_MASK;}
#define P_E2_WRITE0				{LATBCLR = _LATB_LATB3_MASK;}
#define P_E2_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB3_MASK else ODCBCLR = _ODCB_ODCB3_MASK;}
#define P_E2_ODCON				{ODCBSET = _ODCB_ODCB3_MASK;}
#define P_E2_ODCOFF				{ODCBCLR = _ODCB_ODCB3_MASK;}
#define P_E2_READ					((PORTB & _PORTB_RB3_MASK) ? 1 : 0)

#define P_E3_TRIS					TRISBbits.TRISB2		// MMBasic ext i/o pin 3
#define P_E3_ANALOG				AD1PCFGbits.PCFG2
#define P_E3_ACHAN				2
#define P_E3_OC	    			ODCBbits.ODCB2
#define P_E3_OUT					LATBbits.LATB2
#define P_E3_IN						PORTBbits.RB2
#define P_E3_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB2_MASK else TRISBCLR = _TRISB_TRISB2_MASK;}
#define P_E3_TRISINP			{TRISBSET = _TRISB_TRISB2_MASK;}
#define P_E3_TRISOUT			{TRISBCLR = _TRISB_TRISB2_MASK;}
#define P_E3_WRITESET(m)	{if(m) LATBSET = _LATB_LATB2_MASK else LATBCLR = _LATB_LATB2_MASK;}
#define P_E3_WRITE1				{LATBSET = _LATB_LATB2_MASK;}
#define P_E3_WRITE0				{LATBCLR = _LATB_LATB2_MASK;}
#define P_E3_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB2_MASK else ODCBCLR = _ODCB_ODCB2_MASK;}
#define P_E3_ODCON				{ODCBSET = _ODCB_ODCB2_MASK;}
#define P_E3_ODCOFF				{ODCBCLR = _ODCB_ODCB2_MASK;}
#define P_E3_READ					((PORTB & _PORTB_RB2_MASK) ? 1 : 0)

#define P_E4_TRIS					TRISBbits.TRISB1		// MMBasic ext i/o pin 4
#define P_E4_ANALOG				AD1PCFGbits.PCFG1
#define P_E4_ACHAN				1
#define P_E4_OC	    			ODCBbits.ODCB1
#define P_E4_OUT					LATBbits.LATB1
#define P_E4_IN						PORTBbits.RB1
#define P_E4_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB1_MASK else TRISBCLR = _TRISB_TRISB1_MASK;}
#define P_E4_TRISINP			{TRISBSET = _TRISB_TRISB1_MASK;}
#define P_E4_TRISOUT			{TRISBCLR = _TRISB_TRISB1_MASK;}
#define P_E4_WRITESET(m)	{if(m) LATBSET = _LATB_LATB1_MASK else LATBCLR = _LATB_LATB1_MASK;}
#define P_E4_WRITE1				{LATBSET = _LATB_LATB1_MASK;}
#define P_E4_WRITE0				{LATBCLR = _LATB_LATB1_MASK;}
#define P_E4_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB1_MASK else ODCBCLR = _ODCB_ODCB1_MASK;}
#define P_E4_ODCON				{ODCBSET = _ODCB_ODCB1_MASK;}
#define P_E4_ODCOFF				{ODCBCLR = _ODCB_ODCB1_MASK;}
#define P_E4_READ					((PORTB & _PORTB_RB1_MASK) ? 1 : 0)

#define P_E5_TRIS					TRISBbits.TRISB9		// MMBasic ext i/o pin 5
#define P_E5_ANALOG				AD1PCFGbits.PCFG9
#define P_E5_ACHAN				9
#define P_E5_OC	    			ODCBbits.ODCB9
#define P_E5_OUT					LATBbits.LATB9
#define P_E5_IN						PORTBbits.RB9
#define P_E5_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB9_MASK else TRISBCLR = _TRISB_TRISB9_MASK;}
#define P_E5_TRISINP			{TRISBSET = _TRISB_TRISB9_MASK;}
#define P_E5_TRISOUT			{TRISBCLR = _TRISB_TRISB9_MASK;}
#define P_E5_WRITESET(m)	{if(m) LATBSET = _LATB_LATB9_MASK else LATBCLR = _LATB_LATB9_MASK;}
#define P_E5_WRITE1				{LATBSET = _LATB_LATB9_MASK;}
#define P_E5_WRITE0				{LATBCLR = _LATB_LATB9_MASK;}
#define P_E5_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB9_MASK else ODCBCLR = _ODCB_ODCB9_MASK;}
#define P_E5_ODCON				{ODCBSET = _ODCB_ODCB9_MASK;}
#define P_E5_ODCOFF				{ODCBCLR = _ODCB_ODCB9_MASK;}
#define P_E5_READ					((PORTB & _PORTB_RB9_MASK) ? 1 : 0)

#define P_E6_TRIS					TRISBbits.TRISB10		// MMBasic ext i/o pin 6
#define P_E6_ANALOG				AD1PCFGbits.PCFG10	 
#define P_E6_ACHAN				10
#define P_E6_OC	    			ODCBbits.ODCB10
#define P_E6_OUT					LATBbits.LATB10
#define P_E6_IN						PORTBbits.RB10
#define P_E6_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB10_MASK else TRISBCLR = _TRISB_TRISB10_MASK;}
#define P_E6_TRISINP			{TRISBSET = _TRISB_TRISB10_MASK;}
#define P_E6_TRISOUT			{TRISBCLR = _TRISB_TRISB10_MASK;}
#define P_E6_WRITESET(m)	{if(m) LATBSET = _LATB_LATB10_MASK else LATBCLR = _LATB_LATB10_MASK;}
#define P_E6_WRITE1				{LATBSET = _LATB_LATB10_MASK;}
#define P_E6_WRITE0				{LATBCLR = _LATB_LATB10_MASK;}
#define P_E6_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB10_MASK else ODCBCLR = _ODCB_ODCB10_MASK;}
#define P_E6_ODCON				{ODCBSET = _ODCB_ODCB10_MASK;}
#define P_E6_ODCOFF				{ODCBCLR = _ODCB_ODCB10_MASK;}
#define P_E6_READ					((PORTB & _PORTB_RB10_MASK) ? 1 : 0)

#define P_E7_TRIS					TRISBbits.TRISB11		// MMBasic ext i/o pin 7
#define P_E7_ANALOG				AD1PCFGbits.PCFG11
#define P_E7_ACHAN				11
#define P_E7_OC	    			ODCBbits.ODCB11
#define P_E7_OUT					LATBbits.LATB11
#define P_E7_IN						PORTBbits.RB11
#define P_E7_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB11_MASK else TRISBCLR = _TRISB_TRISB11_MASK;}
#define P_E7_TRISINP			{TRISBSET = _TRISB_TRISB11_MASK;}
#define P_E7_TRISOUT			{TRISBCLR = _TRISB_TRISB11_MASK;}
#define P_E7_WRITESET(m)	{if(m) LATBSET = _LATB_LATB11_MASK else LATBCLR = _LATB_LATB11_MASK;}
#define P_E7_WRITE1				{LATBSET = _LATB_LATB11_MASK;}
#define P_E7_WRITE0				{LATBCLR = _LATB_LATB11_MASK;}
#define P_E7_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB11_MASK else ODCBCLR = _ODCB_ODCB11_MASK;}
#define P_E7_ODCON				{ODCBSET = _ODCB_ODCB11_MASK;}
#define P_E7_ODCOFF				{ODCBCLR = _ODCB_ODCB11_MASK;}
#define P_E7_READ					((PORTB & _PORTB_RB11_MASK) ? 1 : 0)

#define P_E8_TRIS					TRISBbits.TRISB12		// MMBasic ext i/o pin 8
#define P_E8_ANALOG				AD1PCFGbits.PCFG12	 
#define P_E8_ACHAN				12
#define P_E8_OC	    			ODCBbits.ODCB12
#define P_E8_OUT					LATBbits.LATB12
#define P_E8_IN						PORTBbits.RB12
#define P_E8_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB12_MASK else TRISBCLR = _TRISB_TRISB12_MASK;}
#define P_E8_TRISINP			{TRISBSET = _TRISB_TRISB12_MASK;}
#define P_E8_TRISOUT			{TRISBCLR = _TRISB_TRISB12_MASK;}
#define P_E8_WRITESET(m)	{if(m) LATBSET = _LATB_LATB12_MASK else LATBCLR = _LATB_LATB12_MASK;}
#define P_E8_WRITE1				{LATBSET = _LATB_LATB12_MASK;}
#define P_E8_WRITE0				{LATBCLR = _LATB_LATB12_MASK;}
#define P_E8_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB12_MASK else ODCBCLR = _ODCB_ODCB12_MASK;}
#define P_E8_ODCON				{ODCBSET = _ODCB_ODCB12_MASK;}
#define P_E8_ODCOFF				{ODCBCLR = _ODCB_ODCB12_MASK;}
#define P_E8_READ					((PORTB & _PORTB_RB12_MASK) ? 1 : 0)

#define P_E9_TRIS					TRISBbits.TRISB13		// MMBasic ext i/o pin 9
#define P_E9_ANALOG				AD1PCFGbits.PCFG13
#define P_E9_ACHAN				13
#define P_E9_OC	    			ODCBbits.ODCB13
#define P_E9_OUT					LATBbits.LATB13
#define P_E9_IN						PORTBbits.RB13
#define P_E9_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB13_MASK else TRISBCLR = _TRISB_TRISB13_MASK;}
#define P_E9_TRISINP			{TRISBSET = _TRISB_TRISB13_MASK;}
#define P_E9_TRISOUT			{TRISBCLR = _TRISB_TRISB13_MASK;}
#define P_E9_WRITESET(m)	{if(m) LATBSET = _LATB_LATB13_MASK else LATBCLR = _LATB_LATB13_MASK;}
#define P_E9_WRITE1				{LATBSET = _LATB_LATB13_MASK;}
#define P_E9_WRITE0				{LATBCLR = _LATB_LATB13_MASK;}
#define P_E9_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB13_MASK else ODCBCLR = _ODCB_ODCB13_MASK;}
#define P_E9_ODCON				{ODCBSET = _ODCB_ODCB13_MASK;}
#define P_E9_ODCOFF				{ODCBCLR = _ODCB_ODCB13_MASK;}
#define P_E9_READ					((PORTB & _PORTB_RB13_MASK) ? 1 : 0)

#define P_E10_TRIS				TRISBbits.TRISB15		// MMBasic ext i/o pin 10
#define P_E10_ANALOG			AD1PCFGbits.PCFG15	 
#define P_E10_ACHAN				15
#define P_E10_OC	   			ODCBbits.ODCB15
#define P_E10_OUT					LATBbits.LATB15
#define P_E10_IN					PORTBbits.RB15
#define P_E10_TRISSET(m)	{if(m) TRISBSET = _TRISB_TRISB15_MASK else TRISBCLR = _TRISB_TRISB15_MASK;}
#define P_E10_TRISINP			{TRISBSET = _TRISB_TRISB15_MASK;}
#define P_E10_TRISOUT			{TRISBCLR = _TRISB_TRISB15_MASK;}
#define P_E10_WRITESET(m)	{if(m) LATBSET = _LATB_LATB15_MASK else LATBCLR = _LATB_LATB15_MASK;}
#define P_E10_WRITE1			{LATBSET = _LATB_LATB15_MASK;}
#define P_E10_WRITE0			{LATBCLR = _LATB_LATB15_MASK;}
#define P_E10_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB15_MASK else ODCBCLR = _ODCB_ODCB15_MASK;}
#define P_E10_ODCON				{ODCBSET = _ODCB_ODCB15_MASK;}
#define P_E10_ODCOFF			{ODCBCLR = _ODCB_ODCB15_MASK;}
#define P_E10_READ				((PORTB & _PORTB_RB15_MASK) ? 1 : 0)

#define P_E11_TRIS				TRISDbits.TRISD8		// MMBasic ext i/o pin 11
#define P_E11_OC    			ODCDbits.ODCD8
#define P_E11_OUT					LATDbits.LATD8
#define P_E11_IN					PORTDbits.RD8
#define P_E11_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD8_MASK else TRISDCLR = _TRISD_TRISD8_MASK;}
#define P_E11_TRISINP			{TRISDSET = _TRISD_TRISD8_MASK;}
#define P_E11_TRISOUT			{TRISDCLR = _TRISD_TRISD8_MASK;}
#define P_E11_WRITESET(m)	{if(m) LATDSET = _LATD_LATD8_MASK else LATDCLR = _LATD_LATD8_MASK;}
#define P_E11_WRITE1			{LATDSET = _LATD_LATD8_MASK;}
#define P_E11_WRITE0			{LATDCLR = _LATD_LATD8_MASK;}
#define P_E11_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD8_MASK else ODCDCLR = _ODCD_ODCD8_MASK;}
#define P_E11_ODCON				{ODCDSET = _ODCD_ODCD8_MASK;}
#define P_E11_ODCOFF			{ODCDCLR = _ODCD_ODCD8_MASK;}
#define P_E11_READ				((PORTD & _PORTD_RD8_MASK) ? 1 : 0)

#define P_E12_TRIS				TRISDbits.TRISD9		// MMBasic ext i/o pin 12
#define P_E12_OC    			ODCDbits.ODCD9	 
#define P_E12_OUT					LATDbits.LATD9
#define P_E12_IN					PORTDbits.RD9
#define P_E12_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD9_MASK else TRISDCLR = _TRISD_TRISD9_MASK;}
#define P_E12_TRISINP			{TRISDSET = _TRISD_TRISD9_MASK;}
#define P_E12_TRISOUT			{TRISDCLR = _TRISD_TRISD9_MASK;}
#define P_E12_WRITESET(m)	{if(m) LATDSET = _LATD_LATD9_MASK else LATDCLR = _LATD_LATD9_MASK;}
#define P_E12_WRITE1			{LATDSET = _LATD_LATD9_MASK;}
#define P_E12_WRITE0			{LATDCLR = _LATD_LATD9_MASK;}
#define P_E12_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD9_MASK else ODCDCLR = _ODCD_ODCD9_MASK;}
#define P_E12_ODCON				{ODCDSET = _ODCD_ODCD9_MASK;}
#define P_E12_ODCOFF			{ODCDCLR = _ODCD_ODCD9_MASK;}
#define P_E12_READ				((PORTD & _PORTD_RD9_MASK) ? 1 : 0)

#define P_E13_TRIS				TRISDbits.TRISD10		// MMBasic ext i/o pin 13
#define P_E13_OC    			ODCDbits.ODCD10
#define P_E13_OUT					LATDbits.LATD10
#define P_E13_IN					PORTDbits.RD10
#define P_E13_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD10_MASK else TRISDCLR = _TRISD_TRISD10_MASK;}
#define P_E13_TRISINP			{TRISDSET = _TRISD_TRISD10_MASK;}
#define P_E13_TRISOUT			{TRISDCLR = _TRISD_TRISD10_MASK;}
#define P_E13_WRITESET(m)	{if(m) LATDSET = _LATD_LATD10_MASK else LATDCLR = _LATD_LATD10_MASK;}
#define P_E13_WRITE1			{LATDSET = _LATD_LATD10_MASK;}
#define P_E13_WRITE0			{LATDCLR = _LATD_LATD10_MASK;}
#define P_E13_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD10_MASK else ODCDCLR = _ODCD_ODCD10_MASK;}
#define P_E13_ODCON				{ODCDSET = _ODCD_ODCD10_MASK;}
#define P_E13_ODCOFF			{ODCDCLR = _ODCD_ODCD10_MASK;}
#define P_E13_READ				((PORTD & _PORTD_RD10_MASK) ? 1 : 0)

#define P_E14_TRIS				TRISDbits.TRISD11		// MMBasic ext i/o pin 14
#define P_E14_OC    			ODCDbits.ODCD11	 
#define P_E14_OUT					LATDbits.LATD11
#define P_E14_IN					PORTDbits.RD11
#define P_E14_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD11_MASK else TRISDCLR = _TRISD_TRISD11_MASK;}
#define P_E14_TRISINP			{TRISDSET = _TRISD_TRISD11_MASK;}
#define P_E14_TRISOUT			{TRISDCLR = _TRISD_TRISD11_MASK;}
#define P_E14_WRITESET(m)	{if(m) LATDSET = _LATD_LATD11_MASK else LATDCLR = _LATD_LATD11_MASK;}
#define P_E14_WRITE1			{LATDSET = _LATD_LATD11_MASK;}
#define P_E14_WRITE0			{LATDCLR = _LATD_LATD11_MASK;}
#define P_E14_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD11_MASK else ODCDCLR = _ODCD_ODCD11_MASK;}
#define P_E14_ODCON				{ODCDSET = _ODCD_ODCD11_MASK;}
#define P_E14_ODCOFF			{ODCDCLR = _ODCD_ODCD11_MASK;}
#define P_E14_READ				((PORTD & _PORTD_RD11_MASK) ? 1 : 0)

#define P_E15_TRIS				TRISDbits.TRISD12		// MMBasic ext i/o pin 15
#define P_E15_OC    			ODCDbits.ODCD12
#define P_E15_OUT					LATDbits.LATD12
#define P_E15_IN					PORTDbits.RD12
#define P_E15_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD12_MASK else TRISDCLR = _TRISD_TRISD12_MASK;}
#define P_E15_TRISINP			{TRISDSET = _TRISD_TRISD12_MASK;}
#define P_E15_TRISOUT			{TRISDCLR = _TRISD_TRISD12_MASK;}
#define P_E15_WRITESET(m)	{if(m) LATDSET = _LATD_LATD12_MASK else LATDCLR = _LATD_LATD12_MASK;}
#define P_E15_WRITE1			{LATDSET = _LATD_LATD12_MASK;}
#define P_E15_WRITE0			{LATDCLR = _LATD_LATD12_MASK;}
#define P_E15_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD12_MASK else ODCDCLR = _ODCD_ODCD12_MASK;}
#define P_E15_ODCON				{ODCDSET = _ODCD_ODCD12_MASK;}
#define P_E15_ODCOFF			{ODCDCLR = _ODCD_ODCD12_MASK;}
#define P_E15_READ				((PORTD & _PORTD_RD12_MASK) ? 1 : 0)

#define P_E16_TRIS				TRISDbits.TRISD13		// MMBasic ext i/o pin 16
#define P_E16_OC    			ODCDbits.ODCD13	 
#define P_E16_OUT					LATDbits.LATD13
#define P_E16_IN					PORTDbits.RD13
#define P_E16_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD13_MASK else TRISDCLR = _TRISD_TRISD13_MASK;}
#define P_E16_TRISINP			{TRISDSET = _TRISD_TRISD13_MASK;}
#define P_E16_TRISOUT			{TRISDCLR = _TRISD_TRISD13_MASK;}
#define P_E16_WRITESET(m)	{if(m) LATDSET = _LATD_LATD13_MASK else LATDCLR = _LATD_LATD13_MASK;}
#define P_E16_WRITE1			{LATDSET = _LATD_LATD13_MASK;}
#define P_E16_WRITE0			{LATDCLR = _LATD_LATD13_MASK;}
#define P_E16_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD13_MASK else ODCDCLR = _ODCD_ODCD13_MASK;}
#define P_E16_ODCON				{ODCDSET = _ODCD_ODCD13_MASK;}
#define P_E16_ODCOFF			{ODCDCLR = _ODCD_ODCD13_MASK;}
#define P_E16_READ				((PORTD & _PORTD_RD13_MASK) ? 1 : 0)

#define P_E17_TRIS				TRISEbits.TRISE4		// MMBasic ext i/o pin 17
#define P_E17_OC    			ODCEbits.ODCE4
#define P_E17_OUT					LATEbits.LATE4
#define P_E17_IN					PORTEbits.RE4
#define P_E17_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE4_MASK else TRISECLR = _TRISE_TRISE4_MASK;}
#define P_E17_TRISINP			{TRISESET = _TRISE_TRISE4_MASK;}
#define P_E17_TRISOUT			{TRISECLR = _TRISE_TRISE4_MASK;}
#define P_E17_WRITESET(m)	{if(m) LATESET = _LATE_LATE4_MASK else LATECLR = _LATE_LATE4_MASK;}
#define P_E17_WRITE1			{LATESET = _LATE_LATE4_MASK;}
#define P_E17_WRITE0			{LATECLR = _LATE_LATE4_MASK;}
#define P_E17_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE4_MASK else ODCECLR = _ODCE_ODCE4_MASK;}
#define P_E17_ODCON				{ODCESET = _ODCE_ODCE4_MASK;}
#define P_E17_ODCOFF			{ODCECLR = _ODCE_ODCE4_MASK;}
#define P_E17_READ				((PORTE & _PORTE_RE4_MASK) ? 1 : 0)

#define P_E18_TRIS				TRISEbits.TRISE5		// MMBasic ext i/o pin 18
#define P_E18_OC    			ODCEbits.ODCE5	 
#define P_E18_OUT					LATEbits.LATE5
#define P_E18_IN					PORTEbits.RE5
#define P_E18_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE5_MASK else TRISECLR = _TRISE_TRISE5_MASK;}
#define P_E18_TRISINP			{TRISESET = _TRISE_TRISE5_MASK;}
#define P_E18_TRISOUT			{TRISECLR = _TRISE_TRISE5_MASK;}
#define P_E18_WRITESET(m)	{if(m) LATESET = _LATE_LATE5_MASK else LATECLR = _LATE_LATE5_MASK;}
#define P_E18_WRITE1			{LATESET = _LATE_LATE5_MASK;}
#define P_E18_WRITE0			{LATECLR = _LATE_LATE5_MASK;}
#define P_E18_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE5_MASK else ODCECLR = _ODCE_ODCE5_MASK;}
#define P_E18_ODCON				{ODCESET = _ODCE_ODCE5_MASK;}
#define P_E18_ODCOFF			{ODCECLR = _ODCE_ODCE5_MASK;}
#define P_E18_READ				((PORTE & _PORTE_RE5_MASK) ? 1 : 0)

#define P_E19_TRIS				TRISDbits.TRISD14		// MMBasic ext i/o pin 19
#define P_E19_OC    			ODCDbits.ODCD14
#define P_E19_OUT					LATDbits.LATD14
#define P_E19_IN					PORTDbits.RD14
#define P_E19_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD14_MASK else TRISDCLR = _TRISD_TRISD14_MASK;}
#define P_E19_TRISINP			{TRISDSET = _TRISD_TRISD14_MASK;}
#define P_E19_TRISOUT			{TRISDCLR = _TRISD_TRISD14_MASK;}
#define P_E19_WRITESET(m)	{if(m) LATDSET = _LATD_LATD14_MASK else LATDCLR = _LATD_LATD14_MASK;}
#define P_E19_WRITE1			{LATDSET = _LATD_LATD14_MASK;}
#define P_E19_WRITE0			{LATDCLR = _LATD_LATD14_MASK;}
#define P_E19_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD14_MASK else ODCDCLR = _ODCD_ODCD14_MASK;}
#define P_E19_ODCON				{ODCDSET = _ODCD_ODCD14_MASK;}
#define P_E19_ODCOFF			{ODCDCLR = _ODCD_ODCD14_MASK;}
#define P_E19_READ				((PORTD & _PORTD_RD14_MASK) ? 1 : 0)

#define P_E20_TRIS				TRISDbits.TRISD15		// MMBasic ext i/o pin 20
#define P_E20_OC    			ODCDbits.ODCD15
#define P_E20_OUT					LATDbits.LATD15
#define P_E20_IN					PORTDbits.RD15
#define P_E20_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD15_MASK else TRISDCLR = _TRISD_TRISD15_MASK;}
#define P_E20_TRISINP			{TRISDSET = _TRISD_TRISD15_MASK;}
#define P_E20_TRISOUT			{TRISDCLR = _TRISD_TRISD15_MASK;}
#define P_E20_WRITESET(m)	{if(m) LATDSET = _LATD_LATD15_MASK else LATDCLR = _LATD_LATD15_MASK;}
#define P_E20_WRITE1			{LATDSET = _LATD_LATD15_MASK;}
#define P_E20_WRITE0			{LATDCLR = _LATD_LATD15_MASK;}
#define P_E20_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD15_MASK else ODCDCLR = _ODCD_ODCD15_MASK;}
#define P_E20_ODCON				{ODCDSET = _ODCD_ODCD15_MASK;}
#define P_E20_ODCOFF			{ODCDCLR = _ODCD_ODCD15_MASK;}
#define P_E20_READ				((PORTD & _PORTD_RD15_MASK) ? 1 : 0)

#define P_E21_TRIS       	TRISAbits.TRISA0       // MMBasic ext i/o pin 21
#define P_E21_OC         	ODCAbits.ODCA0
#define P_E21_OUT        	LATAbits.LATA0
#define P_E21_IN         	PORTAbits.RA0
#define P_E21_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA0_MASK else TRISACLR = _TRISA_TRISA0_MASK;}
#define P_E21_TRISINP			{TRISASET = _TRISA_TRISA0_MASK;}
#define P_E21_TRISOUT			{TRISACLR = _TRISA_TRISA0_MASK;}
#define P_E21_WRITESET(m)	{if(m) LATASET = _LATA_LATA0_MASK else LATACLR = _LATA_LATA0_MASK;}
#define P_E21_WRITE1			{LATASET = _LATA_LATA0_MASK;}
#define P_E21_WRITE0			{LATACLR = _LATA_LATA0_MASK;}
#define P_E21_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA0_MASK else ODCACLR = _ODCA_ODCA0_MASK;}
#define P_E21_ODCON				{ODCASET = _ODCA_ODCA0_MASK;}
#define P_E21_ODCOFF			{ODCACLR = _ODCA_ODCA0_MASK;}
#define P_E21_READ				((PORTA & _PORTA_RA0_MASK) ? 1 : 0)

#define P_E22_TRIS       	TRISAbits.TRISA1       // MMBasic ext i/o pin 22
#define P_E22_OC         	ODCAbits.ODCA1
#define P_E22_OUT        	LATAbits.LATA1
#define P_E22_IN         	PORTAbits.RA1
#define P_E22_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA1_MASK else TRISACLR = _TRISA_TRISA1_MASK;}
#define P_E22_TRISINP			{TRISASET = _TRISA_TRISA1_MASK;}
#define P_E22_TRISOUT			{TRISACLR = _TRISA_TRISA1_MASK;}
#define P_E22_WRITESET(m)	{if(m) LATASET = _LATA_LATA1_MASK else LATACLR = _LATA_LATA1_MASK;}
#define P_E22_WRITE1			{LATASET = _LATA_LATA1_MASK;}
#define P_E22_WRITE0			{LATACLR = _LATA_LATA1_MASK;}
#define P_E22_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA1_MASK else ODCACLR = _ODCA_ODCA1_MASK;}
#define P_E22_ODCON				{ODCASET = _ODCA_ODCA1_MASK;}
#define P_E22_ODCOFF			{ODCACLR = _ODCA_ODCA1_MASK;}
#define P_E22_READ				((PORTA & _PORTA_RA1_MASK) ? 1 : 0)

#define P_E23_TRIS       	TRISAbits.TRISA2       // MMBasic ext i/o pin 23
#define P_E23_OC         	ODCAbits.ODCA2
#define P_E23_OUT        	LATAbits.LATA2
#define P_E23_IN         	PORTAbits.RA2
#define P_E23_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA2_MASK else TRISACLR = _TRISA_TRISA2_MASK;}
#define P_E23_TRISINP			{TRISASET = _TRISA_TRISA2_MASK;}
#define P_E23_TRISOUT			{TRISACLR = _TRISA_TRISA2_MASK;}
#define P_E23_WRITESET(m)	{if(m) LATASET = _LATA_LATA2_MASK else LATACLR = _LATA_LATA2_MASK;}
#define P_E23_WRITE1			{LATASET = _LATA_LATA2_MASK;}
#define P_E23_WRITE0			{LATACLR = _LATA_LATA2_MASK;}
#define P_E23_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA2_MASK else ODCACLR = _ODCA_ODCA2_MASK;}
#define P_E23_ODCON				{ODCASET = _ODCA_ODCA2_MASK;}
#define P_E23_ODCOFF			{ODCACLR = _ODCA_ODCA2_MASK;}
#define P_E23_READ				((PORTA & _PORTA_RA2_MASK) ? 1 : 0)

#define P_E24_TRIS       	TRISAbits.TRISA3       // MMBasic ext i/o pin 24
#define P_E24_OC         	ODCAbits.ODCA3
#define P_E24_OUT        	LATAbits.LATA3
#define P_E24_IN         	PORTAbits.RA3
#define P_E24_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA3_MASK else TRISACLR = _TRISA_TRISA3_MASK;}
#define P_E24_TRISINP			{TRISASET = _TRISA_TRISA3_MASK;}
#define P_E24_TRISOUT			{TRISACLR = _TRISA_TRISA3_MASK;}
#define P_E24_WRITESET(m)	{if(m) LATASET = _LATA_LATA3_MASK else LATACLR = _LATA_LATA3_MASK;}
#define P_E24_WRITE1			{LATASET = _LATA_LATA3_MASK;}
#define P_E24_WRITE0			{LATACLR = _LATA_LATA3_MASK;}
#define P_E24_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA3_MASK else ODCACLR = _ODCA_ODCA3_MASK;}
#define P_E24_ODCON				{ODCASET = _ODCA_ODCA3_MASK;}
#define P_E24_ODCOFF			{ODCACLR = _ODCA_ODCA3_MASK;}
#define P_E24_READ				((PORTA & _PORTA_RA3_MASK) ? 1 : 0)

#define P_E25_TRIS       	TRISAbits.TRISA4       // MMBasic ext i/o pin 25
#define P_E25_OC         	ODCAbits.ODCA4
#define P_E25_OUT        	LATAbits.LATA4
#define P_E25_IN         	PORTAbits.RA4
#define P_E25_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA4_MASK else TRISACLR = _TRISA_TRISA4_MASK;}
#define P_E25_TRISINP			{TRISASET = _TRISA_TRISA4_MASK;}
#define P_E25_TRISOUT			{TRISACLR = _TRISA_TRISA4_MASK;}
#define P_E25_WRITESET(m)	{if(m) LATASET = _LATA_LATA4_MASK else LATACLR = _LATA_LATA4_MASK;}
#define P_E25_WRITE1			{LATASET = _LATA_LATA4_MASK;}
#define P_E25_WRITE0			{LATACLR = _LATA_LATA4_MASK;}
#define P_E25_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA4_MASK else ODCACLR = _ODCA_ODCA4_MASK;}
#define P_E25_ODCON				{ODCASET = _ODCA_ODCA4_MASK;}
#define P_E25_ODCOFF			{ODCACLR = _ODCA_ODCA4_MASK;}
#define P_E25_READ				((PORTA & _PORTA_RA4_MASK) ? 1 : 0)

#define P_E26_TRIS       	TRISAbits.TRISA5       // MMBasic ext i/o pin 26
#define P_E26_OC         	ODCAbits.ODCA5
#define P_E26_OUT        	LATAbits.LATA5
#define P_E26_IN         	PORTAbits.RA5
#define P_E26_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA5_MASK else TRISACLR = _TRISA_TRISA5_MASK;}
#define P_E26_TRISINP			{TRISASET = _TRISA_TRISA5_MASK;}
#define P_E26_TRISOUT			{TRISACLR = _TRISA_TRISA5_MASK;}
#define P_E26_WRITESET(m)	{if(m) LATASET = _LATA_LATA5_MASK else LATACLR = _LATA_LATA5_MASK;}
#define P_E26_WRITE1			{LATASET = _LATA_LATA5_MASK;}
#define P_E26_WRITE0			{LATACLR = _LATA_LATA5_MASK;}
#define P_E26_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA5_MASK else ODCACLR = _ODCA_ODCA5_MASK;}
#define P_E26_ODCON				{ODCASET = _ODCA_ODCA5_MASK;}
#define P_E26_ODCOFF			{ODCACLR = _ODCA_ODCA5_MASK;}
#define P_E26_READ				((PORTA & _PORTA_RA5_MASK) ? 1 : 0)

#define P_E27_TRIS       	TRISAbits.TRISA6       // MMBasic ext i/o pin 27
#define P_E27_OC         	ODCAbits.ODCA6
#define P_E27_OUT        	LATAbits.LATA6
#define P_E27_IN         	PORTAbits.RA6
#define P_E27_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA6_MASK else TRISACLR = _TRISA_TRISA6_MASK;}
#define P_E27_TRISINP			{TRISASET = _TRISA_TRISA6_MASK;}
#define P_E27_TRISOUT			{TRISACLR = _TRISA_TRISA6_MASK;}
#define P_E27_WRITESET(m)	{if(m) LATASET = _LATA_LATA6_MASK else LATACLR = _LATA_LATA6_MASK;}
#define P_E27_WRITE1			{LATASET = _LATA_LATA6_MASK;}
#define P_E27_WRITE0			{LATACLR = _LATA_LATA6_MASK;}
#define P_E27_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA6_MASK else ODCACLR = _ODCA_ODCA6_MASK;}
#define P_E27_ODCON				{ODCASET = _ODCA_ODCA6_MASK;}
#define P_E27_ODCOFF			{ODCACLR = _ODCA_ODCA6_MASK;}
#define P_E27_READ				((PORTA & _PORTA_RA6_MASK) ? 1 : 0)

#define P_E28_TRIS       	TRISAbits.TRISA7       // MMBasic ext i/o pin 28
#define P_E28_OC         	ODCAbits.ODCA7
#define P_E28_OUT        	LATAbits.LATA7
#define P_E28_IN         	PORTAbits.RA7
#define P_E28_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA7_MASK else TRISACLR = _TRISA_TRISA7_MASK;}
#define P_E28_TRISINP			{TRISASET = _TRISA_TRISA7_MASK;}
#define P_E28_TRISOUT			{TRISACLR = _TRISA_TRISA7_MASK;}
#define P_E28_WRITESET(m)	{if(m) LATASET = _LATA_LATA7_MASK else LATACLR = _LATA_LATA7_MASK;}
#define P_E28_WRITE1			{LATASET = _LATA_LATA7_MASK;}
#define P_E28_WRITE0			{LATACLR = _LATA_LATA7_MASK;}
#define P_E28_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA7_MASK else ODCACLR = _ODCA_ODCA7_MASK;}
#define P_E28_ODCON				{ODCASET = _ODCA_ODCA7_MASK;}
#define P_E28_ODCOFF			{ODCACLR = _ODCA_ODCA7_MASK;}
#define P_E28_READ				((PORTA & _PORTA_RA7_MASK) ? 1 : 0)

#define P_E29_TRIS       	TRISAbits.TRISA9       // MMBasic ext i/o pin 29
#define P_E29_OC         	ODCAbits.ODCA9
#define P_E29_OUT        	LATAbits.LATA9
#define P_E29_IN         	PORTAbits.RA9
#define P_E29_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA9_MASK else TRISACLR = _TRISA_TRISA9_MASK;}
#define P_E29_TRISINP			{TRISASET = _TRISA_TRISA9_MASK;}
#define P_E29_TRISOUT			{TRISACLR = _TRISA_TRISA9_MASK;}
#define P_E29_WRITESET(m)	{if(m) LATASET = _LATA_LATA9_MASK else LATACLR = _LATA_LATA9_MASK;}
#define P_E29_WRITE1			{LATASET = _LATA_LATA9_MASK;}
#define P_E29_WRITE0			{LATACLR = _LATA_LATA9_MASK;}
#define P_E29_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA9_MASK else ODCACLR = _ODCA_ODCA9_MASK;}
#define P_E29_ODCON				{ODCASET = _ODCA_ODCA9_MASK;}
#define P_E29_ODCOFF			{ODCACLR = _ODCA_ODCA9_MASK;}
#define P_E29_READ				((PORTA & _PORTA_RA9_MASK) ? 1 : 0)

#define P_E30_TRIS       	TRISAbits.TRISA10       // MMBasic ext i/o pin 30
#define P_E30_OC         	ODCAbits.ODCA10
#define P_E30_OUT        	LATAbits.LATA10
#define P_E30_IN         	PORTAbits.RA10
#define P_E30_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA10_MASK else TRISACLR = _TRISA_TRISA10_MASK;}
#define P_E30_TRISINP			{TRISASET = _TRISA_TRISA10_MASK;}
#define P_E30_TRISOUT			{TRISACLR = _TRISA_TRISA10_MASK;}
#define P_E30_WRITESET(m)	{if(m) LATASET = _LATA_LATA10_MASK else LATACLR = _LATA_LATA10_MASK;}
#define P_E30_WRITE1			{LATASET = _LATA_LATA10_MASK;}
#define P_E30_WRITE0			{LATACLR = _LATA_LATA10_MASK;}
#define P_E30_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA10_MASK else ODCACLR = _ODCA_ODCA10_MASK;}
#define P_E30_ODCON				{ODCASET = _ODCA_ODCA10_MASK;}
#define P_E30_ODCOFF			{ODCACLR = _ODCA_ODCA10_MASK;}
#define P_E30_READ				((PORTA & _PORTA_RA10_MASK) ? 1 : 0)

#define P_E31_TRIS				TRISAbits.TRISA14       // MMBasic ext i/o pin 31
#define P_E31_OC					ODCAbits.ODCA14
#define P_E31_OUT					LATAbits.LATA14
#define P_E31_IN					PORTAbits.RA14
#define P_E31_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA14_MASK else TRISACLR = _TRISA_TRISA14_MASK;}
#define P_E31_TRISINP			{TRISASET = _TRISA_TRISA14_MASK;}
#define P_E31_TRISOUT			{TRISACLR = _TRISA_TRISA14_MASK;}
#define P_E31_WRITESET(m)	{if(m) LATASET = _LATA_LATA14_MASK else LATACLR = _LATA_LATA14_MASK;}
#define P_E31_WRITE1			{LATASET = _LATA_LATA14_MASK;}
#define P_E31_WRITE0			{LATACLR = _LATA_LATA14_MASK;}
#define P_E31_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA14_MASK else ODCACLR = _ODCA_ODCA14_MASK;}
#define P_E31_ODCON				{ODCASET = _ODCA_ODCA14_MASK;}
#define P_E31_ODCOFF			{ODCACLR = _ODCA_ODCA14_MASK;}
#define P_E31_READ				((PORTA & _PORTA_RA14_MASK) ? 1 : 0)

#define P_E32_TRIS				TRISAbits.TRISA15       // MMBasic ext i/o pin 32
#define P_E32_OC					ODCAbits.ODCA15
#define P_E32_OUT					LATAbits.LATA15
#define P_E32_IN					PORTAbits.RA15
#define P_E32_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA15_MASK else TRISACLR = _TRISA_TRISA15_MASK;}
#define P_E32_TRISINP			{TRISASET = _TRISA_TRISA15_MASK;}
#define P_E32_TRISOUT			{TRISACLR = _TRISA_TRISA15_MASK;}
#define P_E32_WRITESET(m)	{if(m) LATASET = _LATA_LATA15_MASK else LATACLR = _LATA_LATA15_MASK;}
#define P_E32_WRITE1			{LATASET = _LATA_LATA15_MASK;}
#define P_E32_WRITE0			{LATACLR = _LATA_LATA15_MASK;}
#define P_E32_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA15_MASK else ODCACLR = _ODCA_ODCA15_MASK;}
#define P_E32_ODCON				{ODCASET = _ODCA_ODCA15_MASK;}
#define P_E32_ODCOFF			{ODCACLR = _ODCA_ODCA15_MASK;}
#define P_E32_READ				((PORTA & _PORTA_RA15_MASK) ? 1 : 0)

#define P_E33_TRIS				TRISCbits.TRISC1       // MMBasic ext i/o pin 33
#define P_E33_OC					ODCCbits.ODCC1
#define P_E33_OUT					LATCbits.LATC1
#define P_E33_IN					PORTCbits.RC1
#define P_E33_TRISSET(m)	{if(m) TRISCSET = _TRISC_TRISC1_MASK else TRISCCLR = _TRISC_TRISC1_MASK;}
#define P_E33_TRISINP			{TRISCSET = _TRISC_TRISC1_MASK;}
#define P_E33_TRISOUT			{TRISCCLR = _TRISC_TRISC1_MASK;}
#define P_E33_WRITESET(m)	{if(m) LATCSET = _LATC_LATC1_MASK else LATCCLR = _LATC_LATC1_MASK;}
#define P_E33_WRITE1			{LATCSET = _LATC_LATC1_MASK;}
#define P_E33_WRITE0			{LATCCLR = _LATC_LATC1_MASK;}
#define P_E33_ODCSET(m)		{if(m) ODCCSET = _ODCC_ODCC1_MASK else ODCCCLR = _ODCC_ODCC1_MASK;}
#define P_E33_ODCON				{ODCCSET = _ODCC_ODCC1_MASK;}
#define P_E33_ODCOFF			{ODCCCLR = _ODCC_ODCC1_MASK;}
#define P_E33_READ				((PORTC & _PORTC_RC1_MASK) ? 1 : 0)

#define P_E34_TRIS				TRISCbits.TRISC2       // MMBasic ext i/o pin 34
#define P_E34_OC					ODCCbits.ODCC2
#define P_E34_OUT					LATCbits.LATC2
#define P_E34_IN					PORTCbits.RC2
#define P_E34_TRISSET(m)	{if(m) TRISCSET = _TRISC_TRISC2_MASK else TRISCCLR = _TRISC_TRISC2_MASK;}
#define P_E34_TRISINP			{TRISCSET = _TRISC_TRISC2_MASK;}
#define P_E34_TRISOUT			{TRISCCLR = _TRISC_TRISC2_MASK;}
#define P_E34_WRITESET(m)	{if(m) LATCSET = _LATC_LATC2_MASK else LATCCLR = _LATC_LATC2_MASK;}
#define P_E34_WRITE1			{LATCSET = _LATC_LATC2_MASK;}
#define P_E34_WRITE0			{LATCCLR = _LATC_LATC2_MASK;}
#define P_E34_ODCSET(m)		{if(m) ODCCSET = _ODCC_ODCC2_MASK else ODCCCLR = _ODCC_ODCC2_MASK;}
#define P_E34_ODCON				{ODCCSET = _ODCC_ODCC2_MASK;}
#define P_E34_ODCOFF			{ODCCCLR = _ODCC_ODCC2_MASK;}
#define P_E34_READ				((PORTC & _PORTC_RC2_MASK) ? 1 : 0)

#define P_E35_TRIS				TRISCbits.TRISC3       // MMBasic ext i/o pin 35
#define P_E35_OC					ODCCbits.ODCC3
#define P_E35_OUT					LATCbits.LATC3
#define P_E35_IN					PORTCbits.RC3
#define P_E35_TRISSET(m)	{if(m) TRISCSET = _TRISC_TRISC3_MASK else TRISCCLR = _TRISC_TRISC3_MASK;}
#define P_E35_TRISINP			{TRISCSET = _TRISC_TRISC3_MASK;}
#define P_E35_TRISOUT			{TRISCCLR = _TRISC_TRISC3_MASK;}
#define P_E35_WRITESET(m)	{if(m) LATCSET = _LATC_LATC3_MASK else LATCCLR = _LATC_LATC3_MASK;}
#define P_E35_WRITE1			{LATCSET = _LATC_LATC3_MASK;}
#define P_E35_WRITE0			{LATCCLR = _LATC_LATC3_MASK;}
#define P_E35_ODCSET(m)		{if(m) ODCCSET = _ODCC_ODCC3_MASK else ODCCCLR = _ODCC_ODCC3_MASK;}
#define P_E35_ODCON				{ODCCSET = _ODCC_ODCC3_MASK;}
#define P_E35_ODCOFF			{ODCCCLR = _ODCC_ODCC3_MASK;}
#define P_E35_READ				((PORTC & _PORTC_RC3_MASK) ? 1 : 0)

#define P_E36_TRIS				TRISCbits.TRISC4       // MMBasic ext i/o pin 36
#define P_E36_OC					ODCCbits.ODCC4
#define P_E36_OUT					LATCbits.LATC4
#define P_E36_IN					PORTCbits.RC4
#define P_E36_TRISSET(m)	{if(m) TRISCSET = _TRISC_TRISC4_MASK else TRISCCLR = _TRISC_TRISC4_MASK;}
#define P_E36_TRISINP			{TRISCSET = _TRISC_TRISC4_MASK;}
#define P_E36_TRISOUT			{TRISCCLR = _TRISC_TRISC4_MASK;}
#define P_E36_WRITESET(m)	{if(m) LATCSET = _LATC_LATC4_MASK else LATCCLR = _LATC_LATC4_MASK;}
#define P_E36_WRITE1			{LATCSET = _LATC_LATC4_MASK;}
#define P_E36_WRITE0			{LATCCLR = _LATC_LATC4_MASK;}
#define P_E36_ODCSET(m)		{if(m) ODCCSET = _ODCC_ODCC4_MASK else ODCCCLR = _ODCC_ODCC4_MASK;}
#define P_E36_ODCON				{ODCCSET = _ODCC_ODCC4_MASK;}
#define P_E36_ODCOFF			{ODCCCLR = _ODCC_ODCC4_MASK;}
#define P_E36_READ				((PORTC & _PORTC_RC4_MASK) ? 1 : 0)

#define P_E37_TRIS				TRISEbits.TRISE9       // MMBasic ext i/o pin 37
#define P_E37_OC					ODCEbits.ODCE9
#define P_E37_OUT					LATEbits.LATE9
#define P_E37_IN					PORTEbits.RE9
#define P_E37_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE9_MASK else TRISECLR = _TRISE_TRISE9_MASK;}
#define P_E37_TRISINP			{TRISESET = _TRISE_TRISE9_MASK;}
#define P_E37_TRISOUT			{TRISECLR = _TRISE_TRISE9_MASK;}
#define P_E37_WRITESET(m)	{if(m) LATESET = _LATE_LATE9_MASK else LATECLR = _LATE_LATE9_MASK;}
#define P_E37_WRITE1			{LATESET = _LATE_LATE9_MASK;}
#define P_E37_WRITE0			{LATECLR = _LATE_LATE9_MASK;}
#define P_E37_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE9_MASK else ODCECLR = _ODCE_ODCE9_MASK;}
#define P_E37_ODCON				{ODCESET = _ODCE_ODCE9_MASK;}
#define P_E37_ODCOFF			{ODCECLR = _ODCE_ODCE9_MASK;}
#define P_E37_READ				((PORTE & _PORTE_RE9_MASK) ? 1 : 0)

#define P_E38_TRIS				TRISFbits.TRISF8       // MMBasic ext i/o pin 38
#define P_E38_OC					ODCFbits.ODCF8
#define P_E38_OUT					LATFbits.LATF8
#define P_E38_IN					PORTFbits.RF8
#define P_E38_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF8_MASK else TRISFCLR = _TRISF_TRISF8_MASK;}
#define P_E38_TRISINP			{TRISFSET = _TRISF_TRISF8_MASK;}
#define P_E38_TRISOUT			{TRISFCLR = _TRISF_TRISF8_MASK;}
#define P_E38_WRITESET(m)	{if(m) LATFSET = _LATF_LATF8_MASK else LATFCLR = _LATF_LATF8_MASK;}
#define P_E38_WRITE1			{LATFSET = _LATF_LATF8_MASK;}
#define P_E38_WRITE0			{LATFCLR = _LATF_LATF8_MASK;}
#define P_E38_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF8_MASK else ODCFCLR = _ODCF_ODCF8_MASK;}
#define P_E38_ODCON				{ODCFSET = _ODCF_ODCF8_MASK;}
#define P_E38_ODCOFF			{ODCFCLR = _ODCF_ODCF8_MASK;}
#define P_E38_READ				((PORTF & _PORTF_RF8_MASK) ? 1 : 0)

#define P_E39_TRIS				TRISFbits.TRISF12       // MMBasic ext i/o pin 39
#define P_E39_OC					ODCFbits.ODCF12
#define P_E39_OUT					LATFbits.LATF12
#define P_E39_IN					PORTFbits.RF12
#define P_E39_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF8_MASK else TRISFCLR = _TRISF_TRISF8_MASK;}
#define P_E39_TRISINP			{TRISFSET = _TRISF_TRISF8_MASK;}
#define P_E39_TRISOUT			{TRISFCLR = _TRISF_TRISF8_MASK;}
#define P_E39_WRITESET(m)	{if(m) LATFSET = _LATF_LATF8_MASK else LATFCLR = _LATF_LATF8_MASK;}
#define P_E39_WRITE1			{LATFSET = _LATF_LATF8_MASK;}
#define P_E39_WRITE0			{LATFCLR = _LATF_LATF8_MASK;}
#define P_E39_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF8_MASK else ODCFCLR = _ODCF_ODCF8_MASK;}
#define P_E39_ODCON				{ODCFSET = _ODCF_ODCF8_MASK;}
#define P_E39_ODCOFF			{ODCFCLR = _ODCF_ODCF8_MASK;}
#define P_E39_READ				((PORTF & _PORTF_RF8_MASK) ? 1 : 0)

#define P_E40_TRIS				TRISFbits.TRISF13       // MMBasic ext i/o pin 40
#define P_E40_OC					ODCFbits.ODCF13
#define P_E40_OUT					LATFbits.LATF13
#define P_E40_IN					PORTFbits.RF13
#define P_E40_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF13_MASK else TRISFCLR = _TRISF_TRISF13_MASK;}
#define P_E40_TRISINP			{TRISFSET = _TRISF_TRISF13_MASK;}
#define P_E40_TRISOUT			{TRISFCLR = _TRISF_TRISF13_MASK;}
#define P_E40_WRITESET(m)	{if(m) LATFSET = _LATF_LATF13_MASK else LATFCLR = _LATF_LATF13_MASK;}
#define P_E40_WRITE1			{LATFSET = _LATF_LATF13_MASK;}
#define P_E40_WRITE0			{LATFCLR = _LATF_LATF13_MASK;}
#define P_E40_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF13_MASK else ODCFCLR = _ODCF_ODCF13_MASK;}
#define P_E40_ODCON				{ODCFSET = _ODCF_ODCF13_MASK;}
#define P_E40_ODCOFF			{ODCFCLR = _ODCF_ODCF13_MASK;}
#define P_E40_READ				((PORTF & _PORTF_RF13_MASK) ? 1 : 0)

#define P_E41_TRIS				TRISGbits.TRISG0       // MMBasic ext i/o pin 41
#define P_E41_OC					ODCGbits.ODCG0
#define P_E41_OUT					LATGbits.LATG0
#define P_E41_IN					PORTGbits.RG0
#define P_E41_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG0_MASK else TRISGCLR = _TRISG_TRISG0_MASK;}
#define P_E41_TRISINP			{TRISGSET = _TRISG_TRISG0_MASK;}
#define P_E41_TRISOUT			{TRISGCLR = _TRISG_TRISG0_MASK;}
#define P_E41_WRITESET(m)	{if(m) LATGSET = _LATG_LATG0_MASK else LATGCLR = _LATG_LATG0_MASK;}
#define P_E41_WRITE1			{LATGSET = _LATG_LATG0_MASK;}
#define P_E41_WRITE0			{LATGCLR = _LATG_LATG0_MASK;}
#define P_E41_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG0_MASK else ODCGCLR = _ODCG_ODCG0_MASK;}
#define P_E41_ODCON				{ODCGSET = _ODCG_ODCG0_MASK;}
#define P_E41_ODCOFF			{ODCGCLR = _ODCG_ODCG0_MASK;}
#define P_E41_READ				((PORTG & _PORTG_RG0_MASK) ? 1 : 0)

#define P_E42_TRIS				TRISGbits.TRISG1       // MMBasic ext i/o pin 42
#define P_E42_OC					ODCGbits.ODCG1
#define P_E42_OUT					LATGbits.LATG1
#define P_E42_IN					PORTGbits.RG1
#define P_E42_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG1_MASK else TRISGCLR = _TRISG_TRISG1_MASK;}
#define P_E42_TRISINP			{TRISGSET = _TRISG_TRISG1_MASK;}
#define P_E42_TRISOUT			{TRISGCLR = _TRISG_TRISG1_MASK;}
#define P_E42_WRITESET(m)	{if(m) LATGSET = _LATG_LATG1_MASK else LATGCLR = _LATG_LATG1_MASK;}
#define P_E42_WRITE1			{LATGSET = _LATG_LATG1_MASK;}
#define P_E42_WRITE0			{LATGCLR = _LATG_LATG1_MASK;}
#define P_E42_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG1_MASK else ODCGCLR = _ODCG_ODCG1_MASK;}
#define P_E42_ODCON				{ODCGSET = _ODCG_ODCG1_MASK;}
#define P_E42_ODCOFF			{ODCGCLR = _ODCG_ODCG1_MASK;}
#define P_E42_READ				((PORTG & _PORTG_RG1_MASK) ? 1 : 0)

#define P_E43_TRIS				TRISGbits.TRISG12       // MMBasic ext i/o pin 43
#define P_E43_OC					ODCGbits.ODCG12
#define P_E43_OUT					LATGbits.LATG12
#define P_E43_IN					PORTGbits.RG12
#define P_E43_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG12_MASK else TRISGCLR = _TRISG_TRISG12_MASK;}
#define P_E43_TRISINP			{TRISGSET = _TRISG_TRISG12_MASK;}
#define P_E43_TRISOUT			{TRISGCLR = _TRISG_TRISG12_MASK;}
#define P_E43_WRITESET(m)	{if(m) LATGSET = _LATG_LATG12_MASK else LATGCLR = _LATG_LATG12_MASK;}
#define P_E43_WRITE1			{LATGSET = _LATG_LATG12_MASK;}
#define P_E43_WRITE0			{LATGCLR = _LATG_LATG12_MASK;}
#define P_E43_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG12_MASK else ODCGCLR = _ODCG_ODCG12_MASK;}
#define P_E43_ODCON				{ODCGSET = _ODCG_ODCG12_MASK;}
#define P_E43_ODCOFF			{ODCGCLR = _ODCG_ODCG12_MASK;}
#define P_E43_READ				((PORTG & _PORTG_RG12_MASK) ? 1 : 0)

#define P_E44_TRIS				TRISGbits.TRISG13       // MMBasic ext i/o pin 44
#define P_E44_OC					ODCGbits.ODCG13
#define P_E44_OUT					LATGbits.LATG13
#define P_E44_IN					PORTGbits.RG13
#define P_E44_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG13_MASK else TRISGCLR = _TRISG_TRISG13_MASK;}
#define P_E44_TRISINP			{TRISGSET = _TRISG_TRISG13_MASK;}
#define P_E44_TRISOUT			{TRISGCLR = _TRISG_TRISG13_MASK;}
#define P_E44_WRITESET(m)	{if(m) LATGSET = _LATG_LATG13_MASK else LATGCLR = _LATG_LATG13_MASK;}
#define P_E44_WRITE1			{LATGSET = _LATG_LATG13_MASK;}
#define P_E44_WRITE0			{LATGCLR = _LATG_LATG13_MASK;}
#define P_E44_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG13_MASK else ODCGCLR = _ODCG_ODCG13_MASK;}
#define P_E44_ODCON				{ODCGSET = _ODCG_ODCG13_MASK;}
#define P_E44_ODCOFF			{ODCGCLR = _ODCG_ODCG13_MASK;}
#define P_E44_READ				((PORTG & _PORTG_RG13_MASK) ? 1 : 0)

#define P_E45_TRIS				TRISGbits.TRISG14       // MMBasic ext i/o pin 45
#define P_E45_OC					ODCGbits.ODCG14
#define P_E45_OUT					LATGbits.LATG14
#define P_E45_IN					PORTGbits.RG14
#define P_E45_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG14_MASK else TRISGCLR = _TRISG_TRISG14_MASK;}
#define P_E45_TRISINP			{TRISGSET = _TRISG_TRISG14_MASK;}
#define P_E45_TRISOUT			{TRISGCLR = _TRISG_TRISG14_MASK;}
#define P_E45_WRITESET(m)	{if(m) LATGSET = _LATG_LATG14_MASK else LATGCLR = _LATG_LATG14_MASK;}
#define P_E45_WRITE1			{LATGSET = _LATG_LATG14_MASK;}
#define P_E45_WRITE0			{LATGCLR = _LATG_LATG14_MASK;}
#define P_E45_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG14_MASK else ODCGCLR = _ODCG_ODCG14_MASK;}
#define P_E45_ODCON				{ODCGSET = _ODCG_ODCG14_MASK;}
#define P_E45_ODCOFF			{ODCGCLR = _ODCG_ODCG14_MASK;}
#define P_E45_READ				((PORTG & _PORTG_RG14_MASK) ? 1 : 0)

#define P_E46_TRIS				TRISGbits.TRISG15       // MMBasic ext i/o pin 46
#define P_E46_OC					ODCGbits.ODCG15
#define P_E46_OUT					LATGbits.LATG15
#define P_E46_IN					PORTGbits.RG15
#define P_E46_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG15_MASK else TRISGCLR = _TRISG_TRISG15_MASK;}
#define P_E46_TRISINP			{TRISGSET = _TRISG_TRISG15_MASK;}
#define P_E46_TRISOUT			{TRISGCLR = _TRISG_TRISG15_MASK;}
#define P_E46_WRITESET(m)	{if(m) LATGSET = _LATG_LATG15_MASK else LATGCLR = _LATG_LATG15_MASK;}
#define P_E46_WRITE1			{LATGSET = _LATG_LATG15_MASK;}
#define P_E46_WRITE0			{LATGCLR = _LATG_LATG15_MASK;}
#define P_E46_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG15_MASK else ODCGCLR = _ODCG_ODCG15_MASK;}
#define P_E46_ODCON				{ODCGSET = _ODCG_ODCG15_MASK;}
#define P_E46_ODCOFF			{ODCGCLR = _ODCG_ODCG15_MASK;}
#define P_E46_READ				((PORTG & _PORTG_RG15_MASK) ? 1 : 0)

#define P_E47_TRIS				TRISFbits.TRISF2       // MMBasic ext i/o pin 47
#define P_E47_OC					ODCFbits.ODCF2
#define P_E47_OUT					LATFbits.LATF2
#define P_E47_IN					PORTFbits.RF2
#define P_E47_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF2_MASK else TRISFCLR = _TRISF_TRISF2_MASK;}
#define P_E47_TRISINP			{TRISFSET = _TRISF_TRISF2_MASK;}
#define P_E47_TRISOUT			{TRISFCLR = _TRISF_TRISF2_MASK;}
#define P_E47_WRITESET(m)	{if(m) LATFSET = _LATF_LATF2_MASK else LATFCLR = _LATF_LATF2_MASK;}
#define P_E47_WRITE1			{LATFSET = _LATF_LATF2_MASK;}
#define P_E47_WRITE0			{LATFCLR = _LATF_LATF2_MASK;}
#define P_E47_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF2_MASK else ODCFCLR = _ODCF_ODCF2_MASK;}
#define P_E47_ODCON				{ODCFSET = _ODCF_ODCF2_MASK;}
#define P_E47_ODCOFF			{ODCFCLR = _ODCF_ODCF2_MASK;}
#define P_E47_READ				((PORTF & _PORTF_RF2_MASK) ? 1 : 0)

#define P_E48_TRIS				TRISEbits.TRISE2       // MMBasic ext i/o pin 48 (white LED)
#define P_E48_OC					ODCEbits.ODCE2
#define P_E48_OUT					LATEbits.LATE2
#define P_E48_IN					PORTEbits.RE2
#define P_E48_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE2_MASK else TRISECLR = _TRISE_TRISE2_MASK;}
#define P_E48_TRISINP			{TRISESET = _TRISE_TRISE2_MASK;}
#define P_E48_TRISOUT			{TRISECLR = _TRISE_TRISE2_MASK;}
#define P_E48_WRITESET(m)	{if(m) LATESET = _LATE_LATE2_MASK else LATECLR = _LATE_LATE2_MASK;}
#define P_E48_WRITE1			{LATESET = _LATE_LATE2_MASK;}
#define P_E48_WRITE0			{LATECLR = _LATE_LATE2_MASK;}
#define P_E48_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE2_MASK else ODCECLR = _ODCE_ODCE2_MASK;}
#define P_E48_ODCON				{ODCESET = _ODCE_ODCE2_MASK;}
#define P_E48_ODCOFF			{ODCECLR = _ODCE_ODCE2_MASK;}
#define P_E48_READ				((PORTE & _PORTE_RE2_MASK) ? 1 : 0)

#define P_E49_TRIS				TRISEbits.TRISE3       // MMBasic ext i/o pin 49 (green LED)
#define P_E49_OC					ODCEbits.ODCE3
#define P_E49_OUT					LATEbits.LATE3
#define P_E49_IN					PORTEbits.RE3
#define P_E49_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE3_MASK else TRISECLR = _TRISE_TRISE3_MASK;}
#define P_E49_TRISINP			{TRISESET = _TRISE_TRISE3_MASK;}
#define P_E49_TRISOUT			{TRISECLR = _TRISE_TRISE3_MASK;}
#define P_E49_WRITESET(m)	{if(m) LATESET = _LATE_LATE3_MASK else LATECLR = _LATE_LATE3_MASK;}
#define P_E49_WRITE1			{LATESET = _LATE_LATE3_MASK;}
#define P_E49_WRITE0			{LATECLR = _LATE_LATE3_MASK;}
#define P_E49_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE3_MASK else ODCECLR = _ODCE_ODCE3_MASK;}
#define P_E49_ODCON				{ODCESET = _ODCE_ODCE3_MASK;}
#define P_E49_ODCOFF			{ODCECLR = _ODCE_ODCE3_MASK;}
#define P_E49_READ				((PORTE & _PORTE_RE3_MASK) ? 1 : 0)

#define P_E50_TRIS				TRISEbits.TRISE6       // MMBasic ext i/o pin 50 (user pushbutton)
#define P_E50_OC					ODCEbits.ODCE6
#define P_E50_OUT					LATEbits.LATE6
#define P_E50_IN					PORTEbits.RE6
#define P_E50_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE6_MASK else TRISECLR = _TRISE_TRISE6_MASK;}
#define P_E50_TRISINP			{TRISESET = _TRISE_TRISE6_MASK;}
#define P_E50_TRISOUT			{TRISECLR = _TRISE_TRISE6_MASK;}
#define P_E50_WRITESET(m)	{if(m) LATESET = _LATE_LATE6_MASK else LATECLR = _LATE_LATE6_MASK;}
#define P_E50_WRITE1			{LATESET = _LATE_LATE6_MASK;}
#define P_E50_WRITE0			{LATECLR = _LATE_LATE6_MASK;}
#define P_E50_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE6_MASK else ODCECLR = _ODCE_ODCE6_MASK;}
#define P_E50_ODCON				{ODCESET = _ODCE_ODCE6_MASK;}
#define P_E50_ODCOFF			{ODCECLR = _ODCE_ODCE6_MASK;}
#define P_E50_READ				((PORTE & _PORTE_RE6_MASK) ? 1 : 0)
                

// Keyboard definitions
#define P_PS2CLK					PORTDbits.RD6			// Pin 54 input  - Keyboard clock
#define P_PS2CLK_PULLUP		CNPUEbits.CNPUE15
#define P_PS2CLK_INT			CNENbits.CNEN15

#define P_PS2DAT					PORTDbits.RD7			// Pin 55 input  - Keyboard data
#define P_PS2DAT_PULLUP		CNPUEbits.CNPUE16


// video defines
#define P_VGA_COMP				PORTCbits.RC14			// VGA/Composite jumper
#define P_VGA_SELECT			1						// state when VGA selected
#define P_VGA_COMP_PULLUP	CNPUEbits.CNPUE0

#define P_VIDEO_SPI				2						// the SPI peripheral used for video.  note: pin G9 is automatically set as the framing input
#define P_SPI_INPUT				SPI2ABUF				// input buffer for the SPI peripheral
#define P_SPI_INTERRUPT		_SPI2A_TX_IRQ			// interrupt used by the video DMA

#define P_VIDEO						PORTGbits.RG8			// video
#define P_VIDEO_TRIS			TRISGbits.TRISG8

#define P_HORIZ						PORTDbits.RD2					// horizontal sync
#define P_HORIZ_TRIS			TRISDbits.TRISD2

#define P_VERT_SET_HI			LATFSET = (1 << 1)		// set vert sync hi
#define P_VERT_SET_LO			LATFCLR = (1 << 1)		// set vert sync lo
#define P_VERT_TRIS				TRISFbits.TRISF1


// SD card defines
// This file is included in SDCard/HardwareProfile.h and replaces the defines in that file
#define P_SD_LED_SET_HI			LATECLR = (1 << 1)		// SD card activity led - Note: Inverted
#define P_SD_LED_SET_LO			LATESET = (1 << 1)		// SD card activity led - Note: Inverted
#define P_SD_ACTIVITY_TRIS	TRISEbits.TRISE1

#define SD_CS_SET_HI				LATESET = (1 << 8)		// SD-SPI Chip Set Output bit high
#define SD_CS_SET_LO				LATECLR = (1 << 8)		// SD-SPI Set Output bit low
#define SD_CS_TRIS					TRISEbits.TRISE8		// SD-SPI Chip Select TRIS bit

#define SD_CD								PORTDbits.RD4			// SD-SPI Card Detect Input bit
#define SD_CD_TRIS					TRISDbits.TRISD4		// SD-SPI Card Detect TRIS bit

#define SD_WE								PORTDbits.RD5			// SD-SPI Write Protect Check Input bit
#define SD_WE_TRIS					TRISDbits.TRISD5		// SD-SPI Write Protect Check TRIS bit

#define SPICON1							SPI3ACON				// The main SPI control register
#define SPISTAT							SPI3ASTAT				// The SPI status register
#define SPIBUF							SPI3ABUF				// The SPI Buffer
#define SPISTAT_RBF					SPI3ASTATbits.SPIRBF	// The receive buffer full bit in the SPI status register
#define SPICON1bits					SPI3ACONbits			// The bitwise define for the SPI control register (i.e. _____bits)
#define SPISTATbits					SPI3ASTATbits			// The bitwise define for the SPI status register (i.e. _____bits)
#define SPIENABLE						SPI3ACONbits.ON			// The enable bit for the SPI module
#define SPIBRG							SPI3ABRG				// The definition for the SPI baud rate generator register (PIC32)

#define SPICLOCK						TRISBbits.TRISB14		// The TRIS bit for the SCK pin
#define SPIIN								TRISFbits.TRISF4		// The TRIS bit for the SDI pin
#define SPIOUT							TRISFbits.TRISF5		// The TRIS bit for the SDO pin

#define putcSPI(spichar)		SpiChnPutC(SPI_CHANNEL3A, spichar)	//SPI library functions
#define getcSPI()						SpiChnGetC(SPI_CHANNEL3A)
#define OpenSPI(config1, config2)	SpiChnOpen(SPI_CHANNEL3A, config1, config2)


// Serial ports defines
#define P_COM1_RX_PIN_NBR		15
#define P_COM1_RX_PORT			PORTDbits.RD12

#define P_COM1_TX_PIN_NBR		16
#define P_COM1_TX_SET_HI		LATDSET = (1 << 13)
#define P_COM1_TX_SET_LO		LATDCLR = (1 << 13)

#define P_COM1_RTS_PIN_NBR	17
#define P_COM1_RTS_SET_HI		LATESET = (1 << 4)
#define P_COM1_RTS_SET_LO		LATECLR = (1 << 4)

#define P_COM1_CTS_PIN_NBR	18
#define P_COM1_CTS_PORT			PORTEbits.RE5

#define P_COM2_RX_PIN_NBR		19
#define P_COM2_RX_PORT			PORTDbits.RD14

#define P_COM2_TX_PIN_NBR		20
#define P_COM2_TX_SET_HI		LATDSET = (1 << 15)
#define P_COM2_TX_SET_LO		LATDCLR = (1 << 15)


// sound output
#define P_SOUND_TRIS				TRISDbits.TRISD1
