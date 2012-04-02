/************************************************************************************************************************
Maximite

I2C.c

Routines to handle I2C access.

Copyright 2011 Gerard Sexton
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/
//	GS I2C

#include <p32xxxx.h>
#include <plib.h>

#include "../MMBasic/MMBasic.h"
#include "../Maximite.h"
#include "../IOPorts.h"

#define INCLUDE_FUNCTION_DEFINES
#include "I2C.h"
#include "External.h"
#include "Misc.h"

volatile unsigned int I2C_State;						// the state of the master state machine
volatile unsigned int I2C_Status;						// status flags
unsigned int I2C_Timer;								// master timeout counter
char *I2C_IntLine;								// pointer to the master interrupt line number
char *I2C_Slave_Send_IntLine;							// pointer to the slave send interrupt line number
char *I2C_Slave_Receive_IntLine;						// pointer to the slave receive interrupt line number

static float *I2C_Rcvbuf;							// pointer to the master receive buffer
static unsigned int I2C_Addr;							// I2C device address
static unsigned int I2C_Prev_Addr = 0xffff;					// previous I2C device address
static unsigned int I2C_Timeout;						// master timeout value
static unsigned int I2C_Slave_Addr;						// slave address
static unsigned int I2C_Slave_Mask;						// slave address mask
static volatile unsigned int I2C_Sendlen;					// length of the master send buffer
static volatile unsigned int I2C_Rcvlen;					// length of the master receive buffer
static volatile unsigned int I2C_Slave_Sendlen;                                 // length of the slave send buffer
static volatile unsigned int I2C_Send_Index;					// current index into I2C_Send_Buffer
static volatile unsigned int I2C_Rcv_Head;					// Receive buffer head
static volatile unsigned int I2C_Rcv_Tail;					// Receive buffer tail
static char I2C_Send_Buffer[256];						// I2C send buffer
static char I2C_Rcv_Buffer[256];						// I2C receive buffer

// defines for MM.I2C
#define I2C_MMI2C_NoAck					1
#define I2C_MMI2C_Timeout				2
#define I2C_MMI2C_GeneralCall		4
static int mmI2Cvalue;								// value of MM.I2C



/*******************************************************************************************
							  I2C related commands in MMBasic
                              ===============================
These are the functions responsible for executing the I2C related commands in MMBasic
They are supported by utility functions that are grouped at the end of this file

********************************************************************************************/




// disable the I2C1 module - master mode
void cmd_i2cDisable(void) {
	checkend(cmdline);
	i2c_disable();
}


// enable the I2C1 module - master mode
void cmd_i2cEnable(void) {
	int speed, timeout, line;
	getargs(&cmdline, 5, ",");
	if (argc != 3 && argc != 5) error("Invalid syntax");
	speed = getinteger(argv[0]);
	if (speed < 10 || speed > 400) error("Invalid I2C baud rate (must be 10 - 400)");
	timeout = getinteger(argv[2]);
	if (timeout < 0 || (timeout > 0 && timeout < 100)) error("Invalid I2C timeout value (must be 0 or greater than 100)" );
	I2C_Timeout = timeout;
	if (I2C_Status & I2C_Status_InProgress) error("Previous master I2C request has not completed");
	if (argc == 5) {
		line = getinteger(argv[4]);									// get the line number
		I2C_IntLine = findline(line, true);							// and save
		I2C_Status |= I2C_Status_Interrupt;							// and set interrupt flag
	} else {
		I2C_Status &= ~I2C_Status_Interrupt;						// else clear interrupt flag
		I2C_IntLine = NULL;											// and set line to null
	}
	i2c_enable(speed);
}


// send data to an I2C slave - master mode
void cmd_i2cSend(void) {
	int addr, i2c_options, sendlen, i;
	void *ptr = NULL;
	unsigned char *cptr = NULL;
	
	getargs(&cmdline, 99, ",");
	if (!(argc & 0x01) || (argc < 7)) error("Invalid syntax");
	if (!(I2C_Status & I2C_Status_Master)) error("Master I2C has not been enabled");
	if (I2C_Status & I2C_Status_InProgress) error("Previous master I2C request has not completed");
	addr = getinteger(argv[0]);
	i2c_options = getinteger(argv[2]);
	if (i2c_options < 0 || i2c_options > 3) error("Option must be between 0 and 3");
	if (i2c_options & 0x01) I2C_Status |= I2C_Status_BusHold;
	else I2C_Status &= ~I2C_Status_BusHold;
	if (i2c_options & 0x02) {
		I2C_Status |= I2C_Status_10BitAddr;
		if (addr < 0x0000 || addr > 0x03ff) error("Invalid 10 bit device address");
	} else {
		I2C_Status &= ~I2C_Status_10BitAddr;
		if (addr < 0x00 || (addr > 0x00 && addr < 0x08) || addr > 0x77) error("Invalid 7 bit device address");
	}
	if (I2C_Status & I2C_Status_Slave && ((addr | I2C_Slave_Mask) == (I2C_Slave_Addr | I2C_Slave_Mask)))
		error("I2C device address matches local slave address");
	I2C_Addr = addr;
	sendlen = getinteger(argv[4]);
	if (sendlen < 1 || sendlen > 255) error("Invalid send length");

	if (sendlen == 1 || argc > 7) {		// numeric expressions for data
		if (sendlen != ((argc - 5) >> 1)) error("Insufficient arguments for send length");
		for (i = 0; i < sendlen; i++) {
			I2C_Send_Buffer[i] = getinteger(argv[i + i + 6]);
		}
	} else {		// an array of float or a string
		ptr = findvar(argv[6], V_NOFIND_NULL);
		if (ptr == NULL) error("Variable/argument not found");
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			cptr++;																	// skip the length byte in a MMBasic string
			for (i = 0; i < sendlen; i++) {
				I2C_Send_Buffer[i] = (int)(*(cptr + i));
			}
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + sendlen) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				for (i = 0; i < sendlen; i++) {
					I2C_Send_Buffer[i] = (int)(*((float *)ptr + i));
				}
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}
	I2C_Sendlen = sendlen;
	I2C_Rcvlen = 0;

	i2c_masterCommand(1);
	if (!(I2C_Status & I2C_Status_Interrupt)) {
		while (!(I2C_Status & I2C_Status_Completed)) {}
		I2C_Status &= ~I2C_Status_Completed;
	}
}


