/***********************************************************************************************************************
MMBasic

commands.c

Handles all the commands in MMBasic
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

#include <stdio.h>

#define INCLUDE_FUNCTION_DEFINES
#include "Maximite.h"
#include "MMBasic.h"
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"

#include "../SDCard/SDCard.h"
#include "../Keyboard/Keyboard.h"
#include "../Video/Video.h"



void flist(int, int, int);
void clearprog(void);
void mergefile(char *);
void execute_one_command(char *p);



// stack to keep track of nested FOR/NEXT loops
struct s_forstack {
	char *forptr;							// pointer to the FOR command in program memory
	char *nextptr;							// pointer to the NEXT command in program memory
	char *nextid;
	float *var;								// value of the for variable
	float tovalue;
	float stepvalue;
} forstack[MAXFORLOOPS] ;

int forindex;



// stack to keep track of nested DO/LOOP loops
struct s_dostack {
	char *evalptr;							// pointer to the expression to be evaluated
	char *loopptr;							// pointer to the loop statement
	char *loopid;
	char *doptr;							// pointer to the do statement
} dostack[MAXDOLOOPS] ;

int doindex;



// stack to keep track of GOSUBs
char *gosubstack[MAXGOSUB];
int gosubindex;

int inside_then_else = false;				// true if the command being executed is inside an IF-THEN-ELSE construct

int DimUsed = false;						// used to catch OPTION BASE after DIM has been used

char LastFile[FILENAME_LENGTH];				// used to keep track of the last file RUN, LOADed or SAVed






/********************************************************************************************************************************************
 commands
 each function is responsible for decoding a command
 all function names are in the form cmd_xxxx() so, if you want to search for the function responsible for the GOSUB command look for cmd_gosub
 
 There are 4 items of information that are setup before the command is run.
 All these are globals.

 int cmdtoken		This is the token number of the command (some commands can handle multiple
				statement types and this helps them differentiate)

 char *cmdline	This is the command line terminated with a zero char and trimmed of leading 
				spaces.  It may exist anywhere in memory (or even ROM).

 char *nextstmt	This is a pointer to the next statement to be executed.  The only thing a 
				command can do with it is save it or change it to some other location.

 int CurrentLineNbr  This is read only and is set to zero if the command is in immediate mode.

 The only actions a command can do to change the program flow is to change nextstmt or 
 execute longjmp(mark, 1) if it wants to abort the program.

 ********************************************************************************************************************************************/



void cmd_null(void) {
	// do nothing (this is just a placeholder for commands that have no action)
}

// the PRINT command
void cmd_print(void) {
	char *s, *p;
	float f;
	int i, t, fnbr;
	int printspace;													// this is used to make sure that only one space is printed
	int concatstr;													// this is used to keep track if we are concatenating strings
	int docrlf;														// this is used to suppress the cr/lf if needed
	
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ";,");				// this is a macro and must be the first executable stmt
	
	//s = 123456; *s = 56;											// for testing the exception handler
	
	concatstr = printspace = false;
	docrlf = true;
	
	if(argc > 0 && *argv[0] == '#') {								// check if the first arg is a file number
		argv[0]++;
		fnbr = getinteger(argv[0]);									// get the number
		i = 1;
		if(argc >= 2 && *argv[1] == ',') i = 2;						// and set the next argument to be looked at
	}
	else {
		fnbr = 0;													// no file number so default to the standard output
		i = 0;	
	}	

	for(; i < argc; i++) {											// step through the arguments
		if(*argv[i] == ',') {
			MMfputc('\t', fnbr);									// print a tab for a comma
			docrlf = true;
		}
		else if(*argv[i] == ';') {
			docrlf = false;											// other than suppress cr/lf do nothing for a semicolon
		}
		else {														// we have a normal expression
			p = argv[i];
			while(*p) {
				t = T_NOTYPE;
				p = evaluate(p, &f, &s, &t);						// get the value and type of the argument
				if(t == T_NBR) {
					sprintf(inpbuf, "% g", f);
					MMfputs(CtoM(inpbuf), fnbr);					// if numeric, convert to a MMBasic string and output
				}
				else if(t == T_STR) {
					MMfputs(s, fnbr);								// print if a string (s is a MMBasic string)
				}
			}
			docrlf = true;
		}
	}
	if(docrlf) MMfputs("\2\r\n", fnbr);								// print the terminating cr/lf unless it has been suppressed
}



void cmd_write(void) {
	char *s;
	float f;
	int i, t, fnbr;
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// this is a macro and must be the first executable stmt

	if(argc > 0 && *argv[0] == '#') {
		argv[0]++;
		fnbr = getinteger(argv[0]);
		i = 1;
		if(argc >= 2 && *argv[1] == ',') i = 2;
	}
	else {
		fnbr = 0;
		i = 0;	
	}	

	for(; i < argc; i++) {
		if(*argv[i] == ',') {
			MMfputc(',', fnbr);
		}
		else {
			t = T_NOTYPE;
			evaluate(argv[i], &f, &s, &t);
			if(t == T_NBR) {
				sprintf(inpbuf, "%g", f);
				MMfputs(CtoM(inpbuf), fnbr);						// convert to a MMBasic string and output
			}
			else if(t == T_STR) {
				MMfputc('"', fnbr);
				MMfputs(s, fnbr);									// output (s is a MMBasic string)
				MMfputc('"', fnbr);
			}	
		}
	}
	MMfputs("\2\r\n", fnbr);										// this is a MMBasic string constant
}



// the LET command
// because the LET is implied (ie, line does not have a recognisable command)
// it ends up as the place where mistyped commands are discovered.  This is why
// the error message is "Unknown command"
void cmd_let(void) {
	char *p1, *p2, *tp;

	p1 = cmdline; 					

	// search through the line looking for the equals sign
	while(*p1 && tokenfunction(*p1) != op_equal) 
			p1++;
	if(!*p1) error("Unknown command");

	// create the variable
	p2 = findvar(cmdline, V_FIND);

	// step over the equals sign, evaluate the rest of the command and save in the variable
	p1++;
	if(vartype(cmdline) == T_STR) {
		//Mstrcpy(p2, getstring(p1));
		tp = getstring(p1);
		Mstrcpy(p2, tp);
	}		
	else 
		(*(float *)p2) = getnumber(p1);
}



void cmd_list(void) {
	int fromnbr = 1;
	int tonbr = MAXLINENBR;
	char ss[2];
	ss[0] = GetTokenValue("-");						// this will be used to split up the argument line
	ss[1] = 0;								// blame Microsoft for a poor choice of syntax
	{									// start a new block
		getargs(&cmdline, 4, ss);					// getargs macro must be the first executable stmt in a block

		if(argc == 1) fromnbr = tonbr = getinteger(argv[0]);		// this is a single line number eg: LIST 340
		if(argc == 2) {
			if(*argv[0] == *ss)
				tonbr = getinteger(argv[1]);			// this is LIST -340
			else if(*argv[1] == *ss)
				fromnbr = getinteger(argv[0]);			// this is LIST 230-
			else
				error("Invalid syntax");
		}
		if(argc == 3) {							// this is LIST 230-340
			fromnbr = getinteger(argv[0]);
			tonbr = getinteger(argv[2]);
		}
		flist(0, fromnbr, tonbr);
	}
}



