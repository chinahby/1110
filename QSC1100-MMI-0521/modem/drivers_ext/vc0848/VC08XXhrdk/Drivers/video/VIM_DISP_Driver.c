
/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : VIM_DISP_Driver.c
 * [Description]          : 5X 's LCDC moudle sub driver:578b 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-03-23 
 * [Platform]              : not limited by platform
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2005-11-02  	MaNing			The first version
 * 2006-06-08  	Angela			Update for 528
 * 2006-03-23  	WendyGuo         	Created 578b
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
#include "VIM_COMMON.h"
extern unsigned int g_b0LayerUpdateFinished;
extern unsigned int g_b1LayerUpdateFinished;
/*-----------------------------------------------------------------------------
Description:
		 set lcdc work mode.
		
Parameters:
		mode	: One of the following <U>VM_LCDC_WORKMODE</U>
 				enumeration values that specify lcdc work mode.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:

 --------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetWorkMode(VIM_DISP_WORKMODE bWorkMode)
{
	UINT32 value32 = 0;


	value32 = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	switch(bWorkMode)
	{
		case VIM_DISP_AFIRST:
			value32&=(~(BIT8				/*!< layer a0 has higher priority, in front of layer b0 */
					|BIT9				/*!< layer a0 has higher priority, in front of layer b1 */
					|BIT10				/*!< disable alpha blending */
					|BIT11				/*!< disable overlay */
					|BIT12));			/*!< normal mode */
			break;
		case VIM_DISP_BFIRST:
			value32&=(~(BIT12			/*!< normal mode */
					|BIT11				/*!< disable overlay */
					|BIT10));			/*!< disable alpha blending */
			value32|=BIT8;  				/*!< layer b0 has higher priority, in front of layer a0 */	
			break;
		case VIM_DISP_B0_OVERLAY:
			value32&=(~BIT12);			/*!< normal mode */
			value32|=BIT11;  				/*!< enable overlay */
			value32&=(~BIT10);			/*!< disable alpha blending */
			value32|=BIT8;   				/*!< layer b0 has higher priority, in front of layer a0 */	
					
			//Temp|=BIT0;					/*!< layer B0 update request, this bit is self-clear */	//wendy guo 
			break;
		case VIM_DISP_B0_BLEND:
			value32&=(~(BIT12			/*!< normal mode */
					|BIT11));			/*!< disable overlay */
			value32|=BIT10;  				/*!< enable alpha blending */
			value32|=BIT8;   				/*!< layer b0 has higher priority, in front of layer a0 */
						
			//Temp|=BIT0;					/*!< layer B0 update request, this bit is self-clear */	//wendy guo 
			break;
		case VIM_DISP_ALL_OVERLAY:
			value32&=(~BIT12);			/*!< normal mode */
			value32|=BIT11;  				/*!< enable overlay */
			value32&=(~BIT10);			/*!< disable alpha blending */

			value32|=BIT9;				/*!< layer b1 has higher priority, in front of layer a0 */
			value32|=BIT8;  				/*!< layer b0 has higher priority, in front of layer a0 */	
			
			break;
		case VIM_DISP_ALL_BLEND:
			value32&=(~(BIT12			/*!< normal mode */
					|BIT11));			/*!< disable overlay and disable B0 display on the LCD screen */
			value32|=BIT10;  				/*!< enable alpha blending */
			value32|=BIT9;   				/*!< layer b1 has higher priority, in front of layer a0 */
			value32|=BIT8;				/*!< layer b0 has higher priority, in front of layer a0 */
			break;
		case VIM_DISP_DECODE:
			value32|=BIT12;  				/*!< decode mode */
			break;
		default:
			return VM_ERR_LCDC_MODE_UNKNOWN;
	}
	
			
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL,value32);
	
	
	VIM_DISP_ResetState();

	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
Description:
		 select lcdc dither mode.
		
Parameters:
		dithersel	: One of the following <U>VM_LCDC_DITHERSEL</U>
 				enumeration values that specify lcdc dither mode.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetDitherSelet(VIM_DISP_DITHERSEL byDitherSelect)
{
	UINT32 value32 = 0;

	if(byDitherSelect > VIM_DISP_RGB332)
		return VM_ERR_LCDC_PARAM_OUTOFRANGE;

	value32 = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	value32 &= ~(BIT2|BIT3);			//regval &= (~((UINT32)3<<2));
	value32 |= (byDitherSelect<<2);	// wendy guo "SFBUF VBUFM" these are not correct comment???
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL,value32);
	return VIM_SUCCEED;
}

/*-----------------------------------------------------------------------------
Description:
		 set lcdc buffer mode.
		
Parameters:
		mode	: One of the following <U>VM_LCDC_BUFFERMODE</U>
 				enumeration values that specify lcdc buffer mode.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetBufferMode(VIM_DISP_BUFFERMODER byBufferMode)
{
	UINT32 value32 = 0;

	if(byBufferMode > VIM_DISP_ONEFRAME){
		return VM_ERR_LCDC_MODE_UNKNOWN;
	}

	if(byBufferMode==VIM_DISP_NODISP)
		byBufferMode=VIM_DISP_LINEBUF;
	if((byBufferMode != 0) && (byBufferMode != 2) && (byBufferMode != 3))
		return VM_ERR_LCDC_MODE_UNKNOWN;

	value32 = VIM_HIF_GetReg16(V5_REG_LCDC_CTRL);
	value32 &= ~(BIT13|BIT14);
	value32 |=( byBufferMode<<13);	// SFBUF VBUFM	//wendy guo, ask Heju there no two frame set,why??
	VIM_HIF_SetReg16(V5_REG_LCDC_CTRL,value32);
	return VIM_SUCCEED;
}

/*-----------------------------------------------------------------------------
Description:
		 get lcdc buffer mode.
		
Parameters:
		void.
		
Returns:
		lcdc buffer mode.
		
Remarks:
--------------------------------------------------------------------------------*/
VIM_DISP_BUFFERMODER VIM_DISP_GetBufferMode(void)
{
	UINT16 value32 = 0;

	value32 = VIM_HIF_GetReg16(V5_REG_LCDC_CTRL);
	value32 &= (BIT13|BIT14);
	value32 >>=13;	
	return (VIM_DISP_BUFFERMODER)value32;
}

