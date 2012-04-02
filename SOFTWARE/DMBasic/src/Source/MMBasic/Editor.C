/* A Screen Editor Subsystem. */
/* Based On Herbert Schildt Editor From The Craft Of C Book */
/* Modded For Maximite By kenseglerdesigns.com */
//



#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>

#define INCLUDE_FUNCTION_DEFINES
#include "MMBasic.h"
#include "Files.h"
#include "FSE.h"
#include "../Keyboard/Keyboard.h"
#include "../Video/Video.h"
#include "../Video/VT100.h"
#include "../Serial/serial.h"
#include "External.h"
#include "Commands.h"


#define BUF_SIZE 32768
#define KILL_BUF_SIZE 4*79 //LINE_LEN
#define FileNumber 1
#define TabSpaces 4
extern char LastFile[15]; // used to keep track of the last file RUN, LOADed or Saved
extern int fontReverse;
//char buf[BUF_SIZE];

char *curloc, *endloc;
int scrnx, scrny;
char killbuf[KILL_BUF_SIZE];
char T;
char *buf;
char *buf2;
char LINE_LEN,MAX_LINES,STATUS_LINE;
char *helpline[80];

char *HelpScreen = "                      FSE V1.0 Commands (c)2011 Ken Segler";


void edit(char *fname), help(void);
void gotoxy(int x, int y), clrline(int y);
void edit_clr_eol(int x, int y), clrscr(void);
void printline(char *p), delete_char(void);
void search(void), kill_line(void);
void upline(void);
void downline(void), left(void), right(void);
void scrolldn(int x, int y);
void scrollup(int topx, int topy, int endx, int endy);
void display_scrn(int x, int y, char *p);
void pagedown(void), pageup(void), replace(void);
void home(void), gotoend(void), paste(void);
int load(char *fname), save(char *fname);
void edit_gets(char *str), info(void);



char putch(char c) {
    // put a character out to the video screen, the serial console and the USB interface
    MMputchar(c);
//    if (!FileXfr) VideoPutc(c); // draw the char on the video screen
//    if (SerialConsole) SerialPutchar(SerialConsole, c); // send it to the serial console if enabled
//    if (!(SerialConsole && FileXfr)) USBPutchar(c); // send it to the USB
    return c;

}

void PrintString(char* s) {
    while (*s) putch(*s++);
}
void show_help(void)
{
    int i=0;
    int Tscrnx, Tscrny;
    Tscrnx=scrnx;
   Tscrny=scrny;
    scrnx=0;
    scrny=0;
    clrscr();
    gotoxy(0,0);
    while(help_fse[i++])
    {
        putch(help_fse[i]);
    }
    MMgetchar();
    clrscr();
//    scrnx=Tscrnx;
//    scrny=Tscrny;
    curloc=buf;
    display_scrn(0, 0, curloc);
}
char getch(void) {
    char c;
    c = MMgetchar();
    if (c == '\n') c = '\r';
    return c;
}

/*  This is the editor's entry function and contains
    its main loop.

    Call it with the name of the file you want to edit.
 */
void cmd_testcalloc(void) {
    char fname[80];
    buf = calloc(16384, 1);
    if (buf == NULL) PrintString("Cant calloc Memory \n\r");
    buf2 = calloc(16384, 1);
    if (buf2 == NULL) PrintString("Cant calloc Memory 2\n\r");
    sprintf(fname, " Buff : %p Buff2 : %p ", buf, buf2);
    PrintString(fname);
    free(buf);
    free(buf2);
}

void ReverseVideo(char r) {
    if (r)
        PrintString("\033[7m");
    else
        PrintString("\033[0m");

}

