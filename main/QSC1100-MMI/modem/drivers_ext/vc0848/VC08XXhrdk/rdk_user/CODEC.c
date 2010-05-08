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
#include "mmd_audio.h"
#include "codec.h"

#include"VIM_COMMON.h"
#include"OEMCFGI.h"

#define VIM_CODEC_WriteReg( _register_, _value_ )		HAL_WRITE_UINT32(_register_,_value_);	audio_codec_latch();
#define CREATE_MASK(msb, lsb)       (((1U << ((msb) - (lsb) + 1)) - 1) << (lsb))
#define MASK_BITS(arg, msb, lsb)    ((arg) & CREATE_MASK(msb, lsb))
#define EXTRACT_BITS(arg, msb, lsb) (MASK_BITS(arg, msb, lsb) >> (lsb))

//===============================//
 #if (V0820_AUDIOCODEC_TYPE == V0820_CODEC_INTERNAL)
 codec_interface_t audio_codec =
{
	Internal_Codec_Initialize,
	Internal_Codec_Exit,
	Internal_Codec_Set_PlayMode,
	Internal_Codec_Stop_PlayMode,
	Internal_Codec_Set_RecordMode,
	Internal_Codec_Stop_RecordMode,
	Internal_Codec_Set_LineinoutMode,
	Internal_Codec_DAC_Volume,
	Internal_Codec_ADC_Volume,
	Internal_Codec_Disable_Mute,
	Internal_Codec_Enable_Mute,
	CODEC_HEADPHONEOUT,	//耳机输出
	0,		//Play flag =0
	0,		//Play Mute flag =0
	0,		//Record flag = 0
	0,		//Record Mute flag = 0
	3,		//Play volume Lch
	3,		//Play volume Rch
	5,		//Record volume Lch
	5		//Record volume Rch
};

MMD_U32 audio_codec_latch(void)
{
   	MMD_U32 i = 0, j = 0;
	MMD_U32 latchount=CODEC_LATCH_COUNT;
	
   
  	for(i=0;i<latchount;i++) j++;
   	//VIM_USER_DelayMs(1);
   
   	VIM_HIF_SetReg32(V5_REG_AUD_CODEC_LATCH, 0x1); 
     
   	for(i=0;i<latchount;i++) j++;
   	//VIM_USER_DelayMs(1);
   
   	VIM_HIF_SetReg32(V5_REG_AUD_CODEC_LATCH, 0x0);
	
   	for(i=0;i<latchount;i++) j++;
   	//VIM_USER_DelayMs(1);
   	return j;
}

/*******************************************************************************************
Description:
 	Codec initialize, and set code in sleep mode
Parameters:	
      	void;
Return:	
	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
********************************************************************************************/
void   Internal_Codec_Initialize(void)
{//VIM AudioCodec Initial
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x0001);	//hbp,Master Power On
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x1001);	//hbp,Finish master reset.
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x3001);	//hbp,Finish Datapath reset
	//VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x3fff);	//Open all power

	/*	
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, 0x000000FF);	//Digital&Analog Playback and Record Mute
	//VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, 0x00000000);	//Digital&Analog Playback and Record Un-Mute
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL2, 0x28285454);	//Digital&Anglog Playback RightCh and LeftCh Volume is 0dB.
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL3, 0x04041F1F);	//Anglog Record Left&Right Ch Volume is 0dB
   																	//Digital Record Left&Right Ch Volume is 10dB
	*/
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_DIG_IF, 0x02);	//Codec Digital-Interface is I2S mode.	
	//VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_ATEST, 0x00122A13);	//default value,有较大po noise.	
	//VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_ATEST, 0x00122B07);	//VCM pre-charge time 1s 10uFcapacitor
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_ATEST, 0x00122B13);	//Bypass power up/down sequences,pop noise 会减小,
   															//VCM pre-charge time 1s 1uFcapacitor
  	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_DTEST, 0x00);	//Normal Function,  ADC bitstream to digital from analog section of embedded codec.
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_MUX, 0x00000101);	//default value
}