void cmd_run(void) {
	char *p = NULL;
	char tmp[FILENAME_LENGTH];
	
	skipspace(cmdline);
	// first see if this is RUN without any arguments (ie, run the program in memory)
	if(*cmdline == 0 || *cmdline == '\'') {
		p  = pmemory;
	} 
	// check if there is a number following and run the specified line number
	else if(isdigit(*cmdline)) {
		p = findline(getinteger(cmdline), true);
	} 
	// otherwise just assume that a file name has been provided
	else {	
		strcpy(tmp, getCstring(cmdline));		// we copy to a temp space because clearvars() will clobber the return from getCstring()
		if(strchr(tmp, '.') == NULL) strcat(tmp, ".BAS");
		CloseAllFiles();
		MMfopen(tmp, "r", 1);					// first open the file as a test
		if(ErrorCheck()) return;				// abort if error and OPTION ERROR CONTINUE is in effect
		MMfclose(1);							// close the file for the merge
//	GS - Run Fix Start
//		strcpy(LastFile, tmp);					// save the name for the SAVE command
		strcpy(tmp, getCstring(cmdline));		// we copy to a temp space because clearvars() will clobber the return from getCstring()
//	GS - Run Fix End
		clearprog();							// and clear the program space so that the merge becomes a load
		mergefile(tmp);							// load the program
		strcpy(LastFile, tmp);					// save the name for the SAVE command
		p = pmemory;
	}
	ClearExternalIO();
	clearvars();
	clearstack();
	nextstmt = p;
	
	#ifdef PROFILE
		StartProfiling();
	#endif
}



void cmd_continue(void) {
	if(CurrentLineNbr != 0) error("Statement can only be run from the command prompt");
	if(ContinuePoint == NULL) error("Cannot continue, program changed");
	checkend(cmdline);
	nextstmt = ContinuePoint;
}



void cmd_save(void) {
	char *p;
	if(*cmdline)								// if the length of the string is zero
		p = getCstring(cmdline);				// file is on the command line
	else
		p = LastFile;							// else use the last
	if(strchr(p, '.') == NULL) strcat(p, ".BAS");
	CloseAllFiles();
	MMfopen(p, "w", 1);
	flist(1, 1, MAXLINENBR);
	strcpy(LastFile, p);						// save the name for a future SAVE command
}

	
//extern int tokencnt;
void cmd_load(void) {
	char tmp[FILENAME_LENGTH];
	strcpy(tmp, getCstring(cmdline));				// we copy to a temp space because clearvars() will clobber the return from getCstring()
	if(strchr(tmp, '.') == NULL) strcat(tmp, ".BAS");
	ClearExternalIO();
	clearvars();
	clearstack();
	clearprog();								// clear the program space
	mergefile(tmp);								// load
	strcpy(LastFile, tmp);                                                  // save the name for a future SAVE command
	longjmp(mark, 1);							// jump back to the input prompt
}


void cmd_merge(void) {
	clearvars();
	clearstack();
	mergefile(getCstring(cmdline));				// get the file name and convert to a C style string
	longjmp(mark, 1);							// jump back to the input prompt
}



void cmd_new(void) {
	//if(CurrentLineNbr != 0) error("Statement can only be run from the command prompt");
	ClearExternalIO();
	checkend(cmdline);
	clearstack();
	clearvars();
	TraceOn = false;    // turn off tron on new
	clearprog();
	*LastFile = 0;
	longjmp(mark, 1);							// jump back to the input prompt
}


void cmd_clear(void) {
	checkend(cmdline);
	clearvars();
}


void cmd_goto(void) {
	nextstmt = findline(getinteger(cmdline), true);
}



void cmd_if(void) {
	int r, i, testgoto, testelseif;
	char ss[3];														// this will be used to split up the argument line
	char *p, *tp, endiftoken, elseiftoken, elsetoken;

	ss[0] = tokenvalue[TKN_THEN];
	ss[1] = tokenvalue[TKN_ELSE];
	ss[2] = 0;
	
	testgoto = false;
	testelseif = false;

retest_an_if:
	{																// start a new block
		getargs(&cmdline, 20, ss);									// getargs macro must be the first executable stmt in a block
		
		if(testelseif && argc > 2) error("Unexpected text");
		
		// if there is no THEN token retry the test with a GOTO.  If that fails flag an error
		if(argc < 2 || *argv[1] != ss[0]) {
			if(testgoto) error("IF without THEN");
			ss[0] = tokenvalue[TKN_GOTO];
			testgoto = true;
			goto retest_an_if;
		}	
		
		
		// allow for IF statements embedded inside this IF
		if(argc >= 3 && *argv[2] == commandvalue("IF") + C_BASETOKEN) argc = 3;// this is IF xx=yy THEN IF ... so we want to evaluate only the first 3 
		if(argc >= 5 && *argv[4] == commandvalue("IF") + C_BASETOKEN) argc = 5;// this is IF xx=yy THEN cmd ELSE IF ... so we want to evaluate only the first 5
		 
		if(argc == 4 || (argc == 5 && *argv[3] != ss[1])) error("Invalid syntax");

		r = (getnumber(argv[0]) != 0);								// evaluate the expression controlling the if statement

		if(r) {
			// the test returned TRUE
			// first check if it is a multiline IF (ie, only 2 args)
			if(argc == 2) {
				// if multiline do nothing, control will fall through to the next line (which is what we want to execute next)
				;
			}
			else {													
				// This is a standard single line IF statement
				// Because the test was TRUE we are just interested in the THEN cmd stage.
				if(isdigit(*argv[2]))
					nextstmt = findline(getinteger(argv[2]), true);	
				else {
					if(argc == 5) { 
						// this is a full IF THEN ELSE and the statement we want to execute is between the THEN & ELSE
						// this is handled by a special routine
						execute_one_command(argv[2]);
					} else {
						// easy - there is no ELSE clause so just point the next statement pointer to the byte after the THEN token
						for(p = cmdline; *p && *p != ss[0]; p++);	// search for the token
						nextstmt = p + 1;							// and point to the byte after
					}
				}
			} 
		} else {
			// the test returned FALSE so we are just interested in the ELSE stage (if present)
			// first check if it is a multiline IF (ie, only 2 args)
			if(argc == 2) {
				// search for the next ELSE, or ENDIF and pass control to the following line
				// if an ELSEIF is found re execute this function to evaluate the condition following the ELSEIF
				endiftoken = commandvalue("ENDIF") + C_BASETOKEN;
				elseiftoken = commandvalue("ELSEIF") + C_BASETOKEN;
				elsetoken = commandvalue("ELSE") + C_BASETOKEN;
				i = 1; p = nextstmt;
				while(1) {
					if(p[0] == 0) {
						if(p[1] == 0) error("Multiline IF without matching ENDIF");
						if(p[1] == T_LINENBR) {
							r = (p[2] << 8) | p[3];					// get the line number for error reporting in the ELSEIF stmt
							p += 3;
						}
						p++;
						skipspace(p);
						if(*p == cmdtoken + C_BASETOKEN) {
							// found a nested IF command, we now need to determine if it is a single or multiline IF
							// search for a THEN, then check if only white space follows.  If so, it is multiline.
							tp = p + 1;
							while(*tp && *tp != ss[0]) tp++;
							if(*tp) tp++;								// step over the THEN
							skipspace(tp);
							if(*tp == 0 || *tp == '\'')					// yes, only whitespace follows
								i++;									// count it as a nested IF
							else										// no, it is a single line IF
								skipelement(p);							// skip to the end so that we avoid an ELSE
							continue;
						}
						
						if(*p == elsetoken && i == 1) {
							// found an ELSE at the same level as this IF.  Step over it and continue with the statement after it
							skipelement(p);	
							nextstmt = p;
							break;
						}
							
						if(*p == elseiftoken && i == 1) { 
							// we have found an ELSEIF statement at the same level as our IF statement
							// setup the environment to make this function evaluate the test following ELSEIF and jump back
							// to the start of the function.  This is not very clean (it uses the dreaded goto for a start) but it works
							cmdline = ++p; 
							skipelement(p); 
							nextstmt = p; 
							CurrentLineNbr = r;
							testgoto = false;
							testelseif = true;
							goto retest_an_if;
						}
						
						if(*p == endiftoken) i--;						// found an ENDIF so decrement our nested counter
						if(i == 0) {
							// found our matching ENDIF stmt.  Step over it and continue with the statement after it
							skipelement(p);	
							nextstmt = p;
							break;
						}		
					}	
					p++;				
				}
			}
			else {
				// this must be a single line IF statement
				// check if there is an ELSE on the same line
				if(argc == 5) {
					// there is an ELSE command
					if(isdigit(*argv[4]))
						// and it is just a number, so get it and find the line
						nextstmt = findline(getinteger(argv[4]), true);	
					else {
						// there is a statement after the ELSE clause  so just point to it (the byte after the ELSE token)
						for(p = cmdline; *p && *p != ss[1]; p++);		// search for the token
						nextstmt = p + 1;								// and point to the byte after
					}
				} else {
					// no ELSE on a single line IF statement, so just continue with the next statement
					skipline(cmdline);
					nextstmt = cmdline;
				}
			}
		}
	}
}



