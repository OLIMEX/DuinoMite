/***********************************************************************************************************************
MMBasic

MMBasic.c

Provides the core functions used in MMBasic.  These include parsing the command line and converting the key
words into tokens, storage and management of the program in memory, storage and management of variables,
the expression execution engine and other useful functions.

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
#include "MMBasic.h"
#include "../Maximite.h"
#include "../Video/Video.h"
#include "../Keyboard/Keyboard.h"
#include "../Timers/Timers.h"
#include "../DuinoMite/RTC.h"

#define INCLUDE_FUNCTION_DEFINES
#include "Functions.h"
#include "Commands.h"
#include "Operators.h"
#include "Files.h"
#include "External.h"
#include "Graphics.h"
#include "Misc.h"
#include "Custom.h"
#include "I2C.h"
#include "XModem.h"
#include "editor.h"
#include "Term.h"
#include "Setup.h"
#include "Help.h"
#include "../DuinoMite/RTC.h"
#include "../DuinoMite/GameDuino.h"
#undef INCLUDE_FUNCTION_DEFINES


// this is the command table that defines the various tokens for commands in the source code
// most of them are listed in the .h files so you should not add your own here but instead add
// them to the appropiate .h file
#define INCLUDE_COMMAND_TABLE
struct s_tokentbl commandtbl[] = {
#include "Functions.h"
#include "Commands.h"
#include "Operators.h"
#include "Files.h"
#include "External.h"
#include "Graphics.h"
#include "Misc.h"
#include "Custom.h"
#include "I2C.h"
#include "XModem.h"
#include "editor.h"
#include "Term.h"
#include "Setup.h"
#include "Help.h"
#include "../DuinoMite/RTC.h"
#include "../DuinoMite/GameDuino.h"
    { "", 0, 0, cmd_null,} // this dummy entry is always at the end
};
#undef INCLUDE_COMMAND_TABLE



// this is the token table that defines the other tokens in the source code
// most of them are listed in the .h files so you should not add your own here
// but instead add them to the appropiate .h file
#define INCLUDE_TOKEN_TABLE
struct s_tokentbl tokentbl[] = {
#include "Functions.h"
#include "Commands.h"
#include "Operators.h"
#include "Files.h"
#include "External.h"
#include "Graphics.h"
#include "Misc.h"
#include "Custom.h"
#include "I2C.h"
#include "Setup.h"
#include "XModem.h"
#include "../DuinoMite/RTC.h"
#include "../DuinoMite/GameDuino.h"
    { "", 0, 0, cmd_null,} // this dummy entry is always at the end
};
#undef INCLUDE_TOKEN_TABLE

// these are initialised at startup
int CommandTableSize, TokenTableSize;

struct s_vartbl vartbl[MAXVARS];
int varcnt;
int VarIndex;

int HeapUsed; // used to track the amount of the heap used.  For the memory command.
char *strtmp[MAXTEMPSTRINGS]; // used to track temporary string space on the heap

unsigned char FunKey[NBRPROGKEYS][MAXKEYLEN + 1]; // data storage for the programmable function keys

jmp_buf mark; // longjump to recover from an error
char inpbuf[STRINGSIZE]; // used to store user keystrokes until we have a line
char tknbuf[STRINGSIZE]; // used to store the tokenised representation of the users input line
char lastcmd[STRINGSIZE]; // used to store the last command in case it is needed by the EDIT command

char pmemory[PROGMEMSIZE]; // program memory, this is where the program is stored
int psize; // the size of the program stored in pmemory[]
char *pp; // global program pointer used by many routines

int TraceOn; // used to track the state of TRON/TROFF
int NextData; // used to track the next item to read in DATA & READ stmts
int OptionBase; // track the state of OPTION BASE


#ifdef TEST_STACK_OVERFLOW
// experimental and for the PIC32 only
unsigned int StackLimit;
#define MAX_STACK_SIZE		(6 * 1024 - 300)
#endif


///////////////////////////////////////////////////////////////////////////////////////////////
// Global information used by operators
//
int targ; // the type of the arguments being passed
float farg1, farg2, fret; // the two float arguments and returned value
char *sarg1, *sarg2, *sret; // the two string arguments and returned value

////////////////////////////////////////////////////////////////////////////////////////////////
// Global information used by functions
// functions use targ, fret and sret as defined for operators (above)
char *ep; // pointer to the argument to the function terminated with a zero byte.
// it is NOT trimmed of spaces

////////////////////////////////////////////////////////////////////////////////////////////////
// Global information used by commands
//
int cmdtoken; // Token number of the command
char *cmdline; // Command line terminated with a zero char and trimmed of spaces
char *nextstmt; // Pointer to the next statement to be executed.
int CurrentLineNbr; // Line number for error reporting (zero if in immediate mode)
char *ContinuePoint; // Where to continue from if using the continue statement
char PromptString[MAXPROMPTLEN] = "\"> \""; // the prompt for input

// in ver 2.7 about 2000 bytes of RAM and 22K of flash is spare for future additions

void initbasic(void);
void getexpr(char *);
void checktype(int *, int);
char *doexpr(char *p, float *fa, char **sa, int *oo, int *t);
char *getvalue(char *p, float *fa, char **sa, int *oo, int *t);
void ClearTempStringSpace(void);
void ExecuteProgram();

extern void SetTokenTableSize(void);

int tokencnt;
unsigned char tokenvalue[TOKEN_LOOKUP_SIZE]; // see InitBasic() below

int LastErrorLine = 0;
int autoOn, autoNext = 10, autoIncr = 10; // use by the AUTO command

extern int SupressVideo; // dont print to video

/********************************************************************************************************************************************
 Program mamagement
 Includes the routines to initialise MMBasic, start running the interpreter, and to run a program in memory
 *********************************************************************************************************************************************/


// Initialise MMBasic

void InitBasic(void) {
    CommandTableSize = (sizeof (commandtbl) / sizeof (struct s_tokentbl));
    TokenTableSize = (sizeof (tokentbl) / sizeof (struct s_tokentbl));

    if (CommandTableSize >= 128) error("Command table too large");
    if (TokenTableSize >= 128) error("Token table too large");

    dp("Command table entries free: %d", 127 - CommandTableSize);
    dp("Token table entries free: %d\r\n", 127 - TokenTableSize);

    varcnt = 0;

    pmemory[0] = pmemory[1] = pmemory[2] = pmemory[3] = 0; // ensure the end of program is well marked
    psize = 1;

    TraceOn = false;
    clearvars();
    clearstack();

    // load the tokenvalue table with commonly used tokens
    // by placing them into a lookup table performance is improved considerably
    tokenvalue[TKN_THEN] = GetTokenValue("THEN");
    tokenvalue[TKN_ELSE] = GetTokenValue("ELSE");
    tokenvalue[TKN_GOTO] = GetTokenValue("GOTO");
    tokenvalue[TKN_EQUAL] = GetTokenValue("=");
    tokenvalue[TKN_TO] = GetTokenValue("TO");
    tokenvalue[TKN_STEP] = GetTokenValue("STEP");
    tokenvalue[TKN_WHILE] = GetTokenValue("WHILE");
    tokenvalue[TKN_UNTIL] = GetTokenValue("UNTIL");
    tokenvalue[TKN_FOR] = GetTokenValue("FOR");
    tokenvalue[TKN_GOSUB] = GetTokenValue("GOSUB");
    tokenvalue[TKN_AS] = GetTokenValue("AS");
    // IMPORTANT
    // If you add to this table you must change TOKEN_LOOKUP_SIZE in MMBasic.h
}



// Start running MMBasic
// this repeatedly gets command lines from the user and executes them
// this function never returns

