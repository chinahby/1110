/**
 * @file A8_graphics.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#if !defined (AIT_GRAPHICS_H)    
#define AIT_GRAPHICS_H

#define	GRAPHICS_REG_BASE		((void *)0x02006B00)

#define	MAX_BUFFER	10
#define	MAX_WIDTH	320
#define	MAX_HEIGHT	320
#define	FIFO_DEPTH	16
#define	FIFO_WIDTH	16
#define	FIFO_RD_THRESHOLD	8
#define	FIFO_WR_THRESHOLD	8
#define	FIFO_RD_WR_THRESHOLD	0x0808

#define		_SAMSUNG_PANEL_
//#define		_QUANTA_PANEL_
//#define		_SONY_PANEL_

#define		_8_BITS_
//#define		_16_BITS_
//#define		_24_BITS_
//#define		_8_24_BITS_

#define	AUTO_REFRESH

#ifdef	_SAMSUNG_PANEL_
#define	WINDOW_WIDTH	128	
#define	WINDOW_HEIGHT	160
#endif
#ifdef	_SONY_PANEL_
#define	WINDOW_WIDTH	160
#define	WINDOW_HEIGHT	200
#endif
#ifdef	_QUANTA_PANEL_
#define	WINDOW_WIDTH	120	
#define	WINDOW_HEIGHT	160
#endif

// Registers for Graphics Engine
//
#define GRAPHICS_ENGINE_REG_BASE          (0x6B00)

#define GRH_SRC_ADDR_0			(0x050 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_ADDR_1			(0x051 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_ADDR_2			(0x052 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_ADDR_3			(0x053 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_OFFSET_0		(0x054 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_OFFSET_1		(0x055 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_OFFSET_2		(0x056 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_SRC_OFFSET_3		(0x057 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_ADDR_0			(0x058 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_ADDR_1			(0x059 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_ADDR_2			(0x05A + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_ADDR_3			(0x05B + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_OFFSET_0		(0x05C + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_OFFSET_1		(0x05D + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_OFFSET_2		(0x05E + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_OFFSET_3		(0x05F + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_ADDR_0			(0x060 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_ADDR_1			(0x061 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_ADDR_2			(0x062 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_ADDR_3			(0x063 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_OFFSET_0		(0x064 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_OFFSET_1		(0x065 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_OFFSET_2		(0x066 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_OFFSET_3		(0x067 + GRAPHICS_ENGINE_REG_BASE)

#define GRH_BLT_WIDTH_0			(0x012 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BLT_WIDTH_1			(0x013 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BLT_HEIGHT_0		(0x014 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BLT_HEIGHT_1		(0x015 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FORE_COLOR_0		(0x016 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FORE_COLOR_1		(0x017 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FORE_COLOR_2		(0x018 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BACK_COLOR_0		(0x019 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BACK_COLOR_1		(0x01A + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BACK_COLOR_2		(0x01B + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_WIDTH			(0x01C + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_HEIGHT			(0x01D + GRAPHICS_ENGINE_REG_BASE)
#define GRH_COLOR_FORMAT		(0x01E + GRAPHICS_ENGINE_REG_BASE)
#define GRH_PAT_START_PTR		(0x01F + GRAPHICS_ENGINE_REG_BASE)
#define GRH_ROP				(0x020 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_BITBLT_CTL			(0x021 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_WR_TH			(0x022 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_RD_TH			(0x023 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_ENABLE_CTL			(0x024 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_STATUS			(0x025 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_UNWR_WORD		(0x026 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_UNRD_WORD		(0x027 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_DAT_PORT_0		(0x028 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_FIFO_DAT_PORT_1		(0x029 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_INT_ENABLE			(0x02A + GRAPHICS_ENGINE_REG_BASE)
#define GRH_INT_FLAG			(0x02B + GRAPHICS_ENGINE_REG_BASE)
#define GRH_LINE_CTL			(0x02C + GRAPHICS_ENGINE_REG_BASE)
#define GRH_RGB_CTL			(0x02D + GRAPHICS_ENGINE_REG_BASE)

#define GRH_DST_PIXEL_OFFSET_0		(0x032 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_DST_PIXEL_OFFSET_1		(0x033 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_ROTATE_CTL			(0x034 + GRAPHICS_ENGINE_REG_BASE)

#define GRH_JPEG_YUV_CTL		(0x036 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_JPEG_BUF_CTL		(0x037 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_JPEG_WIDTH			(0x038 + GRAPHICS_ENGINE_REG_BASE)
#define	GRH_JPEG_HEIGHT			(0x039 + GRAPHICS_ENGINE_REG_BASE)
#define GRH_JPEG_ADDR_0			(0x03A + GRAPHICS_ENGINE_REG_BASE)
#define	GRH_JPEG_ADDR_1			(0x03B + GRAPHICS_ENGINE_REG_BASE)

typedef struct
{
    u_char       SRC_ADDR[3];
    u_char       SRC_OFFSET[3];
    u_char       DST_ADDR[3];
    u_char       DST_OFFSET[3];
    u_char       PAT_ADDR[3];
    u_char       PAT_OFFSET[3];
    u_char       BLT_WIDTH[2];
    u_char       BLT_HEIGHT[2];
    u_char       FORE_COLOR[3];
    u_char       BACK_COLOR[3];
    u_char       PAT_HEIGHT;
    u_char       PAT_WIDTH;
	u_char		COLOR_FORMAT;
	u_char		PAT_START_PTR;
	u_char		ROP;
	u_char		BITBLT_CTL;
	u_char		FIFO_WR_TH;
	u_char		FIFO_RD_TH;
	u_char		ENABLE_CTL;
	u_char		FIFO_STATUS;
	u_char		FIFO_UNWR_WORD;
	u_char		FIFO_UNRD_WORD;
	u_char		FIFO_DAT_PORT[2];
	u_char		INT_ENABLE;
	u_char		INT_FLAG;
	u_char		LINE_CTL;
	u_char		RGB_CTL;
	u_char		RESERVE[4];
	u_char		DST_PIXEL_OFFSET[2];
	u_char		ROTATE_CTL;
	u_char		RESERVE1;
	u_char		JPEG_YUV_CTL;
	u_char		JPEG_BUF_CTL;
	u_char		JPEG_WIDTH;
	u_char		JPEG_HEIGHT;
	u_char		JPEG_ADDR[2];
} StructGRH_8;

typedef struct
{
 	u_short		SRC_ADDR1_0;			//0x00
	u_short		SRC_OFFSET0_SRC_ADDR2;	//0x02
	u_short		SRC_OFFSET2_1;			//0x04	
	u_short		DST_ADDR1_0;			//0x06
	u_short		DST_OFFSET0_DST_ADDR2;	//0x08
	u_short		DST_OFFSET2_1;			//0x0a
	u_short		PAT_ADDR1_0;			//0x0c
	u_short		PAT_OFFSET0_PAT_ADDR2;	//0x0e
	u_short		PAT_OFFSET2_1;			//0x10
	u_short		BLT_WIDTH;				//0x12
	u_short		BLT_HEIGHT;				//0x14
	u_short		FORE_COLOR1_0;			//0x16
	u_short		BACK_COLOR0_FORE_COLOR2;	//0x18
	u_short		BACK_COLOR2_1;			//0x1a
	u_short		PAT_WIDTH_HEIGHT;		//0x1c
	u_short		PAT_START_PTR_COLOR_FORMAT;	//0x1e
	u_short		BITBLT_CTL_ROP;			//0x20	
	u_short		FIFO_RD_WR_TH;			//0x22
	u_short		FIFO_STATUS_ENABLE_CTL;	//0x24
	u_short		FIFO_UNRD_UNWR_WORD;	//0x26
	u_short		FIFO_DAT_PORT;			//0x28
	u_short		INT_ENABLE_FLAG;		//0x2a	
	u_short		RGB_LINE_CTL;			//0x2c
	u_short		RESERVE[2];				//0x2e	//0x30
	u_short		DST_PIXEL_OFFSET;		//0x32
	u_short		RSV_ROTATE_CTL;			//0x34
	u_short		JPEG_BUF_YUV_CTL;		//0x36
	u_short		JPEG_HEIGHT_WIDTH;		//0x38
	u_char		JPEG_ADDR1_0;			//0x3a

} StructGRH_16;

typedef struct
{
    u_short		width;
    u_short		height;
    u_short		lineoffset;
    u_int		startaddr;
    u_short		colordepth;
} StructBUF;

typedef struct	_VERTEX_P
{
	u_short	x;
	u_short	y;
} VERTEX_P;

typedef struct	_LINE_INFO
{
	int						x;
	int						delta_x;
	u_short			y_max;
	u_char			line_type;
	u_char			slope_type;
	struct	_LINE_INFO		*pNext;	
} LINE_INFO;

typedef struct	_LINK_LIST
{
	LINE_INFO	*pt1stEntry;
} LINK_LIST;

/*----------------------------------*/
/* COLOR_FORMAT: Color format		*/
/*----------------------------------*/
#define	GRH_BIT_DAT_CON			0x80
#define	GRH_BIT_DAT_NOT_CON		0x00
#define	GRH_BIT_DAT_M_2_L		0x40
#define	GRH_BIT_DAT_L_2_M		0x00

