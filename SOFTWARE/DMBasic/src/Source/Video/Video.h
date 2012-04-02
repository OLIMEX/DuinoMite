/************************************************************************************************************************
Maximite

Video.h

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/


#define VBUFSIZE     432    // video buffer vert size
#define HBUFSIZE     480    // video buffer horiz size
	
// Global variables provided by Video.c and DrawChar.c
extern int VRes, HRes;
extern int MMPosX, MMPosY;
extern int MMCharPos;
extern int ListCnt;						// line count used by the LIST and FILES commands
extern int ScrollTop ;
extern int ScrollBottom ;
extern int VA[];						// video buffer
extern int vga;							// true if we are using vga
extern int fontWidth, fontHeight, fontScale;

#define NBRFONTS	10

struct s_font {
	void *p;
	unsigned char width, height;
	unsigned char start, end;
        unsigned char binary;                                    //if its a binary font
};

extern struct s_font ftbl[NBRFONTS];	

// Facilities provided by Video.c and DrawChar.c
extern char VT100Filter(char Data);

extern void MMCursor(int b);
extern void MMcls(void);
extern void initVideo(void);
extern void haltVideo(void);
extern void plot(int x, int y, int b); 
extern int pixel(int x, int y);
extern void MMline(int x1, int y1, int x2, int y2, int colour) ;
extern void MMCircle(int x, int y, int radius, int fill, int colour) ;
extern void MMbox(int x1, int y1, int x2, int y2, int fill, int colour);

extern void initDrawChar(void);
extern void SetFont(int font, int scale, int reverse);
extern void initFont(void);
extern void UnloadFont(int font);

// cursor definition
void ShowCursor(int show);
typedef enum {C_OFF = 0, C_STANDARD, C_INSERT } Cursor_e ;
extern Cursor_e Cursor;