void __attribute__((noreturn)) MMBasicStart(void) {
    static int FirstTimeRun = true;
    char autorun[] = "AUTORUN.BAS";
    int r = 0;

#ifdef TEST_STACK_OVERFLOW										// experimental and for the PIC32 only
    register unsigned int msp asm("sp");
    StackLimit = msp - MAX_STACK_SIZE;
#endif

    InitBasic();
#ifdef MAXIMITE
   ExtSet(0, 1); // turn on the power LED
#endif
    while (1) {
        if (setjmp(mark) == 0) { // return to here on error or when we want to halt execution
            ShowCursor(false); // just in case it was left on after a CTRL-C
            if (MMAbort) autoOn = false;
            MMAbort = false; // MMAbort is set on CTRL-C
            FileXfr = false; // obviously we are not doing a file transfer
            while (MMInkey() != -1); // clear the input buffer
            // if this is the first time that MMBasic has been run check for AUTORUN and AUTOKEYS
            if (FirstTimeRun) {
                FirstTimeRun = false; // we only run this code once
                ReadRTC();
                if(S.DTimeDate) PrintDateTime();
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // look for AUTORUN.BAT
                if (Autorun) { // run only if we are NOT recovering from an exception
                    DefaultDrive = FLASHFS;
                    r = (FindFlashBlock(autorun, 1) != NULL);
                    if (!r) {
#ifdef UBW32
                        if (InitSDCard() == true)
#endif
#ifdef OLIMEX
                        if (InitSDCard() == true)
#endif
                        {
                            DefaultDrive = SDFS;
                            OptionErrorAbort = false; // do not flag an error if the file does not exist
                            r = SDFileExists(autorun); // and check for the autorun file on the SDCard fs
                            OptionErrorAbort = true; // restore normal file error handling
                        }
                    }
                    if (r) {
                        mergefile(autorun); // load the program
                        nextstmt = pmemory;
                        ExecuteProgram(); // and execute it
                    }
                }
            }

#ifdef PROFILE
            StopProfiling();
#endif

            InterruptReturn = NULL;
            SetFont(0, 1, 0); // set a reasonable default font
            Cursor = C_STANDARD;
            USBOn = S.UsbEnable;
            VideoOn = S.VideoMode;
            //                        USBOn = VideoOn = true;									// turn on the outputs;
            if (MMCharPos > 1) MMPrintString("\r\n"); // prompt should be on a new line
            if ((MMPosY % (fontHeight * fontScale)) != 0)
                MMPosY += (fontHeight * fontScale) - (MMPosY % (fontHeight * fontScale)); // and ensure that it is an even scan line
            MMPrintString(getCstring(PromptString)); // evaluate prompt string and output the result
            ClearTempStringSpace(); // clear temp string space (might have been used by the prompt)
            CurrentLineNbr = 0; // do not use the line number in error reporting
            if (autoOn) { // the AUTO command is running
                if (IsValidLine(autoNext)) {
                    MMputchar('*'); // indicate that this will overwrite
                    sprintf(inpbuf, "%3d ", autoNext); // preload the buffer with the line number
                } else
                    sprintf(inpbuf, "%4d ", autoNext);
            } else
                *inpbuf = 0; // empty the input buffer
            EditInputLine(0); // get the input
            if (!*inpbuf) continue; // ignore an empty line
            InsertLastcmd(inpbuf); // save in case we want to edit it later
            ProcessInputLine(); // and execute the line - this never returns
            // we will never reach this position in the code
        } else {
            // we got here via a long jump which means an error or CTRL-C
            ContinuePoint = nextstmt; // save where we were in the program incase the user wants to invoke the continue command
            //while(MMInkey() != -1);						// consume any characters in the input buffer
        }
    }
}

