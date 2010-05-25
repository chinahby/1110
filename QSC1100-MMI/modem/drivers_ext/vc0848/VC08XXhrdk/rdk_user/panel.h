
#ifndef _TRM_PANEL_H_
#define _TRM_PANEL_H_

#include "mmp_rdk.h"

#if HX_PANEL
#define LCD_HX8347_18BIT	1
#elif LPFD5420A
#define LCD_LPFD5420A		1
#elif S6D0170_PANEL
#define LCD_S6D0170		1
#elif LGDP4525_PANEL
#define LCD_LGDP4525    1
#else
#define	LCD_ILI9320_16BIT				1
#endif
 typedef struct LCD_TReg {
	MMD_U32 adr;			/* register address */
	MMD_U16 val;			/* register setting value */
	MMD_U16 wait;		/* ms */
} LCDReg, *PLCDReg;
 
typedef struct LCD_TRegGroup {
	MMD_U16 len;			/* register list length */
	LCDReg *regval;		/* registers array point */	
} LCDRegGroup, *PLCDTRegGroup;

typedef struct LCD_TPanelParm {
	LCDRegGroup			poweron;
	LCDRegGroup			poweroff;
	LCDRegGroup			sleep;
	LCDRegGroup			wakeup;
} LCDParm, *PLCDParm;
void Lcd_Panel_SetRSFunc(unsigned long isBypass);
void Lcd_MPanel_SetList(PLCDTRegGroup list,unsigned long isBypass);
void Lcd_SPanel_SetList(PLCDTRegGroup list,unsigned long isBypass);
void  MLcdSetReg(int adr,int val);
void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf );
void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color );
void Lcd_Rotate(unsigned char degree);

 extern LCDParm MLcdParm;
 extern LCDParm SLcdParm;
 
#endif


