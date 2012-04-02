/***********************************************************************************************************************
MMBasic

functions.c

Handles all the functions in MMBasic.

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/

#define INCLUDE_FUNCTION_DEFINES

#include <stdio.h>
#include "MMBasic.h"




/********************************************************************************************************************************************
 basic functions
 each function is responsible for decoding a basic function
 all function names are in the form fun_xxxx() so, if you want to search for the function responsible for the ASC() function look for fun_asc

 There are 4 globals used by these functions:

 char *ep       This is a pointer to the argument of the function
                Eg, in the case of INT(35/7) ep would point to "35/7)"

 fret           Is the return value for a basic function that returns a float

 sret           Is the return value for a basic function that returns a string

 tret           Is the type of the return value.  normally this is set by the caller and is not changed by the function

 ********************************************************************************************************************************************/




void fun_abs(void) {
	fret = fabsf(getnumber(ep));
}



void fun_asc(void) {
	char *s;

	s = getstring(ep);
	fret = (float)*(s + 1);
}




void fun_atn(void) {
	fret = atanf(getnumber(ep));
}




void fun_chr(void) {
	int i;

	sret = GetTempStringSpace();										// this will last for the life of the command
	i = getinteger(ep);
	if(i < 0 || i > 0xff) error("Number out of bounds");
	sret[0] = 1;
	sret[1] = i;
}




void fun_cint(void) {
	fret = (float)getinteger(ep);
}




void fun_cos(void) {
	fret = cosf(getnumber(ep));
}




void fun_exp(void) {
	fret = expf(getnumber(ep));
}




void fun_hex(void) {
	int i;

	sret = GetTempStringSpace();								// this will last for the life of the command
	i = getinteger(ep);
	sprintf(sret, "%X", i);
	CtoM(sret);
}



// syntax:  nbr = INSTR([start,] string1, string2)
//          find the position of string2 in string1 starting at start chars in string1
void fun_instr(void) {
	char *s1, *s2;
	int start;
	char ss[2] = {',', 0  };
	getargs(&ep, 5, ss);

	if(argc == 5) {
		start = getinteger(argv[0]) - 1;
		s1 = getstring(argv[2]);
		s2 = getstring(argv[4]);
		if(start < 0 || start > MAXSTRLEN) error("Number out of bounds");
	}
	else if(argc == 3) {
		start = 0;
		s1 = getstring(argv[0]);
		s2 = getstring(argv[2]);
	}
	else
		error("Incorrect number of arguments");

	if(start > *s1 - *s2 + 1 || *s2 == 0)
		fret = 0;
	else {
		// find s2 in s1 using MMBasic strings
		int i;
		for(i = start; i < *s1 - *s2 + 1; i++) {
			if(memcmp(s1 + i + 1, s2 + 1, *s2) == 0) {
				fret = i + 1;
				return;
			}
		}
	}
	fret = 0;
}




void fun_int(void) {
	fret = floorf(getnumber(ep));
}



void fun_fix(void) {
	fret = (float)((int)(getnumber(ep)));
}




void fun_left(void) {
	int i;
	char ss[2] = {',', 0  };
	getargs(&ep, 3, ss);

	if(argc != 3) error("Incorrect number of arguments");
	sret = GetTempStringSpace();									// this will last for the life of the command
	Mstrcpy(sret, getstring(argv[0]));
	i = getinteger(argv[2]);
	if(i < 0 || i > MAXSTRLEN) error("Number out of bounds");
	*sret = i;														// truncate
}



void fun_len(void) {
	fret = (float)*getstring(ep);									// first byte is the length
}




void fun_log(void) {
	fret = logf(getnumber(ep));
}


// syntax:  S$ = MID$(s, spos [, nbr])
void fun_mid(void) {
	char *s, *p1, *p2;
	int spos, nbr, i;
	char ss[2] = {',', 0  };
	getargs(&ep, 5, ss);							// get the args

	if(argc == 5) {								// we have MID$(s, n, m)
		nbr = getinteger(argv[4]);					// nbr of chars to return
	}
	else if(argc == 3) {							// we have MID$(s, n)
		nbr = MAXSTRLEN;						// default to all chars
	}
	else
		error("Incorrect number of arguments");

	sret = GetTempStringSpace();									// this will last for the life of the command
	s = getstring(argv[0]);											// the string
	spos = getinteger(argv[2]);										// the mid position
	if(nbr < 0 || nbr > MAXSTRLEN || spos < 1 || spos > MAXSTRLEN) error("Number out of bounds");

	if(spos > *s || nbr == 0)										// if the numeric args are not in the string
		return;														// return a null string
	else {
		i = *s - spos + 1;											// find how many chars remaining in the string
		if(i > nbr) i = nbr;										// reduce it if we don't need that many
		p1 = sret; p2 = s + spos;
		*p1++ = i;													// set the length of the MMBasic string
		while(i--) *p1++ = *p2++;									// copy the nbr chars required
	}
}




void fun_oct(void) {
	int i;

	sret = GetTempStringSpace();								// this will last for the life of the command
	i = getinteger(ep);
	sprintf(sret, "%o", i);
	CtoM(sret);
}