// receive data from an I2C slave (and optionally send a command first) - master mode
void cmd_i2cReceive(void) {
	int addr, i2c_options, rcvlen, sendlen, i;
	void *ptr = NULL;
	unsigned char *cptr = NULL;
	
	getargs(&cmdline, 99, ",");
	if (!(argc & 0x01) || (argc < 7) || (argc == 9)) error("Invalid syntax");
	if (!(I2C_Status & I2C_Status_Master)) error("Master I2C has not been enabled");
	if (I2C_Status & I2C_Status_InProgress) error("Previous master I2C request has not completed");
	addr = getinteger(argv[0]);
	i2c_options = getinteger(argv[2]);
	if (i2c_options < 0 || i2c_options > 3) error("Option must be between 0 and 3");
	if (i2c_options & 0x01) I2C_Status |= I2C_Status_BusHold;
	else I2C_Status &= ~I2C_Status_BusHold;
	if (i2c_options & 0x02) {
		I2C_Status |= I2C_Status_10BitAddr;
		if (addr < 0x0000 || addr > 0x03ff) error("Invalid 10 bit device address");
	} else {
		I2C_Status &= ~I2C_Status_10BitAddr;
		if (addr < 0x01 || (addr > 0x00 && addr < 0x08) || addr > 0x77) error("Invalid 7 bit device address");
	}
	if (I2C_Status & I2C_Status_Slave && ((addr | I2C_Slave_Mask) == (I2C_Slave_Addr | I2C_Slave_Mask)))
		error("I2C device address matches local slave address");
	I2C_Addr = addr;
	rcvlen = getinteger(argv[4]);
	if (rcvlen < 1 || rcvlen > 255) error("Invalid receive length");

	ptr = findvar(argv[6], V_NOFIND_NULL);
	if (ptr == NULL) error("Receive variable not found");
	if (vartbl[VarIndex].type != T_NBR) error("Receive variable not numeric");
	if (vartbl[VarIndex].dims[1] != 0) error("Array must be one dimensional");
	if (vartbl[VarIndex].dims[0] == 0) {		// Not an array
		if (rcvlen != 1) error("Receive length not 1 and variable not an array");
	} else {		// An array
		if ( (((float *)ptr - vartbl[VarIndex].val.fa) + rcvlen) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) )
			error("Insufficient space in array");
	}
	I2C_Rcvbuf = (float*)ptr;
	I2C_Rcvlen = rcvlen;

	if (argc >= 11) {
		sendlen = getinteger(argv[8]);
		if (sendlen < 1 || sendlen > 255) error("Invalid I2C send length");
		if (sendlen == 1 || argc > 11) {		// numeric expressions for data
			if (sendlen != ((argc - 9) >> 1)) error("Insufficient arguments for send length");
			for (i = 0; i < sendlen; i++) {
				I2C_Send_Buffer[i] = getinteger(argv[i + i + 10]);
			}
		} else {		// an array of float or a string
			ptr = findvar(argv[10], V_NOFIND_NULL);
			if (ptr == NULL) error("Variable not found");
			if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
				cptr = (unsigned char *)ptr;
				cptr++;																	// skip the length byte in a MMBasic string
				for (i = 0; i < sendlen; i++) {
					I2C_Send_Buffer[i] = (int)(*(cptr + i));
				}
			} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
				if ( (((float *)ptr - vartbl[VarIndex].val.fa) + sendlen) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
					error("Insufficient data in array");
				} else {
					for (i = 0; i < sendlen; i++) {
						I2C_Send_Buffer[i] = (int)(*((float *)ptr + i));
					}
				}
			} else error("Variable not a one dimensional numeric array or a string");
		}
		I2C_Sendlen = sendlen;
	} else {
		I2C_Sendlen = 0;
	}

	i2c_masterCommand(1);
	if (!(I2C_Status & I2C_Status_Interrupt)) {
		while (!(I2C_Status & I2C_Status_Completed)) {}
		I2C_Status &= ~I2C_Status_Completed;
	}
}


