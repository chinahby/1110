/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_lcd_driver.c
 * [Description]          : VC0820 LCD related API.
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
 
/*
 * INCLUDE FILE                                              
 */

#include "mmp_rdk.h"
#include "VIM_COMMON_Typedef.h"
#include "VIM_Drivers.h"
#include	"..\..\apps\me\decoder\mp4v\decoder_mp4v.h"
#include "AEEStdlib.h"
#include "vc0848.h"
#include "err.h"



#define MMD_LCD_DEBUG_INFO
#define MMD_LCD_DEBUG_ERR

#ifdef MMD_LCD_DEBUG_ERR
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
    #define PRINTFS(WriteVal)   MMD_PRINTFS(WriteVal) //success
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTF(WriteVal)
    #define PRINTFN(WriteVal) 
    #define PRINTFE(WriteVal)
    #define PRINTFS(WriteVal)
#endif

// system mode initial status, 0: uninit; 1: init
static MMD_U32 g_LCD_status=0;

MMD_U16 old_x;
MMD_U16 old_y;
MMD_U16 old_partCount;
MMD_U32 setOverlay=0;
TIMER_STATE	emMMDTimerState;
UPDATE_STATE emMMDUpdateState = UPDATED;
BLAYER_PARAMS_FILL	strMMDFillBlParams;


extern int rotateStauts;
static MMD_U32 MMD_GetLCDWidth(void)
{
    if (rotateStauts)
    {
        return MMD_LCD_HEIGHT;

    }
    return  MMD_LCD_WIDTH;
}

static MMD_U32 MMD_GetLCDHeight(void)
{
    if (rotateStauts)
    {
        return MMD_LCD_WIDTH;

    }
    return MMD_LCD_HEIGHT;
}


void MMD_LCD_Init_Normal(void)
{
	// LCD panel pin config
	Vm_SetReg(0x6017010C, 0);
	Vm_SetReg(0x60170114,Vm_GetReg(0x60170114)|((0x01<<19)));  /* set lcd_rst up-pull enable */ 
	MMD_USER_SetChipMode(MMD_CHIP_MODE_NORMAL);
	MMD_USER_ResetLCD_Normal();
	LCDIF_SetLCDIFParm(MMD_HAPI_LCDPANEL_MAIN);
	Lcd_MPanel_SetList(&(MLcdParm.poweron),0);	
	MMD_USER_PowerOnLogo_Normal();
}

void MMD_LCD_InitPanel_Pin_Config()
{
	Vm_SetReg(0x6017010C, 0);
}
#if 0
void MMD_LCD_Init_Bypass(void)
{
	// LCD panel pin config
	Vm_SetReg(0x6017010C, 0);
	//MMD_USER_SetChipMode(MMD_CHIP_MODE_BYPASS);
	//MMD_USER_ResetLCD_Bypass();
	//Lcd_MPanel_SetList(&(MLcdParm.poweron),1);		
	//MMD_USER_PowerOnLogo_Bypass();
}
#endif
void MMD_LCD_Mallco(int layer,int x,int y,int w,int h)
{
	blayer_params b_property;


	b_property.layer=layer;
	b_property.fmt=MMD_LCD_BLAYER_RGB565;
	b_property.src_w=w;
	b_property.src_h=h;
	b_property.fo_x=x;
	b_property.fo_y=y;	
	b_property.so_x=x;
	b_property.so_y=y;
	b_property.so_w=w;
	b_property.so_h=h;
	MMD_LCD_SetGwin( &b_property);
}
void MMD_LCD_BLY_Release(void)
{
    FARF(MSG,("MMD_LCD_BLY_Release"));
	VIM_HAPI_LCDCBuffer_Release();
	old_x = 0; 
	old_y = 0; 
	old_partCount = 0;
}

void MMD_LCD_BLY_Cast(void)
{
	old_x = 0; 
	old_y = 0; 
	old_partCount = 0;
	VIM_HAPI_LCDCBuffer_Cast();
}

