/***********************************************************************************************************************
MMBasic

IOPorts.h

Include file that defines the IOPins for the PIC32 chip in MMBasic.
This file needs to be completed and MMBasic rewritten to use the defines in this file (a ToDo job).
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/


// General defines
#define P_INPUT				1						// for setting the TRIS on I/O bits
#define P_OUTPUT			0
#define P_ON				1
#define P_OFF				0


#ifdef MAXIMITE
	#include "IOPorts - Maximite.h"
#endif

#ifdef UBW32
	#include "IOPorts - UBW32.h"
#endif

#ifdef OLIMEX
        #include "IOPorts - Olimex.h"
#endif