void   Internal_Codec_Exit(void)
{
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x0000); 
}

/*******************************************************************************************
Description:
  Codec set DA conveter to play mode
Parameters: 
       MMD_U08  nCahnnel  1 leftchannel, 2 right channel, 3 left+right=stereo
       MMD_U08  nOutType  1 speaker,     2 headphone,    3 speaker+headphone
Return: 
 void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
	   0.2			Hao Baopeng			2009.02.04		modified version
********************************************************************************************/
void   Internal_Codec_Set_PlayMode(MMD_U32 nChannel, MMD_U32 nOutType)
{
	MMD_U32 regValue,io_regval,tempReg;

	audio_codec.PlyFlag = 1;
	audio_codec.OutType = nOutType;
	//---------将CODEC内部HS和SPK电源都打开,为了抑制耳机输出泄漏-------------//
	HAL_READ_UINT32(V5_REG_AUD_CODEC_SYS, regValue);
	//regValue &= ~0x7E;
	regValue &= ~ CREATE_MASK(6,1);
	if(nChannel == CODEC_LEFTCHANNEL)
	{//仅左声道输出
		regValue |= 0x2A;	//Left DAC,Left headset driver,Left line driver PowerOn.
	}
	else if(nChannel == CODEC_RIGHTCHANNEL)
	{//仅右声道输出
		regValue |= 0x54;	//Right DAC,Right headset driver,Right line driver PowerOn.
	}
	else
	{//左右声道输出
		regValue |= 0x7E;	//DAC,Headset,Line driver PowerOn
	}
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, regValue);
	//----------------------//
	switch (nOutType)
	{
		default:	//缺省为耳机输出
		case CODEC_HEADPHONEOUT:	//耳机输出
			//===== Speaker's PA PowerOff =====//
			HAL_READ_UINT32(V5_REG_GPIO_A_CFG, io_regval);
			io_regval |=(1<<22);	//Set PA22  is used as a GPIO pin.
			HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG, io_regval);
			//--------//
			HAL_READ_UINT32(V5_REG_GPIO_A_DIR, io_regval);
			if((io_regval&(1<<22))==0)
			{//If PA22 is input, then set PA22's direction is output
				io_regval |=(1<<22);
				HAL_WRITE_UINT32(V5_REG_GPIO_A_DIR, io_regval);
			}
			//--------//
			HAL_READ_UINT32(V5_REG_GPIO_A_I, io_regval);
			io_regval &= ~(1<<22);		//PA22=0, Speaker's PA PowerOff
			HAL_WRITE_UINT32(V5_REG_GPIO_A_I, io_regval);
			//--------//
			break;
		case CODEC_SPEAKEROUT:		//喇叭输出
			//===== Speaker's PA PowerOn =====//
			HAL_READ_UINT32(V5_REG_GPIO_A_CFG, io_regval);
			io_regval |=(1<<22);	//Set PA22  is used as a GPIO pin.
			HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG, io_regval);
			//--------//
			HAL_READ_UINT32(V5_REG_GPIO_A_DIR, io_regval);
			if((io_regval&(1<<22))==0)
			{//If PA22 is input, then set PA22's direction is output
				io_regval |=(1<<22);
				HAL_WRITE_UINT32(V5_REG_GPIO_A_DIR, io_regval);
			}
			//--------//
			HAL_READ_UINT32(V5_REG_GPIO_A_I, io_regval);
			io_regval |= (1<<22);		//PA22=1, Speaker's PA PowerOn
			HAL_WRITE_UINT32(V5_REG_GPIO_A_I, io_regval);
			//----------------------//
			break;
		case CODEC_SPEAKER_HP_OUT:	//耳机和喇叭同时输出
			//===== Speaker's PA PowerOn =====//
			HAL_READ_UINT32(V5_REG_GPIO_A_CFG, io_regval);
			io_regval |=(1<<22);	//Set PA22  is used as a GPIO pin.
			HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG, io_regval);
			//--------//
			HAL_READ_UINT32(V5_REG_GPIO_A_DIR, io_regval);
			if((io_regval&(1<<22))==0)
			{//If PA22 is input, then set PA22's direction is output
				io_regval |=(1<<22);
				HAL_WRITE_UINT32(V5_REG_GPIO_A_DIR, io_regval);
			}
			//--------//
			HAL_READ_UINT32(V5_REG_GPIO_A_I, io_regval);
			io_regval |= (1<<22);		//PA22=1, Speaker's PA PowerOn
			HAL_WRITE_UINT32(V5_REG_GPIO_A_I, io_regval);
			//--------//
			break;
	}
	HAL_READ_UINT32(V5_REG_AUD_CODEC_MUX, regValue);
	regValue &= 0x0000FFFF;		//Analog Mixer digital source only
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_MUX, regValue);
	audio_codec.codec_DAC_volume(audio_codec.Codec_PlyLChVol,audio_codec.Codec_PlyRChVol);
}