/*-----------------------------------------------------------------------------
Description:
		 enable/disable lcdc dither effect.
		
Parameters:
		enable	: 1 for enable, 0 for disable.
		
Returns:
		void.
		
Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetDitherEnable(BOOL Enable)
{
	UINT16 value16 = 0;

	value16 = VIM_HIF_GetReg16(V5_REG_LCDC_CTRL);
	value16 &= ~(BIT1);				//Dither enable bit, wendy guo
	value16 |= (Enable<<1);				//wendy guo bit1 is enable bit! SFBUF VBUFM???
	VIM_HIF_SetReg16(V5_REG_LCDC_CTRL,value16);
	return VIM_SUCCEED;
}
/***************************************************************
Description:
		 enable/disable display of specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
				</VM_LCDC_LAYER_A0>
				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BALL>
		enable	: 1 for enable, 0 for disable.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
****************************************************************/
VIM_RESULT VIM_DISP_SetLayerEnable(VIM_DISP_LAYER bLayer,BOOL Enable)
{
	UINT32 regval = 0;
	//VIM_DISP_ResetState();				//wendy guo 4/13/2007
	regval = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	switch(bLayer)
	{
	case VIM_DISP_ALAYER:
//#if (BOARD_TYPE==VC0820_FPGA)
		//regval &= (~((UINT32)((1<<16)|(1<<15))));
		//regval |= ((UINT32)((Enable<<16)|(Enable<<15)));		/*!< enable/disable layer a0 display */
//#else
		regval &= (~((UINT32)1<<16));
		regval |= ((UINT32)Enable<<16);		/*!< enable/disable layer a0 display */
		regval |= ((UINT32)1<<15);			/*!< enable layer a0 operation */
//#endif
		break;
	case VIM_DISP_A1LAYER:
		regval &= (~((UINT32)1<<17));
		regval |= ((UINT32)Enable<<17);		/*!< enable/disable layer a0 display */
		break;
	case VIM_DISP_B0LAYER:
		regval &= (~((UINT32)1<<18));
		regval |= ((UINT32)Enable<<18);		/*!< enable/disable layer b0 display */
		break;
	case VIM_DISP_B1LAYER:
		regval &= (~((UINT32)1<<19));
		regval |= ((UINT32)Enable<<19);		/*!< enable/disable layer b1 display */
		break;
	case VIM_DISP_ALLBLAYER:
		if(Enable){
			regval |= ((UINT32)3<<18);		/*!< enable layer b0/b1 display */
		}else{
			regval &= (~((UINT32)3<<18));	/*!< disable layer b0/b1 display */
		}
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL, regval);
	VIM_USER_DelayMs(1);

	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 set lcd panel interface mode(dbi/dpi).
		
Parameters:
		mode	: One of the following <U>VM_LCDC_PANELIFMODE</U>
 				enumeration values that specify lcdc layer.
				</VM_LCDC_PANELIF_DBI>
				</VM_LCDC_PANELIF_DPI>

Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_DISP_SetPanelifMode(VIM_DISP_PANELIFMODE mode)
{
	UINT32	regval = 0;
	
	if((mode!=VM_LCDC_PANELIF_DBI) || (mode !=VM_LCDC_PANELIF_DPI)){
		return VM_ERR_LCDC_PANELIF_UNKNOWN;
	}
	regval = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	regval &= (~((UINT32)0x03<<20));
	regval |= ((UINT32)mode<<20);
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL, regval);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 get lcd panel interface mode(dbi/dpi).
		
Parameters:
		void.
		
Returns:
		panel interface mode(VM_LCDC_PANELIFMODE).
			</VM_LCDC_PANELIF_DBI>
			</VM_LCDC_PANELIF_DPI>
		
Remarks:
		
************************************************************************************/
UINT8 VIM_DISP_GetPanelifMode(void)
{
	UINT32	regval = 0;
	
	regval = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	regval >>= 20;
	
	return (VIM_DISP_PANELIFMODE)(regval&0x03);
}



/***********************************************************************************
Description:
		 update specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BALL>
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/


/*-----------------------------------------------------------------------------
  Summary:
	Set Dither enable
  Parameters:
	Enable :enable or not
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
#if 0
VIM_RESULT VIM_DISP_SaveAramEnable(BOOL Enable)
{
					//in 578b, there are no  this register bit to set! wendy guo 
	UINT16 value16 = 0;

	value16 = VIM_HIF_GetReg16(V5_REG_LCDC_DE);
	value16 &= ~(BIT0);
	value16 |= (Enable);	// SFBUF VBUFM
	if(Enable==DISABLE)
		value16 &= ~(BIT1);
	VIM_HIF_SetReg16(V5_REG_LCDC_DE,value16);

	return VIM_SUCCEED;


}
#endif


/*-----------------------------------------------------------------------------
  Summary:
	Reset all Display except register
  Parameters:
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
void VIM_DISP_ResetState(void)
{
	VIM_HIF_SetReg32(V5_REG_LCDC_RESET, 0x00);
	VIM_HIF_SetReg32(V5_REG_LCDC_RESET, 0x01);
}

/*-----------------------------------------------------------------------------
Description:
		 update specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BALL>
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_Update(VIM_DISP_LAYER BLayer)
{
	UINT32 value = 0;

	if((BLayer !=VIM_DISP_B0LAYER)&&( BLayer != VIM_DISP_B1LAYER)&&(BLayer!=VIM_DISP_ALLBLAYER))
		return VM_ERR_LCDC_LAYER_UNKNOWN;

	value = VIM_HIF_GetReg32(V5_REG_LCDC_UPDATE);

	switch(BLayer)
	{
	case VIM_DISP_B0LAYER:
		value |= 1;
		VIM_HIF_SetReg32(V5_REG_LCDC_UPDATE,value);
		while(VIM_HIF_GetReg32(V5_REG_LCDC_UPDATE) & 1);
		break;
	case VIM_DISP_B1LAYER:
		value |= 2;
		VIM_HIF_SetReg32(V5_REG_LCDC_UPDATE,value);
		while(VIM_HIF_GetReg32(V5_REG_LCDC_UPDATE) & 2);
		break;
	case VIM_DISP_ALLBLAYER:
		value |= 3;
		VIM_HIF_SetReg32(V5_REG_LCDC_UPDATE,value);
		while(VIM_HIF_GetReg32(V5_REG_LCDC_UPDATE) & 3);
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	
	return VIM_SUCCEED;

}
/*-----------------------------------------------------------------------------
  Summary:
	set Rotate mode
  Parameters:
	byARotateMode : A0 layer Rotate Mode	(only A0 layer can be rotated)
	
  Note:
  Returns: VIM_SUCCEED
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetRotateMode(VIM_DISP_ROTATEMODE RotateMode)
{
	UINT32	regval = 0, layera0size = 0;
	TSize	size;
	
	if(RotateMode > VIM_DISP_MIRROR_270){
		return VM_ERR_LCDC_ROTATE_UNKNOWN; 
	}
	
	regval = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	//prevmode = (regval>>28)&0x07;		///?????????????????????
	//if((RotateMode%2) != (prevmode%2)){
	if(RotateMode%2){
		VIM_DISP_GetAWinsize(&size);				//original is &Size, wendy maked
				
		layera0size=(UINT32)size.cx-1;					//NOTE: Write the size.cx to Height, size.cy to Width!
		layera0size|=((UINT32)size.cy-1)<<16;
		VIM_HIF_SetReg32(V5_REG_LCDC_A0_SIZE_H,	layera0size);	//high 9bit is Width, low 9bit  is Height
	}
	regval &= (~((UINT32)7<<28));
	regval |= RotateMode<<28;						//wendy guo 4/18/2007
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL, regval);
	
	return VIM_SUCCEED;
	
}



/***********************************************************************************
Description:
		 set overlay mode of specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BALL>
 		mode	: One of the following <U>VM_LCDC_OVERLAYMODE</U>
 				enumeration values that specify overlay mode.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_DISP_SetOverlayMode(VIM_DISP_LAYER layer, VIM_DISP_OVERLAYMODE mode)
{
	UINT32	regval = 0;
	
	if(mode > VM_LCDC_OVERLAY_INVERT){
		return VM_ERR_LCDC_OVERLAY_UNKNOWN;
	}
	
	regval = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	switch(layer)
	{
	case VIM_DISP_B0LAYER:					//Overlay mode for layer B0
		regval &= (~((UINT32)3<<4));			//[5:4]=00, Transparent overlay; [5:4]=01, AND overlay; 
		regval |= ((UINT32)mode<<4);			//[5:4]=00, OR overlay; 		[5:4]=00, INVERT overlay; 
		break;
	case VIM_DISP_B1LAYER:					//Overlay mode for layer B1
		regval &= (~((UINT32)3<<6));			//[7:6]=00, Transparent overlay; [7:6]=01, AND overlay;
		regval |= ((UINT32)mode<<6);			//[7:6]=01, OR overlay;		[7:6]=01, INVERT overlay;
		break;
	case VIM_DISP_ALLBLAYER:
		regval &= (~((UINT32)3<<4));
		regval |= ((UINT32)mode<<4);
		regval &= (~((UINT32)3<<6));
		regval |= ((UINT32)mode<<6);
		break;
	default:
		return VM_ERR_LCDC_PARAM_OUTOFRANGE;
	}
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL, regval);
	
	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
  Summary:
	Set A layer Display panel startpoint and size
  Parameters:
	startPoint	: start Point in LCD 
	winSize		: size in LCD
  Note:

       important calss: *******
       V5_REG_LCDC_AW value really is width-1
       V5_REG_LCDC_AH value really is height-1
       
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetA1_DisplayPanel(TPoint startPoint,TSize winSize)
{
	if(startPoint.x > 1023 || startPoint.y > 1023)
		return VIM_ERROR_DISP_PARAM;

	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set start Point
	VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS+2,startPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS,startPoint.y);
	// Set Panel Size
	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;
	VIM_HIF_SetReg16(V5_REG_LCDC_A1_SIZE_W,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_A1_SIZE_H,winSize.cy);
	return VIM_SUCCEED;
}
void VIM_DISP_GetA1Winsize(PTSize winSize)
{
	winSize->cx=VIM_HIF_GetReg16(V5_REG_LCDC_A1_SIZE_W)+1;			//9bit is width=cx
	winSize->cy=VIM_HIF_GetReg16(V5_REG_LCDC_A1_SIZE_H)+1;			//9bit is hight=cy
}

/*-----------------------------------------------------------------------------
  Summary:
	Set A layer Display panel startpoint and size
  Parameters:
	startPoint	: start Point in LCD 
	winSize		: size in LCD
  Note:

       important calss: *******
       V5_REG_LCDC_AW value really is width-1
       V5_REG_LCDC_AH value really is height-1
       
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetA_DisplayPanel(TPoint startPoint,TSize winSize)
{
	if(startPoint.x > 1023 || startPoint.y > 1023)
		return VIM_ERROR_DISP_PARAM;

	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set start Point
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS+2,startPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS,startPoint.y);
	// Set Panel Size
	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_SIZE_W,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_SIZE_H,winSize.cy);
	return VIM_SUCCEED;
}

/*-----------------------------------------------------------------------------
  Summary:
	Set A layer Display panel startpoint and size
  Parameters:
	startPoint	: start Point in LCD 
	winSize		: size in LCD
  Note:

       important calss: *******
       V5_REG_LCDC_AW value really is width-1
       V5_REG_LCDC_AH value really is height-1
       
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetA_DisplaySize(TSize winSize)
{
	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set Panel Size
	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_SIZE_W,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_SIZE_H,winSize.cy);
	return VIM_SUCCEED;
}
void VIM_DISP_GetAWinsize(PTSize winSize)
{
	winSize->cx=VIM_HIF_GetReg16(V5_REG_LCDC_A0_SIZE_W)+1;			//9bit is width=cx
	winSize->cy=VIM_HIF_GetReg16(V5_REG_LCDC_A0_SIZE_H)+1;			//9bit is hight=cy
}
/*-----------------------------------------------------------------------------
  Summary:
	Set A layer memory panel startpoint and size
  Parameters:
	memPoint	: start Point in memory buffer 
	memSize		: size of memory buffer
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetA_Memory(TPoint memPoint,TSize memSize)
{
	if(memPoint.x > AM_MAXPIXEL || memPoint.y > AM_MAXPIXEL)
		return VIM_ERROR_DISP_PARAM;

	if(memSize.cx > AM_MAXPIXEL || memSize.cy > AM_MAXPIXEL)
		return VIM_ERROR_DISP_PARAM;
   
	// Set mem Point
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_VFRAME_POS+2,memPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_VFRAME_POS,memPoint.y);
	
	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
  Summary:
	Set B0 layer Display panel startpoint and size
  Parameters:
	startPoint	: start Point in LCD 
	winSize		: size in LCD
  Note: set BDX0\BDY0 is 0,BMW0 is winSize's width
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetB0_DisplayPanel(TPoint startPoint,TSize winSize)
{
	if(startPoint.x > 1023 || startPoint.y > 1023)
		return VIM_ERROR_DISP_PARAM;

	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set start Point
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,startPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,startPoint.y);
	
	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;

	// Set Panel Size
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_SIZE+2,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_SIZE, winSize.cy);
	// Set Memory Width
	VIM_HIF_SetReg16(V5_REG_LCDC_GBUF_WIDTH,winSize.cx+1);		//low 16bit(10bit) is Layer B0 graphic buffer width
	
	// Set B0 Position in Graphic Buffer 0
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_GBUF0_POS+2,0);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_GBUF0_POS,0);
	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
  Summary:
	set B0 layer Refresh Block start point and size
  Parameters:
	startPoint	: start Point in B0 layer 
	winSize		: size in B0 layer
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetB0_RefreshBlock(TPoint startPoint,TSize winSize)
{
	if(startPoint.x > 1023 || startPoint.y > 1023)
		return VIM_ERROR_DISP_PARAM;

	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set start Point
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_UPD_POS+2,startPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_UPD_POS,startPoint.y);

	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;	
	// Set Panel Size
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_UPD_SIZE+2,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_UPD_SIZE,winSize.cy);
	return VIM_SUCCEED;
}



/*-----------------------------------------------------------------------------
  Summary:
	Set B1 layer Display panel startpoint and size
  Parameters:
	startPoint	: start Point in LCD 
	winSize		: size in LCD
  Note: set BDX1/BDY1 is 0,BMW1 is winSize's width
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetB1_DisplayPanel(TPoint startPoint,TSize winSize)
{
	if(startPoint.x > 1023 || startPoint.y > 1023)
		return VIM_ERROR_DISP_PARAM;

	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set start Point
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS+2,startPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS,startPoint.y);
	
	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;
	// Set Panel Size
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_SIZE+2,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_SIZE,winSize.cy);
	// Set Memory Width
	VIM_HIF_SetReg16(V5_REG_LCDC_GBUF_WIDTH+2,winSize.cx+1);	//high 16bit(10bit) is Layer B1 graphic buffer width
	// Set BDX0 and BDY0
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_GBUF1_POS+2,0);
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_GBUF1_POS,0);
	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
  Summary:
	set B1 layer Refresh Block start point and size
  Parameters:
	startPoint	: start Point in B1 layer 
	winSize		: size in B1 layer
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetB1_RefreshBlock(TPoint startPoint,TSize winSize)
{
	if(startPoint.x > 1023 || startPoint.y > 1023)
		return VIM_ERROR_DISP_PARAM;

	if(winSize.cx > 1024 || winSize.cy > 1024)
		return VIM_ERROR_DISP_PARAM;

	// Set start Point
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_UPD_POS+2,startPoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_UPD_POS,startPoint.y);

	if(winSize.cx)
		winSize.cx--;
	if(winSize.cy)
		winSize.cy--;
	// Set Panel Size
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_UPD_SIZE+2,winSize.cx);
	VIM_HIF_SetReg16(V5_REG_LCDC_B1_UPD_SIZE,winSize.cy);
	return VIM_SUCCEED;	

}



/*-----------------------------------------------------------------------------
Description:
		 set alpha belnding ratio of specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BALL>
 		ratio	: alpha value(transparent ration) for blending.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetBlendRatio(VIM_DISP_LAYER BLayer,UINT16 byBlendRatio)
{

	if((BLayer!=VIM_DISP_B0LAYER) && (BLayer!=VIM_DISP_B1LAYER) && (BLayer!=VIM_DISP_ALLBLAYER)){
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	
	if(byBlendRatio > 255)
		return VIM_ERROR_DISP_PARAM;

	VIM_HIF_SetReg16(V5_REG_LCDC_BLENDRATIO_B, byBlendRatio);		//low 8bit is Layer B0 blend ratio, high 8bit is layer B1 blend ratio
	
	return VIM_SUCCEED;
}

/*-----------------------------------------------------------------------------
  Description:
		 set b layer graphic buffer data format.
		
Parameters:
		format	: One of the following <U>VM_LCDC_BFORMAT</U>
 				enumeration values that specify lcdc work mode.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetGbufFormat(VIM_DISP_BFORMAT byGbufFmt)
{
	UINT32 bufFMT;
	
	if(byGbufFmt > VIM_DISP_BLAYER_RGB32)
		return VM_ERR_LCDC_BFORMAT_UNKNOWN;
	
	bufFMT=VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	bufFMT&=(~((UINT32)0x0f<<24));
	bufFMT|=((UINT32)byGbufFmt<<24);								//bit24--bit27 is buffer format
	
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL, bufFMT);
	return VIM_SUCCEED;
}

/*-----------------------------------------------------------------------------
  Summary:
	Get B layer graphic buffer format
  Parameters:
	BLayer : select B0 / B1 / all layer
	byGbufFmt : point of graphic buffer formate VIM_DISP_Blayer_FORMAT
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
void VIM_DISP_GetGbufFormat(VIM_DISP_BFORMAT * byGbufFmt)
{

	UINT32 bufFMT;
	bufFMT=VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);

	bufFMT>>=24;
	*byGbufFmt=(VIM_DISP_BFORMAT)(bufFMT&0x0f);		//If use (UINT8), will produce a warning when comple the project

}

/*-----------------------------------------------------------------------------
Description:
		 set overlay key color of specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BALL>
 		keycolor: overlay key color.	
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		

--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_SetOverlayKeyColor(VIM_DISP_LAYER BLayer,UINT32 dwKeyColor)
{
	
	if((BLayer!=VIM_DISP_B0LAYER) && (BLayer!=VIM_DISP_B1LAYER) && (BLayer!=VIM_DISP_ALLBLAYER)){
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}

	dwKeyColor&=0xffffff;				 //clear okcm use key color, only 24bit effected
	if(BLayer & VIM_DISP_B0LAYER)
	{
		VIM_HIF_SetReg32(V5_REG_LCDC_OKC_B0,dwKeyColor);
	}
	if(BLayer & VIM_DISP_B1LAYER)
	{
		VIM_HIF_SetReg32(V5_REG_LCDC_OKC_B1,dwKeyColor);
	}
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 get window size od specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_A0>
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 		psize	: address to store size.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_DISP_GetWinSize(VIM_DISP_LAYER layer, PTSize psize)
{
	UINT32	regval = 0;
	
	switch(layer)
	{
	case VIM_DISP_ALAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_A0_SIZE_H);
		break;
	case VIM_DISP_B0LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B0_SIZE);
		break;
	case VIM_DISP_B1LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B1_SIZE);
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	psize->cy = (UINT16)((regval&0x3ff)+1);
	psize->cx = (UINT16)(((regval>>16)&0x3ff)+1);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 set window size od specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_A0>
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 		size	: window size.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
#if 0
VIM_RESULT VIM_DISP_SetWinSize(VIM_DISP_LAYER layer, TSize size)
{
	UINT32	regval = 0, regaddr = 0;
	
	switch(layer)
	{
	case VIM_DISP_ALAYER:
		regaddr = (UINT32)V5_REG_LCDC_A0_SIZE_H;
		break;
	case VIM_DISP_B0LAYER:
		regaddr = (UINT32)V5_REG_LCDC_B0_SIZE;
		break;
	case VIM_DISP_B1LAYER:
		regaddr = (UINT32)V5_REG_LCDC_B1_SIZE;
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	regval |= (UINT32)(((size.cx&0x3ff)-1)<<16);
	regval |= (UINT32)((size.cy&0x3ff)-1);
	VIM_HIF_SetReg32(regaddr, regval);
	
	return VIM_SUCCEED;
}
#endif
/***********************************************************************************
Description:
		 set display panel of specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
  				</VM_LCDC_LAYER_A0>
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 		startpt	: start point of display panel.
 		winsize	: window size of display panel.
 		
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
#if 0
VIM_RESULT VIM_DISP_SetDisplayPanel(VIM_DISP_LAYER layer, TPoint startpt, TSize winsize)
{
	UINT32	posval=0, sizeval=0, posaddr=0, sizeaddr=0;

	
	if((startpt.x>1023) || (startpt.y>1023)){
		return VM_ERR_LCDC_PARAM_OUTOFRANGE;
	}
	if((winsize.cx>1024) || (winsize.cy>1024)){
		return VM_ERR_LCDC_PARAM_OUTOFRANGE;
	}
	
	switch(layer)
	{
	case VIM_DISP_ALAYER:
		posaddr		= (UINT32)V5_REG_LCDC_A0_LCD_POS;
		sizeaddr	= (UINT32)V5_REG_LCDC_A0_SIZE_H;
		break;
	case VIM_DISP_B0LAYER:
		posaddr		= (UINT32)V5_REG_LCDC_B0_LCD_POS;
		sizeaddr	= (UINT32)V5_REG_LCDC_B0_SIZE;
		break;
	case VIM_DISP_B1LAYER:
		posaddr		= (UINT32)V5_REG_LCDC_B1_LCD_POS;
		sizeaddr	= (UINT32)V5_REG_LCDC_B1_SIZE;
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	posval	|= (UINT32)((startpt.x&0x3ff)<<16);
	posval	|= (UINT32)(startpt.y&0x3ff);
	sizeval	|= (UINT32)(((winsize.cx&0x3ff)-1)<<16);
	sizeval	|= (UINT32)((winsize.cy&0x3ff)-1);
	VIM_HIF_SetReg16(posaddr, posval);
	VIM_HIF_SetReg16(sizeaddr, sizeval);
	
	return VIM_SUCCEED;
}
#endif
/***********************************************************************************
Description:
		 set update block of specified b layer(b0 or b1).
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 				</VM_LCDC_LAYER_BG>
 		startpt	: start point of update block.
 		winsize	: update block size.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
#if 0
VIM_RESULT VIM_DISP_SetRefreshBlock(VIM_DISP_LAYER layer, TPoint startpt, TSize winsize)
{
	UINT32	posval=0, sizeval=0, posaddr=0, sizeaddr=0;
	
	if((startpt.x>511) || (startpt.y>511)){
		return VM_ERR_LCDC_PARAM_OUTOFRANGE;
	}
	if((winsize.cx>512) || (winsize.cy>512)){
		return VM_ERR_LCDC_PARAM_OUTOFRANGE;
	}
	
	switch(layer)
	{
	case VIM_DISP_B0LAYER:
		posaddr	= (UINT32)V5_REG_LCDC_B0_UPD_POS;
		sizeaddr= (UINT32)V5_REG_LCDC_B0_UPD_SIZE;
		break;
	case VIM_DISP_B1LAYER:
		posaddr	= (UINT32)V5_REG_LCDC_B1_UPD_POS;
		sizeaddr= (UINT32)V5_REG_LCDC_B1_UPD_SIZE;
		break;
	case VM_LCDC_LAYER_BG:
		posaddr	= (UINT32)V5_REG_LCDC_BG_UPD_POS;
		sizeaddr= (UINT32)V5_REG_LCDC_BG_UPD_SIZE;
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	posval	|= (UINT32)((startpt.x&0x3ff)<<16);
	posval	|= (UINT32)(startpt.y&0x3ff);
	sizeval	|= (UINT32)(((winsize.cx&0x3ff)-1)<<16);
	sizeval	|= (UINT32)((winsize.cy&0x3ff)-1);
	VIM_HIF_SetReg16(posaddr	, posval);
	VIM_HIF_SetReg16(sizeaddr	, sizeval);

	return VIM_SUCCEED;
}
#endif
/*-----------------------------------------------------------------------------
  Summary:
	Get B layer size
  Parameters:
	BLayer :	select B0 / B1 / all layer
	pTSize :	point of B layer memory size
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_GetBSize(VIM_DISP_LAYER BLayer,PTSize pTSize)
{
	if((BLayer !=VIM_DISP_B0LAYER)&&( BLayer != VIM_DISP_B1LAYER))
		return VIM_ERROR_DISP_BLAYER;	

	if(BLayer & VIM_DISP_B0LAYER)
	{
		pTSize->cx = VIM_HIF_GetReg16(V5_REG_LCDC_B0_SIZE+2);
		pTSize->cy = VIM_HIF_GetReg16(V5_REG_LCDC_B0_SIZE);
	}
	else if(BLayer & VIM_DISP_B1LAYER)
	{
		pTSize->cx = VIM_HIF_GetReg16(V5_REG_LCDC_B1_SIZE+2);
		pTSize->cy = VIM_HIF_GetReg16(V5_REG_LCDC_B1_SIZE);
	}

	if(pTSize->cx)
	{
		pTSize->cx++;
	}
	if(pTSize->cy)
	{
		pTSize->cy++;
	}

	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 get start point of specified layer.
		
Parameters:
		layer	: One of the following <U>VM_LCDC_LAYER</U>
 				enumeration values that specify lcdc layer.
 				</VM_LCDC_LAYER_A0>
 				</VM_LCDC_LAYER_B0>
 				</VM_LCDC_LAYER_B1>
 		pstartpt: address to store start point.
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
#if 0
VIM_RESULT VIM_DISP_GetStartPoint(VIM_DISP_LAYER layer, PTPoint pstartpt)
{
	UINT32	regval = 0;
	
	switch(layer)
	{
	case VIM_DISP_ALAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_A0_LCD_POS);
		break;
	case VIM_DISP_B0LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B0_LCD_POS);
		break;
	case VIM_DISP_B1LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B1_LCD_POS);
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	pstartpt->x = (UINT16)((regval>>16)&0x3ff);
	pstartpt->y = (UINT16)(regval&0x3ff);
		
	return VIM_SUCCEED;
}
#endif

/***********************************************************************************
Description:
		 specify the background color of lcdc.
		
Parameters:
		mode	: One of the following <U>VM_LCDC_BGCOLORFORMAT</U>
 				enumeration values that specify backgroun color formt.
  		color	: specified color(rgb24 or rgb 32).
		
Returns:
		success	: VIM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_DISP_UpdateBackgroud(VIM_DISP_BGCOLORFORMAT format, UINT32 color, UINT32 x, UINT32 y, UINT32 width, UINT32 height)
{
	UINT32	addr = 0;
	UINT32	val = 0;

	if( width )
		width  -= 0x01;
	if( height )
		height -= 0x01;

	val = (( x << 16 ) | y ) & 0x03ff03ff;
	VIM_HIF_SetReg32( V5_REG_LCDC_BG_UPD_POS, val );
	val = (( width << 16 ) | height ) & 0x03ff03ff;
	VIM_HIF_SetReg32( V5_REG_LCDC_BG_UPD_SIZE, val );
	
	if(format == VM_LCDC_BGCOLOR_RGB24){
		addr = (UINT32)V5_REG_LCDC_BGD;
		color &= 0xffffff;
	}else if(format == VM_LCDC_BGCOLOR_RGB32){
		addr = (UINT32)V5_REG_LCDC_PALETTE;
	}else{
		return VM_ERR_LCDC_BGCOLORFMT_UNKNOWN;
	}
	VIM_HIF_SetReg32(addr,	color);

	val = VIM_HIF_GetReg32( V5_REG_LCDC_UPDATE );
	VIM_HIF_SetReg32( V5_REG_LCDC_UPDATE, ( val | BIT4 ));
	
	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
  Summary:
	Get B layer and A layer start point
  Parameters:
	BLayer :	select B0 / B1/A0
	pTSize :	point of B layer/A layer memory size
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
VIM_RESULT VIM_DISP_GetBStartPoint(VIM_DISP_LAYER BLayer,PTPoint pStartPoint)
{
	UINT32 regval=0 ;

	if(BLayer > VIM_DISP_B1LAYER || BLayer < VIM_DISP_B0LAYER)
		return VIM_ERROR_DISP_BLAYER;	

	
	switch(BLayer)
	{
	case VIM_DISP_ALAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_A0_LCD_POS);
		break;
	case VIM_DISP_B0LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B0_LCD_POS);
		break;
	case VIM_DISP_B1LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B1_LCD_POS);
		break;
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	pStartPoint->x = (UINT16)((regval>>16)&0x3ff);
	pStartPoint->y = (UINT16)(regval&0x3ff);
		
	return VIM_SUCCEED;
}


/*-----------------------------------------------------------------------------
  Summary:
	set enable lcdc RGB 565 mode
  Parameters:
	bEnable :enable or not
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
#if 0
void VIM_DISP_EnableVrgbMode(UINT8 bEnable)
{
	UINT8 byMode;
	byMode = VIM_HIF_GetReg8(V5_REG_LCDC_DE);
	byMode &= 0xfe;
	byMode |= (bEnable & 0x01);
	VIM_HIF_SetReg8(V5_REG_LCDC_DE, byMode);
}
#endif
/*-----------------------------------------------------------------------------
  Summary:
	set enable lcdc RGB 565 mode
  Parameters:
	bEnable :enable or not
  Note:
  Returns: 
  Remarks:
--------------------------------------------------------------------------------*/
#if 0
void VIM_DISP_EnableDecodeSpecial(UINT8 bEnable)
{

	UINT8 byMode;
	byMode = VIM_HIF_GetReg8(V5_REG_LCDC_BR1_H);
	byMode &= 0x1f;
	byMode |= ((bEnable & 0x01)<<5);
	VIM_HIF_SetReg8(V5_REG_LCDC_BR1_H, byMode);

}
#endif

/********************************************************************************
  Description:
  	interrupt of disp
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/
static const UINT8 gDispIntPriority[VIM_DISP_INT_NUM]	= 
{ 
	VIM_INT_DISP_A0_DONE, VIM_INT_DISP_A1_DONE, VIM_INT_DISP_A0_START, VIM_INT_DISP_A0_BEF,VIM_INT_DISP_B0_UPDATE,
	VIM_INT_DISP_B1_UPDATE, VIM_INT_DISP_BG_UPDATE, VIM_INT_DISP_FIFO
};

void _ISR_DispIntHandle(UINT8 wIntFlag)
{
	UINT8 i = 0;
	UINT8 bySecondLevelInt = 0;

	for(i=0; i<8; i++)
	{
		bySecondLevelInt = gDispIntPriority[i];
		if(TSTINT(wIntFlag, bySecondLevelInt))
		{
			if(gVc0578b_Isr.Dispisr[bySecondLevelInt])
			{
				gVc0578b_Isr.Dispisr[bySecondLevelInt]();
			}
		}
	}
}


/********************************************************************************
  Description:
  	register isr 
  Parameters:

  	bySecondLevelInt:
		VIM_INT_DISP_B0_UPDATE		= 0,
		VIM_INT_DISP_B1_UPDATE		= 1, //ignore
		VIM_INT_DISP_BG_UPDATE			= 2,
		VIM_INT_DISP_FIFO			= 3,
		VIM_INT_DISP_A0_DONE		= 4,
		VIM_INT_DISP_A1_DONE	= 5, //ignore
		VIM_INT_DISP_A0_START		= 6,
		VIM_INT_DISP_A0_BEF			= 7,
		VIM_INT_DISP_ALL			= VIM_DISP_INT_NUM
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_DISP_RegisterISR(VIM_DISP_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr)
{
UINT8 i;
	if(bySecondLevelInt>=VIM_INT_DISP_ALL)
	{
		for(i=0;i<VIM_INT_DISP_ALL;i++)
			gVc0578b_Isr.Dispisr[i] = Isr;
		return;
	}
	gVc0578b_Isr.Dispisr[bySecondLevelInt] = Isr;
}

void VIM_DISP_B0_Done_isr(void)
{
	g_b0LayerUpdateFinished=1;
//	MMDPrintf("b0\n");
}
void VIM_DISP_B1_Done_isr(void)
{
g_b1LayerUpdateFinished=1;
//	MMDPrintf("b1\n");
}
/********************************************************************************
  Description:
  	set enable DISP interrupt
  Parameters:

  	bySecondLevelInt:
		VIM_INT_DISP_B0_UPDATE		= 0,
		VIM_INT_DISP_B1_UPDATE		= 1, //ignore
		VIM_INT_DISP_BG_UPDATE			= 2,
		VIM_INT_DISP_FIFO			= 3,
		VIM_INT_DISP_A0_DONE		= 4,
		VIM_INT_DISP_A1_DONE	= 5, //ignore
		VIM_INT_DISP_A0_START		= 6,
		VIM_INT_DISP_A0_BEF			= 7,
		VIM_INT_DISP_ALL			= VIM_DISP_INT_NUM
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_DISP_SetIntEnable(VIM_DISP_INT_TYPE bySecondLevelInt,BOOL Eanble)
{
	UINT8 bInten;
	if(bySecondLevelInt<VIM_INT_DISP_ALL)
	{
		bInten=VIM_HIF_GetIntEnableSec(VM_HIF_INT_LCDC);
		if(Eanble==ENABLE)
			bInten |= (0x1<<bySecondLevelInt);
		else
			bInten &= (~(0x1<<bySecondLevelInt));	
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_LCDC,bInten);
		if(bInten)
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_LCDC,ENABLE);
//		else
		//	VIM_HIF_SetIntModuleEn(VM_HIF_INT_LCDC,DISABLE);
	}
	else
	{
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_LCDC,0xff);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_LCDC,ENABLE);
	}
}

VIM_RESULT VIM_DISP_SetRGBgammaValue(void)              //guoying 12/19/2008

{

/*
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R0123,0x0f0a0400);        
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R4567,0x2d231914);            
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R891011,0x554b4137);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R12131415,0xa0786c5f);    
*/
 
   /* VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G0123,0x0f0a0400);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G4567,0x2d231914);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G891011,0x554b4137);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G12131415,0xa0786c5f);    

    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B0123,0x0f0a0400);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B4567,0x2d231914);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B891011,0x554b4137);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B12131415,0xa0786c5f);    */


//背景非常暗,人脸很红,并且发黑!,衣服头发基本融在背景黑色中!
/*    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R0123,0x12080400);        
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R4567,0xb9a5823c);            
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R891011,0xeee6d9cd);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R12131415,0xfdfcfaf6);    
   
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G0123,0x12080400);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G4567,0xb9a5823c);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G891011,0xeee6d9cd);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G12131415,0xfdfcfaf6);    

    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B0123,0x12080400);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B4567,0xb9a5823c);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B891011,0xeee6d9cd);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B12131415,0xfdfcfaf6);
 
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R16G16B16,0x00ffffff);    
*/

    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R0123,0x16090400);        
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R4567,0xb09b783c);            
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R891011,0xe9e0d2c4);
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R12131415,0xfdfaf4f0);    
   
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G0123,0x16090400);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G4567,0xb09b783c);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G891011,0xe9e0d2c4);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_G12131415,0xfdfaf4f0);    

    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B0123,0x16090400);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B4567,0xb09b783c);    
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B891011,0xe9e0d2c4);  
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_B12131415,0xfdfaf4f0);
 
    VIM_HIF_SetReg32(V5_REG_LCDC_GLUT_R16G16B16,0x00ffffff);    
     
    return VIM_SUCCEED;

}

/*-----------------------------------------------------------------------------

Description:
         enable/disable lcdc RGB gamma function.
         
Parameters:
        enable  : 1 for enable, 0 for disable.
        
Returns:
        void.

Remarks:

--------------------------------------------------------------------------------*/

VIM_RESULT VIM_DISP_SetRGBgammaEnable(BOOL Enable)      //guoying 12/19/2008

{

    UINT16 value16 = 0;

    value16 = VIM_HIF_GetReg16(V5_REG_LCDC_CTRL);
    value16 &= ~(BIT0);             //RGB gamma enable bit, wendy guo
    value16 |= (Enable);                

    VIM_HIF_SetReg16(V5_REG_LCDC_CTRL,value16);

    return VIM_SUCCEED;

}
