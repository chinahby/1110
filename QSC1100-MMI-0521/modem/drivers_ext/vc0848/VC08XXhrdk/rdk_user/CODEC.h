#ifndef CODEC
#define CODEC
/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : codec.c
 * [Description]        : Audio codec driver
 *				
 * [Author]                 : Zhang Xuecheng
 * [Date Of Creation]  : 2007-10-09 
 * [Platform]               : Nu820 MP
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     			Modifications
 * ------------------------------------------------------------------------------
 * 2007-10-09  		Zhang Xuecheng      	Created
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
/* audio codec diffrence type define, define the type must assure the driver is ok*/
#define V0820_CODEC_DEFAULT		0	/* define by Amanda deng, it's driver finished in 2007.02.07*/
#define V0820_CODEC_WM8750		1 	/* define by Amanda deng, it's driver finished in 2007.02.07*/
#define V0820_CODEC_INTERNAL		2	/* zhang xuecheng,2007-12-18 13:33:52 */

/* audio codec type select by user*/
#define V0820_AUDIOCODEC_TYPE		V0820_CODEC_INTERNAL	

//==========================================================================
#define CODEC_LEFTCHANNEL			(0x01)
#define CODEC_RIGHTCHANNEL			(0x01<<1)
#define CODEC_STEROECHANNEL		(CODEC_LEFTCHANNEL | CODEC_RIGHTCHANNEL)
//////
#define CODEC_SPEAKEROUT			(0x01)
#define CODEC_HEADPHONEOUT		(0x01<<1)
#define CODEC_SPEAKER_HP_OUT		(CODEC_SPEAKEROUT | CODEC_HEADPHONEOUT)
//add by hbp
#define CODEC_LINEIN					(0x01)		//Line Input 2
#define CODEC_MICPHONEIN			(0x02)		//±£Áô¾É°æ¼æÈÝ(²î·ÖMICÊäÈë)
#define CODEC_MICIN_SINGLE			(0x03)		//µ¥¶ËMICÊäÈë
#define CODEC_MICIN_DIFFERENCE		(0x04)		//²î·ÖMICÊäÈë
///////

#define CODEC_LATCH_COUNT			800
#include "mmd_data_type.h"
#if (V0820_AUDIOCODEC_TYPE == V0820_CODEC_INTERNAL)
void   Internal_Codec_Initialize(void);
void   Internal_Codec_Exit(void);
void   Internal_Codec_Set_PlayMode(MMD_U32 nCahnnel, MMD_U32 nOutType);
void   Internal_Codec_Stop_PlayMode(void);
void   Internal_Codec_Set_RecordMode(MMD_U32 nCahnnel, MMD_U32 nInputType);
void   Internal_Codec_Stop_RecordMode(void);
void   Internal_Codec_DAC_Volume(MMD_U32 vLeft, MMD_U32 vRight);
void   Internal_Codec_ADC_Volume(MMD_U32 vLeft, MMD_U32 vRight);
void	 Internal_Codec_Set_LineinoutMode(void);
void   Internal_Codec_Disable_Mute(MMD_U32 mode);
void   Internal_Codec_Enable_Mute(MMD_U32 mode);
void MMD_USE_FM_Flag(MMD_U08 flag); //zhangxiang add for fm

#endif
#endif

