/***********************************************************************************************************************
 Duinomite

 CAN.h

 Include file that contains the globals and defines for can.c
  
 Copyright 2012 Frank Voorburg - http://www.feaser.com
 This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.  You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.

***********************************************************************************************************************/


/***********************************************************************************************************************
 the C language function associated with commands, functions or operators should be
 declared here
***********************************************************************************************************************/
#ifdef INCLUDE_FUNCTION_DEFINES
// format:
//      void cmd_???(void)
//      void fun_???(void)
//      void op_???(void)
void cmd_canOpen(void);
void cmd_canClose(void);
void cmd_canTransmit(void);
void cmd_canReceive(void);
#endif


/***********************************************************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
***********************************************************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)
    { "CANOPEN",    T_CMD,  0,  cmd_canOpen     },
    { "CANCLOSE",   T_CMD,  0,  cmd_canClose    },
    { "CANSEND",    T_CMD,  0,  cmd_canTransmit },
    { "CANRCV",     T_CMD,  0,  cmd_canReceive  },
#endif


#ifdef INCLUDE_FUNCTION_DEFINES
#ifndef CAN_H
#define CAN_H
/***********************************************************************************************************************
 Function prototypes
***********************************************************************************************************************/
void CanInit(void);


#endif /* CAN_H */
#endif /* INCLUDE_FUNCTION_DEFINES */
/************************************************* end of CAN.h *******************************************************/