void MMD_LCD_UPDate(VIM_HAPI_LAYER BLayer)
{
	VIM_DISP_Update((VIM_DISP_LAYER)BLayer);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_Init
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_Init(void)
{
    	PRINTFN(("[LCD] MMD_LCD_Init\r\n"));
    
    	g_LCD_status =1;
    	g_system_mode|=(MODE_LCD);
	
    	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_Quit
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_Quit(void)
{
    	MMD_U32 ret = MMD_RET_NOERR;

    	PRINTFN(("[LCD] MMD_LCD_Quit\r\n"));

    	// TODO: free LCD memory

   	// set mode status flag
    	g_LCD_status =0;
    	// clear mode flag
    	g_system_mode&=~(MODE_LCD);

    	return ret;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_PanelSwitch
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_PanelSwitch(pnl_idx idx )
{
    	MMD_U32 ret;

	if(!g_LCD_status)
	{
		PRINTFE(("LCD mode error\r\n"));
		return MMD_RET_MODEERR;
	}
	ret = VIM_HAPI_SelectLCDPanel(idx-1);
	if (ret != MMD_RET_NOERR)
    	{
        	PRINTFE(("MMD_LCD_PanelSwitch error\r\n"));
    	}


    	return ret;
}



void MMD_LCD_ReGwin(UINT32 newX,UINT32 newY)
{
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,newX);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,newY);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_SetGwin
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_SetGwin(blayer_params * blayer_property )
{
	MMD_U32 ret;
	VIM_HAPI_LCDWORKMODE lcd_layer;	
	if(!g_LCD_status)
	{
	    PRINTFE(("LCD mode error\r\n"));
	    return MMD_RET_MODEERR;
	}

	PRINTFN(("layer=[%d]\n", blayer_property->layer));
	PRINTFN(("fmt=[%d]\n", blayer_property->fmt));
	PRINTFN(("src_w=[%d]\n", blayer_property->src_w));
	PRINTFN(("src_h=[%d]\n", blayer_property->src_h));
	PRINTFN(("fo_x=[%d]\n", blayer_property->fo_x));
	PRINTFN(("fo_y=[%d]\n", blayer_property->fo_y));	
	PRINTFN(("so_x=[%d]\n", blayer_property->so_x));
	PRINTFN(("so_y=[%d]\n", blayer_property->so_y));
	PRINTFN(("so_w=[%d]\n", blayer_property->so_w));
	PRINTFN(("so_h=[%d]\n", blayer_property->so_h));

	//if(blayer_property->src_h == 320)
	//	blayer_property->src_h =160;
	ret =VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);
	ret = VIM_HAPI_SetLCDSize(blayer_property->layer,blayer_property->so_x,blayer_property->so_y,blayer_property->src_w,blayer_property->src_h);
	if(ret != MMD_RET_NOERR)
	{
		PRINTFE(("WriteParam error\r\n"));
		return ret;
	}
    //zhangxiang mod start
    	if(blayer_property->layer == MMD_LCD_B0LAYER || blayer_property->layer == MMD_LCD_B1LAYER || blayer_property->layer == MMD_LCD_ALLBLAYER)
		lcd_layer = VIM_HAPI_LCDMODE_BLONLY;
	else if(blayer_property->layer == MMD_LCD_A1LAYER|| blayer_property->layer == MMD_LCD_ALAYER )
		lcd_layer = VIM_HAPI_LCDMODE_AFIRST;
	else
		lcd_layer = VIM_HAPI_LCDMODE_OVERLAY;
	ret =VIM_HAPI_SetLCDWorkMode(lcd_layer,0);
    //ret =VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_OVERLAY,0xffff);   //zhangxiang mod
	ret =VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);
	 //zhangxiang mod end
    	return ret;

}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_UpdateGwin
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_UpdateGwin (MMD_U08 layer,  MMD_U16 so_x, MMD_U16 so_y, MMD_U16 so_w, MMD_U16 so_h )
{
	MMD_U32 ret=0;

	if(!g_LCD_status)
	{
		PRINTFE(("LCD mode error\r\n"));
		return MMD_RET_MODEERR;
	}
	if(emMMDTimerState==NO_TIMER_START)
	{
	
		VIM_HAPI_UpdateLCD(layer,so_x,so_y,so_w,so_h);
		
		if (ret != MMD_RET_NOERR)
		{
			PRINTFE(("VIM_HAPI_UpdateLCD error\r\n"));
		}
	}
	else
	{
		MMD_MemSet(&strMMDFillBlParams,0,sizeof(BLAYER_PARAMS_UPDATE));
		strMMDFillBlParams.layer = layer;
		strMMDFillBlParams.startpointX = so_x;
		strMMDFillBlParams.startpointY = so_y;
		strMMDFillBlParams.screenW = so_w;
		strMMDFillBlParams.screenH = so_h;
		emMMDUpdateState = NEED_UPDATE;
		ret = 0;
	}
	

    return ret;

}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_UpdateBgWin
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
//The background color is set in direct color format (RGB 8:8:8 no
//alpha bit). Truncating the least significant bits can get the other format.
MMD_U32 MMD_LCD_UpdateBgWin ( bg_params* bg_property )
{
	MMD_U32 ret;

	if(!g_LCD_status)
	{
		PRINTFE(("LCD mode error\r\n"));
		return MMD_RET_MODEERR;
	}
	ret = VIM_HAPI_UpdateBG(bg_property->color,bg_property->x,bg_property->y,bg_property->w,bg_property->h);
	if (ret != MMD_RET_NOERR)
	{
		PRINTFE(("VIM_HAPI_UpdateBG error\r\n"));
	}
	
	return ret;

}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_RenderGBuf
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_RenderGbuf(MMD_U16 layer,MMD_U16 sx,MMD_U16 sy,MMD_U16 width,MMD_U16 height,MMD_U08 *pdata)
{

	MMD_U32 ret;

	if(!g_LCD_status)
	{
		PRINTFE(("LCD mode error\r\n"));
		return MMD_RET_MODEERR;
	}	
	if(emMMDTimerState==NO_TIMER_START)
	{
		ret = VIM_HAPI_DrawLCDRctngl(layer,sx,sy,width,height,pdata);
		if(ret)
			PRINTFE(("VIM_HAPI_DrawLCDRctngl error errno(%d)!",ret));
		}
	else
	{
		MMD_MemSet(&strMMDFillBlParams,0,sizeof(BLAYER_PARAMS_FILL));
		strMMDFillBlParams.layer = layer;
		strMMDFillBlParams.startpointX = sx;
		strMMDFillBlParams.startpointY = sy;
		strMMDFillBlParams.screenW = width;
		strMMDFillBlParams.screenH = height;
		strMMDFillBlParams.pBuff = pdata;
		emMMDUpdateState = NEED_UPDATE;
		ret= 0;
	}
	
    	return ret;

}

