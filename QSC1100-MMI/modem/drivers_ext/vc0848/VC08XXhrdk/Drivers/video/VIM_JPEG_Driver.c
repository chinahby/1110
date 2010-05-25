/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_JPEG_Driver.c
 * [Description]        : 适用于578b.和jpeg相关的函数   
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-19 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-24 		angela      		 Created
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

#define V5_JPEG_JPEG_INT_NUM 3 //maybe 16 in future
#define V5_JPEG_LBUF_INT_NUM 1 //maybe 16 in future

#define DIV_ROUND(x, y)  ( (x + y - 1) / y )
#define ZERO_ROUND(x)  ( x = x ? x : 1 )

#define HABLAN_MAX 0x3FF

static const UINT8 gHT0[] = 
{
	0x0, 0x1, 0x5, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb
};
static const UINT8 gHT1[] = 
{
	0x0, 0x2, 0x1, 0x3, 0x3, 0x2, 0x4, 0x3, 0x5, 0x5, 0x4, 0x4, 0x0, 0x0, 0x1, 0x7d,
	0x1, 0x2, 0x3, 0x0, 0x4, 0x11, 0x5, 0x12, 0x21, 0x31, 0x41, 0x6, 0x13, 0x51, 0x61,
	0x7, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x8, 0x23, 0x42, 0xb1, 0xc1, 0x15,
	0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x9, 0xa, 0x16, 0x17, 0x18, 0x19,
	0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a,
	0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
	0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93,
	0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
	0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4,
	0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9,
	0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3,
	0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};
static const UINT8 gHT2[] = 
{
	0x0, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb
};
static const UINT8 gHT3[] = 
{
	0x0, 0x2, 0x1, 0x2, 0x4, 0x4, 0x3, 0x4, 0x7, 0x5, 0x4, 0x4, 0x0, 0x1, 0x2, 0x77,
	0x0, 0x1, 0x2, 0x3, 0x11, 0x4, 0x5, 0x21, 0x31, 0x6, 0x12, 0x41, 0x51, 0x7, 0x61, 
	0x71, 0x13, 0x22, 0x32, 0x81, 0x8, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x9, 0x23, 
	0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0xa, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 
	0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 
	0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 
	0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a,
	0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
	0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 
	0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 
	0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 
	0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};
	
static const UINT8 gPixel[] = {1, 1, 3, 3, 7, 7, 7, 7, 0xb, 0xb, 0xb, 0xb, 0xf, 0xf, 0xf, 0xf,
	0x11,0x11,0x13,0x13,0x17,0x17,0x17,0x17,0x1b,0x1b,0x1b,0x1b,0x1f,0x1f,0x1f,0x1f};

//Jpeg Parse

//////////////////////////////tools start//////////////////////////////
static UINT8 VIM_JPEG_GetComponent(UINT8 fmt, UINT16 *comp)
{
	UINT8 compnum = 0x23;

	comp[0] = 0x120b;//0x0b12;
	comp[1] = 0x5104;//0x0451;
	comp[2] = 0x5104;//0x0451;
	switch(fmt)
	{
	case JPEG_422:
		break;
	case JPEG_420:
		comp[0] = 0x2213;//0x1322;
		break;
	case JPEG_411:
		comp[0] = 0x1413;//0x1314;
		break;
	case JPEG_400:
		compnum = 0x11;
		comp[0] =0x1107;// 0x0711;
		comp[1] = 0;
		comp[2] = 0;
		break;
	case JPEG_444:
		compnum = 0x11;
		comp[0] = 0x1107;//0x0711;
		break;
	default:
		break;
	}
	return compnum;
}

void VIM_JPEG_GetJpegSize(VIM_JPEG_YUVMODE fmt, TSize size, TSize *size1)
{
	switch(fmt)
	{
	case JPEG_422:
		size1->cx = ((size.cx + 15) >> 4) << 4;
		size1->cy = ((size.cy + 7) >> 3) << 3;
		break;
	case JPEG_420:
		size1->cx = ((size.cx + 15) >> 4) << 4;
		size1->cy = ((size.cy + 15) >> 4) << 4;
		break;
	case JPEG_411:
		size1->cx = ((size.cx + 31) >> 5) << 5;
		size1->cy = ((size.cy + 7) >> 3) << 3;
		break;
	case JPEG_400:
	case JPEG_444:
		size1->cx = ((size.cx + 7) >> 3) << 3;
		size1->cy = ((size.cy + 7) >> 3) << 3;
		break;
	default:
		break;
	}
}
UINT32	VIM_JPEG_JpegCalBlockNum(VIM_JPEG_YUVMODE fmt,TSize size)
{
	UINT32  blocknum=0;
	switch(fmt)
	{
		case 	JPEG_422:
			blocknum = size.cx/4;
			break;
		case 	JPEG_420:
			blocknum = (size.cx*6)/16;
			break;
		case 	JPEG_411:
			blocknum = (size.cx*6)/32;
			break;
		case 	JPEG_400:
			blocknum = size.cx/8;
			break;
		case 	JPEG_444:
			blocknum = size.cx*3/8;
			break;
		default:
			blocknum = 0;
			break;
	}
	return blocknum;
}
static UINT16 VIM_JPEG_GetJpegBlockNum(VIM_JPEG_YUVMODE fmt, TSize size)
{
	TSize size1;

	VIM_JPEG_GetJpegSize(fmt, size, &size1);
	switch(fmt)
	{
	case JPEG_422:
		size1.cx >>= 2;
		break;
	case JPEG_420:
		size1.cx >>= 2;
		size1.cx += size1.cx >> 1;
		break;
	case JPEG_411:
		size1.cx >>= 3;
		size1.cx += size1.cx >> 1;
		break;
	case JPEG_400:
		size1.cx >>= 3;
		break;
	case JPEG_444:
		size1.cx >>= 2;
		size1.cx += size1.cx >> 1;
		break;
	default:
		break;
	}
	return size1.cx;
}
static UINT32 VIM_JPEG_CalCapDelay(UINT8 fmt, TSize JpegSize)
{
UINT32	dlycnt;

	switch(fmt)
	{
	case JPEG_420:
		dlycnt=JpegSize.cx/16*3;
		dlycnt+=JpegSize.cx*16;
		dlycnt+=JpegSize.cx*8;
		break;
	case JPEG_411:
		dlycnt=JpegSize.cx/32*3;
		dlycnt+=JpegSize.cx*4;
		dlycnt+=JpegSize.cx*8;
		break;
	case JPEG_400:
		dlycnt=JpegSize.cx/64*3;
		dlycnt+=JpegSize.cx*8;
		dlycnt+=9;
		break;
	default: //JPEG_422:
		dlycnt=JpegSize.cx/32*3;
		dlycnt+=JpegSize.cx*16;
		//dlycnt*=4;
		break;
	}
	return (UINT32)dlycnt;
}

UINT32 VIM_JPEG_GetYuvSize(VIM_JPEG_YUVMODE yuvfmt, TSize size)
{
	UINT32 len;
	TSize size1;
	//VIM_JPEG_GetJpegSize(yuvfmt, size, &size1);
	size1.cx=size.cx;
	size1.cy=size.cy;
	len = (UINT32)size1.cx & 0xffff;
	len *= (UINT32)size1.cy & 0xffff;
	switch(yuvfmt)
	{
	case JPEG_422:
		len <<= 1;
		break;
	case JPEG_420:
	case JPEG_411:
		len *= 3;
		len >>= 1;
		break;
	case JPEG_444:
		len *= 3;
		break;
	case JPEG_400:
	default:
		break;
	}
	return len;
}
//////////////////////////////tools end///////////////////////////
/********************************************************************************
* Description:
*	Reset jpeg module
* Parameters:
*	None
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_CTRL bit3
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_ResetState(void)
{
	UINT8 qf=VIM_JPEG_GetQFactor();
	
	VIM_JPEG_SetBrcQF(qf);
	VIM_HIF_SetReg32(V5_REG_JPEG_CTRL, BIT3);
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][JPEG]VIM_JPEG_ResetState qf=",qf);
#endif

}

/********************************************************************************
	Description:
		set jpeg work mode
	Parameters:
		byMode:	work mode
			VIM_JPEG_MODE_PREVIEW=0,
			VIM_JPEG_MODE_CAPTURE_MJPEG,
			VIM_JPEG_MODE_DISPLAY_STILL,
			VIM_JPEG_MODE_DISPLAY_VIDEO,
			VIM_JPEG_MODE_DECODE,
			VIM_JPEG_MODE_ENCODE
	Returns:
	Remarks:
*********************************************************************************/

void VIM_JPEG_SetMode(VIM_JPEG_MODE bMode)
{
UINT8 bBypass=0;
UINT8 bLineMode=0;
UINT8 bValue=0;

	//ready set lbuf working mode, bit0-2 is working mode 
	bLineMode=VIM_HIF_GetReg8(V5_REG_LBUF_MODE+3);
	bLineMode&=(~(BIT7|BIT5|BIT6|BIT4));	

      	//jpeg decode bypass disable
	bBypass=VIM_HIF_GetReg8(V5_REG_JPEG_DEC_BYPASS);
	bBypass&=(~BIT4);	
	//jpeg work mode
	bValue=VIM_HIF_GetReg32(V5_REG_JPEG_MODE);
	bValue&=(~(BIT0|BIT1));	
	switch(bMode)
		{
		case VIM_JPEG_MODE_PREVIEW:
			bValue|=00;			//set jpeg capture  mode
			break;
		case VIM_JPEG_MODE_CAPTURE_MJPEG:
			bValue|=00;			//set jpeg capture  mode
			break;
	
		case VIM_JPEG_MODE_DISPLAY_JPEG:
			bValue|=01;			//set jpegdisplay  mode
			break;
		case VIM_JPEG_MODE_DISPLAY_YUV:
			bValue|=01;			//set jpeg display  mode
			bBypass|=BIT4;//jpeg decode bypass enable
			bLineMode|=BIT5;		//preshift bypass
			break;
		case VIM_JPEG_MODE_DISPLAY_VIDEO:
			bValue|=01;			//set jpeg display  mode
			break;		
		case VIM_JPEG_MODE_DECODE_JPEGTOYUV:
			bValue|=03;			//set jpeg decode  mode
			break;	
		case VIM_JPEG_MODE_DECODE_JPEGTORGB:
			bValue|=03;			//set jpeg decode  mode
			bLineMode|=BIT6;// select rgb 565 when decode mode
			break;
#if(BOARD_TYPE==VC0820_FPGA)
		case VIM_JPEG_MODE_DECODE_YUV:
			bValue|=03;			//set jpeg decode  mode
			bBypass|=BIT4;		//jpeg decode bypass enable
#if	VIM_DECODE_BYBASSLBUF	
			bLineMode|=(BIT7|BIT5);//line buf  bypass
#else
			bLineMode|=BIT5;//preshift bypass
#endif
			break;
#endif
		case VIM_JPEG_MODE_ENCODE:
			bValue|=02;			//set jpeg encode  mode
			break;	
		case VIM_JPEG_MODE_CAPTURE_MPG4:
			bValue|=00;			//set jpeg capture  mode
			bLineMode|=BIT4;// disable preshift when decode mode
			break;
		case VIM_JPEG_MODE_DISPLAY_MPG4:
			bValue|=03;			//set jpeg decode  mode
			bLineMode|=BIT5;// preshift bypass
			break;
		case VIM_JPEG_MODE_VIDEO_MPG4:
			bValue|=00;			//set jpeg capture  mode
			bLineMode|=BIT4;// preshift bypass
			bLineMode|=BIT5;// preshift bypass
			break;
		}
	//jpeg module reset
	VIM_JPEG_ResetState();
	//line buffer working mode set 
	VIM_HIF_SetReg8(V5_REG_LBUF_MODE+3,bLineMode);
	//jpeg working mode set 
	VIM_HIF_SetReg32(V5_REG_JPEG_MODE,bValue);
		//jpeg decode bypass set
	VIM_HIF_SetReg8(V5_REG_JPEG_DEC_BYPASS,bBypass);
}
/********************************************************************************
* Description:
*	Enable/Disalbe preshift when display yuv
*     from-128-+127->0-255
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* 
**********************************************************************************/
void VIM_JPEG_SetPreShifEn(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_LBUF_MODE+3);
	byValue&=(~(BIT5|BIT4));
	byValue|=(bEnable<<5);
	byValue|=(bEnable<<4);
	VIM_HIF_SetReg8(V5_REG_LBUF_MODE+3, byValue);
}