// disable the I2C1 module - slave mode
void cmd_i2cSlaveDisable(void) {
	checkend(cmdline);
	i2c_slave_disable();
}


// enable the I2C1 module - slave mode
void cmd_i2cSlaveEnable(void) {
	int options, addr, mask, line;
	getargs(&cmdline, 9, ",");
	if (argc != 9) error("Invalid syntax");
	addr = getinteger(argv[0]);
	mask = getinteger(argv[2]);
	options = getinteger(argv[4]);
	if (options < 0 || options > 3) error("Invalid options - must be 0, 1, 2 or 3");
	if (options & 0x02) {	// 10 bit addressing
		if (addr < 0x0000 || addr > 0x03ff) error("Invalid 10 bit device address");
		if (mask < 0x0000 || mask > 0x03ff) error("Invalid 10 bit address mask");
	} else {	// 7 bit addressing
		if (addr < 0x00 || (addr > 0x00 && addr < 0x08) || addr > 0x77 || (addr == 0x00 && !(options & 0x01)))
			error("Invalid 7 bit device address");
		if (mask < 0x00 || mask > 0x77) error("Invalid 7 bit address mask");
	}
	if (I2C_Status & I2C_Status_Slave) error("Must disable slave mode first");
	I2C_Slave_Addr = addr;
	I2C_Slave_Mask = mask;
	line = getinteger(argv[6]);										// get the send interrupt line number
	I2C_Slave_Send_IntLine = findline(line, true);					// and save
	line = getinteger(argv[8]);										// get the Receive interrupt line number
	I2C_Slave_Receive_IntLine = findline(line, true);				// and save
	i2c_slave_enable(options);
}


// send data to the I2C master - slave mode
void cmd_i2cSlaveSend(void) {
	int sendlen, i;
	void *ptr;
	unsigned char *cptr = NULL;

	getargs(&cmdline, 99, ",");
	if (!(argc & 0x01) || argc < 3) error("Invalid syntax");
	if (!(I2C_Status & I2C_Status_Slave)) error("Slave mode has not been enabled");
	if (!(I2C_Status & I2C_Status_Slave_Send)) error("Not in slave send state");
	sendlen = getinteger(argv[0]);
	if (sendlen < 1 || sendlen > 255) error("Invalid send length");
	if (sendlen == 1 || argc > 3) {		// numeric expressions for data
		if (sendlen != ((argc - 1) >> 1)) error("Insufficient arguments for send length");
		for (i = 0; i < sendlen; i++) {
			I2C_Send_Buffer[i] = getinteger(argv[i + i + 2]);
		}
	} else {		// an array of float or a string
		ptr = findvar(argv[2], V_NOFIND_NULL);
		if (ptr == NULL) error("Variable not found");
		if (vartbl[VarIndex].type == T_STR && vartbl[VarIndex].dims[0] == 0) {		// string
			cptr = (unsigned char *)ptr;
			cptr++;																	// skip the length byte in a MMBasic string
			for (i = 0; i < sendlen; i++) {
				I2C_Send_Buffer[i] = (int)(*(cptr + i));
			}
		} else if (vartbl[VarIndex].type == T_NBR && vartbl[VarIndex].dims[0] != 0 && vartbl[VarIndex].dims[1] == 0) {		// numeric array
			if ( (((float *)ptr - vartbl[VarIndex].val.fa) + sendlen) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) ) {
				error("Insufficient data in array");
			} else {
				for (i = 0; i < sendlen; i++) {
					I2C_Send_Buffer[i] = (int)(*((float *)ptr + i));
				}
			}
		} else error("Variable not a one dimensional numeric array or a string");
	}
	i2c_slave_send(sendlen);
}


// receive data from the I2C master - slave mode
void cmd_i2cSlaveReceive(void) {
	int rcvlen;
	void *dataptr, *rcvdlen;
	getargs(&cmdline, 5, ",");
	if (argc != 5) error("Invalid syntax");
	if (!(I2C_Status & I2C_Status_Slave)) error("Slave mode has not been enabled");
	if (!(I2C_Status & I2C_Status_Slave_Receive)) error("Not in slave read state");
	rcvlen = getinteger(argv[0]);
	if (rcvlen < 1 || rcvlen > 255) error("Invalid receive length");
	dataptr = findvar(argv[2], V_NOFIND_NULL);
	if (dataptr == NULL) error("Receive variable not found");
	if (vartbl[VarIndex].type != T_NBR) error("Receive variable not numeric");
	if (vartbl[VarIndex].dims[1] != 0) error("Array must be one dimensional");
	if (vartbl[VarIndex].dims[0] == 0) {							// Not an array
		if (rcvlen != 1) error("Receive length not 1 and variable not an array");
	} else {		// An array
		if ( (((float *)dataptr - vartbl[VarIndex].val.fa) + rcvlen) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) )
			error("Insufficient space in array");
	}
	rcvdlen = findvar(argv[4], V_FIND);
	if (rcvdlen == NULL) error("Received length variable not found");
	if (vartbl[VarIndex].type != T_NBR) error("Receive variable not numeric");
	i2c_slave_receive(rcvlen, dataptr, rcvdlen);
}