void cmd_fse(void) //StartEditor(char *fname)
{
    char name[20];
    char c;
    LINE_LEN = ((HRes/6)-1);
    MAX_LINES = ((VRes/12)-1); //35
    STATUS_LINE =(MAX_LINES+1); //36

    ClearExternalIO();
    clearstack();
    clearvars();
    TraceOn = false;    // turn off tron on new
    clearprog();
    
    memset(name, 0, 20);
    Cursor = C_STANDARD;
    if (*cmdline) {
        strcpy(name, getCstring(cmdline));
        if (strchr(name, '.') == NULL) strcat(name, ".BAS");
    }
    buf = calloc(16384, 1);
    if (buf == NULL) PrintString("Cant calloc Memory \n\r");
    buf2 = calloc(16384, 1);
    if (buf2 == NULL) PrintString("Cant calloc Memory 2\n\r");

    /* try to load the file */
    if (SDFileExists(name)) {
        load(name);
    } else {
        curloc = endloc = buf;
    }

    //  strcpy(name, fname);
    clrscr();
    /* set initial values to X,Y coordinate vars */
    scrnx = scrny = 0;
    /* display a screen full of text */
    display_scrn(0, 0, curloc);
    help(); /* print the help line */
    gotoxy(0, 0);
    /* editor main loop */
    do {
        c = getch();
        if (c >= 0x7f) { /* is a command */
            switch (c) {
                case 0x91: /* F1: save file */
                    save(name);
                    break;
                case 0x92: /* F2: load file */
                    /*
                                fontReverse=1;
                              clrline(STATUS_LINE+1);
                              gotoxy(0, STATUS_LINE+1);
                              PrintString("enter filename: ");
                              edit_gets(name);
                              fontReverse=0;
                    //          if(*name) load(name);
                    //        if(strchr(name, '.') == NULL) strcat(name, ".BAS");
                            if(SDFileExists(name) && *name )
                            {
                            memset(buf, 0, 32768);
                                load(name);
                            }
                              clrscr();
                              help();
                              display_scrn(0, 0, curloc);
                               scrnx = scrny = 0;
                     */
                    save(name);
                    free(buf);
                    free(buf2);
                    MMcls();
                    ClearExternalIO();
                    clearvars();
                    clearstack();
                    clearprog(); // clear the program space
                    mergefile(name); // load
                    strcpy(LastFile, name); // save the name for a future SAVE command
                    nextstmt = pmemory;
                    ExecuteProgram();
                    longjmp(mark, 1);
                    break;
                case 0x93: /* F3: search */
                    search();
                    break;
                case 0x94: /* F4: replace */
                    replace();
                    break;
                case 0x9a:
                    show_help();
                    break;
                case 0x86: /* home */
                    home();
                    break;
                case 0x87: /* end */
                    gotoend();
                    break;
                case 0x82: /* left */
                    left();
                    break;
                case 0x83: /* right */
                    right();
                    break;
                case 0x80: /* up  */
                    upline();
                    break;
                case 0x81: /* down */
                    downline();
                    break;
                case 0x88: /* page up */
                    pageup();
                    break;
                case 0x89: /* page down */
                    pagedown();
                    break;
                case 0x7f: /* Del */
                    if (curloc < endloc) delete_char();
                    break;
            }
            if (curloc < buf) {
                scrnx = scrny = 0;
                curloc = buf;
            }
            gotoxy(scrnx, scrny); /* position cursor */
        } else { /* enter keystroke into file */
            switch (c) {
                case '\r': /* carriage return */
                    /* see if buffer is full */
                    if (endloc == buf + BUF_SIZE - 2) break;

                    /* move contents of file below current
                       location down one byte to make room
                       for the RETURN
                     */
                    memmove(curloc + 1, curloc, endloc - curloc + 1);

                    *curloc = c; /* put RETURN in file */
                    curloc++;

                    /* clear rest of line */
                    edit_clr_eol(scrnx, scrny);
                    scrnx = 0;
                    scrny++;

                    /* move text on screen down */
                    if (scrny == MAX_LINES) { /* at bottom of page */
                        scrny = MAX_LINES - 1;
                        scrollup(0, 0, LINE_LEN, scrny);
                    } else scrolldn(scrnx, scrny);

                    gotoxy(scrnx, scrny);
                    printline(curloc); /* display the line */
                    endloc++; /* advance the end of file pointer */
                    break;
                case '\t': /* Tabs */
                    for (T = 0; T < TabSpaces; T++) {
                        memmove(curloc + 1, curloc, endloc - curloc + 1);
                        *curloc = ' '; /* put keystroke in file */
                        putch(*curloc); /* display the key on the screen */
                        scrnx++; /* advance X */
                        gotoxy(scrnx, scrny);
                        printline(curloc + 1); /* display the line */
                        curloc++; /* advance the current location */
                        endloc++; /* advance the end of file pointer */
                    }
                    break;
                case '\b': /* backspace */
                    if (curloc == buf) break;
                    left();
                    delete_char();
                    break;
                case 0x0b: /* control-K: kill line */
                    kill_line();
                    break;
                case 0x10: /* control-P: paste kill buffer */
                    paste();
                    break;
                default:
                    /* see if buffer is full */
                    if (endloc == buf + BUF_SIZE - 2) break;

                    /* can't type past end of line */
                    if (scrnx == LINE_LEN) break;

                    /* move contents of file below current
                       location down one byte to make room
                       for the character
                     */
                    memmove(curloc + 1, curloc, endloc - curloc + 1);
                    *curloc = c; /* put keystroke in file */
                    putch(*curloc); /* display the key on the screen */
                    scrnx++; /* advance X */
                    gotoxy(scrnx, scrny);
                    printline(curloc + 1); /* display the line */
                    curloc++; /* advance the current location */
                    endloc++; /* advance the end of file pointer */
            }
            gotoxy(scrnx, scrny);
        }
        help();
    } while (c != 26); /* control-Z quits the editor */
    if( buf !=NULL ) free(buf);
    if( buf2 !=NULL ) free(buf2);
    clrscr();
}