/*******************************************************************************************
Description:
 	Codec set DA conveter to stop, pause mode
Parameters:	void;
Return:		void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
	   0.2			Hao Baopeng			2009.02.04		modified version
********************************************************************************************/
void   Internal_Codec_Stop_PlayMode(void)
{
	UINT32	regValue,io_regval;
	
	audio_codec.PlyFlag = 0;
	//----------------------//
	HAL_READ_UINT32(V5_REG_GPIO_A_I, io_regval);
	io_regval &= ~(1<<22);		//PA22=0, Speaker's PA PowerOff
	HAL_WRITE_UINT32(V5_REG_GPIO_A_I, io_regval);
	//----------------------//
	HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL1, regValue);
	regValue |= 0x0F;			//Digital&Analog Playback Mute
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, regValue);
	//----------------------//	
	HAL_READ_UINT32(V5_REG_AUD_CODEC_SYS, regValue);
	regValue &= ~0x7E;		//DAC,Headset,Line driver PowerDown
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, regValue);
}
MMD_U32 g_is_fm = 0;
void MMD_USE_FM_Flag(MMD_U08 flag) //zhangxiang add for fm
{
   g_is_fm=flag;
}


/*******************************************************************************************
Description:
  	Codec set DA conveter volume set
Parameters: 
       int vLeft    left channel volume percent (0-100) 
       int vRight  right channel volume percent (0-100) 
Return: 
 void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
	   0.2			Hao Baopeng			2009.02.06		modified version
********************************************************************************************/
extern int Appscomm_is_incoming_state(int bb);
void   Internal_Codec_DAC_Volume(MMD_U32 vLeft, MMD_U32 vRight)
{
	unsigned int regValue1,regValue2;
    MMD_U32 vc0848_vLeft,vc0848_vRight;
	boolean headset_status;    
	if(vLeft > 100)
	{	vLeft = 100;
	}
	if(vRight > 100)
	{	vRight = 100;
	}
	audio_codec.Codec_PlyLChVol = vLeft;
	audio_codec.Codec_PlyRChVol = vRight;
	//---------------------//
	if(audio_codec.PlyFlag==0)
	{	return;
	}
	if(audio_codec.PlyMuteFlag)
	{	return;
	}
	HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL1, regValue1);	//Mute Control Register
	HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL2, regValue2);	//Playback Volume Control Register
	regValue2 &= ~0xFFFF;
	if(vLeft == 0)
	{	regValue1 |= 0x05;	//Digital&Analog playback left channel mute.
	}
	else
	{	regValue1 &= ~0x05;	//Digital&Analog playback left channel un-mute.
		//-----------//
		//0x2B是测试出的固定值,100%~1%的变化范围对应到CTRL2寄存器的变化为0x54~0x2B.
		//vLeft--;				//Adjust vLeft range to 0~99.
		//regValue2 |= 0x22+vLeft/2;		//0x54-0x22 = 50;
    
        OEM_GetConfig(CFGI_HEADSET_PRESENT,&headset_status, sizeof(headset_status));
        if((0 == headset_status)  //headset is plug-out
            ||(1 == Appscomm_is_incoming_state(-1)))
        {
		    vc0848_vLeft = vLeft*21/80+61;//0x45+(vLeft*3)/20;	//0x54-0x2B = 41;   
		    vc0848_vRight = vRight*21/80+61;
        }
        else    // plug-in
        {
            vc0848_vLeft = vLeft*12/80+60;       
            vc0848_vRight = vRight*12/80+60;
        }
        regValue2 |= vc0848_vLeft;
	}
	if(vRight == 0)
	{	regValue1 |= 0x0A;	//Digital&Analog playback Right channel mute.
	}
	else
	{	regValue1 &= ~0x0A;	//Digital&Analog playback Right channel un-mute.
		//-----------//
		//vRight--;			//Adjust vRight range to 0~99.
		//regValue2 |=( 0x22+vRight/2)<<8;			//0x54-0x22 = 50;
		regValue2 |= vc0848_vRight<<8;//(0x45+(vRight*3)/20)<<8;	//0x54-0x2B = 41;

	}
	if(audio_codec.OutType == CODEC_SPEAKEROUT)
	{//spk输出时，将耳机音量置为最小，用于抑制耳机输出泄漏
		regValue1 |= 0x0C;		//analog playback mute.(耳机部分静音)
		regValue2 &= 0xFFFF;		//analog playback volume minum//将耳机PA音量置为最小
	}
    else
    {
        regValue2 |= 0x28280000;
	}

	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, regValue1);
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL2, regValue2);
}