/*----------------------------------*/
/* GRH_ROP: Raster Operation Code	*/
/*----------------------------------*/
#define	GRH_ROP_MASK			0x0f
#define	GRH_ROP_BLACKNESS		0x00
#define	GRH_ROP_NOTSRCERASE		0x01	// ~(S+D)
#define GRH_ROP_NOTSRCCOPY		0x03	// ~S
#define	GRH_ROP_SRCERASE		0x04	// S.~D
#define	GRH_ROP_DSTINVERT		0x05	// ~D
#define GRH_ROP_SRCINVERT		0x06	// S^D
#define GRH_ROP_SRCAND			0x08	// S.D
#define GRH_ROP_MERGEPAINT		0x0B	// ~S+D
#define	GRH_ROP_SRCCOPY			0x0C	// S
#define	GRH_ROP_SRCPAINT		0x0E	// S+D
#define	GRH_ROP_WHITENESS		0x0F

/*----------------------------------*/
/* GRH_BITBLT_CTL: BitBLT Control	*/
/*----------------------------------*/
#define	GRH_OP_FUNX_MASK		0x03
#define	GRH_OP_DO_ROP			0x00		
#define	GRH_OP_DO_TRANSP		0x01
#define	GRH_COLOR_EXPANSION		0x02

#define	GRH_OP_DAT_FLOW			0x1C
#define	GRH_MEM_2_HOST			0x00
#define	GRH_HOST_2_MEM			0x04
#define	GRH_MEM_2_MEM			0x08
#define	GRH_PAT_2_DISPLAY		0x0C
#define	GRH_SOLID_FILL			0x10
#define	GRH_DRAW_PIXEL			0x14