void cmd_else(void) {
	int i;
	char *p, *tp, endiftoken, iftoken, thentoken;

	// search for the next ENDIF and pass control to the following line
	iftoken = commandvalue("IF") + C_BASETOKEN;
	endiftoken = commandvalue("ENDIF") + C_BASETOKEN;
	thentoken = tokenvalue[TKN_THEN];
	i = 1; p = nextstmt;

	if(cmdtoken ==  commandvalue("ELSE")) checkend(cmdline);
	
	while(1) {
		if(p[0] == 0) {
			if(p[1] == 0) error("Multiline IF without matching ENDIF");
			if(p[1] == T_LINENBR) {
				p += 3;
			}
			p++;
			skipspace(p);
			if(*p == iftoken) { //cmdtoken + C_BASETOKEN) {
				// found a nested IF command, we now need to determine if it is a single or multiline IF
				// search for a THEN, then check if only white space follows.  If so, it is multiline.
				tp = p + 1;
				while(*tp && *tp != thentoken) tp++;
				if(*tp) tp++;											// step over the THEN
				skipspace(tp);
				if(*tp == 0 || *tp == '\'')								// yes, only whitespace follows
					i++;												// count it as a nested IF
			}
			if(*p == endiftoken) i--;									// found an ENDIF so decrement our nested counter
			if(i == 0) break;											// found our matching ENDIF stmt
		}
		p++;
	}
	// found a matching ENDIF.  Step over it and continue with the statement after it
	skipelement(p);	
	nextstmt = p;
}



void cmd_end(void) {
	checkend(cmdline);
	longjmp(mark, 1);												// jump back to the input prompt
}



void cmd_input(void) {
	char s[STRINGSIZE];
	char *p, *sp, *tp;
	int i, fnbr;
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",;");				// this is a macro and must be the first executable stmt

	// is the first argument a file number specifier?  If so, get it
	if(argc >= 3 && *argv[0] == '#') {
		argv[0]++;
		fnbr = getinteger(argv[0]);
		i = 2;
	}
	else {
		fnbr = 0;
		// is the first argument a prompt?
		// if so, print it followed by an optional question mark
		if(argc >= 3 && *argv[0] == '"' && (*argv[1] == ',' || *argv[1] == ';')) {
			*(argv[0] + strlen(argv[0]) - 1) = 0;
			argv[0]++;
			MMPrintString(argv[0]);
			if(*argv[1] == ';') MMPrintString("? ");
			i = 2;
		}
		else {
			MMPrintString("? ");										// no prompt?  then just print the question mark
			i = 0;
		}
	}	
	
	if(argc - i < 1) error("Invalid syntax");						// no variable to input to

	*inpbuf = 0;													// start with an empty buffer
	if(fnbr == 0)
		EditInputLine(0);											// if keyboard get the line with editing
	else
		MMgetline(fnbr, inpbuf);									// otherwise use the old way of getting the input line											
	p = inpbuf;

	// step through the variables listed for the input statement
	// and find the next item on the line and assign it to the variable
	for(; i < argc; i++) {
		sp = s;														// sp is a temp pointer into s[]
		if(*argv[i] == ',' || *argv[i] == ';') continue;
		skipspace(p);
		if(*p != 0) {
			if(*p == '"') {												// if it is a quoted string
				p++;													// step over the quote
				while(*p && *p != '"')  *sp++ = *p++;					// and copy everything upto the next quote
				while(*p && *p != ',') p++;								// then find the next comma
			} else {													// otherwise it is a normal string of characters
				while(*p && *p != ',') *sp++ = *p++;					// copy up to the comma
				while(sp > s && sp[-1] == ' ') sp--;					// and trim trailing whitespace
			}
		}	
		*sp = 0;													// terminate the string
		tp = findvar(argv[i], V_FIND);								// get the variable and save its new value
		if(vartype(argv[i]) == T_STR) {
			strcpy(tp, s);
			CtoM(tp);												// convert to a MMBasic string
		}	
		else
			*((float *)tp) = atof(s);
		if(*p == ',') p++;
	}
}


		

void cmd_tron(void) {
	char *p;

	p = cmdline;
	if ((p = checkstring(p, "SERIAL")) != NULL) {
		TraceOn = 2;
	} else {
		TraceOn = true;
	}
}



void cmd_troff(void) {
	checkend(cmdline);
	TraceOn = false;
}



// FOR command
void cmd_for(void) {
	int i, t, vlen;
	char ss[4];														// this will be used to split up the argument line
	char *p, *tp, *xp;
	float *vptr;
	char *vname;
	char fortoken, nexttoken;

	fortoken = commandvalue("FOR") + C_BASETOKEN;
	nexttoken = commandvalue("NEXT") + C_BASETOKEN;

	ss[0] = tokenvalue[TKN_EQUAL];
	ss[1] = tokenvalue[TKN_TO];
	ss[2] = tokenvalue[TKN_STEP];
	ss[3] = 0;

	{																// start a new block
		getargs(&cmdline, 7, ss);									// getargs macro must be the first executable stmt in a block
		if(argc < 5 || argc == 6 || *argv[1] != ss[0] || *argv[3] != ss[1]) error("FOR with misplaced = or TO");
		if(argc == 6 || (argc == 7 && *argv[5] != ss[2])) error("Invalid syntax");

		// get the variable name and trim any spaces
		vname = argv[0];
		if(*vname && *vname == ' ') vname++;
		while(*vname && vname[strlen(vname) - 1] == ' ') vname[strlen(vname) - 1] = 0;
		vlen = strlen(vname);

		vptr = findvar(argv[0], V_FIND);							// create the variable
		if(vartype(argv[0]) != T_NBR) error("Numeric variable required");	// sanity check

		// check if the FOR variable is already in the stack and remove it if it is
		// this is necessary as the program can jump out of the loop without hitting
		// the NEXT statement and this will eventually result in a stack overflow
		for(i = 0; i < forindex ;i++) {
			if(forstack[i].var == vptr) {
				while(i < forindex - 1) {
					forstack[i].forptr = forstack[i+1].forptr;
					forstack[i].nextptr = forstack[i+1].nextptr;
					forstack[i].nextptr = forstack[i+1].nextid;
					forstack[i].var = forstack[i+1].var;
					forstack[i].tovalue = forstack[i+1].tovalue;
					forstack[i].stepvalue = forstack[i+1].stepvalue;
					i++;
				}
				forindex--;
				break;
			}
		}

		if(forindex == MAXFORLOOPS) error("Too many nested FOR loops");

		forstack[forindex].var = vptr;								// save the variable index
		*vptr = getnumber(argv[2]);									// get the starting value and save
		forstack[forindex].tovalue = getnumber(argv[4]);			// get the to value and save
		if(argc == 7)
			forstack[forindex].stepvalue = getnumber(argv[6]);		// get the step value
		else
			forstack[forindex].stepvalue = 1;						// default is +1
		
		forstack[forindex].forptr = nextstmt;						// return to here when looping

		// now find the matching NEXT command
		t = 1; tp = p = nextstmt; 
	//	inexit = false;
		while(1) {
			if(*p == 0) {
				if(p[1] == 0) error("FOR without matching NEXT");
				tp = p + 1;
				if(p[1] == T_LINENBR) {
					p += 3;
				}
				p++;
				skipspace(p);
				if(*p == fortoken) t++;								// count the FOR
				if(*p == nexttoken) {								// is it NEXT
					xp = p + 1;										// point to after the NEXT token
					while(*xp && strncasecmp(xp, vname, vlen)) xp++;// step through looking for our variable
					if(*xp && !isnamechar(xp[vlen]))				// is it terminated correctly?
						t = 0;										// yes, found the matching NEXT
					else
						t--;										// no luck, just decrement our stack counter
				}
				if(t == 0) {										// found the matching NEXT
					forstack[forindex].nextptr = tp;				// pointer to the start of the NEXT command
					forstack[forindex].nextid = p + 1;				// pointer to after the NEXT token
					break;
				}
			}
			p++;
		}

		while(*forstack[forindex].nextid == ' ') forstack[forindex].nextid++;	// step over any spaces at the destination

		if((forstack[forindex].stepvalue >= 0 && *vptr > forstack[forindex].tovalue)
						|| (forstack[forindex].stepvalue < 0 && *vptr < forstack[forindex].tovalue)) {
			// loop is invalid at the start, so go to the end of the NEXT command
			tp += 3;					// step over the line number
			skipelement(tp);            // find the command after the NEXT command
			nextstmt = tp;              // this is where we will continue
		} else 
			forindex++;					// save the loop data and continue on with the command after the FOR statement
	}
}



