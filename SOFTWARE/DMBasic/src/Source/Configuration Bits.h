/***********************************************************************************************************************
MMBasic

Configuration Bits.h

Include file that defines the configuration bits (fuses) for the PIC32 chip in MMBasic.
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/
// PIC32MX795F512H or PIC32MX695F512H configuration switches


#ifndef CONFIGURATION_BITS_H
#define CONFIGURATION_BITS_H

#pragma config FNOSC = PRIPLL			// Oscillator Selection
										// PRI		is Primary oscillator (XT, HS, EC)  
										// PRIPLL	is Primary oscillator (XT, HS, EC) w/ PLL  
										// SOSC		is Secondary oscillator  
										// LPRC		is Low power RC oscillator  
										// FRC		is Fast RC oscillator  
										// FRCPLL	is Fast RC oscillator w/ PLL  
										// FRCDIV16	is Fast RC oscillator with divide by 16  
										// FRCDIV	is Fast RC oscillator with divide  

#pragma config POSCMOD = HS				// Primary Oscillator Selection
										// HS		is HS oscillator  
										// EC		is EC oscillator  
										// XT		is XT oscillator  
										// OFF		is Disabled  

// IMPORTANT: If any of these are changed you must update CLOCKFREQ in Maximite.h
#pragma config FPLLIDIV = DIV_2			// PLL Input Divide by 1, 2, 3, 4, 5, 6 or 10 
#pragma config FPLLMUL = MUL_20			// PLL Multiply by 15, 16, 17, 18, 19, 20, 21 or 24
#pragma config FPLLODIV = DIV_1			// PLL Output Divide by 1, 2, 4, 8, 16, 32, 64, or 256

// IMPORTANT: THIS HAS NO EFFECT.  The bus frequency is set in the first few lines of main.c
#pragma config FPBDIV = DIV_2			// Peripheral Bus Clock Divide by 1, 2, 4 or 8		

#pragma config FSOSCEN = OFF			// Secondary oscillator OFF or ON
#pragma config IESO = ON//OFF				// Internal External Switchover (Two-Speed Start-up) OFF or ON
#pragma config OSCIOFNC = OFF			// CLKO output signal active on the OSCO pin. Select ON or OFF

#pragma config FCKSM = CSDCMD			// Clock Switching and Monitor Selection
										// CSECME	is Clock Switching Enabled, Clock Monitoring Enabled  
										// CSECMD	is Clock Switching Enabled, Clock Monitoring Disabled  
										// CSDCMD	is Clock Switching Disabled, Clock Monitoring Disabled  
#ifdef __DEBUG
#pragma config DEBUG = ON				// Background Debugger ON or OFF
#else
#pragma config DEBUG = OFF				// Background Debugger ON or OFF
#endif

#pragma config UPLLEN = ON				// USB PLL ON or OFF
#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divide by 1, 2, 3, 4, 5, 6, 10 or 12  

#pragma config FVBUSONIO = ON			// USB VBUS_ON pin control
										// OFF		is by the Port Function  
										// ON		is by the USB Module  

#pragma config FUSBIDIO = OFF			// USB USBID pin control
										// OFF		is by the Port Function  
										// ON		is by the USB Module  

#if defined(__32MX795F512H__)
#pragma config FCANIO = ON				// CAN IO Pins. OFF = Alternate, ON = Default
#endif

#pragma config FETHIO = ON				// Ethernet IO Pins. OFF = Alternate, ON = Default
#pragma config FMIIEN = OFF				// Ethernet MII Enable. OFF = RMII enabled, ON = MII enabled

#pragma config FSRSSEL = PRIORITY_7		// SRS Interrupt Priority in the range of 0 to 7  

#pragma config FWDTEN = OFF				// Watchdog Timer ON or OFF
#pragma config WDTPS = PS1				// Watchdog Timer Postscale from 1:1 to 1:1,048,576

#pragma config CP = OFF					// Code Protect Enable ON or OFF (prevents ANY read/write)
#pragma config BWP = OFF				// Boot Flash Write Protect ON or OFF 

#pragma config PWP = OFF				// Program Flash Write Protect ON, OFF or PWP4K to PWP512K in steps of 6  

#pragma config ICESEL = ICS_PGx1		// ICE/ICD Communications Channel Select
										// ICS_PGx1	is ICE pins are shared with PGC1, PGD1  
										// ICS_PGx2	is ICE pins are shared with PGC2, PGD2 
										
#endif  // CONFIGURATION_BITS_H