#define	GRH_BITBLT_DIR			0x60
#define	GRH_LEFT_TOP_RIGHT		0x00
#define	GRH_RIGHT_TOP_LEFT		0x20
#define	GRH_LEFT_BOT_RIGHT		0x40
#define	GRH_RIGHT_BOT_LEFT		0x60

/*----------------------------------*/
/* GPH_ENABLE_CTL: FIFO Control		*/
/*----------------------------------*/
#define GRH_SCALE_ENABLE	0x04
#define	GRH_FIFO_RESET	0x02
#define	GRH_ENABLE		0x01

/*----------------------------------*/
/* GPH_FIFO_STATUS: FIFO Status		*/
/*----------------------------------*/
#define	FIFO_EMPTY			0x01
#define	FIFO_FULL			0x02
#define	FIFO_WR_UNDER_THRES	0x04
#define	FIFO_RD_OVER_THRES	0x08

/*----------------------------------*/
/* GPH_INT_ENABLE: Interrupt Enable */
/*----------------------------------*/
#define	FIFO_EMPTY_INT_EN	0x01
#define	FIFO_FULL_INT_EN	0x02
#define	FIFO_WR_UNDER_THRES_INT_EN	0x04
#define	FIFO_RD_OVER_THRES_INT_EN	0x08
#define	GRAPHICS_DONE_INT_EN	0x10

/*----------------------------------*/
/* GPH_INT_FLAG: Interrupt Flag		*/
/*----------------------------------*/
#define	FIFO_EMPTY_INT		0x01
#define	FIFO_FULL_INT		0x02
#define	FIFO_WR_UNDER_THRES_INT	0x04
#define	FIFO_RD_OVER_THRES_INT	0x08
#define	GRAPHICS_DONE_INT	0x10

