/**
 * @file A8_panel.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#ifndef AIT_PANEL_H 
#define AIT_PANEL_H

//-----------------------------------------------------------------------------//
// The following definition is an option to disable sub-LCD related function
#define A8_DISABLE_SUB_LCD_FUNCTION
//-----------------------------------------------------------------------------//

#define NEED_SET_ADDRESS 0x10
#define NO_NEED_SET_ADDRESS 0x00

#define TYPE_80    0x40
#define TYPE_68    0x00

#define POLARITY_ENABLE 0x20
#define POLARITY_DISABLE 0x00

#define PHASE_ENABLE 0x10
#define PHASE_DISABLE 0x00

#define BURST_MODE 0x08
#define NON_BURST_MODE 0x00

#define DATA_BUS_8   0x00
#define DATA_BUS_16   0x01
#define DATA_BUS_18   0x02
#define DATA_BUS_12   0x03


#define RGB56516BIT   0x00
#define RGB44416BIT   0x01
#define RGB66616BIT   0x02
//#define RGB56516BIT   0x03
#define RGB33208BIT   0x04
#define RGB44408BIT   0x05
#define RGB66608BIT   0x06
#define RGB56508BIT   0x07

#define PANEL2_RGB 0x00
#define PANEL2_BGR 0x80

#define DUAL_PANEL_PREVIEW 0x20
#define NO_DUAL_PANEL_PREVIEW 0x00

#define LCD1LCD2OFF 0x00
#define LCD1ACTIVE   0x08
#define LCD2ACTIVE   0x10

#define PANEL1_RGB 0x00
#define PANEL1_BGR 0x04
	

//======================================================================//
typedef struct LCDInfo
{
   u_short Width, Height;
   u_short BusWidth;      /* 8bit or 16bit */
   u_short BusType;       /* 80 or 68 type */
   u_short Burst;         /* Burst mode or Non burst mode */
} LCDInfoDT;


typedef struct{
	unsigned char PIXEL_CLOCK_DIV;//, = 0x03;
	unsigned short ADDRESS_INDEX;//, = 0x0021;
	unsigned short ADDRESS_DATA;// = 0x0000; //080401 0x0002
	unsigned short DATA_INDEX;// = 0x0022;

	LCDInfoDT	LCD_BusCtl;
	
	POINT_AIT PixelStart;// = 0;

	POINT_AIT PixelOffset;//= 1;

}sPanel_Ctrl_Attrib;		
		

s_short A8L_SendLCDIndexCommand( u_short index, u_short command );
s_short A8L_SendLCDIndexCommand8B( u_short index, u_short command );
void A8L_SendLCDCommand(u_short Command);
void A8L_SendLCDIndex(u_short Index);

u_char	A800_UsingLCD1(void);
u_char	A800_HostInitLCD(void);
u_char	A800_HostInitLCDCommon(void);
#endif