void EditInputLine(int line) {
    unsigned char *p = NULL;
    unsigned char buf[MAXKEYLEN + 2];
    int lastcmd_idx, lastcmd_edit;
    int insert, startline, maxchars;
    int CharIndex, UpDownIndex, BufEdited;
    int c, i, j;

    maxchars = HRes > 400 ? 80 : 50;
    if (strlen(inpbuf) >= maxchars) {
        MMPrintString(inpbuf);
        error("Line is too long to edit");
    }
    startline = MMCharPos - 1; // save the current cursor position
    MMPrintString(inpbuf); // display the contents of the input buffer (if any)
    CharIndex = strlen(inpbuf); // get the current cursor position in the line
    //if(!autoOn) while(CharIndex)  { MMputchar('\b'); CharIndex--; }                   // and go back to the beginning
    UpDownIndex = maxchars;
    insert = false;
    Cursor = C_STANDARD;
    lastcmd_edit = lastcmd_idx = 0;
    BufEdited = (CharIndex != 0);
    while (1) {
        c = MMgetchar();
        if (c == TAB) {
            strcpy(buf, "        ");
            buf[8 - (CharIndex % 8)] = 0;
        } else {
            buf[0] = c;
            buf[1] = 0;
        }
        do {
            switch (buf[0]) {
                case '\r':
                case '\n': if (autoOn && atoi(inpbuf) > 0) autoNext = atoi(inpbuf) + autoIncr;
                    if (autoOn && !BufEdited) *inpbuf = 0;
                    goto saveline;
                    break;

                case '\b': if (CharIndex > 0) {
                        BufEdited = true;
                        i = CharIndex - 1;
                        for (p = inpbuf + i; *p; p++) *p = *(p + 1); // remove the char from inpbuf
                        while (CharIndex) {
                            MMputchar('\b');
                            CharIndex--;
                        } // go to the beginning of the line
                        MMPrintString(inpbuf);
                        MMputchar(' ');
                        MMputchar('\b'); // display the line and erase the last char
                        for (CharIndex = strlen(inpbuf); CharIndex > i; CharIndex--)
                            MMputchar('\b'); // return the cursor to the righ position
                    }
                    break;

                case LEFT: if (CharIndex > 0) {
                        MMputchar('\b');
                        CharIndex--;
                    }
                    break;

                case RIGHT: if (CharIndex < strlen(inpbuf)) {
                        MMputchar(inpbuf[CharIndex]);
                        CharIndex++;
                    }
                    break;

                case DEL: if (CharIndex < strlen(inpbuf)) {
                        BufEdited = true;
                        i = CharIndex;
                        for (p = inpbuf + i; *p; p++) *p = *(p + 1); // remove the char from inpbuf
                        while (CharIndex) {
                            MMputchar('\b');
                            CharIndex--;
                        } // go to the beginning of the line
                        MMPrintString(inpbuf);
                        MMputchar(' ');
                        MMputchar('\b'); // display the line and erase the last char
                        for (CharIndex = strlen(inpbuf); CharIndex > i; CharIndex--)
                            MMputchar('\b'); // return the cursor to the right position
                    }
                    break;

                case INSERT:insert = !insert;
                    Cursor = C_STANDARD + insert;
                    break;

                case HOME: while (CharIndex) {
                        MMputchar('\b');
                        CharIndex--;
                    }
                    break;

                case END: while (CharIndex < strlen(inpbuf))
                        MMputchar(inpbuf[CharIndex++]);
                    break;

                case 0x91:
                case 0x92:
                case 0x93:
                case 0x94:
                case 0x95:
                case 0x96:
                case 0x97:
                case 0x98:
                case 0x99:
                case 0x9a:
                case 0x9b:
                case 0x9c: if (*FunKey[buf[0] - 0x91])
                        strcpy(&buf[1], FunKey[buf[0] - 0x91]); // copy a function key string into the buffer
                    break;

                case UP: if (!(BufEdited || autoOn || CurrentLineNbr)) {
                        if (LastErrorLine) { // we had an error so edit the error line
                            line = LastErrorLine;
                            LastErrorLine = 0;
                            BufEdited = true;
                            llist(inpbuf, findline(line, false));
                            if (line) {
                                CharIndex = startline = 0;
                                MMputchar('\r');
                            } // we don't want the prompt any more
                            goto insert_lastcmd; // gotos are bad, I know, I know
                        }
                        while (CharIndex) {
                            MMputchar('\b');
                            CharIndex--;
                        } // go to the beginning of line
                        if (lastcmd_edit) {
                            i = lastcmd_idx + strlen(&lastcmd[lastcmd_idx]) + 1; // find the next command
                            if (lastcmd[i] != 0 && i < STRINGSIZE - 1) lastcmd_idx = i; // and point to it for the next time around
                        } else
                            lastcmd_edit = true;
                        strcpy(inpbuf, &lastcmd[lastcmd_idx]); // get the command into the buffer for editing
                        line = atoi(inpbuf); // get the line number
                        if (line) {
                            CharIndex = startline = 0;
                            MMputchar('\r');
                        } // we don't want the prompt any more if editing a program line
                        goto insert_lastcmd;
                    }
                    goto edit_line_change; // we must be changing lines


                case DOWN: if (!(BufEdited || autoOn || CurrentLineNbr)) {
                        while (CharIndex) {
                            MMputchar('\b');
                            CharIndex--;
                        } // go to the beginning of line
                        if (lastcmd_idx == 0)
                            *inpbuf = lastcmd_edit = 0;
                        else {
                            for (i = lastcmd_idx - 2; i > 0 && lastcmd[i - 1] != 0; i--); // find the start of the previous command
                            lastcmd_idx = i; // and point to it for the next time around
                            strcpy(inpbuf, &lastcmd[i]); // get the command into the buffer for editing
                            line = atoi(inpbuf); // get the line number
                            if (line) {
                                CharIndex = startline = 0;
                                MMputchar('\r');
                            } // we don't want the prompt any more if editing a program line
                        }
                        goto insert_lastcmd; // gotos are bad, I know, I know
                    }

edit_line_change: // goto here if we are changing the line being edited
                    if (line == 0 || autoOn) break; // cannot do this if not editing a line in a program
                    if (buf[0] == DOWN) { // down arrow
                        p = (unsigned char *) findline(line + 1, false); // find the next program line
                        if (p[0] == 0 && p[1] == 0) break; // skip if at program end
                    } else { // up arrow
                        if (p == (unsigned char *) pmemory) break; // skip if at program start
                        for (i = line - 1; i > 0; i--) { // step back in small steps
                            if ((p = findline(i, false)) == (unsigned char *) pmemory) break;
                            if (line != ((p[1] << 8) | p[2])) break; // stop when we have found a new line
                        }
                        if (i == 0) break; // sanity check
                        line = i;
                    }
                    tokenise(); // turn into executable code
                    if (*tknbuf != T_LINENBR) { // is there a line number?
                        pp = tknbuf; // if not we are in immediate mode
                        Cursor = C_STANDARD;
                        ExecuteProgram(); // execute the line straight away
                        longjmp(mark, 1); // and go straight back to the prompt
                    } else if (BufEdited) { // we are adding this line to program memory
                        clearvars(); // clear any leftovers from the previous program
                        clearstack();
                        AddProgramLine(); // add to program memory
                    }
                    while (CharIndex) {
                        MMputchar('\b');
                        CharIndex--;
                    } // go to the beginning of line
                    if (buf[0] == DOWN) p = findline(line + 1, false); // find the program line again in case saving moved pmemory
                    llist(inpbuf, p); // expand the line
                    line = (p[1] << 8) | p[2]; // save the line number for the future

insert_lastcmd: // goto here if we are just editing a command
                    if (strlen(inpbuf) + startline >= maxchars) { // if the line is too long
                        while (CharIndex) {
                            MMputchar('\b');
                            CharIndex--;
                        } // go to the start of the line
                        MMPrintString(inpbuf); // display the offending line
                        error("Line is too long to edit");
                    }
                    MMPrintString(inpbuf); // display the line
                    CharIndex = strlen(inpbuf); // get the current cursor position in the line
                    for (i = 1; i < maxchars - strlen(inpbuf) - startline; i++) {
                        MMputchar(' '); // erase the rest of the line
                        CharIndex++;
                    }
                    j = UpDownIndex;
                    if (j > strlen(inpbuf)) j = strlen(inpbuf); // this line might be shorter
                    while (CharIndex > j) {
                        MMputchar('\b');
                        CharIndex--;
                    } // return the cursor to the right position
                    break;

                default: if (buf[0] >= ' ' && buf[0] < 0x7f) {
                        BufEdited = true; // this means that something was typed
                        LastErrorLine = 0;
                        i = CharIndex;
                        j = strlen(inpbuf);
                        if (insert) {
                            if (strlen(inpbuf) >= maxchars - 1) break; // sorry, line full
                            for (p = inpbuf + strlen(inpbuf); j >= CharIndex; p--, j--) *(p + 1) = *p;
                            inpbuf[CharIndex] = buf[0]; // insert the char
                            MMPrintString(&inpbuf[CharIndex]); // display new part of the line
                            CharIndex++;
                            for (j = strlen(inpbuf); j > CharIndex; j--)
                                MMputchar('\b'); // return the cursor to the right position
                        } else {
                            inpbuf[strlen(inpbuf) + 1] = 0; // incase we are adding to the end of the string
                            inpbuf[CharIndex++] = buf[0]; // overwrite the char
                            MMputchar(buf[0]); // display it
                            if (CharIndex + startline >= maxchars) { // has the input gone beyond the end of the line?
                                MMgetline(0, inpbuf); // use the old fashioned way of getting the line
                                if (autoOn && atoi(inpbuf) > 0) autoNext = atoi(inpbuf) + autoIncr;
                                goto saveline;
                            }
                        }
                    }
                    break;
            }
            if (buf[0] != UP && buf[0] != DOWN) UpDownIndex = CharIndex; // this keeps the cursor in the same position going up & down
            for (i = 0; i < MAXKEYLEN - 1; i++) buf[i] = buf[i + 1]; // suffle down the buffer to get the next char
        } while (*buf);
    }

saveline:
    Cursor = C_STANDARD;
    MMPrintString("\r\n");
}

void ProcessInputLine(void) {
    tokenise(); // turn into executable code
    if (*tknbuf != T_LINENBR) { // is there a line number?
        pp = tknbuf; // if not we are in immediate mode
        ExecuteProgram(); // execute the line straight away
    } else { // we are adding this line to program memory
        clearvars(); // clear any leftovers from the previous program
        clearstack();
        AddProgramLine(); // add to program memory
    }
    longjmp(mark, 1); // go straight back to the prompt
}




//take an input string in inpbuf[] and copy it to tknbuf[] and:
// - convert the line number to a binary number
// - convert keywords to tokens
// - convert the colon to a zero char
//the result in tknbuf[] is terminated with double zero chars

