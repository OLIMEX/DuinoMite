/***********************************************************************************************************************
MMBasic

Custom.c

Handles all the custom commands and functions in MMBasic.  These are commands and functions that are not
normally part of the core BASIC language.  This is a good place to insert your own customised commands.

One Wire support Copyright 2011 Gerard Sexton

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/

#include <p32xxxx.h>
#include <plib.h>

#include "../MMBasic/MMBasic.h"
#include "../Maximite.h"
#include "../IOPorts.h"
// Ingmar's addition start
#include "../Video/Video.h"
// Ingmar's addition end

#define INCLUDE_FUNCTION_DEFINES
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"
#include "Custom.h"

static int LastDiscrepancy;
static int LastFamilyDiscrepancy;
static int LastDeviceFlag;
static int mmOWvalue;								// value of MM.OW
static unsigned char SerialNum[8];
static const unsigned short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
static const unsigned char dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};
static unsigned short utilcrc16;
static unsigned char utilcrc8;


/*************************************************************************************************************************
**************************************************************************************************************************
IMPORTANT:
This module is empty and should be used for your special functions and commands.  In the standard distribution this file  
will never be changed, so your code should be safe here.  You should avoid placing commands and functions in other files as
they may be changed and you would then need to re insert your changes in a new release of the source.

Also, the MIPS core timer is not used in MMBasic so this is also free for a custom use.

**************************************************************************************************************************
**************************************************************************************************************************/


/**************************************************************************************************************************
 custom commands and functions
 each function is responsible for decoding a command
 all function names are in the form cmd_xxxx() (for a basic command) or fun_xxxx() (for a basic function) so,
 if you want to search for the function responsible for the NAME command look for cmd_name

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

 **************************************************************************************************************************/

//	GS OW Start
// send one wire reset and optionally return presence response
void cmd_owReset(void) {
	int pin;
	void *ptr = NULL;

	getargs(&cmdline, 5, ",");
	if (!(argc & 0x01) || (argc < 1) || (argc > 3)) error("Invalid syntax");
	pin = getinteger(argv[0]);
	ow_pinChk(pin);
	if (argc == 3) {
		ptr = findvar(argv[2], V_FIND);
		if (ptr == NULL) error("Presence variable not found");
		if (vartbl[VarIndex].type != T_NBR) error("Presence variable not numeric");
		if (vartbl[VarIndex].dims[1] != 0) error("Array must be one dimensional");
		if (vartbl[VarIndex].dims[0] != 0) {		// An array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + 1) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) )
				error("Insufficient space in array");
		}
	}

// set up initial pin status (open drain, output, high)
	ExtCfg(pin, EXT_NOT_CONFIG);										// set pin to unconfigured
	ow_onOCPin(pin);
	ow_setPin(pin);
	ow_outputPin(pin);

	ow_reset(pin);
	if (argc == 3) *((float*)ptr) = mmOWvalue;
	return;
}


// send one wire data
void cmd_owWrite(void) {
	int pin, flag, len, i, buf[255], type = 0;
	void *ptr;
	char *cp;
	unsigned char *cptr;

	getargs(&cmdline, 99, ",");
	if (!(argc & 0x01) || (argc < 7)) error("Invalid syntax");
	pin = getinteger(argv[0]);
	ow_pinChk(pin);

	flag = getinteger(argv[2]);
	if (flag < 0 || flag > 15) error("Flag must be between 0 and 15");
	len = getinteger(argv[4]);
	if (len < 1 || len > 255) error("Invalid send length");

	// check the first char for a legal variable name
	cp = argv[6];
	skipspace(cp);
	if (isnamestart(*cp)) type = 1;
	if (argc > 7 || (len == 1 && type == 0)) {	// numeric expressions for data
		if (len != ((argc - 5) >> 1)) error("Incorrect number of arguments for send length");
		for (i = 0; i < len; i++) {
			buf[i] = getinteger(argv[i + i + 6]);
		}
	} else {		// an array of float or a string
		ptr = findvar(argv[6], V_NOFIND_NULL);
		if (ptr == NULL) error("Variable/argument not found");
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			if ((*cptr) < len) error("Insufficient data in string");
			cptr++;															// skip the length byte in a MMBasic string
			for (i = 0; i < len; i++) {
				buf[i] = (int)(*(cptr + i));
			}
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + len) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				for (i = 0; i < len; i++) {
					buf[i] = (int)(*((float *)ptr + i));
				}
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}

// set up initial pin status (open drain, output, high)
	ExtCfg(pin, EXT_NOT_CONFIG);										// set pin to unconfigured
	ow_onOCPin(pin);
	ow_setPin(pin);
	ow_outputPin(pin);
	
	if (flag & 0x01) ow_reset(pin);
	for (i = 0; i < len; i++) {
		if (flag & 0x04) {
			ow_writeBit(pin, buf[i]);
		} else {
			ow_writeByte(pin, buf[i]);
		}
	}
	if (flag & 0x02) ow_reset(pin);

	if (flag & 0x08) {																// strong pullup required?
		ow_offOCPin(pin);																// set strong pullup
	}
	return;
}


// read one wire data
void cmd_owRead(void) {
	int pin, flag, option = 0, len, i, buf[255];
	void *ptr = NULL;
	unsigned char *cptr = NULL;

	getargs(&cmdline, 99, ",");
	if (!(argc & 0x01) || (argc < 7)) error("Invalid syntax");
	pin = getinteger(argv[0]);
	ow_pinChk(pin);

	flag = getinteger(argv[2]);
	if (flag < 0 || flag > 15) error("Flag must be between 0 and 15");
	len = getinteger(argv[4]);
	if (len < 1 || len > 255) error("Invalid receive length");

	ptr = findvar(argv[6], V_FIND);
	if (ptr == NULL) error("Variable not found");
	if ((len == 1 && vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] == 0) || argc > 7) {	// numeric expressions for data
		if (len != ((argc - 5) >> 1)) error("Incorrect number of arguments for receive length");
		for (i = 0; i < len; i++) {
			ptr = findvar(argv[i + i + 6], V_FIND);
			if (ptr == NULL) error("Variable not found");
			if (vartbl[VarIndex].type != T_NBR) error("Variable not numeric");
			if (vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
				if ( (((float *)ptr - vartbl[VarIndex].val.fa) + 1) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
					error("Insufficient space in array");
				}
			}
		}
		option = 0x01;
	} else {		// must be an array of float or a string
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			option = 0x02;
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + len) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				option = 0x04;
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}

// set up initial pin status (open drain, output, high)
	ExtCfg(pin, EXT_NOT_CONFIG);										// set pin to unconfigured
	ow_onOCPin(pin);
	ow_setPin(pin);
	ow_outputPin(pin);

	if (flag & 0x01) ow_reset(pin);
	for (i = 0; i < len; i++) {
		if (flag & 0x04) {
			buf[i] = ow_readBit(pin);
		} else {
			buf[i] = ow_readByte(pin);
		}
	}
	if (flag & 0x02) ow_reset(pin);
		
	if (flag & 0x08) {																// strong pullup required?
		ow_offOCPin(pin);																// set strong pullup
	}

	if (option & 0x02) {
		*cptr = (unsigned char)len;
		cptr++;
	}
	for (i = 0; i < len; i++) {
		if (option & 0x01) {															// numerical variables
			ptr = findvar(argv[i + i + 6], V_NOFIND_NULL);
			*((float *)ptr) = buf[i];
		} else if (option & 0x20) {												// string variable
			*(cptr + i) = (unsigned char)buf[i];
		} else {																				// numeric array
			*((float *)ptr + i) = buf[i];
		}
	}
	
	return;
}