/********************************************************************************
* Description:
*	Enable/Disalbe custom quantization table.
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_MODE bit3
**********************************************************************************/
void VIM_JPEG_SetUseCustomQTEnable(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_MODE);
	byValue&=(~BIT3);
	byValue|=(bEnable<<3);
	VIM_HIF_SetReg8(V5_REG_JPEG_MODE, byValue);
}
/********************************************************************************
* Description:
*	Enable/Disalbe custom quantization table.
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_MODE bit3
**********************************************************************************/
void VIM_JPEG_SetUseSameQTEnable(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_MODE);
	byValue&=(~BIT4);
	byValue|=(bEnable<<4);
	VIM_HIF_SetReg8(V5_REG_JPEG_MODE, byValue);
}
/********************************************************************************
* Description:
*	Enable/Disalbe custom quantization table.
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_MODE bit3
**********************************************************************************/
void VIM_JPEG_SetUseCustomHTEnable(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_MODE);
	byValue&=(~BIT5);
	byValue|=(bEnable<<5);
	VIM_HIF_SetReg8(V5_REG_JPEG_MODE, byValue);
}
/********************************************************************************
* Description:
*	Enable/Disalbe Enhanced brc 
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_MODE bit7
**********************************************************************************/
void VIM_JPEG_SetEnhancedBrcEn(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_MODE);
	byValue&=(~BIT7);
	byValue|=(bEnable<<7);
	VIM_HIF_SetReg8(V5_REG_JPEG_MODE, byValue);
}
/********************************************************************************
* Description:
*	Enable/Disalbe feed header 
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_MODE bit7
**********************************************************************************/

void VIM_JPEG_FeedHeaderEnable(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_FEEDTHROUGH_HEAD);
	byValue&=(~BIT0);
	byValue|=bEnable;
	VIM_HIF_SetReg8(V5_REG_JPEG_FEEDTHROUGH_HEAD, byValue);
}
/********************************************************************************
* Description:
*	Start jpeg capture
* Parameters:
*	None
* Return:
*	None
* Note:
		Only be used in encode mode
* Set V5_REG_JPEG_JPEG_CTRL bit0
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_StartEncode(BOOL Enable)
{
UINT32 Temp,Loop=0x100;
	Temp=VIM_HIF_GetReg32(V5_REG_IPP_CTRL);
	if(Enable==ENABLE)
		VIM_HIF_SetReg32(V5_REG_IPP_CTRL, Temp|BIT7);
	else
		VIM_HIF_SetReg32(V5_REG_IPP_CTRL, Temp&(~BIT7));
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	if(Enable==DISABLE)
	{
		while(Loop--)
		{
			if(!VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE))
				break;
		}
	}
	if(!Loop)
	{
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("VC0578B][JPEG]VIM_JPEG_StartEncode disable over time[ERROR]");
#endif	
	}

}

/********************************************************************************
* Description:
*	Start jpeg decode
* Parameters:
*	None
* Return:
*	None
* Note:
	Only be used in decode mode
* Set V5_REG_JPEG_JPEG_CTRL bit1
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_StartDecode(void)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_CTRL, BIT1);
}

/********************************************************************************
* Description:
*	Restart jpeg decode
* Parameters:
*	None
* Return:
*	None
* Note:
	Only be used in decode mode
* Set V5_REG_JPEG_JPEG_CTRL bit2
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_RestartDecode(void)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_CTRL, BIT2);
	//VIM_HIF_SetReg8(V5_REG_LBUF_STATUS+2, BIT2|BIT1);
	
}



/********************************************************************************
* Description:
*	Start writing quantization table
* Parameters:
*	None
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_CTRL bit4
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_StartWriteQT(void)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_CTRL, BIT4);
}
/********************************************************************************
* Description:
*	Start writing Huffman table
* Parameters:
*	None
* Return:
*	None
* Note:
* Set V5_REG_JPEG_JPEG_CTRL bit5
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_StartWriteHT(void)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_CTRL, BIT5);
}
/********************************************************************************
* Description:
*	Huffman table select
* Parameters:
*	byHTIndex: Huffman table select index. It's from 0 to 3.
* Return:
*	TRUE: Select success
*	FALSE: byHTIndex is out of rang (not from 0 to 3)
* Note:
* Set V5_REG_JPEG_JPEG_CTRL bit6,7
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
* huff_sel[1:0]=2’b00 : y_dc Huffman table select 
* huff_sel[1:0]=2’b01 : y_ac Huffman table select 
* huff_sel[1:0]=2’b10 : c_dc Huffman table select 
* huff_sel[1:0]=2’b11 : c_cc Huffman table select
**********************************************************************************/
void  VIM_JPEG_SelectHT(UINT8 byHTIndex)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_CTRL, (UINT32)(byHTIndex << 6));
}

/********************************************************************************
* Description:
*	The JPEG status:
*   Encode done, Encode done, Decode error flag, Header done, JPEG control state machine
* Parameters:
*	None
* Return:
*	JPEG status
* Note:
* Get V5_REG_JPEG_JPEG_STATUS
**********************************************************************************/
UINT8 VIM_JPEG_GetStatus(void)
{
	return VIM_HIF_GetReg8(V5_REG_JPEG_STATUS);
}

BOOL VIM_JPEG_DecodeIsDone(void)
{
	UINT8 byValue = VIM_HIF_GetReg8(V5_REG_JPEG_STATUS);
	return(byValue & BIT1);
}

UINT16 VIM_JPEG_DecodeIsError(void)
{
	UINT8 byValue = VIM_HIF_GetReg8(V5_REG_JPEG_STATUS);
	if (byValue & BIT2)
		return VIM_ERROR_DECODE_ERROR;
	else 
		return VIM_SUCCEED;
}

UINT16 VIM_JPEG_EncodeIsError(void)
{
	UINT8 byValue = VIM_HIF_GetReg8(V5_REG_JPEG_STATUS);
	if (byValue & BIT4)
		return VIM_ERROR_DECODE_ERROR;
	else 
		return VIM_SUCCEED;
}

/********************************************************************************
* Description:
*	Set Initial Q factor.
* Parameters:
*	byQFactor: Q factor
* Return:
*	None
* Note:
* Set V5_REG_JPEG_BRC bit0 ~ 6
**********************************************************************************/
void VIM_JPEG_SetBrcQF(UINT8 byQFactor)
{
	UINT8 byValue;

	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_BRC);
	byValue &= BIT7;
	byQFactor &= (~BIT7);
	VIM_HIF_SetReg8(V5_REG_JPEG_BRC, byValue | byQFactor);
}

/********************************************************************************
* Description:
*	Enable/Disalbe Auto Bit Rate Control
* Parameters:
*	bEnable: 
*		TRUE: Enable.
*		FALSE: Disable.
* Return:
*	None
* Note:
* Set V5_REG_JPEG_BRC bit7
**********************************************************************************/
void VIM_JPEG_SetBitRateControlEn(BOOL bEnable)
{
	UINT8 byValue;
	byValue = VIM_HIF_GetReg8(V5_REG_JPEG_BRC);
	byValue&=(~BIT7);
	byValue|=(bEnable<<7);
	VIM_HIF_SetReg8(V5_REG_JPEG_BRC, byValue);
}

/********************************************************************************
* Description:
*	Set Target compression ratio
* Parameters:
*	byRatio: Compress ratio
* Return:
*	None
* Note:
* Set V5_REG_JPEG_TCR
* This bit is used when auto bit rate control is on. 
**********************************************************************************/
void VIM_JPEG_SetTargetCompressRatio(UINT8 byRatio)
{
	byRatio<<=2;
	byRatio++;
	VIM_HIF_SetReg8(V5_REG_JPEG_TCR, byRatio);
}
UINT8 VIM_JPEG_GetTargetCompressRatio(void)
{
UINT8 bRatio;
	bRatio=VIM_HIF_GetReg8(V5_REG_JPEG_TCR);
	bRatio--;
	bRatio>>=2;
	return bRatio;
}
/********************************************************************************
* Description:
*	Get the Q factor of current frame 
* Parameters:
*	None
* Return:
*	Q factor of current frame 
* Note:
* Get V5_REG_JPEG_QF
* This register is used when auto bit rate control is enabled. 
* Q factor of each frame may be different.
**********************************************************************************/
UINT8 VIM_JPEG_GetQFactor(void)
{
	return VIM_HIF_GetReg8(V5_REG_JPEG_QFACTOR);
}
/********************************************************************************
* Description:
*	Set Target Word Count
* Parameters:
*	dwCount: Target Word Count
* Return:
*	None
* Note:
* Set V5_REG_JPEG_TARGET_WC_0,1,2,3
*  If the image size is 1600x1200 (2.0Meg):
* TARGETWC_0=8’h00, TARGETWC_1=8’h01, TARGETWC_2=8’h77, TARGETWC_3=8’h00 
*  If the image size is 1280x960 (1.3Meg): 
* TARGETWC_0=8’h00, TARGETWC_1=8’h00, TARGETWC_2=8’hf0, TARGETWC_3=8’h00 
*
*  Target word count register is used when auto bit rate control is enabled. 
*  The value set in the register is 1/4 of the actual byte count of target file size. 
*  For example, if the image size is 1280x960 and image format is YUV422, 
*  the image byte count before compression is 1280x960x2 bytes. 
*  If the target compression ratio is 15, the target byte count is 1280x960x2/10 = 245760 bytes. 
*  In this case, the target word count register will be set to 245760/4 = 61440 (F000H).
**********************************************************************************/
void VIM_JPEG_SetTargetWordCount(UINT32 dwCount)
{
	dwCount>>=2;
	VIM_HIF_SetReg32(V5_REG_JPEG_TARGET_WC, (UINT32)dwCount);
}
UINT32 VIM_JPEG_GetTargetWordCount(void)
{
  UINT32 value32;
	value32=(UINT32)VIM_HIF_GetReg32(V5_REG_JPEG_TARGET_WC);
	value32<<=2;	
  return value32;	
  }
/********************************************************************************
* Description:
*	Set Video Word Count
* Parameters:
*	dwCount: Video Word Count
* Return:
*	None
* Note:
* Set V5_REG_JPEG_VIDEO_WC_0,1,2,3
*  If the image size is 1600x1200 (2.0Meg): 
*  VIDEOWC_0=8’h00, VIDEOWC _1=8’h0e, VIDEOWC _2=8’ha6, VIDEOWC _3=8’h00 
* If the image size is 1280x1024 (1.3Meg): 
*  VIDEOWC _0=8’h00, VIDEOWC _1=8’h09, VIDEOWC _2=8’h60, VIDEOWC _3=8’h00 
* Video word count register is used in each mode. 
* The value set in the register is 1/4 of the actual byte count of uncompressed image. 
*
* For example, if the image size is 1280x960 and image format is YUV422, 
* the image byte count before compression is 1280x960x2 = 2457600 bytes. 
* In this case, the video word count register will be set to 2457600/4 = 96000 (H). 
* If the image format is YUV420, the image byte count before compression is 1280x960x1.5 = 1843200 bytes. 
* In this case, the register value will be 460800 (70800H).
**********************************************************************************/
void VIM_JPEG_SetVideoWordCount(UINT32 dwCount)
{
	dwCount>>=2;
	VIM_HIF_SetReg32(V5_REG_JPEG_VIDEO_WC, (UINT32)dwCount);
}

