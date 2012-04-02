/*



 */
/**********************************************************************************
 the C language function associated with commands, functions or operators should be
 declared here
**********************************************************************************/
#ifdef INCLUDE_FUNCTION_DEFINES
// format:
//      void cmd_???(void)
//      void fun_???(void)
//      void op_???(void)
void cmd_GDFill(void);
void cmd_GDWriteB(void);
void cmd_GDWriteW(void);
void fun_GDReadB(void);
void fun_GDReadW(void);
void cmd_GDSpriteC(void);
void cmd_GDPlayWav(void);
void fun_GDInit(void);
void cmd_GDLoadF(void);
void cmd_GDWaitVblank(void);
void fun_GDRGB(void);
void cmd_GDVoices(void);
/*
void fun_RAM_PIC(void);
void fun_RAM_CHR(void);
void fun_RAM_PAL(void);
void fun_IDENT(void);
void fun_REV(void); 
void fun_FRAME(void); 
void fun_VBLANK(void); 
void fun_SCROLL_X(void); 
void fun_SCROLL_Y(void); 
void fun_JK_MODE(void); 
void fun_J1_RESET(void); 
void fun_SPR_DISABLE(void);
void fun_SPR_PAGE(void);
void fun_IOMODE(void);
void fun_BG_COLOR(void);
void fun_SAMPLE_L(void);
void fun_SAMPLE_R(void);
void fun_MODULATOR(void);
void fun_VIDEO_MODE(void);
void fun_MODE_800x600_72(void);
void fun_MODE_800x600_60(void);
void fun_SCREENSHOT_Y(void);
void fun_PALETTE16A(void);
void fun_PALETTE16B(void);
void fun_PALETTE4A(void);
void fun_PALETTE4B(void);
void fun_COMM(void);
void fun_COLLISION(void);
void fun_VOICES(void);
void fun_J1_CODE(void);
void fun_SCREENSHOT(void);
void fun_RAM_SPR(void);
void fun_RAM_SPRPAL(void);
void fun_RAM_SPRIMG(void);
*/
#endif



/**********************************************************************************
 All command tokens tokens (eg, PRINT, FOR, etc) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)
	{ "GDWRITEB",		T_CMD,				0, cmd_GDWriteB		},
	{ "GDWRITEW",		T_CMD,				0, cmd_GDWriteW		},
	{ "GDSPRITEC",		T_CMD,				0, cmd_GDSpriteC	},
	{ "GDPLAYWAV",		T_CMD,				0, cmd_GDPlayWav	},
	{ "GDLOADF",		T_CMD,				0, cmd_GDLoadF	},
	{ "GDWAITVBLANK",	T_CMD,				0, cmd_GDWaitVblank	},
	{ "GDFILL",     	T_CMD,				0, cmd_GDFill	},
	{ "GDVOICES",     	T_CMD,				0, cmd_GDVoices	},
#endif


/**********************************************************************************
 All other tokens (keywords, functions, operators) should be inserted in this table
**********************************************************************************/
#ifdef INCLUDE_TOKEN_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is T_NA, T_FUN, T_FNA or T_OPER argumented by the types T_STR and/or T_NBR
// and P is the precedence (which is only used for operators)
	{ "GDREADB(",           T_FUN | T_NBR,  		0, fun_GDReadB	},
	{ "GDREADW(",           T_FUN | T_NBR,          	0, fun_GDReadW	},
	{ "GDINIT(",		T_FUN | T_NBR,			0, fun_GDInit	},
	{ "GDRGB(",		T_FUN | T_NBR,			0, fun_GDRGB	},
