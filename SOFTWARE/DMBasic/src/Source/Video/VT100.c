/* VT100 Filter 
 
Esc[ValueA 	Move cursor up n lines 	CUU
Esc[ValueB 	Move cursor down n lines 	CUD
Esc[ValueC 	Move cursor right n lines 	CUF
Esc[ValueD 	Move cursor left n lines 	CUB
Esc[H           Move cursor to upper left corner 	cursorhome
Esc[;H          Move cursor to upper left corner 	cursorhome
Esc[Line;ColumnH 	Move cursor to screen location v,h 	CUP
Esc[f           Move cursor to upper left corner 	hvhome
Esc[;f          Move cursor to upper left corner 	hvhome
Esc[Line;Columnf 	Move cursor to screen location v,h 	CUP
EscD            Move/scroll window up one line 	IND
EscM            Move/scroll window down one line 	RI
EscE            Move to next line 	NEL
Esc7            Save cursor position and attributes 	DECSC
Esc8            Restore cursor position and attributes 	DECSC  
 
Esc[K 	Clear line from cursor right 	EL0
Esc[0K 	Clear line from cursor right 	EL0
Esc[1K 	Clear line from cursor left 	EL1
Esc[2K 	Clear entire line 	EL2

Esc[J 	Clear screen from cursor down 	ED0
Esc[0J 	Clear screen from cursor down 	ED0
Esc[1J 	Clear screen from cursor up 	ED1
Esc[2J 	Clear entire screen 	ED2 
 
 
 */
#define INCLUDE_FUNCTION_DEFINES
#include <string.h>
#include <stdlib.h>
#include "GenericTypeDefs.h"
#include "Video.h"


#define VT100HaveESC 1
#define VT100HaveBracket 2
#define VT100HaveQuestionmark 3
#define VT100HaveNumber 4
#define VT100HaveLetter 5
//#define fontWidth 6
//#define fontHeight 12
unsigned char VT100ClearStart = 0;
unsigned char VT100ClearEnd = 0;

unsigned char VT100State = 0;
unsigned char VT100Buff[4];
unsigned char VT100BuffIndex = 0;
unsigned char VT100Line = 0;
unsigned char VT100Column = 0;
BOOL VT100HaveSemiColon = 0;
BOOL VT100ScrollArea = 0;
extern int fontReverse;

void VT100cls(unsigned char Start,unsigned char End) {
    unsigned int Count;
    MMPosX = 0;
    MMPosY = (Start*fontHeight);
    MMCharPos = 0;
    for(Count=(Start*(HBUFSIZE/32)*fontHeight);Count < (End*(HBUFSIZE/32)*fontHeight);Count++) VA[Count]=0;

    //    memset(VA, 0,8192);// VBUFSIZE*(HBUFSIZE/8));
}

char VT100Filter(char Data)
{
    switch(VT100State)
    {
        case 0:
            if( Data == 0x1b ) {
            VT100State = VT100HaveESC;
            memset(VT100Buff,0,4);
            VT100BuffIndex=0;
            Data=0;
            }
            break;
        case 1:
            //scroll down
            if( Data == 'M' )
            {
                Data=0;
                VT100State=0;
            }
            // scroll up
            if( Data == 'D' )
            {
                Data=0;
                VT100State=0;
            }
            
            if( Data == '[' ) {
                VT100State = VT100HaveBracket;
                Data=0;
            }
            else
            {
                VT100State = 0;
            }
            break;
        case 2:
            if( Data == '?' )
            {
                VT100State = VT100HaveQuestionmark;
                Data = 0;
            }

            if( ( Data >= '0') && ( Data <= '9') ){
                VT100Buff[VT100BuffIndex++] = Data;
                Data=0;
            }
            if ( ( Data =='J' ) && (VT100Buff[0] == '2') ) {
                    //clear screen home cursor
                    //start over
                VT100State = 0;
                VT100BuffIndex = 0;
                VT100cls(VT100ClearStart,VT100ClearEnd);
                //                MMcls();
                Data=0;
                }
/*
Esc[J 	Clear screen from cursor down 	ED0
Esc[0J 	Clear screen from cursor down 	ED0
Esc[1J 	Clear screen from cursor up 	ED1
Esc[2J 	Clear entire screen 	ED2
 */
            if( Data =='J') // delete from cursor
            {
                VT100State=0;
                VT100BuffIndex=0;
                Data=0;
            }
/*
Esc[K 	Clear line from cursor right 	EL0
Esc[0K 	Clear line from cursor right 	EL0
Esc[1K 	Clear line from cursor left 	EL1
Esc[2K 	Clear entire line               EL2
 */
            if( Data =='K') // delete from cursor
            {
                VT100State=0;
                VT100BuffIndex=0;
                Data=0;
            }
            if( Data == 'm' )
            {
                if(VT100Buff[0]=='7') fontReverse =1;
                if(VT100Buff[0]=='1') fontReverse =0;
                if(VT100Buff[0]=='0') fontReverse =0;
                VT100State=0;
                VT100BuffIndex=0;
                Data=0;
            }
            if ( Data == ';') {
                VT100HaveSemiColon = 1;
                VT100Line=atoi(VT100Buff);
                VT100BuffIndex=0;
                memset(VT100Buff,0,4);
                Data=0;
                //convert number vt100 line
            }
            // set scroll area
            if ( Data == 'r')
            {
                VT100State=0;
                Data=0;
            }
            if ( (Data == 'H') && ( VT100Line == 0 )) {
                //Home cursor
                MMPosX=0;
                MMPosY=(VT100ClearStart * fontHeight);
                VT100State=0;
                Data=0;
            }
            if ( Data == 'H' )
            {
                VT100HaveSemiColon = 1;
                VT100Column=atoi(VT100Buff);
                VT100BuffIndex=0;
                MMPosX=((VT100Column -1) * fontWidth);
                MMPosY=(((VT100Line+VT100ClearStart) -1) * fontHeight);
                VT100State=0;
                // convert number to vt100 column
                // move cursor to VT100Line and VT100 Column
                Data=0;
            }
            if (Data == 'A')
            {
             MMPosY -= (VT100Line * fontHeight);
             VT100State=0;
             Data=0;
             //cursor up # of lines
            }
            if (Data == 'B')
            {
             MMPosY += (VT100Line * fontHeight);
             VT100State=0;
             Data=0;
                //cursor down # of lines
            }
            if (Data == 'C')
            {
             MMPosX += (VT100Column * fontWidth);
             VT100State=0;
             Data=0;
                //cursor right # of columns
            }
            if (Data == 'D')
            {
             MMPosX -= (VT100Column * fontWidth);
             VT100State=0;
             Data=0;
                //cursor left # of columns
            }
            break;
        case    3:
            if( Data == '6' )
            {
                VT100ScrollArea=1;
                Data=0;
            }
            // Scroll Area Off
            if( Data =='l' && VT100ScrollArea)
            {
                VT100ScrollArea=0;
                VT100State = 0;
                Data=0;
            }
            // Scroll Area On
            if( Data =='h' && VT100ScrollArea)
            {
                VT100ScrollArea=0;
                VT100State = 0;
                Data=0;
            }
            break;
    }
    return Data;
}