UINT32 VIM_JPEG_GetVideoWordCount(void)
{
  UINT32 value32;
  	value32=(UINT32)VIM_HIF_GetReg32(V5_REG_JPEG_VIDEO_WC);
	value32<<=2;	
  return value32;	
}
/********************************************************************************
* Description:
*	Set JPEG image size.
* Parameters:
*	wWidth: image width
*   wHeight: image height
* Return:
*	None
* Note:
*
**********************************************************************************/
void VIM_JPEG_SetImgSize(VIM_JPEG_YUVMODE bYUVmode,TSize ImgeSize)
{

	TSize LineBufSize;

	VIM_JPEG_GetJpegSize(bYUVmode,ImgeSize,&LineBufSize);
	//set jpeg image size	
	VIM_HIF_SetReg32(V5_REG_JPEG_IMAGEWIDTH, (UINT32)ImgeSize.cx);
	VIM_HIF_SetReg32(V5_REG_JPEG_IMAGEHEIGHT, (UINT32)ImgeSize.cy);
	//Set line buf image width
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE,(UINT16)LineBufSize.cx);
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE+2,(UINT16)(LineBufSize.cy));
	//Set mpeg 4 image size
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE,(UINT16)LineBufSize.cx);
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE+2,(UINT16)(LineBufSize.cy));
	//line buffer counter reset
	//VIM_JPEG_ResetState();	
}



/********************************************************************************
* Description:
*	Get Frame Count
* Parameters:
*	None
* Return:
*	Frame Count
* Note:
*
**********************************************************************************/
UINT16 VIM_JPEG_GetFrameCount(void)
{
	UINT16	wFrameCount = 0;

	wFrameCount = VIM_HIF_GetReg16(V5_REG_JPEG_FRAMECOUNT);

	return  (UINT16)wFrameCount;
}

UINT16 VIM_JPEG_DelayFrame(UINT16 wFrameCount)
{
	UINT16 wOldCount = 0;
	UINT16 wNewCount = 0;
	UINT32 wDelayTimes = 0x3fff;

	wOldCount  = VIM_JPEG_GetFrameCount();
	while (wDelayTimes--)
	{
		wNewCount=VIM_JPEG_GetFrameCount();
		if((wNewCount - wOldCount) >= wFrameCount)
		{
			return VIM_SUCCEED;
		}
		VIM_USER_DelayMs(1);
	}
	return VIM_ERROR_DELAY_FRAME;
}
/********************************************************************************
* Description:
*	Write YUV Data to JPEG module.
* Parameters:
*	byData: YUV Data.
* Return:
*	None
* Note:
* After header data is written to JPEG buffer, host begins to write DCT data to this register.
* The index of data is increased automatically. The data sequence must be block by block.
**********************************************************************************/
void VIM_JPEG_WriteYUVData(UINT8 byData)
{
	VIM_HIF_SetReg8(V5_REG_JPEG_DCT_DATA, byData);
}

/********************************************************************************
* Description:
*	Write Quantization Table
* Parameters:
*	byData: Quantization Table Data.
* Return:
*	None
* Note:
* This byte is used in JPEG decode mode and display mode. 
* Before setting start decode bit, host writes quantization table to jpeg module. 
* The index of table data is automatically increased. 
* After setting WQ bit in “jpeg_ctrl” register, host writes data to this register byte by byte.
* The sequence of tables is quantization table 0 to 3. Not all the tables are needed. 
* This is decided by jpeg header. And if possible, host needn’t write quantization table to jpeg module
* if the table is fixed.
**********************************************************************************/
void VIM_JPEG_WriteQtable(UINT8 byData)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_QUANTATIONTABLE, (UINT32)byData);
}

/********************************************************************************
* Description:
*	Write Huffman Table
* Parameters:
*	byData: Huffman Table Data.
* Return:
*	None
* Note:
* This byte is used in JPEG decode mode and display mode. 
* Before setting start decode bit, host writes Huffman table to jpeg module. 
* The index of table data is automatically increased. 
* After setting WH bit in “jpeg_ctrl” register, host writes data to this register byte by byte. 
* The sequence of tables is DC Luminance, DC Chrominance, AC Luminance and AC Chrominance. 
*　If possible, host needn’t write Huffman table to jpeg module if the table is fixed.
**********************************************************************************/
void VIM_JPEG_WritetHtable(UINT8 byData)
{
	VIM_HIF_SetReg8(V5_REG_JPEG_HUFFMANTABLE, (UINT32)byData);
}

/********************************************************************************
* Description:
*	Set the number of quantization tables in the header.
* Parameters:
*	byNum: Number of quantization tables.The value is from 1 to 4.
* Return:
*	None
* Note:
* 
**********************************************************************************/
void VIM_JPEG_SetComponentNum(UINT8 byNum)
{
	VIM_HIF_SetReg8(V5_REG_JPEG_COMP_QUANT, (UINT8)(byNum>>4)|(UINT8)(byNum<<4));
}

/********************************************************************************
* Description:
*	Set the Component0 to Component3 Parameters
* Parameters:
*	byIndex: Index of Component0 to Component3, it's from 0 to 3
*	wValue: The value of the component parameter.
* Return:
*	TRUE: Set success
*	FALSE: byIndex is out of range (Not from 0 to 3).
* Note:
* 
**********************************************************************************/
void VIM_JPEG_SetComponentParm(UINT8 byIndex, UINT16 wValue)
{
	switch(byIndex)
	{
	case 0:
		VIM_HIF_SetReg16(V5_REG_JPEG_COMP_01, (UINT16)(wValue>>8)|(UINT16)(wValue<<8));
		break;
	case 1:
		VIM_HIF_SetReg16(V5_REG_JPEG_COMP_01+2, (UINT16)(wValue>>8)|(UINT16)(wValue<<8));
		break;
	case 2:
		VIM_HIF_SetReg16(V5_REG_JPEG_COMP_23, (UINT16)(wValue>>8)|(UINT16)(wValue<<8));
		break;
	case 3:
		VIM_HIF_SetReg16(V5_REG_JPEG_COMP_23+2, (UINT16)(wValue>>8)|(UINT16)(wValue<<8));
		break;
	default:
		break;
	}


}
/********************************************************************************
* Description:
*	Set the numbers of 8x8 blocks in 8 lines
* Parameters:
*	wNum: the numbers of 8x8 blocks in 8 lines
* Return:
*	None
* Note:
* If the image size is 1600x1200 (2.0Meg): 
* NUM_BLK_0=2’b01, NUM_BLK_1=8’h90 
* If the image size is 1280x960 (1.3Meg): 
* NUM_BLK_0=2’b01, NUM_BLK_1=8’h40 
* If the image size is 1280x960: 
* the color mode is YUV422, num_blk =1280*(2+1+1)/16 =320(140H). 
* the color mode is YUV420, num_blk= 1280*(4+1+1)/16 =480(1E0H). 
* the color mode is YUV411, num_blk= 1280*(4+1+1)/32 =240(F0H). 
* the color mode is YUV400, num_blk= 1280*(1+0+0)/8 =160(A0H).
**********************************************************************************/
void VIM_JPEG_SetBlockNum(UINT16 wNum)
{
	VIM_HIF_SetReg32(V5_REG_JPEG_BLOCKNUMBER, (UINT32)wNum);
}

/********************************************************************************
* Description:
*	Set the jpeg pixel rate
* Parameters:
	bPixelrate:
		pixel rate of jpeg buf and line buf
		0 is not permitted
		1-15
* Return:
* Note:
* 
**********************************************************************************/
void VIM_JPEG_SetPixelRate(UINT8 bPixelrate)
{
	UINT8 bValue;

	VIM_HIF_SetReg8(V5_REG_LBUF_MODE+2,bPixelrate);
	bValue=VIM_HIF_GetReg8(V5_REG_JPEG_DEC_BYPASS);
	bPixelrate&=0x0f;
	bValue&=0xf0;
	bValue|=bPixelrate;
	VIM_HIF_SetReg8(V5_REG_JPEG_DEC_BYPASS,bValue);
//	VIM_HIF_SetReg8(V5_REG_LBUF_MODE+2,bPixelrate);
}

/********************************************************************************
* Description:
*	Set the jpeg h blank
* Parameters:
	bHblank:
		hblank of jpeg and line buf
* Return:
* Note:
* 
**********************************************************************************/
void VIM_JPEG_SetLineBlank(UINT16 bHblank)
{
	// line buf hblank
	VIM_HIF_SetReg32(V5_REG_LBUF_HBLANK,(UINT32)bHblank);
	bHblank++;
	// jpeg h blank
	VIM_HIF_SetReg32(V5_REG_JPEG_HBLANK,(UINT32)bHblank);

}
/********************************************************************************
* Description:
*	Set the timer interval when display video
* Parameters:
	dwMs:
		ms (timer interval)
	dwMclk:
		the mclk vc0578 running
* Return:
* Note:
* 
**********************************************************************************/
void VIM_JPEG_SetTimerInterval(UINT32 dwMs,UINT32 dwMclk)
{
	UINT16 Num1Ms;
		Num1Ms=dwMclk/1000;// 1000 us=(1000000/mclk)(us)*Num
		dwMs*=Num1Ms;
		VIM_HIF_SetReg32(V5_REG_JPEG_TIMERINTERVAL,(dwMs));
}
/********************************************************************************
* Description:
*	get line num after one decode processing
* Parameters:
* Return:
	Lines can be read after decode
* Note:
* 
**********************************************************************************/
UINT8 VIM_JPEG_GetDecodeLines(void)
{
	return (VIM_HIF_GetReg8(V5_REG_LBUF_STATUS+1));
}
/********************************************************************************
* Description:
*	get the status if the line buf can be read
* Parameters:
* Return:
	TRUE: decode ok
	FALSE: decodeing ...
* Note:
* 
**********************************************************************************/
UINT8 VIM_JPEG_CheckDecodeOK(void)
{
	//return ((VIM_HIF_GetReg8(V5_REG_LBUF_LBUF_STATUS)>>1)&1);
	return ((VIM_HIF_GetReg8(V5_REG_LBUF_STATUS)>>1)&1);
}

