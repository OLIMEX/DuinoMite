/************************************************************************************************************************
Maximite

Video.c

Based on an idea and code by Lucio Di Jasio presented in his excellent book
"Programming 32-bit Microcontrollers in C - Exploring the PIC32".

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/


#include <p32xxxx.h>
#include <plib.h>
#include <string.h>

#include "./Video.h"
#include "../Maximite.h"
#include "../IOPorts.h"
#include "../Setup.h"

#include "./USB/Microchip/Include/USB/usb.h"
#include "./USB/Microchip/Include/USB/usb_function_cdc.h"
#include "./USB/HardwareProfile.h"

// Parameters for VGA video with 31.5KHz horizontal scanning and 60Hz vertical refresh
// Graphics is 480x432 pixels.  This gives us 80 chars per line and 36 lines per screen
#define VGA_VRES     432						// Vert graphics resolution (pixels)
#define VGA_HRES     480						// Horiz graphics resolution (pixels)
#define VGA_LINE_N   525        						// number of lines in VGA frame
#define VGA_LINE_T   2540       						// Tpb clock in a line (31.777us)
#define VGA_VSYNC_N  2          						// V sync lines
#define VGA_VBLANK_N (VGA_LINE_N - VGA_VRES - VGA_VSYNC_N)       		// Nbr of blank lines
#define VGA_PREEQ_N   ((VGA_VBLANK_N/2) - 12)         				// Nbr blank lines at the bottom of the screen
#define VGA_POSTEQ_N  VGA_VBLANK_N - VGA_PREEQ_N 				// Nbr blank lines at the top of the screen
#define VGA_PIX_T    4          						// Tpb clock per pixel
#define VGA_HSYNC_T  300        						// Tpb clock width horizontal pulse
#define VGA_BLANKPAD  5								// number of zero bytes before sending data
/*
// Parameters for PAL composite video
// Graphics is 304x216 pixels.  This gives us 50 chars per line and 18 lines per screen
#define C_VRES	     216											// Vert graphics resolution (pixels)
#define C_HRES	     304											// Horiz graphics resolution (pixels)
#define C_LINE_N     312        									// number of lines in PAL frames
#define C_LINE_T     5120       									// Tpb clock in a line (64us)
#define C_VSYNC_N    3          									// V sync lines
#define C_VBLANK_N   (C_LINE_N - C_VRES - C_VSYNC_N)  				// Nbr of blank lines
#define C_PREEQ_N    ((C_VBLANK_N/2) - 8)         					// Nbr blank lines at the bottom of the screen
#define C_POSTEQ_N   (C_VBLANK_N - C_PREEQ_N)  						// Nbr blank lines at the top of the screen
#define C_PIX_T      11          									// Tpb clock per pixel
#define C_HSYNC_T    374        									// Tpb clock width horizontal pulse
#define C_BLANKPAD   8												// number of zero words (4 bytes each) before sending data
*/

// Parameters for NTSC composite video
// Graphics is 304x216 pixels.  This gives us 50 chars per line and 18 lines per screen
/*
#define C_VRES	     234 //216							// Vert graphics resolution (pixels)
#define C_HRES	     480//304							// Horiz graphics resolution (pixels)
#define C_LINE_N     262 //(525 / 2)        					// number of lines in NTSC frames
#define C_LINE_T     5080//5084       						// Tpb clock in a line (63.55us)
#define C_VSYNC_N    3          						// V sync lines
#define C_VBLANK_N   (C_LINE_N - C_VRES - C_VSYNC_N)  				// Nbr of blank lines
#define C_PREEQ_N    ((C_VBLANK_N/2) - 8)         				// Nbr blank lines at the bottom of the screen
#define C_POSTEQ_N   (C_VBLANK_N - C_PREEQ_N)  					// Nbr blank lines at the top of the screen
#define C_PIX_T      9//11          						// Tpb clock per pixel
#define C_HSYNC_T    374        						// Tpb clock width horizontal pulse
#define C_BLANKPAD   4//8							// number of zero words (4 bytes each) before sending data
*/


// states of the vertical sync state machine
#define SV_PREEQ    0								// generating blank lines before the vert sync
#define SV_SYNC     1								// generating the vert sync
#define SV_POSTEQ   2								// generating blank lines after the vert sync
#define SV_LINE     3								// visible lines, send the video data out

