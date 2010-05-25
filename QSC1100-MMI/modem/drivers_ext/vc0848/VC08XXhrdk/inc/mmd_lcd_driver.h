/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_lcd_driver.h
 * [Description]          : Image mode header file
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-03-13
 * [Platform]             : Platform independent
 * [Note]                 : None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-13  zhangxf    Created
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/

#ifndef _MMD_LCD_H_
#define _MMD_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * INCLUDE FILE                                              
 */
 #include "mmp_rdk.h"

#if(CHIP_ID==ID_VC0824)
#define 	MMD_V5_1TSRAM_BASE						0x40E00000//0x40C00000
#define 	MMD_V5_1TSRAM_TOTALSIZE					(0x200000-TTB_SIZE-BIU_COMMAND_SIZE)//0x100000
#else
// 878
#define 	MMD_V5_1TSRAM_BASE						0x40340000
#define 	MMD_V5_1TSRAM_TOTALSIZE					(0xB0000-TTB_SIZE-BIU_COMMAND_SIZE)
#endif

typedef	enum enum_lcdc_overlay_mode
{
	OVL_TSP = 0,
	OVL_AND = 1,
	OVL_OR  = 2,
	OVL_INV = 3
} ovl_mod;

typedef struct
{
	MMD_U16 layer;
	MMD_U16 fmt;
	MMD_U16 src_w;
	MMD_U16 src_h;
	MMD_U16 fo_x;
	MMD_U16 fo_y;
	MMD_U16 so_x;
	MMD_U16 so_y;
	MMD_U16 so_w;
	MMD_U16 so_h;
}blayer_params;

typedef  struct
{
 	MMD_U16 layer;
 	MMD_U16 sx;
 	MMD_U16 sy;
 	MMD_U16 width;
 	MMD_U16 height;
 	MMD_U16 up_flg;
 	MMD_U08 *pdata;
}render_params;

typedef  struct
{
 	MMD_U32 addr;	
 	MMD_U32 rotate_mode;
 	MMD_U16 fmt;
 	MMD_U16 width;
 	MMD_U16 height;
	MMD_U16 dpt_x;
	MMD_U16 dpt_y;
	MMD_U16 panel_width;
}MMD_gbuf_info;

typedef struct
{
    MMD_U32 color;
    MMD_U16 x;
    MMD_U16 y;
    MMD_U16 w;
    MMD_U16 h;
}bg_params;

typedef enum _MMD_LCD_LAYER_enum
{
	MMD_LCD_ALAYER = 1,					//528 parameters
	MMD_LCD_B0LAYER= 2,
	MMD_LCD_B1LAYER=4,
	MMD_LCD_ALLBLAYER=6,
	MMD_LCD_A1LAYER=8,
	MMD_LCD_ALLLAYER=0xf,
	MMD_LCD_LAYER_ERROR = 0x7fffffff
}MMD_LCD_LAYER;

typedef enum MMD_LCD_BFORMAT{
	MMD_LCD_BLAYER_RGB1=0,
	MMD_LCD_BLAYER_RGB2,
	MMD_LCD_BLAYER_RGB4,
	MMD_LCD_BLAYER_RGB8,
	MMD_LCD_BLAYER_RGB444,
	MMD_LCD_BLAYER_RGB555A,
	MMD_LCD_BLAYER_RGB565,
	MMD_LCD_BLAYER_RGB666,
	MMD_LCD_BLAYER_RGB888,
	MMD_LCD_BLAYER_RGB888A,
	MMD_LCD_BFORMAT_ERROR = 0x7fffffff
}MMD_LCD_BFORMAT;

#define	MMD_HAPI_LCDPANEL_MAIN	0
#define 	MMD_HAPI_LCDPANEL_SUB	1

typedef	enum enum_vm_flag_en
{
	VM_ENABLE  = 0,
	VM_DISABLE = 1
} vm_flg_en;

typedef	enum enum_overlay_flag
{
	NO_OVERLAY  = 0,
	SET_OVERLAY = 1
} overlay_flag;

typedef struct{	
	MMD_U16 cx;
	MMD_U16 cy;
} tsize, *ptsize;

