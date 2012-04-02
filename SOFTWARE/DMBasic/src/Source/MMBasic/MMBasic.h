/***********************************************************************************************************************
MMBasic.h

Include file that contains the globals and defines for MMBasic.c in MMBasic.
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#define TEST_STACK_OVERFLOW							// define for stack overflow check - will only work on the PIC32

//#ifndef EOF
//	#define EOF -1
//#endif

#include "../MMBasic/Files.h"
#include "../Maximite.h"

// types used to define an item of data.  Used in tokens, variables and arguments to functions
#define T_NOTYPE	0x00            					// type not set or discovered
#define T_NBR		0x01							// number (or float) type
#define T_STR		0x02 							// string type

// types of tokens.  These are or'ed with the data types above to fully define a token
#define T_INV		0							// an invalid token
#define T_NA		0							// an invalid token
#define T_CMD		0x10							// a command
#define T_OPER		0x20							// an operator
#define T_FUN		0x40							// a function (also used for a function that can operate as a command)
#define T_FNA		0x80							// a function that has no arguments

#define C_BASETOKEN 0x80							// the base of the token numbers

#define T_EOL		0
#define T_LINENBR	1

#define E_END		255							// dummy last operator in an expression

#define V_FIND				0x01
#define V_CREATE_TMP		0x02

#define V_NOFIND_ERR		0x10
#define V_NOFIND_NULL		0x20
#define V_DIM_ARRAY			0x40


#ifndef BOOL_ALREADY_DEFINED
	#define BOOL_ALREADY_DEFINED
	typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    // Undefined size
#endif

#ifndef true
	#define true		1
#endif

#ifndef false
	#define false		0
#endif


/**************************************************************************************************************************
Define RAM usage - this is VERY simple memory management
**************************************************************************************************************************/

#define MAXVARLEN			32						// maximum length of a variable name
#define MAXSTRLEN			255						// maximum length of a string
#define STRINGSIZE			256						// must be 1 more than MAXSTRLEN.  2 of these buffers are staticaly created
#define MAXTEMPSTRINGS		48						// each entry takes up 4 bytes
#define MAXOPENFILES		10						// maximum number of open files
#define MAXDIM				8						// maximum nbr of dimensions to an array

#define NBRPROGKEYS			12						// number of programmable function keys
#define MAXKEYLEN			12						// maximum length of a programmable function key string

#define PROGMEMSIZE			(30 * 1024)				// Assuming each line takes 40 bytes (25 lines/KB) this is 750 lines
#define MAXVARS				250						// 8 + MAXVARLEN + MAXDIM * 2  (ie, 56 bytes) - these do not incl array members
#define MAXFORLOOPS			20						// each entry uses 17 bytes
#define MAXDOLOOPS			20						// each entry uses 12 bytes
#define MAXGOSUB			250						// each entry uses 4 bytes
#define MAX_MULTILINE_IF	20						// each entry uses 8 bytes

/*
	With the above configuration
	--------------------------------
	program space			      =  30K
	variable space approx	      =  13K
	other static RAM used	      =   3K
							      ------
	total                            46K
	                              ======
	if the RAM available is       =  74K (after video, USB, etc)
	this leaves array & str space =  38K (accessed by malloc() from the heap)

**************************************************************************************************************************/

#define MAXLINENBR	65001

// define the maximum number of arguments to PRINT, INPUT, WRITE, ON, DIM, ERASE, DATA and READ
// each entry uses zero bytes.  The number is limited by the length of a command line
#define MAX_ARG_COUNT 		50

#define MAXPROMPTLEN		49										// max length of a prompt incl the terminating null

// skip whitespace
// finishes with x pointing to the next non space char
#define skipspace(x)	while(*x == ' ') x++

// skip to the next element
// finishes pointing to the zero char that preceeds an element
#define skipelement(x)	while(*x) x++

// skip to the next line
// skips text and and element separators until it is pointing to the zero char marking the start of a new line.
// the next byte will be either the line number token or zero char if end of program
#define skipline(x)		while(!(x[-1] == 0 && (x[0] == T_LINENBR || x[0] == 0)))x++

// find a token
// finishes pointing to the token or zero char if not found in the line
#define findtoken(x)	while(*x != (tkn) && *x)x++

#define isnamestart(c)	(isalpha(c) || c == '_')					// true if valid atart of a variable name
#define isnamechar(c)	(isalnum(c) || c == '_' || c == '.')		// true if valid part of a variable name
#define isnameend(c)	(isalnum(c) || c == '$' || c == '(')		// true if valid end of a variable name

#define tokentype(i)	((i >= C_BASETOKEN && i < TokenTableSize - 1 + C_BASETOKEN) ? (tokentbl[i - C_BASETOKEN].type) : 0)				// get the type of a token
#define tokenfunction(i)((i >= C_BASETOKEN && i < TokenTableSize - 1 + C_BASETOKEN) ? (tokentbl[i - C_BASETOKEN].fptr) : (tokentbl[0].fptr))	// get the function pointer  of a token
#define tokenname(i)	((i >= C_BASETOKEN && i < TokenTableSize - 1 + C_BASETOKEN) ? (tokentbl[i - C_BASETOKEN].name) : "")			// get the name of a token

