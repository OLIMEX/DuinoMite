/***********************************************************************************************************************
MMBasic

custom.c

Handles all the miscelaneous commands and functions in MMBasic.  These are commands and functions that do not
comfortably anywhere else.

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/

#include <stdio.h>
#include <plib.h>

#define INCLUDE_FUNCTION_DEFINES
#include "Maximite.h"
#include "MMBasic.h"
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"
//	GS I2C Start
#include "I2C.h"
//	GS I2C End
#include "../IOPorts.h"     //ken
#include "../SDCard/HardwareProfile.h"
#include "../Video/Video.h"
#include "../SDCard/SDCard.h"
#include "../Timers/Timers.h"
#include "../Serial/Serial.h"
#include "Setup.h"
#include "../DuinoMite/RTC.h"
struct s_inttbl inttbl[NBRPINS + 2];
char *InterruptReturn;






/********************************************************************************************************************************************
Miscelaneous commands and functions
===================================

Each function is responsible for decoding a command
all function names are in the form cmd_xxxx() (for a basic command) or fun_xxxx() (for a basic function) so, if you want to search for the
function responsible for the NAME command look for cmd_name

There are 4 items of information that are setup before the command is run.
All these are globals.

int cmdtoken	This is the token number of the command (some commands can handle multiple
			statement types and this helps them differentiate)

char *cmdline	This is the command line terminated with a zero char and trimmed of leading
			spaces.  It may exist anywhere in memory (or even ROM).

char *nextstmt	This is a pointer to the next statement to be executed.  The only thing a
			command can do with it is save it or change it to some other location.

int CurrentLineNbr  This is read only and is set to zero if the command is in immediate mode.

The only actions a command can do to change the program flow is to change nextstmt or
execute longjmp(mark, 1) if it wants to abort the program.

********************************************************************************************************************************************/



// this is invoked as a command (ie, TIMER = 0)
// search through the line looking for the equals sign and step over it,
// evaluate the rest of the command and save in the timer
void cmd_timer(void) {
	while(*cmdline && tokenfunction(*cmdline) != op_equal) cmdline++;
	if(!*cmdline) error("Invalid syntax");
	mSecTimer = getnumber(++cmdline);
}



// this is invoked as a function
void fun_timer(void) {
	fret = mSecTimer;
}



void cmd_pause(void) {
	static int interrupted = false;
	int i;

	if(InterruptReturn == NULL) {
		// we are running pause in a normal program
		// first check if we have reentered (from an interrupt) and only zero the timer if we have NOT been interrupted.
		// This means an interrupted pause will resume from where it was when interrupted
		if(!interrupted) PauseTimer = 0;
		interrupted = false;

		i = getnumber(cmdline);
		while(PauseTimer < i) {
			CheckAbort();
			if(check_interrupt()) {
				// if there is an interrupt fake the return point to the start of this stmt
				// and return immediately to the program processor so that it can send us off
				// to the interrupt routine.  When the interrupt routine finishes we should reexecute
				// this stmt and because the variable interrupted is static we can see that we need to
				// resume pausing rather than start a new pause time.
				while(*cmdline && *cmdline != cmdtoken + C_BASETOKEN) cmdline--;	// step back to find the command token
				InterruptReturn = cmdline;			// point to it
				interrupted = true;				// show that this stmt was interrupted
				return;						// and let the interrupt run
			}
		}
	}
	else {
		// we are running pause in an interrupt, this is much simpler but note that
		// we use a different timer from the main pause code (above)
		IntPauseTimer = 0;
		i = getnumber(cmdline);
		while(IntPauseTimer < i) CheckAbort();
	}
}