typedef struct{	
	MMD_U16 x;
	MMD_U16 y;
} tpoint, *ptpoint;


typedef enum _MMD_ROTATEMODE{
	MMD_ROTATE_0=0,
	MMD_ROTATE_90
}MMD_ROTATEMODE;


#define	MMD_PLAYER_B_SIZE		8192
#define	MMD_RECORD_B_SIZE	7168
#define	MMD_USB_B_SIZE	8192

/*! LCDC registers */
#define MMD_REG_LCDC_BASE							0x60080000
#define MMD_REG_LCDC_CTRL							(MMD_REG_LCDC_BASE+0x0000)
#define MMD_REG_LCDC_RESET						(MMD_REG_LCDC_BASE+0x0004)
#define MMD_REG_LCDC_UPDATE						(MMD_REG_LCDC_BASE+0x0008)
#define MMD_REG_LCDC_VBBA0						(MMD_REG_LCDC_BASE+0x000c)
#define MMD_REG_LCDC_VBBA1						(MMD_REG_LCDC_BASE+0x0010)
#define MMD_REG_LCDC_VBBA2						(MMD_REG_LCDC_BASE+0x0014)
#define MMD_REG_LCDC_VBBA3						(MMD_REG_LCDC_BASE+0x0018)
#define MMD_REG_LCDC_GBBA0						(MMD_REG_LCDC_BASE+0x001c)
#define MMD_REG_LCDC_GBBA1						(MMD_REG_LCDC_BASE+0x0020)
#define MMD_REG_LCDC_A0_LCD_POS					(MMD_REG_LCDC_BASE+0x0024)
#define MMD_REG_LCDC_A0_SIZE_H					(MMD_REG_LCDC_BASE+0x0028)
#define MMD_REG_LCDC_A0_SIZE_W					(MMD_REG_LCDC_BASE+0x0028+2)

#define MMD_REG_LCDC_A0_VFRAME_POS				(MMD_REG_LCDC_BASE+0x002c)
#define MMD_REG_LCDC_A1_LCD_POS					(MMD_REG_LCDC_BASE+0x0034)
#define MMD_REG_LCDC_A1_SIZE_H					(MMD_REG_LCDC_BASE+0x0038)
#define MMD_REG_LCDC_A1_SIZE_W					(MMD_REG_LCDC_BASE+0x0038+2)

