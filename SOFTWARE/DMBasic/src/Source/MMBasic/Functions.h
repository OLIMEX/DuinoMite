/***********************************************************************************************************************
MMBasic

functions.h

Include file that contains the globals and defines for functions.c in MMBasic.
  
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

void fun_abs(void);
void fun_asc(void);
void fun_atn(void);
void fun_chr(void);
void fun_cint(void);
void fun_cos(void);
void fun_exp(void);
void fun_fix(void);
void fun_hex(void);
void fun_instr(void);
void fun_int(void);
void fun_left(void);
void fun_len(void);
void fun_log(void);
void fun_mid(void);
void fun_oct(void);
void fun_right(void);
void fun_rnd(void);
void fun_sgn(void);
void fun_sin(void);
void fun_log(void);
void fun_sqr(void);
void fun_tan(void);
void fun_val(void);
void fun_space(void);
void fun_str(void);
void fun_string(void);
void fun_format(void);
void fun_ucase(void);
void fun_lcase(void);
void fun_peek(void);
void fun_version(void);

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
	{ "ABS(",		T_FUN  | T_NBR,			0, fun_abs		},
	{ "ASC(",		T_FUN  | T_NBR,			0, fun_asc		},
	{ "ATN(",		T_FUN  | T_NBR,			0, fun_atn		},
	{ "CHR$(",		T_FUN  | T_STR,			0, fun_chr,		},
	{ "CINT(",		T_FUN  | T_NBR,			0, fun_cint		},
	{ "COS(",		T_FUN  | T_NBR,			0, fun_cos		},
	{ "EXP(",		T_FUN  | T_NBR,			0, fun_exp		},
	{ "FIX(",		T_FUN  | T_NBR,			0, fun_fix		},
	{ "HEX$(",		T_FUN  | T_STR,			0, fun_hex		},
	{ "INSTR(",		T_FUN  | T_NBR,			0, fun_instr	},
	{ "INT(",		T_FUN  | T_NBR,			0, fun_int		},
	{ "LEFT$(",		T_FUN  | T_STR,			0, fun_left		},
	{ "LEN(",		T_FUN  | T_NBR,			0, fun_len		},
	{ "MID$(",		T_FUN  | T_STR,			0, fun_mid		},
	{ "OCT$(",		T_FUN  | T_STR,			0, fun_oct		},
	{ "RIGHT$(",	T_FUN  | T_STR,			0, fun_right	},
	{ "RND(",		T_FUN  | T_NBR,			0, fun_rnd		},
	{ "RND",		T_FNA  | T_NBR,			0, fun_rnd		},
	{ "SGN(",		T_FUN  | T_NBR,			0, fun_sgn		},
	{ "SIN(",		T_FUN  | T_NBR,			0, fun_sin		},
	{ "LOG(",		T_FUN  | T_NBR,			0, fun_log		},
	{ "SQR(",		T_FUN  | T_NBR,			0, fun_sqr		},
	{ "TAN(",		T_FUN  | T_NBR,			0, fun_tan		},
	{ "VAL(",		T_FUN  | T_NBR,			0, fun_val		},
	{ "SPACE$(",	T_FUN  | T_STR,			0, fun_space	},
	{ "SPC(",		T_FUN  | T_STR,			0, fun_space	},
	{ "STR$(",		T_FUN  | T_STR,			0, fun_str		},
	{ "STRING$(",	T_FUN  | T_STR,			0, fun_string	},
	{ "FORMAT$(",	T_FUN  | T_STR,			0, fun_format	},
	{ "UCASE$(",	T_FUN  | T_STR,			0, fun_ucase	},
	{ "LCASE$(",	T_FUN  | T_STR,			0, fun_lcase	},
	{ "PEEK(",		T_FUN  | T_NBR,			0, fun_peek		},
	{ "MM.VER",		T_FNA  | T_NBR,			0, fun_version	},

#endif