/*******************************************************************************************
Description:
 	Codec set AD conveter to record mode
Parameters:	
      	MMD_U08  nCahnnel    1 leftchannel, 2 right channel, other left+right=stereo
      	MMD_U08 nInputType  1 linein,         2 micphone,      other micphone
Return:	
	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
	   0.2			Hao Baopeng			2009.02.09		modified version
********************************************************************************************/
void   Internal_Codec_Set_RecordMode(MMD_U32 nChannel, MMD_U32 nInputType)
{
	MMD_U32 regValue;

	audio_codec.RecFlag = 1;
	HAL_READ_UINT32(V5_REG_AUD_CODEC_SYS, regValue);
	regValue &= ~ CREATE_MASK(11,7);
	if(nInputType != CODEC_LINEIN)
	{	//MIC输入
		regValue |= CREATE_MASK(11,11);						//Microphone bias PowerOn.
	}
	if(nChannel == CODEC_LEFTCHANNEL)
	{//仅左声道输入
		regValue |= CREATE_MASK(9,9)|CREATE_MASK(7,7);		//Left ADC & record PGA Power On.
	}
	else if(nChannel == CODEC_RIGHTCHANNEL)
	{//仅右声道输入
		regValue |= CREATE_MASK(10,10)|CREATE_MASK(8,8);	//Right ADC & record PGA Power On.
	}
	else
	{//左右声道输入
		regValue |= CREATE_MASK(10,7);						//Left ADC & record PGA and Right PowerOn.
	}
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, regValue);
	//----根据实际调试得出结论(MIC输入与LINEIN输入analog增益设置不同)-----//
	
	HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL3, regValue);
	if(nInputType == CODEC_LINEIN)
	{//LINEIN输入
		regValue &= ~0xFFFF;
		regValue |= 0x0909;	//Anglog Record Left&Right Ch Volume is 0dB
	}
	else
	{//MIC输入
		regValue &= ~0xFFFF;
		regValue |= 0x2525;	//Anglog Record Left&Right Ch Volume is 19dB
	}
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL3, regValue);
	
	//----------//
	HAL_READ_UINT32(V5_REG_AUD_CODEC_MUX, regValue);
	regValue &= 0xFFFF0000;
	switch (nInputType)
	{
		case CODEC_MICPHONEIN:		//兼容旧版
		case CODEC_MICIN_DIFFERENCE:
			regValue |= 0x0404;		//Microphone input(differential),miclp & micln	(MIC差分输入)
			break;
		case CODEC_MICIN_SINGLE:
			regValue |= 0x0202;		//Microphone input(single-ended),miclp & vcm (MIC单端输入)
			break;
		case CODEC_LINEIN:
			regValue |= 0x0101;		//Line input 2
			//regValue |= 0x0303;		//Line input 1
			break;
	}
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_MUX, regValue);
	audio_codec.codec_ADC_volume(audio_codec.Codec_RecLChVol,audio_codec.Codec_RecRChVol);
}

