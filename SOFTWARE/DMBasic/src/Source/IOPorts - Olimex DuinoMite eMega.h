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

Edit by SPP for Olimex DuinoMite eMega 19.03.2012

 ************************************************************************************************************************/


#ifndef IOPORTS_OLIMEX_DUINOMITE_EMEGA_H
#define IOPORTS_OLIMEX_DUINOMITE_EMEGA_H
//#include <p32xxxx.h>
//#include <plib.h>

/*
Note that we never use TRIS to set a pin as an input because that is the default after powerup
 */

// General defines
#define P_INPUT		1						// for setting the TRIS on I/O bits
#define P_OUTPUT	0
#define P_ON		1
#define P_OFF		0

#define	TRUE		1
#define	FALSE		0
#define	DIGITAL		1
#define	ANALOG		0


// Defines for the external I/O pins
#define P_BUTTON_IN			PORTEbits.RE8			// this is the push button
#define P_LED_OUT			LATBbits.LATB15			// this is the LED
#define P_LED_TRIS			TRISBbits.TRISB15

#define P_E1_TRIS			TRISBbits.TRISB2		// MMBasic ext i/o pin 1
#define	P_E1_AN_FUN			TRUE
#define P_E1_ANALOG			AD1PCFGbits.PCFG2
#define P_E1_ACHAN			2
#define P_E1_OC				ODCBbits.ODCB2
#define P_E1_OUT			LATBbits.LATB2
#define P_E1_IN				PORTBbits.RB2
#define P_E1_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB2_MASK else TRISBCLR = _TRISB_TRISB2_MASK;}
#define P_E1_TRISINP		{TRISBSET = _TRISB_TRISB2_MASK;}
#define P_E1_TRISOUT		{TRISBCLR = _TRISB_TRISB2_MASK;}
#define P_E1_WRITESET(m)	{if(m) LATBSET = _LATB_LATB2_MASK else LATBCLR = _LATB_LATB2_MASK;}
#define P_E1_WRITE1			{LATBSET = _LATB_LATB2_MASK;}
#define P_E1_WRITE0			{LATBCLR = _LATB_LATB2_MASK;}
#define P_E1_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB2_MASK else ODCBCLR = _ODCB_ODCB2_MASK;}
#define P_E1_ODCON			{ODCBSET = _ODCB_ODCB2_MASK;}
#define P_E1_ODCOFF			{ODCBCLR = _ODCB_ODCB2_MASK;}
#define P_E1_READ			((PORTB & _PORTB_RB2_MASK) ? 1 : 0)

#define P_E2_TRIS			TRISBbits.TRISB3		// MMBasic ext i/o pin 2
#define	P_E2_AN_FUN			TRUE
#define P_E2_ANALOG			AD1PCFGbits.PCFG3
#define P_E2_ACHAN			3
#define P_E2_OC				ODCBbits.ODCB3
#define P_E2_OUT			LATBbits.LATB3
#define P_E2_IN				PORTBbits.RB3
#define P_E2_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB3_MASK else TRISBCLR = _TRISB_TRISB3_MASK;}
#define P_E2_TRISINP		{TRISBSET = _TRISB_TRISB3_MASK;}
#define P_E2_TRISOUT		{TRISBCLR = _TRISB_TRISB3_MASK;}
#define P_E2_WRITESET(m)	{if(m) LATBSET = _LATB_LATB3_MASK else LATBCLR = _LATB_LATB3_MASK;}
#define P_E2_WRITE1			{LATBSET = _LATB_LATB3_MASK;}
#define P_E2_WRITE0			{LATBCLR = _LATB_LATB3_MASK;}
#define P_E2_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB3_MASK else ODCBCLR = _ODCB_ODCB3_MASK;}
#define P_E2_ODCON			{ODCBSET = _ODCB_ODCB3_MASK;}
#define P_E2_ODCOFF			{ODCBCLR = _ODCB_ODCB3_MASK;}
#define P_E2_READ			((PORTB & _PORTB_RB3_MASK) ? 1 : 0)

#define P_E3_TRIS			TRISBbits.TRISB4		// MMBasic ext i/o pin 3
#define	P_E3_AN_FUN			TRUE
#define P_E3_ANALOG			AD1PCFGbits.PCFG4
#define P_E3_ACHAN			4
#define P_E3_OC				ODCBbits.ODCB4
#define P_E3_OUT			LATBbits.LATB4
#define P_E3_IN				PORTBbits.RB4
#define P_E3_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB4_MASK else TRISBCLR = _TRISB_TRISB4_MASK;}
#define P_E3_TRISINP		{TRISBSET = _TRISB_TRISB4_MASK;}
#define P_E3_TRISOUT		{TRISBCLR = _TRISB_TRISB4_MASK;}
#define P_E3_WRITESET(m)	{if(m) LATBSET = _LATB_LATB4_MASK else LATBCLR = _LATB_LATB4_MASK;}
#define P_E3_WRITE1			{LATBSET = _LATB_LATB4_MASK;}
#define P_E3_WRITE0			{LATBCLR = _LATB_LATB4_MASK;}
#define P_E3_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB4_MASK else ODCBCLR = _ODCB_ODCB4_MASK;}
#define P_E3_ODCON			{ODCBSET = _ODCB_ODCB4_MASK;}
#define P_E3_ODCOFF			{ODCBCLR = _ODCB_ODCB4_MASK;}
#define P_E3_READ			((PORTB & _PORTB_RB4_MASK) ? 1 : 0)

#define P_E4_TRIS			TRISBbits.TRISB6		// MMBasic ext i/o pin 4
#define	P_E4_AN_FUN			TRUE
#define P_E4_ANALOG			AD1PCFGbits.PCFG6
#define P_E4_ACHAN			6
#define P_E4_OC				ODCBbits.ODCB6
#define P_E4_OUT			LATBbits.LATB6
#define P_E4_IN				PORTBbits.RB6
#define P_E4_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB6_MASK else TRISBCLR = _TRISB_TRISB6_MASK;}
#define P_E4_TRISINP		{TRISBSET = _TRISB_TRISB6_MASK;}
#define P_E4_TRISOUT		{TRISBCLR = _TRISB_TRISB6_MASK;}
#define P_E4_WRITESET(m)	{if(m) LATBSET = _LATB_LATB6_MASK else LATBCLR = _LATB_LATB6_MASK;}
#define P_E4_WRITE1			{LATBSET = _LATB_LATB6_MASK;}
#define P_E4_WRITE0			{LATBCLR = _LATB_LATB6_MASK;}
#define P_E4_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB6_MASK else ODCBCLR = _ODCB_ODCB6_MASK;}
#define P_E4_ODCON			{ODCBSET = _ODCB_ODCB6_MASK;}
#define P_E4_ODCOFF			{ODCBCLR = _ODCB_ODCB6_MASK;}
#define P_E4_READ			((PORTB & _PORTB_RB6_MASK) ? 1 : 0)

#define P_E5_TRIS			TRISBbits.TRISB7		// MMBasic ext i/o pin 5
#define	P_E5_AN_FUN			TRUE
#define P_E5_ANALOG			AD1PCFGbits.PCFG7
#define P_E5_ACHAN			7
#define P_E5_OC				ODCBbits.ODCB7
#define P_E5_OUT			LATBbits.LATB7
#define P_E5_IN				PORTBbits.RB7
#define P_E5_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB7_MASK else TRISBCLR = _TRISB_TRISB7_MASK;}
#define P_E5_TRISINP		{TRISBSET = _TRISB_TRISB7_MASK;}
#define P_E5_TRISOUT		{TRISBCLR = _TRISB_TRISB7_MASK;}
#define P_E5_WRITESET(m)	{if(m) LATBSET = _LATB_LATB7_MASK else LATBCLR = _LATB_LATB7_MASK;}
#define P_E5_WRITE1			{LATBSET = _LATB_LATB7_MASK;}
#define P_E5_WRITE0			{LATBCLR = _LATB_LATB7_MASK;}
#define P_E5_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB7_MASK else ODCBCLR = _ODCB_ODCB7_MASK;}
#define P_E5_ODCON			{ODCBSET = _ODCB_ODCB7_MASK;}
#define P_E5_ODCOFF			{ODCBCLR = _ODCB_ODCB7_MASK;}
#define P_E5_READ			((PORTB & _PORTB_RB7_MASK) ? 1 : 0)

