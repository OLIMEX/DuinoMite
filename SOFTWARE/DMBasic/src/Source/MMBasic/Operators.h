/***********************************************************************************************************************
MMBasic

operators.h

Include file that contains the globals and defines for operators.c in MMBasic.
  
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

void op_invalid(void);
void op_exp(void);
void op_mul(void);
void op_div(void);
void op_divint(void);
void op_add(void);
void op_subtract(void);
void op_mod(void);
void op_ne(void);
void op_gte(void);
void op_lte(void);
void op_lt(void);
void op_gt(void);
void op_equal(void);
void op_and(void);
void op_or(void);
void op_xor(void);
void op_not(void);

#endif




/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                    P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
	{ "^",			T_OPER | T_NBR,			0, op_exp		},
	{ "*",			T_OPER | T_NBR,			1, op_mul		},
	{ "/",			T_OPER | T_NBR,			1, op_div		},
	{ "\\",			T_OPER | T_NBR,			1, op_divint	},
	{ "MOD",		T_OPER | T_NBR,			1, op_mod		},
	{ "+",			T_OPER | T_NBR | T_STR,	2, op_add		},
	{ "-",			T_OPER | T_NBR,			2, op_subtract	},
	{ "NOT",		T_OPER | T_NBR,			3, op_not		},
	{ "<>",			T_OPER | T_NBR | T_STR,	4, op_ne		},
	{ ">=",			T_OPER | T_NBR | T_STR,	4, op_gte		},
	{ "=>",			T_OPER | T_NBR | T_STR,	4, op_gte		},
	{ "<=",			T_OPER | T_NBR | T_STR,	4, op_lte		},
	{ "=<",			T_OPER | T_NBR | T_STR,	4, op_lte		},
	{ "<",			T_OPER | T_NBR | T_STR,	4, op_lt		},
	{ ">",			T_OPER | T_NBR | T_STR,	4, op_gt		},
	{ "=",			T_OPER | T_NBR | T_STR,	5, op_equal		},
	{ "AND",		T_OPER | T_NBR,			6, op_and		},
	{ "OR",			T_OPER | T_NBR,			6, op_or		},
	{ "XOR",		T_OPER | T_NBR,			6, op_xor		},
	
#endif

