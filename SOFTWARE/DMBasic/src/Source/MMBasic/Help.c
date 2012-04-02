/*





 */

#include <p32xxxx.h>								// device specific defines
#include <plib.h>									// peripheral libraries
#include <stdlib.h>									// standard library functions
#include <string.h>									// string functions
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
#include "Setup.h"

        extern int VRes;
        int FileNumber =0;
        unsigned char LineCount=1;
	char *p;
        unsigned char t;
        unsigned char CurrentDir[64];
        unsigned char HelpCommand[64];
void cmd_help(void) {
    if(S.SDEnable){
        if(!InitSDCard()) return;
        DefaultDrive = SDFS;
	FSgetcwd(CurrentDir, 64);
        strcpy(HelpCommand,"\\HELP");
        FSchdir(HelpCommand);
        LineCount=1;
        FileNumber = FindFreeFileNbr();
        if(FileNumber!=0){
        if(*cmdline)
        {
        strcpy(HelpCommand, getCstring(cmdline));
        if (strchr(HelpCommand, '.') == NULL) strcat(HelpCommand, ".TXT");
        }
        else
        {
        strcpy(HelpCommand,"HELP.TXT");
        }
      if (SDFileExists(HelpCommand)) {
        MMfopen(HelpCommand, "r", FileNumber);
        MMcls();
    while (!MMfeof(FileNumber))  {
        t = MMfgetc(FileNumber);
            MMputchar(t);
            if(t==0x0d) LineCount++;
            if((LineCount+1)==(VRes/12))
            {
                MMPrintString("\n\r");
                MMPrintString("PRESS ANY KEY ...");
		MMgetchar();
		MMPrintString("\r                 \r");
                MMPosY -=12;
                LineCount=1;
            }
            }
            MMfclose(FileNumber);
        }
        else 
        {
            MMPrintString("Help Not Found");
        }
        FSchdir(CurrentDir);

        }
        else
        {
            MMPrintString("Can Not Get FileNumber\n\r");
        }
    }
    else
    {
        MMPrintString("Must have SD Enabled\n\r");
    }
    }