/********************************************************************************
	In Work Mode:

	display
	display bypass
	decode frame
	decode frame bypass

	Use this functions to calculate pixrate.
********************************************************************************/
UINT8 VIM_JPEG_GetPixRate()
{
	UINT16  src_cx, src_cy;
	UINT32 lcddelay;
	UINT32 rate;
	VIM_DISP_BUFFERMODER Amode;
	TSize DisSize;
	UINT8 LineMode;
#if(BOARD_TYPE==VC0820_FPGA)
	UINT8 JpegBypass;
	UINT32 ImageCount;
#endif
	// mode is display and use one frame
	Amode=VIM_DISP_GetBufferMode();
	LineMode=(VIM_HIF_GetReg8(V5_REG_LBUF_MODE+1)&0x7);

#if(BOARD_TYPE==VC0820_FPGA)
	JpegBypass=(VIM_HIF_GetReg8(V5_REG_JPEG_DEC_BYPASS)>>4);
#endif
	if(Amode!=VIM_DISP_LINEBUF)
	{
		if(LineMode==0x2)		//jpeg mode 
#if(BOARD_TYPE==VC0820_FPGA)
		{
			if(JpegBypass==1)
				return 4;
			else
			{
				ImageCount=gVc0578b_JpegInfo.JpgSize.cx*gVc0578b_JpegInfo.JpgSize.cy;
				if(ImageCount<100000)
				{
					return 3;
				}
				else if((ImageCount>=100000)&&(ImageCount<1000000))
				{
					return 2;
				}
				else if((ImageCount>=1000000)&&(ImageCount<2000000))
				{
					return 2;
				}
				else if((ImageCount>=2000000)&&(ImageCount<4000000))
				{
					return 2;
				}				
				else if((ImageCount>=4000000)&&(ImageCount<10000000))
				{
					return 1;
				}
				else 
				{
					return 2;
				}
			}				
		}
#else
			return 1;
#endif
		if(LineMode==0x6)		// mp4 decode mode
			return 2;
	}
	if(((LineMode==0x3))&&(Amode==VIM_DISP_LINEBUF))
		return  7;
	if(((LineMode==0x6))&&(Amode==VIM_DISP_LINEBUF))
		return  0x7;
	
	VIM_IPP_GetWindowSize(&src_cx, &src_cy);
	if(LineMode==0x7)// video conference
		VIM_IPP_GetDispalyWindowSize(&DisSize.cx,&DisSize.cy);		//angela change it to capture window
	else
		VIM_IPP_GetCaptureWindowSize(&DisSize);		//angela change it to capture window

	ZERO_ROUND(src_cx);
	ZERO_ROUND(src_cy);
	ZERO_ROUND(DisSize.cx);
	ZERO_ROUND(DisSize.cy);

	lcddelay = VIM_HIF_GetReg8(V5_REG_LCDC_DEDLY+1) + 2;
	rate = DIV_ROUND( DIV_ROUND( lcddelay * DisSize.cy * DisSize.cx, src_cy ), src_cx);

	if(VIM_IPP_GetCaputreSizeSelect()==VIM_IPP_CAPMODE_UP)	//angela add it for display mpeg4 zoom out 2007-8-21
		rate<<=1;
	else
	{
		if(((VIM_HIF_GetReg8(V5_REG_LCDC_CTRL+3)>>4)&0x7)%2)		
	 		rate<<=3;	//angela rate>>=3 because 8 rates of speed when rotation  2007-8-29 changed it to max value
	}
	if(rate < 2)
		rate = 2;
	if(rate > 16)
		rate = 16;
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][JPEG]rate ",rate);
#endif
	if(LineMode==0x7)// video conference	
		return gPixel[rate+2];	
	else
		return gPixel[rate-1];

}

/********************************************************************************
	In Work Mode:

	display
	display bypass
	decode frame
	decode frame bypass

	Use this functions to calculate hblank.
********************************************************************************/
UINT16 VIM_JPEG_GetHbanlk(UINT8 pixelrate)
{
	UINT16  src_cx, src_cy;
	UINT32 lcddelay;
	UINT32 hblank;
	VIM_DISP_BUFFERMODER Amode;
	TSize DispSize;
	UINT8 LineMode;
	VIM_IPP_CAPSIZE_MODE Sizer;
	
	// mode is display and use one frame
	Amode=VIM_DISP_GetBufferMode();
	LineMode=(VIM_HIF_GetReg8(V5_REG_LBUF_MODE+1)&0x7);
	Sizer = VIM_IPP_GetCaputreSizeSelect();
	if(Amode!=VIM_DISP_LINEBUF)
	{
		if(LineMode==2)	//jpeg mode 
			return 0;
		if(LineMode==6)	//mp4 decode mode
		{
			if(Sizer==VIM_IPP_CAPMODE_UP)
				return 2;
			else
				return 0;
		}	
	}
	if(((LineMode==0x6))&&(Amode==VIM_DISP_LINEBUF))
		return  0x0;
	if(((VIM_HIF_GetReg8(V5_REG_LBUF_MODE+1)&0x7)==0x3)&&(Amode==VIM_DISP_LINEBUF))
		return  0x60;

	VIM_IPP_GetWindowSize(&src_cx, &src_cy);
	VIM_IPP_GetCaptureWindowSize(&DispSize );

	ZERO_ROUND(src_cx);
	ZERO_ROUND(src_cy);
	ZERO_ROUND(DispSize.cx);
	ZERO_ROUND(DispSize.cy);

	if(VIM_IPP_GetCaputreSizeSelect()==VIM_IPP_CAPMODE_UP)	//angela add it for display mpeg4 zoom out 2007-8-21
	{
		lcddelay = VIM_HIF_GetReg8(V5_REG_LCDC_DEDLY)+ VIM_HIF_GetReg8(V5_REG_LCDC_DEDLY+2)+ 4;
		hblank=src_cx*pixelrate+DIV_ROUND( lcddelay * DispSize.cy, src_cy)*2;
		hblank = DIV_ROUND( hblank, pixelrate);	
	}
	else
	{
		lcddelay = VIM_HIF_GetReg8(V5_REG_LCDC_DEDLY)+ VIM_HIF_GetReg8(V5_REG_LCDC_DEDLY+2)+ 4;
		hblank = DIV_ROUND( lcddelay * DispSize.cy, src_cy);
		hblank = DIV_ROUND( hblank, pixelrate);
		if(((VIM_HIF_GetReg8(V5_REG_LCDC_CTRL+3)>>4)&0x7)%2)		//angela add it for display mpeg4 rotation 2007-8-26
			hblank<<=6;
	}


		
	if(hblank > HABLAN_MAX)
		hblank = HABLAN_MAX;
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][JPEG]hblank ",hblank);
#endif
	//hblank+=(VIM_HIF_GetReg8(V5_REG_LBUF_MODE)&0x3);
	return (UINT16)hblank;		//angela test
}
//end add 2005-12-31 14:10

/********************************************************************************
* Description:
*	adjust pixel clk and hblank in display mode
* Parameters:

* Return:
.* Note:
* 
**********************************************************************************/
void VIM_JPEG_AdjustClkBlank(void)
{
	UINT8  pixelrate;
	UINT16 blank;
//	pixelrate =VIM_JPEG_GetPixRate();
	pixelrate=0x2;//0xa;
	MMDPrintf("*******pixelrate=%d\n",pixelrate);
	VIM_JPEG_SetPixelRate(pixelrate);
//	blank = VIM_JPEG_GetHbanlk(pixelrate);
	blank=0x2;
	MMDPrintf("*******blank=%d\n",blank);
	VIM_JPEG_SetLineBlank(blank);
}


/********************************************************************************
* Description:
*	Set jpeg size 
* Parameters:
	bYUVmode: 422/411/400/444
	ImageSize: width and heigth
* Return:
.* Note:
* 
**********************************************************************************/

VIM_RESULT VIM_JPEG_SetSize(VIM_JPEG_YUVMODE bYUVmode,TSize ImageSize)
{

UINT16 wComp[3];
UINT32 wLineBuvDelay;

	if((ImageSize.cx>VIM_MAX_JPEGSIZE_W)||(ImageSize.cy>VIM_MAX_JPEGSIZE_H))
		return VIM_ERROR_JPEG_OVERRANGE;
	// set line buf yuv mode
	if(bYUVmode>=JPEG_UNKNOWN)
		return VIM_ERROR_YUV_UNKNOW;

	VIM_JPEG_SetUseSameQTEnable(DISABLE);
	VIM_JPEG_SetUseCustomQTEnable(DISABLE);
	VIM_JPEG_SetUseCustomHTEnable(DISABLE);
	
	VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)bYUVmode);
	
	// set jpeg and line buf image width
	VIM_JPEG_SetImgSize(bYUVmode,ImageSize);
	// set video word counter
	VIM_JPEG_GetJpegSize(bYUVmode,ImageSize,&ImageSize);
	VIM_JPEG_SetVideoWordCount(VIM_JPEG_GetYuvSize(bYUVmode,ImageSize));
	//set block number
#if(BOARD_TYPE==VC0820_FPGA)
	VIM_JPEG_SetBlockNum(VIM_JPEG_JpegCalBlockNum(bYUVmode,ImageSize));
#else
	VIM_JPEG_SetBlockNum(VIM_JPEG_GetJpegBlockNum(bYUVmode,ImageSize));
#endif
	// set component
#if(BOARD_TYPE==VC0820_FPGA)
	if((gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_DECODERGBBYPASS_MODE)||(gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_DISPLAYSTILLBYPASS_MODE))
	{}
	else
	{
		VIM_JPEG_SetComponentNum(VIM_JPEG_GetComponent(bYUVmode,wComp));
		VIM_JPEG_SetComponentParm(0,wComp[0]);
		VIM_JPEG_SetComponentParm(1,wComp[1]);
		VIM_JPEG_SetComponentParm(2,wComp[2]);
	}

#endif	
	// adjust pixel clk and h blank according to a mode
	//VIM_JPEG_AdjustClkBlank();
	
	// set line buf delay to marb

	if((VIM_HIF_GetReg8(V5_REG_LBUF_MODE+1)&0x7)==0x1)//capture mode
	{
		wLineBuvDelay=VIM_JPEG_CalCapDelay(bYUVmode,ImageSize);
		VIM_HIF_SetReg32(V5_REG_LBUF_VSYNCDELAY,(UINT32)(wLineBuvDelay|BIT28));
		//VIM_HIF_SetReg32(V5_REG_JPEG_H_SIZE,(UINT32)7);
	}
	else
	{
		VIM_HIF_SetReg32(V5_REG_LBUF_VSYNCDELAY,0);
	}

	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		set lbuf1 line number & threshold.
		
Parameters:
		maxnum		: lbuf1 max store image line number.
		threshold	: lbuf1 send interrupt threshold.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_JPEG_SetDecodeLine(UINT8 maxnum, UINT8 threshold)
{
	UINT32	regval = 0;
	
	regval |= (UINT32)maxnum;
	regval |= ((UINT32)threshold<<16);
	VIM_HIF_SetReg32(V5_REG_LBUF_BUF1_LINENUM,	regval);
	
	return VIM_SUCCEED;
}
#if(BOARD_TYPE==VC0820_FPGA)
/***********************************************************************************
Description:
		set lbuf1 line number & threshold.
		
Parameters:
		maxnum		: lbuf1 max store image line number.
		threshold	: lbuf1 send interrupt threshold.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
void VIM_JPEG_SetLbuf1Line(VIM_JPEG_YUVMODE mode)
{
	switch(mode)
	{
		case JPEG_420:
			 VIM_JPEG_SetDecodeLine(24,9);
			 break;
		case JPEG_422:
		case JPEG_411:
		case JPEG_400:
		case JPEG_444:
			 VIM_JPEG_SetDecodeLine(16,9);
			 break;
		default:
			 break;
	}
}
#endif

/********************************************************************************
  Description:
	get line buf yuv mode
  Parameters:
	
  Return:
  Remarks:
*********************************************************************************/

VIM_JPEG_YUVMODE VIM_JPEG_GetYUVMode(void)
{

UINT8 bValue;

	bValue = VIM_HIF_GetReg8(V5_REG_LBUF_MODE);
	bValue&=0x7;
	return  (VIM_JPEG_YUVMODE)(bValue);
}
/********************************************************************************
  Description:
	get line buf width 
  Parameters:
	
  Return:
  Remarks:
*********************************************************************************/

void  VIM_JPEG_GetSize(PTSize Size)
{
#if(BOARD_TYPE==VC0820_FPGA)
	Size->cx=VIM_HIF_GetReg16(V5_REG_JPEG_IMAGEWIDTH);
	Size->cy=VIM_HIF_GetReg16(V5_REG_JPEG_IMAGEHEIGHT);
#else
	Size->cx=VIM_HIF_GetReg16(V5_REG_JPEG_IMAGEWIDTH);
	Size->cy=VIM_HIF_GetReg16(V5_REG_JPEG_IMAGEWIDTH+2);
#endif
}
/********************************************************************************
  Description:
	get line buf width 
  Parameters:
	
  Return:
  Remarks:
*********************************************************************************/