// convert float to 4 numeric values
void cmd_num2byte(void) {
	float f, *fp;
	int i;
	unsigned char *cp;
	void *ptr;
	getargs(&cmdline, 9, ",");
	if (argc != 3 && argc != 9) error("Invalid syntax");
	cp = (unsigned char *)&f;
	f = getnumber(argv[0]);
	if (argc == 3) {
		ptr = findvar(argv[2], V_NOFIND_NULL);
		if (ptr == NULL) error("Array argument not found");
		if (vartbl[VarIndex].type != T_NBR) error("Argument not numeric");
		if (vartbl[VarIndex].dims[1] != 0) error("Array argument must be one dimensional");
		if (vartbl[VarIndex].dims[0] == 0) error("Argument not an array");	// Not an array
		if ( (((float *)ptr - vartbl[VarIndex].val.fa) + 4) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) )
			error("Insufficient arguments in array");
		fp = (float *)ptr;
		for (i = 0; i < 4; i++) {
			fp[i] = (float)cp[i];
		}
	} else {
		for (i = 0; i < 4; i++) {
			ptr = findvar(argv[(i << 1) + 2], V_FIND);
			if (ptr == NULL) error("Argument not found");
			if (vartbl[VarIndex].type != T_NBR) error("Argument not numeric");
			fp = (float *)ptr;
			*fp = (float)cp[i];
		}
	}
}


// convert 4 numeric values to a float
void fun_byte2num(void){
	float f, *fp;
	int i, x;
	unsigned char *cp;
	void *ptr;
	getargs(&ep, 7, ",");											// this is a macro and must be the first executable stmt in a block
	if(argc != 1 && argc != 7) error("Invalid Syntax");
	cp = (unsigned char *)&f;
	if (argc == 1) {
		ptr = findvar(argv[0], V_NOFIND_NULL);
		if (ptr == NULL) error("Argument not found");
		if (vartbl[VarIndex].type != T_NBR) error("Argument not numeric");
		if (vartbl[VarIndex].dims[1] != 0) error("Array must be one dimensional");
		if (vartbl[VarIndex].dims[0] == 0) error("Argument not an array");	// Not an array
		if ( (((float *)ptr - vartbl[VarIndex].val.fa) + 4) > (vartbl[VarIndex].dims[0] + 1 - OptionBase) )
			error("Insufficient arguments in array");
		fp = (float *)ptr;
		for (i = 0; i < 4; i++) {
			x = (int)fp[i];
			if (x < 0 || x > 255) error("Argument values must be between 0 and 255");
			cp[i] = (unsigned char)x;
		}
	} else {
		for (i = 0; i < 4; i++) {
			x = getinteger(argv[i << 1]);
			if (x < 0 || x > 255) error("Argument values must be between 0 and 255");
			cp[i] = (unsigned char)x;
		}
	}
	fret = f;
}


void fun_mmi2c(void) {
	fret = (float)mmI2Cvalue;
}