// One wire search
// flag:  1 = reset search
//        2 = alarm set
//        4 = family search
//        8 = skip current family
//       16 = verify
//
void cmd_owSearch(void) {
	int pin, flag, option = 0, alarm, i;
	void *ptr = NULL;
	unsigned char *cptr = NULL, buf[8];

	getargs(&cmdline, 99, ",");
	if (!(argc & 0x01) || (argc < 5)) error("Invalid syntax");
	pin = getinteger(argv[0]);
	ow_pinChk(pin);

	flag = getinteger(argv[2]);
	if (flag < 0 || flag > 31) error("Flag must be between 0 and 31");
	if (((flag & 0x01) && flag > 3) || ((flag & 0x04) && flag > 7) || ((flag & 0x08) && flag > 15)) error("Invalid flag combination");

	if ((flag & 0x04) || (flag & 0x10)) {
		ptr = findvar(argv[4], V_NOFIND_NULL);
	} else {
		ptr = findvar(argv[4], V_FIND);
	}
	if (ptr == NULL) error("Variable not found");
	if ((vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] == 0) || argc > 5) {	// numeric expressions for data
		if (8 != ((argc - 3) >> 1)) error("Incorrect number of arguments");
		for (i = 0; i < 8; i++) {
			if (flag & 0x10) {
				ptr = findvar(argv[i + i + 4], V_NOFIND_NULL);
			} else {
				ptr = findvar(argv[i + i + 4], V_FIND);
			}
			if (ptr == NULL) error("Variable not found");
			if (vartbl[VarIndex].type != T_NBR) error("Variable not numeric");
			if (vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
				if ( (((float *)ptr - vartbl[VarIndex].val.fa) + 1) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
					error("Insufficient space in array");
				}
			}
			if ((flag & 0x04) && (i == 0)) buf[0] = (unsigned char)(*((float *)ptr));
			if (flag & 0x10) buf[i] = (unsigned char)(*((float *)ptr));
		}
		option = 0x01;
	} else {		// must be an array of float or a string
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			if ( ((flag & 0x04) && (*cptr < 1)) || ((flag & 0x10) && (*cptr < 8)) ) error("Insufficient data in string");
			if (flag & 0x04) buf[0] = *(cptr + 1);
			if (flag & 0x10) for (i = 0; i < 8; i++) {buf[i] = *(cptr + 1 + i);}
			option = 0x02;
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + 8) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				if (flag & 0x04) buf[0] = (unsigned char)(*((float *)ptr));
				if (flag & 0x10) for (i = 0; i < 8; i++) {buf[i] = (unsigned char)(*((float *)ptr + i));}
				option = 0x04;
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}
	if (flag & 0x02) alarm = 1; else alarm = 0;

	// set up initial pin status (open drain, output, high)
	ExtCfg(pin, EXT_NOT_CONFIG);										// set pin to unconfigured
	ow_onOCPin(pin);
	ow_setPin(pin);
	ow_outputPin(pin);

	if (flag & 0x01) {
		mmOWvalue = ow_first(pin, 1, alarm);
	} else if (flag & 0x04) {
		ow_familySearchSetup(buf[0]);
		mmOWvalue = ow_next(pin, 1, alarm);
	} else if (flag & 0x08) {
		ow_skipFamily();
		mmOWvalue = ow_next(pin, 1, alarm);
	} else if (flag & 0x10) {
		ow_serialNum(buf, 0);
		mmOWvalue = ow_verify(pin, alarm);
	} else {
		mmOWvalue = ow_next(pin, 1, alarm);
	}

	if (option & 0x02) {
		*cptr = 8;
		cptr++;
	}
	for (i = 0; i < 8; i++) {
		if (option & 0x01) {															// numerical variables
			ptr = findvar(argv[i + i + 4], V_NOFIND_NULL);
			*((float *)ptr) = SerialNum[i];
		} else if (option & 0x20) {												// string variable
			*(cptr + i) = SerialNum[i];
		} else {																					// numeric array
			*((float *)ptr + i) = SerialNum[i];
		}
	}
	
	return;
}


void fun_owCRC8(void){
	int len, i, x, type = 0;
	char *cp;
	unsigned char *cptr, buf[255], uc = 0;
	void *ptr;

	getargs(&ep, 99, ",");										// this is a macro and must be the first executable stmt in a block
	if (!(argc & 0x01) || (argc < 3)) error("Invalid syntax");
	len = getinteger(argv[0]);
	if ((len < 1) || (len > 255)) error("Invalid length");

	// check the first char for a legal variable name
	cp = argv[2];
	skipspace(cp);
	if (isnamestart(*cp)) type = 1;
	if (argc > 3 || (len == 1 && type == 0)) {	// numeric expressions for data
		if (len != ((argc - 1) >> 1)) error("Incorrect number of arguments for send length");
		for (i = 0; i < len; i++) {
			x = getinteger(argv[i + i + 6]);
			if (x < 0 || x > 255) error("Value not between 0 and 255");
			buf[i] = (unsigned char)x;
		}
	} else {		// an array of float or a string
		ptr = findvar(argv[2], V_NOFIND_NULL);
		if (ptr == NULL) error("Variable/argument not found");
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			if ((*cptr) < len) error("Insufficient data in string");
			cptr++;															// skip the length byte in a MMBasic string
			for (i = 0; i < len; i++) {
				buf[i] = *(cptr + i);
			}
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + len) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				for (i = 0; i < len; i++) {
					x = (int)(*((float *)ptr + i));
					if (x < 0 || x > 255) error("Value not between 0 and 255");
					buf[i] = (unsigned char)x;
				}
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}
	setcrc8(0);
	for (i = 0; i < len; i++) {
		uc = docrc8(buf[i]);
	}
	fret = (float)uc;
}


void fun_owCRC16(void){
	int len, i, x, type = 0;
	char *cp;
	unsigned char *cptr;
	void *ptr;
	unsigned short buf[255], us = 0;

	getargs(&ep, 99, ",");										// this is a macro and must be the first executable stmt in a block
	if (!(argc & 0x01) || (argc < 3)) error("Invalid syntax");
	len = getinteger(argv[0]);
	if ((len < 1) || (len > 255)) error("Invalid length");

	// check the first char for a legal variable name
	cp = argv[2];
	skipspace(cp);
	if (isnamestart(*cp)) type = 1;
	if (argc > 3 || (len == 1 && type == 0)) {	// numeric expressions for data
		if (len != ((argc - 1) >> 1)) error("Incorrect number of arguments for send length");
		for (i = 0; i < len; i++) {
			x = getinteger(argv[i + i + 6]);
			if (x < 0 || x > 65535) error("Value not between 0 and 65535");
			buf[i] = (unsigned short)x;
		}
	} else {		// an array of float or a string
		ptr = findvar(argv[2], V_NOFIND_NULL);
		if (ptr == NULL) error("Variable/argument not found");
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			if ((*cptr) < len) error("Insufficient data in string");
			cptr++;															// skip the length byte in a MMBasic string
			for (i = 0; i < len; i++) {
				buf[i] = (unsigned short)(*(cptr + i));
			}
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + len) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				for (i = 0; i < len; i++) {
					x = (int)(*((float *)ptr + i));
					if (x < 0 || x > 65535) error("Value not between 0 and 65535");
					buf[i] = (unsigned short)x;
				}
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}
	setcrc16(0);
	for (i = 0; i < len; i++) {
		us = docrc16(buf[i]);
	}
	fret = (float)us;
}