UINT16  VIM_JPEG_LineGetWidth(void)
{
UINT16 dWidth;
	dWidth=VIM_HIF_GetReg16(V5_REG_LBUF_IMAGESIZE);
	return dWidth;
}
/********************************************************************************
  Description:
	Decode jpeg one frame. write jpeg huffman table and q_table
  Parameters:
	pinfo :	jpeg info. include jpeg huffman table and q_table
  Return:
  Remarks:
*********************************************************************************/
void VIM_JPEG_SetJpegInfo(PVIM_JPEG_Index pinfo)
{
	UINT16 i,j;


	VIM_JPEG_SetUseSameQTEnable(DISABLE);
	VIM_JPEG_SetUseCustomQTEnable(DISABLE);
	//set componment number and data
	VIM_JPEG_SetComponentNum(pinfo->CompCount | (pinfo->QTCount << 4));
	for(i = 0; i < 4; i++)
		VIM_JPEG_SetComponentParm(i, pinfo->Comp[i]);
#if(BOARD_TYPE==VC0820_FPGA)
	if(pinfo->QTCount)
	{
		VIM_JPEG_SetUseCustomQTEnable(ENABLE);
		VIM_JPEG_StartWriteQT();
		for(i = 0; i < pinfo->QTCount; i++)
		{
			for(j = 0; j < 64; j++)
				VIM_JPEG_WriteQtable(pinfo->QT[i][j]);
		}
	}
	//enable custom HT, and write HT table data
	
	if(pinfo->HTCount)
	{
		VIM_JPEG_SetUseCustomHTEnable(ENABLE);
		VIM_JPEG_StartWriteHT();
		for(i = 0; i < pinfo->HTCount; i++)
		{
			VIM_JPEG_SelectHT(i);
			for(j = 0; j < pinfo->HTLen[i]; j++)
				VIM_JPEG_WritetHtable(pinfo->HT[i][j]);
		}
	}
#else
	//enable custom QT, and write QT table data
	VIM_JPEG_SetUseCustomQTEnable(ENABLE);
	VIM_JPEG_StartWriteQT();
	for(i = 0; i < pinfo->QTCount; i++)
	{
		for(j = 0; j < 64; j++)
			VIM_JPEG_WriteQtable(pinfo->QT[i][j]);
	}
	//enable custom HT, and write HT table data
	VIM_JPEG_SetUseCustomHTEnable(ENABLE);
	VIM_JPEG_StartWriteHT();
	for(i = 0; i < pinfo->HTCount; i++)
	{
		VIM_JPEG_SelectHT(i);
		for(j = 0; j < pinfo->HTLen[i]; j++)
			VIM_JPEG_WritetHtable(pinfo->HT[i][j]);
	}
#endif
}

#if(BOARD_TYPE==VC0820_FPGA)
 void VIM_JPEG_SetMotionJpegInfo(PVIM_JPEG_Index pinfo)
 {
	 UINT16 i;
 
 
	 VIM_JPEG_SetUseSameQTEnable(DISABLE);
	 VIM_JPEG_SetUseCustomQTEnable(DISABLE);
	 VIM_JPEG_SetUseCustomHTEnable(DISABLE);
	 //set componment number and data
	 VIM_JPEG_SetComponentNum(pinfo->CompCount | (pinfo->QTCount << 4));
	 for(i = 0; i < 4; i++)
		 VIM_JPEG_SetComponentParm(i, pinfo->Comp[i]);
 
	 VIM_JPEG_SetUseSameQTEnable(ENABLE);
	 VIM_JPEG_SetTimerInterval(2500,1000000);
	 return;
	 
 }

/********************************************************************************
* Description:
*	Start jpeg capture
* Parameters:
*	None
* Return:
*	None
* Note:
		Only be used in encode mode
* Set V5_REG_JPEG_JPEG_CTRL bit0
* In V5_REG_JPEG_JPEG_CTRL register, only one bit can be 1 at the same time.
* This is for each bit of V5_REG_JPEG_JPEG_CTRL controls one start operation.
**********************************************************************************/
void VIM_JPEG_StartEncodeYUV(BOOL Enable)
{
	UINT8 Temp;
	Temp=VIM_HIF_GetReg8(V5_REG_JPEG_CTRL);

	if(Enable==ENABLE)
		VIM_HIF_SetReg8(V5_REG_JPEG_CTRL, Temp|BIT0);
	else
		VIM_HIF_SetReg8(V5_REG_JPEG_CTRL, Temp&(~BIT0));
}

void  VIM_JPEG_UYVYToBlockLine(UINT8 *psrcbuf, UINT8 *pdesbuf, UINT16 Width,UINT16 Height)
{
	UINT16 uint,i,j,m,leavex,uint1;
	UINT32 k;
	uint=uint1=Width/16;
	leavex=Width%16;
	
	if(leavex)
		uint1++;
	if(!Height)
		return;
	
	k=0;
	for(i=0;i<uint1;i++)
	{
		for(j=0;j<8;j++)	//y0 block
			for(m=0;m<8;m++)
				{
					if(((m>leavex)&&(i==uint))||(Height==j))
						{
							pdesbuf[k]=0;
							k++;
						}
					else
						{
							pdesbuf[k]=(psrcbuf[i*32+j*Width*2+m*2+1]+128);
							k++;
						}
				}
		for(j=0;j<8;j++)	//y1 block
			for(m=0;m<8;m++)
				{
					if((m>(leavex+8))&&(i==uint)||(Height==j))
						{
							pdesbuf[k]=0;
							k++;
						}
					else
						{
							pdesbuf[k]=(psrcbuf[i*32+16+2*j*Width+2*m+1]+128);
							k++;
						}
				}
		for(j=0;j<8;j++)	//u block
			for(m=0;m<8;m++)
				{
					if((m>(leavex*2))&&(i==uint)||(Height==j))
						{
							pdesbuf[k]=0;
							k++;
						}
					else
						{
							pdesbuf[k]=(psrcbuf[i*32+j*Width*2+4*m]+128);
							k++;
						}
				}
		for(j=0;j<8;j++)	//v block
			for(m=0;m<8;m++)
				{
					if((m>(leavex*2))&&(i==uint)||(Height==j))
						{
							pdesbuf[k]=0;
							k++;
						}
					else
						{
							pdesbuf[k]=(psrcbuf[i*32+j*Width*2+4*m+2]+128);
							k++;
						}
				}
	}
}
#endif

 void VIM_JPEG_SetDefaultHT(void)
{
	UINT8 i;
	VIM_JPEG_SetUseCustomQTEnable(ENABLE);
	VIM_JPEG_SetUseCustomHTEnable(ENABLE);

	VIM_JPEG_StartWriteHT();

	VIM_JPEG_SelectHT(0);
	for(i = 0; i < sizeof(gHT0); i++)
		VIM_JPEG_WritetHtable(gHT0[i]);

	VIM_JPEG_SelectHT(1);
	for(i = 0; i < sizeof(gHT1); i++)
		VIM_JPEG_WritetHtable(gHT1[i]);

	VIM_JPEG_SelectHT(2);
	for(i = 0; i < sizeof(gHT2); i++)
		VIM_JPEG_WritetHtable(gHT2[i]);

	VIM_JPEG_SelectHT(3);
	for(i = 0; i < sizeof(gHT3); i++)
		VIM_JPEG_WritetHtable(gHT3[i]);
}

/********************************************************************************
  Description:
  	interrupt of JPEG
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/


static 	const UINT8 gJpegIntPriority[VIM_JPEG_JPEG_INT_NUM] = 
{ 
	VIM_INT_JPEG_ENC_DONE, VIM_INT_JPEG_DEC_DONE, VIM_INT_JPEG_DEC_ERR
};

void _ISR_JpegJpegIntHandle(UINT8 wIntFlag)
{
	UINT8 i = 0;
	UINT8 bySecondLevelInt = 0;

	for(i=0; i<VIM_JPEG_JPEG_INT_NUM; i++)
	{
		bySecondLevelInt = gJpegIntPriority[i];
		if(TSTINT(wIntFlag, bySecondLevelInt))
		{
			if(gVc0578b_Isr.jpegisr[bySecondLevelInt])
			{
				gVc0578b_Isr.jpegisr[bySecondLevelInt]();
			}
		}
	}
}
/********************************************************************************
  Description:
  	register isr 
  Parameters:
  	bySecondLevelInt:
		VIM_INT_JPEG_ENC_DONE		= 0,
		VIM_INT_JPEG_DEC_DONE		= 1,
		VIM_INT_JPEG_DEC_ERR		= 2,
		VIM_INT_JPEG_LINBUF_READ	=3,
		VIM_JPEG_JPEG_INT_ALL		= VIM_JPEG_JPEG_INT_NUM
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_JPEG_RegisterISR(VIM_JPEG_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr)
{
UINT8 i;
	if(bySecondLevelInt>=VIM_JPEG_JPEG_INT_ALL)
	{
		for(i=0;i<VIM_JPEG_JPEG_INT_ALL;i++)
			gVc0578b_Isr.jpegisr[i] = Isr;
		return;
	}
	gVc0578b_Isr.jpegisr[bySecondLevelInt] = Isr;
}
/********************************************************************************
  Description:
  	register lbuf isr 
  Parameters:
  	bySecondLevelInt:
	
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_LBUF_RegisterISR(VIM_LBUF_INT_TYPE bySecondLevelInt, PVIM_Isr Isr)
{
UINT8 i;
	if(bySecondLevelInt>=VIM_INT_LBUF_INT_ALL)
	{
		for(i=0;i<VIM_INT_LBUF_INT_ALL;i++)
			gVc0578b_Isr.lbufisr[i] = Isr;
		return;
	}
	gVc0578b_Isr.lbufisr[bySecondLevelInt] = Isr;
}
/********************************************************************************
  Description:
  	enable jpeg interrupt
  Parameters:
  	bySecondLevelInt:
		VIM_INT_JPEG_ENC_DONE		= 0,
		VIM_INT_JPEG_DEC_DONE		= 1,
		VIM_INT_JPEG_DEC_ERR		= 2,
		VIM_INT_JPEG_LINBUF_READ	=3,
		VIM_JPEG_JPEG_INT_ALL		= VIM_JPEG_JPEG_INT_NUM
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_JPEG_SetIntEnable(VIM_JPEG_INT_TYPE bySecondLevelInt,BOOL Eanble)
{
	UINT8 bInten;
	if(bySecondLevelInt<VIM_JPEG_JPEG_INT_ALL)
	{
		bInten=VIM_HIF_GetIntEnableSec(VM_HIF_INT_JPEG);
		if(Eanble==ENABLE)
			bInten |= (0x1<<bySecondLevelInt);
		else
			bInten &= (~(0x1<<bySecondLevelInt));	
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_JPEG,bInten);
		if(bInten)
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_JPEG,ENABLE);
		else
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_JPEG,DISABLE);
	}
	else
	{
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_JPEG,0xff);
#if(BOARD_TYPE==VC0820_FPGA)
#else
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_LBUF,0xff);
#endif
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_JPEG,ENABLE);
#if(BOARD_TYPE==VC0820_FPGA)
#else
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_LBUF,ENABLE);
#endif
	}
}
/********************************************************************************
  Description:
  	enable jpeg interrupt
  Parameters:
  	bySecondLevelInt:
		VIM_INT_JPEG_ENC_DONE		= 0,
		VIM_INT_JPEG_DEC_DONE		= 1,
		VIM_INT_JPEG_DEC_ERR		= 2,
		VIM_INT_JPEG_LINBUF_READ	=3,
		VIM_JPEG_JPEG_INT_ALL		= VIM_JPEG_JPEG_INT_NUM
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_LBUF_SetIntEnable(VIM_LBUF_INT_TYPE bySecondLevelInt,BOOL Eanble)
{
	UINT16 bInten;
	if(bySecondLevelInt<VIM_INT_LBUF_INT_ALL)
	{
		bInten=VIM_HIF_GetIntEnableSec(VM_HIF_INT_LBUF);
		if(Eanble==ENABLE)
			bInten |= (0x1<<bySecondLevelInt);
		else
			bInten &= (~(0x1<<bySecondLevelInt));	
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_LBUF,bInten);
		if(bInten)
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_LBUF,ENABLE);
		else
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_LBUF,DISABLE);
	}
	else
	{
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_LBUF,0xff);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_LBUF,ENABLE);
	}
}

/********************************************************************************
  Description:
  	interrupt of lenbuf
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/

static const UINT8 gLbufIntPriority[VIM_JPEG_LBUF_INT_NUM] = {VIM_INT_LBUF_READ_INT	,VIM_INT_LBUF_FRAME_START};

 void _ISR_JpegLbufIntHandle(UINT8 wIntFlag)
{
	UINT8 i = 0;
	UINT8 bySecondLevelInt = 0;

	for(i=0; i<VIM_JPEG_LBUF_INT_NUM; i++)
	{
		bySecondLevelInt = gLbufIntPriority[i];
		if(TSTINT(wIntFlag, bySecondLevelInt))
		{
			if(gVc0578b_Isr.lbufisr[bySecondLevelInt])
			{
				gVc0578b_Isr.lbufisr[bySecondLevelInt]();
			}
		}
	}	
}

/********************************************************************************
 *		   for here copy form jpeg tool file of 568, Amanda deng 2005-12-20 16:49			*
 ********************************************************************************/