/**************************************************************************************************
I2C 1 interrupt.
Used to process the I2C requests
***************************************************************************************************/
void __ISR(_I2C_1_VECTOR, ipl5) I2C1Interrupt(void) {
	unsigned int i;

	if (mI2C1MGetIntFlag() && (I2C_Status & I2C_Status_Timeout)) {	// Timeout interrupt
		I2C_Status &= ~(I2C_Status_Timeout | I2C_Status_NoAck | I2C_Status_InProgress | I2C_Status_BusOwned |  I2C_Status_BusHold |
										I2C_Status_10BitAddr | I2C_Status_Send | I2C_Status_Receive | I2C_Status_MasterCmd);
		if (!(I2C_Status & I2C_Status_Disable)) {
			I2C1CONCLR = _I2C1CON_I2CEN_MASK; 						// disable I2C1 module
			I2C1STATCLR = _I2C1STAT_BCL_MASK;						// clear BCL flag
			I2C_State = I2C_State_Idle;
			I2C_Send_Index = 0;
			I2C_Prev_Addr = 0xffff;
			mmI2Cvalue = I2C_MMI2C_Timeout;
			I2C_Status |= I2C_Status_Completed;
			I2C1CONSET = _I2C1CON_I2CEN_MASK; 						// enable I2C1 module
			mI2C1BClearIntFlag();    								// clear the interrupt flag
			mI2C1MClearIntFlag();    								// clear the interrupt flag
		}
	}

	if (mI2C1BGetIntFlag()) {										// Bus collision detected
		I2C1STATCLR = _I2C1STAT_BCL_MASK;							// clear BCL flag
		I2C_Status &= ~(I2C_Status_InProgress | I2C_Status_BusOwned | I2C_Status_Send | I2C_Status_Receive);
		I2C_State = I2C_State_Idle;
		I2C_Prev_Addr = 0xffff;
		if (I2C_Status & I2C_Status_Disable) {
			I2C_Status |= I2C_Status_Completed;
		} else {
			i2c_masterCommand(0);
		}
		mI2C1BClearIntFlag();    									// clear the interrupt flag
	}

	if (mI2C1MGetIntFlag()) {										// Master interrupt
		if (I2C1STAT & _I2C1STAT_P_MASK) {
			I2C_State = I2C_State_Stop;
		}
		switch (I2C_State) {    									// master state machine
			case I2C_State_Start:  		// 1
				if (I2C_Status & I2C_Status_Send) {					// send to I2C device
					I2C_Status |= I2C_Status_BusOwned;
					if (I2C_Status & I2C_Status_10BitAddr) {		 // 10 bit addressing
						I2C_State = I2C_State_10Bit;
						I2C1TRN = ((I2C_10BitAddr_Mask + (I2C_Addr >> 8)) << 1);
					} else {	// 7 bit addressing
						I2C_Prev_Addr = 0xffff;
						I2C_State = I2C_State_Send;
						I2C1TRN = (I2C_Addr << 1);
					}
				} else {	// receive from I2C device
					if (I2C_Status & I2C_Status_10BitAddr) { 		// 10 bit addressing
						if (I2C_Status & I2C_Status_BusOwned && I2C_Addr == I2C_Prev_Addr) {
							I2C_State = I2C_State_RcvAddr;
							I2C1TRN = ((I2C_10BitAddr_Mask + (I2C_Addr >> 8)) << 1) + 1;
						} else {
							I2C_Status |= I2C_Status_BusOwned;
							I2C_State = I2C_State_10Bit;
							I2C1TRN = ((I2C_10BitAddr_Mask + (I2C_Addr >> 8)) << 1);
						}
					} else {	// 7 bit addressing
						I2C_Status |= I2C_Status_BusOwned;
						I2C_State = I2C_State_RcvAddr;
						I2C1TRN = (I2C_Addr << 1) + 1;
					}
				}
				break;

			case I2C_State_10Bit:   // 2
				if (I2C1STAT & _I2C1STAT_ACKSTAT_MASK) {
					I2C_Status |= I2C_Status_NoAck;
					I2C_State = I2C_State_Stop;
					I2C1CONSET = _I2C1CON_PEN_MASK;
				} else {
					I2C_Prev_Addr = I2C_Addr;
					if (I2C_Status & I2C_Status_Send) {
						I2C_State = I2C_State_Send;
						I2C1TRN = (I2C_Addr & 0xff);
					} else {
						I2C_State = I2C_State_10BitRcv;
						I2C1TRN = (I2C_Addr & 0xff);
					}
				}
				break;

			case I2C_State_10BitRcv:   // 3
				if (I2C1STAT & _I2C1STAT_ACKSTAT_MASK) {
					I2C_Status |= I2C_Status_NoAck;
					I2C_State = I2C_State_Stop;
					I2C1CONSET = _I2C1CON_PEN_MASK;
				} else {
					I2C_State = I2C_State_Start;
					I2C1CONSET = _I2C1CON_RSEN_MASK;
				}
				break;

			case I2C_State_RcvAddr:   // 4
				if (I2C1STAT & _I2C1STAT_ACKSTAT_MASK) {
					I2C_Status |= I2C_Status_NoAck;
					I2C_State = I2C_State_Stop;
					I2C1CONSET = _I2C1CON_PEN_MASK;
				} else {
					I2C_State = I2C_State_Receive;
					I2C1CONSET = _I2C1CON_RCEN_MASK;
				}
				break;

			case I2C_State_Send:   		// 5
				if (I2C1STAT & _I2C1STAT_ACKSTAT_MASK) {
					I2C_Status |= I2C_Status_NoAck;
					I2C_State = I2C_State_Stop;
					I2C1CONSET = _I2C1CON_PEN_MASK;
				} else {
					if (I2C_Send_Index < I2C_Sendlen) {
						I2C1TRN = I2C_Send_Buffer[I2C_Send_Index++];
					} else {
						I2C_Status &= ~I2C_Status_Send;
						I2C_Send_Index = 0;
						if (I2C_Status & I2C_Status_Receive) {
							I2C_State = I2C_State_Start;
							I2C1CONSET = _I2C1CON_RSEN_MASK;
						} else {
							if (I2C_Status & I2C_Status_BusHold) {
								I2C_State = I2C_State_Idle;
								I2C_Timer = 0;
								I2C_Status &= ~(I2C_Status_NoAck | I2C_Status_Timeout | I2C_Status_InProgress | I2C_Status_Send | I2C_Status_Receive);
								I2C_Status |= I2C_Status_Completed;
							} else {
								I2C_State = I2C_State_Stop;
								I2C1CONSET = _I2C1CON_PEN_MASK;
							}
						}
					}
				}
				break;

			case I2C_State_Receive:   // 6
				I2C_Rcvbuf[I2C_Send_Index++] = I2C1RCV;
				if (I2C_Send_Index < I2C_Rcvlen) {
					I2C1CONCLR = _I2C1CON_ACKDT_MASK;
				} else {
					I2C1CONSET = _I2C1CON_ACKDT_MASK;
				}
				I2C_State = I2C_State_Ack;
				I2C1CONSET = _I2C1CON_ACKEN_MASK;
				break;

			case I2C_State_Ack:   		// 7
				if (I2C_Send_Index < I2C_Rcvlen) {
					I2C_State = I2C_State_Receive;
					I2C1CONSET = _I2C1CON_RCEN_MASK;
				} else {
					if (I2C_Status & I2C_Status_BusHold) {
						I2C_State = I2C_State_Idle;
						I2C_Timer = 0;
						I2C_Status &= ~(I2C_Status_NoAck | I2C_Status_Timeout | I2C_Status_InProgress | I2C_Status_Send | I2C_Status_Receive);
						I2C_Status |= I2C_Status_Completed;
					} else {
						I2C_State = I2C_State_Stop;
						I2C1CONSET = _I2C1CON_PEN_MASK;
					}
				}
				break;

			case I2C_State_Stop:   		// 8
				I2C_Timer = 0;
				I2C_Send_Index = 0;
				I2C_Slave_Sendlen = 0;
				I2C_Rcv_Tail = I2C_Rcv_Head;
				I2C_State = I2C_State_Idle;
				if (I2C_Status & I2C_Status_NoAck) {
					mmI2Cvalue = I2C_MMI2C_NoAck;
				}
				I2C_Status &= ~(I2C_Status_NoAck | I2C_Status_Timeout | I2C_Status_BusOwned | I2C_Status_InProgress |
												I2C_Status_10BitAddr | I2C_Status_Send | I2C_Status_Receive | I2C_Status_Slave_Send |
												I2C_Status_Slave_Receive | I2C_Status_Slave_Send_Rdy | I2C_Status_Slave_Receive_Rdy | I2C_Status_Slave_Receive_Full);
				I2C_Prev_Addr = 0xffff;
				I2C1CONSET = _I2C1CON_SCLREL_MASK;
				I2C_Status |= I2C_Status_Completed;
				break;
    }
		mI2C1MClearIntFlag();    											// clear the interrupt flag
	}

	if (mI2C1SGetIntFlag()) {		// Slave interrupt
		if (!(I2C1STAT & _I2C1STAT_S_MASK) || I2C1STAT & _I2C1STAT_P_MASK) {
				i = I2C1RCV;
				I2C_Status &= ~(I2C_Status_Slave_Send | I2C_Status_Slave_Receive | I2C_Status_Slave_Send_Rdy |
												I2C_Status_Slave_Receive_Rdy | I2C_Status_Slave_Receive_Full);
				I2C_Slave_Sendlen = 0;
				I2C_Rcv_Tail = I2C_Rcv_Head;
				I2C1CONSET = _I2C1CON_SCLREL_MASK;
		} else {
			if (I2C1STAT & _I2C1STAT_R_W_MASK) {	// send to master
				if (!(I2C1STAT & _I2C1STAT_D_A_MASK)) {	// address processing
					I2C_Status |= I2C_Status_Slave_Send;
					I2C_Slave_Sendlen = 0;
					I2C_Send_Index = 0;
					if (I2C1STAT & _I2C1STAT_RBF_MASK) {
						i = I2C1RCV;
					}
				}
				if (!(I2C1STAT & _I2C1STAT_ACKSTAT_MASK)) {
					if (I2C_Slave_Sendlen > 0) {	// write from buffer
						I2C1TRN = I2C_Send_Buffer[I2C_Send_Index++];
						I2C_Status &= ~I2C_Status_Slave_Receive;
						I2C_Slave_Sendlen--;
						I2C1CONSET = _I2C1CON_SCLREL_MASK;
					} else {	// raise MMBasic slave send interrupt
						I2C_Status |= I2C_Status_Slave_Send_Rdy;
					}
				} else {
					I2C_Status &= ~(I2C_Status_Slave_Send | I2C_Status_Slave_Receive | I2C_Status_Slave_Send_Rdy |
													I2C_Status_Slave_Receive_Rdy | I2C_Status_Slave_Receive_Full);
					I2C_Slave_Sendlen = 0;
					I2C_Rcv_Tail = I2C_Rcv_Head;
					I2C1CONSET = _I2C1CON_SCLREL_MASK;
				}
			} else {	// receive from master
				if (I2C1STAT & _I2C1STAT_D_A_MASK) {	// data processing
					i = (I2C_Rcv_Head + 1) & 0x000000ff;
					if (i != I2C_Rcv_Tail) {	// room available in receive buffer
						I2C_Rcv_Buffer[I2C_Rcv_Head] = I2C1RCV;
						I2C_Rcv_Head = i;
						I2C1CONSET = _I2C1CON_SCLREL_MASK;
					} else {	// no room in receive buffer
						I2C_Status |= I2C_Status_Slave_Receive_Full;
					}
					I2C_Status &= ~I2C_Status_Slave_Send;
					I2C_Status |= I2C_Status_Slave_Receive | I2C_Status_Slave_Receive_Rdy;
				} else {	// address processing
					I2C_Slave_Sendlen = 0;
					if (I2C1STAT & _I2C1STAT_GCSTAT_MASK) {	// general call
						mmI2Cvalue = I2C_MMI2C_GeneralCall;
					} else {
						mmI2Cvalue = 0;
					}
					if (I2C1STAT & _I2C1STAT_RBF_MASK) {
						i = I2C1RCV;
						I2C1CONSET = _I2C1CON_SCLREL_MASK;
					}
				}
			}
		}
		mI2C1SClearIntFlag();    									// clear the interrupt flag
	}

}


