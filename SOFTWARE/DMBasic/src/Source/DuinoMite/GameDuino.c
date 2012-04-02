/*
(C) 2012 Ken Segler





 */
#include <stdio.h>
#include <plib.h>

#define INCLUDE_FUNCTION_DEFINES
#include "Maximite.h"
#include "MMBasic.h"
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"
//	GS I2C Start
#include "I2C.h"
//	GS I2C End
#include "../IOPorts.h"     //ken
#include "../SDCard/HardwareProfile.h"
#include "../Video/Video.h"
#include "../SDCard/SDCard.h"
#include "../Timers/Timers.h"
#include "../Serial/Serial.h"
#include "Setup.h"
#include "../DuinoMite/RTC.h"
#include "../DuinoMite/GameDuino.h"

    unsigned int Val;
    int anim;
    int x,y;
#define SPRITE_FRAMES 56
    static inline __attribute__((always_inline)) unsigned char SPICalutateBRG(unsigned int pb_clk, unsigned int spi_clk)
{
    unsigned int brg;

    brg = pb_clk / (2 * spi_clk);

    if(pb_clk % (2 * spi_clk))
        brg++;

    if(brg > 0x100)
        brg = 0x100;

    if(brg)
        brg--;

    return (unsigned char) brg;
}
/* we do these as functions just to make the basic code easier */
/*
void fun_RAM_PIC(void) { fret = (unsigned int) RAM_PIC; }
void fun_RAM_CHR(void) { fret = RAM_CHR; }
void fun_RAM_PAL(void) { fret = RAM_PAL; }
void fun_IDENT(void) { fret = IDENT; }
void fun_REV(void) { fret = REV; }
void fun_FRAME(void) { fret = FRAME; }
void fun_VBLANK(void) { fret = VBLANK; }
void fun_SCROLL_X(void) { fret = SCROLL_X; }
void fun_SCROLL_Y(void) { fret = SCROLL_Y; }
void fun_JK_MODE(void) { fret = JK_MODE; }
void fun_J1_RESET(void) { fret = J1_RESET; }
void fun_SPR_DISABLE(void) { fret = SPR_DISABLE; }
void fun_SPR_PAGE(void) { fret = SPR_PAGE; }
void fun_IOMODE(void) { fret = IOMODE; }

void fun_BG_COLOR(void) { fret =  BG_COLOR; }
void fun_SAMPLE_L(void) { fret = SAMPLE_L; }
void fun_SAMPLE_R(void) { fret = SAMPLE_R; }
void fun_MODULATOR(void) { fret = MODULATOR; }
void fun_VIDEO_MODE(void) { fret = VIDEO_MODE; }

void fun_MODE_800x600_72(void) { fret = MODE_800x600_72; }
void fun_MODE_800x600_60(void) { fret = MODE_800x600_60; }

void fun_SCREENSHOT_Y(void) { fret = SCREENSHOT_Y; }
void fun_PALETTE16A(void) { fret = PALETTE16A; }
void fun_PALETTE16B(void) { fret = PALETTE16B; }
void fun_PALETTE4A(void) { fret = PALETTE4A; }
void fun_PALETTE4B(void) { fret = PALETTE4B; }
void fun_COMM(void) { fret = COMM; }
void fun_COLLISION(void) { fret = COLLISION; }
void fun_VOICES(void) { fret = VOICES; }
void fun_J1_CODE(void) { fret = J1_CODE; }
void fun_SCREENSHOT(void) { fret = SCREENSHOT; }

void fun_RAM_SPR(void) { fret = RAM_SPR; }
void fun_RAM_SPRPAL(void) { fret = RAM_SPRPAL; }
void fun_RAM_SPRIMG(void) { fret = RAM_SPRIMG; }
*/

