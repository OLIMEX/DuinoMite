/*
 Serial Term With VT100 Support
(C) 2011 Ken Segler


 * page down 0x89
 * page up 0x88







 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../MMBasic/MMBasic.h"
#include "../MMBasic/Files.h"
#include "../Keyboard/Keyboard.h"
#include "../Video/Video.h"
//#include "../Video/VT100.h"
#include "../Serial/serial.h"
extern int fontReverse;
extern int scrollStart;
extern int scrollEnd;
extern unsigned char VT100ClearStart ;
extern unsigned char VT100ClearEnd ;
const long TBaudRateList[] = {
    1200,
    2400,
    9600,
    19200,
    38400,
    57600,
    115200
};
	char *fname;
int c;
char BaudRate = 6;
char ComPort = 1;
char PortOpen = 0;
char Capture = 0;
char HexMode = false;
char Temp[80];
void UpdateMenu(void)
{
    int TempY,TempX;
    TempY=MMPosY;
    TempX=MMPosX;
    MMPosY=(VRes-24);
    MMPosX =0;
    fontReverse=1;
    sprintf(Temp,"  %s  [F2] Port - COM%d  [F3] Bits - 8  [F4] Handshake - Off     ",fname,ComPort);
    //MMPrintString("  [F1] Baud - 115200  [F2] Port - COM4  [F3] Bits - 8  [F4] Handshake - Off     ");
    MMPrintString(Temp);
//    MMPrintString("  [F5] Capture - Off  [F6] Port Open    [F8] Exit                               ");
    sprintf(Temp,"  [PageUp] - Upload  [PageDown] - Download                                      ");
    MMPrintString(Temp);
    fontReverse=0;
    MMPosY=TempY;
    MMPosX=TempX;
}
void ScrollWindow(void)
{
    int *pd = VA + ((HBUFSIZE / 32) * (VT100ClearStart*fontHeight));
    int *ps = pd + (((HBUFSIZE / 32) * (VT100ClearStart*fontHeight)*2));
    int i;
    for (i = 0; i < ((VT100ClearEnd-3)*(HBUFSIZE/32)*fontHeight); i++) *pd++ = *ps++; // scroll up
    for (i = 0; i < ((HBUFSIZE / 32) * fontHeight); i++) *pd++ = 0; // clear the new line
}
void cmd_term(void)
{

	getargs(&cmdline, 5, ",");									// getargs macro must be the first executable stmt in a block
	fname = getCstring(argv[0]);
	makeupper(fname);
        SerialOpen(fname, 0);
        ComPort=(fname[3]-0x30);

        PortOpen=true;
    DisableMMAbort=true;
    MMcls();
    fontReverse=1;
    //            |         1         2         3         4         5         6         7         8
    //            |12345678901234567890123456789012345678901234567890123456789012345678901234567890
    MMPrintString("                           VT100 Term  By Ken Segler                            ");
    fontReverse=0;
    UpdateMenu();
    VT100ClearStart=1;
    VT100ClearEnd=34;
    ScrollTop=1;
    ScrollBottom=34;
    MMPosY=12;
    MMPosX=0;
    c=0;
    Cursor = C_STANDARD;
    while(c!=F8)
    {
    ShowCursor(1);
    c = MMInkey();
    switch(c)
    {
        case F1:
            if(++BaudRate >6) BaudRate=0;
            UpdateMenu();
            break;
        case F2:
            if(++ComPort >4) ComPort=1;
            UpdateMenu();
            break;
        case F3:
            HexMode = ~HexMode;
            break;
        case F4:
            break;
        case F5:
            break;
        case F6:
        sprintf(inpbuf,"COM%d:%d",ComPort,TBaudRateList[BaudRate]);
        SerialOpen(inpbuf, 0);
        PortOpen=true;
            break;
        case F7:
            break;
        case F8:
            SerialClose(ComPort);
            break;
        case F9:
            break;
        case F10:
            break;
        case F11:
            break;
        case F12:
            break;
        case PUP:
            break;
        case PDOWN:
            break;
        default:
 
            if(PortOpen){
            if(c != -1) {
            SerialPutchar(ComPort, c);
            if(HexMode){
            sprintf(Temp,"[%02X]",c);
            MMPrintString(Temp);
            }
            }
            if(SerialRxStatus(ComPort))  {
            c=SerialGetchar(ComPort);
            ShowCursor(0);
            MMputchar(c);
            if(HexMode){
            sprintf(Temp,"{%02X}",c);
            MMPrintString(Temp);
            }
            }
            break;
            }
    }

    }
    MMcls();
    DisableMMAbort=false;
}

