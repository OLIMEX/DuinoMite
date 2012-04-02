/***********************************************************************************************************************
MMBasic

custom.h

Include file that contains the globals and defines for custom.c in MMBasic.
  
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

void cmd_pause(void);
void cmd_timer(void);
void cmd_date(void);
void cmd_time(void);
void cmd_sound(void);
void cmd_ireturn(void);
void cmd_settick(void);
void cmd_copyright(void);
void cmd_font(void);
#ifdef OLIMEX
void fun_dow(void);
#endif

void fun_timer(void);
void fun_date(void);
void fun_time(void);
void fun_inkey(void);
void fun_tab(void);
void fun_pos(void);
void fun_spi(void);
void fun_pos(void);

#endif




/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

	{ "PAUSE",		T_CMD,				0, cmd_pause	},
	{ "TIMER",		T_CMD | T_FUN,		0, cmd_timer	},
	{ "DATE$",		T_CMD | T_FUN,		0, cmd_date		},
	{ "TIME$",		T_CMD | T_FUN,		0, cmd_time		},
	{ "SOUND",		T_CMD,				0, cmd_sound	},
	{ "PWM",		T_CMD,				0, cmd_sound	},
	{ "IRETURN",	T_CMD,				0, cmd_ireturn 	},
	{ "SETTICK",	T_CMD,				0, cmd_settick 	},
	{ "COPYRIGHT",  T_CMD,				0, cmd_copyright},
	{ "FONT",  		T_CMD,				0, cmd_font		},

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
	{ "POS",		T_FNA | T_NBR,		0, fun_pos		},
	{ "TIMER",		T_FNA | T_NBR,		0, fun_timer	},
	{ "DATE$",		T_FNA | T_STR,		0, fun_date		},
	{ "TIME$",		T_FNA | T_STR,		0, fun_time		},
	{ "INKEY$",		T_FNA | T_STR,		0, fun_inkey	},
	{ "TAB(",		T_FUN | T_STR,		0, fun_tab,		},
	{ "SPI(",		T_FUN | T_NBR,		0, fun_spi,		},
	{ "LOAD",		T_NA,			0, op_invalid	},
	{ "LOADB",		T_NA,			0, op_invalid	},
#ifdef OLIMEX
	{ "DOW",		T_FNA | T_NBR,		0, fun_dow		},
#endif
#endif


#ifdef INCLUDE_FUNCTION_DEFINES
// General definitions used by other modules

#ifndef MISC_HEADER
#define MISC_HEADER

extern char *InterruptReturn;
extern int check_interrupt(void);

// struct for the interrupt configuration
// the tick interrupt uses pin number 0 which is not a valid physical pin
struct s_inttbl {
	int last;					// the last value of the pin (ie, hi or low)
	char *intp;					// pointer to the interrupt routine
	unsigned int lohi;			// trigger on lo to hi if true.  Also used for the tic period
};

extern struct s_inttbl inttbl[NBRPINS + 2];

#endif
#endif