void fun_mmOW(void) {
	fret = (float)mmOWvalue;
}


void ow_pinChk(int pin) {
	#ifdef OLIMEX
		if ((pin < 1) || (pin > NBRPINS) || (pin == 11)) error("Invalid I/O pin");
	#else
		if ((pin < 11) || (pin > NBRPINS)) error("Invalid I/O pin");
	#endif
	if (ExtCurrentConfig[pin] >= EXT_COM_RESERVED) error("Pin is allocated to a communications function");
	return;
}


// send one wire reset and detect presence response - returns 1 if found else 0
int ow_reset(int pin) {
	asm("di");
	ow_clrPin(pin);																	// drive pin low
	asm("ei");
	uSec(481);																			// wait 481uSec
	asm("di");
	ow_setPin(pin);																	// release the bus
	ow_inputPin(pin);																// set as input
	asm("ei");
	uSec(70);																				// wait 70uSec
	asm("di");
	mmOWvalue = ow_readPin(pin) ^ 0x01;							// read pin and invert response
	ow_outputPin(pin);															// set as output
	asm("ei");
	uSec(411);																			// wait 411uSec
	return mmOWvalue;
}


void ow_writeByte(int pin, int data) {
	int loop;

	for (loop = 0; loop < 8; loop++) {
		ow_writeBit(pin, data & 0x01);
		data >>= 1;
	}
	return;
}


int ow_readByte(int pin) {
	int loop, result = 0;

	for (loop = 0; loop < 8; loop++) {
		result >>= 1;
		if (ow_readBit(pin)) result |= 0x80;
	}
	return result;
}


int ow_verifyByte(int pin, int data) {
	return (ow_touchByte(pin, data) == data) ? 1 : 0;
}


int ow_touchByte(int pin, int data) {
	int loop, result = 0;

	for (loop = 0; loop < 8; loop++) {
		result >>= 1;
		if (data & 0x01) {	// if sending a '1' then read a bit else write a '0'
			if (ow_readBit(pin)) result |= 0x80;
		} else {
			ow_writeBit(pin, 0x00);
		}
		data >>= 1;
	}
	return result;
}


int ow_touchBit(int pin, int owbit) {
	int result = 0;

	if (owbit & 0x01) {	// if sending a '1' then read a bit else write a '0'
		if (ow_readBit(pin)) result = 1;
	} else {
		ow_writeBit(pin, 0x00);
	}
	return result;
}


void ow_writeBit(int pin, int owbit) {
	int status_save;

	if (owbit) {
		// Write '1' bit
		asm("di");
		status_save = _CP0_GET_STATUS();
		_CP0_SET_STATUS(status_save | 0x1000);				// allow interrupts if priority greater than 4
		ow_clrPin(pin);																// drive pin low
		asm("ei");
		uSec(6);																			// wait 6uSec
		asm("di");
		ow_setPin(pin);																// release the bus
		_CP0_SET_STATUS(status_save);									// allow all interrupts
		asm("ei");
		uSec(64);																			// wait 64Sec
	} else {
		// Write '0' bit
		asm("di");
		ow_clrPin(pin);																// drive pin low
		asm("ei");
		uSec(60);																			// wait 60uSec
		asm("di");
		ow_setPin(pin);																// release the bus
		asm("ei");
		uSec(10);																			// wait 10Sec
	}
	return;
}


int ow_readBit(int pin) {
	int status_save, result;

	asm("di");
	status_save = _CP0_GET_STATUS();
	_CP0_SET_STATUS(status_save | 0x1000);					// allow interrupts if priority greater than 4
	ow_clrPin(pin);																	// drive pin low
	asm("ei");
	uSec(6);																				// wait 6uSec
	asm("di");
	ow_setPin(pin);																	// release the bus
	ow_inputPin(pin);																// set as input
	asm("ei");
	uSec(8);																				// wait 8uSec
	asm("di");
	result = ow_readPin(pin);												// read pin
	ow_outputPin(pin);															// set as output
	_CP0_SET_STATUS(status_save);										// allow all interrupts
	asm("ei");
	uSec(56);																				// wait 56uSec
	return result;
}


//--------------------------------------------------------------------------
// The 'ow_first' finds the first device on the 1-Wire Net
//
// When 'alarm_only' is TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'pin'        - I/O Pin.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                        Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): There are no devices on the 1-Wire Net.
//
int ow_first(int pin, int do_reset, int alarm_only) {
	// reset the search state
	LastDiscrepancy = 0;
	LastDeviceFlag = FALSE;
	LastFamilyDiscrepancy = 0;

	return ow_next(pin, do_reset, alarm_only);
}