/*=============================================================
		Exif parser 
==============================================================*/
UINT16 VIM_JPEG_ToolExifGetWord(HUGE UINT8* ptr, UINT8 order)
{
	if (!ptr) 
		return 0;
	switch(order)
	{
		case EXIF_BYTE_ORDER_MOTOROLA:
			return GETWORD(ptr, 0);
		case EXIF_BYTE_ORDER_INTEL:
			return GETWORD_INV(ptr, 0);
		default:
			return 0;
	}
}

UINT32 VIM_JPEG_ToolExifGetDword(HUGE UINT8* ptr, UINT8 order)
{
	if (!ptr) 
		return 0;
	switch(order)
	{
        case EXIF_BYTE_ORDER_MOTOROLA:
			return GETDWORD(ptr, 0);
        case EXIF_BYTE_ORDER_INTEL:
			return GETDWORD_INV(ptr, 0);
		default:
			return 0;
	}
}


UINT16 VIM_JPEG_ToolGetExifInfo(HUGE UINT8* ptr, UINT32 point, PTThumbNail pthumbinfo)
{
	UINT32 i;
	UINT16 len;

	i = point;
	len = GETWORD(ptr, i);
	i += 2;

	if(ptr[i] == 'E' && ptr[i+1] == 'x' 
		&& ptr[i+2] == 'i' && ptr[i+3] == 'f'
		&& ptr[i+4] == 0 && ptr[i+5] == 0)
	{
		i += 6;
		VIM_JPEG_ToolParseExif(ptr+i, pthumbinfo);
	}

	return len;
}
/*============================================================================
	Desc: get exif ifd info
	notice: ptr is the point to TIFF header, all of offset is based on it
=============================================================================*/
UINT32 VIM_JPEG_ToolGetIFD0(HUGE UINT8* ptr, UINT32 point, PTThumbNail pthumbinfo)
{
	UINT16 number;
	UINT32 i = point;
	UINT32 ifd1Offset = 0;

	number = VIM_JPEG_ToolExifGetWord(ptr+i, pthumbinfo->byteOrder);
	i += 2;

	i += number * 12;										//skip the 12-byte field Interoperability arrays

	ifd1Offset = VIM_JPEG_ToolExifGetDword(ptr+i, pthumbinfo->byteOrder);

	return ifd1Offset;
}

UINT32 VIM_JPEG_ToolGetIFD1(HUGE UINT8* ptr, UINT32 point, PTThumbNail pthumbinfo)
{
	UINT8 unit = 0;
	UINT16 number, j;	
	UINT16	tag, type;
	UINT32	i, nextIfdOffset = 0;
	UINT32	count, offset = 0;	

	TExifIFD1 ifd1info;
	VIM_USER_MemSet(( UINT8 *)&(ifd1info),0,sizeof(TExifIFD1));
       i = point;
	number = VIM_JPEG_ToolExifGetWord(ptr+i, pthumbinfo->byteOrder);
	i += 2;
	
	for(j = 0; j < number; j++)
	{
		tag = VIM_JPEG_ToolExifGetWord(ptr+i, pthumbinfo->byteOrder);
		i += 2;
		type = VIM_JPEG_ToolExifGetWord(ptr+i, pthumbinfo->byteOrder);
		i += 2;
		count = VIM_JPEG_ToolExifGetDword(ptr+i, pthumbinfo->byteOrder);
		i += 4;
		switch(type)
		{
			case EXIF_FORMAT_BYTE:
			case EXIF_FORMAT_SBYTE:
			case EXIF_FORMAT_UNDEFINED:
			case EXIF_FORMAT_ASCII:
				unit = 1;
				offset = (UINT32)ptr[i];
				break;
			case EXIF_FORMAT_SHORT:
			case EXIF_FORMAT_SSHORT:
				unit = 2;
				offset = (UINT32)VIM_JPEG_ToolExifGetWord(ptr+i, pthumbinfo->byteOrder);
				break;
			case EXIF_FORMAT_LONG:
			case EXIF_FORMAT_SLONG:
				unit = 4;
				offset = VIM_JPEG_ToolExifGetDword(ptr+i, pthumbinfo->byteOrder);
				break;
			case EXIF_FORMAT_RATIONAL:
			case EXIF_FORMAT_SRATIONAL:
			case EXIF_FORMAT_FLOAT:
			case EXIF_FORMAT_DOUBLE:
				//currently, we don't need these cases, just bypass them
			default:
				//error
				unit = 0;
				offset = 0;
				break;
		}
		
		switch(tag)				//current support tag data size less than 4, so offset is not used,but it can be extended easily
		{
			case EXIF_TAG_COMPRESSION:
				ifd1info.compression = (UINT16)offset;
				break;
			case EXIF_TAG_IMAGE_WIDTH:
				pthumbinfo->thumbWidth = ifd1info.thumbWidth = offset;
				break;
			case EXIF_TAG_IMAGE_LENGTH:
				pthumbinfo->thumbHeight = ifd1info.thumbHeight = offset;
				break;			
			case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
				ifd1info.PhotometricInterpretation = (UINT16)offset;
				break;
			case EXIF_TAG_YCBCR_SUB_SAMPLING:
				ifd1info.YCbCrSubSampling = (UINT16)offset;
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
				ifd1info.JPEGInterchangeFormat = offset;				
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:				
				ifd1info.JPEGInterchangeFormatLength = offset;
				break;			
			case EXIF_TAG_STRIP_OFFSETS:			//do not support strip store
				ifd1info.StripOffsets = offset;				
				break;
			case EXIF_TAG_STRIP_BYTE_COUNTS:				
				ifd1info.StripByteCounts = offset;
				break;
			case EXIF_TAG_PLANAR_CONFIGURATION:
				ifd1info.PlanarConfiguration = (UINT16)offset;
				break;

			case EXIF_TAG_ROWS_PER_STRIP:				
			case EXIF_TAG_BITS_PER_SAMPLE:				
			case EXIF_TAG_SAMPLES_PER_PIXEL:								
			case EXIF_TAG_YCBCR_POSITIONING:			
				break;
			default:									//currently, we don't need others' info
				break;
		}
		i += 4;	
	}	

	pthumbinfo->thumbtype = THUMBNAIL_EXIF;
	if (ifd1info.compression == 6)
	{
		pthumbinfo->thumbfmt = JPEG422;
		
		pthumbinfo->pthumbbuf = (UINT8*)ptr + ifd1info.JPEGInterchangeFormat;
		pthumbinfo->thumblen = ifd1info.JPEGInterchangeFormatLength;
	}
	else if (ifd1info.compression == 1)				//no compression
	{
		pthumbinfo->pthumbbuf = (UINT8*)ptr + ifd1info.StripOffsets;
		pthumbinfo->thumblen = ifd1info.StripByteCounts;

		if (ifd1info.PhotometricInterpretation == 2)	// RGB
		{
			if (ifd1info.PlanarConfiguration == 1)
				pthumbinfo->thumbfmt = RAW_RGB;
			else
				pthumbinfo->thumbfmt = RAW_RGB_RpGpBp;
		}
		else			//YCbCr
		{
			if (ifd1info.YCbCrSubSampling == 0x22)
			{
                if (ifd1info.PlanarConfiguration == 1)
					pthumbinfo->thumbfmt = YCbCr420_YYYYCbCr;
				else
					pthumbinfo->thumbfmt = YCbCr420_YpCbpCrp;
			}
			else
			{
				if (ifd1info.PlanarConfiguration == 1)
					pthumbinfo->thumbfmt = YCbCr422_YYCbCr;
				else
					pthumbinfo->thumbfmt = YCbCr422_YpCbpCrp;
			}
		}
	}
	else
		pthumbinfo->thumbtype = NO_THUMBNAIL;
    
	nextIfdOffset = VIM_JPEG_ToolExifGetDword(ptr+i, pthumbinfo->byteOrder);

	return nextIfdOffset;
}

void VIM_JPEG_ToolParseExif(HUGE UINT8* ptr, PTThumbNail pthumbinfo)
{
	UINT32 i = 0;
	UINT16 byteOrder;
	UINT32 ifdOffset;
	
	byteOrder = GETWORD(ptr, i);
	i += 2;

	if(byteOrder == 0x4949)
		pthumbinfo->byteOrder = EXIF_BYTE_ORDER_INTEL;
	else if(byteOrder == 0x4D4D)
		pthumbinfo->byteOrder = EXIF_BYTE_ORDER_MOTOROLA;
	else		
		return;		

	i += 2;												//skip '2A00'
	
	ifdOffset = VIM_JPEG_ToolExifGetDword(ptr+i, pthumbinfo->byteOrder);

	ifdOffset = VIM_JPEG_ToolGetIFD0(ptr, ifdOffset, pthumbinfo);

	if(ifdOffset != 0)
		ifdOffset = VIM_JPEG_ToolGetIFD1(ptr, ifdOffset, pthumbinfo);
}

UINT16 VIM_JPEG_ToolGetJfifInfo(HUGE UINT8* ptr, UINT32 point, PTThumbNail pthumbinfo)
{
	UINT32  i;
	UINT16 len;
	UINT8 isJFIF, isJFXX;
	UINT8 extensioncode;

	i      =   point;
	len    =   GETWORD(ptr, point);
	i     +=    2;
	//pjfifinfo->fieldLen =  len;
	if( (ptr[i] == 'J') && (ptr[i+1] == 'F') 
		&& (ptr[i+2] == 'I') && (ptr[i+3] == 'F')
		&& (ptr[i+4] == 0) )
		isJFIF = 1;
	else
		isJFIF = 0;
	if(ptr[i] == 'J' && ptr[i+1] == 'F' 
		&& ptr[i+2] == 'X' && ptr[i+3] == 'X'
		&& ptr[i+4] == 0)
		isJFXX = 1;
	else 
		isJFXX = 0;

	i += 5;
	if(isJFIF)
	{	
		i += 7;		
		pthumbinfo->thumbWidth = ptr[i];
		i++;		
		pthumbinfo->thumbHeight = ptr[i];
		i++;
		pthumbinfo->thumblen = len - 0x10;
			
		if(pthumbinfo->thumblen)
		{
			pthumbinfo->thumbtype = THUMBNAIL_JFIF;
			pthumbinfo->thumbfmt = RGB24;
			pthumbinfo->pthumbbuf = ptr + i;			
		}
	}
	if(isJFXX)
	{		
		extensioncode  =  ptr[i];
		switch(extensioncode)
		{
		case 0x10:
			pthumbinfo->thumbfmt = JPEG422;
			pthumbinfo->pthumbbuf = ptr + point + 0x08;
			pthumbinfo->thumblen = len - 0x08;			
			break;
		case 0x11:
			pthumbinfo->thumbfmt = RGB8;
			pthumbinfo->thumbWidth = ptr[i + 1];
			pthumbinfo->thumbHeight = ptr[i + 2];
			pthumbinfo->ppalette = ptr + point + 0x0a;
			pthumbinfo->pthumbbuf = ptr + point + 0x0a + 0x300;
			pthumbinfo->thumblen = len - 0x0a - 0x300;			
			break;
		case 0x13:
			pthumbinfo->thumbfmt = RGB24;
			pthumbinfo->thumbWidth = ptr[i + 1];
			pthumbinfo->thumbHeight = ptr[i + 2];
			pthumbinfo->pthumbbuf = ptr + point + 0x0a;
			pthumbinfo->thumblen = len - 0x0a;			
			break;
		default: 
			break;
		}
		if(pthumbinfo->thumblen)
			pthumbinfo->thumbtype = THUMBNAIL_JFXX;
	}
	return len;
}