#define	LCD_LINE_NUM(x,y)	x/(y*2)
MMD_U32 lcd_line_num=10;

MMD_U32 MMD_LCD_Refresh_Part_Update(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U32 lcd_update_part,MMD_U32 lastLineNum,MMD_U32 updateFlag,MMD_U08 *pdata)
{
	MMD_U32 iFor=0,ret=0,top=0,bottom=0;
	
	
	top = sy;
	MMD_LCD_ReGwin(0,sy);
	MMD_USER_WaitMICRS(20);//wlh 时	
	for(iFor=0;iFor<lcd_update_part;iFor++)
	{
		bottom = lcd_line_num*(iFor+1);
		ret = VIM_HAPI_DrawLCDRctngl(MMD_LCD_B0LAYER,sx, top,width,bottom+sy-top, pdata);		
		if(ret)
			return ret;
		if(!updateFlag)
		{
			ret = VIM_HAPI_UpdateLCD(MMD_LCD_B0LAYER,sx, top,width,bottom+sy-top);
			if(ret)
				return ret;
		}
		
		MMD_LCD_ReGwin(0,bottom+sy);
		top = bottom+sy;
	}
	if(lastLineNum)
	{
		ret = VIM_HAPI_DrawLCDRctngl(MMD_LCD_B0LAYER,sx, top,width,lastLineNum, pdata);		
		if(ret)
			return ret;
		if(!updateFlag)
		{
			ret = VIM_HAPI_UpdateLCD(MMD_LCD_B0LAYER,sx, top,width,lastLineNum);
			if(ret)
				return ret;
		}
	}
	
	return 0;
}

MMD_U32 MMD_LCD_Refresh_Update(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U32 updateFlag,MMD_U08 *pdata)
{
	MMD_U32 ret=0;
	ret = VIM_HAPI_DrawLCDRctngl(layer,sx,sy,width,height,pdata);
	if(!updateFlag)
		ret = VIM_HAPI_UpdateLCD(layer,sx,sy,width,height);
	return ret;
}


void MMD_LCD_Refresh_In_Timer()
{
	if(!setOverlay)
	{
		MMD_LCD_Refresh_Part_Update(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,strMMDFillBlParams.startpointY,strMMDFillBlParams.screenW,strMMDFillBlParams.screenH,strMMDFillBlParams.lcd_update_part,strMMDFillBlParams.lastLineNum,strMMDFillBlParams.updateFlag,strMMDFillBlParams.pBuff);
	}
	else
	{
		VIM_HAPI_DrawLCDRctngl(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,strMMDFillBlParams.startpointY,strMMDFillBlParams.screenW,strMMDFillBlParams.screenH,strMMDFillBlParams.pBuff);
		if(!strMMDFillBlParams.updateFlag)
			VIM_HAPI_UpdateLCD(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,strMMDFillBlParams.startpointY,strMMDFillBlParams.screenW,strMMDFillBlParams.screenH);
	
	}
}
MMD_U32 timerRun=0;

extern MMD_U08 play_timer_flag;
extern MMD_U08 lcd_refresh_flag;