void cmd_next(void) {
	int i, vindex;
	float *vtbl[MAXFORLOOPS];
	int vcnt;
	getargs(&cmdline, MAXFORLOOPS * 2, ",");						// getargs macro must be the first executable stmt in a block

	vindex = 0;														// keep lint happy

	for(vcnt = i = 0; i < argc; i++) {
		if(i & 0x01) {
			if(*argv[i] != ',') error("Illegal character in variable");
		}
		else
			vtbl[vcnt++] = findvar(argv[i], V_FIND | V_NOFIND_ERR);
	}
	
	loopback:
	// first search the for stack for a loop with the same variable specified on the NEXT's line
	if(vcnt) {
		for(i = forindex - 1; i >= 0; i--) 
			for(vindex = vcnt - 1; vindex >= 0 ; vindex--) 
				if(forstack[i].var == vtbl[vindex])
					goto breakout;
	} else {
		// if no variables specified search the for stack looking for an entry with the same program position as
		// this NEXT statement. This cheats by using the cmdline as an identifier and may not work inside an IF THEN ELSE
		for(i = 0; i < forindex; i++)
			if(forstack[i].nextid == cmdline)
				goto breakout;
	}

	error("Cannot find a matching FOR");

	breakout:

	// found a match
	// apply the STEP value to the variable and test against the TO value
	*forstack[i].var += forstack[i].stepvalue;
	if((forstack[i].stepvalue >= 0 && *forstack[i].var > forstack[i].tovalue) || (forstack[i].stepvalue < 0 && *forstack[i].var < forstack[i].tovalue)) {
		// the loop has terminated
		// remove the entry in the table, then skip forward to the next element and continue on from there
		while(i < forindex - 1) {
			forstack[i].forptr = forstack[i+1].forptr;
			forstack[i].nextptr = forstack[i+1].nextptr;
			forstack[i].nextid = forstack[i+1].nextid;
			forstack[i].var = forstack[i+1].var;
			forstack[i].tovalue = forstack[i+1].tovalue;
			forstack[i].stepvalue = forstack[i+1].stepvalue;
			i++;
		}
		forindex--;
		if(vcnt > 0) {
			// remove that entry from our FOR stack
			for(; vindex < vcnt - 1; vindex++) vtbl[vindex] = vtbl[vindex + 1];
			vcnt--;
			if(vcnt > 0)
				goto loopback;
			else
				return;
		}

	} else {
		// we have not reached the terminal value yet, so go back and loop again
		nextstmt = forstack[i].forptr;
	}
}




void cmd_do(void) {
	int i, whileloop;
	char *p, *tp, *evalp;
	char looptoken, whiletoken;

	whiletoken = commandvalue("WHILE") + C_BASETOKEN;
	whileloop = (cmdtoken + C_BASETOKEN == whiletoken);
	if(whileloop)
		looptoken = commandvalue("WEND") + C_BASETOKEN;
	else {
		looptoken = commandvalue("LOOP") + C_BASETOKEN;
		whiletoken = tokenvalue[TKN_WHILE];
	}	

	if(whileloop)
		// if it is a WHILE WEND loop we can just point to the command line
		evalp = cmdline; 
	else {
		// if it is a DO loop find the WHILE token and (if found) get a pointer to its expression
		while(*cmdline && *cmdline != whiletoken) cmdline++;
		if(*cmdline == whiletoken) {
			if(inside_then_else) error ("Cannot run DO WHILE inside THEN ELSE");
			evalp = ++cmdline;
		}
		else
			evalp = NULL;
	}

	// check if this loop is already in the stack and remove it if it is
	// this is necessary as the program can jump out of the loop without hitting
	// the LOOP or WEND stmt and this will eventually result in a stack overflow
	for(i = 0; i < doindex ;i++) {
		if(dostack[i].doptr == nextstmt) {
			while(i < doindex - 1) {
				dostack[i].evalptr = dostack[i+1].evalptr;
				dostack[i].loopptr = dostack[i+1].loopptr;
				dostack[i].loopid = dostack[i+1].loopid;
				dostack[i].doptr = dostack[i+1].doptr;
				i++;
			}
			doindex--;
			break;
		}
	}

	// add our pointers to the top of the stack
	if(doindex == MAXDOLOOPS) error("Too many nested DO or WHILE loops");
	dostack[doindex].evalptr = evalp;
	dostack[doindex].doptr = nextstmt;

	// now find the matching LOOP command
	i = 1; tp = p = nextstmt;
	while(1) {
		if(p[0] == 0) {
			if(p[1] == 0) {
				if(whileloop) 
					error("WHILE without matching WEND");
				else
					error("DO without matching LOOP");
			}
			tp = p + 1;													// point to the start of the line
			if(p[1] == T_LINENBR) {
				p += 3;
			}
			p++;
			skipspace(p);
			if(*p == cmdtoken + C_BASETOKEN) i++;						// entered a nested DO or WHILE loop
			if(*p == looptoken) i--;									// exited a nested loop
			if(i == 0) {												// found our matching LOOP or WEND stmt
				dostack[doindex].loopptr = tp;
				dostack[doindex].loopid = p + 1;
				break;
			}
		}
		p++;
	}

	while(*dostack[doindex].loopid == ' ') dostack[doindex].loopid++;	// step over any spaces at the destination

	if(!whileloop  && dostack[doindex].evalptr != NULL) {
		// if this is a DO WHILE ... LOOP statement
		// search the LOOP statement for a WHILE or UNTIL token (p is pointing to the matching LOOP statement)
		p++;
		while(*p && *p < 0x80) p++;
		if(*p == tokenvalue[TKN_WHILE]) error("Matching LOOP also has a WHILE test");
		if(*p == tokenvalue[TKN_UNTIL]) error("Matching LOOP also has an UNTIL test");
	}

	// do the evaluation (if there is something to evaluate) and if false go straight to the LOOP or WEND statement
	if(dostack[doindex].evalptr != NULL && getnumber(dostack[doindex].evalptr) == 0)
		nextstmt = dostack[doindex].loopptr;

	doindex++;
}