// this is invoked as a command (ie, date$ = "6/7/2010")
// search through the line looking for the equals sign and step over it,
// evaluate the rest of the command, split it up and save in the system counters
void cmd_date(void) {
	char *arg;
	int d, m, y;
	while(*cmdline && tokenfunction(*cmdline) != op_equal) cmdline++;
	if(!*cmdline) error("Invalid syntax");
	++cmdline;
	arg = getCstring(cmdline);
	{
		getargs(&arg, 5, "-/");						// this is a macro and must be the first executable stmt in a block
		if(argc != 5) error("Invalid syntax");
                if(!S.DateFormat)
                {
                d = atoi(argv[0]);
		m = atoi(argv[2]);
                }
                else
                {
                m = atoi(argv[0]);
		d = atoi(argv[2]);
                }
                y = atoi(argv[4]);
		if(d < 1 || d > 31 || m < 1 || m > 12 || y < 0) error("Invalid date");
		if(y < 100) y += 2000;
		mT4IntEnable(0);       						// disable the timer interrupt to prevent any conflicts while updating
		day = d;
		month = m;
		year = y;
                WriteRTCC();
              mT4IntEnable(1);       						// enable interrupt
	}
}



// this is invoked as a function
void fun_date(void) {
	sret = GetTempStringSpace();						// this will last for the life of the command
        mT4IntEnable(0);       							// disable the timer interrupt to prevent any conflicts while updating
        ReadRTCC();
        if(!S.DateFormat)
        sprintf(sret, "%02d-%02d-%04d", day, month, year);
        else
        sprintf(sret, "%02d-%02d-%04d", month, day, year);
	mT4IntEnable(1);  	     						// enable interrupt
	CtoM(sret);
}
#ifdef OLIMEX

void fun_dow(void) {								// this will last for the life of the command 											// disable the timer interrupt to prevent any conflicts while updating
        ReadRTCC();
        fret=dow;
}
#endif



// this is invoked as a command (ie, time$ = "6:10:45")
// search through the line looking for the equals sign and step over it,
// evaluate the rest of the command, split it up and save in the system counters
void cmd_time(void) {
	char *arg;
	int h = 0;
	int m = 0;
	int s = 0;

	while(*cmdline && tokenfunction(*cmdline) != op_equal) cmdline++;
	if(!*cmdline) error("Invalid syntax");
	++cmdline;
	arg = getCstring(cmdline);
	{
		getargs(&arg, 5, ":");								// this is a macro and must be the first executable stmt in a block
		if(argc%2 == 0) error("Invalid syntax");
		h = atoi(argv[0]);
		if(argc >= 3) m = atoi(argv[2]);
		if(argc == 5) s = atoi(argv[4]);
		if(h < 0 || h > 24 || m < 0 || m > 60 || s < 0 || s > 60) error("Invalid time");
		mT4IntEnable(0);       										// disable the timer interrupt to prevent any conflicts while updating
		hour = h;
		minute = m;
		second = s;
                WriteRTCC();
                mT4IntEnable(1);       										// enable interrupt
	}
}


// this is invoked as a function
void fun_time(void) {
	sret = GetTempStringSpace();						// this will last for the life of the command
	mT4IntEnable(0);       							// disable the timer interrupt to prevent any conflicts while updating
        ReadRTCC();
        sprintf(sret, "%02d:%02d:%02d", hour, minute, second);
	mT4IntEnable(1);  	     						// enable interrupt
	CtoM(sret);
}



void fun_inkey(void){
	sret = GetTempStringSpace();						// this will last for the life of the command
	int i;
	i = MMInkey();
	if(i == -1)
		sret[0] = 0;							// this is a MMBasic string and this is the length
	else
		{
		sret[0] = 1;							// same again...  this is the length
		sret[1] = i;							// and this is the character
	}
}