// global variables used in other parts of the Maximite
int VRes, HRes;									// Global vert and horiz resolution
int VA[VBUFSIZE*(HBUFSIZE/32)] __attribute__ ((aligned (4)));                   // image buffer.  it is assumed that the startup code will zero this

int vga;									// true if we are using the VGA video

volatile int VCount;								// counter for the number of lines in a frame
volatile int VState;								// the state of the state machine

int VS[4] = { SV_SYNC, SV_POSTEQ, SV_LINE, SV_PREEQ};				// the next state table
int VC[4];									// the next counter table (initialise in initVideo() below)

int zero[] = {0, 0, 0};



/**************************************************************************************************
Initialise the video components
***************************************************************************************************/
void initVideo( void)
{
    // setup the I/O pins used by the video
#ifdef MAXIMITE
    CNCONbits.ON = 1;       						// turn on Change Notification module
    P_VGA_COMP_PULLUP = P_ON;						// turn on the pullup for pin C14 also called CN0
#endif
    TRISBbits.TRISB11=1;
    vga = (P_VGA_COMP == P_VGA_SELECT);						// vga will be true if the jumper is NOT there
    P_VIDEO_TRIS = P_OUTPUT;  //P_VIDEO = 1;					// Video output
    P_HORIZ_TRIS = P_OUTPUT;  //P_HORIZ = 1;                                    // Horiz sync output
    if(vga)
        P_VERT_TRIS = P_OUTPUT;  //P_VERT_SET_HI;				// Vert sync output used by VGA

    if(vga)
    {
        VRes = VGA_VRES;
        HRes = VGA_HRES;
        VC[0] = VGA_VSYNC_N;						// setup the table used to count lines
        VC[1] = VGA_POSTEQ_N;
        VC[2] = VGA_VRES;
        VC[3] = VGA_PREEQ_N;
        // enable the SPI channel which will clock the video data out.  Set master and framing mode.  The last arg sets the speed
        SpiChnOpen(P_VIDEO_SPI, SPICON_ON | SPICON_MSTEN |  SPICON_MODE32 | SPICON_FRMEN | SPICON_FRMSYNC | SPICON_FRMPOL, VGA_PIX_T);
        // enable the output compare which is used to time the width of the horiz sync pulse
#ifdef MAXIMITE
        OpenOC3(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, VGA_HSYNC_T);
#endif
// SPP +
#ifdef OLIMEX
    #ifdef	OLIMEX_DUINOMITE_EMEGA	// patch for eMega
        OpenOC3(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, VGA_HSYNC_T);
    #else	// original by Geoff Graham for DuinoMite-Mega
        OpenOC5(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, VGA_HSYNC_T);
    #endif
#else
        OpenOC3(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, VGA_HSYNC_T);
#endif
// SPP -
        // enable timer 3 and set to the horizontal scanning frequency
        OpenTimer3( T3_ON | T3_PS_1_1 | T3_SOURCE_INT, VGA_LINE_T-1);
    }
    else
    {	// this is for the composite output and is the same as VGA with timing differences
        VRes = S.C_VRES;
        HRes = S.C_HRES;
        VC[0] = S.C_VSYNC_N;
        VC[1] = S.C_POSTEQ_N;
        VC[2] = S.C_VRES;
        VC[3] = S.C_PREEQ_N;
        SpiChnOpen(P_VIDEO_SPI, SPICON_ON | SPICON_MSTEN | SPICON_MODE32 | SPICON_FRMEN | SPICON_FRMSYNC | SPICON_FRMPOL, S.C_PIX_T);
#ifdef MAXIMITE
        OpenOC3(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, S.C_HSYNC_T);
#endif
#ifdef OLIMEX
    #ifdef	OLIMEX_DUINOMITE_EMEGA	// patch for eMega
        OpenOC3(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, S.C_HSYNC_T);
    #else
        OpenOC5(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, S.C_HSYNC_T);
    #endif
#endif
        OpenTimer3(T3_ON | T3_PS_1_1 | T3_SOURCE_INT, S.C_LINE_T-1);
    }

    VState = SV_PREEQ;  // initialise the state machine
    VCount = 1;         // set the count so that the first interrupt will increment the state

    // setup DMA 1 to send data to SPI channel 2
    DmaChnOpen(1, 1, DMA_OPEN_DEFAULT);
    DmaChnSetEventControl(1, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(P_SPI_INTERRUPT));

    if(vga)
        DmaChnSetTxfer(1, (void*)VA, (void *)&P_SPI_INPUT, VGA_HRES/8, 4, 4);
    else
    {
        DmaChnSetTxfer(1, (void*)zero, (void *)&P_SPI_INPUT, S.C_BLANKPAD, 4, 4);
        // setup DMA 0 to pump the data from the graphics buffer to the SPI peripheral
        DmaChnOpen( 0, 0, DMA_OPEN_DEFAULT);
        DmaChnSetEventControl(0, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(P_SPI_INTERRUPT));
        DmaChnSetTxfer(0, (void*)VA, (void *)&P_SPI_INPUT, S.C_HRES/8 + 6, 4, 4);
        // chain DMA 0 so that it will start on completion of the DMA 1 transfer
        DmaChnSetControlFlags(0, DMA_CTL_CHAIN_EN | DMA_CTL_CHAIN_DIR);
    }

    mT3SetIntPriority(7);    							// set priority level 7 for the timer 3 interrupt to use shadow register set
    mT3IntEnable(1);								// Enable Timer3 Interrupt
}