/*******************************************************************************************
Description:
 	Codec set AD conveter to stop, pause mode
Parameters:	void;
Return:	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
	   0.2			Hao Baopeng			2009.02.09		modified version
********************************************************************************************/
void   Internal_Codec_Stop_RecordMode(void)
{
	UINT32	regValue;
	
	audio_codec.RecFlag = 0;
	HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL1, regValue);
	regValue |= 0xF0;			//Digital&Analog Record Mute
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, regValue);
	//----------------------//
	HAL_READ_UINT32(V5_REG_AUD_CODEC_SYS, regValue);
	regValue &= ~ CREATE_MASK(11,7);	//Record All PowerDown
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, regValue);
}
/*******************************************************************************************
Description:
 	Codec set AD conveter volume set
Parameters:	
       int vLeft    left channel volume percent (0-100) 
       int vRight  right channel volume percent (0-100) 
Return:	
	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
	   0.2			Hao Baopeng			2009.02.10		modified version
********************************************************************************************/
void   Internal_Codec_ADC_Volume(MMD_U32 vLeft, MMD_U32 vRight)
{
	unsigned int regValue1,regValue2;
	
	if(vLeft > 100)
	{	vLeft = 100;
	}
	if(vRight > 100)
	{	vRight = 100;
	}
	audio_codec.Codec_RecLChVol = vLeft;
	audio_codec.Codec_RecRChVol = vRight;	
	//---------------------//
	if(audio_codec.RecFlag==0)
	{	return;
	}
	if(audio_codec.RecMuteFlag)
	{	return;
	}
	HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL1, regValue1);	//Mute Control Register
	//HAL_READ_UINT32(V5_REG_AUD_CODEC_VOL_CTRL3, regValue2);	//Playback Volume Control Register	
	//regValue2 &= 0xFFFF;		//Clear Bit31~16
	regValue2 = 0;
	regValue2 |= 0x1D1D;
	if(vLeft == 0)
	{	regValue1 |= 0x50;	//Digital&Analog record left channel mute.
	}
	else
	{	regValue1 &= ~0x50;	//Digital&Analog record left channel un-mute.
		//-----------//
		//5~1的变化范围(寄存器值由小到大，对应到音量由大到小).
		switch(vLeft)
		{
			case 20:
				regValue2 |= 0x10<<16;
				break;
			case 40:
				regValue2 |= 0x0C<<16;
				break;
			case 60:
				regValue2 |= 0x08<<16;
				break;
			case 80:
				regValue2 |= 0x04<<16;
				break;
			case 100:
				regValue2 |= 0x00<<16;
				break;
			default:
				break;
		}
	}
	if(vRight == 0)
	{	regValue1 |= 0xA0;	//Digital&Analog record Right channel mute.
	}
	else
	{	regValue1 &= ~0xA0;	//Digital&Analog record Right channel un-mute.
		//-----------//
		switch(vRight)
		{
			case 20:
				regValue2 |= 0x10<<24;
				break;
			case 40:
				regValue2 |= 0x0C<<24;
				break;
			case 60:
				regValue2 |= 0x08<<24;
				break;
			case 80:
				regValue2 |= 0x04<<24;
				break;
			case 100:
				regValue2 |= 0x00<<24;
				break;
			default:
				break;
		}
	}
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, regValue1);
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL3, regValue2);
}