void cmd_sound(void) {
	int f, d, dcy, period;
	static int sf;

	getargs(&cmdline, 5, ",");
	if(argc < 3 || argc > 5) error("Invalid syntax");

	f = getinteger(argv[0]);										// the frequency
	d = getinteger(argv[2]);										// the duration
	if(argc == 5)
		dcy = MMround(getnumber(argv[4]) * 10);						// the duty cycle
	else
		dcy = 500;

	if(d == 0 || f == 0) { 											// see if the user wants to cancel any playing sound
		SoundPlay = 0;
		CloseTimer2();
#ifdef MAXIMITE
        CloseOC2();
#endif
#ifdef OLIMEX
        CloseOC1();
#endif
                return;
	}

	if(f < 20 || f > 1000000 || d < 0 || dcy < 0 || dcy > 1000) error("Number out of bounds");

	period = BUSFREQ/f;												// convert the frequency into bus cycles
	if(f < 1250) period /= 64;										// adjust if we need to scale the timer's clock

	if(SoundPlay && f == sf)
#ifdef MAXIMITE
            SetDCOC2PWM((period * dcy) / 1000);							// if only changing the duty cycle (avoids glitches)
#endif
#ifdef OLIMEX
            SetDCOC1PWM((period * dcy) / 1000);							// if only changing the duty cycle (avoids glitches)
#endif

        else {
		// we are starting up or changing the frequency so do the full configuration
		// enable the output compare which is used to generate the duty cycle
#ifdef MAXIMITE
            OpenOC2( OC_ON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE , (period * dcy) / 1000, 0x0000 );
#endif
#ifdef OLIMEX
            OpenOC1( OC_ON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE , (period * dcy) / 1000, 0x0000 );
#endif

            // enable timer 2 and set to the desired frequency
	    OpenTimer2(T2_ON | ((f < 1250) ? T2_PS_1_64 : T2_PS_1_1), period);
	}

	sf = f;															// used to detect if the frequency is different
	SoundPlay = d;													// set the duration, each tick is 1mS
}



void cmd_ireturn(void){
	if(InterruptReturn == NULL) error("Return from interrupt when not in an interrupt");
	checkend(cmdline);
	nextstmt = InterruptReturn;
	InterruptReturn = NULL;
}



// set up the tick interrupt
void cmd_settick(void){
	int period;
	int dest;
	getargs(&cmdline, 3, ",");
	if(argc != 3) error("Invalid syntax");
	period = getinteger(argv[0]);
	dest = getinteger(argv[2]);
	if(period < 0 || dest < 0) error("Number out of bounds");
	if(dest == 0)
		inttbl[NBRPINS + 2].intp = NULL;										// turn off the interrupt
	else {
		inttbl[NBRPINS + 2].lohi  = period;
		inttbl[NBRPINS + 2].intp = findline(dest, true);
		TickTimer = 0;												// set the timer running
	}
}


void fun_pos(void){
	fret = (float)MMCharPos;
}





void fun_tab(void) {
	int i;
	char *p;

	sret = p = GetTempStringSpace();							// this will last for the life of the command
	i = getinteger(ep);
	if(i < 1 || i > 255) error("Number out of bounds");
	if(MMCharPos > i) {
		i--;
		*p++ = '\r';
		*p++ = '\n';
	}
	else
		i -= MMCharPos;
	memset(p, ' ', i);
	p[i] = 0;
	CtoM(sret);
}



void cmd_copyright(void) {
#ifdef MAXIMITE
	MMPrintString("Maximite Firmware and MMBasic V" VERSION "\r\n");
	MMPrintString("For updates goto http://geoffg.net/maximite.html\r\n\n");
#endif
#ifdef UBW32
	MMPrintString("UBW32 Firmware and MMBasic V" VERSION "\r\n");
	MMPrintString("For updates goto http://geoffg.net/ubw32.html\r\n\n");
#endif

	MMPrintString("Overall Copyright (c) " YEAR " Geoff Graham.\r\n");
	MMPrintString("I2C, One Wire Support Copyright 2011 Gerard Sexton.\r\n");
        MMPrintString("Setup, Terminal, Full Screen Editor, GameDuino Code (c) 2011 Ken Segler\r\n");
        MMPrintString("USB/CDC HID SD/FAT Support Copyright 2010\r\n");
	MMPrintString("Microchip Technology Incorporated.\r\n\n");

	MMPrintString("Video and keyboard routines from Lucio Di Jasio's\r\n");
	MMPrintString("book \"Programming 32-bit Microcontrollers in C\".\r\n");
//	MMPrintString("USB VID and PIDs are sublicensed by Microchip.\r\n\n");

	MMPrintString("This is free software and comes with absolutely\r\n");
	MMPrintString("no warranty.  It may be used and copied under the\r\n");
	MMPrintString("terms of the GNU General Public License V3.0.\r\n");
}
static inline __attribute__((always_inline)) unsigned char SPICalutateBRG(unsigned int pb_clk, unsigned int spi_clk)
{
    unsigned int brg;

    brg = pb_clk / (2 * spi_clk);

    if(pb_clk % (2 * spi_clk))
        brg++;

    if(brg > 0x100)
        brg = 0x100;

    if(brg)
        brg--;

    return (unsigned char) brg;
}

