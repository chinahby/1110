/**
 * @file A8_win.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "AIT700_ebibus.h"
#include "A8_common.h"
#include "A8_win.h"
#include "A8_graphics.h"
#include "A8_panel.h"
#include "A8_dscapi.h"

extern	StructBUF		gsBuffer[MAX_BUFFER];

WINDOW_INFO	window_info[4];
extern StructBUF	gsBuffer[MAX_BUFFER];

//-------------------------------------------------------------------------//
// The variables are defined in A8_panel.c for main/sub LCD dimension
//Vin@20091219: Select Panel Size in same lib.
extern unsigned short gA8MainLCDWidth;
extern unsigned short gA8MainLCDHeight;
extern unsigned short gA8SubLCDWidth;
extern unsigned short gA8SubLCDHeight;
//-------------------------------------------------------------------------//


/**
 @fn		u_short	A8L_SetWindowActive(u_short windowID, u_short winActive)
 @brief	Set the window active attribute for future operation.
 @param	windowID - the ID of the window for setting.
 @param	winActive - Set Active Attribute.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetWindowActive(u_short windowID, u_short winActive)
{
	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
		case A8_GAME_WINDOW:
			if (winActive)
				SetA8RegB(DISP_GENERAL_CONTROL2_RG, 
					GetA8RegB(DISP_GENERAL_CONTROL2_RG) | (0x01 << windowID));
			else
				SetA8RegB(DISP_GENERAL_CONTROL2_RG, 
					GetA8RegB(DISP_GENERAL_CONTROL2_RG) & (~(0x01 << windowID)));
			return WIN_NO_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}	
}


/**
 @fn		u_short	A8L_SetWindowTransparency(u_short windowID, u_short transpActive, u_int transpColor)
 @brief	Set the window active attribute and transparency color for future operation.
 @param	windowID - the ID of the window for setting.
 @param	transpActive - Set Active Attribute.
 @param	transpColor - Set the transparency color.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetWindowTransparency(u_short windowID, u_short transpActive, u_int transpColor)
{
	u_short	offset, temp;
	u_int	longtmp;

	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
		case A8_GAME_WINDOW:
			offset = windowID * 0x20;
			if (transpActive) {
				temp = GetA8RegB(MAIN_WIN_TP_SET_RG + offset);
				SetA8RegB(MAIN_WIN_TP_SET_RG + offset, temp | 0x01);
				if (window_info[windowID].colordepth == A8_DEPTH_8B) {
					longtmp = (transpColor & 0xff);
				}
				else if (window_info[windowID].colordepth == A8_DEPTH_16B) {	
					longtmp = (transpColor & 0xffff);
				}
				else if (window_info[windowID].colordepth == A8_DEPTH_24B) {		
					longtmp = transpColor & 0xffffff;
				}	
				else if (window_info[windowID].colordepth == A8_YUV_422) {		
					longtmp = transpColor & 0xffffff;
				}	
				else if (window_info[windowID].colordepth == A8_DEPTH_4I) {
					longtmp = (transpColor & 0x0f);
				}
				else 
					return WIN_PARAS_ERROR;;

				SetA8RegW(MAIN_WIN_TP_COLOR_B_RG + offset, (u_short)(longtmp & 0xFFFF));
				SetA8RegB(MAIN_WIN_TP_COLOR_R_RG + offset, (u_char) (longtmp >> 16));
			}
			else 
				SetA8RegB(MAIN_WIN_TP_SET_RG + offset, 0x00);
			return WIN_NO_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}	
}


/**
 @fn		u_short A8L_SetWindowSemiTransparency(u_short windowID, u_short SemiTranspActive, u_int semiTransColor, u_short semiFunx, u_short semiWeight)
 @brief	Set the window active attribute and semi transparency for future operation.
 @param	windowID - the ID of the window for setting.
 @param	SemiTranspActive - Set Active Attribute.
 @param	semiTransColor - Set the semi-transparency color.
 @param	semiFunx - FUNX_AVERAGE, FUNX_AND, FUNX_OR, FUNX_INVERSE.
 @param	semiWeight - the weighting for the highest priority windows, if semiFunx == FUNX_AVERAGE.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short A8L_SetWindowSemiTransparency(u_short windowID, u_short SemiTranspActive, 
			u_int semiTransColor, u_short semiFunx, u_short semiWeight)
{
	u_short	offset, temp;
	u_int	longtmp;

	if (semiFunx & 0xfffc || semiWeight > 16) 
		return WIN_PARAS_ERROR;
	
	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
		case A8_GAME_WINDOW:
			offset = windowID * 0x20;
			if (SemiTranspActive) {
				temp = GetA8RegB(MAIN_WIN_TP_SET_RG + offset);
				SetA8RegB(MAIN_WIN_TP_SET_RG + offset, temp | (0x02+((semiFunx & 0x3)<<2)));
				if (window_info[windowID].colordepth == A8_DEPTH_8B) {
					longtmp = (semiTransColor & 0xff);
				}
				else if (window_info[windowID].colordepth == A8_DEPTH_16B) {	
					longtmp = (semiTransColor & 0xffff);
				}
				else if (window_info[windowID].colordepth == A8_DEPTH_24B) {		
					longtmp = semiTransColor & 0xffffff;
				}	
				else if (window_info[windowID].colordepth == A8_YUV_422) {		
					longtmp = semiTransColor & 0xffffff;
				}	
				else 
					return WIN_PARAS_ERROR;

				SetA8RegW(MAIN_WIN_TP_COLOR_B_RG + offset, (u_short)(longtmp & 0xFFFF));
				SetA8RegB(MAIN_WIN_TP_COLOR_R_RG + offset, (u_char) (longtmp >> 16));
				
				if (semiFunx == FUNX_AVERAGE)
					SetA8RegB(MAIN_WIN_SEMI_WEIGHT_RG + offset, (semiWeight & 0xff));
			}
			else 
				SetA8RegB(MAIN_WIN_TP_SET_RG + offset, 0x00);
			return WIN_NO_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}	
}


/**
 @fn		u_short	A8L_SetWindowPaletteActive(u_short windowID, u_short paletteActive)
 @brief	Set the window palette active attribute for future operation.
 @param	windowID - the ID of the window for setting.
 @param	paletteActive - Set Active Attribute.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetWindowPaletteActive(u_short windowID, u_short paletteActive)
{
	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
		case A8_GAME_WINDOW:
			if (paletteActive)
				SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
						GetA8RegB(DISP_GENERAL_CONTROL1_RG) & (~(0x10 << windowID)));
			else
				SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
						GetA8RegB(DISP_GENERAL_CONTROL1_RG) | (0x10 << windowID));
			return WIN_NO_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}	
}


/**
 @fn		u_short	A8L_SetWindowPriority(u_short firstPrio, u_short secondPrio, u_short thirdPrio, u_short fourthPrio)
 @brief	Set the window priority attribute for future operationn.
 @param	firstPrio - the first priority window.
 @param	secondPrio - the second priority window.
 @param	thirdPrio - the third priority window.
 @param	fourthPrio - the fourth priority window.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetWindowPriority(u_short firstPrio, u_short secondPrio, u_short thirdPrio, u_short fourthPrio)
{
	u_short Temp;
	
	if (firstPrio & 0xfffc || secondPrio & 0xfffc ||
		thirdPrio & 0xfffc || fourthPrio & 0xfffc) 
		return WIN_PARAS_ERROR;
		
	Temp = ((fourthPrio << 6)+ (thirdPrio << 4) + (secondPrio << 2) + firstPrio);	
	SetA8RegB(0x5043, Temp);
	//Temp = GetA8RegB(0x5043);
	/*AIT_Message_P1("A802: SetWindowPriority %x\n", Temp);*/
	
	/*
	SetA8RegB(DISP_WIN_PRIORITY_RG, (fourthPrio << 6) 
								+ (thirdPrio << 4) 
								+ (secondPrio << 2) 
								+ firstPrio);*/
	return WIN_NO_ERROR;
}