//====================================================================//


void Internal_Codec_Set_LineinoutMode(void)
{
	unsigned int io_regval;
	
	//===== Speaker's PA PowerOn =====//
	HAL_READ_UINT32(V5_REG_GPIO_A_CFG, io_regval);
	io_regval |=(1<<22);	//Set PA22  is used as a GPIO pin.
	HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG, io_regval);
	//--------//
	HAL_READ_UINT32(V5_REG_GPIO_A_DIR, io_regval);
	if((io_regval&(1<<22))==0)
	{//If PA22 is input, then set PA22's direction is output
		io_regval |=(1<<22);
		HAL_WRITE_UINT32(V5_REG_GPIO_A_DIR, io_regval);
	}
	//--------//
	HAL_READ_UINT32(V5_REG_GPIO_A_I, io_regval);
	io_regval |= (1<<22);		//PA22=1, Speaker's PA PowerOn
	HAL_WRITE_UINT32(V5_REG_GPIO_A_I, io_regval);
	//=======================//
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x0001);	//hbp,Master Power On
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x1001);	//hbp,Finish master reset.
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x3001);	//hbp,Finish Datapath reset
	//VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x367f);
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_SYS, 0x3661);	//Open all power
	
 	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL1, 0x00000000);	//Digital&Analog Playback and Record Un-Mute
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL2, 0x28285454);	//Digital&Anglog Playback RightCh and LeftCh Volume is 0dB.
   	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_VOL_CTRL3, 0x08081F1F);	//Anglog Record Left&Right Ch Volume is 0dB
   																	//Digital Record Left&Right Ch Volume is 19dB
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_ATEST, 0x00122b07);
	VIM_CODEC_WriteReg(V5_REG_AUD_CODEC_MUX, 0x01010101);
}

/*******************************************************************************************
Description:
 	Codec set AD conveter to stop, pause mode
Parameters:	void;
Return:	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
********************************************************************************************/
void   Internal_Codec_Disable_Mute(MMD_U32 mode)
{
	MMD_U32 val;
	val=VIM_HIF_GetReg32(V5_REG_AUD_CODEC_VOL_CTRL1);
	if(mode)
	{//Playback
		audio_codec.PlyMuteFlag =0;
		val &=0xF0;
		VIM_HIF_SetReg32(V5_REG_AUD_CODEC_VOL_CTRL1, val);
   		audio_codec_latch();
		audio_codec.codec_DAC_volume(audio_codec.Codec_PlyLChVol,audio_codec.Codec_PlyRChVol);
	}
   	else
   	{//Record
		audio_codec.RecMuteFlag = 0;
   		val &=0x0F;
		VIM_HIF_SetReg32(V5_REG_AUD_CODEC_VOL_CTRL1, val);
   		audio_codec_latch();
		audio_codec.codec_ADC_volume(audio_codec.Codec_RecLChVol,audio_codec.Codec_RecRChVol);
	}
}

/*******************************************************************************************
Description:
 	Codec set AD conveter to stop, pause mode
Parameters:	void;
Return:	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Zhang Xuecheng		2007.12.18		create first version
********************************************************************************************/
void   Internal_Codec_Enable_Mute(MMD_U32 mode)
{	
	MMD_U32 val;
	val=VIM_HIF_GetReg32(V5_REG_AUD_CODEC_VOL_CTRL1);
	if(mode)
	{//Playback
		audio_codec.PlyMuteFlag =1;
		val &=0xF0;
		val |=0x0F;
		VIM_HIF_SetReg32(V5_REG_AUD_CODEC_VOL_CTRL1, val);
   		audio_codec_latch();
	}
   	else
   	{//Record
		audio_codec.RecMuteFlag =1;
   		val &=0x0F;
		val |=0xF0;
		VIM_HIF_SetReg32(V5_REG_AUD_CODEC_VOL_CTRL1, val);
   		audio_codec_latch();
	}
}
#endif