//--------------------------------------------------------------------------
// The 'ow_next' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'ow_first' function.
//
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'pin'        - I/O Pin.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
int ow_next(int pin, int do_reset, int alarm_only) {
	int bit_test, search_direction, bit_number;
	int last_zero, serial_byte_number, next_result;
	unsigned char serial_byte_mask, lastcrc8;

	// initialize for search
	bit_number = 1;
	last_zero = 0;
	serial_byte_number = 0;
	serial_byte_mask = 1;
	next_result = 0;
	lastcrc8 = 0;
	setcrc8(0);

	// if the last call was not the last one
	if (!LastDeviceFlag) {
		// check if reset first is requested
		if (do_reset)	{
			// reset the 1-wire
			// if there are no parts on 1-wire, return FALSE
			if (!ow_reset(pin)) {
				// reset the search
				LastDiscrepancy = 0;
				LastFamilyDiscrepancy = 0;
				return FALSE;
			}
		}

		// If finding alarming devices issue a different command
		if (alarm_only) {
			ow_writeByte(pin, 0xEC);  // issue the alarming search command
		} else {
			ow_writeByte(pin, 0xF0);  // issue the search command
		}

		// loop to do the search
		do {
			// read a bit and its compliment
			bit_test = ow_touchBit(pin, 1) << 1;
			bit_test |= ow_touchBit(pin, 1);

			// check for no devices on 1-wire
			if (bit_test == 3) break;
			else {
				// all devices coupled have 0 or 1
				if (bit_test > 0) {
				  search_direction = !(bit_test & 0x01);  // bit write value for search
				} else {
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (bit_number < LastDiscrepancy)
						search_direction = ((SerialNum[serial_byte_number] & serial_byte_mask) > 0);
					else
						// if equal to last pick 1, if not then pick 0
						search_direction = (bit_number == LastDiscrepancy);

					// if 0 was picked then record its position in LastZero
					if (search_direction == 0) {
						last_zero = bit_number;

						// check for Last discrepancy in family
						if (last_zero < 9) LastFamilyDiscrepancy = last_zero;
					}
				}

				// set or clear the bit in the SerialNum byte serial_byte_number
				// with mask serial_byte_mask
				if (search_direction == 1)
				  SerialNum	[serial_byte_number] |= serial_byte_mask;
				else
				  SerialNum	[serial_byte_number] &= ~serial_byte_mask;

				// serial number search direction write bit
				ow_touchBit(pin, search_direction);

				// increment the byte counter bit_number
				// and shift the mask serial_byte_mask
				bit_number++;
				serial_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum byte serial_byte_number
				// and reset mask
				if (serial_byte_mask == 0) {
					// The below has been added to accomodate the valid CRC with the
					// possible changing serial number values of the DS28E04.
					if (((SerialNum[0] & 0x7F) == 0x1C) && (serial_byte_number == 1))
						lastcrc8 = docrc8(0x7F);
					else
						lastcrc8 = docrc8(SerialNum[serial_byte_number]);  // accumulate the CRC

					serial_byte_number++;
					serial_byte_mask = 1;
				}
			}
		} while(serial_byte_number < 8);  // loop until through all SerialNum[portnum] bytes 0-7

		// if the search was successful then
		if (!((bit_number < 65) || lastcrc8)) {
			// search successful so set LastDiscrepancy, LastDeviceFlag, next_result
			LastDiscrepancy = last_zero;
			LastDeviceFlag = (LastDiscrepancy == 0);
			next_result = TRUE;
		}
	}

	// if no device found then reset counters so next 'next' will be
	// like a first
	if (!next_result || !SerialNum[0]) {
		LastDiscrepancy = 0;
		LastDeviceFlag = FALSE;
		LastFamilyDiscrepancy = 0;
		next_result = FALSE;
	}

	return next_result;
}


//--------------------------------------------------------------------------
// The 'ow_verify' function checks that the device with the serial number
// in the global SerialNum buffer is present.
//
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'pin'        - I/O Pin.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : device verified present.
//            FALSE (0): device not present.
//
int ow_verify(int pin, int alarm_only) {
	unsigned char serialNum_backup[8];
	int i, rslt, ld_backup, ldf_backup, lfd_backup;

	// keep a backup copy of the current state
	for (i = 0; i < 8; i++) serialNum_backup[i] = SerialNum[i];
	ld_backup = LastDiscrepancy;
	ldf_backup = LastDeviceFlag;
	lfd_backup = LastFamilyDiscrepancy;

	// set search to find the same device
	LastDiscrepancy = 64;
	LastDeviceFlag = FALSE;
	if (ow_next(pin, 1, alarm_only)) {
		// check if same device found
		rslt = TRUE;
		for (i = 0; i < 8; i++) {
			if (serialNum_backup[i] != SerialNum[i]) {
				rslt = FALSE;
				break;
			}
		}
	} else {
		rslt = FALSE;
	}

	// restore the search state
	for (i = 0; i < 8; i++) SerialNum[i] = serialNum_backup[i];
	LastDiscrepancy = ld_backup;
	LastDeviceFlag = ldf_backup;
	LastFamilyDiscrepancy = lfd_backup;

	// return the result of the verify
	return rslt;
}


//--------------------------------------------------------------------------
// The 'ow_serialNum' function either reads or sets the SerialNum buffer
// that is used in the search functions 'ow_first' and 'ow_next'.
// This function contains two parameters, 'serialnum_buf' is a pointer
// to a buffer provided by the caller.  'serialnum_buf' should point to
// an array of 8 unsigned chars.  The second parameter is a flag called
// 'do_read' that is TRUE (1) if the operation is to read and FALSE
// (0) if the operation is to set the internal SerialNum buffer from
// the data in the provided buffer.
//
// 'serialnum_buf' - buffer to that contains the serial number to set
//                   when do_read = FALSE (0) and buffer to get the serial
//                   number when do_read = TRUE (1).
// 'do_read'       - flag to indicate reading (1) or setting (0) the current
//                   serial number.
//
void ow_serialNum(unsigned char *serialnum_buf, int do_read)
{
	unsigned char i;

	// read the internal buffer and place in 'serialnum_buf'
	if (do_read) {
		for (i = 0; i < 8; i++) serialnum_buf[i] = SerialNum[i];
	} else { // set the internal buffer from the data in 'serialnum_buf'
		for (i = 0; i < 8; i++) SerialNum[i] = serialnum_buf[i];
	}
}


//--------------------------------------------------------------------------
// Setup the search algorithm to find a certain family of devices
// the next time a search function is called 'owNext'.
//
// 'search_family' - family code type to set the search algorithm to find
//                   next.
//
void ow_familySearchSetup(int search_family)
{
	int i;

	// set the search state to find SearchFamily type devices
	SerialNum[0] = search_family;
	for (i = 1; i < 8; i++) SerialNum[i] = 0;
	LastDiscrepancy = 64;
	LastDeviceFlag = FALSE;
}


//--------------------------------------------------------------------------
// Set the current search state to skip the current family code.
//
void ow_skipFamily(void)
{
	// set the Last discrepancy to last family discrepancy
	LastDiscrepancy = LastFamilyDiscrepancy;
	LastFamilyDiscrepancy = 0;

	// check for end of list
	if (LastDiscrepancy == 0) LastDeviceFlag = TRUE;
}


void setcrc16(unsigned short reset) {
	utilcrc16 = reset;
	return;
}


unsigned short docrc16(unsigned short cdata) {
	cdata = (cdata ^ (utilcrc16 & 0xff)) & 0xff;
	utilcrc16 >>= 8;

	if (oddparity[cdata & 0xf] ^ oddparity[cdata >> 4])	utilcrc16 ^= 0xc001;

	cdata <<= 6;
	utilcrc16 ^= cdata;
	cdata <<= 1;
	utilcrc16 ^= cdata;

	return utilcrc16;
}


void setcrc8(unsigned char reset) {
	utilcrc8 = reset;
	return;
}


unsigned char docrc8(unsigned char cdata) {
	utilcrc8 = dscrc_table[utilcrc8 ^ cdata];
	return utilcrc8;
}