void StartSpi(void)
{
    SD_CS_SET_HI; 		// SD_CS = 1;                     //Initialize Chip Select line
    SD_CS_TRIS = 0;            //Card Select - output

    SPICLOCK = 0;
    SPIOUT = 0;                  // define SDO1 as output (master or slave)
    SPIIN = 1;                  // define SDI1 as input (master or slave)

    ExtCfg(7,EXT_DIG_OUT);
    ExtSet(7,1);
    OpenSPI((PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_OFF  ), SPI_ENABLE); //
    SPIBRG = SPICalutateBRG(GetPeripheralClock(), 16000000);
    SDCardRemoved=true;
}
void GDInit(void)
{
    StartSpi();
    mSec(200);
    GDWriteB(J1_RESET,1);
    if(S.GameDuino == 1) GDWriteB(VIDEO_MODE,MODE_800x600_60); else GDWriteB(VIDEO_MODE,MODE_800x600_72);
    GDFill(RAM_PIC,0,4096);
    GDFill(RAM_PAL,0,2048);
    GDFill(RAM_CHR,0,4096);
    GDFill(RAM_SPRPAL,0,2048);
    GDFill(RAM_SPR,0,2048);
    GDFill(RAM_SPRIMG,0,16384);
    GDFill(VOICES,0,256);
    GDFill(PALETTE16A,0,128);


    SPISTAT &= 0x7FFF;
 }
void fun_GDInit(void){
    getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// getargs macro must be the first executable stmt in a block
    GDWriteB(J1_RESET,1);

    if(S.GameDuino == 1) GDWriteB(VIDEO_MODE,MODE_800x600_60); else GDWriteB(VIDEO_MODE,MODE_800x600_72);
    GDFill(RAM_PIC,0,4096);
    GDFill(RAM_SPRPAL,0,2048);
    GDFill(RAM_SPR,0,2048);
    GDFill(VOICES,0,256);
    GDFill(PALETTE16A,0,128);
    SPI3STATCLR = 1<<6;
    fret = (float)GDReadB(0x2800);      //return board id should be 0x6d or 109

 }

void GDCopy(unsigned int Add,unsigned int *val,unsigned int Len)
{
    char *ptr;
    ptr= val[0];
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    Add |=0x8000;
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    uSec(1);
    while(Len--)
    {
    while(!SPISTATbits.SPITBE);
    putcSPI(*ptr++);
    }
    val=SPI3BUF;
    SPI3STATCLR = 1<<6;
    ExtSet(7,1);
}
void cmd_GDFill(void){
    unsigned int add,len;
    char val;
    getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// this is a macro and must be the first executable stmt
    if(argc < 3) error("Invalid syntax");
    add=getinteger(argv[0]);
    val=getinteger(argv[2]);
    len=getinteger(argv[4]);
    GDFill(add,val,len);
}
void GDFill(unsigned int Add,char Val,unsigned long Len)
{
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    Add |=0x8000;
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    uSec(1);
    while(Len--)
    {
    while(!SPISTATbits.SPITBE);
        putcSPI(Val);
    }
    Val=SPI3BUF;
    SPI3STATCLR = 1<<6;
    ExtSet(7,1);
}

void GDWriteW(unsigned int Add,unsigned int Val)
{
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    Add |=0x8000;
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    while(!SPISTATbits.SPITBE);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Val);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Val>>8);
    uSec(1);
    ExtSet(7,1);
    Val=SPI3BUF;
    SPI3STATCLR = 1<<6;
}

void GDWriteB(unsigned int Add, unsigned char Val)
{
    ExtCfg(7,EXT_DIG_OUT);
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    Add |=0x8000;
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    while(!SPISTATbits.SPITBE);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Val);
    uSec(1);
    ExtSet(7,1);
    Val=SPI3BUF;
    SPI3STATCLR = 1<<6;

}