void cmd_loop(void) {
	int tst;
	int i;

	// search the do table looking for an entry with the same program position as this LOOP statement
	for(i = 0; i < doindex ;i++) {
		if(dostack[i].loopid == cmdline) {
			// found a match
			// first check if the DO statement had a WHILE component
			// if not find the WHILE statement here and evaluate it
			if(dostack[i].evalptr == NULL) {						// if it was a DO without a WHILE
				if(*cmdline >= 0x80) {								// if there is something
					if(*cmdline == tokenvalue[TKN_WHILE])
						tst = (getnumber(++cmdline) != 0);			// evaluate the expression
					else if(*cmdline == tokenvalue[TKN_UNTIL])
						tst = (getnumber(++cmdline) == 0);			// evaluate the expression
					else
						error("Invalid syntax");
				} 
				else {
					tst = 1;										// and loop forever
					checkend(cmdline);								// make sure that there is nothing else
				}	
			}
			else {													// if was DO WHILE
				tst = (getnumber(dostack[i].evalptr) != 0);			// evaluate its expression
				checkend(cmdline);									// make sure that there is nothing else
			}	

			// test the expression value and reset the program pointer if we are still looping
			// otherwise remove this entry from the do stack
			if(tst)
				nextstmt = dostack[i].doptr;						// loop again
			else {
				// the loop has terminated
				// remove the entry in the table, then just let the default nextstmt run and continue on from there
				while(i < doindex - 1) {
					dostack[i].evalptr = dostack[i+1].evalptr;
					dostack[i].loopptr = dostack[i+1].loopptr;
					dostack[i].loopid = dostack[i+1].loopid;
					dostack[i].doptr = dostack[i+1].doptr;
					i++;
				}
				doindex--;
				// just let the default nextstmt run
			}
			return;	
		}
	}
	error("LOOP without a matching DO");
}



void cmd_exit(void) {
	char fortoken;
	fortoken = tokenvalue[TKN_FOR];

	// look for the FOR token and if found set the program pointer to the NEXT pointer on the for stack
	// otherwise (if the token is not found) set the pointer to the LOOP pointer on the do stack
	while(*cmdline && *cmdline != fortoken) cmdline++;	
	if(*cmdline == fortoken) {
		cmdline++;
		if(forindex == 0) error("No FOR loop is in effect");
		nextstmt = forstack[--forindex].nextptr;
	} else {
		if(doindex == 0) error("No DO loop is in effect");
		nextstmt = dostack[--doindex].loopptr;
	}
	checkend(cmdline);
	nextstmt += 3;
	skipelement(nextstmt);
}



void cmd_error(void) {
	if(*cmdline)
		error(getCstring(cmdline));
	else
		error("");
}



void cmd_randomize(void) {
	int i;
	i = getnumber(cmdline);
	if(i < 0) error("Number out of bounds");
	srand(i);
}



void cmd_gosub(void) {
	if(gosubindex >= MAXGOSUB) error("Too many nested GOSUB");
	gosubstack[gosubindex++] = nextstmt;
	nextstmt = findline(getinteger(cmdline), true);
}



void cmd_return(void) {
	checkend(cmdline);
	if(gosubindex == 0) error("No GOSUB to return to");
	nextstmt = gosubstack[--gosubindex];
}



void cmd_read(void) {
	int i, j, dnbr, linenbr, nbrvalues;
	char *p, datatoken;
	char *vtbl[MAX_ARG_COUNT];
	int vtype[MAX_ARG_COUNT];
	int vcnt, vidx;
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// getargs macro must be the first executable stmt in a block

	if(argc == 0) error("No variables to read");

	// step through the arguments and save the pointer and type
	for(vcnt = i = 0; i < argc; i++) {
		if(i & 0x01) {
			if(*argv[i] != ',') error("Expected a comma");
		}
		else {
			vtbl[vcnt] = findvar(argv[i], V_FIND);
			vtype[vcnt] = vartype(argv[i]);
			vcnt++;
		}
	}
	
	// setup for a search through the whole memory
	linenbr = vidx = dnbr = 0;
	datatoken = commandvalue("DATA") + C_BASETOKEN;
	p =pmemory;

	// search looking for a DATA statement.  We keep returning to this point until all the data is found
search_again:
	while(1) {
		if(p[0] == 0) {
			if(p[1] == 0) error("No more DATA to read");			// end of the program and we still need more data	
			if(p[1] == T_LINENBR) {
				linenbr = (p[2] << 8) | p[3];						// get the line number incase of error reading the DATA stmt
				p += 3;
			}
			p++;
			skipspace(p);
			if(*p == datatoken) break;								// found a DATA statement
		}
		p++;
	}
	p++;															// step over the token

	{	// we have a DATA statement, first split the line into arguments
		// new block, the getargs macro must be the first executable stmt in a block
		getargs(&p, (MAX_ARG_COUNT * 2) - 1, ",");
		// check how much data is here and if not look for another DATA stmt
		nbrvalues = (argc / 2) + 1;									// number of values in the DATA statement
		if(dnbr + nbrvalues <= NextData) {
			dnbr += nbrvalues;			
			goto search_again;
		}
		i = (NextData - dnbr) * 2;
		// now step through the variables on the READ line and get their new values from the argument list
		// we set the line number to the number of the DATA stmt so that any errors are reported correctly
		while(vidx < vcnt) {
			j = CurrentLineNbr;
			CurrentLineNbr = linenbr;
			if(vtype[vidx] == T_STR) {
				if(*argv[i] == '"') {								// if quoted string
					argv[i]++;										// step over the quote
					*(strchr(argv[i], '"')) = 0;					// replace the closing quote with a zero char
				} 
				strcpy(vtbl[vidx], argv[i]);						// get the string
				CtoM(vtbl[vidx]);									// convert to a MMBasic string
			}
			else
				*((float *)vtbl[vidx]) = getnumber(argv[i]);		// much easier if numeric variable

			NextData++;
			dnbr++;
			vidx++;
			i++;
			if(i < argc && *argv[i] != ',') error("Expected a comma");
			CurrentLineNbr = j;
			i++;
			if(vidx < vcnt && i >= argc) goto search_again;			// need more data?  go back and look for more
		}
	}
}



void cmd_restore(void) {
	checkend(cmdline);
	NextData = 0;
}



void cmd_lineinput(void) {
	char *vp;
	int i, fnbr;
	getargs(&cmdline, 3, ",;");										// this is a macro and must be the first executable stmt
	if(argc > 0 && (vp = checkstring(argv[0], "INPUT"))) {			// check if this was redirected from the LINE command
		skipspace(vp);
		argv[0] = vp;												// and, if so remove the word LINE from the first arg
	}	
	if(argc == 0 || argc == 2) error("Invalid syntax");
	
	i = 0;
	fnbr = 0;
	if(argc == 3) {
		// is the first argument a file number specifier?  If so, get it
		if(*argv[0] == '#' && *argv[1] == ',') {
			argv[0]++;
			fnbr = getinteger(argv[0]);
		}
		else {
			// is the first argument a prompt?  if so, print it otherwise there are too many arguments
			if(*argv[1] != ',' && *argv[1] != ';') error("Invalid syntax");
			MMfputs(getstring(argv[0]), 0);
		}
	i = 2;
	}

	if(argc - i != 1) error("Invalid syntax");
	vp = findvar(argv[i], V_FIND);
	if(vartype(argv[i]) != T_STR) error("String variable required");
	*inpbuf = 0;													// start with an empty buffer
	if(fnbr == 0)
		EditInputLine(0);											// if keyboard get the line with editing
	else
		MMgetline(fnbr, inpbuf);									// otherwise use the old way of getting the input line											
	strcpy(vp, inpbuf);
	CtoM(vp);														// convert to a MMBasic string
}