// output and get a byte via SPI
// this takes 0.2mS to execute with the speed set to H(igh)
void fun_spi(void) {
	volatile int i;
	int t, b;
	int rx, tx, clk;
	int speed = 0, in, out;
	
	getargs(&ep, 9, ",");
	if(argc < 5 || (argc & 0x01) == 0) error("Invalid number of parameters");
	rx = getinteger(argv[0]);
	tx = getinteger(argv[2]);
	clk = getinteger(argv[4]);

	if(argc >= 7)
		out = getinteger(argv[6]);
	else
		out = 0;

        if(rx==9 && tx==8 && clk==10){
//        SPISTAT &= 0x7FFF;
//        OpenSPI((PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_OFF ), SPI_ENABLE);
//        SPIBRG = SPICalutateBRG(GetPeripheralClock(), 8000000);
        putcSPI(out);
        in=getcSPI();
        
 //       SDCardRemoved=true;
        }
        else
        {
  
    if(argc >= 9) {
		switch(toupper(*argv[8])) {
			case 'H':	speed = 0; break;
			case 'M':	speed = 16; break;
			case 'L':	speed = 176; break;
			default:	error("Invalid speed specification");
		}
	}

	if(rx < 1 || rx > 20 || tx < 1 || tx > 20 || clk < 1 || clk > 20) error("Invalid pin number");
	if(ExtCurrentConfig[rx] != EXT_DIG_IN) error("Pin is not configured for input");
	if((ExtCurrentConfig[tx] != EXT_DIG_OUT && ExtCurrentConfig[tx] != EXT_OC_OUT) || (ExtCurrentConfig[clk] != EXT_DIG_OUT && ExtCurrentConfig[clk] != EXT_OC_OUT))
		error("Pin is not configured for output");

	// start the transmit/receive loop
	b = 0b10000000;
	t = in = 0;
	while(b) {
		ExtSet(clk, 0);							// set the clock low (active)
		for(i = speed; i > 0; i--);					// delay to get the right speed
		in <<= 1;							// shift up the input data
		in |= t;							// and add the bit from the previous loop
		ExtSet(tx, out & b); 						// output the next bit
		i = i + 10;							// small delay to get the timing right
		ExtSet(clk, 1);							// strobe data on the low to high transition of the clock
		if(speed) uSec(speed);						// delay to get the right speed
		ExtInp(rx, &t);							// get the next input bit (will be processed in the next loop)
		b >>= 1;							// shift down the mask bit for the output
	}
	in <<= 1;								// this was not completed in the loop
	in |= t;
        }
      	fret = (float)in;
}