/**************************************************************************************************
Timer 3 interrupt.
Used to generate the horiz and vert sync pulse under control of the state machine
***************************************************************************************************/
void __ISR(_TIMER_3_VECTOR, ipl7) T3Interrupt(void) {
	static int *VPtr;
    switch ( VState) {    							// vertical state machine
        case SV_PREEQ:  // 0
            VPtr = VA;            									// prepare for the new frame
            break;

        case SV_SYNC:   // 1
#ifdef MAXIMITE
            if(!vga) OC3R = S.C_LINE_T - S.C_HSYNC_T;			        // start the vertical sync pulse for composite
#endif
#ifdef OLIMEX
    #ifdef	OLIMEX_DUINOMITE_EMEGA	// patch for eMega
            if(!vga) OC3R = S.C_LINE_T - S.C_HSYNC_T;			        // start the vertical sync pulse for composite
    #else
            if(!vga) OC5R = S.C_LINE_T - S.C_HSYNC_T;			        // start the vertical sync pulse for composite
    #endif
#endif
            if(vga) P_VERT_SET_LO;						// start the vertical sync pulse for vga
            break;

        case SV_POSTEQ: // 2
#ifdef MAXIMITE
        	if(!vga) OC3R = S.C_HSYNC_T; 			        	// end of the vertical sync pulse for composite
#endif
#ifdef	OLIMEX
    #ifdef	OLIMEX_DUINOMITE_EMEGA	// patch for eMega
        	if(!vga) OC3R = S.C_HSYNC_T; 			        	// end of the vertical sync pulse for composite
    #else
        	if(!vga) OC5R = S.C_HSYNC_T; 			        	// end of the vertical sync pulse for composite
    #endif
#endif
            if(vga) P_VERT_SET_HI;						// end of the vertical sync pulse for vga
            break;

        case SV_LINE:   // 3
            P_SPI_INPUT = 0;            					// preload the SPI with 4 zero bytes to pad the start of the video
            if(vga)
            	DCH1SSA = KVA_TO_PA((void*) (VPtr));                            // update the DMA1 source address (DMA1 is used for VGA data)
            else
            	DCH0SSA = KVA_TO_PA((void*) (VPtr - 1));                        // update the DMA0 source address (DMA0 is used for composite data)
            VPtr += HBUFSIZE/32;						// move the pointer to the start of the next line
            DmaChnEnable(1);							// arm the DMA transfer
            break;
   }

    if (--VCount == 0) {							// count down the number of lines
        VCount = VC[VState&3];							// set the new count
        VState = VS[VState&3];    						// and advance the state machine
    }

    mT3ClearIntFlag();    							// clear the interrupt flag
}