void tokenise(void) {
    char *p, *op;
    int i, cmdlen;
    char *tp, c;
    int firstnonwhite;
    int emptyline = true;

    // first, make sure that only printable characters are in the line
    p = inpbuf;
    while (*p) {
        *p = *p & 0x7f;
        if (*p < ' ' || *p == 0x7f) *p = ' ';
        p++;
    }

    // setup the input and output buffers
    p = inpbuf;
    op = tknbuf;
    *op = 0;

    if (*p == 0) return;

    // get the line number if it exists
    skipspace(p);
    i = strtol(p, &tp, 10);
    if (i != 0) {
        if (i < 0 || i >= MAXLINENBR) error("Invalid line number");
        *op++ = T_LINENBR;
        *op++ = i >> 8;
        *op++ = (i & 0xff);
        p = tp;
    }

    // skip a colon immediately following the line number (LIST does that)
    if (*p == ':') p++;

    // process the rest of the line
    firstnonwhite = true;
    while (*p) {

        if (*p == ' ') {
            *op++ = *p++;
            continue;
        } else
            emptyline = false;

        // first look for quoted text and copy it across
        // this will also accept a string without the closing quote and it will add the quote in
        if (*p == '"') {
            do {
                *op++ = *p++;
            } while (*p != '"' && *p);
            *op++ = '"';
            if (*p == '"') p++;
            continue;
        }

        // copy anything after a comment (')
        if (*p == '\'') {
            do {
                *op++ = *p++;
            } while (*p);
            continue;
        }

        // check for multiline separator (colon) and replace with a zero char
        if (*p == ':') {
            *op++ = 0;
            p++;
            firstnonwhite = true;
            continue;
        }

        // not whitespace or string or comment  - try a number
        if (isdigit(*p) || *p == '.') { // valid chars at the start of a number
            while (isdigit(*p) || *p == '.' || *p == 'E' || *p == 'e')
                if (*p == 'E' || *p == 'e') { // check for '+' or '-' as part of the exponent
                    *op++ = *p++; // copy the number
                    if (*p == '+' || *p == '-') { // BUGFIX by Gerard Sexton
                        *op++ = *p++; // copy the '+' or '-'
                    }
                } else {
                    *op++ = *p++; // copy the number
                }
            firstnonwhite = false;
            continue;
        }

        // not whitespace or string or comment or number - see if we can find a token identifier
        c = toupper(*p); // get the first char to quickly identify a potential command or token
        if (firstnonwhite) { // first entry on the line? must be a command
            // test if there is a command and, if so, convert the command into a command token
            for (i = 0; i < CommandTableSize - 1; i++) {
                tp = commandtbl[i].name;
                if (c == *tp && !strncasecmp(p, tp, cmdlen = strlen(tp))) { // we have found a potential match
                    if (!isnamechar(p[cmdlen]) || p[cmdlen - 1] == '(') {// check that the identifier is terminated correctly
                        tp = p + cmdlen;
                        skipspace(tp); // find the character after the command
                        // only treat this as a command if there is not an equals following and it is not a function type command
                        if (*tp != '=' || (commandtbl[i].type & T_FUN) || i == commandvalue("REM")) {
                            *op++ = i + C_BASETOKEN; // insert the token
                            p += cmdlen; // and step over it in the source text
                            if (i == commandvalue("REM")) // check if it is a REM command
                                while (*p) *op++ = *p++; // just copy everything
                            firstnonwhite = false;
                            break;
                        }
                    }
                }
            }
            if (i != CommandTableSize - 1) continue;
        } else {
            // test if there is a function or operator and, if so, convert it into a token
            for (i = 0; i < TokenTableSize - 1; i++) {
                tp = tokentbl[i].name;
                if (c == *tp && !strncasecmp(p, tp, cmdlen = strlen(tp))) { // we have found a potential match
                    // check that the identifier is terminated correctly - there is a special case with functions with arguments
                    // in that case we always assume that it is correctly terminated regardless
                    if (!isalpha(*p) || !isnamechar(p[cmdlen]) || (tokentbl[i].type & T_FUN)) {
                        i += C_BASETOKEN;
                        *op++ = i; // insert the token
                        p += cmdlen; // and step over it in the source text
                        if (i == tokenvalue[TKN_THEN] || i == tokenvalue[TKN_ELSE])
                            firstnonwhite = true; // a command is valid after a THEN or ELSE
                        else
                            firstnonwhite = false;
                        break;
                    }
                }
            }
            if (i != TokenTableSize - 1) continue;
        }

        // not whitespace or string or comment or token identifier or number - try for a variable name
        if (isalpha(*p) || *p == '_') { // valid chars at the start of a variable name
            if (firstnonwhite) // first entry on the line? must be an implied let
                *op++ = commandvalue("LET") + C_BASETOKEN; // find let's token value and copy into memory
            while (isnamechar(*p)) *op++ = *p++; // copy the the variable name
            firstnonwhite = false;
            continue;
        }


        // something else, so just copy the one character
        *op++ = *p++;
        firstnonwhite = false;

    } // end of loop, if the loop has no printable characters make sure it is terminated properly
    if (emptyline) {
        if (*tknbuf == T_LINENBR)
            op = tknbuf + 3;
        else
            op = tknbuf;
    }
    *op++ = 0;
    *op++ = 0;
    *op++ = 0; // terminate with  zero chars
}



// run a program
// this will continuously execute a program until the end (marked by TWO zero chars)
// before calling this the global pp must be set to the first line to be executed

void ExecuteProgram(void) {
    while (1) {
        CheckAbort();
        skipspace(pp); // just in case, skip any whitespace
        if (*pp == 0) pp++; // step over the zero byte marking the beginning of a new element
        if (*pp == T_LINENBR) {
            CurrentLineNbr = (((pp[1]) << 8) | (pp[2])); // get the line number for error reporting
            if (TraceOn) {
                sprintf(inpbuf, "[%d]", CurrentLineNbr);
                if(TraceOn==2)
                    SupressVideo=true;
                MMPrintString(inpbuf);
                    SupressVideo=false;
            }
            pp += 3; // and step over the number
        }
        skipspace(pp); // and skip any trailing whitespace

        nextstmt = cmdline = pp + 1;
        skipspace(cmdline);
        skipelement(nextstmt);
        if (*pp && *pp != '\'') { // ignore an empty line
            if (*pp >= C_BASETOKEN && *pp - C_BASETOKEN < CommandTableSize - 1 && (commandtbl[*pp - C_BASETOKEN].type & T_CMD)) {
                cmdtoken = *pp - C_BASETOKEN;
                targ = T_CMD;
                commandtbl[*pp - C_BASETOKEN].fptr(); // execute the command
            } else { // if it is not a token then assume that it is an implied LET
                cmdtoken = commandvalue("LET") + C_BASETOKEN;
                cmdline = pp;
                commandtbl[cmdtoken - C_BASETOKEN].fptr(); // execute the LET command
            }
            ClearTempStringSpace(); // at the end of each command we need to clear any temporary string vars
            check_interrupt(); // check for a hardware interrupt and execute
        }
        pp = nextstmt;
        if (pp[0] == 0 && pp[1] == 0) return; // the end of the program is marked by TWO zero chars
    }
}


/********************************************************************************************************************************************
 Program memory mamagement
 *********************************************************************************************************************************************/

// takes a program line (in the global tknbuf[]) and inserts it into the program
// memory starting at pmemory.  This will delete the program line if the incomming
// line is zero length.

void AddProgramLine(void) {
    int nbr, i, length;
    char *p1, *p2;

    pp = pmemory;

    if (*tknbuf != T_LINENBR) return; // safty check
    nbr = (tknbuf[1] << 8) | tknbuf[2]; // get the line number

    for (p1 = tknbuf + 3; !(p1[0] == 0 && p1[1] == 0); p1++); // count the length of the line
    length = p1 - tknbuf + 1; // the +1 includes the zero terminating char
    if (tknbuf[0] == 0 || tknbuf[3] == 0) length = 0; // check if it is actually zero length

    // look for a matching line number
    //i = nbr;
    //p1 = findline(&i);
    p1 = pmemory;
    i = 0;
    while (1) {
        if (p1[0] == 0 && p1[1] == 0) { // if end of the program
            p1++; // leave a zero byte between the old and new lines
            break;
        }
        if (*p1 == T_LINENBR) {
            i = (p1[1] << 8) | p1[2]; // get the number
            if (i >= nbr) break; // we are looking for a line greater or equals
            p1 += 2; // step over the line nbr
        }
        p1++;
    }

    // found the spot to insert the code (p1), first check if we need to to delete an existing line
    // and if so, calculate the length of the line, then delete
    if (i == nbr) {
        for (p2 = p1 + 3; !(p2[0] == 0 && (p2[1] == T_LINENBR || p2[1] == 0)); p2++);
        p2++; // point to the next line's line number
        memmove(p1, p2, psize - (p2 - pmemory));
        psize -= (p2 - p1);
    }

    // insert the new line if it is not empty
    if (length) {
        if ((psize + length + 4) >= PROGMEMSIZE) error("Out of program memory"); // the 4 is a safty margin to ensure enough zeros at the end
        memmove(p1 + length, p1, psize - (p1 - pmemory)); // move down the existing code to make space
        memmove(p1, tknbuf, length); // and insert the new line
        psize += length;
    }
    pmemory[psize] = pmemory[psize + 1] = pmemory[psize + 2] = pmemory[psize + 3] = 0; // ensure that the last four are zero
}