void cmd_GDPlayWav(void)
{
/*
#define FileNumber 5
    char c;
        unsigned char WavName[64];
        strcpy(WavName,"1.MID");

        if (SDFileExists(WavName)) {
        sprintf(inpbuf,"COM4:%d",31250);
        SerialOpen(inpbuf, 0);
      
        MMPrintString("Found it\r\n");
        MMfopen(WavName, "r", FileNumber);
        while (!MMfeof(FileNumber) ) {
        c = MMfgetc(FileNumber);
        SerialPutchar(4, c);
        mSec(100);
        }
        MMfclose(FileNumber);
        SerialClose(4);
        }
 */
}
void cmd_GDWriteB(void)
{
    unsigned int Add;
    unsigned char Val;
    getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// getargs macro must be the first executable stmt in a block
    if(argc < 3) error("Invalid syntax");
    Add=getinteger(argv[0]);
    Val=getinteger(argv[2]);
    GDWriteB(Add,Val);
}
void cmd_GDWriteW(void)
{
    unsigned int Add;
    getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// getargs macro must be the first executable stmt in a block
    if(argc < 3) error("Invalid syntax");
    ExtCfg(7,EXT_DIG_OUT);
    Add=getinteger(argv[0]);
    Val=getinteger(argv[2]);
    GDWriteW(Add,Val);
}

char GDReadB(unsigned int Add)
{
    ExtCfg(7,EXT_DIG_OUT);
    Val=SPI3BUF;
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    while(!SPISTATbits.SPIRBF);
    Val=SPI3BUF;
    while(SPISTATbits.SPIBUSY);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    while(!SPISTATbits.SPIRBF);
    Val=SPI3BUF;
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(0xff);
    while(!SPISTATbits.SPIRBF);
    Val=SPI3BUF;
    uSec(1);
    ExtSet(7,1);
    SPI3STATCLR = 1<<6;
    return Val;
}
void fun_GDReadB(void)
{
    unsigned int Add;
    getargs(&ep, 1, ",");
    if(argc != 1) error("Invalid syntax");
    Add=getinteger(argv[0]);
    fret = (float)GDReadB(Add);
}
void fun_GDReadW(void)
{
    unsigned int Add;
    unsigned int Val;
    getargs(&ep, 1, ",");
    if(argc != 1) error("Invalid syntax");
    ExtCfg(7,EXT_DIG_OUT);
    Add=getinteger(argv[0]);
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    while(!SPISTATbits.SPIRBF);
    Val=SPI3BUF;
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    while(!SPISTATbits.SPIRBF);
    Val=SPI3BUF;
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(0xff);
    while(!SPISTATbits.SPIRBF);
    Val =(SPI3BUF<<8);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(0xff);
    while(!SPISTATbits.SPIRBF);
    Val +=SPI3BUF;

    ExtSet(7,1);
    SPI3STATCLR = 1<<6;
    fret = (float)Val;
}
/*
 GDSPRITEC sprite#,x,y,image,pal,rot,jk

 */
void cmd_GDSpriteC(void)
{
    unsigned int Add;
    unsigned int X;
    unsigned int Y;
    unsigned int Sprite;
    char Palette;
    char Rot;
    char JK;
    char Image;


    getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// getargs macro must be the first executable stmt in a block
    if(argc < 7) error("Invalid syntax");
    Add=0x3000;
    Sprite=getinteger(argv[0]);
    X=getinteger(argv[2]);
    Y=getinteger(argv[4]);
    Image=getinteger(argv[6]);
    Palette=getinteger(argv[8]);
    Rot=getinteger(argv[10]);
    JK=getinteger(argv[12]);
    Add +=(Sprite<<2);
    GDWriteB(Add,X & 0x00ff);
    Add++;
    X >>=8;
    GDWriteB(Add,(Palette <<4 )|(Rot <<1)| (X & 1));
    Add++;
    GDWriteB(Add,Y & 0x00ff);
    Add++;
    Y >>=8;
    GDWriteB(Add,(JK << 7)|(Image <<1)|(Y & 1));


}
void GDXsprite( unsigned int ox,unsigned int oy,signed char x,signed char y,unsigned int image,byte palette,byte rot,byte jk)
{
    unsigned int Add;
    if( rot &2 )
        x = -16-x;
    if( rot &4 )
        y = -16-y;
    if( rot & 1 ) {
        int s=0;
        s = s ; x = y ; y = s;
    }
    ox += x;
    oy += y;
    Add=0x3000;
    Add +=(anim<<2);
    GDWriteB(Add,ox);
    Add++;
    ox >>=8;
    GDWriteB(Add,(palette <<4 )|(rot <<1)| (ox & 1));
    Add++;
    GDWriteB(Add,oy);
    Add++;
    oy >>=8;
    GDWriteB(Add,(jk << 7)|(image <<1)|(oy & 1));
}