/* Display a line pointed to by p.  This function
   stops when it hits a carriage return or the
   end of the file.
 */
void printline(register char *p) {
    register int i;

    i = scrnx;
    while (*p != '\r' && *p && i < LINE_LEN) {
        putch(*p);
        p++;
        i++;
    }
}

/* Insert previously killed line. */
void paste(void) {
    char *p;

    p = killbuf;
    while (*p) {
        memmove(curloc + 1, curloc, endloc - curloc + 1);
        *curloc = *p; /* put keystroke in file */
        if (scrnx < LINE_LEN) {
            putch(*curloc); /* display the key on the screen */
            scrnx++;
        }
        curloc++;
        endloc++;
        p++;
    }
    printline(curloc);
}

/* Delete the line at the current location. */
void kill_line(void) {
    register int i;
    char *p, *killbufptr;

    if (*curloc == '\r') {
        delete_char();
        return;
    }

    edit_clr_eol(scrnx, scrny); /* clear to CR */

    /* find out how many characters are in the line */
    p = curloc;
    i = 0;
    killbufptr = killbuf;
    while (*p != '\r' && p < endloc) {
        i++;
        *killbufptr = *p; /* put in kill buffer */
        p++;
        if (killbufptr < killbuf + KILL_BUF_SIZE - 2) killbufptr++;
    }
    *killbufptr = '\0';

    /* remove the line */
    memmove(curloc, curloc + i, endloc - curloc);
    endloc -= i;
}

/* Global search and replace. */
void replace(void) {
    register int len1;
    char str1[80], str2[80];
    char *p, *p2;
    ReverseVideo(true); //    fontReverse=1;
    clrline(STATUS_LINE + 1);
    gotoxy(0, STATUS_LINE + 1);
    PrintString("enter string to replace: ");
    edit_gets(str1);

    clrline(STATUS_LINE + 1);
    gotoxy(0, STATUS_LINE + 1);
    PrintString("enter replacement: ");
    edit_gets(str2);
    ReverseVideo(false); //fontReverse=0;
    p = curloc;
    len1 = strlen(str1);

    while (*str1) {
        /* search for the string */
        while (*p && strncmp(str1, p, len1)) p++;
        if (!*p) break; /* not found */

        /* remove old string */
        memmove(p, p + len1, endloc - p);
        endloc -= len1;

        /* insert new string */
        p2 = str2;
        while (*p2) {
            memmove(p + 1, p, endloc - p + 1);
            *p = *p2;
            p++;
            endloc++;
            p2++;
        }
    }
    clrscr();

    /* find location of top of screen */
    p = curloc;
    for (len1 = scrny; len1 >= 0 && p > buf;) {
        p--;
        if (*p == '\r') len1--;
    }
    if (*p == '\r') p++;

    /* redisplay current screen */
    display_scrn(0, 0, p);
    help();
}