UINT16 VIM_JPEG_ToolGetCompFrmSOF(HUGE UINT8 *buf)
{
	UINT8	h, v, n;
	UINT16	val = 0;
	
	h = (buf[0] & 0x70) >> 4;
	v = buf[0] & 0x3;
	n = buf[1];
	val = (UINT16)(h | (v << 4) | (n << 6)) << 8;
	n = h*v << 2;
	val |= (UINT16)n;
	return val;
}

UINT16 VIM_JPEG_ToolGetSOF0(PVIM_JPEG_Index pinfo, HUGE UINT8 * ptr)
{
	UINT16 len = GETWORD(ptr,0);
	UINT8 temp;

	pinfo->data_precision = ptr[2];
	pinfo->ImageSize.cy = GETWORD(ptr,3);
	pinfo->ImageSize.cx = GETWORD(ptr,5);
	pinfo->CompCount = ptr[7];
	temp=ptr[8];//angela 2007-1-11
	if(pinfo->CompCount == 1)
	{
		if((ptr[8]-temp) > 3)
			return 0;
		pinfo->YUVType = JPEG_400;
		pinfo->Comp[ ptr[8]-temp ] = VIM_JPEG_ToolGetCompFrmSOF(ptr+9);;
	}
	else
	{
		if( ((ptr[11] - temp) > 3) || ((ptr[14] - temp) > 3) || ((ptr[8] - temp) > 3) )
			return 0;
		pinfo->Comp[ ptr[8]-temp ] = VIM_JPEG_ToolGetCompFrmSOF(ptr+9);
		if((ptr[9] == 0x21) )//|| (ptr[9] == 0x12))	//angela 2007-3-6
			pinfo->YUVType = JPEG_422;
		else if(ptr[9] == 0x22)
			pinfo->YUVType = JPEG_420;
		else if(ptr[9] == 0x41)
			pinfo->YUVType = JPEG_411;
		else if(ptr[9] == 0x11)
			pinfo->YUVType = JPEG_444;
		else
			return 0;
		pinfo->Comp[ ptr[11]-temp ] = VIM_JPEG_ToolGetCompFrmSOF(ptr+12);
		pinfo->Comp[ ptr[14]-temp ] = VIM_JPEG_ToolGetCompFrmSOF(ptr+15);
	}
	
	VIM_JPEG_GetJpegSize((VIM_JPEG_YUVMODE)pinfo->YUVType, pinfo->ImageSize, &(pinfo->JpgSize));
       //VIM_JPEG_GetJpegSize((VIM_JPEG_YUVMODE)pinfo->YUVType, pinfo->JpgSize, &size1);
	pinfo->vwc = VIM_JPEG_GetYuvSize((VIM_JPEG_YUVMODE)pinfo->YUVType, pinfo->JpgSize) >> 2;
	pinfo->blkcount = VIM_JPEG_GetJpegBlockNum((VIM_JPEG_YUVMODE)pinfo->YUVType, pinfo->JpgSize);

	return len;
}

UINT16 VIM_JPEG_ToolGetDQT(PVIM_JPEG_Index pinfo, HUGE UINT8 * ptr)
{
	UINT16 len, i = 0;
	UINT8	n;
	
	len = GETWORD(ptr,0);
	
	i += 2;
	while(i < len)
	{
		if(ptr[i] & 0xf0)		//16 bit precision
		{
			n = ptr[i] & 0xf;
			if(n > 3)
				return 0;
			pinfo->QTPrecision[n] = 16;
			pinfo->QT[n] = ptr+i+1;
			i += 1 + (64 << 1);
		}
		else		//8 bit precision
		{
			n = ptr[i] & 0xf;
			if(n > 3)
				return 0;
			pinfo->QTPrecision[n] = 8;
			pinfo->QT[n] = ptr+i+1;
			i += 1 + 64;
		}
		pinfo->QTCount++;
	}
	return len;
}

UINT16 VIM_JPEG_ToolGetDHT(PVIM_JPEG_Index pinfo,HUGE UINT8 * ptr)
{
	UINT16 len, j, n, k, HTLen;
//	UINT16 tablen[4] = {0x1c, 0xb2, 0x1c, 0xb2};
	UINT8 *pd = ptr+2;	
	
	j = 0;
	len = GETWORD(ptr,0);

	while(j < len-3)
	{
		HTLen = 0;
		n = pd[j] & 0xf;
		k = pd[j] & 0xf0;
		n <<= 1;
		n += k >> 4;
		if(n > 3)
			return 0;
		j++;
		pinfo->HT[n] = pd+j;
		k = j + 16;
		while(j < k)
		{
			HTLen = HTLen + (UINT16)(pd[j]);
			j++;
		}
		pinfo->HTLen[n] = (UINT8)(16 + HTLen);
		j = j+HTLen;
		pinfo->HTCount++;
	}
	return len;
}

UINT16 VIM_JPEG_ToolGetSOS(PVIM_JPEG_Index pinfo,  HUGE UINT8 * ptr)
{
	UINT16 len;
	UINT8 i, k, n, ac, dc, count, *pbuf = ptr+3,temp;	
	
	len = GETWORD(ptr,0);
	count = ptr[2];
	if(len != 6 + (count << 1))
		return 0;
	temp=pbuf[0];	//angela 2007-1-11
	for(i = 0; i < count; i++)
	{
		k = i << 1;
		n = pbuf[k]-temp;
		if(n > 3)
			return 0;
		ac = pbuf[k+1] & 0xf;
		dc = pbuf[k+1] & 0xf0;
		if(!ac)
			pinfo->Comp[n] |= 0x2;
		if(!dc)
			pinfo->Comp[n] |= 0x1;
	}
	return len;
}

UINT16 VIM_JPEG_ToolGetSOFUnsupport(PVIM_JPEG_Index pinfo, HUGE UINT8 * ptr)
{
	UINT16 len = ((UINT16)ptr[0] << 8) + (UINT16)ptr[1];
	
	//unsupported pic, set everything to zero, YUVType to unknown and bypass the length
	pinfo->data_precision = 0;
	pinfo->ImageSize.cy = 0;
	pinfo->ImageSize.cx = 0;
	pinfo->CompCount = 0;
	pinfo->YUVType = JPEG_UNKNOWN;
	return len;
}

UINT16 VIM_JPEG_ToolGetComment(PVIM_JPEG_Index pinfo, HUGE UINT8 *ptr)
{
	UINT16 len = GETWORD(ptr, 0);

	pinfo->frmcnt = GETWORD_INV(ptr, 2);
	pinfo->qf = GETBYTE(ptr, 25);

	return len;
}

UINT16 VIM_JPEG_ToolByPassMarker(HUGE UINT8 * ptr)
{
	UINT16 len = GETWORD(ptr,0);
	return len;
}

void VIM_JPEG_ToolJpegParse(HUGE UINT8 *ptr, UINT32 length, PVIM_JPEG_Index pinfo,UINT8 FindEnd)
{
	UINT32 i = 0;
	UINT16 len;
	VIM_USER_MemSet(( UINT8 *)pinfo,0,sizeof(VIM_JPEG_Index));
	pinfo->YUVType = JPEG_UNKNOWN;
	pinfo->thumbinfo.thumbtype = NO_THUMBNAIL;
	pinfo->frmEnd = FALSE;
	//try to find the 1st marker SOI
	while(i < length)
	{
		if(ptr[i]==0xff)
		{
			if(ptr[i+1]==0xd8)
				break;
		}
		i++;
	}
	//find the SOI marker, start parsing
	while(i < length)
	{
		if(ptr[i] == 0xff)
		{
			do
			{
				i++;
			}
			while(ptr[i]==0xff);
			
			switch(ptr[i])
			{
			case M_SOI:								/* jpeg start header */
				pinfo->sop = i-1;
				break;
			case M_APP1:							/* App1 is used for EXIF */
				len = VIM_JPEG_ToolGetExifInfo(ptr, i+1, &pinfo->thumbinfo);
				i += len;
				break;
			case M_APP0:							/* App0 is used for JFIF */
				len = VIM_JPEG_ToolGetJfifInfo(ptr, i+1, &pinfo->thumbinfo);
				i += len;
				break;
			case M_SOF0:							/* Baseline */
				len = VIM_JPEG_ToolGetSOF0(pinfo, ptr+i+1);
				if(len == 0)
				{
					pinfo->YUVType = JPEG_UNKNOWN;
					pinfo->eop = i;
					return;
				}
				i += len;
				break;
			case M_DQT:								/* segment DQT */
				len = VIM_JPEG_ToolGetDQT(pinfo, ptr+i+1);
				if(len == 0)
				{
					pinfo->YUVType = JPEG_UNKNOWN;
					pinfo->eop = i;
					return;
				}
				i += len;
				break;
			case M_DHT:								/* segment DHT */
				len = VIM_JPEG_ToolGetDHT(pinfo, ptr+i+1);
				if(len == 0)
				{
					pinfo->YUVType = JPEG_UNKNOWN;
					pinfo->eop = i;
					return;
				}
				i += len;
				break;
			case M_SOS:								/* segment SOS */
				//if (pinfo->HTCount != 4)		//angela 2007-3-5
				//{
				//	pinfo->YUVType = JPEG_UNKNOWN;
				//	pinfo->eop = i;
				//	return;
				//}
				len = VIM_JPEG_ToolGetSOS(pinfo, ptr+i+1);
				if(len)
				{
					i += len;
					pinfo->offset = i+1;
				}

				//angela changed
				if(FindEnd)
				{
					while(i < (length-1))
					{
						if(ptr[i]==0xff)
						{
							if(ptr[i+1]==0xd9)
							{
								pinfo->eop = i+2;
								pinfo->frmEnd = TRUE;
								return;
							}
						}
						i++;
					}
					pinfo->eop = i;
				}
				else
				{
					pinfo->eop = i;
				}
				return;
				

				break;
			case M_EOI:
				pinfo->eop = i+1;
				pinfo->frmEnd = TRUE;
				return;
			case M_SOF1:		/* Extended sequential, Huffman */
			case M_SOF2:		/* Progressive, Huffman */
			case M_SOF9:		/* Extended sequential, arithmetic */
			case M_SOF10:		/* Progressive, arithmetic */
			case M_SOF3:		/* Lossless, Huffman */
			case M_SOF5:		/* Differential sequential, Huffman */
			case M_SOF6:		/* Differential progressive, Huffman */
			case M_SOF7:		/* Differential lossless, Huffman */
			case M_SOF11:		/* Lossless, arithmetic */
			case M_SOF13:		/* Differential sequential, arithmetic */
			case M_SOF14:		/* Differential progressive, arithmetic */
			case M_SOF15:		/* Differential lossless, arithmetic */
			case M_JPG:			/* Reserved for JPEG extensions */
								/* Currently unsupported SOFn types */
								/* set YUV_TYPE = unknown */
				len = VIM_JPEG_ToolGetSOFUnsupport(pinfo, ptr+i+1);
				i += len;
				break;
			case M_COM:			/* Comment */
				len = VIM_JPEG_ToolGetComment(pinfo, ptr+i+1);
				i += len;
				break;
			case M_DAC:			/* Define arithmetic coding conditioning */
			case M_DRI:			/* Define restart interval */
			case M_APP2:		/* Reserved for application segments */
			case M_APP3:
			case M_APP4:
			case M_APP5:
			case M_APP6:
			case M_APP7:
			case M_APP8:
			case M_APP9:
			case M_APP10:
			case M_APP11:
			case M_APP12:
			case M_APP13:
			case M_APP14:
			case M_APP15:
			case M_RST0:		/* these are all parameterless */
			case M_RST1:
			case M_RST2:
			case M_RST3:
			case M_RST4:
			case M_RST5:
			case M_RST6:
			case M_RST7:
			case M_DNL:			/* Ignore DNL ... perhaps the wrong thing */
								/* Currently unsupported, bypass! */
				len = VIM_JPEG_ToolByPassMarker(ptr+i+1);
				i += len;
				break;
			default:
								/* error! You could show error here */
								/* set YUVType = Unknown, and return */
				break;
			}
		}
		i++;
	}
	pinfo->YUVType = JPEG_UNKNOWN;
	pinfo->eop = i;
}
//from here define for yuv to rgb
/******************************************* yuv2rgb **********************************************/
const UINT16 gTableV2R[]  =
{
	0,   1,   2,   4,   5,   7,   8,   9,  11,  12,  14,  15,  16,  18,  19,  21, 
	22,  23,  25,  26,  28,  29,  30,  32,  33,  35,  36,  37,  39,  40,  42,  43, 
	44,  46,  47,  49,  50,  51,  53,  54,  56,  57,  58,  60,  61,  63,  64,  65, 
	67,  68,  70,  71,  72,  74,  75,  77,  78,  79,  81,  82,  84,  85,  86,  88, 
	89,  91,  92,  93,  95,  96,  98,  99, 100, 102, 103, 105, 106, 107, 109, 110, 
	112, 113, 114, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 131, 133, 
	134, 136, 137, 138, 140, 141, 143, 144, 145, 147, 148, 150, 151, 152, 154, 155, 
	157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 173, 175, 176, 178, 
	179, 180, 182, 183, 185, 186, 187, 189, 190, 192, 193, 194, 196, 197, 199, 200, 
	201, 203, 204, 206, 207, 208, 210, 211, 213, 214, 215, 217, 218, 220, 221, 222, 
	224, 225, 227, 228, 229, 231, 232, 234, 235, 236, 238, 239, 241, 242, 244, 245, 
	246, 248, 249, 251, 252, 253, 255, 256, 258, 259, 260, 262, 263, 265, 266, 267,
	269, 270, 272, 273, 274, 276, 277, 279, 280, 281, 283, 284, 286, 287, 288, 290,
	291, 293, 294, 295, 297, 298, 300, 301, 302, 304, 305, 307, 308, 309, 311, 312,
	314, 315, 316, 318, 319, 321, 322, 323, 325, 326, 328, 329, 330, 332, 333, 335, 
	336, 337, 339, 340, 342, 343, 344, 346, 347, 349, 350, 351, 353, 354, 356, 357 
};

