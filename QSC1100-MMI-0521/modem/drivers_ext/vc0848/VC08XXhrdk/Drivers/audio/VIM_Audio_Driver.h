/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_Audio_Driver.h
 * [Description]        : Audio driver header file
 *				
 * [Author]                 : Haobaopeng
 * [Date Of Creation]  :   2009-02-04
 * [Platform]               : Nu820 MP
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     			Modifications
 * ------------------------------------------------------------------------------
 * 2009-02-04 		Hao Baopeng      		Created
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

typedef enum A_MODE_ENUM
{
	SLAV,
	MASTER,
	MODE_ERROR = 0x7fffffff
}A_MODE;

typedef enum A_WDLEN_ENUM
{
	B16,
	B8,
	B12,
	B18,
	B20,
	B24,
	B32,
	WDLEN_ERROR = 0x7fffffff
}A_WDLEN;

typedef enum A_MSBFST_ENUM
{
	LSBFST,
	MSBFST,
	MSBFST_ERROR = 0x7fffffff
}A_MSBFST;

typedef enum A_CHN_ENUM
{
	A_STEREO,
	A_MONO,
	CHN_ERROR = 0x7fffffff
}A_CHN;

typedef enum A_WSWIDTH_ENUM
{
	FS16,
	FS32,
	FS64,
	WSWIDTH_ERROR = 0x7fffffff
}A_WSWIDTH;

typedef enum A_PCMDATAFMT_ENUM
{
	PCM16,
	PCM8,
	PCM32,
	PCMDATAFMT_ERROR = 0x7fffffff
}A_PCMDATAFMT;
 
typedef enum A_SAMRATE_ENUM
{
	SAM48K,
	SAM8K,
	SAM11K,
	SAM12K,
	SAM16K,
	SAM22K,
	SAM24K,
	SAM32K,
	SAM44K,
	SAM88K,
	SAM96K,
	SAMRATE_ERROR = 0x7fffffff
}A_SAMRATE;

typedef enum A_FSTWS_ENUM
{
	SECSCLK,
	FSTSCLK,
	FSTWS_ERROR = 0x7fffffff
}A_FSTWS;

typedef enum A_ALIGN_ENUM
{
	RIGHT,
	LEFT,
	I2SM,
	ALIGN_ERROR = 0x7fffffff
}A_ALIGN;

typedef enum A_CODEC_ENUM
{
	EX_CODEC,
	IN_CODEC,
	CODEC_ERROR = 0x7fffffff
}A_CODEC;

typedef enum A_IF_ENUM
{
	I2S_IF,
	PCM_IF,
	IF_ERROR = 0x7fffffff
}A_IF;

typedef enum A_DUTECYCLE_ENUM
{
	D0,
	D15,
	D31,
	D63,
	DUTECYCLE_ERROR = 0x7fffffff
}A_DUTECYCLE;

typedef enum A_EDGE_MODE_ENUM
{
	EDGE0,
	EDGE1,
	EDGE_MODE_ERROR = 0x7fffffff
}A_EDGE_MODE;

/* audio interface playback parameter info.*/
typedef struct audio_output_param{
	unsigned int msMode;
	unsigned int I2SMode;
    	unsigned int wdLen;
    	unsigned int msbFst;
    	unsigned int mono;
    	unsigned int fifoDepth;
    	unsigned int pcmDataFmt;
    	unsigned int samRate;
    	unsigned int fstWs;
    	unsigned int wsWidth;
    	unsigned int edgemode;
}audio_output_param_t;

typedef struct audio_input_param{
	unsigned int msMode;
    	unsigned int wdLen;
    	unsigned int msbFst;
    	unsigned int mono;
    	unsigned int fifoDepth;
    	unsigned int pcmDataFmt;
    	unsigned int samRate;
    	unsigned int fstWs;
    	unsigned int wsWidth;
    	unsigned int edgemode;
	unsigned int align;
}audio_input_param_t;

typedef enum{
	AU_IF_IDLE,
	AU_IF_START,
	AU_IF_STOP,
	AU_IF_ERROR = 0x7fffffff
}audio_if_stat_t;

typedef struct audio_if{
	audio_output_param_t 	output_param;
	audio_input_param_t  	input_param;
	audio_if_stat_t 		stat;
}audio_if_t;

/*======================================================================
	 v8 audio interface return define . Zhang Xuecheng 2007/10/10
=======================================================================*/
typedef enum {
	AU_SUCCESS				= 0,	
	AU_ERROR				= 1,
	AU_STATUS_ERROR 		= 2,
	AU_CLIENT_MAX_NUM 	= 3,
	AU_SRC_BUF_EMPTY 		= 4,
	AU_DES_BUF_FULL 		= 5,
	AU_OTHER_CLIENT_ON 	= 6,
	AU_CLIENT_WORKING   	= 7,
	AU_NO_SUCH_CLIENT   	= 8,
	AU_RESERVE				= 0x7fffffff
}audio_return_t;

/*======================================================================
	 v8 audio interface mode define . Zhang Xuecheng 2007/10/10
=======================================================================*/
typedef enum{
	AUDIO_MODE_PLAY_REC,
	AUDIO_MODE_PLAY,
	AUDIO_MODE_REC,
	AUDIO_MODE_UNKNOWN,
	AUDIO_MODE_ERROR = 0x7fffffff
}audio_work_mode_t;
//=========================================//
extern unsigned int	gAudio_RecChSel;
extern unsigned int	PlayMixFlag;
//---------------------------------------------//
void VIM_Audio_ClkConfig(unsigned int sampleRate);
void VIM_Audio_PlayMode_Cfg(audio_output_param_t  pParam);
void VIM_Audio_PlayModeIf_Cfg(MMD_U32 IfMode);
void VIM_Audio_RecordMode_Cfg(audio_input_param_t  rParam);
void VIM_Audio_FM_Cfg(unsigned int sampleRate, unsigned int MixFlag);