void cmd_delete(void) {
	char ss[2];
	char *p1, *p2;
	ss[0] = GetTokenValue("-");										// this will be used to split up the argument line
	ss[1] = 0;														// blame Microsoft for a poor choice of syntax
	{																// start a new block
		getargs(&cmdline, 4, ss);									// getargs macro must be the first executable stmt in a block
		
		if(argc == 1) {
			p2 = p1 = findline(getinteger(argv[0]), true);			// this is a single line number eg: DELETE 340
			p2 += 3;
			skipline(p2);
		}
		else if(argc == 2) {
			if(*argv[0] == *ss) {
				p1 = pmemory + 1;
				p2 = findline(getinteger(argv[1]), true);			// this is DELETE -340
				p2 += 3;
				skipline(p2);
			}
			else if(*argv[1] == *ss) {
				p1 = findline(getinteger(argv[0]), true);			// this is DELETE 230-
				p2 = pmemory + psize;
			}
			else
				error("Invalid syntax");
		}
		else if(argc == 3) {										// this is DELETE 230-340
			p1 = findline(getinteger(argv[0]), true);
			p2 = findline(getinteger(argv[2]), true);
			p2 += 3;
			skipline(p2);
		}
		else
			error("Invalid syntax");
		
		// delete the lines and update the program size counter
		memmove(p1, p2, psize - (p2 - pmemory));
		psize -= (p2 - p1);
		pmemory[psize] = pmemory[psize + 1] = pmemory[psize + 2] = pmemory[psize + 3] = 0;// ensure that the last four are zero
		longjmp(mark, 1);											// jump back to the input prompt
	}
}



void cmd_on(void) {
	int r, i;
	char ss[4];														// this will be used to split up the argument line

	ss[0] = tokenvalue[TKN_GOTO];
	ss[1] = tokenvalue[TKN_GOSUB];
	ss[2] = ',';
	ss[3] = 0;
	{																// start a new block
		getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ss);				// getargs macro must be the first executable stmt in a block
		if(argc < 3 || !(*argv[1] == ss[0] || *argv[1] == ss[1])) error("Invalid syntax");
		if(argc%2 == 0) error("Invalid syntax");

		r = getinteger(argv[0]);									// evaluate the expression controlling the statement
		if(r < 0 || r > 255) error("Number out of range");
		if(r == 0 || r > argc/2) return;							// microsoft say that we just go on to the next line
		i =  getinteger(argv[r*2]);									// get the target line

		if(*argv[1] == ss[1]) {
			// this is a GOSUB, same as a GOTO but we need to first push the return pointer
			if(gosubindex >= MAXGOSUB) error("Too many nested GOSUB");
			gosubstack[gosubindex++] = nextstmt;
		}

		nextstmt = findline(i, true);
	}
}



void cmd_dim(void) {
	int i;
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");			// getargs macro must be the first executable stmt in a block

	if((argc & 0x01) == 0) error("Invalid syntax");

	for(i = 0; i < argc; i += 2)
		findvar(argv[i], V_FIND | V_DIM_ARRAY);
	
	DimUsed = true;
}



void cmd_erase(void) {
	int i,j,k, u;
	char p[MAXVARLEN + 1];
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");			// getargs macro must be the first executable stmt in a block

	if((argc & 0x01) == 0) error("Invalid syntax");

	for(i = 0; i < argc; i += 2) {
		strcpy(p, argv[i]);
		strcat(p, "(");
		for(j = 0; j < varcnt; j++) {
			if(str_equal(p, vartbl[j].name)  && vartbl[j].dims[0] != 0) {
				free(vartbl[j].val.s);								// free the memory
				// this is messy but we have to figure out how much memory was freed and update HeapUsed
				u = vartbl[j].dims[0] - (OptionBase?1:0);
				for(k = 1; k < 8; k++) u *= vartbl[j].dims[k] - (OptionBase?1:0);
				if( vartbl[j].type == T_NBR)
					HeapUsed -= u * sizeof(float);
				else
					HeapUsed -= u * STRINGSIZE;
				vartbl[j].type = T_NOTYPE;
				*vartbl[j].name = 0;
				for(k = 0; k < MAXDIM; k++) vartbl[j].dims[k] = 0;
				if(j == varcnt - 1) { j--; varcnt--; }
				break;
			}
		}
		if(j == varcnt) error("Cannot find variable");
	}
}



void cmd_option(void) {
	char *tp;
	int i;
	
	tp = checkstring(cmdline, "BASE");
	if(tp) {
		if(DimUsed) error("OPTION BASE must be before DIM");
		OptionBase = getinteger(tp);
		if(OptionBase < 0 || OptionBase > 1) error("Number out of range");
		return;
	}
	
	tp = checkstring(cmdline, "USB");
	if(tp) {
		if(checkstring(tp, "ON")) {
				USBOn = true;
				return;
			}	
		if(checkstring(tp, "OFF")) {
				USBOn = false;
				return;
			}	
	}	
	
	tp = checkstring(cmdline, "VIDEO");
	if(tp) {
		if(checkstring(tp, "ON")) {
				VideoOn = true;
				return;
			}	
		if(checkstring(tp, "OFF")) {
				VideoOn = false;
				return;
			}	
	}	
	
	tp = checkstring(cmdline, "ERROR");
	if(tp) {
		if(checkstring(tp, "CONTINUE")) {
				OptionErrorAbort = false;
				return;
			}	
		if(checkstring(tp, "ABORT")) {
				OptionErrorAbort = true;
				return;
			}	
	}	
	
	tp = checkstring(cmdline, "PROMPT");
	if(tp) {
		skipspace(tp);
		if(strlen(tp) >= MAXPROMPTLEN) error("String too long");
		getstring(tp);												// check for any expression errors
		strcpy(PromptString, tp);									// and save for later evaluation
		return;
	}
	
	// check for a programmable function key
	if(toupper(*cmdline) == 'F') {
		for(i = 1; i <= NBRPROGKEYS; i++) {
			if(atoi(cmdline+1) == i) {								// is this a match to a key number
				cmdline += 2 + (i < 10 ? 0:1);						// step over the number
				skipspace(cmdline);
				tp = getCstring(cmdline);							// get the string
				if(strlen(tp) > MAXKEYLEN) error("String too long");
				strcpy(FunKey[i - 1], tp);							// save into our array
				return;
			}	
		}
	}			
	
	error("Unrecognised option");
}	



void cmd_memory(void) {
	checkend(cmdline);
	sprintf(inpbuf, "%5dkB (%2d%%) Program memory used\r\n", (psize + 512)/1024, (psize * 100)/PROGMEMSIZE); 
	MMPrintString(inpbuf);
	sprintf(inpbuf, "%5dkB (%2d%%) Variable memory used\r\n", ((varcnt * (8 + MAXVARLEN + MAXDIM * 2)) + 512)/1024 , (varcnt * 100)/MAXVARS); 
	MMPrintString(inpbuf);
	if(HeapUsed < 0) HeapUsed = 0;
	sprintf(inpbuf, "%5dkB (%2d%%) Array and string memory used\r\n", (HeapUsed + 512)/1024, (HeapUsed * 100) / (38 * 1024)); 
	MMPrintString(inpbuf);
}