// clear program memory

void clearprog(void) {
    pmemory[0] = pmemory[1] = pmemory[3] = 0;
    psize = 1;
    clearvars();
    clearstack();
    autoOn = 0;
    autoNext = 10;
    autoIncr = 10; // use by the AUTO command
}



// search through program memory looking for a line number. Stops when it has a matching or larger number
// returns a pointer to the T_LINENBR token or a pointer to the two zero characters representing the end of the program

char *findline(int nbr, int mustfind)
{
#undef  OLIMEX
// SPP +
    #ifdef  OLIMEX
    unsigned char *p;   // edit by SPP
    #else
    char *p;         // original
    #endif
// SPP -
    int i;

    p = pmemory;
    while (1) {
        if (p[0] == 0 && p[1] == 0)
        {
            i = MAXLINENBR;
            // p++;
            break;
        }
    // SPP +
    #ifdef  OLIMEX
        if (p[0] >= T_LINENBR)    // edin by SPP
    #else
        if (p[0] == T_LINENBR)    // original
    #endif
    // SPP -
        {
            i = (p[1] << 8) | p[2];
            if (i >= nbr) break;
            p += 2;
        }
        p++;
    }
    if (mustfind && i != nbr) error("Invalid line number");
    return p;
}
#define OLIMEX



// returns true if 'line' is a valid line in the program

int IsValidLine(int nbr) {
    unsigned char *p;
    p = findline(nbr, false);
    if (p[0] == T_LINENBR) {
        if (((p[1] << 8) | p[2]) == nbr) return true;
    }
    return false;
}




/********************************************************************************************************************************************
routines for storing and manipulating variables
 ********************************************************************************************************************************************/


// find or create a variable
// if autocreate is false this will throw an error if the variable is NOT found
// there are four types of variable:
//  - T_NOTYPE a free slot that was used but is now free for reuse
//  - T_STR string variable (created if the last byte of the name is a $ char)
//  - T_NBR holds a float (created when the above criteria do not apply)
// if it is type T_NBR the value is held in the variable slot otherwise a block of memory of
// MAXSTRLEN size will be malloc'ed and the pointer stored in the variable slot.

void *findvar(char *p, int action) {
    char name[MAXVARLEN + 1];
    char arg[STRINGSIZE];
    int i, j, ifree, nbr, vtype;
    char *s;
    int dim[MAXDIM], dnbr;
#ifdef TEST_STACK_OVERFLOW												// experimental and for the PIC32 only
    register unsigned int msp asm("sp");
    if (msp < StackLimit) error("Stack overflow. Expression is too complex");
#endif

    vtype = T_NBR;
    dnbr = 0;

    // first zero the array used for holding the dimension values
    for (i = 0; i < MAXDIM; i++) dim[i] = 0;
    ifree = varcnt;

    // check the first char for a legal variable name
    skipspace(p);
    if (!isnamestart(*p)) error("Variable not found");

    // copy the variable name into name
    s = name;
    i = 0;
    do {
        *s++ = *p++;
        if (i++ > MAXVARLEN) error("Variable name too long");
    } while (isnamechar(*p));

    // check the terminating char.  If it is $ set the type and copy to name
    if (*p == '$') {
        vtype = T_STR;
        *s++ = *p++;
        if (i++ > MAXVARLEN) error("Variable name too long");
    }

    // check if this is an array
    if (*p != '(') {
        *s = 0; // not an array, so terminate
    } else {
        // this is an array, so we first copy the opening bracket into name and terminate it
        *s++ = *p++;
        if (i++ > MAXVARLEN) error("Variable name too long");
        *s = 0;
        // point to where we will store the argument and setup for the loop
        s = arg;
        i = 1;
        // copy the argument into arg keeping track of nested brackets
        // when we start p is pointing to the char after the opening bracket
        while (1) {
            if (*p == 0) error("Expected closing bracket");
            if (*p == ')') if (--i == 0) break;
            if (*p == '(' || (tokentype(*p) & T_FUN)) i++;
            *s++ = *p++;
        }
        *s = 0;
        s = arg;
        p++; // step over the closing bracket
        { // start a new block - getargs macro must be the first executable stmt in a block
            // split the argument into individual elements
            // find the value of each dimension and store in dims[]
            getargs(&s, MAXDIM * 2, ",");
            if ((argc & 0x01) == 0) error("Invalid array dimension");
            dnbr = argc / 2 + 1;
            if (dnbr >= MAXDIM) error("Too many dimensions");
            for (i = 0; i < argc; i += 2) {
                dim[i / 2] = getinteger(argv[i]);
                if (dim[i / 2] < OptionBase) error("Invalid array dimension");
            }
        }
    }

    // we now have the variable name and, if it is an array, the parameters
    // search the table looking for a match
    for (i = 0; i < varcnt; i++) {
        if (str_equal(name, vartbl[i].name))
            break;
        else if (vartbl[i].type == T_NOTYPE)
            ifree = i;
    }

    // if we found an existing and matching variable
    // set the global VarIndex indicating the index in the table
    if (i < varcnt && *vartbl[i].name != 0) {
        VarIndex = i;

        // if it is not an array this is easy, just calculate and return a pointer to the value
        if (vartbl[VarIndex].dims[0] == 0) {
            if (vtype == T_NBR)
                return &(vartbl[VarIndex].val.f);
            else
                return vartbl[VarIndex].val.s;
        }

        // if we reached this point it must be a reference to an existing array
        // check that we are not using DIM and that all parameters are within the dimensions
        if (action & V_DIM_ARRAY) error("Cannot re dimension array");
        for (i = 0; i < MAXDIM && vartbl[VarIndex].dims[i] != 0; i++);
        if (i != dnbr) error("Number of dimensions");
        for (i = 0; i < dnbr; i++)
            if (dim[i] > vartbl[VarIndex].dims[i] || dim[i] < OptionBase)
                error("Array index out of bounds");

        // then calculate the index into the array.  Bug fix by Gerard Sexton.
        nbr = dim[0] - OptionBase;
        j = 1;
        for (i = 1; i < dnbr; i++) {
            j *= (vartbl[VarIndex].dims[i - 1] + 1 - OptionBase);
            nbr += (dim[i] - OptionBase) * j;
        }

        // finally return a pointer to the value
        if (vtype == T_NBR)
            return vartbl[VarIndex].val.s + (nbr * sizeof (float));
        else
            return vartbl[VarIndex].val.s + (nbr * STRINGSIZE);
    }

    // we reached this point if no existing variable has been found
    if (action & V_NOFIND_ERR) error("Cannot find variable");
    if (action & V_NOFIND_NULL) return NULL;

    // at this point we need to create the variable
    // as a result of the previous search ifree is the index to the entry that we should use

    if (dnbr && !(action & V_DIM_ARRAY)) error("Array must be dimensioned first");

    // if we are adding to the top, increment the number of vars
    if (ifree == varcnt) varcnt++;
    if (ifree > MAXVARS) error("Out of memory");
    VarIndex = ifree;

    // initialise it: save the name, set the initial value to zero and set the type
    strcpy(vartbl[ifree].name, name);
    vartbl[ifree].type = vtype;
    for (j = 0; j < MAXDIM; j++) vartbl[ifree].dims[j] = 0;

    // the easy request is for is a non array numeric variable, so just initialise to
    // zero and return the pointer
    if (dim[0] == 0 && vtype == T_NBR) {
        vartbl[ifree].val.f = 0;
        return &(vartbl[ifree].val.f);
    }

    // if this is an array copy the array dimensions and calculate the overall size
    // for a non array string this will leave nbr = 1 which is just what we want
    for (nbr = 1, i = 0; i < dnbr; i++) {
        if (dim[i] <= OptionBase) error("Invalid dimension");
        vartbl[VarIndex].dims[i] = dim[i];
        nbr *= (dim[i] + 1 - OptionBase);
    }

    // we now have a string, an array of strings or an array of numbers
    // all need some memory to be allocated (note: getmemory() zeros the memory)
    vartbl[ifree].val.s = NULL;
    if (vtype == T_NBR)
        p = getmemory(nbr * sizeof (float));
    else
        p = getmemory(nbr * STRINGSIZE);

    vartbl[ifree].type = vtype;
    vartbl[ifree].val.s = p;
    return p;
}



