/***********************************************************************************************************************
MMBasic

Files.c

Handles all the file input/output in MMBasic.
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

#include <p32xxxx.h>								// device specific defines
#include <plib.h>								// peripheral libraries
#include <stdlib.h>								// standard library functions
#include <string.h>								// string functions
#include <stdio.h>

#define INCLUDE_FUNCTION_DEFINES
#include "Maximite.h"
#include "MMBasic.h"
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"

#include "../Keyboard/Keyboard.h"
#include "../SDCard/SDCard.h"
#include "../SDCard/HardwareProfile.h"
#include "../Timers/Timers.h"
#include "../Serial/Serial.h"
#include "../Video/Video.h"
#ifdef OLIMEX
	#include "Setup.h"
#endif

FSFILE *MMFilePtr[MAXOPENFILES];
	 
int DefaultDrive = SDFS;

// define the flash file status
#define CLOSED		0
#define OPENREAD	1
#define OPENWRITE	2
int FlashStatus = CLOSED; 

int FlashEOF = false;
int OptionErrorAbort = true;
int MMerrno = 0;
char MSDEnable=0;

void FlashList(char *s);
int FlashFreeSpace(void);
void FlashOpenRead(char *s);
void FlashOpenWrite(char *s);
void FlashOpenAppend(char *s);
//	GS - Flash fix Start
void FlashGetEOF(void);
//	GS - Flash fix End
void FlashPutc(char c);
void FlashPutStr(char *s);
void FlashCloseRead(void);
void FlashCloseWrite(void);
unsigned char FlashGetc(void);
int FlashKill(char *fn);
volatile struct structBlock *FindFlashBlock(char* fn, int block);
void FlashRename(char *old, char *new);

extern BYTE MSDStatus;
extern BOOL SoftDetach[0 + 1];
#define LUNSoftAttach(LUN) SoftDetach[LUN]=FALSE;
#define LUNSoftDetach(LUN) SoftDetach[LUN]=TRUE;
//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// ERROR HANDLING ////////////////////////////////////////////


/*****************************************************************************************
Mapping of errors reported by the Microchip FAT 16/32 file system to MMBasic file errors
*****************************************************************************************/
const int ErrorMap[34] = {			0, 	// 0  =   No error
									11, // 1  =   An erase failed
									1, 	// 2  =   No SD card found
									15, // 3  =   The disk is of an unsupported format
									15, // 4  =   The boot record is bad
									15, // 5  =   The file system type is unsupported
									15, // 6  =   An initialization error has occurred
									15, // 7  =   An operation was performed on an uninitialized device
									10, // 8  =   A bad read of a sector occurred
									11, // 9  =   Could not write to a sector
									15, // 10 =  Invalid cluster value
									6, 	// 11 =  Could not find the file on the device
									7, 	// 12 =  Could not find the directory
									10, // 13 =  File is corrupted
									0, 	// 14 =  No more files in this directory
									15, // 15 =  Could not load/allocate next cluster in file
									5, 	// 16 =  A specified file name is too long to use
									9, 	// 17 =  A specified filename already exists on the device
									5, 	// 18 =  Invalid file name
									12, // 19 =  Attempt to delete a directory with KILL
									4, 	// 20 =  All root directory entries are taken
									3, 	// 21 =  All clusters in partition are taken
									14, // 22 =  This directory is not empty yet, remove files before deleting
									15, // 23 =  The disk is too big to format as FAT16
									2, 	// 24 =  Card is write protected
									11, // 25 =  File not opened for the write
									11, // 26 =  File location could not be changed successfully
									10, // 27 =  Bad cache read
									15, // 28 =  FAT 32 - card not supported
									8, 	// 29 =  The file is read-only
									10, // 30 =  The file is write-only
									15, // 31 =  Invalid argument
									9, 	// 32 =  Too many files are already open
									15, // 33 =  Unsupported sector size
							};									

/******************************************************************************************
Text for the file related error messages reported by MMBasic
******************************************************************************************/
	
const char *FErrorMsg[NBRERRMSG] = {	"No error",									// 0
										"SD card not found",						// 1
										"SD card is write protected",				// 2
										"No space on media",						// 3
										"All root directory entries are taken",		// 4
										"Invalid file or directory name",			// 5
										"Cannot find file",							// 6
										"Cannot find or create directory",			// 7
										"File is read only",						// 8
										"Cannot open file",							// 9
										"Cannot read from file",					// 10
										"Cannot write to file",						// 11
										"Not a file",								// 12
										"Not a directory",							// 13
										"Directory not empty",						// 14
										"Cannot access the SD card",				// 15
										"Cannot access flash memory"				// 16
									};							


int ErrorThrow(int e) {
	MMerrno = e;
	if(e > 0 && e < NBRERRMSG && OptionErrorAbort) error((char *)FErrorMsg[e]);
	return e;
}	


int ErrorCheck(void) {
	int e;
	e = FSerror();
	if(e < 1 || e > 33) return e;
	return ErrorThrow(ErrorMap[e]);
}	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************************
File related commands in MMBasic
================================
These are the functions responsible for executing the file related commands in MMBasic
They are supported by utility functions that are grouped at the end of this file

Each function is responsible for decoding a command
all function names are in the form cmd_xxxx() (for a basic command) or fun_xxxx() (for a 
basic function) so, if you want to search for the function responsible for the LOCATE command 
look for cmd_name

There are 4 items of information that are setup before the command is run.
All these are globals.

int cmdtoken	This is the token number of the command (some commands can handle multiple
			statement types and this helps them differentiate)

char *cmdline	This is the command line terminated with a zero char and trimmed of leading
			spaces.  It may exist anywhere in memory (or even ROM).

char *nextstmt	This is a pointer to the next statement to be executed.  The only thing a
			command can do with it is save it or change it to some other location.

int CurrentLineNbr  This is read only and is set to zero if the command is in immediate mode.

The only actions a command can do to change the program flow is to change nextstmt or
execute longjmp(mark, 1) if it wants to abort the program.

********************************************************************************************/



