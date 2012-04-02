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
#define P_BUTTON_IN				PORTCbits.RC13			// this is the push button
#define P_LED_OUT					LATFbits.LATF0			// this is the LED
#define P_LED_TRIS				TRISFbits.TRISF0

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

#define P_E3_TRIS					TRISBbits.TRISB6		// MMBasic ext i/o pin 3
#define P_E3_ANALOG				AD1PCFGbits.PCFG6
#define P_E3_ACHAN				6
#define P_E3_OC	    			ODCBbits.ODCB6
#define P_E3_OUT					LATBbits.LATB6
#define P_E3_IN						PORTBbits.RB6
#define P_E3_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB6_MASK else TRISBCLR = _TRISB_TRISB6_MASK;}
#define P_E3_TRISINP			{TRISBSET = _TRISB_TRISB6_MASK;}
#define P_E3_TRISOUT			{TRISBCLR = _TRISB_TRISB6_MASK;}
#define P_E3_WRITESET(m)	{if(m) LATBSET = _LATB_LATB6_MASK else LATBCLR = _LATB_LATB6_MASK;}
#define P_E3_WRITE1				{LATBSET = _LATB_LATB6_MASK;}
#define P_E3_WRITE0				{LATBCLR = _LATB_LATB6_MASK;}
#define P_E3_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB6_MASK else ODCBCLR = _ODCB_ODCB6_MASK;}
#define P_E3_ODCON				{ODCBSET = _ODCB_ODCB6_MASK;}
#define P_E3_ODCOFF				{ODCBCLR = _ODCB_ODCB6_MASK;}
#define P_E3_READ					((PORTB & _PORTB_RB6_MASK) ? 1 : 0)

#define P_E4_TRIS					TRISBbits.TRISB7		// MMBasic ext i/o pin 4
#define P_E4_ANALOG				AD1PCFGbits.PCFG7
#define P_E4_ACHAN				7
#define P_E4_OC	    			ODCBbits.ODCB7
#define P_E4_OUT					LATBbits.LATB7
#define P_E4_IN						PORTBbits.RB7
#define P_E4_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB7_MASK else TRISBCLR = _TRISB_TRISB7_MASK;}
#define P_E4_TRISINP			{TRISBSET = _TRISB_TRISB7_MASK;}
#define P_E4_TRISOUT			{TRISBCLR = _TRISB_TRISB7_MASK;}
#define P_E4_WRITESET(m)	{if(m) LATBSET = _LATB_LATB7_MASK else LATBCLR = _LATB_LATB7_MASK;}
#define P_E4_WRITE1				{LATBSET = _LATB_LATB7_MASK;}
#define P_E4_WRITE0				{LATBCLR = _LATB_LATB7_MASK;}
#define P_E4_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB7_MASK else ODCBCLR = _ODCB_ODCB7_MASK;}
#define P_E4_ODCON				{ODCBSET = _ODCB_ODCB7_MASK;}
#define P_E4_ODCOFF				{ODCBCLR = _ODCB_ODCB7_MASK;}
#define P_E4_READ					((PORTB & _PORTB_RB7_MASK) ? 1 : 0)

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

#define P_E10_TRIS					TRISBbits.TRISB15		// MMBasic ext i/o pin 10
#define P_E10_ANALOG			AD1PCFGbits.PCFG15	 
#define P_E10_ACHAN				15
#define P_E10_OC    			ODCBbits.ODCB15
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

#define P_E15_TRIS				TRISEbits.TRISE2		// MMBasic ext i/o pin 15
#define P_E15_OC    			ODCEbits.ODCE2
#define P_E15_OUT					LATEbits.LATE2
#define P_E15_IN					PORTEbits.RE2
#define P_E15_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE2_MASK else TRISECLR = _TRISE_TRISE2_MASK;}
#define P_E15_TRISINP			{TRISESET = _TRISE_TRISE2_MASK;}
#define P_E15_TRISOUT			{TRISECLR = _TRISE_TRISE2_MASK;}
#define P_E15_WRITESET(m)	{if(m) LATESET = _LATE_LATE2_MASK else LATECLR = _LATE_LATE2_MASK;}
#define P_E15_WRITE1			{LATESET = _LATE_LATE2_MASK;}
#define P_E15_WRITE0			{LATECLR = _LATE_LATE2_MASK;}
#define P_E15_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE2_MASK else ODCECLR = _ODCE_ODCE2_MASK;}
#define P_E15_ODCON				{ODCESET = _ODCE_ODCE2_MASK;}
#define P_E15_ODCOFF			{ODCECLR = _ODCE_ODCE2_MASK;}
#define P_E15_READ				((PORTE & _PORTE_RE2_MASK) ? 1 : 0)