void cmd_GDLoadF(void)
{
     char name[13];
     getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// this is a macro and must be the first executable stmt
     strcpy(name ,getCstring(argv[0]));
     makeupper(name);
     GDLoadF(name,getinteger(argv[2]));

}

char GDLoadF( char *fname ,unsigned int GDRam) {
    /* Load a file. */
#define BUF_SIZE 16384
    char c,*p,*buf;
    int FileNumber,Len = 0;
    buf = calloc(16384, 1);
    if (buf == NULL) error("Cant calloc Memory");
    if((FileNumber=FindFreeFileNbr()) == 0) error("No Free File Numbers");
    if (!SDFileExists(fname)) error("File Not Found");
    MMfopen(fname, "r", FileNumber);
    p = buf;
    while (!MMfeof(FileNumber) && p != buf + BUF_SIZE - 2) {
        c = MMfgetc(FileNumber);
            *p = c;
            p++;
            Len++;
    }
    p=buf;
    MMfclose(FileNumber);
    ExtCfg(7,EXT_DIG_OUT);
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    GDRam |=0x8000;
    while(!SPISTATbits.SPITBE);
    putcSPI(GDRam>>8);
    while(!SPISTATbits.SPITBE);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(GDRam);
    uSec(1);
    p=buf;
    while(Len--){
    while(!SPISTATbits.SPITBE);
    putcSPI(*p++);
    uSec(1);
    }
    ExtSet(7,1);
    Val=SPI3BUF;
    SPI3STATCLR = 1<<6;
    free(buf);
    return 1;
}
void cmd_GDWaitVblank(void)
{
    while(GDReadB(VBLANK)==1);
    while(GDReadB(VBLANK)==0);
}
void fun_GDRGB(void)
{
    int R,G,B,Temp;
    R=G=B=Temp=0;
    getargs(&ep, 5, ",");				// this is a macro and must be the first executable stmt
    if(argc < 3) error("Invalid syntax");
    R =getinteger(argv[0]);
    G =getinteger(argv[2]);
    B =getinteger(argv[4]);
    Temp=(((R >> 3) << 10) | ((G >> 3) << 5) | (B >> 3));
    fret = (float)Temp;
}
/* Controll Gameduino voices voice,freq, W,LeftV, RightV*/
void cmd_GDVoices(void)
{
    int freq,Add;
    char voice,w,lv,rv;
    getargs(&cmdline, (MAX_ARG_COUNT * 2) - 1, ",");				// this is a macro and must be the first executable stmt
    voice =getinteger(argv[0]);
    freq =getinteger(argv[2]);
    w =getinteger(argv[4]);
    lv =getinteger(argv[6]);
    rv =getinteger(argv[8]);
    Add=0x2A00; //VOICES;
    Add |=0x8000;
    Add +=(voice*4);
    uSec(1);
    ExtSet(7,0);
    uSec(1);
    if(w) freq |=0x8000;
    while(!SPISTATbits.SPITBE);
    putcSPI(Add>>8);
    while(!SPISTATbits.SPITBE);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(Add);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(freq);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(freq>>8);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(lv);
    uSec(1);
    while(!SPISTATbits.SPITBE);
    putcSPI(rv);
    uSec(1);
    ExtSet(7,1);
    Val=SPI3BUF;
    SPI3STATCLR = 1<<6;
}