// this function is only used by cmd_renumber() below
// it will first copy to the output buffer the character pointed to by p if it is not numeric
//   then it will get the old number pointed to by p + 1
//   then search the program to find that number while calculating the new number
//   when found will copy the new number to the output buffer
//   and return with the pointer arguments (to the output buffer and program) updated
void GetNewNumber(char **tpp, char **pp, int currentline, int first, int incr, int start) {
	int i, j;
	char *tp, *p, *xp;
	
	tp = *tpp;  p = *pp;											// this makes pointer arithmetic easier to debug
	
	if(!isdigit(*p)) *tp++ = *p++;									// copy the token or character and step over it
	while(*p == ' ') *tp++ = *p++;									// copy any spaces to the buffer and skip over them
	i = 0;
	while(isdigit(*p)) i = (i * 10) + (*p++ - '0');					// get the following number
	if(i < 1 || i >= MAXLINENBR) {
		CurrentLineNbr = currentline;
		error("Invalid line number. Cancelling RENUMBER");
	}	
	if(i <  start)
		tp += sprintf(tp, "%d", i);									// and copy to the buffer
	else {
		xp = pmemory;												// xp is used to scan program memory
		while(1) {
			if(xp[0] == 0) {										// start of a new command
				if(xp[1] == 0) {									// end of program
					CurrentLineNbr = currentline;
					error("Line number does not exist. Cancelling RENUMBER");
				}	
				if(xp[1] == T_LINENBR) {							// this command has a line number
					j = (xp[2] << 8) | xp[3];						// get the number
					if(j == i) break;								// success
					if(j >= start) first += incr;					// otherwise keep track of the new number if past the start
					xp += 3;										// and step over this number
				}	
			}	
			xp++;
		}
		tp += sprintf(tp, "%d", first);								// and copy to the buffer
	}	
	*tpp = tp;  *pp = p;											// update the returned pointers
}



void cmd_renumber(void){
	int first = 10,
	    incr = 10,
	    start = 1;
	    
	char gotocommand, gosubcommand, oncommand, setpincommand, settickcommand, ifcommand;
	char thentoken, elsetoken, gototoken, gosubtoken;
	char *p, *sp, *tp;
	int i, j, currentline = 0, secondrun;
	int renumb, lcnt, ccnt;
	
	getargs(&cmdline, 5, ",");
	p = pmemory;
	if(p[1] == 0) error("No program to renumber");

	if(argc > 0 && argc%2 == 0) error("Invalid syntax");
	if(argc >= 1) first = getinteger(argv[0]);
	if(argc >= 3) incr = getinteger(argv[2]);
	if(argc == 5) start = getinteger(argv[4]);
	
	if(first < 1 || incr < 1 || start < 1) error("Invalid argument");
	if(first < start) error("Cannot make the first line less than the start line");
	
	gotocommand = commandvalue("GOTO") + C_BASETOKEN;
	gosubcommand = commandvalue("GOSUB") + C_BASETOKEN;
	oncommand = commandvalue("ON") + C_BASETOKEN;
	setpincommand = commandvalue("SETPIN") + C_BASETOKEN;
	settickcommand = commandvalue("SETTICK") + C_BASETOKEN;
	ifcommand = commandvalue("IF") + C_BASETOKEN;
	thentoken = tokenvalue[TKN_THEN];
	elsetoken = tokenvalue[TKN_ELSE];
	gototoken = tokenvalue[TKN_GOTO];
	gosubtoken = tokenvalue[TKN_GOSUB];
	
	secondrun = false;
	
	// this loop executes twice.  The first is a dummy run, the second actually changes memory
	// this is done so that any error will be found in the first run leaving program memory unchanged
	while(1) {
		p = pmemory;
		lcnt = ccnt = 0;
		
		// this loop executes once for every line in the program
		while(1) {
			if(p[1] == 0) break;									// end of program
			if(p[1] != T_LINENBR) error("Corrupt program memory. Cancelling RENUMBER");// we must be at the start of a line at this point
			*p++;													// step over the start of line marker (ie, zero)
			tp = tknbuf;											// prepare for copying the line into the buffer
			sp = p;													// needed later to get the line length
			renumb = false;
			p++;													// step over the marker
			currentline = (p[0] << 8) | p[1];						// get the number
			//dp("%d\r\n", currentline);
			*tp++ = T_LINENBR; *tp++ = *p++; *tp++ = *p++;			// also copy into the buffer
			
			// this loop executes once for every command in the line (commands are separated by a colon or zero byte)
			while(1) {
				while(*p == ' ') *tp++ = *p++;						// skip spaces and copy to the buffer
				
				// now check the possible commands
				if(*p == gotocommand || *p == gosubcommand) {		// GOTO or GOSUB
					GetNewNumber(&tp, &p, currentline, first, incr, start);	// get the target number, renumber it, copy to buffer
					ccnt++;											// and count that we have renumbered a reference
					renumb = true;
				} else
				
				if(*p == oncommand) {								// ON
					*tp++ = *p++;
					while(*p && !(*p == gototoken || *p == gosubtoken)) 
						*tp++ = *p++;								// skip up to the GOTO or GOSUB token
					if(!*p) {
						CurrentLineNbr = currentline;
						error("Invalid syntax. Cancelling RENUMBER");
					}	
					do {
						GetNewNumber(&tp, &p, currentline, first, incr, start);	// get the target number, renumber it, copy to buffer
						ccnt++;										// and count that we have renumbered a reference
						renumb = true;
						while(*p && *p != ',' && *p != '\'') *tp++ = *p++;// search for the next comma or null byte or comment
					} while(*p == ',');								// keep going while we have a comma
				} else 
				
				if(*p == setpincommand || *p == settickcommand) {	// SETPIN or SETTICK
					i = 1;											// the line nbr is the second arg for SETTICK
					if(*p == setpincommand) i++;					// and the third arg for SETPIN
					while(i > 0 && *p) {							// skip commas while copying into the buffer
						if(*p == ',') i--;
						*tp++ = *p++;
					}
					if(*p && atoi(p) != 0) {						// null means that SETPIN only had two arguments
																	// atoi(p) == 0 is valid in SETTICK and should not be renumbered
						GetNewNumber(&tp, &p, currentline, first, incr, start);	// get the target number, renumber it, copy to buffer
						renumb = true;
						ccnt++;										// and count that we have renumbered a reference
					}	
				} else 
				
				if(*p == ifcommand) {								// IF ... THEN ... ELSE
					do {
						while(*p && !(*p == thentoken || *p == elsetoken || *p == gototoken)) *tp++ = *p++;
						if(*p) {
							*tp++ = *p++;							// copy the token
							while(*p == ' ') *tp++ = *p++;			// skip spaces and copy to the buffer
							if(isdigit(*p) || *p == gotocommand || *p == gosubcommand) {
								GetNewNumber(&tp, &p, currentline, first, incr, start); // get the target number, renumber it, copy to buffer
								renumb = true;
								ccnt++;								// and count that we have renumbered a reference
							}
							if(*p) *tp++ = *p++;
						}	
					} while(*p);		
				}
				
				while(*p) *tp++ = *p++;								// copy the rest of the command
				
				// this is the end of a command

				if(p[1] == 0 || p[1] == T_LINENBR) {				// check if this is the end of the line
					if(renumb && secondrun) {						// if this is the second run and therefor no errors were found
						*tp++ = 0; *tp++ = 0;						// terminate with two zero bytes
						if(secondrun) {								
							p += GetLineLength(tknbuf) - GetLineLength(sp);// adjust p for the change in program length
							AddProgramLine();						// replace the line in memory with tokenbuf
						}
					}
					break;
				} else
					*tp++ = *p++;									// must be a command separator, copy it and loop
			}														// end loop for each command
		}															// end loop for each line
		
		// all the line number references in the program have been renumbered
		// now find each line and renumber the actual lines one by one
		p = pmemory;
		j = first;
		while(1) {
			if(p[0] == 0) {										// if a new command
				if(p[1] == 0) break;							// terminate loop if end of program
				if(p[1] == T_LINENBR) {							// got a line number
					i = (p[2] << 8) | p[3];
					if(i >= start) {							// only start renumbering after the start nbr
						if(secondrun) {
							p[2] = j >> 8;					// renumber it if this is the second run through
							p[3] = j & 0xff;
						}	
						j += incr;							// the next line number
						if(j  >= MAXLINENBR) {
							sprintf(tknbuf, "Maximum line number is %d. Cancelling RENUMBER", MAXLINENBR - 1);
							error(tknbuf);
						}	
						lcnt++;									// and count the nbr of lines renumbered
					}	
					p += 3;										// step over the line number
				}
			}
			p++;
		}			
		sprintf(inpbuf, "Renumbered %d lines and %d references\r\n", lcnt, ccnt); 
		if(secondrun) {
			MMPrintString(inpbuf);
			longjmp(mark, 1);										// jump back to the input prompt
			// this line is never executed
		}	
		secondrun = true;											// setup for the second run
	}																// end loop for test or execute
}