void fun_right(void) {
	int nbr;
	//char ss[2] = {',', 0  };
	char *s, *p1, *p2;
	getargs(&ep, 3, ",");

	if(argc != 3) error("Incorrect number of arguments");
	s = getstring(argv[0]);
	nbr = getinteger(argv[2]);
	if(nbr < 0 || nbr > MAXSTRLEN) error("Number out of bounds");
	if(nbr > *s) nbr = *s;											// get the number of chars to copy
	sret = GetTempStringSpace();									// this will last for the life of the command
	p1 = sret; p2 = s + (*s - nbr) + 1;
	*p1++ = nbr;													// inset the length of the returned string
	while(nbr--) *p1++ = *p2++;										// and copy the characters
}


void fun_rnd(void) {
	fret = (float)rand()/(float)RAND_MAX;
}


void fun_sgn(void) {
	float f;
	f = getnumber(ep);
	if(f > 0)
		fret = +1;
	else if(f < 0)
		fret = -1;
	else
		fret = 0;
}



void fun_sin(void) {
	fret = sinf(getnumber(ep));
}



void fun_sqr(void) {
	float f;
	f = getnumber(ep);
	if(f < 0) error("Negative argument to SQR()");
	fret = sqrtf(f);
}



void fun_tan(void) {
	fret = tanf(getnumber(ep));
}



void fun_val(void) {
	char *p;
	p = getCstring(ep);
	if(*p == '&') {
		switch(*++p) {
			case 'h':
			case 'H': fret = (float)strtol(++p, NULL, 16); break;
			case 'o':
			case 'O': fret = (float)strtol(++p, NULL, 8); break;
			case 'b':
			case 'B': fret = (float)strtol(++p, NULL, 2); break;
			default : fret = 0;
		}
	} else
		fret = atof(p);
}


void fun_space(void) {
	int i;

	sret = GetTempStringSpace();									// this will last for the life of the command
	i = getinteger(ep);
	if(i < 0 || i > MAXSTRLEN) error("Number out of bounds");
	memset(sret + 1, ' ', i);
	*sret = i;
}


void fun_str(void) {
	float f;

	sret = GetTempStringSpace();									// this will last for the life of the command
	f = getnumber(ep);
	sprintf(sret, "%G", f);
	CtoM(sret);
}



void fun_string(void) {
	int i, j;
	char *p;
	getargs(&ep, 3, ",");
	if(argc != 3) error("Invalid syntax");

	sret = GetTempStringSpace();									// this will last for the life of the command
	i = getinteger(ep);
	if(i < 0 || i > MAXSTRLEN) error("Number out of bounds");
	if(isdigit(*argv[2]))
		j = getinteger(argv[2]);
	else {
		p = getstring(argv[2]);
		if(*p == 0) error("Zero length string as an argument");
		j = p[1];
	}
	if(j < 0 || j > 255) error("Number out of bounds");
	memset(sret + 1, j, i);
	*sret = i;
}



void fun_format(void) {
	char *p, *fmt;
	int inspec;

	getargs(&ep, 3, ",");
	if(argc%2 == 0) error("Invalid syntax");
	sret = GetTempStringSpace();									// this will last for the life of the command
	if(argc == 3)
		fmt = getCstring(argv[2]);
	else
		fmt = "%g";

	// check the format string for errors that might crash the CPU
	for(inspec = 0, p = fmt; *p; p++) {
		if(*p == '%') {
			inspec++;
			if(inspec > 1) error("Only one format specifier (%) allowed");
			continue;
		}

		if(inspec == 1 && (*p == 'g' || *p == 'G' || *p == 'f' || *p == 'e' || *p == 'E'))
			inspec++;

		if(inspec == 1 && !(isdigit(*p) || *p == '+' || *p == '-' || *p == '.' || *p == ' '))
			error("Illegal character in format specification");
	}
	if(inspec != 2) error("Format specification not found");
	sprintf(sret, fmt, getnumber(argv[0]));
	CtoM(sret);
}




void fun_ucase(void) {
	char *s, *p;
	int i;

	s = getstring(ep);
	p = sret = GetTempStringSpace();								// this will last for the life of the command
	i = *p++ = *s++;												// get the length of the string and save in the destination
	while(i--) {
		*p = toupper(*s);
		p++; s++;
	}
}


void fun_lcase(void) {
	char *s, *p;
	int i;

	s = getstring(ep);
	p = sret = GetTempStringSpace();								// this will last for the life of the command
	i = *p++ = *s++;												// get the length of the string and save in the destination
	while(i--) {
		*p = tolower(*s);
		p++; s++;
	}
}



void fun_peek(void) {
	getargs(&ep, 3, ",");
	if(argc != 3) error("Invalid syntax");
	fret = *(char *)((getinteger(argv[0]) << 16) | getinteger(argv[2]));
}


// function (which looks like a pre defined variable) to return the version number
// it pulls apart the VERSION string to create a numeric number
void fun_version(void){
	char *s;
	s = VERSION;
	fret = (float)(s[0] - '0') + ((float)atoi(&s[2])/100);
	if(isalpha(s[strlen(s) - 1])) fret += (float)(toupper(s[strlen(s) - 1]) - 'A' + 1) / 10000.0;
}
