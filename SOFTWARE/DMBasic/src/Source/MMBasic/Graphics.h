/***********************************************************************************************************************
MMBasic

Screen.h

Include file that contains the definition of the MMBasic commands for handling the video graphics.

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

void cmd_cls(void);
void cmd_circle(void);
void cmd_line(void);
void cmd_pset(void);
void cmd_preset(void);
void cmd_locate(void);
void cmd_pixel(void);
void cmd_savebmp(void);

void fun_pixel(void);
void fun_hres(void);
void fun_vres(void);

#endif




/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

	{ "CLS",		T_CMD,				0, cmd_cls		},
	{ "CIRCLE",		T_CMD,				0, cmd_circle	},
	{ "LINE",		T_CMD,				0, cmd_line		},
	{ "PSET",		T_CMD,				0, cmd_pset		},
	{ "PRESET",		T_CMD,				0, cmd_preset	},
	{ "LOCATE",		T_CMD,				0, cmd_locate	},
	{ "PIXEL(",		T_CMD | T_FUN,		0, cmd_pixel	},
	{ "SAVEBMP",	T_CMD,				0, cmd_savebmp	},

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
	{ "PIXEL(",		T_FUN | T_NBR,		0, fun_pixel	},
	{ "MM.HRES",	T_FNA | T_NBR,		0, fun_hres		},
	{ "MM.VRES",	T_FNA | T_NBR,		0, fun_vres		},

#endif