#define P_E6_TRIS			TRISBbits.TRISB8		// MMBasic ext i/o pin 6
#define	P_E6_AN_FUN			TRUE
#define P_E6_ANALOG			AD1PCFGbits.PCFG8
#define P_E6_ACHAN			8
#define P_E6_OC				ODCBbits.ODCB8
#define P_E6_OUT			LATBbits.LATB8
#define P_E6_IN				PORTBbits.RB8
#define P_E6_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB8_MASK else TRISBCLR = _TRISB_TRISB8_MASK;}
#define P_E6_TRISINP		{TRISBSET = _TRISB_TRISB8_MASK;}
#define P_E6_TRISOUT		{TRISBCLR = _TRISB_TRISB8_MASK;}
#define P_E6_WRITESET(m)	{if(m) LATBSET = _LATB_LATB8_MASK else LATBCLR = _LATB_LATB8_MASK;}
#define P_E6_WRITE1			{LATBSET = _LATB_LATB8_MASK;}
#define P_E6_WRITE0			{LATBCLR = _LATB_LATB8_MASK;}
#define P_E6_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB8_MASK else ODCBCLR = _ODCB_ODCB8_MASK;}
#define P_E6_ODCON			{ODCBSET = _ODCB_ODCB8_MASK;}
#define P_E6_ODCOFF			{ODCBCLR = _ODCB_ODCB8_MASK;}
#define P_E6_READ			((PORTB & _PORTB_RB8_MASK) ? 1 : 0)

#define P_E7_TRIS			TRISBbits.TRISB10		// MMBasic ext i/o pin 7
#define	P_E7_AN_FUN			TRUE
#define P_E7_ANALOG			AD1PCFGbits.PCFG10
#define P_E7_ACHAN			10
#define P_E7_OC				ODCBbits.ODCB10
#define P_E7_OUT			LATBbits.LATB10
#define P_E7_IN				PORTBbits.RB10
#define P_E7_TRISSET(m)		{if(m) TRISBSET = _TRISB_TRISB10_MASK else TRISBCLR = _TRISB_TRISB10_MASK;}
#define P_E7_TRISINP		{TRISBSET = _TRISB_TRISB10_MASK;}
#define P_E7_TRISOUT		{TRISBCLR = _TRISB_TRISB10_MASK;}
#define P_E7_WRITESET(m)	{if(m) LATBSET = _LATB_LATB10_MASK else LATBCLR = _LATB_LATB10_MASK;}
#define P_E7_WRITE1			{LATBSET = _LATB_LATB10_MASK;}
#define P_E7_WRITE0			{LATBCLR = _LATB_LATB10_MASK;}
#define P_E7_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB10_MASK else ODCBCLR = _ODCB_ODCB10_MASK;}
#define P_E7_ODCON			{ODCBSET = _ODCB_ODCB10_MASK;}
#define P_E7_ODCOFF			{ODCBCLR = _ODCB_ODCB10_MASK;}
#define P_E7_READ			((PORTB & _PORTB_RB10_MASK) ? 1 : 0)

#define P_E8_TRIS			TRISDbits.TRISD12		// MMBasic ext i/o pin 8
#define	P_E8_AN_FUN			FALSE
#define P_E8_OC				ODCDbits.ODCD12
#define P_E8_OUT			LATDbits.LATD12
#define P_E8_IN				PORTDbits.RD12
#define P_E8_TRISSET(m)		{if(m) TRISDSET = _TRISD_TRISD12_MASK else TRISDCLR = _TRISD_TRISD12_MASK;}
#define P_E8_TRISINP		{TRISDSET = _TRISD_TRISD12_MASK;}
#define P_E8_TRISOUT		{TRISDCLR = _TRISD_TRISD12_MASK;}
#define P_E8_WRITESET(m)	{if(m) LATDSET = _LATD_LATD12_MASK else LATDCLR = _LATD_LATD12_MASK;}
#define P_E8_WRITE1			{LATDSET = _LATD_LATD12_MASK;}
#define P_E8_WRITE0			{LATDCLR = _LATD_LATD12_MASK;}
#define P_E8_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD12_MASK else ODCDCLR = _ODCD_ODCD12_MASK;}
#define P_E8_ODCON			{ODCDSET = _ODCD_ODCD12_MASK;}
#define P_E8_ODCOFF			{ODCDCLR = _ODCD_ODCD12_MASK;}
#define P_E8_READ			((PORTD & _PORTD_RD12_MASK) ? 1 : 0)

#define P_E9_TRIS			TRISDbits.TRISD13		// MMBasic ext i/o pin 9
#define	P_E9_AN_FUN			FALSE
#define P_E9_OC				ODCDbits.ODCD13
#define P_E9_OUT			LATDbits.LATD13
#define P_E9_IN				PORTDbits.RD13
#define P_E9_TRISSET(m)		{if(m) TRISDSET = _TRISD_TRISD13_MASK else TRISDCLR = _TRISD_TRISD13_MASK;}
#define P_E9_TRISINP		{TRISDSET = _TRISD_TRISD13_MASK;}
#define P_E9_TRISOUT		{TRISDCLR = _TRISD_TRISD13_MASK;}
#define P_E9_WRITESET(m)	{if(m) LATDSET = _LATD_LATD13_MASK else LATDCLR = _LATD_LATD13_MASK;}
#define P_E9_WRITE1			{LATDSET = _LATD_LATD13_MASK;}
#define P_E9_WRITE0			{LATDCLR = _LATD_LATD13_MASK;}
#define P_E9_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD13_MASK else ODCDCLR = _ODCD_ODCD13_MASK;}
#define P_E9_ODCON			{ODCDSET = _ODCD_ODCD13_MASK;}
#define P_E9_ODCOFF			{ODCDCLR = _ODCD_ODCD13_MASK;}
#define P_E9_READ			((PORTD & _PORTD_RD13_MASK) ? 1 : 0)

#define P_E10_TRIS			TRISAbits.TRISA9		// MMBasic ext i/o pin 10
#define	P_E10_AN_FUN		FALSE
#define P_E10_OC			ODCAbits.ODCA9
#define P_E10_OUT			LATAbits.LATA9
#define P_E10_IN			PORTAbits.RA9
#define P_E10_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA9_MASK else TRISACLR = _TRISA_TRISA9_MASK;}
#define P_E10_TRISINP		{TRISASET = _TRISA_TRISA9_MASK;}
#define P_E10_TRISOUT		{TRISACLR = _TRISA_TRISA9_MASK;}
#define P_E10_WRITESET(m)	{if(m) LATASET = _LATA_LATA9_MASK else LATACLR = _LATA_LATA9_MASK;}
#define P_E10_WRITE1		{LATASET = _LATA_LATA9_MASK;}
#define P_E10_WRITE0		{LATACLR = _LATA_LATA9_MASK;}
#define P_E10_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA9_MASK else ODCACLR = _ODCA_ODCA9_MASK;}
#define P_E10_ODCON			{ODCASET = _ODCA_ODCA9_MASK;}
#define P_E10_ODCOFF		{ODCACLR = _ODCA_ODCA9_MASK;}
#define P_E10_READ			((PORTA & _PORTA_RA9_MASK) ? 1 : 0)