/*----------------------------------*/
/* GRH_LINE_CTL: Ling Control Flag	*/
/*----------------------------------*/
#define	LINE_COMPLETE	0x01

/*----------------------------------*/
/* GRH_RGB_CTL: RBG Control Flag	*/
/*----------------------------------*/
#define	B_G_R	0x0		// need advance check
#define	R_G_B	0x1

/*----------------------------------*/
/* DST_PIXEL_OFFSET					*/
/*----------------------------------*/
#define	NEG_PIXEL_OFFSET	0x0000
#define	POS_PIXEL_OFFSET	0x8000

/*----------------------------------*/
/* ROTATE_CTL						*/
/*----------------------------------*/
#define	ROTATE_MODE	0x01
#define	NEG_LINE_OFFSET	0x00
#define	POS_LINE_OFFSET	0x02

/*----------------------------------*/
/* JPEG_YUV_CTL						*/
/*----------------------------------*/
#define	HOST_CTL_MODE		0x01
#define	JPEG_MODE			0x02
#define	JPEG_UP_SCALING_MASK	0xFC

/*----------------------------------*/
/* JPEG_BUF_CTL						*/
/*----------------------------------*/
#define	JPEG_BUF_READY		0x01

//==================//
//	Error Codes		//
//==================//				
#define	GRH_NO_ERROR		0x00
#define	GRH_PARAS_ERROR		0x01
#define	GRH_PARAS_OUT_RANGE	0x02
#define	GRH_FUNX_ERROR		0x03
#define	GRH_HW_ERROR		0x04
#define	GRH_OUT_MEM			0x05

#define	INC_X	0x0
#define	INC_Y	0x1

#define	FRAME_STYLE		0x0
#define	SOLID_STYLE		0x1

#define	CONT_TYPE		0x0
#define	NOT_CONT_TYPE		0x1

//#define	DIR_H		0x0
#define	DIR_V		0x1

#define	DO_NOT_SKIP	0x0
#define	SKIP_POINT	0x1

#define	OTHER_EDGE	0x01
//#define	RIGHT_EDGE	0x02
#define	H_EDGE	0x03
#define	V_EDGE	0x04

#define	SLOPE_OVER_ONE	0x00
#define	SLOPE_OVER_MONE	0x01		
#define	SLOPE_ELSE		0x02		


u_short	SetGraphicsTransparent(u_short transpActive);
u_int	SetGraphicsBackground(u_int backColor);
u_int	SetGraphicsForeground(u_int foreColor);
u_short	A8L_CopyImageBuftoFIFO(u_short *dstAddr, u_short width, u_short height, 
			u_short bufferID, u_short srcx, u_short srcy);
u_short	A8L_CopyImageBuftoBuf(u_short width, u_short height, 
			u_short srcBufferID, u_short srcx, u_short srcy, 
			u_short tgtBufferID, u_short tgtx, u_short tgty, u_short ropCode);
u_short	A8L_SetBufferAttribute(u_short bufferID, u_short width, u_short height, 
			u_short lineOffset, u_short colorDepth, u_int bufferAddr); 
u_short	A8L_GetBufferAttribute(u_short bufferID, u_short *width, u_short *height, 
			u_short *lineOffset, u_short *colorDepth, u_int *bufferAddr);
u_short	A8L_SetBufferAttribute1(u_short bufferID, u_short width, u_short height, 
			u_short lineOffset, u_short colorDepth, u_int bufferAddr) ;
u_short	A8L_DrawRectangletoBuf(u_short width, u_short height, u_short bufferID, 
			u_short tgtx, u_short tgty, u_short style, u_short ropCode);

u_short	A8L_RotateImageBuftoBuf(u_short srcWidth, u_short srcHeight, 
			u_short srcBufferID, u_short srcx, u_short srcy, 
			u_short tgtBufferID, u_short tgtx, u_short tgty, 
			u_short ropCode, u_short rotateType);
u_short	A8L_CopyImageFIFOtoBuf(u_short *srcAddr, u_short width, u_short height, 
			u_short colorDepth, u_short bufferID, u_short tgtx, u_short tgty, 
			u_short ropCode);
u_short	ExpandMonoBitmapFIFOtoBuf(u_char *bmpAddr, u_short bmpWidth, u_short bmpHeight, 
			u_short bufferID, u_short tgtx, u_short tgty);
#endif /* graphics_h */