void cmd_open(void) {
	int fnbr, i;
	char *mode, *fname;
	char ss[3];														// this will be used to split up the argument line

	ss[0] = tokenvalue[TKN_FOR];
	ss[1] = tokenvalue[TKN_AS];
	ss[2] = 0;
	{																// start a new block
		getargs(&cmdline, 5, ss);									// getargs macro must be the first executable stmt in a block
		fname = getCstring(argv[0]);
		makeupper(fname);
		
		// check if it is a serial port that we are opening and, if so, handle it as a special case

#ifdef MAXIMITE
    if (argc == 3 && (memcmp(fname, "COM1:", 5) == 0 || memcmp(fname, "COM2:", 5) == 0)) {
#endif
       
#ifdef OLIMEX
    if (argc == 3 && (memcmp(fname, "COM1:", 5) == 0 || memcmp(fname, "COM2:", 5) == 0 || memcmp(fname, "COM3:", 5) == 0 || memcmp(fname, "COM4:", 5) == 0)) {
#endif
      i = (str_equal(argv[2], "CONSOLE")) ? 1 : 0;
			SerialOpen(fname, i);
			if(i == false) {
				// if it is NOT the console get the file number
				if(*argv[2] == '#') argv[2]++;
				fnbr = getinteger(argv[2]);
				if(fnbr < 1 || fnbr > 10) error("Invalid file number");
				if(MMFilePtr[fnbr - 1] != NULL) error("File number is already open");
				//MMFilePtr[fnbr - 1] = ((fname[3] == '1') ? COM1_FILE_POINTER : COM2_FILE_POINTER);
        if ((fname[3]) == '1') MMFilePtr[fnbr - 1] = COM1_FILE_POINTER;
        if ((fname[3]) == '2') MMFilePtr[fnbr - 1] = COM2_FILE_POINTER;
#ifdef OLIMEX
        if ((fname[3]) == '3') MMFilePtr[fnbr - 1] = COM3_FILE_POINTER;
        if ((fname[3]) == '4') MMFilePtr[fnbr - 1] = COM4_FILE_POINTER;
#endif
			}	
			return;
		}
		
		if(argc != 5) error("Invalid Syntax");
		if(str_equal(argv[2], "OUTPUT"))
			mode = "w";
		else if(str_equal(argv[2], "APPEND"))
			mode = "a";
		else if(str_equal(argv[2], "INPUT"))
			mode = "r";
		else
			error("Invalid file access mode");
		if(*argv[4] == '#') argv[4]++;
		fnbr = getinteger(argv[4]);
		MMfopen(fname, mode, fnbr);
	}	
}


	
void cmd_close(void) {
	int i;
	getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// getargs macro must be the first executable stmt in a block
	if((argc & 0x01) == 0) error("Invalid syntax");
	
	if(argc == 1 && str_equal(argv[0], "CONSOLE")) {
		SerialClose(SerialConsole);
		return;
	}	

	for(i = 0; i < argc; i += 2) {
		if(*argv[i] == '#') argv[i]++;
		MMfclose(getinteger(argv[i]));
	}
}	


void fun_inputstr(void) {
	int nbr, fnbr;
	char *p;
	getargs(&ep, 3, ",");
	if(argc != 3) error("Invalid syntax");
	nbr = getinteger(argv[0]);
	if(nbr < 1 || nbr > MAXSTRLEN) error("Number out of bounds");
	if(*argv[2] == '#') argv[2]++;
	fnbr = getinteger(argv[2]);
	sret = GetTempStringSpace();						// this will last for the life of the command
	p = sret + 1;								// point to the start of the char array
	*sret = nbr;								// set the length of the returned string
	while(nbr) {
		if(MMfeof(fnbr)) break;
		*p++ = MMfgetc(fnbr);						// get the char and save in our returned string
		nbr--;
	}
	*sret -= nbr;								// correct if we get less than nbr chars	
}



void cmd_files(void) {
	extern int VRes;
	#define MAXFILES 200
	SearchRec file;
	int r, i;
	char b[512];								// must be big enough to take a full flash filesystem listing
	char *p,c;
	struct s_flist {
		char fn[14];
		int fs;
	} flist[MAXFILES];
	int fcnt;
	char ts[14] = "";
	
	if(*cmdline) 
		p = getCstring(cmdline);
	else
		p = ts;
	
	// is this the flash filesystem?
	if((p[0] == 0 && DefaultDrive == FLASHFS) || (p[1] == ':' && toupper(p[0]) == 'A') || (p[1] != ':' && DefaultDrive == FLASHFS)) {
		FlashList(b);
		MMPrintString(b);
		return;
	}
	
	if((p[1] == ':' && p[2] == 0) || *p == 0) strcat(p, "*.*");		// add wildcard if needed
	GetDrive(p);								// use this function to strip the drive and make uppercase
	if(!InitSDCard()) return;
	
	FSgetcwd(b, STRINGSIZE);
	MMPrintString("Directory: "); MMPrintString(b); MMPrintString("\r\n");
	
	fcnt = 0;
#ifdef OLIMEX
	P_LED_OUT = ~P_LED_OUT;
#endif
	r = FindFirst (p, ATTR_HIDDEN | ATTR_SYSTEM | ATTR_READ_ONLY | ATTR_DIRECTORY |ATTR_ARCHIVE, &file);
#ifdef OLIMEX
	P_LED_OUT = ~P_LED_OUT;
#endif
	while(!r) {
		if(ErrorCheck()) return;
		if(fcnt >= MAXFILES) error("Too many files to list");
		if(file.attributes & ATTR_DIRECTORY) 
			ts[0] = 'D';
		else
			ts[0] = 'F';
		strcpy(&ts[1], file.filename);
		for(i = fcnt; i > 0; i--) {
			if(strcmp(flist[i - 1].fn, ts) > 0)
				flist[i] = flist[i - 1];
			else
				break;
		}
		strcpy(flist[i].fn, ts);
		flist[i].fs = file.filesize;
		fcnt++;
		r = FindNext(&file);
		SDActivityLED = SDActivityTime;
	}
	ListCnt = 2;
	for(i = 0; i < fcnt; i++) {
		MMPrintString((flist[i].fn + 1));
		while(MMCharPos < 15) MMputchar(' ');
		if(flist[i].fn[0] == 'D') 
			strcpy(b,"       <DIR>");
		else
			sprintf(b, "%12d", flist[i].fs);
		MMPrintString(b);
		MMPrintString("\r\n");
		// check if it is more than a screenfull
		if(ListCnt >= (VRes / (fontHeight * fontScale)) && i < fcnt - 1) {
			MMPrintString("PRESS ANY KEY OR Q TO STOP");
			c=MMgetchar();
			MMPrintString("\r                          \r");
                        if(c=='q' || c=='Q')  longjmp(mark, 1); // jump back to the input prompt added KSD
                        else
                        ListCnt = 1;
		}	
	}	
}




void cmd_mkdir(void) {
	char *p;
	
	p = getCstring(cmdline);										// get the directory name and convert to a standard C string
	if(GetDrive(p) == FLASHFS) error("Command not valid on A:");	
	if(!InitSDCard()) return;
	if(SDFileExists(p)) { ErrorThrow(7); return; }
	SDActivityLED = SDActivityTime;
	FSmkdir(p);
	ErrorCheck();
}	



void cmd_rmdir(void){
	char *p;
	
	p = getCstring(cmdline);										// get the directory name and convert to a standard C string
	if(GetDrive(p) == FLASHFS) error("Command not valid on A:");	
	if(!InitSDCard()) return;
	if(!SDFileExists(p)) { ErrorThrow(7); return; }
	SDActivityLED = SDActivityTime;
	FSrmdir(p, false);
	ErrorCheck();
}



void cmd_chdir(void){
	char *p;

	p = getCstring(cmdline);										// get the directory name and convert to a standard C string
	if(GetDrive(p) == FLASHFS) error("Command not valid on A:");	
	if(!InitSDCard()) return;
	SDActivityLED = SDActivityTime;
	FSchdir(p);
	ErrorCheck();
}




void fun_eof(void) {
	getargs(&ep, 1, ",");
	if(argc == 0) error("Invalid syntax");
	if(*argv[0] == '#') argv[0]++;
	fret = (float)MMfeof(getinteger(argv[0]));
}	



void fun_loc(void) {
	int fnbr, i;
	getargs(&ep, 1, ",");
	if(argc == 0) error("Invalid syntax");
	if(*argv[0] == '#') argv[0]++;
	fnbr = getinteger(argv[0]);
    if (MMFilePtr[fnbr - 1] == COM1_FILE_POINTER)
        i = SerialRxStatus(1);
    else if (MMFilePtr[fnbr - 1] == COM2_FILE_POINTER)
        i = SerialRxStatus(2);
#ifdef OLIMEX
    else if (MMFilePtr[fnbr - 1] == COM3_FILE_POINTER)
        i = SerialRxStatus(3);
    else if (MMFilePtr[fnbr - 1] == COM4_FILE_POINTER)
        i = SerialRxStatus(4);
#endif
	else
		error("Must be an open COM port");
	if(i > MAXSTRLEN) i = MAXSTRLEN;
	fret = (float)i;
}	



void fun_lof(void) {
	int fnbr, i;
	getargs(&ep, 1, ",");
	if(argc == 0) error("Invalid syntax");
	if(*argv[0] == '#') argv[0]++;
	fnbr = getinteger(argv[0]);
    if (MMFilePtr[fnbr - 1] == COM1_FILE_POINTER)
        i = com1_buf_size - SerialTxStatus(1);
    else if (MMFilePtr[fnbr - 1] == COM2_FILE_POINTER)
        i = com2_buf_size - SerialTxStatus(2);
#ifdef OLIMEX
    else if (MMFilePtr[fnbr - 1] == COM3_FILE_POINTER)
        i = com3_buf_size - SerialTxStatus(3);
    else if (MMFilePtr[fnbr - 1] == COM4_FILE_POINTER)
        i = com4_buf_size - SerialTxStatus(4);
#endif
	else
		error("Must be an open COM port");
	fret = (float)i;
}	



void fun_cwd(void) {
	if(DefaultDrive == FLASHFS) 
		sret ="\1\\";
	else
		sret = MMgetcwd();
}
	


void cmd_kill(void){
	char *p;

	p = getCstring(cmdline);										// get the file name and convert to a standard C string
	
	if(GetDrive(p) == FLASHFS)
		FlashFileKill(p);
	else {
		if(!InitSDCard()) return;
		SDActivityLED = SDActivityTime;
		FSremove(p);
		ErrorCheck();
	}	
}



void cmd_name(void) {
	FSFILE *fp;
	char *old, *new, ss[2];
	ss[0] = tokenvalue[TKN_AS];										// this will be used to split up the argument line
	ss[1] = 0;
	{																// start a new block
		getargs(&cmdline, 3, ss);									// getargs macro must be the first executable stmt in a block
		if(argc != 3) error("Invalid syntax");
		old = getCstring(argv[0]);									// get the old file name and convert to a standard C string
		new = getCstring(argv[2]);									// get the new file name and convert to a standard C string
		if(GetDrive(old) == FLASHFS) {
			if(GetDrive(new) != FLASHFS) error("Cannot rename across drives");
			FlashRename(old, new);
		} else {	
			if(GetDrive(new) != SDFS) error("Cannot rename across drives");
			if(strlen(old) == 0 || strlen(old) > 12 || strlen(new) == 0 || strlen(new) > 12) { ErrorThrow(5); return; }
			if(!InitSDCard()) return;
			if(!SDFileExists(old)) { ErrorThrow(6); return; }
			if(SDFileExists(new)) { ErrorThrow(9); return; }
			fp = FSfopen(old, "r");
			if(ErrorCheck() || fp == NULL) return;
			SDActivityLED = SDActivityTime;
			FSrename(new, fp);
			if(ErrorCheck()) return;
			FSfclose(fp);
		}	
	}
}	


void fun_errno(void) {
	fret = (float)MMerrno;
}
	

void cmd_drive(void){
	char *p;
	
	p = getCstring(cmdline);
	makeupper(p);
	if(*p == 'B')
		DefaultDrive = SDFS;
	else
		if(*p == 'A')
			DefaultDrive = FLASHFS;
		else
			error("Unrecognised drive letter");
}



void fun_mmdrive(void) {
	if(DefaultDrive == FLASHFS)
		sret = "\2A:";
	else
		sret = "\2B:";
}	



void fun_mmfname(void) {
	char *tp;
	tp = GetTempStringSpace();						// this will last for the life of the command
	strcpy(tp, LastFile);							// get the string and save in a temp place
	CtoM(tp);								// convert to a MMBasic style string
	sret = tp;
}	



/*******************************************************************************************
********************************************************************************************

Utility routines for the file I/O commands in MMBasic

********************************************************************************************
********************************************************************************************/



// use standard I/O functions if not compiling for the MAXIMITE
//#ifndef MAXIMITE
//
//int MMCharPos;
//
//char MMGetc() {
//	char c;
//	c = getchar();
//	if(c == '\n' || c == '\r')
//		MMCharPos = 1;
//	return c;
//}
//
//char MMPutc(char c) {
//	MMCharPos++;
//	if(c == '\n') {
//		putchar('\r');
//		MMCharPos = 1;
//	}
//	return putchar(c);
//}
//
//
//#endif
//


// get a line from the keyboard or a file handle
// most of the keyboard input is handled by EditInputLine() but it will use this function 
// if a line greater than he screen width is being entered.
// IMPORTANT: This will append to the buffer pointed to by p, so (if you don't want this)
//            make sure that the first char of p is zero before calling this.
void MMgetline(int filenbr, char *p) {
	int nbrchars;
	unsigned char c, *tp;

	nbrchars = strlen(p);											// the line might not be empty and we want to add to the end
	p += nbrchars;

	while(1) {
		
		CheckAbort();												// jump right out if CTRL-C
		
		// ignore EOF on the COM ports
		if(MMFilePtr[filenbr - 1] == COM1_FILE_POINTER && SerialRxStatus(1) == 0) continue;
		if(MMFilePtr[filenbr - 1] == COM2_FILE_POINTER && SerialRxStatus(2) == 0) continue;
#ifdef OLIMEX
		if(MMFilePtr[filenbr - 1] == COM3_FILE_POINTER && SerialRxStatus(3) == 0) continue;
		if(MMFilePtr[filenbr - 1] == COM4_FILE_POINTER && SerialRxStatus(4) == 0) continue;
#endif
		if(MMfeof(filenbr)) break;									// end of file - stop collecting

		c = MMfgetc(filenbr);
		
		if(filenbr == 0 && c >= F1 && c <= F12) {					// expand if a function key
			for(tp = FunKey[c - 0x91]; *tp; ) {
				if(++nbrchars > MAXSTRLEN) error("Line is too long");
				if(*tp == '\n') break;
				MMfputc(*tp, 0);
				*p++ = *tp++;
			}
			continue;
		}		
				
		
		if(c == '\t') {							// expand tabs to spaces
			 do {
				if(++nbrchars > MAXSTRLEN) error("Line is too long");
				*p++ = ' ';
				if(filenbr == 0) MMfputc(' ', 0);
			} while(nbrchars % 8);	
			continue;
		}
		
		if(c == '\b') {							// handle the backspace
			if(nbrchars) {
				if(filenbr == 0) MMfputs("\3\b \b", 0);
				nbrchars--;
				p--;
			}
			continue;
		}
		
		if(c == '\r') {
			continue;						// skip a lf (it should follow a cr)
		}
				
		if(c == '\n') {
			if(filenbr == 0) MMfputs("\2\r\n", 0);
			break;							// end of the line - stop collecting
		}
		
		if(isprint(c)) {
			if(filenbr == 0) MMfputc(c, 0);				// Maximite requires that chars be specificially echoed
		}	
		if(++nbrchars > MAXSTRLEN) error("Line is too long");		// stop collecting if maximum length
		*p++ = c;							// save our char
	}
	*p = 0;
}



void CheckAbort(void) {
	if(MMAbort) {
		longjmp(mark, 1);
	}	
}	


// output a string to a file
// the string must be a MMBasic string
void MMfputs(char *p, int filenbr) {
	int i;
	i = *p++;
	while(i--) MMfputc(*p++, filenbr);
}



// return the drive designation
// this will:
//   - find the drive prefix (A: or B:) in the argument and remove it from the string
//   - throw an error if the prefix is not a valid drive
//   - throw an error if the file name is empty or too long
//   - return with the drive designation (ie, FLASHFS or SDCARD)
//   - if the prefix is not specified this will return with the current default drive.
int GetDrive(char *p) {
	int fs;
	
	makeupper(p);
	if(*p && p[1] == ':') {
		if(p[0] == 'B')
			fs = SDFS;
		else
			if(p[0] == 'A')
				fs = FLASHFS;
			else
				error("Unrecognised drive letter");
		memmove(p, p+2, strlen(p));
	} else
		fs = DefaultDrive;
	if(!*p || strlen(p) > 12) error("Invalid Filename");
	return fs;
}



// fname must be a standard C style string (not the MMBasic style)
void MMfopen(char *fname, char *mode, int fnbr) {
	if(fnbr < 1 || fnbr > 10) error("Invalid file number");
	fnbr--;
	if(GetDrive(fname) == FLASHFS) {
		// Flash filesystem
		if(FlashStatus != CLOSED) error("Only one internal flash file can be open at a time");
		if(*mode == 'r') FlashOpenRead(fname);
		if(*mode == 'w') FlashOpenWrite(fname);
		if(*mode == 'a') FlashOpenAppend(fname);
		MMFilePtr[fnbr] = (FSFILE *)FLASHFS;
	} else {
		// SD card filesystem
		if(!InitSDCard()) return;
		if(MMFilePtr[fnbr] != NULL) error("File number is already open");
		makeupper(fname);
		if(*mode == 'r' && !SDFileExists(fname)) { ErrorThrow(6); return; }
		if(*mode == 'w' && SD_WE) { ErrorThrow(2); return; }
		if(year > 2010) SetClockVars(year, month, day, hour, minute, second);
		SDActivityLED = SDActivityTime;
		MMFilePtr[fnbr] = FSfopen(fname, mode);
		if(ErrorCheck()) return;
		if(MMFilePtr[fnbr] == NULL) ErrorThrow(9);
	}	
}


void MMfread(void *ptr, size_t size, size_t n, int fnbr)
{
                FSfread(ptr,size,n,MMFilePtr[fnbr]);
}
	

void MMfclose(int fnbr) {
	if(fnbr < 1 || fnbr > 10) error("Invalid file number");
	fnbr--;
    if (MMFilePtr[fnbr] == COM1_FILE_POINTER)
        SerialClose(1);
    else if (MMFilePtr[fnbr] == COM2_FILE_POINTER)
        SerialClose(2);
#ifdef OLIMEX
    else if (MMFilePtr[fnbr] == COM3_FILE_POINTER)
        SerialClose(3);
    else if (MMFilePtr[fnbr] == COM4_FILE_POINTER)
        SerialClose(4);
#endif
	else if((int)MMFilePtr[fnbr] == FLASHFS) {
		// Flash filesystem
		if(FlashStatus == OPENREAD)
			FlashCloseRead();
		else if(FlashStatus == OPENWRITE)
			FlashCloseWrite();
	} else {
		// SD card filesystem
		if(!InitSDCard()) return;
		if(MMFilePtr[fnbr] == NULL) error("File number is not open");
		if(year > 2010) SetClockVars(year, month, day, hour, minute, second);
		SDActivityLED = SDActivityTime;
		FSfclose(MMFilePtr[fnbr]);
		MMFilePtr[fnbr] = NULL;
		ErrorCheck();
	}	
	MMFilePtr[fnbr] = NULL;
}	



unsigned char MMfgetc(int fnbr) {
	unsigned char ch;
	if(fnbr < 0 || fnbr > 10) error("Invalid file number");
	if(fnbr == 0) return MMgetchar();
	fnbr--;
   	if(MMFilePtr[fnbr] == COM1_FILE_POINTER) return SerialGetchar(1);
	if(MMFilePtr[fnbr] == COM2_FILE_POINTER) return SerialGetchar(2);
#ifdef OLIMEX
        if (MMFilePtr[fnbr] == COM3_FILE_POINTER) return SerialGetchar(3);
        if (MMFilePtr[fnbr] == COM4_FILE_POINTER) return SerialGetchar(4);
#endif
	if((int)MMFilePtr[fnbr] == FLASHFS)  	 return FlashGetc();		// Flash filesystem

	// SD card filesystem
	if(!InitSDCard()) return 0;
	if(MMFilePtr[fnbr] == NULL) error("File number is not open");
	SDActivityLED = SDActivityTime;
#ifdef OLIMEX
	P_LED_OUT = ~P_LED_OUT;
#endif
	if(FSfread(&ch, 1, 1, MMFilePtr[fnbr]) == 0) ch = 0xff;
#ifdef OLIMEX
	P_LED_OUT = ~P_LED_OUT;
#endif
	ErrorCheck();
	return ch;
}	
	
	

char MMfputc(char c, int fnbr) {
	static char t;
	static int nbr;

	if(fnbr < 0 || fnbr > 10) error("Invalid file number");
	if(fnbr == 0) return MMputchar(c);
	fnbr--;
	if(MMFilePtr[fnbr] == COM1_FILE_POINTER) return SerialPutchar(1, c);
	if(MMFilePtr[fnbr] == COM2_FILE_POINTER) return SerialPutchar(2, c);
#ifdef OLIMEX
	if (MMFilePtr[fnbr] == COM3_FILE_POINTER) return SerialPutchar(3, c);
	if (MMFilePtr[fnbr] == COM4_FILE_POINTER) return SerialPutchar(4, c);
#endif
	if((int)MMFilePtr[fnbr] == FLASHFS) {
		// Flash filesystem
		FlashPutc(c);
		return c;
	} else {	
		// SD card filesystem
		t = c;
		nbr = fnbr;
		if(!InitSDCard()) return 0;
		if(MMFilePtr[fnbr] == NULL) error("File number is not open");
		SDActivityLED = SDActivityTime;
#ifdef OLIMEX
		P_LED_OUT = ~P_LED_OUT;
#endif
		if(FSfwrite(&t, 1, 1, MMFilePtr[nbr]) == 0) if(ErrorCheck() == 0) ErrorThrow(9);
#ifdef OLIMEX
		P_LED_OUT = ~P_LED_OUT;
#endif
		return t;
	}	
}	



int MMfeof(int fnbr) {
	int i;
	if(fnbr < 0 || fnbr > 10) error("Invalid file number");
	if(fnbr == 0) return 0;
	fnbr--;
	if(MMFilePtr[fnbr] == COM1_FILE_POINTER) return SerialRxStatus(1) == 0;
	if(MMFilePtr[fnbr] == COM2_FILE_POINTER) return SerialRxStatus(2) == 0;
#ifdef OLIMEX
        if (MMFilePtr[fnbr] == COM3_FILE_POINTER) return SerialRxStatus(3) == 0;
        if (MMFilePtr[fnbr] == COM4_FILE_POINTER) return SerialRxStatus(4) == 0;
#endif
	if((int)MMFilePtr[fnbr] == FLASHFS) 	 return FlashEOF;		// Flash filesystem
	
	// SD card filesystem
	if(!InitSDCard()) return 0;
	if(MMFilePtr[fnbr] == NULL) error("File number is not open");
	i = (FSfeof(MMFilePtr[fnbr]) != 0) ? -1 : 0;
	ErrorCheck();
	return i;
}	
	


char *MMgetcwd(void) {
	char *b;
	if(!InitSDCard()) return 0;
	b = GetTempStringSpace();
	FSgetcwd(b, STRINGSIZE);
	ErrorCheck();
	CtoM(b);														// convert to a MMBasic string
	return b;
}	
	


void CloseAllFiles(void) {
	int i, prev;
	if(year > 2010) SetClockVars(year, month, day, hour, minute, second);
	prev = OptionErrorAbort;
	OptionErrorAbort = false;										// don't abort on error
	for(i = 0; i < MAXOPENFILES; i++) {
		if(MMFilePtr[i] != NULL) MMfclose(i + 1);					// try and close a file
		MMFilePtr[i] = NULL;										// make sure that the entry is removed
	}
	OptionErrorAbort = prev;
}			

void cmd_msdon(void)
{
    if(!S.SDEnable) error("SD Card Is Disabled");
    MSDEnable=true;
    LUNSoftAttach(0);
    MMPrintString("Hit a key when done\n\r");
    MMgetchar();
    LUNSoftDetach(0);
    MSDEnable=false;

}
void cmd_msdoff(void)
{
    MSDEnable=false;
    LUNSoftDetach(0);
}
void cmd_sdformat(void)
{
    if(!FSInit()) MMPrintString("Fsinit failed\n\r");
    MMPrintString("Start Format\n\r");
    FSformat (1, 0, "DMBasic");
    MMPrintString("End Format\n\r");
}
extern int MDD_SDSPI_MediaDetect(void);								// MDD_MediaDetect is a #define for this function

int InitSDCard(void) {
	#define MDD_MediaDetect         MDD_SDSPI_MediaDetect
	int i;
#ifdef OLIMEX

        if(!S.SDEnable) return false;
#endif
        ErrorThrow(0);				// reset mm.errno to zero
	if(SDCardRemoved == false) return true;
	for(i = 0; i < MAXOPENFILES; i++) MMFilePtr[i] = NULL;	            // make sure that the table is empty
//	if(!MDD_MediaDetect()) { ErrorThrow(1); return false; }             //DuinoMite does not have a card detect
	if(!FSInit())  { SDCardRemoved = true; ErrorThrow(15); return false; }
	SDCardRemoved = false;
	SDActivityLED = SDActivityTime;
	return true;
}	
	
	
	
int SDFileExists(char *fname) {
	SearchRec file;
	int r;
	
	if(InitSDCard() == false) return 0;
	r = FindFirst(fname, ATTR_HIDDEN | ATTR_SYSTEM | ATTR_READ_ONLY | ATTR_DIRECTORY |ATTR_ARCHIVE, &file)?0:1;
        if(r) return file.filesize;
        else
        return r;

}	


// finds the first available free file number.  Returns with zero if no free file numbers
int FindFreeFileNbr(void) {
	int i;
	for(i = 0; i < MAXOPENFILES; i++) 
		if(MMFilePtr[i] == NULL) return i + 1;
	return 0;
}	



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// flash filesystem //////////////////////////////////////////////////
/*

Structure of the flash filesystem
=================================
This filesystem is designed to be simple to implement, work with the PIC32 flash memory and spread the wear 
load on the flash.

Each page of flash memory is 4096 bytes.  A file will occupy one or more pages.
Each page has the following structure:
  unsigned char 			Block Number (01 = first block, 02 = second, etc)  00 means that the block is erased.
  unsigned char[15] 		File Name    (an ascii string terminated with a zero byte, eg:  "FILE.BAS")
  unsigned word				Generation   (starts at 0xff and counts down)
  unsigned char[4076]  		Data		 (can be any bit pattern except 0xff which is used to detect end of file.
                                          0xff followed by another 0xff is eof while 0x0ff followed by anything 
                                          else is the single char 0xff)

There is no file table or FAT in this filesystem.  Each block contains all the information needed (the Block 
Number and File Name).  The first block allocated to a file is given a Block Number of 01, the second 02, etc.
All the blocks in a file can be found by searching for the File Name (which is imbedded in each block) and then
checking if the Block Number matches the particular block being looked for.

When a file is erased its Block Number is over written with zero.  This can be done without erasing as a write to
flash can change bits to zero without an erase (the erase sets all bits back to ones).

The Generation word is used for wear leveling.  Everytime a block is written this word is decremented and written 
with the block.  When searching for a page to reuse the software must use a block with Block Number = 0x00 or 0xff
(which indicates that the block is free) and the highest Generation number.  This ensures that page erase will be
distributed across the whole of the flash memory.

When a free block is found it may need erasing.  The PIC32 flash memory can only be erased in pages of 4096 bytes
which is why the filesystem page size is the same.  When the page is erased all bits are set to one.  When writing
to flash bits are set to zero.  Each write must be in a word (4 bytes).  The write routines buffer each byte to be 
written until 4 are accumulated then all 4 are written simultaneously.

********************************************************************************************************************************/


// note:  in the project -funsigned-char must be defined



//////////////////////////////////////////////////////////////////////////////////////////////
// Macro to reserve flash memory for saving/loading data and initialise to 0xFF's
// Note that “bytes” need to be a multiple of:-
//  - BYTE_PAGE_SIZE (and aligned that way) if you intend to erase
//  - BYTE_ROW_SIZE (and aligned that way) if you intend to write rows
//  - sizeof(int) if you intend to write words
//
// allocate space in flash for program saves.  "ProgMem" is aligned on erasable page boundary
#define NVM_ALLOCATE(name, align, bytes) volatile unsigned char name[(bytes)] \
	__attribute__ ((aligned(align),section(".text,\"ax\",@progbits #"))) = \
	{ [0 ...(bytes)-1] = 0xFF }
NVM_ALLOCATE(Flash, FLASH_PAGE_SIZE, FLASH_PAGE_SIZE * PROGMEM_NBR_PAGES);
//
///////////////////////////////////////// globals ///////////////////////////////////////////

union {
	unsigned int wrd;
	unsigned char ch[4];
} WBuf;

int WBufCnt;
volatile unsigned char *WBufPtr;
unsigned char fnbuf[FILENAME_LENGTH+1];
unsigned char bnbr;
volatile unsigned char *RPtr;
int RCnt;


///////////////////////// utility routines used to access the flash /////////////////////////

void FlashWriteWord(volatile unsigned char *p, int i) {
	int r;

	INTDisableInterrupts();
	uSec(100);														// wait for the DMA to stop
	r = NVMWriteWord((void *)p, i);									// write the word
	INTEnableInterrupts();
	if(r != 0) ErrorThrow(16);
}	


void FlashWriteByte(char c) {
	if(WBufPtr == NULL) return;
	WBuf.ch[WBufCnt % 4] = c;
	WBufCnt++;
	if((WBufCnt % 4) == 0) {										// if we have filled up a word (4 bytes)
			FlashWriteWord(WBufPtr, WBuf.wrd);
			WBufPtr += 4;
	}
}	


///////////////// utility functions specific to saving/loading program files //////////////////

volatile struct structBlock *FindFlashBlock(char* fn, int block) {
	int i;
	volatile struct structBlock *b;
	b = (volatile struct structBlock *)Flash;

	for(i = 0; i < PROGMEM_NBR_PAGES; i++, b++)
		if(b->bnbr == block && strncasecmp(fn, (unsigned char *)b->fname, FILENAME_LENGTH) == 0)
			return b;
	return NULL;
}	


volatile struct structBlock *FindFreeFlashBlock(void) {
	int i, r, t;
	volatile struct structBlock *b, *n;

	// first search for a block that has never been used
	b = (volatile struct structBlock *)Flash;
	for(i = 0; i < PROGMEM_NBR_PAGES; i++, b++)	
		if(b->bnbr == 0xff) return b;				// and return it if found

	// find the erased block with the highest generation number
	t = 0; n = NULL;
	b = (volatile struct structBlock *)Flash;
	for(i = 0; i < PROGMEM_NBR_PAGES; i++, b++)
		if(b->bnbr == 0 && b->gnbr > t) {
			t = b->gnbr;											// save the generation number
			n = b;													// and save the pointer incase this is the one
		}
	if(n == NULL) {
		ErrorThrow(3);												// cannot find any free space
		return n;
	}	
	
	// erase it
	INTDisableInterrupts();
	uSec(100);														// wait for the DMA to stop
	r = NVMErasePage((void *)n);									// erase
	INTEnableInterrupts();
	if(r != 0) ErrorThrow(16);										// hardware error
	return n;														// otherwise return the block pointer
}	


unsigned int GetNextGeneration(void) {
	volatile struct structBlock *b;
	unsigned int i, t;

	b = (volatile struct structBlock *)Flash;
	t = 0xffffffff;
	for(i = 0; i < PROGMEM_NBR_PAGES; i++, b++)
		if(b->gnbr < t) t = b->gnbr;
	return t - 1;
}


///////////////////////////////////// intermediate routines for low level access ///////////////////////////

void FlashAllocateNewBlock(void) {
	int i;

	bnbr++;														// increment the block number		
	//	GS - Flash fix Start
	WBufCnt = 0;
	//	GS - Flash fix End
	WBufPtr = (unsigned char *)FindFreeFlashBlock();			// find a free block
	if(WBufPtr == NULL) {
		FlashStatus = CLOSED;
		ErrorThrow(3);											// out of space
		return;
	}	
	FlashWriteByte(bnbr);										// write the block number
	for(i = 0; i < FILENAME_LENGTH; i++)
		FlashWriteByte(fnbuf[i]);								// write the filename
	FlashWriteWord(WBufPtr, GetNextGeneration());				// write the generation number
	WBufPtr += 4; 
	WBufCnt += 4;
}


void FlashWriteNextByte(char c) {
	if((WBufCnt % FLASH_PAGE_SIZE) == 0) 
		FlashAllocateNewBlock(); 								// allocate a new block if we have filled the current one
	FlashWriteByte(c);
}


unsigned char FlashGetNextByte(void) {
	volatile struct structBlock *b;

//	GS - Flash fix Start
	if (RCnt == FLASH_PAGE_SIZE) {
//	GS - Flash fix End
		bnbr++;
		b = FindFlashBlock(fnbuf, bnbr);
		if (b == NULL) {FlashEOF = true; return 0;}
		RPtr = b->dat;
//	GS - Flash fix Start
		RCnt = (FLASH_PAGE_SIZE - PROG_DATA_LENGTH);;
//	GS - Flash fix End
	//	dump((char *)b, 16 *8);
	}
	RCnt++;
	return *RPtr++;
}	


int FlashFreeSpace(void) {
	int i, free;
	volatile struct structBlock *b;

	b = (volatile struct structBlock *)Flash;	
	for(free = i = 0; i < PROGMEM_NBR_PAGES; i++, b++)
		if(b->bnbr == 0 || b->bnbr == 0xff) free += PROG_DATA_LENGTH;
	return free;
}	


/////////////////////////////////////// routines used by the fine I/O interface functions (below) //////////////////////////////////

void FlashList(char *s) {
	int i, j;
	volatile struct structBlock *b;
	b = (volatile struct structBlock *)Flash;

	SDActivityLED = SDActivityTime;
	for(j = i = 0; i < PROGMEM_NBR_PAGES; i++, b++) {
		if(b->bnbr == 1) {
			strcpy(s, (unsigned char *)b->fname);
			s += strlen((unsigned char *)b->fname);		
			*s++ = '\r';
			*s++ = '\n';
			j++;
		}
	}
	sprintf(s, "%d file%s, %d bytes free\r\n", j, (j==1?"":"s"), FlashFreeSpace());
}


int FlashFileKill(char *fn) {
	int i, ok;
	volatile struct structBlock *b;

	ok = false;
	for(i = 1; i < PROGMEM_NBR_PAGES + 1; i++) {
		b = FindFlashBlock(fn, i);									// find any occurances of that filename
		if(b != NULL) {
			FlashWriteWord((unsigned char *)b, 0);
			ok = true;
		}	
	}
	return ok;
}


void FlashOpenWrite(char *fn) {
	strncpy(fnbuf, fn, FILENAME_LENGTH - 1);						// and the file name
	fnbuf[FILENAME_LENGTH - 1] = 0;									// terminate the string if the name was too long
	FlashFileKill(fnbuf);											// delete any file with the same name

	bnbr = 0;														// put the block number in the buffer
	WBufCnt = 0;													// start counting from the start of the page
	FlashAllocateNewBlock(); 										// allocate the first block
	if(WBufPtr == NULL) {
		return;
	}	
	FlashEOF = false;
	FlashStatus = OPENWRITE;
}


void FlashOpenRead(char *fn) {
	volatile struct structBlock *b;
	
	bnbr = 1;														// put the block number in the buffer
	strncpy(fnbuf, fn, FILENAME_LENGTH - 1);						// and the file name
	fnbuf[FILENAME_LENGTH - 1] = 0;									// terminate the string if the name was too long
	b = FindFlashBlock(fnbuf, bnbr);
	if(b == NULL) {
		ErrorThrow(6);												// not found
		return;
	}	
	RPtr = b->dat;
//	GS - Flash fix Start
	RCnt = (FLASH_PAGE_SIZE - PROG_DATA_LENGTH);;
//	GS - Flash fix End
	FlashEOF = false;
	FlashStatus = OPENREAD;
}


void FlashOpenAppend(char *fn) {
	volatile struct structBlock *b;

	bnbr = 1;														// put the block number in the buffer
	strncpy(fnbuf, fn, FILENAME_LENGTH - 1);						// and the file name
	fnbuf[FILENAME_LENGTH - 1] = 0;									// terminate the string if the name was too long
	b = FindFlashBlock(fnbuf, bnbr);
	if(b == NULL) {
		ErrorThrow(6);												// file not found
		return;
	}	
	RPtr = b->dat;
//	GS - Flash fix Start
	RCnt = (FLASH_PAGE_SIZE - PROG_DATA_LENGTH);
	FlashEOF = false;
	WBufCnt = RCnt;
	WBufPtr = RPtr;
	while(!FlashEOF) FlashGetEOF();									// search for the end of file
//	GS - Flash fix End
	FlashEOF = false;
	FlashStatus = OPENWRITE;
}


void FlashRename(char *old, char *new) {
	int i, j;
	volatile struct structBlock *b;
	volatile unsigned char *p;

	if(FlashStatus != CLOSED) error("Only one flash file can be open at a time");
	
	// copy the old to the new, page by page
	j = 1;
	while((b = FindFlashBlock(old, j)) != NULL) {						// while there are pages in the file
		//char *yy;
		//dump((char *)b, 16*8);
		p = b->dat;														// point to the data in that page
		WBufPtr = (unsigned char *)FindFreeFlashBlock();				// point to the flash to be written to
		if(WBufPtr == NULL) {
			ErrorThrow(3);												// error free block not found
			return;														// error return
		}	
		WBufCnt = 0;													// start counting from the start of the page
		FlashWriteByte(j);												// write the block number
		for(i = 0; i < FILENAME_LENGTH; i++) {
			FlashWriteByte(new[i]);										// write the new file name
		}
		FlashWriteWord(WBufPtr, GetNextGeneration());					// write the generation number
		WBufPtr += 4;													// point to the data
		while(WBufCnt < FLASH_PAGE_SIZE) FlashWriteByte(*p++);			// copy to the new page with the new filename
	//	dump(yy, 16*8);
		FlashWriteWord((volatile unsigned char *)b, 0);					// erase the old page with the old filename
		//dump((char *)b, 16*8);
		j++;
	}
	if(j == 1) ErrorThrow(6);											// error file not found
}	


//	GS - Flash fix Start
// get EOF position
void FlashGetEOF(void) {
	unsigned char c, cc;

	SDActivityLED = SDActivityTime;
	WBuf.ch[WBufCnt % 4] = c = FlashGetNextByte();					// get the char
	if (FlashEOF) return;																		// check incase we already have reached eof
	if (c == 0xff) {
		cc = FlashGetNextByte();															// and get the following char if 0xff
		if (FlashEOF) return;																	// check incase we already have reached eof
		if (cc == 0xff) {
			FlashEOF = true;
			return;
		} else {
			WBuf.ch[++WBufCnt % 4] = cc;
		}
	}
	WBufCnt = RCnt;
	WBufPtr = RPtr;
	return;
}	
//	GS - Flash fix End


// get a character
// end of file is returned by setting eof true
// includes special processing for 0xff
//   0xff followed by another 0xff is eof
//   0x0ff followed by anything else is the single char 0xff
// end of file is returned by setting eof true
unsigned char FlashGetc(void) {
	unsigned char c;
	int RCntTmp, bnbrTmp;
	volatile unsigned char *RPtrTmp;

	SDActivityLED = SDActivityTime;
	if(FlashEOF) return 0;											// safety check incase we already have reached eof
	c = FlashGetNextByte();											// get the char
	if(c == 0xff) FlashGetNextByte();								// and gobble the following char if 0xff
	RCntTmp = RCnt;  RPtrTmp = RPtr; bnbrTmp = bnbr;				// save where we are in the file
	FlashEOF = (FlashGetNextByte() == 0xff && FlashGetNextByte() == 0xff);  // test if we are at eof
	RCnt = RCntTmp;  RPtr = RPtrTmp; bnbr = bnbrTmp;				// restore position
	return c;
}	


// write a char to the flash file
// special processing for 0xff.  It must be followed by a zero byte indicating that it is not a eof flag
void FlashPutc(char c) {
	SDActivityLED = SDActivityTime;
	FlashWriteNextByte(c);
	if(c == 0xff) FlashWriteNextByte(0);
}


void FlashPutStr(char *s) {
	while(*s) FlashPutc(*s++);
}


void FlashCloseWrite(void) {
	FlashStatus = CLOSED;
	while((WBufCnt % 4) != 0) FlashWriteByte(0xff);						// while the buffer has something in it write a padding char
	FlashEOF = false;
}


void FlashCloseRead(void) {
	FlashStatus = CLOSED;
	FlashEOF = false;
}	