#define P_E11_TRIS			TRISAbits.TRISA0		// MMBasic ext i/o pin 11
#define	P_E11_AN_FUN		FALSE
#define P_E11_OC			ODCAbits.ODCA0
#define P_E11_OUT			LATAbits.LATA0
#define P_E11_IN			PORTAbits.RA0
#define P_E11_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA0_MASK else TRISACLR = _TRISA_TRISA0_MASK;}
#define P_E11_TRISINP		{TRISASET = _TRISA_TRISA0_MASK;}
#define P_E11_TRISOUT		{TRISACLR = _TRISA_TRISA0_MASK;}
#define P_E11_WRITESET(m)	{if(m) LATASET = _LATA_LATA0_MASK else LATACLR = _LATA_LATA0_MASK;}
#define P_E11_WRITE1		{LATASET = _LATA_LATA0_MASK;}
#define P_E11_WRITE0		{LATACLR = _LATA_LATA0_MASK;}
#define P_E11_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA0_MASK else ODCACLR = _ODCA_ODCA0_MASK;}
#define P_E11_ODCON			{ODCASET = _ODCA_ODCA0_MASK;}
#define P_E11_ODCOFF		{ODCACLR = _ODCA_ODCA0_MASK;}
#define P_E11_READ			((PORTA & _PORTA_RA0_MASK) ? 1 : 0)

#define P_E12_TRIS			TRISAbits.TRISA1		// MMBasic ext i/o pin 12
#define	P_E12_AN_FUN		FALSE
#define P_E12_OC			ODCAbits.ODCA1
#define P_E12_OUT			LATAbits.LATA1
#define P_E12_IN			PORTAbits.RA1
#define P_E12_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA1_MASK else TRISACLR = _TRISA_TRISA1_MASK;}
#define P_E12_TRISINP		{TRISASET = _TRISA_TRISA1_MASK;}
#define P_E12_TRISOUT		{TRISACLR = _TRISA_TRISA1_MASK;}
#define P_E12_WRITESET(m)	{if(m) LATASET = _LATA_LATA1_MASK else LATACLR = _LATA_LATA1_MASK;}
#define P_E12_WRITE1		{LATASET = _LATA_LATA1_MASK;}
#define P_E12_WRITE0		{LATACLR = _LATA_LATA1_MASK;}
#define P_E12_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA1_MASK else ODCACLR = _ODCA_ODCA1_MASK;}
#define P_E12_ODCON			{ODCASET = _ODCA_ODCA1_MASK;}
#define P_E12_ODCOFF		{ODCACLR = _ODCA_ODCA1_MASK;}
#define P_E12_READ			((PORTA & _PORTA_RA1_MASK) ? 1 : 0)

#define P_E13_TRIS			TRISAbits.TRISA2		// MMBasic ext i/o pin 13
#define	P_E13_AN_FUN		FALSE
#define P_E13_OC			ODCAbits.ODCA2
#define P_E13_OUT			LATAbits.LATA2
#define P_E13_IN			PORTAbits.RA2
#define P_E13_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA2_MASK else TRISACLR = _TRISA_TRISA2_MASK;}
#define P_E13_TRISINP		{TRISASET = _TRISA_TRISA2_MASK;}
#define P_E13_TRISOUT		{TRISACLR = _TRISA_TRISA2_MASK;}
#define P_E13_WRITESET(m)	{if(m) LATASET = _LATA_LATA2_MASK else LATACLR = _LATA_LATA2_MASK;}
#define P_E13_WRITE1		{LATASET = _LATA_LATA2_MASK;}
#define P_E13_WRITE0		{LATACLR = _LATA_LATA2_MASK;}
#define P_E13_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA2_MASK else ODCACLR = _ODCA_ODCA2_MASK;}
#define P_E13_ODCON			{ODCASET = _ODCA_ODCA2_MASK;}
#define P_E13_ODCOFF		{ODCACLR = _ODCA_ODCA2_MASK;}
#define P_E13_READ			((PORTA & _PORTA_RA2_MASK) ? 1 : 0)

#define P_E14_TRIS			TRISAbits.TRISA3		// MMBasic ext i/o pin 14
#define	P_E14_AN_FUN		FALSE
#define P_E14_OC			ODCAbits.ODCA3
#define P_E14_OUT			LATAbits.LATA3
#define P_E14_IN			PORTAbits.RA3
#define P_E14_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA3_MASK else TRISACLR = _TRISA_TRISA3_MASK;}
#define P_E14_TRISINP		{TRISASET = _TRISA_TRISA3_MASK;}
#define P_E14_TRISOUT		{TRISACLR = _TRISA_TRISA3_MASK;}
#define P_E14_WRITESET(m)	{if(m) LATASET = _LATA_LATA3_MASK else LATACLR = _LATA_LATA3_MASK;}
#define P_E14_WRITE1		{LATASET = _LATA_LATA3_MASK;}
#define P_E14_WRITE0		{LATACLR = _LATA_LATA3_MASK;}
#define P_E14_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA3_MASK else ODCACLR = _ODCA_ODCA3_MASK;}
#define P_E14_ODCON			{ODCASET = _ODCA_ODCA3_MASK;}
#define P_E14_ODCOFF		{ODCACLR = _ODCA_ODCA3_MASK;}
#define P_E14_READ			((PORTA & _PORTA_RA3_MASK) ? 1 : 0)

#define P_E15_TRIS			TRISAbits.TRISA4		// MMBasic ext i/o pin 15
#define	P_E15_AN_FUN		FALSE
#define P_E15_OC			ODCAbits.ODCA4
#define P_E15_OUT			LATAbits.LATA4
#define P_E15_IN			PORTAbits.RA4
#define P_E15_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA4_MASK else TRISACLR = _TRISA_TRISA4_MASK;}
#define P_E15_TRISINP		{TRISASET = _TRISA_TRISA4_MASK;}
#define P_E15_TRISOUT		{TRISACLR = _TRISA_TRISA4_MASK;}
#define P_E15_WRITESET(m)	{if(m) LATASET = _LATA_LATA4_MASK else LATACLR = _LATA_LATA4_MASK;}
#define P_E15_WRITE1		{LATASET = _LATA_LATA4_MASK;}
#define P_E15_WRITE0		{LATACLR = _LATA_LATA4_MASK;}
#define P_E15_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA4_MASK else ODCACLR = _ODCA_ODCA4_MASK;}
#define P_E15_ODCON			{ODCASET = _ODCA_ODCA4_MASK;}
#define P_E15_ODCOFF		{ODCACLR = _ODCA_ODCA4_MASK;}
#define P_E15_READ			((PORTA & _PORTA_RA4_MASK) ? 1 : 0)

#define P_E16_TRIS			TRISAbits.TRISA5		// MMBasic ext i/o pin 16
#define	P_E16_AN_FUN		FALSE
#define P_E16_OC			ODCAbits.ODCA5
#define P_E16_OUT			LATAbits.LATA5
#define P_E16_IN			PORTAbits.RA5
#define P_E16_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA5_MASK else TRISACLR = _TRISA_TRISA5_MASK;}
#define P_E16_TRISINP		{TRISASET = _TRISA_TRISA5_MASK;}
#define P_E16_TRISOUT		{TRISACLR = _TRISA_TRISA5_MASK;}
#define P_E16_WRITESET(m)	{if(m) LATASET = _LATA_LATA5_MASK else LATACLR = _LATA_LATA5_MASK;}
#define P_E16_WRITE1		{LATASET = _LATA_LATA5_MASK;}
#define P_E16_WRITE0		{LATACLR = _LATA_LATA5_MASK;}
#define P_E16_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA5_MASK else ODCACLR = _ODCA_ODCA5_MASK;}
#define P_E16_ODCON			{ODCASET = _ODCA_ODCA5_MASK;}
#define P_E16_ODCOFF		{ODCACLR = _ODCA_ODCA5_MASK;}
#define P_E16_READ			((PORTA & _PORTA_RA5_MASK) ? 1 : 0)

