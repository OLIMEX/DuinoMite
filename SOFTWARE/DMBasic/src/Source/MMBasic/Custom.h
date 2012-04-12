/***********************************************************************************************************************
MMBasic

Custom.h

Include file that contains the globals and defines for Custom.c in MMBasic.

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


/**********************************************************************************
 the C language function associated with commands, functions or operators should be
 declared here
**********************************************************************************/
#ifdef INCLUDE_FUNCTION_DEFINES
// format:
//      void cmd_???(void)
//      void fun_???(void)
//      void op_???(void)

//	GS OW Start
void cmd_owReset(void);
void cmd_owWrite(void);
void cmd_owRead(void);
void cmd_owSearch(void);
void fun_owCRC8(void);
void fun_owCRC16(void);
void fun_mmOW(void);
//	GS OW End

// Ingmar's additions start

// Ingmar's additions end
#endif


/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

//	GS OW Start
	{ "OWRESET",		T_CMD,				0, cmd_owReset	},
	{ "OWWRITE",		T_CMD,				0, cmd_owWrite	},
	{ "OWREAD",			T_CMD,				0, cmd_owRead		},
	{ "OWSEARCH",		T_CMD,				0, cmd_owSearch },
//	GS OW End
// Ingmar's additions start
	{ "SHIFTOUT",	T_CMD,				0, cmd_shiftout	},
	{ "SHIFTIN",	T_CMD,				0, cmd_shiftin	},
	{ "LOADBMP",	T_CMD,				0, cmd_loadbmp	},
// Ingmar's additions end

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)

//	GS OW Start
	{ "OWCRC8(",	T_FUN | T_NBR,	0, fun_owCRC8 },
	{ "OWCRC16(",	T_FUN | T_NBR,	0, fun_owCRC16 },
	{ "MM.OW",		T_FNA | T_NBR,	0, fun_mmOW		},
//	GS OW End
// Ingmar's additions start
	{ "DM.VMEM",	T_FNA | T_NBR,		0, fun_vmem		},
// Ingmar's additions end
#endif


/**********************************************************************************
 All other required definitions and global variables should be define here
**********************************************************************************/
#ifdef INCLUDE_FUNCTION_DEFINES
#ifndef CUSTOM_HEADER
#define CUSTOM_HEADER

// Functions provided by Custom.c

//	GS OW Start
void ow_pinChk(int pin);
int ow_reset(int pin);
void ow_writeByte(int pin, int data);
int ow_readByte(int pin);
int ow_verifyByte(int pin, int data);
int ow_touchByte(int pin, int data);
int ow_touchBit(int pin, int bit);
void ow_writeBit(int pin, int bit);
int ow_readBit(int pin);

void setcrc16(unsigned short reset);
unsigned short docrc16(unsigned short cdata);
void setcrc8(unsigned char reset);
unsigned char docrc8(unsigned char cdata);

int ow_first(int pin, int do_reset, int alarm_only);
int ow_next(int pin, int do_reset, int alarm_only);
int ow_verify(int pin, int alarm_only);
void ow_serialNum(unsigned char *serialnum_buf, int do_read);
void ow_familySearchSetup(int search_family);
void ow_skipFamily(void);

void ow_onOCPin(int pin);
void ow_offOCPin(int pin);
void ow_setPin(int pin);
void ow_clrPin(int pin);
void ow_inputPin(int pin);
void ow_outputPin(int pin);
int ow_readPin(int pin);

// SPP +
#if defined OLIMEX_DUINOMITE_EMEGA
//int ow_isAnalogPin(int pin);
void ow_PinFunction(int pin, int function);
#endif
// SPP -

//	GS OW End

// Ingmar's additions start
void cmd_loadbmp(void);
void fun_vmem(void);
void cmd_shiftout(void);
void cmd_shiftin(void);
// Ingmar's additions end
#endif
#endif