MMD_U32 MMD_LCD_Refresh(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,/*MMD_U32 overlayFlag,*/MMD_U08 *pdata,MMD_U32 updateFlag)
{

	MMD_U32 getBuff=0,ret=0,lcd_update_part=0,lastLineNum=0;
	blayer_params blayer_property;
    while(play_timer_flag==1)
    {
        rex_sleep(1);
    }
       
    lcd_refresh_flag=1;
	getBuff = VIM_GetBlayerBuff(layer);
    sx=0;
    width=176;
	if(!getBuff)
	{
        //ERR("VIM_GetBlayerBuff =%x ",getBuff,0,0);
		if(!setOverlay)
		{
			blayer_property.fo_y = sy;
			blayer_property.so_y= sy;
			switch(MMD_SYS_Mode_Get())
			{
				case SCENE_MEDIAPLAY:
					lcd_line_num = LCD_LINE_NUM(MMD_PLAYER_B_SIZE,MMD_GetLCDWidth()) ;
					break;
				case SCENE_USB:
					lcd_line_num = LCD_LINE_NUM(MMD_USB_B_SIZE,MMD_GetLCDWidth()) ;
					break;
				case SCENE_REC_AMR:
					lcd_line_num = LCD_LINE_NUM(MMD_RECORD_B_SIZE,MMD_GetLCDWidth()) ;
					break;
				case SCENE_CMMB:
					lcd_line_num = MMD_GetLCDHeight();
					setOverlay = 1;
					blayer_property.fo_y = 0;
					blayer_property.so_y= 0;
					break;
				//case SCENE_RV:
				case SCENE_RV_OVERLAY:
					lcd_line_num = MMD_GetLCDHeight();
					setOverlay = 1;
				default:
					break;
			}
			
			blayer_property.src_h = lcd_line_num;
		}
		else
		{
			blayer_property.fo_y = sy;
			blayer_property.so_y= sy;
			blayer_property.src_h=height;
		}
		
		blayer_property.so_h= blayer_property.src_h;
		blayer_property.fmt= MMD_LCD_BLAYER_RGB565;
		blayer_property.src_w=MMD_GetLCDWidth();//width;
		blayer_property.so_w= MMD_GetLCDWidth();//width;
		blayer_property.fo_x = 0;
		blayer_property.so_x= 0;
		blayer_property.layer= layer;
		MMD_LCD_SetGwin(&blayer_property);
		//MMD_USER_WaitMICRS(10);
	}
	//printf("height(%d) part(%d) lastLineNum(%d) linenum(%d) sx(%d) sy(%d) width(%d) \n",height,lcd_update_part,lastLineNum,lcd_line_num,sx,sy,width);
	if(!setOverlay)
	{
		MMD_LCD_ReGwin(0,sy);
		MMD_USER_WaitMICRS(20);
		lcd_update_part = height /lcd_line_num ;
		lastLineNum = height - lcd_line_num *lcd_update_part;
		if(emMMDTimerState==NO_TIMER_START)
		{
			MMD_LCD_Refresh_Part_Update(layer,sx, sy, width, height,lcd_update_part,lastLineNum,0,pdata);
            MMD_USER_WaitMICRS(5);
		}
		else
		{
			MMD_MemSet(&strMMDFillBlParams,0,sizeof(BLAYER_PARAMS_FILL));
			strMMDFillBlParams.layer = layer;
			strMMDFillBlParams.startpointX = sx;
			strMMDFillBlParams.startpointY = sy;
			strMMDFillBlParams.screenW = width;
			strMMDFillBlParams.screenH = height;
			strMMDFillBlParams.pBuff = pdata;
			strMMDFillBlParams.lcd_update_part = lcd_update_part;
			strMMDFillBlParams.lastLineNum = lastLineNum;
			strMMDFillBlParams.updateFlag = 0;
			emMMDUpdateState = NEED_UPDATE;
		}

	}
	else
	{
		if(emMMDTimerState==NO_TIMER_START)
		{
			MMD_LCD_ReGwin(0,sy);
			MMD_USER_WaitMICRS(20);
			//lcd_line_num = height;
			MMD_LCD_Refresh_Update(layer,sx, sy, width, height,updateFlag,pdata);
		}
		else
		{
			lcd_line_num = height;
			MMD_MemSet(&strMMDFillBlParams,0,sizeof(BLAYER_PARAMS_FILL));
			strMMDFillBlParams.layer = layer;
			strMMDFillBlParams.startpointX = sx;
			strMMDFillBlParams.startpointY = sy;
			strMMDFillBlParams.screenW = width;
			strMMDFillBlParams.screenH = height;
			strMMDFillBlParams.pBuff = pdata;
			strMMDFillBlParams.lcd_update_part = 1;
			strMMDFillBlParams.lastLineNum = 0;
			strMMDFillBlParams.updateFlag = updateFlag;
			emMMDUpdateState = NEED_UPDATE;
		}
	}
	MMD_USER_WaitMICRS(5);
	MMD_LCD_ReGwin(0,sy);
    lcd_refresh_flag=0;
    if(emMMDTimerState==TIMER_STARTED)
    {
        while(emMMDUpdateState == NEED_UPDATE)
        {
           rex_sleep(1);
        }
    }
	return ret;
}

MMD_U32 MMD_LCD_Refresh_FM(MMD_U16 layer,MMD_U16 sx,MMD_U16 sy,MMD_U16 width,MMD_U16 height,MMD_U08 *pdata)
{
    MMD_U32 ret;
    MMD_LCD_BLY_Release();
    MMD_Set_LCDIFParm(MMD_HAPI_LCDPANEL_MAIN);
    VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_BLONLY,0xffff);
    VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);
    ret = VIM_HAPI_SetLCDSize(layer,sx,sy,width,height);
    //ret = VIM_HAPI_SetLCDSize(layer,0,0,176,220);
    if(ret)
            return ret;
    MMD_LCD_RenderGbuf(MMD_LCD_B0LAYER, sx,sy,width,height, pdata);
    MMD_LCD_UpdateGwin(MMD_LCD_B0LAYER, sx,sy,width,height);
    MMD_USER_WaitMICRS(300);
}