/**
 @fn		u_short	A8L_SetWindowAttribute(u_short windowID, u_short width, u_short height, u_short colorDepth, u_int baseAddr)
 @brief	Set the window basic attribute for future operation.
 @param	windowID - the ID of the window for setting.
 @param	width - the image width of the window.
 @param	height - the image height of the window.
 @param	colorDepth - bits per pixel on the image.
 @param	baseAddr - the start address in A800 memory (the content of the window must be continuous).
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetWindowAttribute(u_short windowID, u_short width, u_short height, u_short colorDepth, u_int baseAddr)
{
	u_short	offset, temp;

	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
			window_info[windowID].baseaddr = baseAddr;
			window_info[windowID].width = width;
			window_info[windowID].height = height;
			window_info[windowID].colordepth = colorDepth;
			offset = windowID * 0x20;

			// set base address
			SetA8RegW(MAIN_WIN_START_ADDR_L_RG + offset, 
				(u_short)(baseAddr & 0xFFFF));
			SetA8RegW(MAIN_WIN_START_ADDR_H_RG + offset,
				(u_char)(baseAddr >> 16));

			// set color depth
			if (windowID == A8_MAIN_WINDOW) {
				if (colorDepth == A8_DEPTH_8B)
					{
					SetA8RegB(DISP_GENERAL_CONTROL4_RG, GetA8RegB(DISP_GENERAL_CONTROL4_RG) & 0xFE );
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, GetA8RegB(DISP_GENERAL_CONTROL1_RG) & 0xFE);
					}
				else if (colorDepth == A8_DEPTH_16B)
					{
					SetA8RegB(DISP_GENERAL_CONTROL4_RG, GetA8RegB(DISP_GENERAL_CONTROL4_RG) & 0xFE );
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, GetA8RegB(DISP_GENERAL_CONTROL1_RG) | 0x01);
					}
				else if (colorDepth == A8_DEPTH_4I)
					{
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, GetA8RegB(DISP_GENERAL_CONTROL1_RG) & 0xFE);
					SetA8RegB(DISP_GENERAL_CONTROL4_RG, GetA8RegB(DISP_GENERAL_CONTROL4_RG) | 0x01 );
					}
				else
					return WIN_PARAS_ERROR;
			}
			else if (windowID == A8_PIP_WINDOW) {
				if (colorDepth == A8_DEPTH_16B) {
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) & 0xFD);
					SetA8RegB(DISP_GENERAL_CONTROL3_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL3_RG) & ~(0x01));
				}	
				else if (colorDepth == A8_DEPTH_24B) {
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) | 0x02);
					SetA8RegB(DISP_GENERAL_CONTROL3_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL3_RG) & ~(0x01));
				}			
				else if (colorDepth == A8_YUV_422) {
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) | 0x02);
					SetA8RegB(DISP_GENERAL_CONTROL3_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL3_RG) | 0x01);
				}			
				else
					return WIN_PARAS_ERROR;
			}		
			else if (windowID == A8_OVERLAY_WINDOW) {
				if (colorDepth == A8_DEPTH_16B)
					{
					temp = GetA8RegB( DISP_GENERAL_CONTROL4_RG );
					temp |= 0x80;
					SetA8RegB( DISP_GENERAL_CONTROL4_RG, temp & 0xFD );
					}
				else if (colorDepth == A8_DEPTH_8B)
					{
					temp = GetA8RegB( DISP_GENERAL_CONTROL4_RG );
					temp &= 0x7D;		// bit1 = 0, bit7 = 0;
					SetA8RegB( DISP_GENERAL_CONTROL4_RG, temp );
					}
				else if (colorDepth == A8_DEPTH_4I)
					{
					temp = GetA8RegB( DISP_GENERAL_CONTROL4_RG );
					temp |= 0x7F;
					SetA8RegB( DISP_GENERAL_CONTROL4_RG, temp | 0x02 );
					}
				else
					return WIN_PARAS_ERROR;
			}
			return WIN_NO_ERROR;
		case A8_GAME_WINDOW:
			if (colorDepth != A8_DEPTH_8B)
				return WIN_PARAS_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}
}


/**
 @fn		u_short	A8L_GetWindowAttribute(u_short windowID, WINDOW_INFO *winInfoPtr )
 @brief	Get the window basic attribute for future operation.
 @param	windowID - the ID of the window for setting.
 @param	winInfoPtr - the pointer to a WINDOW_INFO structure for storing the window attributes	.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_GetWindowAttribute(u_short windowID, WINDOW_INFO *winInfoPtr )
{
	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
			winInfoPtr->width = window_info[windowID].width;
			winInfoPtr->height = window_info[windowID].height;
			winInfoPtr->colordepth = window_info[windowID].colordepth;
			winInfoPtr->baseaddr = window_info[windowID].baseaddr;
			return WIN_NO_ERROR;
		case A8_GAME_WINDOW:
			winInfoPtr->width = 256;
			winInfoPtr->height = 256;
			winInfoPtr->colordepth = A8_DEPTH_8B;
			winInfoPtr->baseaddr = 0L;
			return WIN_NO_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}
}


/**
 @fn		u_short	A8L_BindBufferToWindow(u_short bufferID, u_short windowID)
 @brief	Set the buffer attributes to window for future operation.
 @param	bufferID - the ID of the buffer to bind with the window.
 @param	windowID - the ID of the window for setting.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_BindBufferToWindow(u_short bufferID, u_short windowID)
{
	u_short	offset, temp;
	u_short	bpp;

	if (gsBuffer[bufferID].colordepth == A8_YUV_422)
		bpp = 16;
	else if (gsBuffer[bufferID].colordepth == A8_DEPTH_8B)
		bpp = 8;
	else if (gsBuffer[bufferID].colordepth == A8_DEPTH_16B)
		bpp = 16;
	else if (gsBuffer[bufferID].colordepth == A8_DEPTH_24B)
		bpp = 24;
	else if (gsBuffer[bufferID].colordepth == A8_DEPTH_4I)
		bpp = 4;
	else
		return WIN_PARAS_ERROR;
		
	if (gsBuffer[bufferID].lineoffset != ((gsBuffer[bufferID].width * bpp)>>3))
		return WIN_PARAS_ERROR;

	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_OVERLAY_WINDOW:
			if (gsBuffer[bufferID].colordepth != A8_DEPTH_8B && gsBuffer[bufferID].colordepth != A8_DEPTH_16B
					&& gsBuffer[bufferID].colordepth != A8_DEPTH_4I )
				return WIN_PARAS_ERROR;
			break;
		case A8_PIP_WINDOW:
			if (gsBuffer[bufferID].colordepth != A8_DEPTH_16B && gsBuffer[bufferID].colordepth != A8_DEPTH_24B
					&& gsBuffer[bufferID].colordepth != A8_YUV_422)
				return WIN_PARAS_ERROR;
			break;	
		case A8_GAME_WINDOW:		
		default:
			return WIN_PARAS_ERROR;
	}	

	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
			window_info[windowID].width = gsBuffer[bufferID].width;
			window_info[windowID].height = gsBuffer[bufferID].height;
			window_info[windowID].colordepth = gsBuffer[bufferID].colordepth;
			offset = windowID * 0x20;

			// set base address
			SetA8RegW(MAIN_WIN_START_ADDR_L_RG + offset,
					(u_short)(gsBuffer[bufferID].startaddr & 0xFFFF));
			SetA8RegW(MAIN_WIN_START_ADDR_H_RG + offset,
					(u_char)(gsBuffer[bufferID].startaddr >> 16));

			// set color depth
			if (windowID == A8_MAIN_WINDOW) {
				if (gsBuffer[bufferID].colordepth == A8_DEPTH_8B)
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) & 0xFE);
				else if (gsBuffer[bufferID].colordepth == A8_DEPTH_16B)
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) | 0x01);
				else
					return WIN_PARAS_ERROR;
			}
			else if (windowID == A8_PIP_WINDOW) {
				if (gsBuffer[bufferID].colordepth == A8_DEPTH_16B) {
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) & 0xFD);
					SetA8RegB(DISP_GENERAL_CONTROL3_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL3_RG) & ~(0x01));
				}
				else if (gsBuffer[bufferID].colordepth == A8_DEPTH_24B) {
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) | 0x02);
					SetA8RegB(DISP_GENERAL_CONTROL3_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL3_RG) & ~(0x01));
				}							
				else if (gsBuffer[bufferID].colordepth == A8_YUV_422) {
					SetA8RegB(DISP_GENERAL_CONTROL1_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL1_RG) | 0x02);
					SetA8RegB(DISP_GENERAL_CONTROL3_RG, 
							GetA8RegB(DISP_GENERAL_CONTROL3_RG) | 0x01);
				}
				else				
					return WIN_PARAS_ERROR;
			}		
			else if (windowID == A8_OVERLAY_WINDOW) {
			
				if (gsBuffer[bufferID].colordepth == A8_DEPTH_16B)
					{
					temp = GetA8RegB( DISP_GENERAL_CONTROL4_RG );
					temp |= 0x80;
					SetA8RegB( DISP_GENERAL_CONTROL4_RG, temp & 0xFD );
					}
				else if (gsBuffer[bufferID].colordepth == A8_DEPTH_8B)
					{
					temp = GetA8RegB( DISP_GENERAL_CONTROL4_RG );
					temp &= 0x7D;		// bit1 = 0, bit7 = 0;
					SetA8RegB( DISP_GENERAL_CONTROL4_RG, temp );
					}
				else if (gsBuffer[bufferID].colordepth == A8_DEPTH_4I)
					{
					temp = GetA8RegB( DISP_GENERAL_CONTROL4_RG );
					temp |= 0x7F;
					SetA8RegB( DISP_GENERAL_CONTROL4_RG, temp | 0x02 );
					}
				else
					return WIN_PARAS_ERROR;
			}
			return WIN_NO_ERROR;
		case A8_GAME_WINDOW:
		default:
			return WIN_PARAS_ERROR;
	}
}


/**
 @fn		u_short	A8L_SetWindowToDisplay(u_short windowID, u_short startx, u_short starty, u_short displayWidth, u_short displayHeight, u_short displayOffsetx, u_short displayOffsety, u_short rotateType)
 @brief	Set the display attribute for the window operation.
 @param	windowID - the ID of the window for setting.
 @param	startx, starty - the start left-top position of the window for disaply.
 @param	displayWidth, displayHeight - the window size for display.
 @param	displayOffsetx, displayOffsety - the start left-top position on the display panel.
 @param	rotateType - define the rotate type of the cut window
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetWindowToDisplay(u_short windowID, u_short startx, u_short starty,
			u_short displayWidth, u_short displayHeight, u_short displayOffsetx, 
			u_short displayOffsety, u_short rotateType)
{
	u_short	offset;
	u_int	startaddr=0;
	u_short	pixelbyte=0, rowbyte=0; /* prevent Chuls 080513 initial value */
	u_short	panelwidth=0, panelheight;
	
	panelwidth = GetA8RegW( DISP_PANEL_WIDTH_L_RG );
	panelheight = GetA8RegW( DISP_PANEL_HEIGHT_L_RG );

	if ( window_info[windowID].colordepth == A8_DEPTH_8B )
		{
		pixelbyte = 1;
		rowbyte = window_info[windowID].width;
		}
	else if ( window_info[windowID].colordepth == A8_DEPTH_16B )
		{
		pixelbyte = 2;
		rowbyte = window_info[windowID].width<<1;
		}
	else if ( window_info[windowID].colordepth == A8_DEPTH_24B )
		{
		pixelbyte = 3;
		rowbyte = window_info[windowID].width * 3;
		}
	else if ( window_info[windowID].colordepth == A8_YUV_422 )
		{
		pixelbyte = 2;
		rowbyte = window_info[windowID].width<<1;
		}
	else if ( window_info[windowID].colordepth == A8_DEPTH_4I )
		{
		pixelbyte = 1;
		rowbyte = window_info[windowID].width >> 1;
		}
	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_PIP_WINDOW:
		case A8_OVERLAY_WINDOW:
			offset = windowID * 0x20;	
			if (startx > window_info[windowID].width ||
				starty > window_info[windowID].height ||
				displayOffsetx > panelwidth || 
				displayOffsety > panelheight || 
				(displayOffsetx + displayWidth) > panelwidth ||
				(displayOffsety + displayHeight) > panelheight)
				return WIN_PARAS_OUT_RANGE; 					

			switch (rotateType & 0x03) {
				case A8_RT_NORMAL:
					if ((startx + displayWidth) > window_info[windowID].width || 
						(starty + displayHeight) > window_info[windowID].height)
						return WIN_PARAS_OUT_RANGE; 					

					SetA8RegW(MAIN_WIN_ROW_OFF_L_RG + offset, rowbyte);
					if (rotateType & 0x04) {	// with mirror
						SetA8RegW(MAIN_WIN_PIXEL_OFF_L_RG + offset, 0x8000 | pixelbyte);
						startaddr = starty * rowbyte + (startx + displayWidth - 1) * pixelbyte;
					}	
					else {
						SetA8RegW(MAIN_WIN_PIXEL_OFF_L_RG + offset, pixelbyte);
						startaddr = starty * rowbyte + startx * pixelbyte;
					}	
					break;					
				case A8_RT_RIGHT_90:
					if ((startx + displayHeight) > window_info[windowID].width || 
						(starty + displayWidth) > window_info[windowID].height)
						return WIN_PARAS_OUT_RANGE; 					

					SetA8RegW(MAIN_WIN_PIXEL_OFF_L_RG + offset,	0x8000 | rowbyte);
					if (rotateType & 0x04) {	// with mirror
						SetA8RegW(MAIN_WIN_ROW_OFF_L_RG + offset, 0x8000 | pixelbyte);
						startaddr = (starty + displayWidth - 1)* rowbyte + (startx + displayHeight - 1) * pixelbyte;
					}	
					else {
						SetA8RegW(MAIN_WIN_ROW_OFF_L_RG + offset, pixelbyte);
						startaddr = (starty + displayWidth - 1)* rowbyte + startx * pixelbyte;
					}	
					break;					
				case A8_RT_RIGHT_180:
					if ((startx + displayWidth) > window_info[windowID].width || 
						(starty + displayHeight) > window_info[windowID].height)
						return WIN_PARAS_OUT_RANGE; 					

					SetA8RegW(MAIN_WIN_ROW_OFF_L_RG + offset, 0x8000 | rowbyte);
					if (rotateType & 0x04) {
						SetA8RegW(MAIN_WIN_PIXEL_OFF_L_RG + offset, pixelbyte);
						startaddr = (starty + displayHeight - 1) * rowbyte + startx * pixelbyte;
					}
					else {
						SetA8RegW(MAIN_WIN_PIXEL_OFF_L_RG + offset, 0x8000 | pixelbyte);
						startaddr = (starty + displayHeight - 1) * rowbyte + (startx + displayWidth - 1) * pixelbyte;
					}	
					break;					
				case A8_RT_RIGHT_270:
					if ((startx + displayHeight) > window_info[windowID].width || 
						(starty + displayWidth) > window_info[windowID].height)
						return WIN_PARAS_OUT_RANGE; 					

					SetA8RegW(MAIN_WIN_PIXEL_OFF_L_RG + offset, rowbyte);
					if (rotateType & 0x04) {
						SetA8RegW(MAIN_WIN_ROW_OFF_L_RG + offset, pixelbyte);
						startaddr = starty * rowbyte + startx * pixelbyte;
					}
					else {		
						SetA8RegW(MAIN_WIN_ROW_OFF_L_RG + offset, 0x8000 | pixelbyte);
						startaddr = starty * rowbyte + (startx + displayHeight -1) * pixelbyte;
					}	
					break;					
			}	
				
			SetA8RegW(MAIN_WIN_START_OFF_L_RG + offset,	(u_short)(startaddr & 0xFFFF));
			SetA8RegW(MAIN_WIN_START_OFF_H_RG + offset,	(u_short)(startaddr  >> 16));

			SetA8RegW(MAIN_WIN_LCD_X_L_RG + offset, displayOffsetx);
			SetA8RegW(MAIN_WIN_LCD_Y_L_RG + offset, displayOffsety);

			SetA8RegW(MAIN_WIN_WIDTH_L_RG + offset, displayWidth);
			SetA8RegW(MAIN_WIN_HEIGHT_L_RG + offset, displayHeight);

			return WIN_NO_ERROR;
		case A8_GAME_WINDOW:
		default:
			return WIN_PARAS_ERROR;		
	}
}