#define P_E17_TRIS			TRISAbits.TRISA6		// MMBasic ext i/o pin 17
#define	P_E17_AN_FUN		FALSE
#define P_E17_OC			ODCAbits.ODCA6
#define P_E17_OUT			LATAbits.LATA6
#define P_E17_IN			PORTAbits.RA6
#define P_E17_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA6_MASK else TRISACLR = _TRISA_TRISA6_MASK;}
#define P_E17_TRISINP		{TRISASET = _TRISA_TRISA6_MASK;}
#define P_E17_TRISOUT		{TRISACLR = _TRISA_TRISA6_MASK;}
#define P_E17_WRITESET(m)	{if(m) LATASET = _LATA_LATA6_MASK else LATACLR = _LATA_LATA6_MASK;}
#define P_E17_WRITE1		{LATASET = _LATA_LATA6_MASK;}
#define P_E17_WRITE0		{LATACLR = _LATA_LATA6_MASK;}
#define P_E17_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA6_MASK else ODCACLR = _ODCA_ODCA6_MASK;}
#define P_E17_ODCON			{ODCASET = _ODCA_ODCA6_MASK;}
#define P_E17_ODCOFF		{ODCACLR = _ODCA_ODCA6_MASK;}
#define P_E17_READ			((PORTA & _PORTA_RA6_MASK) ? 1 : 0)

#define P_E18_TRIS			TRISAbits.TRISA7		// MMBasic ext i/o pin 18
#define	P_E18_AN_FUN		FALSE
#define P_E18_OC			ODCAbits.ODCA7
#define P_E18_OUT			LATAbits.LATA7
#define P_E18_IN			PORTAbits.RA7
#define P_E18_TRISSET(m)	{if(m) TRISASET = _TRISA_TRISA7_MASK else TRISACLR = _TRISA_TRISA7_MASK;}
#define P_E18_TRISINP		{TRISASET = _TRISA_TRISA7_MASK;}
#define P_E18_TRISOUT		{TRISACLR = _TRISA_TRISA7_MASK;}
#define P_E18_WRITESET(m)	{if(m) LATASET = _LATA_LATA7_MASK else LATACLR = _LATA_LATA7_MASK;}
#define P_E18_WRITE1		{LATASET = _LATA_LATA7_MASK;}
#define P_E18_WRITE0		{LATACLR = _LATA_LATA7_MASK;}
#define P_E18_ODCSET(m)		{if(m) ODCASET = _ODCA_ODCA7_MASK else ODCACLR = _ODCA_ODCA7_MASK;}
#define P_E18_ODCON			{ODCASET = _ODCA_ODCA7_MASK;}
#define P_E18_ODCOFF		{ODCACLR = _ODCA_ODCA7_MASK;}
#define P_E18_READ			((PORTA & _PORTA_RA7_MASK) ? 1 : 0)

#define P_E19_TRIS			TRISGbits.TRISG1		// MMBasic ext i/o pin 19
#define	P_E19_AN_FUN		FALSE
#define P_E19_OC			ODCGbits.ODCG1
#define P_E19_OUT			LATGbits.LATG1
#define P_E19_IN			PORTGbits.RG1
#define P_E19_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG1_MASK else TRISGCLR = _TRISG_TRISG1_MASK;}
#define P_E19_TRISINP		{TRISGSET = _TRISG_TRISG1_MASK;}
#define P_E19_TRISOUT		{TRISGCLR = _TRISG_TRISG1_MASK;}
#define P_E19_WRITESET(m)	{if(m) LATGSET = _LATG_LATG1_MASK else LATGCLR = _LATG_LATG1_MASK;}
#define P_E19_WRITE1		{LATGSET = _LATG_LATG1_MASK;}
#define P_E19_WRITE0		{LATGCLR = _LATG_LATG1_MASK;}
#define P_E19_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG1_MASK else ODCGCLR = _ODCG_ODCG1_MASK;}
#define P_E19_ODCON			{ODCGSET = _ODCG_ODCG1_MASK;}
#define P_E19_ODCOFF		{ODCGCLR = _ODCG_ODCG1_MASK;}
#define P_E19_READ			((PORTG & _PORTG_RG1_MASK) ? 1 : 0)

#define P_E20_TRIS			TRISGbits.TRISG0		// MMBasic ext i/o pin 20
#define	P_E20_AN_FUN		FALSE
#define P_E20_OC			ODCGbits.ODCG0
#define P_E20_OUT			LATGbits.LATG0
#define P_E20_IN			PORTGbits.RG0
#define P_E20_TRISSET(m)	{if(m) TRISGSET = _TRISG_TRISG0_MASK else TRISGCLR = _TRISG_TRISG0_MASK;}
#define P_E20_TRISINP		{TRISGSET = _TRISG_TRISG0_MASK;}
#define P_E20_TRISOUT		{TRISGCLR = _TRISG_TRISG0_MASK;}
#define P_E20_WRITESET(m)	{if(m) LATGSET = _LATG_LATG0_MASK else LATGCLR = _LATG_LATG0_MASK;}
#define P_E20_WRITE1		{LATGSET = _LATG_LATG0_MASK;}
#define P_E20_WRITE0		{LATGCLR = _LATG_LATG0_MASK;}
#define P_E20_ODCSET(m)		{if(m) ODCGSET = _ODCG_ODCG0_MASK else ODCGCLR = _ODCG_ODCG0_MASK;}
#define P_E20_ODCON			{ODCGSET = _ODCG_ODCG0_MASK;}
#define P_E20_ODCOFF		{ODCGCLR = _ODCG_ODCG0_MASK;}
#define P_E20_READ			((PORTG & _PORTG_RG0_MASK) ? 1 : 0)

#define P_E21_TRIS			TRISEbits.TRISE0		// Parallel Master ext i/o pin 1
#define	P_E21_AN_FUN		FALSE
#define P_E21_OC			ODCEbits.ODCE0
#define P_E21_OUT			LATEbits.LATE0
#define P_E21_IN			PORTEbits.RE0
#define P_E21_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE0_MASK else TRISECLR = _TRISE_TRISE0_MASK;}
#define P_E21_TRISINP		{TRISESET = _TRISE_TRISE0_MASK;}
#define P_E21_TRISOUT		{TRISECLR = _TRISE_TRISE0_MASK;}
#define P_E21_WRITESET(m)	{if(m) LATESET = _LATE_LATE0_MASK else LATECLR = _LATE_LATE0_MASK;}
#define P_E21_WRITE1		{LATESET = _LATE_LATE0_MASK;}
#define P_E21_WRITE0		{LATECLR = _LATE_LATE0_MASK;}
#define P_E21_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE0_MASK else ODCECLR = _ODCE_ODCE0_MASK;}
#define P_E21_ODCON			{ODCESET = _ODCE_ODCE0_MASK;}
#define P_E21_ODCOFF		{ODCECLR = _ODCE_ODCE0_MASK;}
#define P_E21_READ			((PORTE & _PORTE_RE0_MASK) ? 1 : 0)

