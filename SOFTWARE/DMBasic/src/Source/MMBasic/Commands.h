/***********************************************************************************************************************
MMBasic

Commands.h

Include file that contains the globals and defines for commands.c in MMBasic.
  
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

void cmd_print(void);
void cmd_let(void);
void cmd_list(void);
void cmd_run(void);
void cmd_save(void);
void cmd_load(void);
void cmd_merge(void);
void cmd_new(void);
void cmd_clear(void);
void cmd_goto(void);
void cmd_if(void);
void cmd_else(void);
void cmd_end(void);
void cmd_input(void);
void cmd_tron(void);
void cmd_troff(void);
void cmd_for(void);
void cmd_next(void);
void cmd_do(void);
void cmd_loop(void);
void cmd_error(void);
void cmd_exit(void);
void cmd_randomize(void);
void cmd_gosub(void);
void cmd_return(void);
void cmd_null(void);
void cmd_read(void);
void cmd_restore(void);
void cmd_lineinput(void);
void cmd_delete(void);
void cmd_on(void);
void cmd_dim(void);
void cmd_erase(void);
void cmd_option(void);
void cmd_continue(void);
void cmd_write(void);
void cmd_memory(void);
void cmd_renumber(void);
void cmd_edit(void);
void cmd_poke(void);
void cmd_auto(void);


extern int forindex;
extern int doindex;
extern int gosubindex;
extern int inside_then_else;
extern int DimUsed;

extern void mergefile(char *fname);
extern char *llist(char *b, char *p);
extern char LastFile[];

#endif




/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

	{ "REM",		T_CMD,				0, cmd_null,	},
	{ "LET",		T_CMD,				0, cmd_let	},
	{ "PRINT",		T_CMD,				0, cmd_print	},
	{ "?",			T_CMD,				0, cmd_print	},
	{ "LIST",		T_CMD,				0, cmd_list	},
	{ "RUN",		T_CMD,				0, cmd_run	},
	{ "SAVE",		T_CMD,				0, cmd_save	},
	{ "LOAD",		T_CMD,				0, cmd_load	},
	{ "MERGE",		T_CMD,				0, cmd_merge	},
	{ "NEW",		T_CMD,				0, cmd_new	},
	{ "CLEAR",		T_CMD,				0, cmd_clear	},
	{ "GOTO",		T_CMD,				0, cmd_goto	},
	{ "IF",			T_CMD,				0, cmd_if	},
	{ "ELSE",		T_CMD,				0, cmd_else	},
	{ "ELSEIF",		T_CMD,				0, cmd_else	},
	{ "ENDIF",		T_CMD,				0, cmd_null	},
	{ "END",		T_CMD,				0, cmd_end	},
	{ "INPUT",		T_CMD,				0, cmd_input	},
	{ "TRON",		T_CMD,				0, cmd_tron	},
	{ "TROFF",		T_CMD,				0, cmd_troff	},
	{ "FOR",		T_CMD,				0, cmd_for	},
	{ "NEXT",		T_CMD,				0, cmd_next	},
	{ "DO",			T_CMD,				0, cmd_do	},
	{ "LOOP",		T_CMD,				0, cmd_loop	},
	{ "WHILE",		T_CMD,				0, cmd_do	},
	{ "WEND",		T_CMD,				0, cmd_loop	},
	{ "ERROR",		T_CMD,				0, cmd_error	},
	{ "EXIT",		T_CMD,				0, cmd_exit	},
	{ "RANDOMIZE",	T_CMD,				0, cmd_randomize},
	{ "GOSUB",		T_CMD,				0, cmd_gosub	},
	{ "RETURN",		T_CMD,				0, cmd_return,	},
	{ "DATA",		T_CMD,				0, cmd_null	},
	{ "READ",		T_CMD,				0, cmd_read	},
	{ "RESTORE",	T_CMD,				0, cmd_restore	},
	{ "LINE INPUT",	T_CMD,				0, cmd_lineinput},
	{ "DELETE",		T_CMD,				0, cmd_delete	},
	{ "ON",			T_CMD,				0, cmd_on	},
	{ "DIM",		T_CMD,				0, cmd_dim	},
	{ "ERASE",		T_CMD,				0, cmd_erase	},
	{ "OPTION",		T_CMD,				0, cmd_option	},
	{ "CONTINUE",	T_CMD,				0, cmd_continue	},
	{ "WRITE",		T_CMD,				0, cmd_write	},
	{ "MEMORY",		T_CMD,				0, cmd_memory	},
	{ "RENUMBER",	T_CMD,				0, cmd_renumber	},
	{ "EDIT",		T_CMD,				0, cmd_edit	},
	{ "POKE",		T_CMD,				0, cmd_poke	},
	{ "AUTO",		T_CMD,				0, cmd_auto	},

#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
//	{ "IF",			T_NA,				0, op_invalid	},
	{ "THEN",		T_NA,				0, op_invalid	},
	{ "ELSE",		T_NA,				0, op_invalid	},
	{ "GOTO",		T_NA,				0, op_invalid	},
	{ "GOSUB",		T_NA,				0, op_invalid	},
	{ "TO",			T_NA,				0, op_invalid	},
	{ "STEP",		T_NA,				0, op_invalid	},
	{ "FOR",		T_NA,				0, op_invalid	},
	{ "WHILE",		T_NA,				0, op_invalid	},
	{ "UNTIL",		T_NA,				0, op_invalid	},

#endif
