/***********************************************************************************************************************
MMBasic

operators.c

Provides the operator functions used in MMBasic  Ie, +, -, *, etc.
  
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

#include "MMBasic.h"




/********************************************************************************************************************************************
 basic operators
 each function is responsible for decoding a basic operator
 all function names are in the form op_xxxx() so, if you want to search for the function responsible for the AND operator look for op_and
 
 There are 5 globals used by these finctions:
 
 farg1, farg2   These are the floating point arguments to the operator.  farg1 is the left argument
 
 sarg1, sarg2   These are the string pointers to the arguments for a the string operator.  sarg1 is the left argument
                
 fret           Is the return value for a basic operator that returns a float value
                 value
 sret           Is the return value for a basic operator that returns a string
                
 targ           Is the type of the arguments.  normally this is set by the caller and is not changed by the function

 ********************************************************************************************************************************************/




void op_invalid(void) {
	error("Syntax error");
}


void op_exp(void) {
	fret = pow(farg1, farg2);
}


void op_mul(void) {
	fret = farg1 * farg2;
}


void op_div(void) {
	if(farg2 == 0) error("Divide by zero");
	fret = farg1 / farg2;
}


void op_divint(void) {
	int a, b;
	a = MMround(farg1);
	b = MMround(farg2);
	if(b == 0) error("Divide by zero");
	fret = (float)(a / b);
}


void op_add(void) {
	if(targ & T_NBR)
		fret = farg1 + farg2;
	else {
		if(*sarg1 + *sarg2 > MAXSTRLEN) error("Concatenated string is too long");
		sret = GetTempStringSpace();								// this will last for the life of the command
		Mstrcpy(sret, sarg1);
		Mstrcat(sret, sarg2);
	}
}



void op_subtract(void) {
	fret = farg1 - farg2;
}


void op_mod(void) {
	int a, b;
	a = MMround(farg1);
	b = MMround(farg2);
	if(b == 0) error("Divide by zero");
	fret = a % b;
}


void op_ne(void) {
	if(targ & T_NBR)
		fret = (farg1 != farg2) ? 1 : 0;
	else 
		fret = (memcmp(sarg1, sarg2, *sarg1 + 1) != 0) ? 1 : 0;
	targ = T_NBR;									// always return a number, even if the args are string
}



void op_gte(void) {
	if(targ & T_NBR)
		fret = (farg1 >= farg2) ? 1 : 0;
	else
		fret = (memcmp(sarg1, sarg2, *sarg1 + 1) >= 0) ? 1 : 0;
	targ = T_NBR;									// always return a number, even if the args are string
}


void op_lte(void) {
	if(targ & T_NBR)
		fret = (farg1 <= farg2) ? 1 : 0;
	else
		fret = (memcmp(sarg1, sarg2, *sarg1 + 1) <= 0) ? 1 : 0;
	targ = T_NBR;									// always return a number, even if the args are string
}


void op_lt(void) {
	if(targ & T_NBR)
		fret = (farg1 < farg2) ? 1 : 0;
	else
		fret = (memcmp(sarg1, sarg2, *sarg1 + 1) < 0) ? 1 : 0;
	targ = T_NBR;									// always return a number, even if the args are string
}


void op_gt(void) {
	if(targ & T_NBR)
		fret = (farg1 > farg2) ? 1 : 0;
	else
		fret = (memcmp(sarg1, sarg2, *sarg1 + 1) > 0) ? 1 : 0;
	targ = T_NBR;									// always return a number, even if the args are string
}


void op_equal(void) {
	if(targ & T_NBR)
		fret = (farg1 == farg2) ? 1 : 0;
	else
		fret = (memcmp(sarg1, sarg2, *sarg1 + 1) == 0) ? 1 : 0;
	targ = T_NBR;									// always return a number, even if the args are string
}


void op_and(void) {
	int a, b;
	a = MMround(farg1);
	b = MMround(farg2);
	fret = (a & b);
}


void op_or(void) {
	int a, b;
	a = MMround(farg1);
	b = MMround(farg2);
	fret = (a | b);
}


void op_xor(void) {
	int a, b;
	a = MMround(farg1);
	b = MMround(farg2);
	fret = (a ^ b);
}


void op_not(void){
	// don't do anything, just a place holder
	error("Syntax error");
}