#define P_E16_TRIS				TRISEbits.TRISE3		// MMBasic ext i/o pin 16
#define P_E16_OC    			ODCEbits.ODCE3	 
#define P_E16_OUT					LATEbits.LATE3
#define P_E16_IN					PORTEbits.RE3
#define P_E16_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE3_MASK else TRISECLR = _TRISE_TRISE3_MASK;}
#define P_E16_TRISINP			{TRISESET = _TRISE_TRISE3_MASK;}
#define P_E16_TRISOUT			{TRISECLR = _TRISE_TRISE3_MASK;}
#define P_E16_WRITESET(m)	{if(m) LATESET = _LATE_LATE3_MASK else LATECLR = _LATE_LATE3_MASK;}
#define P_E16_WRITE1			{LATESET = _LATE_LATE3_MASK;}
#define P_E16_WRITE0			{LATECLR = _LATE_LATE3_MASK;}
#define P_E16_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE3_MASK else ODCECLR = _ODCE_ODCE3_MASK;}
#define P_E16_ODCON				{ODCESET = _ODCE_ODCE3_MASK;}
#define P_E16_ODCOFF			{ODCECLR = _ODCE_ODCE3_MASK;}
#define P_E16_READ				((PORTE & _PORTE_RE3_MASK) ? 1 : 0)

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

#define P_E19_TRIS				TRISEbits.TRISE6		// MMBasic ext i/o pin 19
#define P_E19_OC    			ODCEbits.ODCE6
#define P_E19_OUT					LATEbits.LATE6
#define P_E19_IN					PORTEbits.RE6
#define P_E19_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE6_MASK else TRISECLR = _TRISE_TRISE6_MASK;}
#define P_E19_TRISINP			{TRISESET = _TRISE_TRISE6_MASK;}
#define P_E19_TRISOUT			{TRISECLR = _TRISE_TRISE6_MASK;}
#define P_E19_WRITESET(m)	{if(m) LATESET = _LATE_LATE6_MASK else LATECLR = _LATE_LATE6_MASK;}
#define P_E19_WRITE1			{LATESET = _LATE_LATE6_MASK;}
#define P_E19_WRITE0			{LATECLR = _LATE_LATE6_MASK;}
#define P_E19_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE6_MASK else ODCECLR = _ODCE_ODCE6_MASK;}
#define P_E19_ODCON				{ODCESET = _ODCE_ODCE6_MASK;}
#define P_E19_ODCOFF			{ODCECLR = _ODCE_ODCE6_MASK;}
#define P_E19_READ				((PORTE & _PORTE_RE6_MASK) ? 1 : 0)

#define P_E20_TRIS				TRISEbits.TRISE7		// MMBasic ext i/o pin 20
#define P_E20_OC    			ODCEbits.ODCE7
#define P_E20_OUT					LATEbits.LATE7
#define P_E20_IN					PORTEbits.RE7
#define P_E20_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE7_MASK else TRISECLR = _TRISE_TRISE7_MASK;}
#define P_E20_TRISINP			{TRISESET = _TRISE_TRISE7_MASK;}
#define P_E20_TRISOUT			{TRISECLR = _TRISE_TRISE7_MASK;}
#define P_E20_WRITESET(m)	{if(m) LATESET = _LATE_LATE7_MASK else LATECLR = _LATE_LATE7_MASK;}
#define P_E20_WRITE1			{LATESET = _LATE_LATE7_MASK;}
#define P_E20_WRITE0			{LATECLR = _LATE_LATE7_MASK;}
#define P_E20_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE7_MASK else ODCECLR = _ODCE_ODCE7_MASK;}
#define P_E20_ODCON				{ODCESET = _ODCE_ODCE7_MASK;}
#define P_E20_ODCOFF			{ODCECLR = _ODCE_ODCE7_MASK;}
#define P_E20_READ				((PORTE & _PORTE_RE7_MASK) ? 1 : 0)


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
#define P_SD_LED_SET_HI			LATESET = (1 << 1)		// SD card activity led
#define P_SD_LED_SET_LO			LATECLR = (1 << 1)		// SD card activity led
#define P_SD_ACTIVITY_TRIS	TRISEbits.TRISE1

#define SD_CS_SET_HI				LATESET = (1 << 0)		// SD-SPI Chip Set Output bit high
#define SD_CS_SET_LO				LATECLR = (1 << 0)		// SD-SPI Set Output bit low
#define SD_CS_TRIS					TRISEbits.TRISE0		// SD-SPI Chip Select TRIS bit

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
#define P_COM1_RX_PORT			PORTEbits.RE2

#define P_COM1_TX_PIN_NBR		16
#define P_COM1_TX_SET_HI		LATESET = (1 << 3)
#define P_COM1_TX_SET_LO		LATECLR = (1 << 3)

#define P_COM1_RTS_PIN_NBR	17
#define P_COM1_RTS_SET_HI		LATESET = (1 << 4)
#define P_COM1_RTS_SET_LO		LATECLR = (1 << 4)

#define P_COM1_CTS_PIN_NBR	18
#define P_COM1_CTS_PORT			PORTEbits.RE5

#define P_COM2_RX_PIN_NBR		19
#define P_COM2_RX_PORT			PORTEbits.RE6

#define P_COM2_TX_PIN_NBR		20
#define P_COM2_TX_SET_HI		LATESET = (1 << 7)
#define P_COM2_TX_SET_LO		LATECLR = (1 << 7)


// sound output
#define P_SOUND_TRIS				TRISDbits.TRISD1
