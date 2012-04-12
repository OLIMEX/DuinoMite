/***********************************************************************************************************************
External.h

Define the MMBasic commands for reading and writing to the digital and analog input/output pins

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

void cmd_setpin(void);
void cmd_pin(void);

void fun_pin(void);

#endif



/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

	{ "PIN(",		T_CMD | T_FUN,		0, cmd_pin		},
	{ "SETPIN",		T_CMD,				0, cmd_setpin	},

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
	{ "PIN(",		T_FUN | T_NBR,		0, fun_pin		},

#endif


#ifdef INCLUDE_FUNCTION_DEFINES
// General definitions used by other modules

#ifndef EXTERNAL_HEADER
#define EXTERNAL_HEADER

extern char *InterruptReturn;
extern int check_interrupt(void);
extern void ClearExternalIO(void);

#define EXT_NOT_CONFIG                          0
#define EXT_ANA_IN				1
#define EXT_DIG_IN				2
#define EXT_FREQ_IN				3
#define EXT_PER_IN				4
#define EXT_CNT_IN				5
#define EXT_INT_HI				6
#define EXT_INT_LO				7
#define EXT_DIG_OUT				8
#define EXT_OC_OUT				9
#define EXT_COM_RESERVED                        100				// this pin is reserved and SETPIN and PIN cannot be used
#define EXT_CONSOLE_RESERVED	EXT_COM_RESERVED + 1                            // this must be one higher than EXT_COM_RESERVED

#ifdef MAXIMITE
	#define NBRPINS				20				// number of pins for external i/o
#endif
#ifdef UBW32
	#define NBRPINS				50				// number of pins for external i/o
#endif
#ifdef OLIMEX
	// SPP +
	#ifdef	OLIMEX_DUINOMITE_EMEGA
		#define NBRPINS				39
	#else
		#define NBRPINS				24
	#endif
	// SPP -
#endif

extern int ExtCurrentConfig[NBRPINS + 1];
extern int INT1Count, INT1Value;
extern int INT2Count, INT2Value;
extern int INT3Count, INT3Value;
extern int INT4Count, INT4Value;

extern void initExtIO(void) ;
extern int ExtCfg(int pin, int cfg) ;
extern int ExtSet(int pin, int val) ;
extern int ExtInp(int pin, int *val) ;

#endif
#endif