/**************************************************************************************************
Turn on or off a single pixel in the graphics buffer
***************************************************************************************************/
void plot(int x, int y, int b) {

    if(x >= 0 && x < HRes && y >= 0 && y < VRes) {
      	if(b)
		    VA[y * (VGA_HRES/32) + x/32] |= (0x80000000>>(x & 0x1f));	// turn on the pixel
       	else
	        VA[y * (VGA_HRES/32) + x/32] &= ~(0x80000000>>(x & 0x1f));    	// turn off the pixel
    }
}



/**************************************************************************************************
Get the value of a single pixel in the graphics buffer
***************************************************************************************************/
int pixel(int x, int y)
{
    if(x >= 0 && x < HRes && y >= 0 && y < VRes)
		if((VA[y * (VGA_HRES/32) + x/32]) & (0x80000000>>(x & 0x1f)))
			return 1;
       	else
	        return 0;
   	else
    	return 0;
}





/**************************************************************************************************
clear the screen
***************************************************************************************************/
void MMcls(void) {
	MMPosX = MMPosY = MMCharPos = 0;
	memset(VA, 0, VBUFSIZE*(HBUFSIZE/8));
}




/**************************************************************************************************
Draw a line on a the video output
	(x1, y1) - the start coordinate
	(x2, y2) - the end coordinate
	colour - zero for erase, non zero to draw
***************************************************************************************************/
#define abs( a)     (((a)> 0) ? (a) : -(a))

void MMline(int x1, int y1, int x2, int y2, int colour) {
   int  x, y, addx, addy, dx, dy;
   int P;
   int i;

   dx = abs(x2 - x1);
   dy = abs(y2 - y1);
   x = x1;
   y = y1;

   if(x1 > x2)
      addx = -1;
   else
      addx = 1;

   if(y1 > y2)
      addy = -1;
   else
      addy = 1;

   if(dx >= dy) {
      P = 2*dy - dx;
      for(i=0; i<=dx; ++i) {
         plot(x, y, colour);
         if(P < 0) {
            P += 2*dy;
            x += addx;
         } else {
            P += 2*dy - 2*dx;
            x += addx;
            y += addy;
         }
      }
   } else {
      P = 2*dx - dy;
      for(i=0; i<=dy; ++i) {
         plot(x, y, colour);
         if(P < 0) {
            P += 2*dx;
            y += addy;
         } else {
            P += 2*dx - 2*dy;
            x += addx;
            y += addy;
         }
      }
   }
}




/**********************************************************************************************
Draw a box on the video output
     (x1, y1) - the start coordinate
     (x2, y2) - the end coordinate
     fill  - 0 or 1
     colour - 0 or 1
***********************************************************************************************/
void MMbox(int x1, int y1, int x2, int y2, int fill, int colour) {
   if(fill) {
      int y, ymax;                          // Find the y min and max
      if(y1 < y2) {
         y = y1;
         ymax = y2;
      } else {
         y = y2;
         ymax = y1;
      }

      for(; y<=ymax; ++y)                    // Draw lines to fill the rectangle
         MMline(x1, y, x2, y, colour);
   } else {
      MMline(x1, y1, x2, y1, colour);      	// Draw the 4 sides
      MMline(x1, y2, x2, y2, colour);
      MMline(x1, y1, x1, y2, colour);
      MMline(x2, y1, x2, y2, colour);
   }
}





/***********************************************************************************************
Draw a circle on the video output
	(x,y) - the center of the circle
	radius - the radius of the circle
	fill - non zero
	colour - zero for erase, non zero to draw
***********************************************************************************************/
void MMCircle(int x, int y, int radius, int fill, int colour) {
   int a, b, P;

   a = 0;
   b = radius;
   P = 1 - radius;

   do {
      if(fill) {
         MMline(x-a, y+b, x+a, y+b, colour);
         MMline(x-a, y-b, x+a, y-b, colour);
         MMline(x-b, y+a, x+b, y+a, colour);
         MMline(x-b, y-a, x+b, y-a, colour);
      } else {
         plot(a+x, b+y, colour);
         plot(b+x, a+y, colour);
         plot(x-a, b+y, colour);
         plot(x-b, a+y, colour);
         plot(b+x, y-a, colour);
         plot(a+x, y-b, colour);
         plot(x-a, y-b, colour);
         plot(x-b, y-a, colour);
      }

      if(P < 0)
         P+= 3 + 2*a++;
      else
         P+= 5 + 2*(a++ - b--);
    } while(a <= b);
}