void ow_onOCPin(int pin) {
	switch(pin) {
		case 1:  P_E1_ODCON;  return;
		case 2:  P_E2_ODCON;  return;
		case 3:  P_E3_ODCON;  return;
		case 4:	 P_E4_ODCON;  return;
		case 5:	 P_E5_ODCON;  return;
		case 6:	 P_E6_ODCON;  return;
		case 7:	 P_E7_ODCON;  return;
		case 8:	 P_E8_ODCON;  return;
		case 9:	 P_E9_ODCON;  return;
		case 10: P_E10_ODCON; return;
		case 11: P_E11_ODCON; return;
		case 12: P_E12_ODCON; return;
		case 13: P_E13_ODCON; return;
		case 14: P_E14_ODCON; return;
		case 15: P_E15_ODCON; return;
		case 16: P_E16_ODCON; return;
		case 17: P_E17_ODCON; return;
		case 18: P_E18_ODCON; return;
		case 19: P_E19_ODCON; return;
		case 20: P_E20_ODCON; return;
#ifdef UBW32
		case 21: P_E21_ODCON;	return;
		case 22: P_E22_ODCON;	return;
		case 23: P_E23_ODCON;	return;
		case 24: P_E24_ODCON;	return;
		case 25: P_E25_ODCON;	return;
		case 26: P_E26_ODCON;	return;
		case 27: P_E27_ODCON;	return;
		case 28: P_E28_ODCON;	return;
		case 29: P_E29_ODCON;	return;
		case 30: P_E30_ODCON;	return;
		case 31: P_E31_ODCON;	return;
		case 32: P_E32_ODCON;	return;
		case 33: P_E33_ODCON;	return;
		case 34: P_E34_ODCON;	return;
		case 35: P_E35_ODCON;	return;
		case 36: P_E36_ODCON;	return;
		case 37: P_E37_ODCON;	return;
		case 38: P_E38_ODCON;	return;
		case 39: P_E39_ODCON;	return;
		case 40: P_E40_ODCON;	return;
		case 41: P_E41_ODCON;	return;
		case 42: P_E42_ODCON;	return;
		case 43: P_E43_ODCON;	return;
		case 44: P_E44_ODCON;	return;
		case 45: P_E45_ODCON;	return;
		case 46: P_E46_ODCON;	return;
		case 47: P_E47_ODCON;	return;
		case 48: P_E48_ODCON;	return;
		case 49: P_E49_ODCON;	return;
		case 50: P_E50_ODCON;	return;
	#endif
	}
	return;
}


void ow_offOCPin(int pin) {
	switch(pin) {
		case 1:  P_E1_ODCOFF;  return;
		case 2:  P_E2_ODCOFF;  return;
		case 3:  P_E3_ODCOFF;  return;
		case 4:	 P_E4_ODCOFF;  return;
		case 5:	 P_E5_ODCOFF;  return;
		case 6:	 P_E6_ODCOFF;  return;
		case 7:	 P_E7_ODCOFF;  return;
		case 8:	 P_E8_ODCOFF;  return;
		case 9:	 P_E9_ODCOFF;  return;
		case 10: P_E10_ODCOFF; return;
		case 11: P_E11_ODCOFF; return;
		case 12: P_E12_ODCOFF; return;
		case 13: P_E13_ODCOFF; return;
		case 14: P_E14_ODCOFF; return;
		case 15: P_E15_ODCOFF; return;
		case 16: P_E16_ODCOFF; return;
		case 17: P_E17_ODCOFF; return;
		case 18: P_E18_ODCOFF; return;
		case 19: P_E19_ODCOFF; return;
		case 20: P_E20_ODCOFF; return;
#ifdef UBW32
		case 21: P_E21_ODCOFF;	return;
		case 22: P_E22_ODCOFF;	return;
		case 23: P_E23_ODCOFF;	return;
		case 24: P_E24_ODCOFF;	return;
		case 25: P_E25_ODCOFF;	return;
		case 26: P_E26_ODCOFF;	return;
		case 27: P_E27_ODCOFF;	return;
		case 28: P_E28_ODCOFF;	return;
		case 29: P_E29_ODCOFF;	return;
		case 30: P_E30_ODCOFF;	return;
		case 31: P_E31_ODCOFF;	return;
		case 32: P_E32_ODCOFF;	return;
		case 33: P_E33_ODCOFF;	return;
		case 34: P_E34_ODCOFF;	return;
		case 35: P_E35_ODCOFF;	return;
		case 36: P_E36_ODCOFF;	return;
		case 37: P_E37_ODCOFF;	return;
		case 38: P_E38_ODCOFF;	return;
		case 39: P_E39_ODCOFF;	return;
		case 40: P_E40_ODCOFF;	return;
		case 41: P_E41_ODCOFF;	return;
		case 42: P_E42_ODCOFF;	return;
		case 43: P_E43_ODCOFF;	return;
		case 44: P_E44_ODCOFF;	return;
		case 45: P_E45_ODCOFF;	return;
		case 46: P_E46_ODCOFF;	return;
		case 47: P_E47_ODCOFF;	return;
		case 48: P_E48_ODCOFF;	return;
		case 49: P_E49_ODCOFF;	return;
		case 50: P_E50_ODCOFF;	return;
	#endif
	}
	return;
}


void ow_setPin(int pin) {
	switch(pin) {
		case 1:  P_E1_WRITE1;  return;
		case 2:  P_E2_WRITE1;  return;
		case 3:  P_E3_WRITE1;  return;
		case 4:	 P_E4_WRITE1;  return;
		case 5:	 P_E5_WRITE1;  return;
		case 6:	 P_E6_WRITE1;  return;
		case 7:	 P_E7_WRITE1;  return;
		case 8:	 P_E8_WRITE1;  return;
		case 9:	 P_E9_WRITE1;  return;
		case 10: P_E10_WRITE1; return;
		case 11: P_E11_WRITE1; return;
		case 12: P_E12_WRITE1; return;
		case 13: P_E13_WRITE1; return;
		case 14: P_E14_WRITE1; return;
		case 15: P_E15_WRITE1; return;
		case 16: P_E16_WRITE1; return;
		case 17: P_E17_WRITE1; return;
		case 18: P_E18_WRITE1; return;
		case 19: P_E19_WRITE1; return;
		case 20: P_E20_WRITE1; return;
#ifdef UBW32
		case 21: P_E21_WRITE1;	return;
		case 22: P_E22_WRITE1;	return;
		case 23: P_E23_WRITE1;	return;
		case 24: P_E24_WRITE1;	return;
		case 25: P_E25_WRITE1;	return;
		case 26: P_E26_WRITE1;	return;
		case 27: P_E27_WRITE1;	return;
		case 28: P_E28_WRITE1;	return;
		case 29: P_E29_WRITE1;	return;
		case 30: P_E30_WRITE1;	return;
		case 31: P_E31_WRITE1;	return;
		case 32: P_E32_WRITE1;	return;
		case 33: P_E33_WRITE1;	return;
		case 34: P_E34_WRITE1;	return;
		case 35: P_E35_WRITE1;	return;
		case 36: P_E36_WRITE1;	return;
		case 37: P_E37_WRITE1;	return;
		case 38: P_E38_WRITE1;	return;
		case 39: P_E39_WRITE1;	return;
		case 40: P_E40_WRITE1;	return;
		case 41: P_E41_WRITE1;	return;
		case 42: P_E42_WRITE1;	return;
		case 43: P_E43_WRITE1;	return;
		case 44: P_E44_WRITE1;	return;
		case 45: P_E45_WRITE1;	return;
		case 46: P_E46_WRITE1;	return;
		case 47: P_E47_WRITE1;	return;
		case 48: P_E48_WRITE1;	return;
		case 49: P_E49_WRITE1;	return;
		case 50: P_E50_WRITE1;	return;
	#endif
	}
	return;
}