const UINT16 gTableU2G[]  =
{
	0,   0,   0,   1,   1,   1,   2,   2,   2,   3,   3,   3,   4,   4,   4,   5, 
	5,   5,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10, 
	11,  11,  11,  12,  12,  12,  13,  13,  13,  14,  14,  14,  15,  15,  15,  16, 
	16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,  20,  20,  21,  21, 
	22,  22,  22,  23,  23,  23,  24,  24,  24,  25,  25,  25,  26,  26,  26,  27, 
	27,  27,  28,  28,  28,  29,  29,  29,  30,  30,  30,  31,  31,  31,  32,  32, 
	33,  33,  33,  34,  34,  34,  35,  35,  35,  36,  36,  36,  37,  37,  37,  38, 
	38,  38,  39,  39,  39,  40,  40,  40,  41,  41,  41,  42,  42,  42,  43,  43, 
	44,  44,  44,  45,  45,  45,  46,  46,  46,  47,  47,  47,  48,  48,  48,  49, 
	49,  49,  50,  50,  50,  51,  51,  51,  52,  52,  52,  53,  53,  53,  54,  54, 
	55,  55,  55,  56,  56,  56,  57,  57,  57,  58,  58,  58,  59,  59,  59,  60, 
	60,  60,  61,  61,  61,  62,  62,  62,  63,  63,  63,  64,  64,  64,  65,  65, 
	66,  66,  66,  67,  67,  67,  68,  68,  68,  69,  69,  69,  70,  70,  70,  71, 
	71,  71,  72,  72,  72,  73,  73,  73,  74,  74,  74,  75,  75,  75,  76,  76, 
	77,  77,  77,  78,  78,  78,  79,  79,  79,  80,  80,  80,  81,  81,  81,  82, 
	82,  82,  83,  83,  83,  84,  84,  84,  85,  85,  85,  86,  86,  86,  87,  87
};

const UINT16 gTableV2G[]  =
{
	0,   0,   1,   2,   2,   3,   4,   5,   5,   6,   7,   7,   8,   9,  10,  10, 
	11,  12,  12,  13,  14,  15,  15,  16,  17,  17,  18,  19,  20,  20,  21,  22, 
	22,  23,  24,  25,  25,  26,  27,  27,  28,  29,  30,  30,  31,  32,  32,  33, 
	34,  35,  35,  36,  37,  37,  38,  39,  40,  40,  41,  42,  42,  43,  44,  45, 
	45,  46,  47,  47,  48,  49,  50,  50,  51,  52,  52,  53,  54,  55,  55,  56, 
	57,  57,  58,  59,  60,  60,  61,  62,  62,  63,  64,  65,  65,  66,  67,  67, 
	68,  69,  70,  70,  71,  72,  72,  73,  74,  75,  75,  76,  77,  77,  78,  79, 
	80,  80,  81,  82,  82,  83,  84,  85,  85,  86,  87,  87,  88,  89,  90,  90, 
	91,  92,  92,  93,  94,  95,  95,  96,  97,  97,  98,  99,  100, 100, 101, 102, 
	102, 103, 104, 105, 105, 106, 107, 107, 108, 109, 110, 110, 111, 112, 112, 113, 
	114, 115, 115, 116, 117, 117, 118, 119, 120, 120, 121, 122, 122, 123, 124, 125, 
	125, 126, 127, 127, 128, 129, 130, 130, 131, 132, 132, 133, 134, 135, 135, 136, 
	137, 137, 138, 139, 140, 140, 141, 142, 142, 143, 144, 145, 145, 146, 147, 147, 
	148, 149, 150, 150, 151, 152, 152, 153, 154, 155, 155, 156, 157, 157, 158, 159, 
	160, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 167, 168, 169, 170, 170, 
	171, 172, 172, 173, 174, 175, 175, 176, 177, 177, 178, 179, 180, 180, 181, 182
};

const UINT16 gTableU2B[]  =
{
	0,   1,   3,   5,   7,   8,  10,  12,  14,   15,  17,  19,  21,  23,  24,  26, 
	28,  30,  31,  33,  35,  37,  39,  40,  42,  44,  46,  47,  49,  51,  53,  54, 
	56,  58,  60,  62,  63,  65,  67,  69,  70,  72,  74,  76,  78,  79,  81,  83, 
	85,  86,  88,  90,  92,  93,  95,  97,  99,  101, 102, 104, 106, 108, 109, 111, 
	113, 115, 117, 118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 
	141, 143, 145, 147, 148, 150, 152, 154, 156, 157, 159, 161, 163, 164, 166, 168, 
	170, 172, 173, 175, 177, 179, 180, 182, 184, 186, 187, 189, 191, 193, 195, 196, 
	198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218, 219, 221, 223, 225, 
	227, 228, 230, 232, 234, 235, 237, 239, 241, 242, 244, 246, 248, 250, 251, 253, 
	255, 257, 258, 260, 262, 264, 266, 267, 269, 271, 273, 274, 276, 278, 280, 281, 
	283, 285, 287, 289, 290, 292, 294, 296, 297, 299, 301, 303, 305, 306, 308, 310, 
	312, 313, 315, 317, 319, 320, 322, 324, 326, 328, 329, 331, 333, 335, 336, 338, 
	340, 342, 344, 345, 347, 349, 351, 352, 354, 356, 358, 360, 361, 363, 365, 367, 
	368, 370, 372, 374, 375, 377, 379, 381, 383, 384, 386, 388, 390, 391, 393, 395, 
	397, 399, 400, 402, 404, 406, 407, 409, 411, 413, 414, 416, 418, 420, 422, 423, 
	425, 427, 429, 430, 432, 434, 436, 438, 439, 441, 443, 445, 446, 448, 450, 452
};

UINT16	 VIM_JPEG_ToolYuv2R(UINT16 *yuv)
{
	INT16 val;
       val = yuv[0] + gTableV2R[yuv[2]] - 179;
	return val > 255 ? 255 : ( val < 0 ? 0 : val );
}

UINT16	 VIM_JPEG_ToolYuv2G(UINT16 *yuv)
{
	INT16 val;
	val = yuv[0] + 135 - (gTableU2G[yuv[1]] + gTableV2G[yuv[2]]);
	return val > 255 ? 255 : ( val < 0 ? 0 : val );
}

UINT16 VIM_JPEG_ToolYuv2B(UINT16 *yuv)
{
	INT16 val;
	val = yuv[0] + gTableU2B[yuv[1]] - 227;
	return val > 255 ? 255 : ( val < 0 ? 0 : val );
}
void VIM_JPEG_ToolYuv2Rgb(UINT16 *yuv, UINT16 *bgr)
{
	bgr[2] = VIM_JPEG_ToolYuv2R(yuv);
	bgr[1] = VIM_JPEG_ToolYuv2G(yuv);
	bgr[0] = VIM_JPEG_ToolYuv2B(yuv);
}
void VIM_JPEG_Toolyuv422_Rgb565(HUGE UINT8 *src, HUGE UINT8 *dest, UINT16 width, UINT16 height)
{
	UINT16 i, size;
	UINT16 temp;
	UINT16 yuv[6],bgr[3];

	size = 4;
	height *= width << 1;
	width = 0;
	for(i = 0; i < height; i += size)
	{
		yuv[0]=*(src+i+1); //y;
		yuv[1]=*(src+i+0); //u;
		yuv[2]=*(src+i+2); //v;
		yuv[3]=*(src+i+3); //y;
		yuv[4]=yuv[1];
		yuv[5]=yuv[2];	
		VIM_JPEG_ToolYuv2Rgb(yuv, bgr);
		temp=(((bgr[2]>>3)&0x1f)<<11) |( ((bgr[1]>>2)&0x3f)<<5) |  ((bgr[0]>>3)&0x1f);
		*dest++=(char)temp;		
		*dest++=(char)(temp>>8);

		VIM_JPEG_ToolYuv2Rgb(yuv + 3, bgr);
		temp=(((bgr[2]>>3)&0x1f)<<11) |( ((bgr[1]>>2)&0x3f)<<5) |  ((bgr[0]>>3)&0x1f);
		*dest++=(char)temp;	
		*dest++=(char)(temp>>8); 
	}
}