#define commandtype(i)	((i >= C_BASETOKEN && i < CommandTableSize - 1 + C_BASETOKEN) ? (commandtbl[i - C_BASETOKEN].type) : 0)				// get the type of a token
#define commandname(i)	((i >= C_BASETOKEN && i < CommandTableSize - 1 + C_BASETOKEN) ? (commandtbl[i - C_BASETOKEN].name) : "")		// get the name of a command

// this macro will allocate temporary memory space and build an argument table in it
// x = pointer to the basic text to be split up (char *)
// y = maximum number of args (will throw an error if exceeded) (int)
// s = a string of characters to be used in detecting where to split the text (char *)
#define getargs(x, y, s) char argbuf[STRINGSIZE]; char *argv[y]; int argc; makeargs(x, y, argbuf, argv, &argc, s)

extern int CommandTableSize, TokenTableSize;
extern int TraceOn;

extern jmp_buf mark;							// longjump to recover from an error

extern char pmemory[];							// program memory
extern int psize;								// size of the program in program memory
extern char *pp;								// general use pointer into program memory

extern int NextData;							// used to track the next item to read in DATA & READ stmts
extern int CurrentLineNbr;						// the current line number being executed (is zero if an immediate command is being executed)
extern char *ContinuePoint;                     // Where to continue from if using the continue statement

extern char inpbuf[];							// used to store user keystrokes until we have a line
extern char tknbuf[];                           // used to store the tokenised representation of the users input line
extern char lastcmd[];							// used to store the last command in case it is needed by the EDIT command

extern int HeapUsed;							// used to track the amount of the heap used.  For the memory command.

extern float farg1, farg2, fret;                // Global floating point variables used by operators
extern char *sarg1, *sarg2, *sret;              // Global string pointers used by operators
extern int targ;                                // Global type of argument (string or float) returned by an operator

extern int cmdtoken;							// Token number of the command
extern char *cmdline;							// Command line terminated with a zero char and trimmed of spaces
extern char *nextstmt;							// Pointer to the next statement to be executed.
extern char *ep;								// Pointer to the argument to a function

struct s_tokentbl {                             // structure of the token table
	char *name;                                 // the string (eg, PRINT, FOR, ASC(, etc)
	char type;                                  // the type returned (T_NBR, T_STR)
	char precedence;							// precedence used by operators only.  operators with equal precedence are processed left to right.
	void (*fptr)(void);							// pointer to the function that will interpret that token
};
extern struct s_tokentbl tokentbl[];
extern struct s_tokentbl commandtbl[];

#define TKN_THEN			0
#define TKN_ELSE			1
#define TKN_GOTO			2
#define TKN_EQUAL			3
#define TKN_TO				4
#define TKN_STEP			5
#define TKN_WHILE			6
#define TKN_UNTIL			7
#define TKN_FOR				8
#define TKN_GOSUB			9
#define TKN_AS				10

#define TOKEN_LOOKUP_SIZE	11
extern unsigned char tokenvalue[TOKEN_LOOKUP_SIZE];

struct s_vartbl {								// structure of the variable table
	char name[MAXVARLEN + 1];					// variable's name
	char type;									// its type (T_NUM or T_STR)
	short int dims[MAXDIM];						// the dimensions. it is an array if the first dimension is NOT zero
	union u_val {
		float f;                                // the value if it is a float
		float *fa;                              // pointer to the allocated memory if it is an array
		char *s;                                // pointer to the allocated memory (always STRINGSIZE bytes) if it is a string
	} val;
};
extern struct s_vartbl vartbl[];

extern int varcnt;                              // number of variables defined (eg, largest index into the variable table)
extern int VarIndex;                            // index of the current variable.  set after the findvar() function has found/created a variable

extern int OptionBase;							// value of OPTION BASE
extern int OptionErrorAbort;					// value of OPTION ERROR
extern char PromptString[MAXPROMPTLEN];			// the command prompt

extern int autoOn, autoNext, autoIncr;			// use by the AUTO command
extern unsigned char FunKey[NBRPROGKEYS][MAXKEYLEN + 1]; // used by the programmable function keys

extern void  __attribute__((noreturn)) MMBasicStart(void);
extern void __attribute__((noreturn)) error(char *);
int MMround(float);
void makeargs(char **tp, int maxargs, char *argbuf, char *argv[], int *argc, char *delim);
void *findvar(char *, int);
void erasearray(char *n);
void clearvars(void);
void clearstack(void);
char *evaluate(char *p, float *fa, char **sa, int *ta);
float getnumber(char *p);
int getinteger(char *p);
char *getstring(char *p);
char *getmemory(int size);
void tokenise(void);
void ExecuteProgram(void);
void AddProgramLine(void);
char *findline(int, int);
void clearprog(void);
char *skipvar(char *p);
int vartype(char *p);
char *getclosebracket(char *p);
void makeupper(char *p);
void checkend(char *p);
int commandvalue(const char *n);
int GetTokenValue(const char *n);
void ClearTempStringSpace(void);
char *checkstring(char *p, char *tkn);
int GetLineLength(char *p);
char *GetTempStringSpace(void);
char *MtoC(char *p);
char *CtoM(char *p);
void Mstrcpy(char *dest, char *src);
void Mstrcat(char *dest, char *src);
char *getCstring(char *p);
void EditInputLine(int line);
void ProcessInputLine(void);
int IsValidLine(int line);
void InsertLastcmd(char *s);


inline int str_equal (const unsigned char *s1, const unsigned char *s2);
int	 strncasecmp (const char *s1, const char *s2, size_t n);