MMD_U32 MMD_LCD_Refresh_ZX(MMD_U16 layer,MMD_U16 sx,MMD_U16 sy,MMD_U16 width,MMD_U16 height,MMD_U08 *pdata)
{
    MMD_U32 ret;
    
    //ret = VIM_HAPI_SetLCDSize(layer, 0, 0, 176, 28);
   // if(ret)
   //     return ret;
    MMD_LCD_RenderGbuf(MMD_LCD_B0LAYER, 0, 0, 176, 220, pdata);
    MMD_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0, 0, 176, 28);
    MMD_USER_WaitMICRS(200);
    MMD_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0, 160, 176, 60);
 #if 0
    MMD_LCD_RenderGbuf(MMD_LCD_B0LAYER, 0, 0, 176, 28, pdata);
    MMD_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0, 0, 176, 28);
    MMD_USER_WaitMICRS(200);
    
   // ret = VIM_HAPI_SetLCDSize(layer, 0, 160, 176, 60);
   // if(ret)
   //     return ret;
    MMD_LCD_RenderGbuf(MMD_LCD_B0LAYER, 0, 160, 176, 60, pdata);
    MMD_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0, 160, 176, 60);
    MMD_USER_WaitMICRS(200);
    
   // MMD_LCD_BLY_Release();
  //  ret = VIM_HAPI_SetLCDSize(layer,0, 72, 176, 88);
   //if(ret)
   //     return ret;
    MMD_LCD_RenderGbuf(MMD_LCD_B0LAYER, 0, 28, 176, 132, pdata);
    MMD_USER_WaitMICRS(300);
#endif
    return ret;
}

void MMD_LCD_Refresh_In_Timer_Video(void)
{
  MMD_U16 count=0,Fori=0,lastline=0,starty=0;
 if(emMMDUpdateState == NEED_UPDATE)
 {//需要刷屏

      count=strMMDFillBlParams.screenH /lcd_line_num;
      lastline=strMMDFillBlParams.screenH-lcd_line_num*count;
     
      for(Fori=0;Fori<count;Fori++)
      {
           starty=strMMDFillBlParams.startpointY+lcd_line_num*Fori;
           MMD_LCD_ReGwin(0,starty);
           VIM_HAPI_DrawLCDRctngl(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,starty,strMMDFillBlParams.screenW,lcd_line_num,strMMDFillBlParams.pBuff);
           VIM_HAPI_UpdateLCD(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,starty,strMMDFillBlParams.screenW,lcd_line_num);
           MMD_USER_WaitMICRS(100);
       
      }
      MMD_LCD_ReGwin(0,starty+lcd_line_num);
      if(lastline)
      {
           VIM_HAPI_DrawLCDRctngl(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,starty+lcd_line_num,strMMDFillBlParams.screenW,lastline,strMMDFillBlParams.pBuff);
           VIM_HAPI_UpdateLCD(strMMDFillBlParams.layer,strMMDFillBlParams.startpointX,starty+lcd_line_num,strMMDFillBlParams.screenW,lastline);
      }
 }
}

MMD_U32 MMD_LCD_Refresh_MP4(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U08 *pdata,MMD_U32 updateFlag)
{
 MMD_U32 getBuff=0,ret=0;
 MMD_U16 count=0,Fori=0,lastline=0,starty=0;
while(play_timer_flag==1)
    {
        rex_sleep(1);
    }
    lcd_refresh_flag=1;

if(emMMDTimerState==NO_TIMER_START)
 {
     MMD_LCD_ReGwin(0,0);
	  VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,0,176,20,strMMDFillBlParams.pBuff);
	  VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,0,176,20);
	  //MMD_USER_WaitMICRS(50);
     //MMD_LCD_ReGwin(0,10);
     //VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,10,176,10,strMMDFillBlParams.pBuff);
	 // VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,10,176,10);
	  //MMD_USER_WaitMICRS(50);
     //count=5;
    // for(Fori=0;Fori<count;Fori++)
    // {
     //    starty=164+10*Fori;
	      MMD_LCD_ReGwin(0,164);
      	VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,164,176,56,strMMDFillBlParams.pBuff);
      	VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,164,176,56);
      	
 }
 else if(emMMDUpdateState == UPDATED)
 {
   strMMDFillBlParams.layer = layer;
    strMMDFillBlParams.startpointX = sx;
    strMMDFillBlParams.startpointY = sy;
    strMMDFillBlParams.screenW = width;
    strMMDFillBlParams.screenH = height;
    strMMDFillBlParams.pBuff = pdata;
    emMMDUpdateState = NEED_UPDATE;
 }
  lcd_refresh_flag=0;
 return ret;
}


void MMD_LCD_Refresh_In_Timer_MP4(void)
{
   
	 MMD_U16 count=0,Fori=0,lastline=0,starty=0;
	 if(emMMDUpdateState == NEED_UPDATE)
	 {//需要刷屏
     MMD_LCD_ReGwin(0,0);
    
	  VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,0,176,20,strMMDFillBlParams.pBuff);
	  VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,0,176,20);
	  // MMD_USER_WaitMICRS(10);
     //MMD_LCD_ReGwin(0,10);
     //VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,10,176,10,strMMDFillBlParams.pBuff);
	 // VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,10,176,10);
	  //MMD_USER_WaitMICRS(50);
     //count=5;
    // for(Fori=0;Fori<count;Fori++)
    // {
     //    starty=164+10*Fori;
	      MMD_LCD_ReGwin(0,164);
         //MMD_USER_WaitMICRS(10);
      	VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,164,176,56,strMMDFillBlParams.pBuff);
      	VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,164,176,56);
         //MMD_USER_WaitMICRS(20);
    // }
    // VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,214,176,6,strMMDFillBlParams.pBuff);
    // VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,214,176,6);

//	  MMD_LCD_ReGwin(0,126);               //菜单与播放的视频相交区域刷屏
//	  VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,126,176,38,strMMDFillBlParams.pBuff);

	 }
}