/**************************************************************************************************
Disable the I2C1 module - master mode
***************************************************************************************************/
void i2c_disable() {
	I2C_Status |= I2C_Status_Disable;
	I2C_Status &= ~(I2C_Status_Interrupt | I2C_Status_MasterCmd);
	I2C_IntLine = NULL;
	I2C_Timer = 0;
	if ((I2C_Status & I2C_Status_BusOwned) && !(I2C_Status & I2C_Status_InProgress)) {	// send stop if required
		I2C_State = I2C_State_Stop;
		I2C1CONSET = _I2C1CON_PEN_MASK;
		while (!(I2C_Status & I2C_Status_Completed)) {}
	}
	I2C_Status &= 0xffff000f;										// clear master status flags
	I2C_Rcvbuf = NULL;												// pointer to the master receive buffer
	I2C_Sendlen = 0;												// length of the master send buffer
	I2C_Rcvlen = 0;													// length of the master receive buffer
	I2C_Addr = 0;													// I2C device address
	I2C_Prev_Addr = 0xffff;											// previous I2C address
	I2C_Timeout = 0;												// master timeout value
	I2C_State = 0;													// the state of the master state machine
	I2C_Send_Index = 0;												// current index into I2C_Send_Buffer
	mI2C1BIntEnable(0);												// disable bus collision interrupt
	mI2C1MIntEnable(0);												// disable master interrupt
	mI2C1BClearIntFlag();											// ensure bus collision flag is clear
	mI2C1MClearIntFlag();											// ensure master flag is clear
	if (!(I2C_Status & I2C_Status_Slave)) {
		I2C1CONCLR = _I2C1CON_I2CEN_MASK; 							// disable I2C1 module
		I2C_Status = 0;												// clear status flags
	}
#ifdef OLIMEX
	ExtCfg(5, EXT_NOT_CONFIG);										// set pins to unconfigured
	ExtCfg(6, EXT_NOT_CONFIG);
#else
	ExtCfg(12, EXT_NOT_CONFIG);										// set pins to unconfigured
	ExtCfg(13, EXT_NOT_CONFIG);
#endif
}