/* Delete character at the current location. */
void delete_char(void) {
    gotoxy(scrnx, scrny);

    if (*curloc == '\r') { /* RETURN, scroll display */
        scrollup(0, scrny + 1, LINE_LEN, MAX_LINES - 1);
        memmove(curloc, curloc + 1, endloc - curloc);
        endloc--;
        display_scrn(scrnx, scrny, curloc);
        help();
    } else {
        memmove(curloc, curloc + 1, endloc - curloc);
        endloc--;
        printline(curloc);
        PrintString(" ");
    }
}

/* So status file size memory left ect .
 */
void info(void) {
    clrscr();
    sprintf((char*)helpline," File Size -> %d",endloc);
    PrintString((char*)helpline);
}

/* Display help line.  You might want to expand on
   this idea.
 */
void help(void) {
    //*  ESC [ ? 6 l       turn off region - full screen mode

    PrintString("\033[?6l");
    gotoxy(0, STATUS_LINE-1);
    ReverseVideo(true);
    sprintf((char*)helpline, "  FSE V1.0 (C)2011 Ken Segler     F10 - Help                             %d:%d  ", scrny+1, scrnx+1);
    PrintString((char*)helpline);
    ReverseVideo(false);
    PrintString("\033[1;35r");
    PrintString("\033[?6h");
    gotoxy(scrnx, scrny);
}

/* Move current location left. */
void left(void) {
    if (curloc == buf) return; /* can't go left */
    scrnx--;
    if (scrnx < 0) { /* at beginning of a line */
        scrnx = 0;
        upline(); /* go up to next line */

        /* find end of line */
        while (*curloc != '\r') {
            curloc++;
            scrnx++;
        }
    } else curloc--;
}

/* Move current position right. */
void right(void) {
    /* can't move right */
    if (curloc + 1 > endloc) return;

    scrnx++;

    /* if at end of line, go to next one */
    if (scrnx > LINE_LEN || *curloc == '\r') {
        scrnx = 0;
        scrny++;
        if (scrny == MAX_LINES) { /* at end of screen */
            scrny = MAX_LINES - 1;
            downline(); /* get next line */

            /* move cursor and current loc to start of
               new line */
            curloc--;
            while (*curloc != '\r') curloc--;
            curloc++;
            scrnx = 0;
        } else curloc++;
    } else curloc++;
}

/* Find a string. */
void search(void) {
    char str[80];
    register char *p;
    int len, i;
    ReverseVideo(true); //fontReverse=1;
    clrline(STATUS_LINE + 1); /* clear message line */
    gotoxy(0, STATUS_LINE + 1);
    PrintString("search string: ");

    edit_gets(str);
    ReverseVideo(false); //fontReverse=0;
    if (!*str) return;
    p = curloc;
    len = strlen(str);
    /* search for the string */
    while (*p && strncmp(str, p, len)) p++;
    if (!*p) return; /* not found */
    /* back up to start of line */
    i = 0;
    while (p > buf && *p != '\r') {
        p--;
        i++;
    }
    p++;
    i--;

    /* reposition current location to start of match */
    curloc = p + i;
    scrnx = i;
    scrny = 0;

    /* display screen of text at location of match */
    clrscr();
    display_scrn(0, 0, p);
    help();
}

/* Move up one line.  If possible, keep scrnx same
   as in previous line.
 */
void upline(void) {
    register int i;
    char *p;

    if (curloc == buf) return; /* at first byte in buffer */

    p = curloc;

    if (*p == '\r') p--; /* if at end of line, back up */

    /* back up locator to start of current line */
    for (; *p != '\r' && p > buf; p--);
    if (*p != '\r') return; /* at first line, cannot go up */
    curloc = p;
    curloc--; /* skip past CR */
    i = scrnx; /* save X coordinate */

    /* find start of next line */
    while (*curloc != '\r' && curloc >= buf) curloc--;
    scrny--;
    scrnx = 0;
    curloc++;

    /* at top of screen, must scroll up */
    if (scrny < 0) {
        scrolldn(0, 0);
        scrny = 0;
        gotoxy(0, 0);
        printline(curloc);
    }

    /* position the cursor and current location at
       same scrnx position as previous line if possible */
    while (i && *curloc != '\r') {
        curloc++;
        scrnx++;
        i--;
    }
}