void cmd_font(void) {
	int font, scale, reverse;
	char ss[4];														// this will be used to split up the argument line
	char *p, *fname;
	uint16 *p16;
	unsigned int *p32;
	int ch, bt, row, bheight;
	int filenbr, width, height, start, end;

	ss[0] = tokenvalue[TKN_AS];
	ss[1] = GetTokenValue("LOAD");
	ss[2] = GetTokenValue("LOADB");
        ss[3] = 0;
	
	// This block of code handles the command:   FONT LOAD "filename" AS #n
	if(*cmdline == ss[1]) {							// start a new block
		getargs(&cmdline, 4, ss);					// getargs macro must be the first executable stmt in a block
		if(argc != 4) error("Invalid Syntax");
		fname = getCstring(argv[1]);
		makeupper(fname);
		if(*argv[3] == '#') argv[3]++;
		font = getinteger(argv[3]);
		if(font < 1 || font > NBRFONTS) error("Invalid font number");
		font--;
		if(ftbl[font].p != NULL) error("Font number is in use");
		filenbr = FindFreeFileNbr();
		if(filenbr == 0) error("Too many files open");
		MMfopen(fname, "r", filenbr);
		do { *inpbuf = 0; MMgetline(filenbr, inpbuf); } while(*inpbuf == '\'');	// get the parameters line
		p = inpbuf;
		{								// start a new block
			getargs(&p, 7, ",");					// getargs macro must be the first executable stmt in a block
			if(argc != 7) error("Invalid font file format");
			ftbl[font].height = height = getinteger(argv[0]);
			ftbl[font].width = width = getinteger(argv[2]);
			ftbl[font].start = start = getinteger(argv[4]);
			ftbl[font].end = end = getinteger(argv[6]);
			if(height < 1 || height > 64 || width < 1 || start < 0 || start > 255 || end < start || end > 255) error("Invalid font file parameters");
		}
		if(height > 32) bheight = 8;
		else if(height > 16) bheight = 4;
		else bheight = 2;
		ftbl[font].p = getmemory((bheight * width) * ((end + 1) - start));	// get the memory
		p16 = (uint16 *)ftbl[font].p;  p32 = (unsigned int *)ftbl[font].p;
		for(ch = 0; ch <= end - start; ch++) {
			for(row = 0; row < height; row++) {
				if(MMfeof(filenbr)) {				// if end of file then something is wrong
					UnloadFont(font);
					error("Invalid font file format");	// and bail out
				}	
				do { *inpbuf = 0; MMgetline(filenbr, inpbuf); } while(*inpbuf == '\'');	// get the next data line
				for(bt = 0; bt < width && inpbuf[bt] != 0; bt++) {
					if(inpbuf[bt] != ' ') {
						if(bheight == 2)
							p16[bt] |= (1 <<  row);
						else if(bheight == 4)
							p32[bt] |= (1 <<  row);
						else {
							if(row < 32)
								p32[bt] |= (1 <<  row);
							else
								p32[bt + width] |= (1 <<  (row - 32));
						}		
					}		
				}
			}
			p16 += width; p32 += (width * (bheight/4));
		}
		MMfclose(filenbr);
		return;
	}	

        // This block of code handles the command:   FONT LOADB "filename" AS #n
	if(*cmdline == ss[2]) {							// start a new block
		getargs(&cmdline, 4, ss);					// getargs macro must be the first executable stmt in a block
		if(argc != 4) error("Invalid Syntax");
		fname = getCstring(argv[1]);
		makeupper(fname);
		if(*argv[3] == '#') argv[3]++;
		font = getinteger(argv[3]);
		if(font < 1 || font > NBRFONTS) error("Invalid font number");
		font--;
		if(ftbl[font].p != NULL) error("Font number is in use");
		filenbr = FindFreeFileNbr();
		if(filenbr == 0) error("Too many files open");
                end=SDFileExists(fname);
                MMfopen(fname, "r", filenbr);
                ftbl[font].p = getmemory(end);	// get the memory
                p = ftbl[font].p;
                ftbl[font].width=8;
                ftbl[font].height=(end/256);
                //                MMfread((uint16 *)ftbl[font].p,1,end-1,filenbr);
                while(end) { *p=MMfgetc(filenbr); p++; end--;};
                ftbl[font].start=0;
                ftbl[font].end=255;
                ftbl[font].binary = 1;
                MMfclose(filenbr);
		return;
        }
	p = cmdline;
	// this block of code handles the command:   FONT UNLOAD #n
	if((p = checkstring(p, "UNLOAD")) != NULL) {
		skipspace(p);
		if(*p == '#') p++;
		font = getinteger(p);
		if(font < 1 || font > NBRFONTS) error("Invalid font number");
		if(font <= 3) error("Cannot unload this font");
		font--;
		if(ftbl[font].p == NULL) error("Font is not loaded");
		UnloadFont(font);
		return;
	}	

	// This block of code handles the command:   FONT #n, size, reverse
	{									// start a new block
		getargs(&cmdline, 5, ",");					// must be first in the block
		if(argc < 1 || (argc & 0x01) == 0) error("Invalid number of parameters");
		if(*argv[0] == '#') argv[0]++;
		font = getinteger(argv[0]);
		if(font < 1 || font > NBRFONTS || ftbl[font - 1].p == NULL) error("Invalid font number");
		scale = 1;
		reverse = 0;
		if(argc >= 3) scale = getinteger(argv[2]);
		if(argc == 5) reverse = getinteger(argv[4]);
		if(scale < 1 || scale > 8) error("Invalid scale");
		SetFont(font - 1, scale, reverse);
	}
}

		