void ow_clrPin(int pin) {
	switch(pin) {
		case 1:  P_E1_WRITE0;  return;
		case 2:  P_E2_WRITE0;  return;
		case 3:  P_E3_WRITE0;  return;
		case 4:	 P_E4_WRITE0;  return;
		case 5:	 P_E5_WRITE0;  return;
		case 6:	 P_E6_WRITE0;  return;
		case 7:	 P_E7_WRITE0;  return;
		case 8:	 P_E8_WRITE0;  return;
		case 9:	 P_E9_WRITE0;  return;
		case 10: P_E10_WRITE0; return;
		case 11: P_E11_WRITE0; return;
		case 12: P_E12_WRITE0; return;
		case 13: P_E13_WRITE0; return;
		case 14: P_E14_WRITE0; return;
		case 15: P_E15_WRITE0; return;
		case 16: P_E16_WRITE0; return;
		case 17: P_E17_WRITE0; return;
		case 18: P_E18_WRITE0; return;
		case 19: P_E19_WRITE0; return;
		case 20: P_E20_WRITE0; return;
#ifdef UBW32
		case 21: P_E21_WRITE0;	return;
		case 22: P_E22_WRITE0;	return;
		case 23: P_E23_WRITE0;	return;
		case 24: P_E24_WRITE0;	return;
		case 25: P_E25_WRITE0;	return;
		case 26: P_E26_WRITE0;	return;
		case 27: P_E27_WRITE0;	return;
		case 28: P_E28_WRITE0;	return;
		case 29: P_E29_WRITE0;	return;
		case 30: P_E30_WRITE0;	return;
		case 31: P_E31_WRITE0;	return;
		case 32: P_E32_WRITE0;	return;
		case 33: P_E33_WRITE0;	return;
		case 34: P_E34_WRITE0;	return;
		case 35: P_E35_WRITE0;	return;
		case 36: P_E36_WRITE0;	return;
		case 37: P_E37_WRITE0;	return;
		case 38: P_E38_WRITE0;	return;
		case 39: P_E39_WRITE0;	return;
		case 40: P_E40_WRITE0;	return;
		case 41: P_E41_WRITE0;	return;
		case 42: P_E42_WRITE0;	return;
		case 43: P_E43_WRITE0;	return;
		case 44: P_E44_WRITE0;	return;
		case 45: P_E45_WRITE0;	return;
		case 46: P_E46_WRITE0;	return;
		case 47: P_E47_WRITE0;	return;
		case 48: P_E48_WRITE0;	return;
		case 49: P_E49_WRITE0;	return;
		case 50: P_E50_WRITE0;	return;
	#endif
	}
	return;
}


void ow_inputPin(int pin) {
	switch(pin) {
		case 1:  P_E1_TRISINP;  return;
		case 2:  P_E2_TRISINP;  return;
		case 3:  P_E3_TRISINP;  return;
		case 4:	 P_E4_TRISINP;  return;
		case 5:	 P_E5_TRISINP;  return;
		case 6:	 P_E6_TRISINP;  return;
		case 7:	 P_E7_TRISINP;  return;
		case 8:	 P_E8_TRISINP;  return;
		case 9:	 P_E9_TRISINP;  return;
		case 10: P_E10_TRISINP; return;
		case 11: P_E11_TRISINP; return;
		case 12: P_E12_TRISINP; return;
		case 13: P_E13_TRISINP; return;
		case 14: P_E14_TRISINP; return;
		case 15: P_E15_TRISINP; return;
		case 16: P_E16_TRISINP; return;
		case 17: P_E17_TRISINP; return;
		case 18: P_E18_TRISINP; return;
		case 19: P_E19_TRISINP; return;
		case 20: P_E20_TRISINP; return;
#ifdef UBW32
		case 21: P_E21_TRISINP;	return;
		case 22: P_E22_TRISINP;	return;
		case 23: P_E23_TRISINP;	return;
		case 24: P_E24_TRISINP;	return;
		case 25: P_E25_TRISINP;	return;
		case 26: P_E26_TRISINP;	return;
		case 27: P_E27_TRISINP;	return;
		case 28: P_E28_TRISINP;	return;
		case 29: P_E29_TRISINP;	return;
		case 30: P_E30_TRISINP;	return;
		case 31: P_E31_TRISINP;	return;
		case 32: P_E32_TRISINP;	return;
		case 33: P_E33_TRISINP;	return;
		case 34: P_E34_TRISINP;	return;
		case 35: P_E35_TRISINP;	return;
		case 36: P_E36_TRISINP;	return;
		case 37: P_E37_TRISINP;	return;
		case 38: P_E38_TRISINP;	return;
		case 39: P_E39_TRISINP;	return;
		case 40: P_E40_TRISINP;	return;
		case 41: P_E41_TRISINP;	return;
		case 42: P_E42_TRISINP;	return;
		case 43: P_E43_TRISINP;	return;
		case 44: P_E44_TRISINP;	return;
		case 45: P_E45_TRISINP;	return;
		case 46: P_E46_TRISINP;	return;
		case 47: P_E47_TRISINP;	return;
		case 48: P_E48_TRISINP;	return;
		case 49: P_E49_TRISINP;	return;
		case 50: P_E50_TRISINP;	return;
	#endif
	}
	return;
}


void ow_outputPin(int pin) {
	switch(pin) {
		case 1:  P_E1_TRISOUT;  return;
		case 2:  P_E2_TRISOUT;  return;
		case 3:  P_E3_TRISOUT;  return;
		case 4:	 P_E4_TRISOUT;  return;
		case 5:	 P_E5_TRISOUT;  return;
		case 6:	 P_E6_TRISOUT;  return;
		case 7:	 P_E7_TRISOUT;  return;
		case 8:	 P_E8_TRISOUT;  return;
		case 9:	 P_E9_TRISOUT;  return;
		case 10: P_E10_TRISOUT; return;
		case 11: P_E11_TRISOUT; return;
		case 12: P_E12_TRISOUT; return;
		case 13: P_E13_TRISOUT; return;
		case 14: P_E14_TRISOUT; return;
		case 15: P_E15_TRISOUT; return;
		case 16: P_E16_TRISOUT; return;
		case 17: P_E17_TRISOUT; return;
		case 18: P_E18_TRISOUT; return;
		case 19: P_E19_TRISOUT; return;
		case 20: P_E20_TRISOUT; return;
#ifdef UBW32
		case 21: P_E21_TRISOUT;	return;
		case 22: P_E22_TRISOUT;	return;
		case 23: P_E23_TRISOUT;	return;
		case 24: P_E24_TRISOUT;	return;
		case 25: P_E25_TRISOUT;	return;
		case 26: P_E26_TRISOUT;	return;
		case 27: P_E27_TRISOUT;	return;
		case 28: P_E28_TRISOUT;	return;
		case 29: P_E29_TRISOUT;	return;
		case 30: P_E30_TRISOUT;	return;
		case 31: P_E31_TRISOUT;	return;
		case 32: P_E32_TRISOUT;	return;
		case 33: P_E33_TRISOUT;	return;
		case 34: P_E34_TRISOUT;	return;
		case 35: P_E35_TRISOUT;	return;
		case 36: P_E36_TRISOUT;	return;
		case 37: P_E37_TRISOUT;	return;
		case 38: P_E38_TRISOUT;	return;
		case 39: P_E39_TRISOUT;	return;
		case 40: P_E40_TRISOUT;	return;
		case 41: P_E41_TRISOUT;	return;
		case 42: P_E42_TRISOUT;	return;
		case 43: P_E43_TRISOUT;	return;
		case 44: P_E44_TRISOUT;	return;
		case 45: P_E45_TRISOUT;	return;
		case 46: P_E46_TRISOUT;	return;
		case 47: P_E47_TRISOUT;	return;
		case 48: P_E48_TRISOUT;	return;
		case 49: P_E49_TRISOUT;	return;
		case 50: P_E50_TRISOUT;	return;
	#endif
	}
	return;
}