// clear (or delete) all variables

void clearvars(void) {
    int i;

    // first step through the variable table and clear any heap allocated to a variable
    for (i = 0; i < varcnt; i++)
        if (vartbl[i].type == T_STR || vartbl[i].dims[0] != 0)
            free(vartbl[i].val.s);

    ClearTempStringSpace(); // clear temp string space

    // we can now delete all variables by zeroing the counters
    varcnt = 0;
    OptionBase = 0;
    DimUsed = false;
    HeapUsed = 0;
}


// clear all stack pointers (eg, FOR/NEXT stack, DO/LOOP stack, GOSUB stack, etc)

void clearstack(void) {
    int i;

    NextData = 0;
    forindex = 0;
    doindex = 0;
    gosubindex = 0;
    ContinuePoint = NULL;
    CloseAllFiles();
    inside_then_else = false;
    for (i = 3; i < NBRFONTS - 1; i++) UnloadFont(i);
}










/********************************************************************************************************************************************
 routines for evaluating expressions
 the main functions are getnumber(), getinteger() and getstring()
 ********************************************************************************************************************************************/


// evaluate an expression.  p points to the start of the expression in memory
// returns either the float or string in the pointer arguments
// *t points to an integer which holds the type of variable we are looking for
//  if *t = T_STR or T_NBR will throw an error if the result is not the correct type
//  if *t = T_NOTYPE it will not throw an error and will return the type found in *t

char *evaluate(char *p, float *fa, char **sa, int *ta) {
    int o;
    int t = *ta;
    char *s;

    p = getvalue(p, fa, &s, &o, &t);
    while (o != E_END) p = doexpr(p, fa, &s, &o, &t);
    if (*ta == T_NBR && t == T_STR) error("Expected a number");
    if (*ta == T_STR && t == T_NBR) error("Expected a string");
    if (o != E_END) error("Incorrect expression syntax");
    *ta = t;
    *sa = s;
    return p;
}


// evaluate an expression to get a number
// note that this does not work when using the program pointer (pp) as this does not advance the pointer

float getnumber(char *p) {
    int t = T_NBR;
    float f;
    char *s;

    evaluate(p, &f, &s, &t);
    return f;
}


// evaluate an expression and return an integer
// this will correctly round the number if it is a fraction of an integer

int getinteger(char *p) {
    return MMround(getnumber(p));
}


// evaluate an expression to get a string
// note that this does not work when using the program pointer (pp) as this does not advance the pointer

char *getstring(char *p) {
    int t = T_STR;
    float f;
    char *s;

    evaluate(p, &f, &s, &t);
    return s;
}



// recursively evaluate an expression observing the rules of operator precedence

char *doexpr(char *p, float *fa, char **sa, int *oo, int *ta) {
    float fa1, fa2;
    int o1, o2;
    int t1, t2;
    char *sa1, *sa2;

    fa1 = *fa;
    sa1 = *sa;
    t1 = *ta;
    o1 = *oo;
    p = getvalue(p, &fa2, &sa2, &o2, &t2);

    while (1) {
        if (o2 == E_END || tokentbl[o1].precedence <= tokentbl[o2].precedence) {
            if (t1 != t2) error("Incompatible types in expression");
            if (!(tokentbl[o1].type & T_OPER) || !(tokentbl[o1].type & t1))
                error("Invalid operator in expression");
            farg1 = fa1;
            farg2 = fa2; // setup the float args (incase it is a float)
            sarg1 = sa1;
            sarg2 = sa2; // ditto string args
            targ = t1;
            tokentbl[o1].fptr(); // call the operator function
            *fa = fret;
            *sa = sret;
            *oo = o2;
            *ta = targ;
            return p;
        }            // the next operator has a higher precedence, recursive call to evaluate it
        else
            p = doexpr(p, &fa2, &sa2, &o2, &t2);
    }
}



// get a value, either from a constant, function or variable
// also returns the next operator to the right of the value or E_END if no operator

char *getvalue(char *p, float *fa, char **sa, int *oo, int *ta) {
    float f = 0;
    char *s = NULL;
    int t = T_NOTYPE;
    char *tp;
    int i;
#ifdef TEST_STACK_OVERFLOW											// experimental and for the PIC32 only
    register unsigned int msp asm("sp");
    if (msp < StackLimit) error("Stack overflow. Expression is too complex");
#endif

    skipspace(p);

    // special processing for the NOT operator
    // just get the next value and invert it
    if (tokenfunction(*p) == op_not) {
        int ro;
        if (t == T_STR) error("Incompatible types in expression");
        p++;
        t = T_NBR;
        p = getvalue(p, &f, &s, &ro, &t); // get the next value
        f = (f != 0) ? 0 : 1; // invert the value returned
        skipspace(p);
        *fa = f; // save what we have
        *sa = s;
        *ta = t;
        *oo = ro;
        return p; // return straight away as we already have the next operator
    }
    // special processing for the uninary - operator
    // just get the next value and invert it
    if (tokenfunction(*p) == op_subtract) {
        int ro;
        if (t == T_STR) error("Incompatible types in expression");
        p++;
        t = T_NBR;
        p = getvalue(p, &f, &s, &ro, &t); // get the next value
        f = -f; // negate the value returned
        skipspace(p);
        *fa = f; // save what we have
        *sa = s;
        *ta = t;
        *oo = ro;
        return p; // return straight away as we already have the next operator
    }        // if a function execute it and save the result
    else if (tokentype(*p) & (T_FUN | T_FNA)) {
        tp = p;
        // if it is a function with arguments we need to locate the closing bracket and copy the argument to
        // a temporary variable so that functions like getarg() will work.
        if (tokentype(*p) & T_FUN) {
            p = getclosebracket(p + 1); // find the closing bracket
            ep = GetTempStringSpace(); // this will last for the life of the command
            *p = 0; // temporarily remove the closing bracket
            strcpy(ep, tp + 1); // save the string
            *p = ')'; // restore the closing quote
        }
        p++; // point to after the function (without argument) or after the closing bracket
        targ = (tokentype(*tp) & (T_NBR | T_STR | T_FUN | T_FNA)); // set the type of the function (which might need to know this)
        tokenfunction(*tp)(); // execute the function
        t = (tokentype(*tp) & (T_NBR | T_STR)); // save the type of the function
        f = fret;
        s = sret; // save the result
    }        // if opening bracket then first evaluate the contents of the bracket
    else if (*p == '(') {
        p++; // step over the bracket
        p = evaluate(p, &f, &s, &t); // recursively get the contents
        if (*p != ')') error("No closing bracket in expression");
        ++p; // step over the closing bracket
    }        // if it is a variable, find it and get its value
    else if (isnamestart(*p)) {
        s = findvar(p, V_FIND);
        t = vartype(p);
        f = (*(float *) s);
        p = skipvar(p);
    }        // if it is a string constant, return a pointer to that.  Note: tokenise() guarantees that strings end with a quote
    else if (*p == '"') {
        p++; // step over the quote
        s = GetTempStringSpace(); // this will last for the life of the command
        *(tp = strchr(p, '"')) = 0; // temporarily remove the closing quote
        strcpy(s, p); // save the string
        CtoM(s); // convert to a MMBasic string
        *tp = '"'; // restore the closing quote
        p = tp + 1; // point to after it
        t = T_STR;
    }        // if it is a numeric constant starting with the & character then get its base and convert
    else if (*p == '&') {
        p++;
        switch (toupper(*p)) {
            case 'H': i = 16;
                break;
            case 'O': i = 8;
                break;
            case 'B': i = 2;
                break;
            default: error("Invalid type specification following &");
        }
        p++;
        f = strtol(p, &p, i);
        t = T_NBR;
    }        // is it an ordinary numeric constant?  get its value if yes
        // a leading + or - might have been converted to a token so we need to check for them also
    else if (isdigit(*p) || *p == '+' || (tokenfunction(*p) == op_subtract) || *p == '-' || (tokenfunction(*p) == op_add) || *p == '+' || *p == '.') {
        if (tokenfunction(*p) == op_subtract) *p = '-';
        if (tokenfunction(*p) == op_add) *p = '+';
#ifdef __PIC32MX__
        {
            // because of a bug in Microchip's implementation of strtod() we have to replace any following token with a null
            // first we search for the token, then replace it, then allow strtod() to do its work and finally put back whatever was there
            char *x, c;
            x = p;
            while (isprint(*x)) x++;
            c = *x;
            *x = 0;
            f = strtod(p, &p);
            *x = c;
        }
#else
        f = strtod(p, &p);
#endif
        t = T_NBR;
    } else
        error("Invalid syntax");

    skipspace(p);
    *fa = f; // save what we have
    *sa = s;
    *ta = t;

    // get the next operator, if there is not an operator set the operator to end of expression (E_END)
    if (tokentype(*p) & T_OPER)
        *oo = *p++ - C_BASETOKEN;
    else
        *oo = E_END;

    return p;
}