MMD_U32 MMD_LCD_Refresh_Video(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,/*MMD_U32 overlayFlag,*/MMD_U08 *pdata,MMD_U32 updateFlag)
{
    
    MMD_U32 getBuff=0,ret=0;
     
     while(play_timer_flag==1)
         {
             rex_sleep(1);
         }
            
         lcd_refresh_flag=1;

     getBuff = VIM_GetBlayerBuff(layer);
     if(getBuff==NULL)
     {
      lcd_line_num = LCD_LINE_NUM(MMD_RECORD_B_SIZE,MMD_GetLCDWidth());
      FARF(MSG,("tlcd_line_num=%d",lcd_line_num));
      VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER, 0, 0, 176, lcd_line_num);
     }
     if(emMMDTimerState==NO_TIMER_START)
     {
        strMMDFillBlParams.layer = layer;
        strMMDFillBlParams.startpointX = sx;
        strMMDFillBlParams.startpointY = sy;
        strMMDFillBlParams.screenW = width;
        strMMDFillBlParams.screenH = height;
        strMMDFillBlParams.pBuff = pdata;
        emMMDUpdateState = NEED_UPDATE;
        MMD_LCD_Refresh_In_Timer_Video();
        FARF(MSG,("emMMDTimerState=NO_TIMER_START"));
        
     }
     else
     {
        strMMDFillBlParams.layer = layer;
        strMMDFillBlParams.startpointX = sx;
        strMMDFillBlParams.startpointY = sy;
        strMMDFillBlParams.screenW = width;
        strMMDFillBlParams.screenH = height;
        strMMDFillBlParams.pBuff = pdata;
        emMMDUpdateState = NEED_UPDATE;
     }
     lcd_refresh_flag=0;
         if(emMMDTimerState==TIMER_STARTED)
         {
             while(emMMDUpdateState == NEED_UPDATE)
             {
                rex_sleep(1);
             }
         }

     return ret;
    
}

//zhangxiang add for Jpeg decode 2009.01.21 
MMD_U32 MMD_LCD_Refresh_BackJpeg(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U08 *pdata,MMD_U32 updateFlag)
{
    extern int jpgSize;
    FARF(MSG,("MMD_LCD_Refresh_BackJpeg"));

    //tcl_lb add for refrash 176*220 pic 20090204
    MMD_LCD_BLY_Release();
    VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER, 0, 0, 176, 220);
    //tcl_lb end
    
    if(jpgSize == VC_CAM_SIZE_176_220)
    {
        FARF(MSG,("MMD_LCD_Refresh_BackJpeg1"));
        VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,0,176,220,pdata);
        MMD_LCD_SetOverlay (VM_ENABLE,MMD_LCD_B0LAYER,OVL_TSP, 0xffffff);//zhangxiang add
        return;
    }  
    
    //VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER, 0, 0, 176, 220);
    VIM_HAPI_DrawLCDRctngl(VIM_HAPI_B0_LAYER,0,0,176,220,pdata);
	VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,0,176,28);
    MMD_USER_WaitMICRS(100);
    VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0,160,176,60);
    MMD_LCD_BLY_Release();
    MMD_LCD_SetOverlay (VM_DISABLE,MMD_LCD_B0LAYER,OVL_TSP, 0xffffff);
}
//zhangxiang end

MMD_U32 MMD_Set_LCDIFParm(MMD_U32 bPanel)
{

	
	LCDIF_SetLCDIFParm(bPanel);

	return MMD_RET_NOERR;
}

void MMD_LCD_DrawBg(MMD_U16 layer,MMD_U16 sx,MMD_U16 sy,MMD_U16 width,MMD_U16 height,MMD_U32 color)
{
	bg_params bg_property;
	bg_property.color=color;
	bg_property.x=sx;
	bg_property.y=sy;
	bg_property.w=width;
	bg_property.h=height;
	MMD_LCD_UpdateBgWin( &bg_property );
}
void MMD_LCD_SetOverlay_Flag(MMD_U32 set)
{
	setOverlay = set;
}