int ow_readPin(int pin) {
	switch(pin) {
		case 1:  return P_E1_READ;
		case 2:  return P_E2_READ;
		case 3:  return P_E3_READ;
		case 4:	 return P_E4_READ;
		case 5:	 return P_E5_READ;
		case 6:	 return P_E6_READ;
		case 7:	 return P_E7_READ;
		case 8:	 return P_E8_READ;
		case 9:	 return P_E9_READ;
		case 10: return P_E10_READ;
		case 11: return P_E11_READ;
		case 12: return P_E12_READ;
		case 13: return P_E13_READ;
		case 14: return P_E14_READ;
		case 15: return P_E15_READ;
		case 16: return P_E16_READ;
		case 17: return P_E17_READ;
		case 18: return P_E18_READ;
		case 19: return P_E19_READ;
		case 20: return P_E20_READ;
#ifdef UBW32
		case 21: return P_E21_READ;
		case 22: return P_E22_READ;
		case 23: return P_E23_READ;
		case 24: return P_E24_READ;
		case 25: return P_E25_READ;
		case 26: return P_E26_READ;
		case 27: return P_E27_READ;
		case 28: return P_E28_READ;
		case 29: return P_E29_READ;
		case 30: return P_E30_READ;
		case 31: return P_E31_READ;
		case 32: return P_E32_READ;
		case 33: return P_E33_READ;
		case 34: return P_E34_READ;
		case 35: return P_E35_READ;
		case 36: return P_E36_READ;
		case 37: return P_E37_READ;
		case 38: return P_E38_READ;
		case 39: return P_E39_READ;
		case 40: return P_E40_READ;
		case 41: return P_E41_READ;
		case 42: return P_E42_READ;
		case 43: return P_E43_READ;
		case 44: return P_E44_READ;
		case 45: return P_E45_READ;
		case 46: return P_E46_READ;
		case 47: return P_E47_READ;
		case 48: return P_E48_READ;
		case 49: return P_E49_READ;
		case 50: return P_E50_READ;
	#endif
	}
	return 0;
}

//	GS OW End

////////////////////////////////////////////////////////////////////
// Start of Ingmar Meins code additions.
// SHIFTOUT dpin,cpin,mode,O$,bits
// SHIFTIN dpin,cpin,mode,I$,bits
// DM.VMEM
// LOADBMP FILENAME$,x,y,m
////////////////////////////////////////////////////////////////////

// This is invoked as a command (ie, shiftout(Dpin, Clkpin, Mode, OutStr, NumBits) )
// Remember that the string returned in outstr will have the length in byte 0, outstr[0]

#define uDly(a)		{WriteCoreTimer(0); while(ReadCoreTimer() < a);}

void cmd_shiftout(void) {
	int dpin, cpin, mode, nbits;
	int ix, iy; // loop counter
	int cnt; // number of bytes to output.
	unsigned char mybyte;

	char * outstr;

	const int BDLY = 3; // Determines clocking rate. # of CoreTimer cycles.

	getargs(&cmdline, 9, ","); // Get the arguments, we should have five, comma seperated.

	if (argc < 9) error("Invalid syntax"); // Test number of arguments given.

	dpin = getinteger(argv[0]); // Data pin
	cpin = getinteger(argv[2]); // Clock pin
	mode = getinteger(argv[4]); // Mode
	outstr = getstring(argv[6]); // String to clock out,treated as an array of bytes.
	nbits = getinteger(argv[8]); // Number of bits to clock out

	if (nbits < 1 || nbits > (outstr[0] * 8)) error("Too many/few bits");

	if (dpin < 0 || dpin > NBRPINS || cpin < 0 || cpin > NBRPINS) error("Invalid pin number");
	if (ExtCurrentConfig[dpin] >= EXT_COM_RESERVED || ExtCurrentConfig[cpin] >= EXT_COM_RESERVED)  error("Pin is allocated to a communications function");

// At this point we should have a valid number of arguments and some possibly valid IO pins.

	cnt = (nbits - 1)  / 8; // Tells us how many complete bytes to loop over.
	cnt++; // at least one byte even if it is a partial byte.

	ExtSet(cpin,0); // CLK pin low
	uDly(BDLY);

	if (mode == 0) { // shift bits out LSB first starting at the END of the string.

		for (ix=0; ix<nbits; ix++) { // loop through all bits
			iy = ix / 8; // byte offset

			mybyte = outstr[outstr[0]-iy]; // current byte being output

			if (mybyte & (1 << (ix & 0x07))) {
				ExtSet(dpin,1);  // output a 1
			} else {
				ExtSet(dpin,0);
			}

			uDly(BDLY);
			ExtSet(cpin,1); // CLK high
			uDly(BDLY);
			ExtSet(cpin,0); // CLK low
			uDly(BDLY);

		} // next bit

	} else { // shift bits out MSB first starting at the BEGINNING of the string.

		for (ix=0; ix<nbits; ix++) { // loop through all bits
			iy = (ix / 8) + 1; // byte offset

			mybyte = outstr[iy]; // current byte being output

			if (mybyte & (128 >> (ix & 0x07))) {
				ExtSet(dpin,1);  // output a 1
			} else {
				ExtSet(dpin,0);
			}

			uDly(BDLY);
			ExtSet(cpin,1); // CLK high
			uDly(BDLY);
			ExtSet(cpin,0); // CLK low
			uDly(BDLY);

		} // next bit
	}

}

/*******************************************
SHIFTIN Dpin, Cpin, Mode, String$, Bits
---- If the user has passed us a string, which they are expected to do, then we know we have 255 bytes
---- available to shiftin data because a string is always 255 bytes long.
*******************************************/

#define MSBPRE 0	// MSB first sample before clock pulse
#define LSBPRE 1	// LSB first sample before clock pulse
#define MSBPOST 2	// MSB first sample after clock pulse
#define LSBPOST 3	// LSB first sample after clock pulse

