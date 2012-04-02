/***********************************************************************************************************************
MMBasic

file.h

Include file that contains the functions for handling file I/O in MMBasic.

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/


/**********************************************************************************
 the C language function associated with commands, functions or operators should be
 declared here
**********************************************************************************/
#ifdef INCLUDE_FUNCTION_DEFINES
// format:
//      void cmd_???(void)
//      void fun_???(void)
//      void op_???(void)

void cmd_open(void);
void cmd_close(void);
void cmd_files(void);
void cmd_mkdir(void);
void cmd_rmdir(void);
void cmd_chdir(void);
void cmd_kill(void);
void cmd_name(void);
void cmd_drive(void);

void cmd_msdon(void);
void cmd_msdoff(void);
void cmd_sdformat(void);


void fun_cwd(void);
void fun_errno(void);
void fun_eof(void);
void fun_loc(void);
void fun_lof(void);
void fun_inputstr(void);
void fun_mmdrive(void);
void fun_mmfname(void);

#endif



/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)


	{ "OPEN",		T_CMD,				0, cmd_open		},
	{ "CLOSE",		T_CMD,				0, cmd_close	},
	{ "FILES",		T_CMD,				0, cmd_files	},
	{ "MKDIR",		T_CMD,				0, cmd_mkdir	},
	{ "RMDIR",		T_CMD,				0, cmd_rmdir	},
	{ "CHDIR",		T_CMD,				0, cmd_chdir	},
	{ "KILL",		T_CMD,				0, cmd_kill		},
	{ "NAME",		T_CMD,				0, cmd_name		},
	{ "DRIVE",		T_CMD,				0, cmd_drive	},
	{ "MSDON",		T_CMD,				0, cmd_msdon	},
	{ "MSDOFF",		T_CMD,				0, cmd_msdoff	},
        { "SDFORMAT",		T_CMD,				0, cmd_sdformat	},
#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
	{ "EOF(",		T_FUN | T_NBR,		0, fun_eof		},
	{ "LOC(",		T_FUN | T_NBR,		0, fun_loc		},
	{ "LOF(",		T_FUN | T_NBR,		0, fun_lof		},
	{ "CWD$",		T_FNA | T_STR,		0, fun_cwd		},
	{ "AS",			T_NA,				0, op_invalid	},
	{ "MM.ERRNO",	T_FNA | T_NBR,		0, fun_errno	},
	{ "INPUT$(",	T_FUN | T_STR,		0, fun_inputstr	},
	{ "MM.DRIVE$",	T_FNA | T_STR,		0, fun_mmdrive	},
	{ "MM.DRIVE",	T_FNA | T_STR,		0, fun_mmdrive	},					// for users who forget that it is a string
	{ "MM.FNAME$",	T_FNA | T_STR,		0, fun_mmfname	},


#endif



#ifdef INCLUDE_FUNCTION_DEFINES
// General definitions used by other modules

#ifndef FILES_HEADER
#define FILES_HEADER

#define COM1_FILE_POINTER			(FSFILE *)1								// the file "pointer" to use when COM1 is open
#define COM2_FILE_POINTER			(FSFILE *)2								// the file "pointer" to use when COM2 is open
#ifdef OLIMEX
#define COM3_FILE_POINTER			(FSFILE *)3								// the file "pointer" to use when COM1 is open
#define COM4_FILE_POINTER			(FSFILE *)4								// the file "pointer" to use when COM2 is open
#endif
extern void MMgetline(int filenbr, char *p);
extern void MMfputs(char *p, int filenbr);
extern void CheckAbort(void) ;
extern void MMfopen(char *fname, char *mode, int fnbr);
extern void MMfclose(int fnbr);
extern unsigned char MMfgetc(int fnbr);
extern char MMfputc(char c, int fnbr);
extern int InitSDCard(void) ;
extern int SDFileExists(char *p);
extern void CloseAllFiles(void);
extern int MMfeof(int fnbr);
extern int FindFreeFileNbr(void);
extern char *MMgetcwd(void);
extern void MMfread(void *ptr, size_t size, size_t n, int fnbr);
extern int GetDrive(char *p);
extern int FlashFileKill(char *fn);
extern void FlashList(char *s);

#define SDFS		8
#define FLASHFS		9
extern int DefaultDrive;

extern char MSDEnable;
//#ifndef MAXIMITE
//	int MMCharPos;
//#endif
//

///////////////////////////// defines for flash storage ///////////////////////////////////////
#define FLASH_PAGE_SIZE		4096						// size of each page of flash storage
#define PROGMEM_NBR_PAGES	32							// size of flash storage used for files in pages

#define FILENAME_LENGTH		15							// max length of a file name
#if ((((FILENAME_LENGTH + 1) >> 2) << 2) != FILENAME_LENGTH + 1)
	#error Filename length + 1 should be an even number of words
#endif

#define PROG_DATA_LENGTH			(FLASH_PAGE_SIZE - FILENAME_LENGTH - sizeof(unsigned char) - sizeof(unsigned int))



// structure of a flash filesystem page (or block)
struct structBlock {
	unsigned char bnbr;
	unsigned char fname[FILENAME_LENGTH];
	unsigned int gnbr;
	unsigned char dat[PROG_DATA_LENGTH];
};

volatile struct structBlock *FindFlashBlock(char* fn, int block) ;


#endif
#endif