/***********************************************************************************************
interrupt check
************************************************************************************************/

// check if an interrupt has occured and if so, set the next command to the interrupt routine
// will return true if interrupt detected or false if not
int check_interrupt(void) {
	int i, v;

	if(InterruptReturn != NULL || CurrentLineNbr == 0) return 0;	// skip if we are in an interrupt or in immediate mode

//	GS I2C Start
	if ((I2C_Status & I2C_Status_Interrupt) && (I2C_Status & I2C_Status_Completed)) {
		I2C_Status &= ~I2C_Status_Completed;						// clear completed flag
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = I2C_IntLine;										// set the next stmt to the interrupt location
		return 1;
	}
	if ((I2C_Status & I2C_Status_Slave_Receive_Rdy)) {
		I2C_Status &= ~I2C_Status_Slave_Receive_Rdy;	// clear completed flag
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = I2C_Slave_Receive_IntLine;						// set the next stmt to the interrupt location
		return 1;
	}
	if ((I2C_Status & I2C_Status_Slave_Send_Rdy)) {
		I2C_Status &= ~I2C_Status_Slave_Send_Rdy;					// clear completed flag
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = I2C_Slave_Send_IntLine;							// set the next stmt to the interrupt location
		return 1;
	}
//	GS I2C End

	// interrupt routines for the serial ports
	if(com1_interrupt != NULL && SerialRxStatus(1) >= com1_ilevel) {// do we need to interrupt?
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = com1_interrupt;									// set the next stmt to the interrupt location
		return 1;
	}
	if(com2_interrupt != NULL && SerialRxStatus(2) >= com2_ilevel) {// do we need to interrupt?
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = com2_interrupt;									// set the next stmt to the interrupt location
		return 1;
	}
#ifdef OLIMEX
	if(com3_interrupt != NULL && SerialRxStatus(3) >= com3_ilevel) {// do we need to interrupt?
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = com3_interrupt;									// set the next stmt to the interrupt location
		return 1;
	}
	if(com4_interrupt != NULL && SerialRxStatus(4) >= com4_ilevel) {// do we need to interrupt?
		InterruptReturn = nextstmt;									// save where we are
		nextstmt = com4_interrupt;									// set the next stmt to the interrupt location
		return 1;
        }
#endif
            // ksd pin 0 int
	for(i = 0; i < NBRPINS + 1; i++) {
		if(inttbl[i].intp != NULL) {					// if an interrupt is enabled for this pin
			ExtInp(i, &v);						// get the current value
			// check if interrupt occured
			if((inttbl[i].lohi && inttbl[i].last < v) || (!inttbl[i].lohi && inttbl[i].last > v)) {
				InterruptReturn = nextstmt;			// save where we are
				nextstmt = inttbl[i].intp;			// set the next stmt to the interrupt location
				inttbl[i].last = v;				// save the new pin value
				return 1;
			} else
				inttbl[i].last = v;				// no interrupt, just update the pin value
		}
	}

	// check if the tick interrupt is enabled and if it has occured
	// the tick interrupt uses pin number 0 which is not a valid physical pin
	// inttbl[0].lohi contains the tick period in mSec
	if(inttbl[NBRPINS + 2].intp != NULL && TickTimer > inttbl[NBRPINS + 2].lohi) {
		// reset for the next tick but skip any ticks completely missed
		while(TickTimer > inttbl[NBRPINS + 2].lohi) TickTimer -= inttbl[NBRPINS + 2].lohi;
		InterruptReturn = nextstmt;
		nextstmt = inttbl[NBRPINS + 2].intp;
		return 1;
	}

	return 0;
}