#define MMD_REG_LCDC_B0_LCD_POS					(MMD_REG_LCDC_BASE+0x003c)
#define MMD_REG_LCDC_B0_SIZE						(MMD_REG_LCDC_BASE+0x0040)
#define MMD_REG_LCDC_B0_UPD_POS					(MMD_REG_LCDC_BASE+0x0044)
#define MMD_REG_LCDC_B0_UPD_SIZE					(MMD_REG_LCDC_BASE+0x0048)
#define MMD_REG_LCDC_B0_GBUF0_POS				(MMD_REG_LCDC_BASE+0x004c)
#define MMD_REG_LCDC_B1_LCD_POS					(MMD_REG_LCDC_BASE+0x0050)
#define MMD_REG_LCDC_B1_SIZE						(MMD_REG_LCDC_BASE+0x0054)
#define MMD_REG_LCDC_B1_UPD_POS					(MMD_REG_LCDC_BASE+0x0058)
#define MMD_REG_LCDC_B1_UPD_SIZE					(MMD_REG_LCDC_BASE+0x005c)
#define MMD_REG_LCDC_B1_GBUF1_POS				(MMD_REG_LCDC_BASE+0x0060)
#define MMD_REG_LCDC_GBUF_WIDTH					(MMD_REG_LCDC_BASE+0x0064)
#define MMD_REG_LCDC_BG_UPD_POS					(MMD_REG_LCDC_BASE+0x0068)
#define MMD_REG_LCDC_BG_UPD_SIZE					(MMD_REG_LCDC_BASE+0x006c)
#define MMD_REG_LCDC_BGD							(MMD_REG_LCDC_BASE+0x0070)
#define MMD_REG_LCDC_PALETTE						(MMD_REG_LCDC_BASE+0x0074) 
#define MMD_REG_LCDC_GLUT_R0123					(MMD_REG_LCDC_BASE+0x0078)
#define MMD_REG_LCDC_GLUT_R4567					(MMD_REG_LCDC_BASE+0x007c)
#define MMD_REG_LCDC_GLUT_R891011				(MMD_REG_LCDC_BASE+0x0080)
#define MMD_REG_LCDC_GLUT_R12131415				(MMD_REG_LCDC_BASE+0x0084)
#define MMD_REG_LCDC_GLUT_G0123					(MMD_REG_LCDC_BASE+0x0088)
#define MMD_REG_LCDC_GLUT_G4567					(MMD_REG_LCDC_BASE+0x008c)
#define MMD_REG_LCDC_GLUT_G891011				(MMD_REG_LCDC_BASE+0x0090)
#define MMD_REG_LCDC_GLUT_G12131415				(MMD_REG_LCDC_BASE+0x0094)
#define MMD_REG_LCDC_GLUT_B0123					(MMD_REG_LCDC_BASE+0x0098)
#define MMD_REG_LCDC_GLUT_B4567					(MMD_REG_LCDC_BASE+0x009c)
#define MMD_REG_LCDC_GLUT_B891011				(MMD_REG_LCDC_BASE+0x00a0)
#define MMD_REG_LCDC_GLUT_B12131415				(MMD_REG_LCDC_BASE+0x00a4)
#define MMD_REG_LCDC_GLUT_R16G16B16				(MMD_REG_LCDC_BASE+0x00a8)
#define MMD_REG_LCDC_DEDLY						(MMD_REG_LCDC_BASE+0x00ac)
#define MMD_REG_LCDC_OKC_B0						(MMD_REG_LCDC_BASE+0x00b0)
#define MMD_REG_LCDC_OKC_B1						(MMD_REG_LCDC_BASE+0x00b4)
#define MMD_REG_LCDC_BLENDRATIO_B				(MMD_REG_LCDC_BASE+0x00b8)

/*
 * PROTOTYPE DECLARATION                                     
 * 
 */
MMD_U32 MMD_LCD_Init(void);
MMD_U32 MMD_LCD_Quit(void);
void MMD_LCD_Init_Normal(void);
//void MMD_LCD_Init_Bypass(void);
void MMD_LCD_InitPanel_Pin_Config(void);
MMD_U32 MMD_Set_LCDIFParm(MMD_U32 bPanel);
MMD_U32 MMD_LCD_PanelSwitch(pnl_idx idx );
MMD_U32 MMD_LCD_UpdateBgWin ( bg_params* bg_property );
MMD_U32 MMD_LCD_SetGwin(blayer_params * blayer_property );
void MMD_LCD_BLY_Release(void);
void MMD_LCD_BLY_Cast(void);
void MMD_LCD_Mallco(int layer,int x,int y,int w,int h);
void MMD_LCD_DrawBg(MMD_U16 layer,MMD_U16 sx,MMD_U16 sy,MMD_U16 width,MMD_U16 height,MMD_U32 color);
MMD_U32 MMD_LCD_SetOverlay ( vm_flg_en flag, MMD_U08 layer, ovl_mod mode, MMD_U32 color );
MMD_U32 MMD_LCDRotate(UINT8 rotateFlag);
void MMD_LCD_ReGwin(UINT32 newX,UINT32 newY);
MMD_U32 CMMB_MMD_LCDRotate(UINT8 rotateFlag);
MMD_U32 MMD_LCD_Refresh(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,/*MMD_U32 overlayFlag,*/MMD_U08 *pdata,MMD_U32 updateFlag);
MMD_U32 MMD_LCD_Refresh_Part_Update(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U32 lcd_update_part,MMD_U32 lastLineNum,MMD_U32 updateFlag,MMD_U08 *pdata);
MMD_U32 MMD_LCD_Refresh_Update(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U32 updateFlag,MMD_U08 *pdata);

void MMD_LCD_Refresh_In_Timer(void);
void MMD_LCD_SetOverlay_Flag(MMD_U32 set);
MMD_U32 MMD_LCD_GetOverlay_Flag(void);
#ifdef __cplusplus                              
}                                               
#endif   

#endif /*_MMD_LCD_H_*/