/********************************************************************************************************************************************
 utility routines
 these routines form a library of functions that any command or function can use when dealing with its arguments
 by centralising these routines it is hoped that bugs can be more easily found and corrected (unlike bwBasic !)
 *********************************************************************************************************************************************/

// take a line of basic code and split it into arguments
// this function should always be called via the macro getargs
//
// a new argument is created by any of the chars in the string delim (not in brackets or quotes)
// with this function commands have much less work to do to evaluate the arguments
//
// The arguments are:
//   pointer to a pointer which points to the string to be broken into arguments.
//   the maximum number of arguments that are expected.  an error will be thrown if more than this are found.
//   buffer where the returned strings are to be stored
//   pointer to an array of strings that will contain (after the function has returned) the values of each argument
//   pointer to an integer that will contain (after the function has returned) the number of arguments found
//   pointer to a string that contains the characters to be used in spliting up the line

void makeargs(char **p, int maxargs, char *argbuf, char *argv[], int *argc, char *delim) {
    char *op;
    int inarg, expect_cmd, then_tkn, else_tkn;
    char *tp;

    tp = *p;
    op = argbuf;
    *argc = 0;
    inarg = false;
    expect_cmd = false;
    then_tkn = tokenvalue[TKN_THEN];
    else_tkn = tokenvalue[TKN_ELSE];

    // skip leading spaces
    while (*tp == ' ') tp++;

    // the command line should not have the command token in it so error if found
    // TODO: figure out if this is really needed and delete if not
    //if(!(tokentype(*tp) & (T_FUN | T_FNA))) error("Token found");

    while (*tp) {

        // comment char causes the rest of the line to be skipped
        if (*tp == '\'') {
            if (inarg) *op = 0;
            return;
        }

        // check if we have a THEN or ELSE token and if so flag that a command should be next
        if (*tp == then_tkn || *tp == else_tkn) expect_cmd = true;

        // the special characters that cause the line to be split up are in the string delim
        // any other chars form part of the one argument
        if (strchr(delim, *tp) != NULL) {
            if (inarg) *op++ = 0; // terminate the previous argument (if there was one)
            inarg = false;
            argv[(*argc)++] = op; // save the pointer for this arg
            *op++ = *tp++; // copy the token or char (always one)
            *op++ = 0; // terminate it
            continue;
        }

        // remove all spaces (outside of quoted text and bracketed text)
        if (*tp == ' ') {
            tp++;
            continue;
        }

        // not a special char so we must start a new argument
        if (!inarg) {
            if (*argc >= maxargs) error("Invalid syntax");
            argv[(*argc)++] = op; // save the pointer for this arg
            inarg = true;
        }

        // if an opening bracket '(' copy everything until we hit the matching closing bracket
        // this includes special characters such as , and ; and keeps track of any nested brackets
        if (*tp == '(' || ((tokentype(*tp) & T_FUN) && !expect_cmd)) {
            int x;
            x = (getclosebracket(tp + 1) - tp) + 1;
            memcpy(op, tp, x);
            op += x;
            tp += x;
            continue;
        }

        // if quote mark (") copy everything until the closing quote
        // this includes special characters such as , and ;
        // the tokenise() function will have ensured that the closing quote is always there
        if (*tp == '"') {
            do {
                *op++ = *tp++;
                if (*tp == 0) error("Invalid syntax");
            } while (*tp != '"');
            *op++ = *tp++;
            continue;
        }

        // anything else is just copied into the argument
        *op++ = *tp++;

        expect_cmd = false;
    }
    *op = 0; // terminate the last argument
    //*p = tp;
}


// throw an error
// this uses longjump to skip back to the command input and cleanup the stack

void __attribute__((noreturn)) error(char *msg) {
    if (MMCharPos > 1) MMPrintString("\r\n");
    SetFont(0, 1, 0); // set a reasonable default font
    if (CurrentLineNbr) {
        sprintf(inpbuf, "Error line %d%s%s\r\n", CurrentLineNbr, *msg ? ": " : "", msg);
        LastErrorLine = CurrentLineNbr;
    } else
        sprintf(inpbuf, "Error%s%s\r\n", *msg ? ": " : "", msg);
    MMPrintString(inpbuf);
    ClearTempStringSpace();
    CloseAllFiles();
    clearstack();
    longjmp(mark, 1);
}



// round a float to an integer

int MMround(float x) {
    if (x < INT_MIN - 0.5 || x > INT_MAX + 0.5)
        error("Number too large for an integer");
    return (x >= 0 ? (int) (x + 0.5) : (int) (x - 0.5));
}



// make a string uppercase

void makeupper(char *p) {
    while (*p) {
        *p = toupper(*p);
        p++;
    }
}


// get some memory from the heap

char *getmemory(int size) {
    char *s;
#ifdef TEST_STACK_OVERFLOW							// experimental and for the PIC32 only
    register unsigned int msp asm("sp");
    if (msp < StackLimit) error("Stack overflow. Expression is too complex");
#endif
    s = calloc(size, 1);
    if (s == NULL) error("Out of memory");
    HeapUsed += size;
    return s;
}


// get a temporary string buffer
// this is used by many BASIC string functions.  The space only lasts for the length of the command.
// A pointer to the space is saved in an array so that it can be returned at the end of the command

char *GetTempStringSpace(void) {
    int i;
    for (i = 0; i < MAXTEMPSTRINGS; i++)
        if (strtmp[i] == NULL)
            return (strtmp[i] = getmemory(STRINGSIZE));
    error("Too many string expressions");
}



// clear any temporary string spaces (these last for just the life of a command) and return the memory to the heap

void ClearTempStringSpace(void) {
    int i;
    for (i = 0; i < MAXTEMPSTRINGS; i++) {
        if (strtmp[i] != NULL) {
            free(strtmp[i]);
            strtmp[i] = NULL;
            HeapUsed -= STRINGSIZE;
        } else
            return;
    }
}