MMD_U32 MMD_LCD_GetOverlay_Flag(void)
{
	return setOverlay ;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_LCD_SetOverlay
[DESCRIPTION]

[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_LCD_SetOverlay ( vm_flg_en flag, MMD_U08 layer, ovl_mod mode, MMD_U32 color )
{
	MMD_U32 clr;
	MMD_U32 overmode,ctrl;

	
	if(layer!=MMD_LCD_B0LAYER&&layer!=MMD_LCD_B1LAYER)
	{
		return MMD_RET_ERR;
	}
	clr      = color;
       color = ((((clr) & 0xf80000) ) | (((clr) & 0xfc00) ) | (((clr) & 0xf8) ));
	//if(color)
      // color |=0x070307;
       
	switch(mode)
	{
		case OVL_TSP:
		case OVL_AND:
		case OVL_OR:
		case OVL_INV:
			overmode = mode;
		break;
		default:
			return MMD_RET_ERR;
		break;
	}
	ctrl = Vm_GetReg(MMD_REG_LCDC_BASE);
	
	if(layer==MMD_LCD_B0LAYER)
	{
		Vm_SetReg(MMD_REG_LCDC_OKC_B0,color);
		ctrl &= ~(3<<4);
		ctrl |= mode<<4;
	}
	else if(layer==MMD_LCD_B1LAYER)
	{
		Vm_SetReg(MMD_REG_LCDC_OKC_B1,color);
		ctrl &= ~(3<<6);
		ctrl |= mode<<6;
		
	}
	if(flag==VM_ENABLE)
	{
		setOverlay = 1;
		ctrl |= 1<<8;
		ctrl |= 1<<9;
		ctrl &= ~(1<<10);
		ctrl |= (1<<12);
		ctrl |= 1<<11; 
	}
	else if(flag==VM_DISABLE)
	{
		setOverlay = 0;
		ctrl &= ~(1<<11);
	}
	
	Vm_SetReg(MMD_REG_LCDC_BASE, ctrl);
	//MMD_LCD_SetOverlay_Flag(1);  //zhangxiang del
	return MMD_RET_NOERR;
	

}

extern MMD_PLYFILE pFileHandle;
extern mp4v_decinfor_t  mp4v_decinfor;
extern VIM_MPEG4INFO mp4v_start_info;
MMD_PLYFILE  g_pPlayFileHandle;
extern int rotateStauts;
MMD_U32 g_rotation=0;
#if 0
MMD_U32 MMD_LCDRotate(UINT8 rotateFlag)
{

	UINT32 ret=0;
	
	MMD_Player_File_Pause(pFileHandle);
	
	if(rotateFlag)
	{
		if(rotateStauts)
		{
			MMD_Player_File_Resume(pFileHandle);
			return 0;
		}
	

		mp4v_start_info.DecWidth		= MMD_LCD_HEIGHT;
		mp4v_start_info.DecHeigth		= MMD_LCD_WIDTH;
		gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_HEIGHT;
		gVc0578b_Info.LcdStatus.Size.cy = 240;
		
		MMD_LCD_DrawBg(MMD_LCD_B0LAYER,0,0,MMD_LCD_WIDTH,MMD_LCD_HEIGHT,0x0);
		
		ret = VIM_HAPI_VideoResize(&mp4v_start_info);
		MMD_USER_WaitMICRS(200);
		VIM_SetRT(1);
		//VIM_HAPI_LCDRotate(VIM_HAPI_ROTATE_90);
		Lcd_Rotate(VIM_HAPI_ROTATE_90);
		rotateStauts = 1;
	}
	else
	{	
		if(!rotateStauts)
		{
			MMD_Player_File_Resume(pFileHandle);
			return 0;
		}


		mp4v_start_info.DecWidth		=mp4v_decinfor.disp_width;
		mp4v_start_info.DecHeigth		=mp4v_decinfor.disp_height;
		gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_WIDTH;
		gVc0578b_Info.LcdStatus.Size.cy = MMD_LCD_HEIGHT;


		
		ret = VIM_HAPI_VideoResize(&mp4v_start_info);
		MMD_USER_WaitMICRS(200);
		
		//VIM_HAPI_LCDRotate(VIM_HAPI_ROTATE_0);
		Lcd_Rotate(VIM_HAPI_ROTATE_0);
		rotateStauts= 0 ;
		MMD_LCD_DrawBg(MMD_LCD_B0LAYER,0,0,MMD_LCD_WIDTH,MMD_LCD_HEIGHT,0x00ffff);
		VIM_SetRT(0);
	}

	g_rotation=1;
	MMD_Player_File_Resume(pFileHandle);

	return ret;
}
#endif
MMD_U32 MMD_LCDRotate(UINT8 rotateFlag)
{
 
                UINT32 ret=0;
//                MMD_PLYFILE  PlayFileHandle;
                
//                PlayFileHandle=Player_Get_PlyHandle();
		while((play_timer_flag==1)||(lcd_refresh_flag==1))
 		 {
      			  rex_sleep(1);
		  }
                if(pFileHandle!=0)
                 MMD_Player_File_Pause(pFileHandle);
                FARF(MSG,("MMD_LCDRotate  pFileHandle = %d",pFileHandle));
                if(rotateFlag)
                {
                                           
                 mp4v_start_info.DecWidth  =MMD_LCD_HEIGHT;
                 mp4v_start_info.DecHeigth  =MMD_LCD_WIDTH;
		  mp4v_start_info.DecPoint_x=0; //zhangxiang add
		   mp4v_start_info.DecPoint_y=0; 
		   
                 mp4v_decinfor.disp_rotation=1;
                 gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_HEIGHT;
                 gVc0578b_Info.LcdStatus.Size.cy = MMD_LCD_WIDTH;
                 MMD_LCD_BLY_Release(); //zhangxiang add
                 MMD_LCD_SetOverlay(VM_DISABLE, MMD_LCD_B0LAYER, OVL_TSP, 0xffffff);
                 VIM_HAPI_SetLCDSize(VIM_HAPI_B1_LAYER, 0, 0, 0, 0);
                 VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER, 0, 0, 0, 0);
                 //VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_AFIRST, 0);
                 
                 MMD_LCD_DrawBg(MMD_LCD_B0LAYER,0,0,MMD_LCD_WIDTH,MMD_LCD_HEIGHT,0x0);
                 
                 ret = VIM_HAPI_VideoResize(&mp4v_start_info);
                 MMD_USER_WaitMICRS(200);
                 VIM_SetRT(1);
               FARF(MSG,("MMD_LCDRotate -Lcd_Rotate"));
 
                 Lcd_Rotate(VIM_HAPI_ROTATE_90);
                 rotateStauts = 1;
                }
                else
                { 
                                
                 mp4v_start_info.DecWidth  =mp4v_decinfor.disp_width;
                 mp4v_start_info.DecHeigth  =mp4v_decinfor.disp_height;
		     mp4v_start_info.DecPoint_x=mp4v_decinfor.disp_xpos; //zhangxiang add
		   mp4v_start_info.DecPoint_y= mp4v_decinfor.disp_ypos;   //zhangxiang add
                // printf("\n w=%d,h=%d\n",mp4v_decinfor.disp_width,mp4v_decinfor.disp_height);
                 mp4v_decinfor.disp_rotation=0;
                 gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_WIDTH;
                 gVc0578b_Info.LcdStatus.Size.cy = MMD_LCD_HEIGHT;
                
                 ret = VIM_HAPI_VideoResize(&mp4v_start_info);
                 MMD_USER_WaitMICRS(200);
                 
                 Lcd_Rotate(VIM_HAPI_ROTATE_0);
                 rotateStauts= 0 ;
                 //MMD_LCD_DrawBg(MMD_LCD_B0LAYER,0,0,MMD_LCD_WIDTH,MMD_LCD_HEIGHT,0x00ffff);  //zhangxiang del
                 VIM_SetRT(0);
                 //VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_OVERLAY, 0);//zhangxiang del 09/02/27
                 VIM_HAPI_SetLCDSize(VIM_HAPI_B1_LAYER, 0, 0, 0, 0);
                 VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER, 0, 164, 176, 56);
                 VIM_HAPI_DrawLCDPureColor(VIM_HAPI_B0_LAYER,0, 164, 176, 56,0);
                 VIM_HAPI_UpdateLCD(VIM_HAPI_B0_LAYER,0, 164, 176, 56);
                 
                 MMD_LCD_SetOverlay (VM_ENABLE,MMD_LCD_B0LAYER,OVL_TSP, 0xffffff);//zhangxiang add
                }
                
                g_rotation=1;
                if(pFileHandle!=0)
                 MMD_Player_File_Resume(pFileHandle);
                
                return ret;
}