/*  Move down one line.  Keep previous scrnx
    location if possible.
 */
void downline(void) {
    register int i;
    char *p;

    i = scrnx;
    p = curloc;

    /* advance current location to start of next line */
    while (*p != '\r' && p < endloc) p++;
    if (p == endloc) return; /* can't go down */
    p++; /* skip past CR */
    curloc = p;
    scrny++;
    scrnx = 0;

    /* if moving down past the bottom of the screen */
    if (scrny == MAX_LINES) {
        scrny = MAX_LINES - 1;
        scrollup(0, 0, LINE_LEN, MAX_LINES - 1);
        gotoxy(scrnx, scrny);
        printline(curloc);
    }

    /* advance to corresponding character in next line */
    while (i && *curloc != '\r' && curloc < endloc) {
        curloc++;
        scrnx++;
        i--;
    }
}

/* Display a screen full of text (up to MAX_LINES lines)
   starting at the specified location.
 */
void display_scrn(int x, int y, char *p) {
    register int i;
    gotoxy(x, y);
    i = 0;
    while (y < MAX_LINES && *p) {
        switch (*p) {
            case '\r':
                putch('\r');
                if(y!=(MAX_LINES-1)) putch('\n');
                y++;
                i = 0;
                break;
            default: if (i < LINE_LEN) putch(*p);
                i++;
        }
        p++;
    }

}

/* Page down MAX_LINES lines. */
void pagedown(void) {
    register int i;

    clrscr();

    /* count down MAX_LINES lines */
    for (i = 0; i < MAX_LINES && curloc < endloc;) {
        if (*curloc == '\r') i++;
        curloc++;
    }
    //  help();
    scrnx = 0;
    scrny = 0;
    display_scrn(0, 0, curloc);
    help();
}

/* Page up MAX_LINES lines. */
void pageup(void) {
    register int i;

    clrscr();
    /* if current location points to a CR,
       move current location back on position */
    if (*curloc == '\r' && curloc > buf) curloc--;

    /* go back MAX_LINES in buffer */
    for (i = 0; i < MAX_LINES + 1 && curloc > buf;) {
        if (*curloc == '\r') i++;
        curloc--;
    }

    /* if not at the top line, increment the
       current location pointer past the CR */
    if (i == MAX_LINES + 1) curloc += 2;

    //  help();
    scrnx = 0;
    scrny = 0;
    display_scrn(0, 0, curloc);
    help();
}

/* Go to the top of the file. */
void home(void) {
    //  clrscr();
    //  curloc = buf;
    //  scrnx = scrny = 0;
    //  display_scrn(0, 0, curloc);
    curloc -= scrnx;
    scrnx = 0;
    help();
}

/* Go to the end of the file. */
void gotoend(void) {
    //  clrscr();
    //  curloc = endloc;
    //  pageup();
    while (*curloc != '\r') {
        curloc++;
        scrnx++;
    }

}

/* Load a file. */
int load(char *fname) {
    char c, *p;
    MMfopen(fname, "r", FileNumber);
    p = buf;
    while (!MMfeof(FileNumber) && p != buf + BUF_SIZE - 2) {
        c = MMfgetc(FileNumber);
        if (c != '\n' && c != EOF) {
            *p = c;
            p++;
        }
    }
    *p = '\r'; // '\0';
    MMfclose(FileNumber);
    curloc = buf;
    endloc = p;
    return 1;
}

/* Save a file. */
int save(char *fname) {
    char cr = '\r';
    char lf = '\n';
    char *p, name[80];

    if (!*fname) {
        ReverseVideo(true); //fontReverse=1;
        clrline(STATUS_LINE + 1); /* clear message line */
        gotoxy(0, STATUS_LINE + 1);
        PrintString("filename: ");
        edit_gets(name);
        ReverseVideo(false); //fontReverse=0;
    } else strcpy(name, fname);
    if (*name) {
        MMfopen(name, "w", FileNumber);
        p = buf;
        while (p != endloc) {
            if (*p != '\r')
                MMfputc(*p, FileNumber);
            else {
                MMfputc(cr, FileNumber);
                MMfputc(lf, FileNumber);
            }
            p++;

        }
        MMfclose(FileNumber);
        ReverseVideo(true);
        gotoxy(0, STATUS_LINE + 1);
        PrintString("   Saved ");
        ReverseVideo(false);
        return true;
    }
    return false;
}