// find the value of a command token given its name

int commandvalue(const char *n) {
    int i;
    for (i = 0; i < CommandTableSize - 1; i++)
        if (str_equal(n, commandtbl[i].name))
            return i;
    error("Internal fault while searching command table (sorry)");
}



// find the value of a token given its name

int GetTokenValue(const char *n) {
    int i;
    for (i = 0; i < TokenTableSize - 1; i++)
        if (str_equal(n, tokentbl[i].name))
            return i + C_BASETOKEN;
    error("Internal fault while searching token table (sorry)");
}


// skip to the end of a variable

char *skipvar(char *p) {
    char *tp;
    int i;

    tp = p;
    // check the first char for a legal variable name
    skipspace(p);
    if (!isnamestart(*p)) return tp;

    do {
        p++;
    } while (isnamechar(*p));

    // check the terminating char.
    if (*p == '$') p++;

    if (p - tp > MAXVARLEN) error("Variable name too long");

    if (*p == '(') {
        // this is an array

        p++;
        if (p - tp > MAXVARLEN) error("Variable name too long");

        // step over the parameters keeping track of nested brackets
        i = 1;
        while (1) {
            if (*p == 0) error("Expected closing bracket");
            if (*p == ')') if (--i == 0) break;
            if (*p == '(' || (tokentype(*p) & T_FUN)) i++;
            p++;
        }
        p++; // step over the closing bracket
    }
    return p;
}


// return the type of a variable (string or numeric)
// p must point to a valid variable

int vartype(char *p) {
    skipspace(p);
    if (!isnamestart(*p)) return -1;
    p++;
    while (isnamechar(*p)) p++;
    if (*p == '$')
        return T_STR;
    else
        return T_NBR;
}


// scans text looking for the matching closing bracket
// it will handle nested brackets and functions
// it expects to be called pointing at the opening bracket or function

char *getclosebracket(char *p) {
    int i = 1;
    //if(!(*p == '(' || (tokentype(*p) & T_FUN) || (commandtype(*p) & T_FUN))) error("Expected opening bracket");
    do {
        if (*p == 0) error("Expected closing bracket");
        if (*p == ')') i--;
        if (*p == '(' || (tokentype(*p) & T_FUN)) i++;
        p++;
    } while (i);
    return p - 1;
}


// check that there is no excess text following an element
// will skip spaces and abort if a zero char is not found

void checkend(char *p) {
    skipspace(p);
    if (*p == '\'') return;
    if (*p) error("Unexpected text");
}


// check if the next text in an element (a basic statement) corresponds to an alpha string
// leading whitespace is skipped and the string must be terminated with a non alpha character
// returns a pointer to the end of the string if found or NULL is not

char *checkstring(char *p, char *tkn) {
    while (*p == ' ') p++; // skip leading spaces
    while (*tkn && (*tkn == toupper(*p))) {
        tkn++;
        p++;
    } // compare the strings
    if (*tkn == 0 && !isalpha(*p)) return p; // return the string if successful
    return NULL; // or NULL is not
}



// count the length of a program line excluding the terminating zero byte
// the pointer p must be pointing to the T_LINENBR token at the start of the line

int GetLineLength(char *p) {
    char *start;
    start = p;
    p += 3; // step over the line number
    while (!(p[0] == 0 && (p[1] == 0 || p[1] == T_LINENBR))) p++;
    return (p - start);
}


// insert a string into the start of the lastcmd buffer.
// the buffer is a sequence of strings separated by a zero byte.
// using the up arrow usere can call up the last few commands executed.

void InsertLastcmd(char *s) {
    int i, slen;
    if (strcmp(lastcmd, s) == 0) return; // don't duplicate
    slen = strlen(s);
    if (slen < 1 || slen > MAXSTRLEN) return;
    slen++;
    for (i = STRINGSIZE - 1; i >= slen; i--)
        lastcmd[i] = lastcmd[i - slen]; // shift the contents of the buffer up
    strcpy(lastcmd, s); // and insert the new string in the beginning
    for (i = MAXSTRLEN; lastcmd[i]; i--) lastcmd[i] = 0; // zero the end of the buffer
}



/********************************************************************************************************************************************
 string routines
 these routines form a library of functions for manipulating MMBasic strings.  These strings differ from ordinary C strings in that the length
 of the string is stored in the first byte and the string is NOT terminated with a zero valued byte.  This type of string can store the full
 range of binary values (0x00 to 0xff) in each character.
 *********************************************************************************************************************************************/

// convert a MMBasic string to a C style string
// if the MMstr contains a null byte that byte is skipped and not copied

char *MtoC(char *p) {
    int i;
    char *p1, *p2;
    i = *p;
    p1 = p + 1;
    p2 = p;
    while (i) {
        if (p1) *p2++ = *p1;
        p1++;
        i--;
    }
    *p2 = 0;
    return p;
}


// convert a c style string to a MMBasic string

char *CtoM(char *p) {
    int len, i;
    char *p1, *p2;
    len = i = strlen(p);
    if (len > MAXSTRLEN) error("String is too long");
    p1 = p + len;
    p2 = p + len - 1;
    while (i--) *p1-- = *p2--;
    *p = len;
    return p;
}


// copy a MMBasic string to a new location

void Mstrcpy(char *dest, char *src) {
    int i;
    i = *src + 1;
    while (i--) *dest++ = *src++;
}



// concatenate two MMBasic strings

void Mstrcat(char *dest, char *src) {
    int i;
    i = *src;
    *dest += i;
    dest += *dest + 1 - i;
    src++;
    while (i--) *dest++ = *src++;
}


// evaluate an expression to get a string using the C style for a string
// as against the MMBasic style returned by getstring()

char *getCstring(char *p) {
    char *tp;
    tp = GetTempStringSpace(); // this will last for the life of the command
    Mstrcpy(tp, getstring(p)); // get the string and save in a temp place
    MtoC(tp); // convert to a C style string
    return tp;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// these library functions went missing in the PIC32 C compiler ver 1.12 and later
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * strncasecmp.c --
 *
 *	Source code for the "strncasecmp" library routine.
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1995-1996 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: strncasecmp.c,v 1.3 2007/04/16 13:36:34 dkf Exp $
 */

/*
 * This array is designed for mapping upper and lower case letter together for
 * a case independent comparison. The mappings are based upon ASCII character
 * sequences.
 */

const static unsigned char charmap[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xe1, 0xe2, 0xe3, 0xe4, 0xc5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

/*
 *----------------------------------------------------------------------
 *
 * strncasecmp --
 *
 *	Compares two strings, ignoring case differences.
 *
 * Results:
 *	Compares up to length chars of s1 and s2, returning -1, 0, or 1 if s1
 *	is lexicographically less than, equal to, or greater than s2 over
 *	those characters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */


int
strncasecmp(
        const char *s1, // First string.
        const char *s2, // Second string.
        size_t length) // Maximum number of characters to compare (stop earlier if the end of either string is reached).
    {
    register unsigned char u1, u2;

    for (; length != 0; length--, s1++, s2++) {
        u1 = (unsigned char) *s1;
        u2 = (unsigned char) *s2;
        if (charmap[u1] != charmap[u2]) {
            return charmap[u1] - charmap[u2];
        }
        if (u1 == '\0') {
            return 0;
        }
    }
    return 0;
}



// Compare two strings, ignoring case differences.
// Returns true if the strings are equal (ignoring case) otherwise returns false.

inline int str_equal(const unsigned char *s1, const unsigned char *s2) {
    if (charmap[*s1] != charmap[*s2]) return 0;
    for (;;) {
        if (*s2 == '\0') return 1;
        if (charmap[*++s1] != charmap[*++s2]) return 0;
    }
}

