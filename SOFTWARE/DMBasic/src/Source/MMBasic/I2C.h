/************************************************************************************************************************
Maximite

I2C.h

Header file for I2C.c
  
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

/**********************************************************************************
 the C language function associated with commands, functions or operators should be
 declared here
**********************************************************************************/
#ifdef INCLUDE_FUNCTION_DEFINES
// format:
//      void cmd_???(void)
//      void fun_???(void)
//      void op_???(void)

void cmd_i2cEnable(void);
void cmd_i2cDisable(void);
void cmd_i2cSend(void);
void cmd_i2cReceive(void);
void cmd_i2cSlaveEnable(void);
void cmd_i2cSlaveDisable(void);
void cmd_i2cSlaveSend(void);
void cmd_i2cSlaveReceive(void);
void cmd_num2byte(void);

void fun_byte2num(void);
void fun_mmi2c(void);

#endif




/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

	{ "I2CEN",		T_CMD,				0, cmd_i2cEnable	},
	{ "I2CDIS",		T_CMD,				0, cmd_i2cDisable	},
	{ "I2CSEND",	T_CMD,				0, cmd_i2cSend		},
	{ "I2CRCV",		T_CMD,				0, cmd_i2cReceive	},
	{ "I2CSEN",		T_CMD,				0, cmd_i2cSlaveEnable },
	{ "I2CSDIS",	T_CMD,				0, cmd_i2cSlaveDisable },
	{ "I2CSSEND",	T_CMD,				0, cmd_i2cSlaveSend },
	{ "I2CSRCV",	T_CMD,				0, cmd_i2cSlaveReceive },
	{ "NUM2BYTE",	T_CMD,				0, cmd_num2byte },

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)

	{ "BYTE2NUM(",	T_FUN | T_NBR,	0, fun_byte2num },
	{ "MM.I2C",			T_FNA | T_NBR,	0, fun_mmi2c		},

#endif



#ifdef INCLUDE_FUNCTION_DEFINES
#ifndef I2C_HEADER
#define I2C_HEADER

#define mI2C1BSetIntFlag()	(IFS0SET = _IFS0_I2C1BIF_MASK)			// macro to set the I2C1 bus collision interrupt flag
#define mI2C1MSetIntFlag()	(IFS0SET = _IFS0_I2C1MIF_MASK)			// macro to set the I2C1 master interrupt flag
#define mI2C1SSetIntFlag()	(IFS0SET = _IFS0_I2C1SIF_MASK)			// macro to set the I2C1 slave interrupt flag

// states of the master state machine
#define I2C_State_Idle     0								// Bus Idle
#define I2C_State_Start    1								// Sending Start or Repeated Start
#define I2C_State_10Bit    2								// Sending a 10 bit address
#define I2C_State_10BitRcv 3								// 10 bit address receive
#define I2C_State_RcvAddr  4								// Receive address
#define I2C_State_Send     5								// Sending Data
#define I2C_State_Receive  6								// Receiving data
#define I2C_State_Ack      7								// Sending Acknowledgement
#define I2C_State_Stop     8								// Sending Stop

// defines for I2C_Status
#define I2C_Status_Enabled						0x00000001
#define I2C_Status_MasterCmd					0x00000002
#define I2C_Status_NoAck							0x00000010
#define I2C_Status_Timeout						0x00000020
#define I2C_Status_InProgress					0x00000040
#define I2C_Status_Completed					0x00000080
#define I2C_Status_Interrupt					0x00000100
#define I2C_Status_BusHold						0x00000200
#define I2C_Status_10BitAddr					0x00000400
#define I2C_Status_BusOwned						0x00000800
#define I2C_Status_Send								0x00001000
#define I2C_Status_Receive						0x00002000
#define I2C_Status_Disable						0x00004000
#define I2C_Status_Master							0x00008000
#define I2C_Status_Slave							0x00010000
#define I2C_Status_Slave_Send					0x00020000
#define I2C_Status_Slave_Receive			0x00040000
#define I2C_Status_Slave_Send_Rdy			0x00080000
#define I2C_Status_Slave_Receive_Rdy	0x00100000
#define I2C_Status_Slave_Receive_Full	0x00200000

#define I2C_10BitAddr_Mask			0x78

// Global variables provided by I2C.c
extern volatile unsigned int I2C_State;						// the state of the master state machine
extern volatile unsigned int I2C_Status;					// status flags
extern unsigned int I2C_Timer;										// master timeout counter
extern char *I2C_IntLine;													// pointer to the master interrupt line number
extern char *I2C_Slave_Send_IntLine;							// pointer to the slave send interrupt line number
extern char *I2C_Slave_Receive_IntLine;						// pointer to the slave receive interrupt line number

// Facilities provided by I2C.c
void i2c_disable(void);
void i2c_enable(int bps);
void i2c_masterCommand(int timer);
void i2c_slave_disable(void);
void i2c_slave_enable(int options);
void i2c_slave_send(int sendlen);
void i2c_slave_receive(int rcvlen, float *dataptr, float *rcvdlen);

#endif
#endif