/**
 @fn		u_short	A8L_LoadWindowPalette(u_short windowID, u_char *palette, u_short entry)
 @brief	Setup window pallette.
 @param	windowID - the ID of the window for setting.
 @param	palette: palette data.
 @param	entry: define the number of palette.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_LoadWindowPalette(u_short windowID, u_char *palette, u_short entry)
{
	u_short	i;
	
	switch (windowID) {
		case A8_MAIN_WINDOW:
		case A8_OVERLAY_WINDOW:
			if (window_info[windowID].colordepth == A8_DEPTH_8B || window_info[windowID].colordepth == A8_DEPTH_4I)
			{
				for (i = 0; i < entry; i++) {
					SetA8RegW(OVERLAY_PALETTE_R0_RG+2*i, (u_short)palette[4*i]);
					SetA8RegW(OVERLAY_PALETTE_G0_RG+2*i, (u_short)palette[4*i+1]);
					SetA8RegW(OVERLAY_PALETTE_B0_RG+2*i, (u_short)palette[4*i+2]);
				}
				return WIN_NO_ERROR;
			}
			else 
				return WIN_PARAS_ERROR;
		case A8_PIP_WINDOW:
			return WIN_PARAS_ERROR;
		case A8_GAME_WINDOW:	// Game	for icon
			if (window_info[windowID].colordepth == A8_DEPTH_8B) {
				for (i = 0; i < entry; i++) {
					SetA8RegW(GAME_PALETTE_R0_RG+2*i, (u_short)palette[4*i]);
					SetA8RegW(GAME_PALETTE_R0_RG+2*i, (u_short)palette[4*i+1]);
					SetA8RegW(GAME_PALETTE_R0_RG+2*i, (u_short)palette[4*i+2]);
				}
				return WIN_NO_ERROR;
			}
			else return WIN_PARAS_ERROR;
		default:
			return WIN_PARAS_ERROR;
	}
}


/**
 @fn		u_int	A8L_SetWindowBackground(u_int backColor)
 @brief	Set the window background color.
 @param	backColor - the backgounnd color of the display.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_int	A8L_SetWindowBackground(u_int backColor)
{
	
	u_int	oldcolor;
	
	oldcolor = GetA8RegW(WIN_BACKGROUND_COLOR_R_RG);
	oldcolor += ((GetA8RegB(WIN_BACKGROUND_COLOR_B_RG)) << 16);
		
	SetA8RegW(WIN_BACKGROUND_COLOR_R_RG, (u_short)(backColor)); 
	SetA8RegB(WIN_BACKGROUND_COLOR_B_RG, (u_char)(backColor >> 16)); 
	 
	return oldcolor;
}


/**
 @fn		u_short	A8L_SetIconAttribute( u_int IconAddr, u_short width, u_short height, u_short x0, u_short y0, u_short ColorDepth, u_short LPF )
 @brief	Set the icon attribute.
 @param	IconAddr - the buffer address for the icon image.
 @param	width ... the icon width
 @param	height ... the icon height	
 @param	x0 ... the icon H position shown on LCD
 @param	y0 ... the icon V position shown on LCD
 @param	ColorDepth - bits per pixel on the image.
 @param	LPF - 
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetIconAttribute( u_int IconAddr, u_short width, u_short height, u_short x0, u_short y0, u_short ColorDepth, u_short LPF )
{
	SetA8RegW( 0x6C00, (u_short) (IconAddr & 0xFFFF ));
	SetA8RegW( 0x6C02, (u_short)(IconAddr >> 16 ));
	SetA8RegW( 0x6C0C, 0x00 );
	SetA8RegW( 0x6C0E, 0x00 );
	SetA8RegW( 0x6C08, width );
	SetA8RegW( 0x6C0A, height );
	if ( ColorDepth == A8_DEPTH_16B )
		SetA8RegB( 0x6C1D, 0x02 );
	else
		SetA8RegB( 0x6C1D, 0x01 );
	
	SetA8RegB( 0x6CF1, LPF & 0x03 );
	
	SetA8RegW( 0x50B6, x0 );
	SetA8RegW( 0x50B8, y0 );
	SetA8RegW( 0x50C0, width );
	SetA8RegW( 0x50C2, height );

	return A8_NO_ERROR;
}	


/**
 @fn		u_short	A8L_SetIconTransparency( u_short IsActive, u_int TransparentColor )
 @brief	Set the icon transparency.
 @param	IsActive - 0:Disable, 1:Active.
 @param	TransparentColor - Set the transparency color.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetIconTransparency( u_short IsActive, u_int TransparentColor )
{
	u_short Temp;
	
	Temp = GetA8RegB( 0x6C1C );
	if ( IsActive != 0 )
		SetA8RegB( 0x6C1C, Temp | 0x01 );
	else
		SetA8RegB( 0x6C1C, Temp & 0xFE );
	SetA8RegW( 0x6C10, (u_short) TransparentColor );
	return A8_NO_ERROR;
}	


/**
 @fn		u_short	A8L_SetIconSemiTransparency( u_short IsActive, u_short IconWeight, u_short SourceWeight )
 @brief	Set the icon semi-transparency.
 @param	IsActive - 0:Disable, 1:Active.
 @param	IconWeight - Weighting ratio of the icon.
 @param	SourceWeight - Weighting ratio of the image overlapped with the icon.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetIconSemiTransparency( u_short IsActive, u_short IconWeight, u_short SourceWeight )
{
	u_short Temp;
	
	Temp = GetA8RegB( 0x6C1C );
	if ( IsActive != 0 )
		SetA8RegB( 0x6C1C, Temp | 0x02 );
	else
		SetA8RegB( 0x6C1C, Temp & 0xFD );
		
	SetA8RegB( 0x6C14, IconWeight );
	SetA8RegB( 0x6C18, SourceWeight );
	return A8_NO_ERROR;
}	


/**
 @fn		u_short	A8L_SetIconActive( u_short IsActive, u_short Mode )
 @brief	Set the icon active or disable.
 @param	IsActive - 0:Disable, 1:Active.
 @param	Mode - 0:for JPEG/video, 1:for LCD display.
 @return	u_short - 0:success, others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetIconActive( u_short IsActive, u_short Mode )
{
	u_short Temp;

	Temp = GetA8RegB( 0x6CF0 );
	if ( Mode == 0 )
		Temp = Temp & 0xFB;		// bit 2 = 0;
	else
		Temp = Temp | 0x04;		// bit 2 = 1;
	
	if ( IsActive != 0 )
	{
		SetA8RegB( 0x6CF0, Temp | 0x01 );
		Temp = GetA8RegB( 0x504B );
		SetA8RegB( 0x504B, Temp | 0x04 );
	}
	else				
	{
		SetA8RegB( 0x6CF0, Temp & 0xFE );
		Temp = GetA8RegB( 0x504B );
		SetA8RegB( 0x504B, Temp & 0xFB );
	}
	return A8_NO_ERROR;
}


void A8L_InitialAlphaBlendingLUT(void)
{
#if 1
   SetA8RegB(0x50d4, 0x10 );
   SetA8RegB(0x50d5, 0x32 );
   SetA8RegB(0x50d6, 0x54 );
   SetA8RegB(0x50d7, 0x86 );
#else
   SetA8RegB(0x50d4, 0x78 );
   SetA8RegB(0x50d5, 0x56 );
   SetA8RegB(0x50d6, 0x34 );
   SetA8RegB(0x50d7, 0x02 );
#endif
}

void A8L_InitialAlphaBlendingLUT_1(void)
{
#if 0
   SetA8RegB(0x50d4, 0x02 );
   SetA8RegB(0x50d5, 0x34 );
   SetA8RegB(0x50d6, 0x56 );
   SetA8RegB(0x50d7, 0x78 );
#else
   SetA8RegB(0x50d4, 0x68 );
   SetA8RegB(0x50d5, 0x45 );
   SetA8RegB(0x50d6, 0x23 );
   SetA8RegB(0x50d7, 0x01 );
#endif
}

s_short A8L_SetWindowAlphaBlendingActive( u_short WinID, u_short IsActive )
{
   u_short Reg = 0x0000; /* prevent Chuls 080513 initial value */
   
   if ( WinID == A8_MAIN_WINDOW )
      Reg = 0x50d0;
   else if ( WinID == A8_PIP_WINDOW )
      Reg = 0x50d1;
   else if ( WinID == A8_OVERLAY_WINDOW )
      Reg = 0x50d2;
      
   if ( IsActive )
      SetA8RegB(Reg, GetA8RegB( Reg ) | 0x03 );
   else
      SetA8RegB(Reg, GetA8RegB( Reg ) & 0xFC );
   return A8_NO_ERROR;
}

void A800_InitLCDLookUpTable(void)
{
	u_int i;
	for(i=0;i<0xFF;i++){
		SetA8RegB(OVERLAY_PALETTE_R0_RG+i*2,(i&0xe0));
		SetA8RegB(OVERLAY_PALETTE_G0_RG+i*2,(i&0x1c)<<3);
		SetA8RegB(OVERLAY_PALETTE_B0_RG+i*2,(i&0x03)<<6);
	}	
		SetA8RegB(OVERLAY_PALETTE_B0_RG+i*2,0xff);
		SetA8RegB(OVERLAY_PALETTE_G0_RG+i*2,0xff);
		SetA8RegB(OVERLAY_PALETTE_R0_RG+i*2,0xff);	
}