#define P_E22_TRIS			TRISEbits.TRISE1		// Parallel Master ext i/o pin 2
#define	P_E22_AN_FUN		FALSE
#define P_E22_OC			ODCEbits.ODCE1
#define P_E22_OUT			LATEbits.LATE1
#define P_E22_IN			PORTEbits.RE1
#define P_E22_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE1_MASK else TRISECLR = _TRISE_TRISE1_MASK;}
#define P_E22_TRISINP		{TRISESET = _TRISE_TRISE1_MASK;}
#define P_E22_TRISOUT		{TRISECLR = _TRISE_TRISE1_MASK;}
#define P_E22_WRITESET(m)	{if(m) LATESET = _LATE_LATE1_MASK else LATECLR = _LATE_LATE1_MASK;}
#define P_E22_WRITE1		{LATESET = _LATE_LATE1_MASK;}
#define P_E22_WRITE0		{LATECLR = _LATE_LATE1_MASK;}
#define P_E22_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE0_MASK else ODCECLR = _ODCE_ODCE1_MASK;}
#define P_E22_ODCON			{ODCESET = _ODCE_ODCE1_MASK;}
#define P_E22_ODCOFF		{ODCECLR = _ODCE_ODCE1_MASK;}
#define P_E22_READ			((PORTE & _PORTE_RE1_MASK) ? 1 : 0)

#define P_E23_TRIS			TRISEbits.TRISE2		// Parallel Master ext i/o pin 3
#define	P_E23_AN_FUN		FALSE
#define P_E23_OC			ODCEbits.ODCE2
#define P_E23_OUT			LATEbits.LATE2
#define P_E23_IN			PORTEbits.RE2
#define P_E23_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE2_MASK else TRISECLR = _TRISE_TRISE2_MASK;}
#define P_E23_TRISINP		{TRISESET = _TRISE_TRISE2_MASK;}
#define P_E23_TRISOUT		{TRISECLR = _TRISE_TRISE2_MASK;}
#define P_E23_WRITESET(m)	{if(m) LATESET = _LATE_LATE2_MASK else LATECLR = _LATE_LATE2_MASK;}
#define P_E23_WRITE1		{LATESET = _LATE_LATE2_MASK;}
#define P_E23_WRITE0		{LATECLR = _LATE_LATE2_MASK;}
#define P_E23_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE2_MASK else ODCECLR = _ODCE_ODCE2_MASK;}
#define P_E23_ODCON			{ODCESET = _ODCE_ODCE2_MASK;}
#define P_E23_ODCOFF		{ODCECLR = _ODCE_ODCE2_MASK;}
#define P_E23_READ			((PORTE & _PORTE_RE2_MASK) ? 1 : 0)

#define P_E24_TRIS			TRISEbits.TRISE3		// Parallel Master ext i/o pin 4
#define	P_E24_AN_FUN		FALSE
#define P_E24_OC			ODCEbits.ODCE3
#define P_E24_OUT			LATEbits.LATE3
#define P_E24_IN			PORTEbits.RE3
#define P_E24_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE3_MASK else TRISECLR = _TRISE_TRISE3_MASK;}
#define P_E24_TRISINP		{TRISESET = _TRISE_TRISE3_MASK;}
#define P_E24_TRISOUT		{TRISECLR = _TRISE_TRISE3_MASK;}
#define P_E24_WRITESET(m)	{if(m) LATESET = _LATE_LATE3_MASK else LATECLR = _LATE_LATE3_MASK;}
#define P_E24_WRITE1		{LATESET = _LATE_LATE3_MASK;}
#define P_E24_WRITE0		{LATECLR = _LATE_LATE3_MASK;}
#define P_E24_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE3_MASK else ODCECLR = _ODCE_ODCE3_MASK;}
#define P_E24_ODCON			{ODCESET = _ODCE_ODCE3_MASK;}
#define P_E24_ODCOFF		{ODCECLR = _ODCE_ODCE3_MASK;}
#define P_E24_READ			((PORTE & _PORTE_RE3_MASK) ? 1 : 0)

#define P_E25_TRIS			TRISEbits.TRISE4		// Parallel Master ext i/o pin 5
#define	P_E25_AN_FUN		FALSE
#define P_E25_OC			ODCEbits.ODCE4
#define P_E25_OUT			LATEbits.LATE4
#define P_E25_IN			PORTEbits.RE4
#define P_E25_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE4_MASK else TRISECLR = _TRISE_TRISE4_MASK;}
#define P_E25_TRISINP		{TRISESET = _TRISE_TRISE4_MASK;}
#define P_E25_TRISOUT		{TRISECLR = _TRISE_TRISE4_MASK;}
#define P_E25_WRITESET(m)	{if(m) LATESET = _LATE_LATE4_MASK else LATECLR = _LATE_LATE4_MASK;}
#define P_E25_WRITE1		{LATESET = _LATE_LATE4_MASK;}
#define P_E25_WRITE0	{LATECLR = _LATE_LATE4_MASK;}
#define P_E25_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE4_MASK else ODCECLR = _ODCE_ODCE4_MASK;}
#define P_E25_ODCON			{ODCESET = _ODCE_ODCE4_MASK;}
#define P_E25_ODCOFF		{ODCECLR = _ODCE_ODCE4_MASK;}
#define P_E25_READ			((PORTE & _PORTE_RE4_MASK) ? 1 : 0)

#define P_E26_TRIS			TRISEbits.TRISE5		// Parallel Master ext i/o pin 6
#define	P_E26_AN_FUN		FALSE
#define P_E26_OC			ODCEbits.ODCE5
#define P_E26_OUT			LATEbits.LATE5
#define P_E26_IN			PORTEbits.RE5
#define P_E26_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE5_MASK else TRISECLR = _TRISE_TRISE5_MASK;}
#define P_E26_TRISINP		{TRISESET = _TRISE_TRISE5_MASK;}
#define P_E26_TRISOUT		{TRISECLR = _TRISE_TRISE5_MASK;}
#define P_E26_WRITESET(m)	{if(m) LATESET = _LATE_LATE5_MASK else LATECLR = _LATE_LATE5_MASK;}
#define P_E26_WRITE1		{LATESET = _LATE_LATE5_MASK;}
#define P_E26_WRITE0		{LATECLR = _LATE_LATE5_MASK;}
#define P_E26_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE5_MASK else ODCECLR = _ODCE_ODCE5_MASK;}
#define P_E26_ODCON			{ODCESET = _ODCE_ODCE5_MASK;}
#define P_E26_ODCOFF		{ODCECLR = _ODCE_ODCE5_MASK;}
#define P_E26_READ			((PORTE & _PORTE_RE5_MASK) ? 1 : 0)

#define P_E27_TRIS			TRISEbits.TRISE6		// Parallel Master ext i/o pin 7
#define	P_E27_AN_FUN		FALSE
#define P_E27_OC			ODCEbits.ODCE6
#define P_E27_OUT			LATEbits.LATE6
#define P_E27_IN			PORTEbits.RE6
#define P_E27_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE6_MASK else TRISECLR = _TRISE_TRISE6_MASK;}
#define P_E27_TRISINP		{TRISESET = _TRISE_TRISE6_MASK;}
#define P_E27_TRISOUT		{TRISECLR = _TRISE_TRISE6_MASK;}
#define P_E27_WRITESET(m)	{if(m) LATESET = _LATE_LATE6_MASK else LATECLR = _LATE_LATE6_MASK;}
#define P_E27_WRITE1		{LATESET = _LATE_LATE6_MASK;}
#define P_E27_WRITE0		{LATECLR = _LATE_LATE6_MASK;}
#define P_E27_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE6_MASK else ODCECLR = _ODCE_ODCE6_MASK;}
#define P_E27_ODCON			{ODCESET = _ODCE_ODCE6_MASK;}
#define P_E27_ODCOFF		{ODCECLR = _ODCE_ODCE6_MASK;}
#define P_E27_READ			((PORTE & _PORTE_RE6_MASK) ? 1 : 0)