void cmd_shiftin(void) {
	int dpin, cpin, mode, nbits;
	int ix, iy; // loop counter
	int cnt; // number of bytes to input.
	int input; // temp return value from ExtInp()
	unsigned char mybyte = 0;

	char * inpstr;

	const int BDLY = 3; // Determines clocking rate. # of CoreTimer cycles.

	getargs(&cmdline, 9, ","); // Get the arguments, we should have five, comma seperated.

	if (argc < 9) error("Invalid syntax"); // Test number of arguments given.

	dpin = getinteger(argv[0]); // Data pin
	cpin = getinteger(argv[2]); // Clock pin
	mode = getinteger(argv[4]); // Mode
	inpstr = getstring(argv[6]); // String to clock in,treated as an array of bytes.
	nbits = getinteger(argv[8]); // Number of bits to clock in

	if (nbits < 1 || nbits > (255 * 8)) error("Too many/few bits");
	if (mode < MSBPRE || mode > LSBPOST) error("Invalid mode");

	if (dpin < 0 || dpin > NBRPINS || cpin < 0 || cpin > NBRPINS) error("Invalid pin number");
	if (ExtCurrentConfig[dpin] >= EXT_COM_RESERVED || ExtCurrentConfig[cpin] >= EXT_COM_RESERVED)  error("Pin is allocated to a communications function");

// At this point we should have a valid number of arguments and some possibly valid IO pins.

	// Figure out how many bytes because this gives us our string length for inpstr[0].

	cnt = (nbits - 1)  / 8; // Tells us how many complete bytes to loop over.
	cnt++; // at least one byte even if it is a partial byte.

	memset(inpstr, 0, cnt+1); // zero our string.
	inpstr[0] = cnt; // set the mmbasic string length.

	ExtSet(cpin,0); // CLK pin low

// Actual clocking in of bits to our destination string.

	if (mode == LSBPRE || mode == LSBPOST) { // shift bits in LSB first starting at the END of the string.

		for (ix=0; ix<nbits; ix++) { // loop through all bits
			iy = ix / 8; // byte offset

			if (mode == LSBPRE) { // clock before getting bit
				uDly(BDLY);
				ExtSet(cpin,1); // CLK high
				uDly(BDLY);
				ExtSet(cpin,0); // CLK low
				uDly(BDLY);
			}

			ExtInp(dpin, &input);  // Test the datapin.

			if (input) {  // Pin was set.
				mybyte = (1 << (ix & 0x07));  // shift a one into correct position for current byte.
			}

			inpstr[cnt-iy] |= mybyte; // Save the current byte/bit in our string.

			if (mode == LSBPOST) { // clock after getting bit
				uDly(BDLY);
				ExtSet(cpin,1); // CLK high
				uDly(BDLY);
				ExtSet(cpin,0); // CLK low
				uDly(BDLY);
			}

		} // next bit

	} else { // shift bits in MSB first starting at the BEGINNING of the string.

		for (ix=0; ix<nbits; ix++) { // loop through all bits
			iy = (ix / 8); // byte offset

			if (mode == MSBPRE) { // clock before getting bit
				uDly(BDLY);
				ExtSet(cpin,1); // CLK high
				uDly(BDLY);
				ExtSet(cpin,0); // CLK low
				uDly(BDLY);
			}

			ExtInp(dpin, &input);  // Test the datapin.

			if (input) {  // Pin was set.
				mybyte = (128 >> (ix & 0x07));  // shift a one into correct position for current byte.
			}

			inpstr[iy+1] |= mybyte; // Save the current byte/bit in our string.

			if (mode == MSBPOST) { // clock after getting bit
				uDly(BDLY);
				ExtSet(cpin,1); // CLK high
				uDly(BDLY);
				ExtSet(cpin,0); // CLK low
				uDly(BDLY);
			}

		} // next bit

	}

}

/////////////////////////////////////////////
// Returns lower 16 bits of video buffer.
/////////////////////////////////////////////
void fun_vmem(void) {
	int vmem = (int)VA;

	vmem &= 0x0000ffff;
	fret = (float)vmem;
}

// Ingmar 4.2.2012
// utility routine used to read a number of bytes
// only used by cmd_loadbmp() below
void xread(char *p, int nbr, int fnbr) {
    while(nbr--) *p++ = MMfgetc(fnbr);
}

// Ingmar 4.2.2012
// loadbmp "filename" optional ,x,y
// original work, based on specs from en.wikipedia.org
// only works for monochrome bmp

void cmd_loadbmp(void) {
    // BMP header variables
    unsigned short magic;
    unsigned int filesz;
    unsigned int bmp_offset;
    int width;
    int height;
    unsigned short colplanes;
    unsigned short bpp;

    // general work variables
    int x,y,ox,oy,m,i;
    char *p;
    int fp;
    unsigned char buffer[64];
    unsigned char pixels;
    unsigned char bpl; // bytes per line = width / 8

    // LOADBMP "name" with optional ,x,y and ,mode
    // x,y top left corner
    // mode bit 0,1 = 0-3 scale factor
    //      bit 7 = 0 normal, 1 invert

    getargs(&cmdline, 9, ","); // Get the arguments, we should have one three or four, comma seperated.

    if ((argc % 2)==0 || argc > 7) error("Invalid syntax"); // Test number of arguments given.

    p = getCstring(argv[0]);

    ox = oy = m = 0;

    if (argc >= 5) {
        ox = getinteger(argv[2]);
        oy = getinteger(argv[4]);
    }

    if (argc == 7 )
        m = getinteger(argv[6]);

    if(strchr(p, '.') == NULL) strcat(p, ".BMP");

    if((fp = FindFreeFileNbr()) == 0) error("Too many files open");

    MMfopen(p, "r", fp);

    // file opened okay and fp is our file handle.
    // read the BMP header up to the bits-per-pixel field.
    xread(buffer,32,fp);

    // this is a kludge to get around annoying 4 byte aligned structures.
    // and i'm too lazy to do it another way.

    magic = buffer[0] + (buffer[1]<<8);
    filesz = buffer[2] + (buffer[3]<<8) + (buffer[4]<<16) + (buffer[5]<<24);
    bmp_offset = buffer[10] + (buffer[11]<<8) + (buffer[12]<<16) + (buffer[13]<<24);
    width = buffer[18] + (buffer[19]<<8) + (buffer[20]<<16) + (buffer[21]<<24);
    height = buffer[22] + (buffer[23]<<8) + (buffer[24]<<16) + (buffer[25]<<24);
    colplanes = buffer[26] + (buffer[27]<<8);
    bpp = buffer[28] + (buffer[29]<<8);

    if (magic != 0x4d42) {
        error("File is not a BMP");
    }

    if (bpp != 1 || colplanes !=1) {
        error("BMP not compatible");
    }

    if (width > 480 || height > 432) {
	error("BMP too big");
    }

    // Skip to the start of the image data.
    i = bmp_offset - 32; // number of bytes to skip.
    while(i--) MMfgetc(fp);

    // figure out how many bytes per line so we don't read byte by byte.
    // multiples of 4 bytes (32 bits)
    bpl = (width >> 5) << 2;  // x / 32 * 4;

    if (width % 32)
        bpl += 4;

    // The x,y origin is allowed for in these routines.
    for (y=height-1; y>0; y--) {
        xread(buffer,bpl,fp); // read in a whole line

	for (x=0; x<width-1; x+=8) {
            pixels = buffer[x/8]; // read 8 bits of image

            for (i=0; i<8; i++) {
                if (x+i < width) {
                    if (m & 0x80)
                        plot(x+i+ox,y+oy,(pixels & (0x80>>i))?1:0);
                    else
                        plot(x+i+ox,y+oy,(pixels & (0x80>>i))?0:1);
                }
            }
	}
    }

    MMfclose(fp);
}