/* Read a string from the keyboard, but do not
   scroll the display when a RETURN is entered.
 */
void edit_gets(char *str) {
    char *p;
    p = str;
    for (;;) {
        *str = getch();
        if (*str == '\r') { /* return when RETURN entered */
            *str = '\0'; /* NULL terminate */
            return;
        }

        if (*str == '\b') { /* backspace */
            if (str > p) {
                str--;
                putch('\b');
                putch(' ');
                putch('\b');
            }
        } else {
            putch(*str);
            str++;
        }
    }
}

/* Read and save cursor coordinates. */
void cursor_pos(void) {
    //  union REGS i, o;

    //  i.h.bh = 0;
    //  i.h.ah = 3;
    //  int86(16, &i, &o);
}

/* Send cursor to specified X,Y (0,0 is upper
   left corner). */
void gotoxy(int x, int y) {
    char temp[12];
    //   MMPosX=(x * 6);
    //   MMPosY=(y * 12);
    sprintf(temp, "%c[%d;%dH", 0x1b, y + 1, x + 1); // VT100 Starts at 1,1
    PrintString(temp);
}

/* Clear entire line given its Y coordinate. */
void clrline(int y) {
    register int i;
    gotoxy(0, y);
    for (i = 0; i < LINE_LEN; i++) putch(' ');
}

/* Clear to end of specified line.  This function
   is for use with the editor only because it clears
   a line up to a carriage return. */
void edit_clr_eol(int x, int y) {
    char *p;
    p = curloc;
    gotoxy(x, y);
    for (; x < LINE_LEN && *p != '\r' && *p; x++, p++) {
        putch(' ');
    }
}

/* Clear the screen. */
void clrscr(void) {
    MMPosX = MMPosY = MMCharPos = 0;
    memset(VA, 0, 408 * (HBUFSIZE / 8));
    PrintString("\033[2J\033[H");
//    PrintString("\033[1;35r");
//    PrintString("\033[?6h");

    /*
     *  ESC [ pt ; pb r   set scroll region
     *  ESC [ ? 6 h       turn on region - origin mode
     *  ESC [ ? 6 l       turn off region - full screen mode
     */
}

/* Scroll down the screen. This function scrolls
   all but the bottom line. */
void scrolldn(int x, int y) {
    //    PrintString("\33[1;33r");
    sprintf((char*)helpline,"\033[%d;%dr",y+1,35);
    PrintString((char*)helpline);
    PrintString("\033M");
    PrintString("\033[1;35r");
    int *pd = VA + ((HBUFSIZE / 32) * 420); // was 396
    int *ps = pd - ((HBUFSIZE / 32) * 12);
    int i;

    for (i = 0; i < (HBUFSIZE / 32) * (420 - (y * 12)); i++)
        *pd-- = *ps--; // scroll down 408 was 396

    for (i = 0; i < ((HBUFSIZE / 32) * 12); i++)
        *pd++ = 0; // clear the new line

}

/* Scroll up the screen using the specified
   coordinates. */
void scrollup(int topx, int topy, int endx, int endy) {
    // PrintString("\33[1;33r");
//    sprintf(helpline,"\033[%d;%dr",topy,endy);
 //   PrintString(helpline);
    PrintString("\033D");
 //   gotoxy(0,35);
//      PrintString("                                                                                \r");
 //   PrintString("\033[1;35r");
    int *pd = VA + (topy*((HBUFSIZE / 32) * 12));
    int *ps = pd + ((HBUFSIZE / 32) * 12);
    int i;

    for (i = 0; i < (HBUFSIZE / 32) * (408 - (topy * 12)); i++) *pd++ = *ps++; // scroll up 396 before

    for (i = 0; i < ((HBUFSIZE / 32) * 12); i++) *pd++ = 0; // clear the new line
}