#define P_E28_TRIS			TRISEbits.TRISE7		// Parallel Master ext i/o pin 8
#define	P_E28_AN_FUN		FALSE
#define P_E28_OC			ODCEbits.ODCE7
#define P_E28_OUT			LATEbits.LATE7
#define P_E28_IN			PORTEbits.RE7
#define P_E28_TRISSET(m)	{if(m) TRISESET = _TRISE_TRISE7_MASK else TRISECLR = _TRISE_TRISE7_MASK;}
#define P_E28_TRISINP		{TRISESET = _TRISE_TRISE7_MASK;}
#define P_E28_TRISOUT		{TRISECLR = _TRISE_TRISE7_MASK;}
#define P_E28_WRITESET(m)	{if(m) LATESET = _LATE_LATE7_MASK else LATECLR = _LATE_LATE7_MASK;}
#define P_E28_WRITE1		{LATESET = _LATE_LATE7_MASK;}
#define P_E28_WRITE0		{LATECLR = _LATE_LATE7_MASK;}
#define P_E28_ODCSET(m)		{if(m) ODCESET = _ODCE_ODCE7_MASK else ODCECLR = _ODCE_ODCE7_MASK;}
#define P_E28_ODCON			{ODCESET = _ODCE_ODCE7_MASK;}
#define P_E28_ODCOFF		{ODCECLR = _ODCE_ODCE7_MASK;}
#define P_E28_READ			((PORTE & _PORTE_RE7_MASK) ? 1 : 0)

#define P_E29_TRIS			TRISDbits.TRISD4		// Parallel Master ext i/o pin 9
#define	P_E29_AN_FUN		FALSE
#define P_E29_OC			ODCDbits.ODCD4
#define P_E29_OUT			LATDbits.LATD4
#define P_E29_IN			PORTDbits.RD4
#define P_E29_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD4_MASK else TRISDCLR = _TRISD_TRISD4_MASK;}
#define P_E29_TRISINP		{TRISDSET = _TRISD_TRISD4_MASK;}
#define P_E29_TRISOUT		{TRISDCLR = _TRISD_TRISD4_MASK;}
#define P_E29_WRITESET(m)	{if(m) LATDSET = _LATD_LATD4_MASK else LATDCLR = _LATD_LATD4_MASK;}
#define P_E29_WRITE1		{LATDSET = _LATD_LATD4_MASK;}
#define P_E29_WRITE0		{LATDCLR = _LATD_LATD4_MASK;}
#define P_E29_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD4_MASK else ODCDCLR = _ODCD_ODCD4_MASK;}
#define P_E29_ODCON			{ODCDSET = _ODCD_ODCD4_MASK;}
#define P_E29_ODCOFF		{ODCDCLR = _ODCD_ODCD4_MASK;}
#define P_E29_READ			((PORTD & _PORTD_RD4_MASK) ? 1 : 0)

#define P_E30_TRIS			TRISDbits.TRISD5		// Parallel Master ext i/o pin 10
#define	P_E30_AN_FUN		FALSE
#define P_E30_OC			ODCDbits.ODCD5
#define P_E30_OUT			LATDbits.LATD5
#define P_E30_IN			PORTDbits.RD5
#define P_E30_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD5_MASK else TRISDCLR = _TRISD_TRISD5_MASK;}
#define P_E30_TRISINP		{TRISDSET = _TRISD_TRISD5_MASK;}
#define P_E30_TRISOUT		{TRISDCLR = _TRISD_TRISD5_MASK;}
#define P_E30_WRITESET(m)	{if(m) LATDSET = _LATD_LATD5_MASK else LATDCLR = _LATD_LATD5_MASK;}
#define P_E30_WRITE1		{LATDSET = _LATD_LATD5_MASK;}
#define P_E30_WRITE0		{LATDCLR = _LATD_LATD5_MASK;}
#define P_E30_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD5_MASK else ODCDCLR = _ODCD_ODCD5_MASK;}
#define P_E30_ODCON			{ODCDSET = _ODCD_ODCD5_MASK;}
#define P_E30_ODCOFF		{ODCDCLR = _ODCD_ODCD5_MASK;}
#define P_E30_READ			((PORTD & _PORTD_RD5_MASK) ? 1 : 0)

#define P_E31_TRIS			TRISBbits.TRISB0		// Parallel Master ext i/o pin 11
#define	P_E31_AN_FUN		TRUE
#define P_E31_ANALOG		AD1PCFGbits.PCFG0
#define P_E31_ACHAN			0
#define P_E31_OC			ODCBbits.ODCB0
#define P_E31_OUT			LATBbits.LATB0
#define P_E31_IN			PORTBbits.RB0
#define P_E31_TRISSET(m)	{if(m) TRISBSET = _TRISB_TRISB0_MASK else TRISBCLR = _TRISB_TRISB0_MASK;}
#define P_E31_TRISINP		{TRISBSET = _TRISB_TRISB0_MASK;}
#define P_E31_TRISOUT		{TRISBCLR = _TRISB_TRISB0_MASK;}
#define P_E31_WRITESET(m)	{if(m) LATBSET = _LATB_LATB0_MASK else LATBCLR = _LATB_LATB0_MASK;}
#define P_E31_WRITE1		{LATBSET = _LATB_LATB0_MASK;}
#define P_E31_WRITE0		{LATBCLR = _LATB_LATB0_MASK;}
#define P_E31_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB0_MASK else ODCBCLR = _ODCB_ODCB0_MASK;}
#define P_E31_ODCON			{ODCBSET = _ODCB_ODCB0_MASK;}
#define P_E31_ODCOFF		{ODCBCLR = _ODCB_ODCB0_MASK;}
#define P_E31_READ			((PORTB & _PORTB_RB0_MASK) ? 1 : 0)

#define P_E32_TRIS			TRISBbits.TRISB1		// Parallel Master ext i/o pin 12
#define	P_E32_AN_FUN		TRUE
#define P_E32_ANALOG		AD1PCFGbits.PCFG1
#define P_E32_ACHAN			1
#define P_E32_OC			ODCBbits.ODCB1
#define P_E32_OUT			LATBbits.LATB1
#define P_E32_IN			PORTBbits.RB1
#define P_E32_TRISSET(m)	{if(m) TRISBSET = _TRISB_TRISB1_MASK else TRISBCLR = _TRISB_TRISB1_MASK;}
#define P_E32_TRISINP		{TRISBSET = _TRISB_TRISB1_MASK;}
#define P_E32_TRISOUT		{TRISBCLR = _TRISB_TRISB1_MASK;}
#define P_E32_WRITESET(m)	{if(m) LATBSET = _LATB_LATB1_MASK else LATBCLR = _LATB_LATB1_MASK;}
#define P_E32_WRITE1		{LATBSET = _LATB_LATB1_MASK;}
#define P_E32_WRITE0		{LATBCLR = _LATB_LATB1_MASK;}
#define P_E32_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB1_MASK else ODCBCLR = _ODCB_ODCB1_MASK;}
#define P_E32_ODCON			{ODCBSET = _ODCB_ODCB1_MASK;}
#define P_E32_ODCOFF		{ODCBCLR = _ODCB_ODCB1_MASK;}
#define P_E32_READ			((PORTB & _PORTB_RB1_MASK) ? 1 : 0)

#define P_E33_TRIS			TRISDbits.TRISD1		// Parallel Master ext i/o pin 13
#define	P_E33_AN_FUN		FALSE
#define P_E33_OC			ODCDbits.ODCD1
#define P_E33_OUT			LATDbits.LATD1
#define P_E33_IN			PORTDbits.RD1
#define P_E33_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD1_MASK else TRISDCLR = _TRISD_TRISD1_MASK;}
#define P_E33_TRISINP		{TRISDSET = _TRISD_TRISD1_MASK;}
#define P_E33_TRISOUT		{TRISDCLR = _TRISD_TRISD1_MASK;}
#define P_E33_WRITESET(m)	{if(m) LATDSET = _LATD_LATD1_MASK else LATDCLR = _LATD_LATD1_MASK;}
#define P_E33_WRITE1		{LATDSET = _LATD_LATD1_MASK;}
#define P_E33_WRITE0		{LATDCLR = _LATD_LATD1_MASK;}
#define P_E33_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD1_MASK else ODCDCLR = _ODCD_ODCD1_MASK;}
#define P_E33_ODCON			{ODCDSET = _ODCD_ODCD1_MASK;}
#define P_E33_ODCOFF		{ODCDCLR = _ODCD_ODCD1_MASK;}
#define P_E33_READ			((PORTD & _PORTD_RD1_MASK) ? 1 : 0)