//	{ "RAM_PIC",		T_FNA | T_NBR,			0, fun_RAM_PIC	},
//	{ "RAM_CHR",		T_FNA | T_NBR,			0, fun_RAM_CHR	},
//	{ "RAM_PAL",		T_FNA | T_NBR,			0, fun_RAM_PAL	},
//	{ "IDENT",		T_FNA | T_NBR,			0, fun_IDENT	},
//	{ "REV",		T_FNA | T_NBR,			0, fun_REV	},
//	{ "FRAME",		T_FNA | T_NBR,			0, fun_FRAME	},
//	{ "VBLANK",		T_FNA | T_NBR,			0, fun_VBLANK	},
//	{ "SCROLL_X",		T_FNA | T_NBR,			0, fun_SCROLL_X	},
//	{ "SCROLL_Y",		T_FNA | T_NBR,			0, fun_SCROLL_Y	},
//	{ "JK_MODE",		T_FNA | T_NBR,			0, fun_JK_MODE	},
//	{ "J1_RESET",		T_FNA | T_NBR,			0, fun_J1_RESET	},
//	{ "SPR_DISABLE",	T_FNA | T_NBR,			0, fun_SPR_DISABLE},
//	{ "SPR_PAGE",		T_FNA | T_NBR,			0, fun_SPR_PAGE	},
//	{ "IOMODE",		T_FNA | T_NBR,			0, fun_IOMODE	},
//	{ "BG_COLOR",		T_FNA | T_NBR,			0, fun_BG_COLOR	},
//	{ "SAMPLE_L",		T_FNA | T_NBR,			0, fun_SAMPLE_L	},
//	{ "SAMPLE_R",		T_FNA | T_NBR,			0, fun_SAMPLE_R	},
//	{ "MODULATOR",		T_FNA | T_NBR,			0, fun_MODULATOR},
//	{ "VIDEO_MODE",		T_FNA | T_NBR,			0, fun_VIDEO_MODE},
//	{ "MODE_800x600_72",	T_FNA | T_NBR,			0, fun_MODE_800x600_72},
//	{ "MODE_800x600_60",	T_FNA | T_NBR,			0, fun_MODE_800x600_60},
//      { "SCREENSHOT_Y",	T_FNA | T_NBR,			0, fun_SCREENSHOT_Y},
//      { "PALETTE16A",		T_FNA | T_NBR,			0, fun_PALETTE16A},
//	{ "PALETTE16B",		T_FNA | T_NBR,			0, fun_PALETTE16B},
//	{ "PALETTE4A",		T_FNA | T_NBR,			0, fun_PALETTE4A},
//	{ "PALETTE4B",		T_FNA | T_NBR,			0, fun_PALETTE4B},
//	{ "COMM",		T_FNA | T_NBR,			0, fun_COMM	},
//	{ "COLLISION",		T_FNA | T_NBR,			0, fun_COLLISION},
//	{ "VOICES",             T_FNA | T_NBR,			0, fun_VOICES   },
//	{ "J1_CODE",		T_FNA | T_NBR,			0, fun_J1_CODE	},
//	{ "SCREENSHOT",		T_FNA | T_NBR,			0, fun_SCREENSHOT},
//	{ "RAM_SPR",            T_FNA | T_NBR,			0, fun_RAM_SPR},
//	{ "RAM_SPRPAL",		T_FNA | T_NBR,			0, fun_RAM_SPRPAL},
//	{ "RAM_SPRIMG",		T_FNA | T_NBR,			0, fun_RAM_SPRIMG},

#endif



#ifdef INCLUDE_FUNCTION_DEFINES
// General definitions used by other modules

#ifndef GameDuino_HEADER
#define GameDuino_HEADER

#define RGB(r,g,b) ((((r) >> 3) << 10) | (((g) >> 3) << 5) | ((b) >> 3))
#define TRANSPARENT (1 << 15) // transparent for chars and sprites

#define RAM_PIC     0x0000    // Screen Picture, 64 x 64 = 4096 bytes
#define RAM_CHR     0x1000    // Screen Characters, 256 x 16 = 4096 bytes
#define RAM_PAL     0x2000    // Screen Character Palette, 256 x 8 = 2048 bytes

#define IDENT         0x2800
#define REV           0x2801
#define FRAME         0x2802
#define VBLANK        0x2803
#define SCROLL_X      0x2804
#define SCROLL_Y      0x2806
#define JK_MODE       0x2808
#define J1_RESET      0x2809
#define SPR_DISABLE   0x280a
#define SPR_PAGE      0x280b
#define IOMODE        0x280c

#define BG_COLOR      0x280e
#define SAMPLE_L      0x2810
#define SAMPLE_R      0x2812

#define MODULATOR     0x2814
#define VIDEO_MODE    0x2815

#define MODE_800x600_72   0
#define MODE_800x600_60   1

#define SCREENSHOT_Y  0x281e

#define PALETTE16A 0x2840   // 16-color palette RAM A, 32 bytes
#define PALETTE16B 0x2860   // 16-color palette RAM B, 32 bytes
#define PALETTE4A  0x2880   // 4-color palette RAM A, 8 bytes
#define PALETTE4B  0x2888   // 4-color palette RAM A, 8 bytes
#define COMM       0x2890   // Communication buffer
#define COLLISION  0x2900   // Collision detection RAM, 256 bytes
#define VOICES     0x2a00   // Voice controls
#define J1_CODE    0x2b00   // J1 coprocessor microcode RAM
#define SCREENSHOT 0x2c00   // screenshot line RAM

#define RAM_SPR     0x3000    // Sprite Control, 512 x 4 = 2048 bytes
#define RAM_SPRPAL  0x3800    // Sprite Palettes, 4 x 256 = 2048 bytes
#define RAM_SPRIMG  0x4000    // Sprite Image, 64 x 256 = 16384 bytes
void GDWriteB(unsigned int Add, unsigned char Val);
char GDReadB(unsigned int Add);
void GDWriteW(unsigned int Add,unsigned int Val);
void GDFill(unsigned int Add,char Val,unsigned long Len);
void GDInit(void);
void GDCopy(unsigned int Add, unsigned int *Val,unsigned int Len);
void GDXsprite( unsigned int ox,unsigned int oy,signed char x,signed char y,unsigned int image,byte Palette,byte rot,byte jk);
void draw_sprite(int x, int y, byte anim, byte rot, byte jk );
char GDLoadF( char *fname ,unsigned int GDRam);
#endif
#endif