void cmd_edit(void) {
	int line;
	
	if(*cmdline == 0) {												// if the user wants to edit the last line entered
		strcpy(inpbuf, lastcmd);									// get the line
		line = 0;
	} else {	
		llist(inpbuf, findline(line = getinteger(cmdline), true));	// else find and expand the line
	}	

	EditInputLine(line);
	ProcessInputLine();												// and execute the line
}	


	
void cmd_poke(void) {
	getargs(&cmdline, 5, ",");
	if(argc != 5) error("Invalid syntax");
	*(char *)((getinteger(argv[0]) << 16) + getinteger(argv[2])) = getinteger(argv[4]);
}	



void cmd_auto(void) {
	getargs(&cmdline, 3, ",");
	if(argc == 2) error("Invalid syntax");
	if(argc > 0) autoNext = getinteger(argv[0]);
	if(argc == 3) autoIncr = getinteger(argv[2]);
	autoOn = true;
}




/***********************************************************************************************
utility functions used by the various commands
************************************************************************************************/

// merge a file into program memory
void mergefile(char *fname) {
	char *tp;
	signed char c;
	int i;

	if(strchr(fname, '.') == NULL) strcat(fname, ".BAS");
	CloseAllFiles();
	MMfopen(fname, "r", 1);

	tp = inpbuf;
	i = 0;
	while(1) {
		if(MMfeof(1)) break;	
		c = MMfgetc(1);

		if(c == '\t') {												// expand tabs to spaces
			while(((++i) % 8) && i <= MAXSTRLEN) 
				*tp++ = ' ';
		}
		
		if(i > MAXSTRLEN) 
			error("Line is too long");

		if((c == '\r' || c == '\n' || MMfeof(1)) && i != 0) {	
			*tp = 0;												// terminate the buffer
			tokenise();												// do some magic
			if(*tknbuf == T_LINENBR) AddProgramLine();				// add the line to the program space
			tp = inpbuf;											// setup for the next line
			i = 0;
		}

		if(isprint(c)) {
			*tp++ = c;												// save in the input buffer
			i++;
		}
	}
	MMfclose(1);
}



// list a line into a buffer (b) given a pointer to the beginning of the line (p).
// this is used by flist() and cmd_edit()
char *llist(char *b, char *p) {
	int i, firstnonwhite = true;
	while(1) {
		if(*p == T_LINENBR) {
			i = (((p[1]) << 8) | (p[2]));							// get the line number
			p += 3;													// and step over the number
			b += sprintf(b, "%d", i);
			if(*p != ' ') *b++ = ' ';
			firstnonwhite = true;
			continue;
			}
		
		if(*p >= C_BASETOKEN) {
			if(firstnonwhite) {
				if(*p - C_BASETOKEN != commandvalue("LET")) 
					strcpy(b, commandname(*p));						// expand the command (if it is not LET)
				firstnonwhite = false;
			} else {												// not a command so must be a token
				strcpy(b, tokenname(*p));							// expand the token
				if(*p == tokenvalue[TKN_THEN] || *p == tokenvalue[TKN_ELSE]) 
					firstnonwhite = true;
				else
					firstnonwhite = false;
			}
			b += strlen(b);											// update pointer to the end of the buffer
			p++;
			continue;
		}	
		
		// hey, an ordinary char, just copy it to the output
		if(*p) {
			*b = *p;												// place the char in the buffer
			if(*p != ' ') firstnonwhite = false;
			p++;  b++;												// move the pointers
			*b = 0;													// terminate the output buffer
			continue;
		}	
	
		// at this point the char must be a zero
		// zero char can mean both a separator or end of line
		if(!(p[1] == T_LINENBR || p[1] == 0)) {	
			*b++ = ':';												// just a separator
			*b = 0;													// terminate the output buffer
			firstnonwhite = true;
			p++;
			continue;
		}	
				
		// must be the end of a line - so return to the caller
		return ++p;
	} // end while
}



// lists the program to a specified file handle
// this decodes line numbers and tokens and outputs them in plain english
// LISTing a program is exactly the same as listing to a file (ie, SAVE)
void flist(int fnbr, int fromnbr, int tonbr) {
	char *fromp,c;
	char b[STRINGSIZE];
	int i;

	fromp = findline(fromnbr, (fromnbr == tonbr) ? true : false);	// set our pointer to the start line
	ListCnt = 1;
	
	while(1) {
		
		if(*fromp == T_LINENBR) {
			i = (((fromp[1]) << 8) | (fromp[2]));			// get the line number
			if(i > tonbr) break;					// end of the listing
			fromp = llist(b, fromp);				// otherwise expand the line
			MMfputs(CtoM(b), fnbr);					// convert to a MMBasic string and output
			MMfputs("\2\r\n", fnbr);				// this is a MMBasic string
			// check if it is more than a screenfull
			if(fnbr == 0 && ListCnt >= (VRes / (fontHeight * fontScale)) && !(fromp[0] == 0 && fromp[1] == 0)) {
				MMPrintString("PRESS ANY KEY OR Q TO STOP");
				c=MMgetchar();
				MMPrintString("\r                          \r");
                                if(c=='q' || c=='Q')  longjmp(mark, 1); // jump back to the input prompt added KSD
                                else
                                ListCnt = 1;
			}	
		}
		//else
		//	error("Internal error in flist()");
		
		// finally, is it the end of the program?
		if(fromp[0] == 0) break;	
	}	
	if(fnbr != 0) MMfclose(fnbr);
}



void execute_one_command(char *p) {
	inside_then_else = true;

	CheckAbort();
	targ = T_CMD;
	pp = p;
	skipspace(p);													// skip any whitespace
	if(*p >= C_BASETOKEN && *p - C_BASETOKEN < CommandTableSize - 1 && (commandtbl[*p - C_BASETOKEN].type & T_CMD)) {
		cmdtoken = *pp;
		cmdline = p + 1;
		commandtbl[*pp - C_BASETOKEN].fptr();							// execute the command
	} else {
		cmdtoken = commandvalue("LET") + C_BASETOKEN;
		cmdline = p;
		commandtbl[cmdtoken - C_BASETOKEN].fptr();					// execute the command
	}
	//pp = nextstmt;
	ClearTempStringSpace();												// at the end of each command we need to clear any temporary string vars

	inside_then_else = false;
}
	
