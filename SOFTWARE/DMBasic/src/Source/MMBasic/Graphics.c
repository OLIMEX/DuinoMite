/***********************************************************************************************************************
MMBasic

custom.c

Handles all the commands and functions related to the video screen in MMBasic.

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

#include "../Video/Video.h"

void getcoord(char *p, int *x, int *y);

static int lastx = 0;			// the last x and y coordinates that were used
static int lasty = 0;



/*******************************************************************************************
Video graphics related commands in MMBasic
==========================================
These are the functions responsible for executing the graphics related  commands in MMBasic
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




void cmd_cls(void) {
	MMPrintString("\033[2J\033[H");									// vt100 clear screen and home cursor
	MMcls();														// same for the video
}


void cmd_circle(void) {
	int x, y, radius, colour, fill;
	getargs(&cmdline, 7, ",");
	if(argc%2 == 0 || argc < 3) error("Invalid syntax");
	if(*argv[0] != '(') error("Expected opening bracket");
	getcoord(argv[0] + 1, &x, &y);
	radius = getinteger(argv[2]);
	if(argc > 3)
		colour = (getinteger(argv[4]) != 0);
	else
		colour = 1;
	if(argc == 7)
		fill = (toupper(*argv[6]) == 'F');
	else
		fill = 0;
	MMCircle(x, y, radius, fill, colour);
	lastx = x; lasty = y;
}



void cmd_line(void) {
	int x1, y1, x2, y2, colour, box, fill;
	char *p;
	getargs(&cmdline, 5, ",");

	// check if it is actually a LINE INPUT command
	if(argc > 0 && checkstring(argv[0], "INPUT")) {					// check if it is actually is a line input command
		cmd_lineinput();
		return;
	}
	if(argc%2 == 0 || argc < 1) error("Invalid syntax");
	x1 = lastx; y1 = lasty; colour = 1; box = false; fill = false;	// set the defaults for optional components
	p = argv[0];
	if(tokenfunction(*p) != op_subtract) {
		// the start point is specified - get the coordinates and step over to where the minus token should be
		if(*p != '(') error("Expected opening bracket");
		getcoord(p + 1, &x1, &y1);
		p = getclosebracket(p + 1) + 1;
		skipspace(p);
	}
	if(tokenfunction(*p) != op_subtract) error("Invalid syntax");
	p++;
	if(*p != '(') error("Expected opening bracket");
	getcoord(p + 1, &x2, &y2);
	if(argc > 1)
		colour = getinteger(argv[2]);
	if(argc == 5) {
		box = (strchr(argv[4], 'b') != NULL || strchr(argv[4], 'B') != NULL);
		fill = (strchr(argv[4], 'f') != NULL || strchr(argv[4], 'F') != NULL);
	}
	if(box)
		MMbox(x1, y1, x2, y2, fill, colour);						// draw a box
	else
		MMline(x1, y1, x2, y2, colour);								// or just a line

	lastx = x2; lasty = y2;											// save in case the user wants the last value
}



void cmd_pset(void) {
	int x, y;
	skipspace(cmdline);
	if(*cmdline != '(') error("Expected opening bracket");
	getcoord(cmdline + 1, &x, &y);
	plot(x, y, 1);
	lastx = x; lasty = y;
}



void cmd_preset(void) {
	int x, y;
	skipspace(cmdline);
	if(*cmdline != '(') error("Expected opening bracket");
	getcoord(cmdline + 1, &x, &y);
	plot(x, y, 0);
	lastx = x; lasty = y;
}



void cmd_locate(void) {
	getargs(&cmdline, 3, ",");
	if(argc != 3) error("Invalid syntax");
	lastx = MMPosX = getinteger(argv[0]);
	lasty = MMPosY = getinteger(argv[2]);
}



void cmd_pixel(void){
	int x, y, value;
	getcoord(cmdline, &x, &y);
	cmdline = getclosebracket(cmdline) + 1;
	while(*cmdline && tokenfunction(*cmdline) != op_equal) cmdline++;
	if(!*cmdline) error("Invalid syntax");
	++cmdline;
	if(!*cmdline) error("Invalid syntax");
	value = getinteger(cmdline);
	plot(x, y, value);
	lastx = x; lasty = y;
}


void fun_pixel(void){
	int x, y;
	getargs(&ep, 3, ",");										// this is a macro and must be the first executable stmt in a block
	if(argc != 3) error("Invalid Syntax");
	x = getinteger(argv[0]);
	y = getinteger(argv[2]);
	fret = pixel(x, y);
}


void fun_hres(void) {
	fret = (float)HRes;
}

	
void fun_vres(void) {
	fret = (float)VRes;
}	


// utility routine used to write a number of bytes
// only used by cmd_savebmp() below
void xwrite(char *p, int nbr, int fnbr) {
	while(nbr--) MMfputc(*p++, fnbr);
}


// write a BMP file
// based on bmpsuite.c by Jason Summers (entropymine.com/jason/bmpsuite/)
// and pf1bit_bmp.c by Adam Majewski (fraktal.republika.pl)
void cmd_savebmp(void) {
	char *p;
	int fp, i, j;

	unsigned int   zero = 0,										// a zero integer to make it easier writing zero values
                   bpp = 1,											// bits per pixel
                   FileHeaderSize = 14, 							// the bmp header size in bytes
                   InfoHeaderSize = 40, 							// the DIB (image) header size in bytes
                   PaletteSize = pow(2,bpp)*4, 						// = 8 = number of bytes in palette
                   BytesPerRow,
                   FileSize,
                   OffBits,											// offset to the image
                   BytesSize; 										// number of bytes in the image

	// open the file
	p = getCstring(cmdline);
	if(strchr(p, '.') == NULL) strcat(p, ".BMP");
	if((fp = FindFreeFileNbr()) == 0) error("Too many files open");
	MMfopen(p, "w", fp);

	// calculate header information
    BytesPerRow=(((HRes * bpp)+31)/32)*4;
    BytesSize=BytesPerRow*VRes;
    FileSize=FileHeaderSize+InfoHeaderSize+PaletteSize+BytesSize;
    OffBits=FileHeaderSize+InfoHeaderSize+PaletteSize;

    // write the bmp file header
    xwrite("BM", 2, fp);											// file Type signature = BM
    xwrite((char *)&FileSize, 4, fp);								// FileSize
    xwrite((char *)&zero, 4, fp);									// two words, both zero for reserved 1 and 2
    xwrite((char *)&OffBits, 4, fp);								// offset to the image

    // write the DIB (image) header
    xwrite((char *)&InfoHeaderSize, 4, fp);							// size of the image header
    xwrite((char *)&HRes, 4, fp);									// width of image in pixels
    xwrite((char *)&VRes, 4, fp);									// height of image in pixels
    MMfputc(1, fp); MMfputc(0, fp); 								// number of planes = 1
    MMfputc(1, fp); MMfputc(0, fp); 								// bits of color per pixel = 1
    xwrite((char *)&zero, 4, fp);									// compression type = 0
    xwrite((char *)&zero, 4, fp);									// Image Data Size, set to 0 when no compression
    xwrite((char *)&zero, 4, fp);									// reserved
    xwrite((char *)&zero, 4, fp);									// reserved
	MMfputc(2, fp); MMfputc(0, fp); MMfputc(0, fp); MMfputc(0, fp);	// number of used colours = 2
    xwrite((char *)&zero, 4, fp);									// reserved 1

    // write the colour table (palette)
    // each colour is 4 bytes as Red, Green, Blue, Alpha (unused)
    // our palette has only 2 colors (black and white)
    // colour 0 = black
    MMfputc(0, fp); MMfputc(0, fp); MMfputc(0, fp); MMfputc(0, fp);
    // colour 1 = white
    MMfputc(255, fp); MMfputc(255, fp); MMfputc(255, fp); MMfputc(0, fp);

	// finally!!  write the video image.  note this must be inverted (ie start with the last row of pixels)
	for(i = VRes - 1; i >= 0; i--)
		for(j = 0; j < BytesPerRow; j++) 
			MMfputc(*((unsigned char *)&VA[(i * HBUFSIZE/32) + (j>>2)] + (3 - (j%4))), fp);

	MMfclose(fp);
}



/***********************************************************************************************
utility functions used by the custom commands
************************************************************************************************/


void getcoord(char *p, int *x, int *y) {
	char *tp, *ttp;
	char b[STRINGSIZE];

	tp = getclosebracket(p);
	*tp = 0;														// remove the closing brackets
	strcpy(b, p);													// copy the coordinates to the temp buffer
	*tp = ')';														// put back the closing bracket
	ttp = b;														// kludge (todo: fix this)
	{
		getargs(&ttp, 3, ",");										// this is a macro and must be the first executable stmt in a block
		if(argc != 3) error("Invalid Syntax");
		*x = getinteger(argv[0]);
		*y = getinteger(argv[2]);
	}
}