#define P_E34_TRIS			TRISDbits.TRISD10		// Parallel Master ext i/o pin 14
#define	P_E34_AN_FUN		FALSE
#define P_E34_OC			ODCDbits.ODCD10
#define P_E34_OUT			LATDbits.LATD10
#define P_E34_IN			PORTDbits.RD10
#define P_E34_TRISSET(m)	{if(m) TRISDSET = _TRISD_TRISD10_MASK else TRISDCLR = _TRISD_TRISD10_MASK;}
#define P_E34_TRISINP		{TRISDSET = _TRISD_TRISD10_MASK;}
#define P_E34_TRISOUT		{TRISDCLR = _TRISD_TRISD10_MASK;}
#define P_E34_WRITESET(m)	{if(m) LATDSET = _LATD_LATD10_MASK else LATDCLR = _LATD_LATD10_MASK;}
#define P_E34_WRITE1		{LATDSET = _LATD_LATD10_MASK;}
#define P_E34_WRITE0		{LATDCLR = _LATD_LATD10_MASK;}
#define P_E34_ODCSET(m)		{if(m) ODCDSET = _ODCD_ODCD10_MASK else ODCDCLR = _ODCD_ODCD10_MASK;}
#define P_E34_ODCON			{ODCDSET = _ODCD_ODCD10_MASK;}
#define P_E34_ODCOFF		{ODCDCLR = _ODCD_ODCD10_MASK;}
#define P_E34_READ			((PORTD & _PORTD_RD10_MASK) ? 1 : 0)

// Battery Level
#define P_E35_TRIS		TRISBbits.TRISB14		// DuinoMite Battery Level	
#define	P_E35_AN_FUN		TRUE
#define P_E35_ANALOG		AD1PCFGbits.PCFG14
#define P_E35_ACHAN		14
#define P_E35_OC                ODCBbits.ODCB14
#define P_E35_OUT		LATBbits.LATB14
#define P_E35_IN		PORTBbits.RB14
#define P_E35_TRISSET(m)	{if(m) TRISBSET = _TRISB_TRISB14_MASK else TRISBCLR = _TRISB_TRISB14_MASK;}
#define P_E35_TRISINP		{TRISBSET = _TRISB_TRISB14_MASK;}
#define P_E35_TRISOUT		{TRISBCLR = _TRISB_TRISB14_MASK;}
#define P_E35_WRITESET(m)	{if(m) LATBSET = _LATB_LATB14_MASK else LATBCLR = _LATB_LATB14_MASK;}
#define P_E35_WRITE1		{LATBSET = _LATB_LATB14_MASK;}
#define P_E35_WRITE0		{LATBCLR = _LATB_LATB14_MASK;}
#define P_E35_ODCSET(m)		{if(m) ODCBSET = _ODCB_ODCB14_MASK else ODCBCLR = _ODCB_ODCB14_MASK;}
#define P_E35_ODCON		{ODCBSET = _ODCB_ODCB14_MASK;}
#define P_E35_ODCOFF		{ODCBCLR = _ODCB_ODCB14_MASK;}
#define P_E35_READ		((PORTB & _PORTB_RB14_MASK) ? 1 : 0)



#define P_E36_TRIS		TRISFbits.TRISF4		// UART2 (UART3A) RX
#define	P_E36_AN_FUN		FALSE
#define P_E36_OC                ODCFbits.ODCF4
#define P_E36_OUT		LATFbits.LATF4
#define P_E36_IN		PORTFbits.RF4
#define P_E36_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF4_MASK else TRISFCLR = _TRISF_TRISF4_MASK;}
#define P_E36_TRISINP		{TRISFSET = _TRISF_TRISF4_MASK;}
#define P_E36_TRISOUT		{TRISFCLR = _TRISF_TRISF4_MASK;}
#define P_E36_WRITESET(m)	{if(m) LATFSET = _LATF_LATF4_MASK else LATFCLR = _LATF_LATF4_MASK;}
#define P_E36_WRITE1		{LATFSET = _LATF_LATF4_MASK;}
#define P_E36_WRITE0		{LATFCLR = _LATF_LATF4_MASK;}
#define P_E36_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF4_MASK else ODCFCLR = _ODCF_ODCF4_MASK;}
#define P_E36_ODCON		{ODCFSET = _ODCF_ODCF4_MASK;}
#define P_E36_ODCOFF		{ODCFCLR = _ODCF_ODCF4_MASK;}
#define P_E36_READ		((PORTF & _PORTF_RF4_MASK) ? 1 : 0)

#define P_E37_TRIS		TRISFbits.TRISF5		// UART2 (UART3A) TX
#define	P_E37_AN_FUN		FALSE
#define P_E37_OC                ODCFbits.ODCF5
#define P_E37_OUT		LATFbits.LATF5
#define P_E37_IN		PORTFbits.RF5
#define P_E37_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF5_MASK else TRISFCLR = _TRISF_TRISF5_MASK;}
#define P_E37_TRISINP		{TRISFSET = _TRISF_TRISF5_MASK;}
#define P_E37_TRISOUT		{TRISFCLR = _TRISF_TRISF5_MASK;}
#define P_E37_WRITESET(m)	{if(m) LATFSET = _LATF_LATF5_MASK else LATFCLR = _LATF_LATF5_MASK;}
#define P_E37_WRITE1		{LATFSET = _LATF_LATF5_MASK;}
#define P_E37_WRITE0		{LATFCLR = _LATF_LATF5_MASK;}
#define P_E37_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF5_MASK else ODCFCLR = _ODCF_ODCF5_MASK;}
#define P_E37_ODCON		{ODCFSET = _ODCF_ODCF5_MASK;}
#define P_E37_ODCOFF		{ODCFCLR = _ODCF_ODCF5_MASK;}
#define P_E37_READ		((PORTF & _PORTF_RF5_MASK) ? 1 : 0)

#define P_E38_TRIS		TRISFbits.TRISF12		// UART5 (UART3B) RX
#define	P_E38_AN_FUN		FALSE
#define P_E38_OC                ODCFbits.ODCF12
#define P_E38_OUT		LATFbits.LATF12
#define P_E38_IN		PORTFbits.RF12
#define P_E38_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF12_MASK else TRISFCLR = _TRISF_TRISF12_MASK;}
#define P_E38_TRISINP		{TRISFSET = _TRISF_TRISF12_MASK;}
#define P_E38_TRISOUT		{TRISFCLR = _TRISF_TRISF12_MASK;}
#define P_E38_WRITESET(m)	{if(m) LATFSET = _LATF_LATF12_MASK else LATFCLR = _LATF_LATF12_MASK;}
#define P_E38_WRITE1		{LATFSET = _LATF_LATF12_MASK;}
#define P_E38_WRITE0		{LATFCLR = _LATF_LATF12_MASK;}
#define P_E38_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF12_MASK else ODCFCLR = _ODCF_ODCF12_MASK;}
#define P_E38_ODCON		{ODCFSET = _ODCF_ODCF12_MASK;}
#define P_E38_ODCOFF		{ODCFCLR = _ODCF_ODCF12_MASK;}
#define P_E38_READ		((PORTF & _PORTF_RF12_MASK) ? 1 : 0)