extern TSize gb0lcdsiz;
MMD_U32 CMMB_MMD_LCDRotate(UINT8 rotateFlag)
{

	UINT32 ret=0;
	

	
	if(rotateFlag)
	{	
		

		mp4v_start_info.SorHeigth	=MMD_LCD_WIDTH;
		mp4v_start_info.SorWidth		= MMD_LCD_HEIGHT;
		mp4v_start_info.DecPoint_x	=0;
		mp4v_start_info.DecPoint_y	= MMD_LCD_HEIGHT;
	
		mp4v_start_info.FrameRate=30;

		mp4v_start_info.DecWidth		= MMD_LCD_HEIGHT;
		mp4v_start_info.DecHeigth		=MMD_LCD_WIDTH;
		gVc0578b_Info.LcdStatus.Size.cx =  MMD_LCD_HEIGHT;
		gVc0578b_Info.LcdStatus.Size.cy = MMD_LCD_WIDTH;
		
		MMD_LCD_DrawBg(MMD_LCD_B0LAYER,0,0,MMD_LCD_WIDTH, MMD_LCD_HEIGHT,0x0);
		
		ret = VIM_HAPI_VideoResize(&mp4v_start_info);
		MMD_USER_WaitMICRS(200);
		
		//VIM_HAPI_LCDRotate(VIM_HAPI_ROTATE_90);
		CMMB_LCD_ROTATE(VIM_HAPI_ROTATE_90);
		VIM_SetRT(1);
		rotateStauts = 1;
	}
	else
	{	
		mp4v_start_info.SorHeigth	=MMD_LCD_WIDTH;
		mp4v_start_info.SorWidth		= MMD_LCD_HEIGHT;
		mp4v_start_info.DecPoint_x	=0;
		mp4v_start_info.DecPoint_y	=0;
	
		mp4v_start_info.FrameRate=30;

		mp4v_start_info.DecWidth		=MMD_LCD_WIDTH;
		mp4v_start_info.DecHeigth		= MMD_LCD_HEIGHT;
		gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_WIDTH;
		gVc0578b_Info.LcdStatus.Size.cy =  MMD_LCD_HEIGHT;

		
		ret = VIM_HAPI_VideoResize(&mp4v_start_info);
		MMD_USER_WaitMICRS(200);
		
		//VIM_HAPI_LCDRotate(VIM_HAPI_ROTATE_0);
		CMMB_LCD_ROTATE(VIM_HAPI_ROTATE_0);
		rotateStauts= 0 ;
		MMD_LCD_DrawBg(MMD_LCD_B0LAYER,0,0,MMD_LCD_WIDTH, MMD_LCD_HEIGHT,0x0);
		VIM_SetRT(0);
	}

	return ret;
}