/**************************************************************************************************
Disable the I2C1 module - slave mode
***************************************************************************************************/
void i2c_slave_disable() {
	I2C1CONCLR = _I2C1CON_GCEN_MASK | _I2C1CON_A10M_MASK;
	I2C1ADD = 0;
	I2C1MSK = 0;
	I2C_Status &= 0xff00ffff;										// clear slave status flags
	I2C_Slave_Send_IntLine = NULL;									// clear send interrupt location
	I2C_Slave_Receive_IntLine = NULL;								// clear receive interrupt location
	I2C_Slave_Sendlen = 0;											// set length of the slave send buffer to 0
	I2C_Slave_Addr = 0;												// slave address
	I2C_Slave_Mask = 0;												// slave address mask
	I2C_Rcv_Tail = I2C_Rcv_Head;									// reset receive buffer pointers
	mI2C1SIntEnable(0);												// disable slave interrupt
	mI2C1SClearIntFlag();											// ensure slave flag is clear
	I2C1CONSET = _I2C1CON_SCLREL_MASK;								// release the clock line
	if (!(I2C_Status & I2C_Status_Master)) {
		I2C1CONCLR = _I2C1CON_I2CEN_MASK; 							// disable I2C1 module
		I2C_Status = 0;												// clear status flags
	}
#ifdef OLIMEX
	ExtCfg(5, EXT_NOT_CONFIG);										// set pins to unconfigured
	ExtCfg(6, EXT_NOT_CONFIG);
#else
	ExtCfg(12, EXT_NOT_CONFIG);										// set pins to unconfigured
	ExtCfg(13, EXT_NOT_CONFIG);
#endif
}


/**************************************************************************************************
Enable the I2C1 module - master mode
***************************************************************************************************/
void i2c_enable(int bps) {
	I2C1BRG = (BUSFREQ / (2000 * bps)) - 2;
	if (!(I2C_Status & I2C_Status_Master)) {
		mI2C1BClearIntFlag();						// ensure bus collision flag is clear
		mI2C1MClearIntFlag();						// ensure master flag is clear
		mI2C1BIntEnable(1);						// enable bus collision interrupt
		mI2C1MIntEnable(1);						// enable master interrupt
		I2C_Status |= I2C_Status_Master;
	}
	if (!(I2C_Status & I2C_Status_Enabled)) {
		I2C_Status |= I2C_Status_Enabled;
	#ifdef OLIMEX
		ExtCfg(5, EXT_COM_RESERVED);					// clear BASIC interrupts and disable PIN and SETPIN
		ExtCfg(6, EXT_COM_RESERVED);
	#else
		ExtCfg(12, EXT_COM_RESERVED);					// clear BASIC interrupts and disable PIN and SETPIN
		ExtCfg(13, EXT_COM_RESERVED);
	#endif
    	mI2C1SetIntPriority(5);							// priority 5
    	I2C1CONCLR = _I2C1CON_DISSLW_MASK;
		I2C1CONSET = _I2C1CON_I2CEN_MASK | _I2C1CON_STRICT_MASK;	// enable I2C1 module & strict addressing enforced
		mmI2Cvalue = 0;
	}
}


