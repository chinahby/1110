/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : audio.h
 * [Description]        : Audio driver header file
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
#ifndef AUDIO_H
#define AUDIO_H
#include "mmd_data_type.h"

#define  AU_SUC   0
#define  AU_FAIL (-1)
#define  AU_BIT(x)  (1 << (x))      
/* this macro sets a bit in the specified register or memory location*/
#define SETBIT(reg,bit) ((reg)|=(bit))

/* this macro clrs a bit in the specified register or memory location*/
#define CLRBIT(reg,bit) ((reg)&=(~bit))

/* this macro toggles a bit in the spec'd register or memory location*/
#define TGLBIT(reg,bit) reg^=bit

/* this macro tests a bit*/
#define TSTBIT(reg,bit) return (reg&bit); 

/* set v8 register bit function*/ 
#define SET_V8_REG(reg,bit) { MMD_U32 val;\
                                  val=VIM_HIF_GetReg32(reg);\
                                  SETBIT(val,bit); \
                                  VIM_HIF_SetReg32(reg,val);\
                            }  
/* clear v8 register bit function */ 
#define CLR_V8_REG(reg,bit) { MMD_U32 val;\
                                  val=VIM_HIF_GetReg32(reg);\
                                  CLRBIT(val,bit); \
                                  VIM_HIF_SetReg32(reg,val);\
                            } 
/* toggle v8 register bit function*/ 
#define TGL_V8_REG(reg,bit) { MMD_U32 val;\
                                  val=VIM_HIF_GetReg32(reg);\
                                  TGLBIT(val,bit); \
                                  VIM_HIF_SetReg32(reg,val);\
                            } 
/* test v8 register bit function */
#define TST_V8_REG(reg,bit) { MMD_U32 val;\
                                  val=VIM_HIF_GetReg32(reg);\
                                  TSTBIT(val,bit); \
                                  VIM_HIF_SetReg32(reg,val);\
                            } 
                        
/* audio param configurate refer macro define*/
#define PLAY_EN()    {SET_V8_REG(V5_REG_AUD_START,AU_BIT(0));}
#define PLAY_DIS()   {CLR_V8_REG(V5_REG_AUD_START,AU_BIT(0));}
#define RECORD_EN()  {SET_V8_REG(V5_REG_AUD_START,AU_BIT(1));}
#define RECORD_DIS() {SET_V8_REG(V5_REG_AUD_START,AU_BIT(1));}

typedef enum _MMD_AUDIOCODEC_CHANNEL{
	MMD_AUDIOCODEC_LCHANNEL=0,
	MMD_AUDIOCODEC_RCHANNEL,
	MMD_AUDIOCODEC_CHANNEL_ERROR = 0x7fffffff
}MMD_AUDIOCODEC_CHANNEL;
typedef enum
{
	MMD_AUDIOCODEC_FM_MIX_DIS =0,
	MMD_AUDIOCODEC_FM_MIX_EN
}MMD_AUDIOCODEC_FM_MIX;

/*======================================================================
	 v8 audio interface channel define . Amanda Deng 2007/02/08
=======================================================================*/
/*//by hbp 2009-02-09
typedef enum{
	STEREO     = 1,		
	MONO_LEFT  = 2,	
	MONO_RIGHT = 3,
	ch_sel_t_RESERVED = 0x7fffffff	
}ch_sel_t;
*/ 
/*======================================================================
	 v8 audio interface bit width define . Amanda Deng 2007/02/08
=======================================================================*/
typedef	enum{
	BIT_DEPTH_8   = 8,
	BIT_DEPTH_16  = 16,
	BIT_DEPTH_32  = 32,
	bit_depth_t_RESERVED = 0x7fffffff		
}bit_depth_t;



typedef struct _codec_interface_s_{
      	void		(*codec_initialize)(void); 
	void		(*codec_exit)(void); 
      	void		(*codec_set_playmode)(MMD_U32 nCahnnel, MMD_U32 nOutType);
	void		(*codec_stop_playmode)(void);
	void   	(*codec_set_recordmode)(MMD_U32 nCahnnel, MMD_U32 nInputType);
	void   	(*codec_stop_recordmode)(void);
	void   	(*codec_set_lineinoutmode)(void);
	void   	(*codec_DAC_volume)(MMD_U32 vLeft, MMD_U32 vRight);
	void   	(*codec_ADC_volume)(MMD_U32 vLeft, MMD_U32 vRight);
	void		(*codec_disable_mute)(MMD_U32 mode);
	void		(*codec_enable_mute)(MMD_U32 mode);
	MMD_U32 OutType;		//Êä³öÀàÐÍ
	MMD_U32 PlyFlag;	
	MMD_U32 PlyMuteFlag;	
	MMD_U32 RecFlag;
	MMD_U32 RecMuteFlag;	
	MMD_U16 Codec_PlyLChVol;	//Playback left channel volume (0~100)
	MMD_U16 Codec_PlyRChVol;	//Playback right channel volume (0~100)
	MMD_U16 Codec_RecLChVol;	//Record left channel volume (0~100)
	MMD_U16 Codec_RecRChVol;	//Record right channel volume (0~100)
}codec_interface_t, *pcodec_interface_t; 

extern codec_interface_t audio_codec;
//MMD_U32 audio_codec_latch(void);
/*======================================================================
	 v8 audio interface default paramter define . Amanda Deng 2007/02/08
=======================================================================*/
#define 	SAMPLE_RATE_DEFAULT			48000
#define 	AUDIO_FIFO_DEPTH_DEFAULT	31
#define 	WORD_LENGTH_DEFAULT			16

#define SYS_BASE_ADDR        0x60010000
#define CKD_BP_CTL			(SYS_BASE_ADDR+0x0108)
#define PLL_CTL			 	(SYS_BASE_ADDR+0x0000)
#define CKD_EN_CTL			(SYS_BASE_ADDR+0x0104)
#define CLK_SWITCH			(SYS_BASE_ADDR+0x0100)
#define CGT_MC_CTL			(SYS_BASE_ADDR+0x0114)
#define SW_RST			 	(SYS_BASE_ADDR+0x0300)
#define CKD_AT_CON			(SYS_BASE_ADDR+0x0128)
#define CKD_CODE_CON		(SYS_BASE_ADDR+0x013c)
#define CKD_SCLK_CON		(SYS_BASE_ADDR+0x0138)
#define RTC_CTL				(SYS_BASE_ADDR+0x1414)
#define PLL_CTRL                     (SYS_BASE_ADDR+0x000)
#define PLL2_CKD                     (SYS_BASE_ADDR+0x008)
#define PLL2_SETV                   (SYS_BASE_ADDR+0x00c)
#define PLL_WAIT                      (SYS_BASE_ADDR+0x0014)
#define PLL_STATUS                          (SYS_BASE_ADDR+0x0010)

#endif //AUDIO_H