#define P_E39_TRIS		TRISFbits.TRISF13		// UART5 (UART3B) TX
#define	P_E39_AN_FUN		FALSE
#define P_E39_OC                ODCFbits.ODCF13
#define P_E39_OUT		LATFbits.LATF13
#define P_E39_IN		PORTFbits.RF13
#define P_E39_TRISSET(m)	{if(m) TRISFSET = _TRISF_TRISF13_MASK else TRISFCLR = _TRISF_TRISF13_MASK;}
#define P_E39_TRISINP		{TRISFSET = _TRISF_TRISF13_MASK;}
#define P_E39_TRISOUT		{TRISFCLR = _TRISF_TRISF13_MASK;}
#define P_E39_WRITESET(m)	{if(m) LATFSET = _LATF_LATF13_MASK else LATFCLR = _LATF_LATF13_MASK;}
#define P_E39_WRITE1		{LATFSET = _LATF_LATF13_MASK;}
#define P_E39_WRITE0		{LATFCLR = _LATF_LATF13_MASK;}
#define P_E39_ODCSET(m)		{if(m) ODCFSET = _ODCF_ODCF13_MASK else ODCFCLR = _ODCF_ODCF13_MASK;}
#define P_E39_ODCON		{ODCFSET = _ODCF_ODCF13_MASK;}
#define P_E39_ODCOFF		{ODCFCLR = _ODCF_ODCF13_MASK;}
#define P_E39_READ		((PORTF & _PORTF_RF13_MASK) ? 1 : 0)



// Keyboard definitions

// D6 (56) -> G6 (10)	(Keyboard Clock)
// Clock Pullup 15 -> 8 
// Clock Interrupt 15 -> 8
// D7 (57) -> G7 (11)	(Keyboard Data)
// Data PullUp 16 -> 9

#define P_PS2CLK			PORTGbits.RG6			// Pin 10 input  - Keyboard clock
#define P_PS2CLK_PULLUP		CNPUEbits.CNPUE8
#define P_PS2CLK_INT		CNENbits.CNEN8
	
#define P_PS2DAT			PORTGbits.RG7			// Pin 11 input  - Keyboard data
#define P_PS2DAT_PULLUP		CNPUEbits.CNPUE9


// video defines

// SPI2 -> SPI1
// RB11 -> RD1		(VGA/Composer jumper)
// RG8 -> RD0		(video)
// RD4 -> RD2		(HSync)
// RB12 -> RD7		(VSync)

#define P_VGA_SELECT		0					// state when VGA selected
		
#define P_VIDEO_SPI			1					// the SPI peripheral used for video.  note: pin G9 is automatically set as the framing input
#define P_SPI_INPUT			SPI1BUF				// input buffer for the SPI peripheral
#define P_SPI_INTERRUPT		_SPI1_TX_IRQ		// interrupt used by the video DMA
	
#define P_VGA_COMP			PORTDbits.RD1		// VGA/Composite jumper

#define P_VIDEO				PORTDbits.RD0		// video
#define P_VIDEO_TRIS		TRISDbits.TRISD0

#define P_HORIZ				PORTDbits.RD2		// horizontal sync
#define P_HORIZ_TRIS		TRISDbits.TRISD2

#define P_VERT_SET_HI		PORTDbits.RD7=1		// set vert sync hi
#define P_VERT_SET_LO		PORTDbits.RD7=0		// set vert sync lo
#define P_VERT_TRIS			TRISDbits.TRISD7


// SD card defines
// This file is included in SDCard/HardwareProfile.h and replaces the defines in that file
#define P_SD_LED_SET_HI			LATBSET = (1 << 15)		// SD card activity led
#define P_SD_LED_SET_LO			LATBCLR = (1 << 15)		// SD card activity led
#define P_SD_ACTIVITY_TRIS		TRISBbits.TRISB15

#define SD_CS_SET_HI			LATGSET = (1 << 12)		// SD-SPI Chip Set Output bit high
#define SD_CS_SET_LO			LATGCLR = (1 << 12)		// SD-SPI Set Output bit low
#define SD_CS_TRIS			TRISGbits.TRISG12			// SD-SPI Chip Select TRIS bit
#define SD_CS                           PORTGbits.RG12

#define SD_CS_SET_HI			LATGSET = (1 << 12) //PORTGbits.RG12=1;                       // SD-SPI Chip Set Output bit high
#define SD_CS_SET_LO			LATGCLR = (1 << 12) //PORTGbits.RG12=0;                       // SD-SPI Set Output bit low
#define SD_CS_TRIS			TRISGbits.TRISG12

#define SD_CD				0 //PORTDbits.RD4			// SD-SPI Card Detect Input bit
//#define SD_CD_TRIS			TRISDbits.TRISD4		// SD-SPI Card Detect TRIS bit

#define SD_WE				0 //PORTDbits.RD5			// SD-SPI Write Protect Check Input bit
//#define SD_WE_TRIS			TRISDbits.TRISD5		// SD-SPI Write Protect Check TRIS bit

#define SPICON1				SPI3CON								// The main SPI control register
#define SPISTAT				SPI3STAT							// The SPI status register
#define SPIBUF				SPI3BUF								// The SPI Buffer
#define SPISTAT_RBF			SPI3STATbits.SPIRBF		// The receive buffer full bit in the SPI status register
#define SPICON1bits			SPI3CONbits						// The bitwise define for the SPI control register (i.e. _____bits)
#define SPISTATbits			SPI3STATbits					// The bitwise define for the SPI status register (i.e. _____bits)
#define SPIENABLE			SPI3CONbits.ON				// The enable bit for the SPI module
#define SPIBRG				SPI3BRG								// The definition for the SPI baud rate generator register (PIC32)

#define SPICLOCK			TRISDbits.TRISD15			// The TRIS bit for the SCK pin
#define SPIIN				TRISFbits.TRISF2			// The TRIS bit for the SDI pin
#define SPIOUT				TRISFbits.TRISF8			// The TRIS bit for the SDO pin

#define putcSPI(spichar)		SpiChnPutC(3, spichar)	//SPI library functions
#define getcSPI()			SpiChnGetC(3)
#define OpenSPI(config1, config2)	SpiChnOpen(3, config1, config2)
#define CloseSPI()                      SpiChnClose(3)


// Serial ports defines
// COM3 Rx = UEXT3A (RF4) ; Tx = UEXT3A (RF5)
// COM4 Rx = UEXT3B (RF12); Tx = UEXT3B (RF13)

#define P_COM1_RX_PIN_NBR		13
#define P_COM1_RX_PORT			PORTEbits.RE2

#define P_COM1_TX_PIN_NBR		14
#define P_COM1_TX_SET_HI		LATESET = (1 << 3)
#define P_COM1_TX_SET_LO		LATECLR = (1 << 3)

#define P_COM1_RTS_PIN_NBR		15
#define P_COM1_RTS_SET_HI		LATESET = (1 << 4)
#define P_COM1_RTS_SET_LO		LATECLR = (1 << 4)

#define P_COM1_CTS_PIN_NBR		16
#define P_COM1_CTS_PORT			PORTEbits.RE5

#define P_COM2_RX_PIN_NBR		17
#define P_COM2_RX_PORT			PORTEbits.RE6

#define P_COM2_TX_PIN_NBR		18
#define P_COM2_TX_SET_HI		LATESET = (1 << 7)
#define P_COM2_TX_SET_LO		LATECLR = (1 << 7)

/*
#define P_COM3_RX_PIN_NBR		22
#define P_COM3_TX_PIN_NBR		23

#define P_COM4_RX_PIN_NBR		11
#define P_COM4_TX_PIN_NBR		12
 */
#define P_COM3_RX_PIN_NBR		36
#define P_COM3_TX_PIN_NBR		37

#define P_COM4_RX_PIN_NBR		38
#define P_COM4_TX_PIN_NBR		39

// sound output
#define P_SOUND_TRIS			TRISDbits.TRISD3

#endif