/**************************************************************************************************
Enable the I2C1 module - slave mode
***************************************************************************************************/
void i2c_slave_enable(int options) {
	if (options & 0x01) {							// respond to general call
		I2C1CONSET = _I2C1CON_GCEN_MASK;
	} else {
		I2C1CONCLR = _I2C1CON_GCEN_MASK;
	}
	if (options & 0x02) {							// 10 bit addressing
		I2C1CONSET = _I2C1CON_A10M_MASK;
	} else {
		I2C1CONCLR = _I2C1CON_A10M_MASK;
	}
	I2C1CONSET = _I2C1CON_STREN_MASK;					// clock stretching enabled
	I2C_Slave_Sendlen = 0;							// set length of the slave send buffer to 0
	I2C1ADD = I2C_Slave_Addr;
	I2C1MSK = I2C_Slave_Mask;
	if (!(I2C_Status & I2C_Status_Slave)) {
		mI2C1SClearIntFlag();						// ensure slave flag is clear
		mI2C1SIntEnable(1);											// enable slave interrupt
		I2C_Status |= I2C_Status_Slave;
	}
	if (!(I2C_Status & I2C_Status_Enabled)) {
		I2C_Status |= I2C_Status_Enabled;
	#ifdef OLIMEX
		ExtCfg(5, EXT_COM_RESERVED);					// clear BASIC interrupts and disable PIN and SETPIN
		ExtCfg(6, EXT_COM_RESERVED);
	#else
		ExtCfg(12, EXT_COM_RESERVED);					// clear BASIC interrupts and disable PIN and SETPIN
		ExtCfg(13, EXT_COM_RESERVED);
	#endif
    	mI2C1SetIntPriority(5);							// priority 5
    	I2C1CONCLR = _I2C1CON_DISSLW_MASK;
		I2C1CONSET = _I2C1CON_I2CEN_MASK | _I2C1CON_STRICT_MASK;	// enable I2C1 module & strict addressing enforced
		mmI2Cvalue = 0;
	}
}


/**************************************************************************************************
Send and/or Receive data - master mode
***************************************************************************************************/
void i2c_masterCommand(int timer) {
	int start_type;
	if (I2C_Sendlen) I2C_Status |= I2C_Status_Send;
	if (I2C_Rcvlen) I2C_Status |= I2C_Status_Receive;
	I2C_Status &= ~(I2C_Status_Completed | I2C_Status_NoAck | I2C_Status_Timeout | I2C_Status_MasterCmd |
									I2C_Status_Slave_Receive | I2C_Status_Slave_Send | I2C_Status_Slave_Send_Rdy | I2C_Status_Slave_Receive_Rdy);
	I2C_Status |= I2C_Status_InProgress;
	I2C_Send_Index = 0;
	mmI2Cvalue = 0;
	if (timer) I2C_Timer = I2C_Timeout;
	if (I2C_Status & I2C_Status_BusOwned) {
		start_type = _I2C1CON_RSEN_MASK;
	} else {
		if (I2C1STAT & _I2C1STAT_S_MASK) {
			I2C_Status |= I2C_Status_MasterCmd;
			return;
		} else {
			start_type = _I2C1CON_SEN_MASK;
		}
	}
	I2C_State = I2C_State_Start;
	I2C1CONSET = start_type;
}


/**************************************************************************************************
Send data - slave mode
***************************************************************************************************/
void i2c_slave_send(int sendlen) {
	I2C1TRN = I2C_Send_Buffer[0];
	I2C_Status &= ~I2C_Status_Slave_Receive;
	I2C_Send_Index = 1;
	I2C_Slave_Sendlen = --sendlen;
	I2C1CONSET = _I2C1CON_SCLREL_MASK;
}


/**************************************************************************************************
Receive data - slave mode
***************************************************************************************************/
void i2c_slave_receive(int rcvlen, float *dataptr, float *rcvdlen) {
	unsigned int i, tail, tail_save;
	tail = tail_save = I2C_Rcv_Tail;
	for (i = 0; i < rcvlen; i++) {
		if (tail != I2C_Rcv_Head) {
			dataptr[i] = I2C_Rcv_Buffer[tail];
			tail = (tail + 1) & 0x000000ff;
		} else {
			break;
		}
	}
	*rcvdlen = i;
	if (i != 0) {
		mI2C1SIntEnable(0);						// disable slave interrupt
		if (tail_save == I2C_Rcv_Tail) {
			I2C_Rcv_Tail = tail;
			if (I2C_Status & I2C_Status_Slave_Receive_Full) {
				I2C_Status &= ~I2C_Status_Slave_Receive_Full;
				mI2C1SSetIntFlag();
			}
		}
		mI2C1SIntEnable(1);						// enable slave interrupt
	}
}
