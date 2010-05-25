/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_Audio_Driver.c
 * [Description]        : VC0820 Audio interface driver
 *				
 * [Author]                 : haobaopeng
 * [Date Of Creation]  : 2009-02-04 
 * [Platform]             : Nu820 MP
 * [Note]                  : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     			Modifications
 * ------------------------------------------------------------------------------
 * 2009-02-04  		Hao Baopeng		     	Created
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
#define CREATE_MASK(msb, lsb)       (((1U << ((msb) - (lsb) + 1)) - 1) << (lsb))

//采样位宽固定为64,(不是ADC/DAC的字长)
//Sclk = SampleRate*64;SampleRate就是WS_Clk.
	 		                  //    48000	     8000	      11025       12000        16000       22050       24000        32000      44100     88200         96000
    	static unsigned int const regSclkVal[3][11] = 
    	                                 {
    	                                         0x130013,0x770077,0x4f004f,0x4f004f,0x3b003b,0x270027,0x270027,0x1d001d,0x130013,0x090009,0x090009,  /* 64*fs  */ 
    	                                         0x090009,0x3b003b,0x270027,0x270027,0x1d001d,0x130013,0x130013,0x0e000e,0x090009,0x040004,0x040004,  /* 32*fs  */ 
    	                                         0x040004,0x1d001d,0x130013,0x130013,0x0e000e,0x090009,0x090009,0x070006,0x040004,0x020001,0x020001,  /* 16*fs  */ 
    	                                  }; 
	static unsigned int const regPllVal[8][11] =    
		                          {   
	/*122.88, 112.896*/		0x0a0100,0x0a0100,0x19024c,0x0a0100,0x0a0100,0x19024c,0x0a0100,0x0a0100,0x19024c,0x19024c,0x0a0100,   /*12MHZ*/					
 	/*122.9,     112.9*/		0x3404cd,0x3404cd,0x340469,0x3404cd,0x3404cd,0x340469,0x3404cd,0x3404cd,0x340469,0x340469,0x3404cd,   /*13MHZ*/ 
 	/*122.91,112.914*/		0x37028a,0x37028a,0x2a01c8,0x37028a,0x37028a,0x2a01c8,0x37028a,0x37028a,0x2a01c8,0x2a01c8,0x37028a,   /*26MHZ*/  
 	/*122.88, 122.88*/		0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,0x0d0400,   /*3.9MHZ*/  
  	/*122.88,112.896*/		0x080080,0x080080,0x28024c,0x080080,0x080080,0x28024c,0x080080,0x080080,0x28024c,0x28024c,0x080080,   /*19.2MHZ*/ 	
 	/*122.88,112.896*/	       0x050100,0x050100,0x190498,0x050100,0x050100,0x190498,0x050100,0x050100,0x190498,0x190498,0x050100,   /*6MHZ*/     
 	/*122.9,    112.9*/		0x1a04cd,0x1a04cd,0x1a0469,0x1a04cd,0x1a04cd,0x1a0469,0x1a04cd,0x1a04cd,0x1a0469,0x1a0469,0x1a04cd,   /*6.5MHZ*/                         							   	                     							   	
	/*122.88,112.896*/		0x140100,0x140100,0x32024c,0x140100,0x140100,0x32024c,0x140100,0x140100,0x32024c,0x32024c,0x140100,   /*24MHZ*/  
    	                                  };
unsigned int gAudio_RecChSel;	//用于保存录音时选择的通道
unsigned int PlayMixFlag = 0;			//播放时CodecMix功能: 1 - 使能 ; 0 - 关闭
/*******************************************************************************************
Description:
 	Set audio interface clock according sample rate.
Parameters:	samplerate.
Return:		void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.2			Haobaopeng			2009.02.04		create first version(transplant form 848 core code)
********************************************************************************************/
void VIM_Audio_ClkConfig(unsigned int sampleRate)
{
	unsigned int offset = 0,pllReady = 0,regValue = 0;
	unsigned int crystal_index=0;

	HAL_READ_UINT32(V5_REG_STRAP_PIN, crystal_index);
	crystal_index = (crystal_index>>16)&0x07;			//get external crystal oscillator configuration
	switch(sampleRate)
	{
		case 48000:
			offset = 0;
			break;
		case 8000:
			offset = 1;
			break;
		case 11025:
			offset = 2;
			break;
		case 12000:
		    offset = 3;
			break;
		case 16000:
		    offset = 4;
			break;
		case 22050:
			offset = 5;
			break; 
		case 24000:
			offset = 6;
			break;
		case 32000:
			offset = 7;
			break;
		case 44100:
			offset = 8;
			break;
		case 88200:
			offset = 9;
			break;
		case 96000:
			offset = 10;
			break;	
		default:
			offset = 0;
			break;							
	}

	HAL_WRITE_UINT32(PLL2_CKD,(int)(regPllVal[crystal_index][offset]));	//Config PLL2 clock divider

	HAL_READ_UINT32(PLL2_SETV,regValue );
	regValue &= 0x0000FFFF;
	regValue |= 0x27700000; 			//实际测试结果:PLL2_DIV5OFF =1时PLL2_DIV5 turn on(与手册不符)
	HAL_WRITE_UINT32(PLL2_SETV, regValue);

	HAL_WRITE_UINT32(V5_REG_CKD_SCLK_CON,(int)(regSclkVal[0][offset]));	//I2S_SCLK = 122.8M/240 =511.66KHz

	HAL_READ_UINT32(CKD_AT_CON, regValue );
	regValue &= 0x0000FFFF;
	HAL_WRITE_UINT32(CKD_AT_CON, regValue);    			//audio clock = 122.88M/2 =61.4M

	HAL_WRITE_UINT32(CKD_CODE_CON, 0x00040004);  		//codec clock = 122.88M/10 = 12.2M
	HAL_WRITE_UINT32(PLL_WAIT, 0xfff); 					//PLL wait =341.25us,(0xfff*1/12MHz)
	
	/////
	HAL_READ_UINT32(PLL_CTRL,regValue );
	regValue &= 0x0f;
	regValue |= 0x40; 		//PLL2 PowerOn,PLL2 not bypass,Start PLL2  reconfiguration
	HAL_WRITE_UINT32(PLL_CTRL, regValue);
	HAL_READ_UINT32(PLL_CTRL,regValue );
	regValue &= 0x0f;		//Finish PLL2  reconfiguration
	HAL_WRITE_UINT32(PLL_CTRL,regValue);
	///////
	while ((pllReady & 0x2)  == 0)
	{//Wait until PLL2 is locked and stable
		HAL_READ_UINT32(PLL_STATUS,pllReady );
	} 
	
	HAL_READ_UINT32(CKD_BP_CTL,regValue );
	regValue &= (~0xC80);				//CKD_SCLK,CKD_CODEC,CKD_AUD is not bypassed
	HAL_WRITE_UINT32(CKD_BP_CTL, regValue);
	
	HAL_READ_UINT32(CKD_EN_CTL,regValue );
	regValue |= 0xC80; 				//CKD_SCLK,CKD_CODEC,CKD_AUD is enabled
	HAL_WRITE_UINT32(CKD_EN_CTL, regValue);

	HAL_READ_UINT32(CGT_MC_CTL,regValue );
	regValue &= (~0x400); 				//I2S clock is not gated.
	HAL_WRITE_UINT32(CGT_MC_CTL, regValue);
}
/*******************************************************************************************
Description:
 	audio interface play mode config (I2S module parameter set)
Parameters:	
      	void;
Return:	
	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.02.08		create first version(transplant form SW code)
	   0.2			Zhang Xuecheng		2007.07.25		modified for mp platform
********************************************************************************************/
void VIM_Audio_PlayMode_Cfg(audio_output_param_t  pParam)
{
	unsigned int regValue;
	
	if(pParam.I2SMode)
	{	//I2S interface
		//------配置为GPIO------//
		HAL_READ_UINT32(V5_REG_GPIO_A_CFG,regValue);
		regValue |= 0x1C000C20;	// 1: Indication this pin is used as a GPIO pin. 
		HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG,regValue);
		HAL_WRITE_UINT32(V5_REG_FS_LEDM,0x00000000);
	       HAL_WRITE_UINT32(V5_REG_FS_SD_GPIO ,0x00000000);
	}	   
	else
	{	//PCM interface
		//------配置GPIO为PCM接口------//
		HAL_READ_UINT32(V5_REG_GPIO_A_CFG,regValue);
		regValue &= ~0x1C000C20;	// 0: Indicates the pin is used for its normal function. 
		HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG,regValue);		
	      HAL_WRITE_UINT32(V5_REG_FS_LEDM,0x00001111);	//Selected AUD_SDI_ADC,AUD_WS_ADC ,AUD_WS_DAC,AUD_SDO_DAC 
	      HAL_WRITE_UINT32(V5_REG_FS_SD_GPIO ,0x00000011);	//Selected AUD_CLK,AUD_SCLK.
	}
    	//----------------------//
    	VIM_Audio_ClkConfig(pParam.samRate);	//根据采样频率配置Audio接口的时钟(PLL2)
   	HAL_READ_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
	if(pParam.I2SMode)
	{	regValue |= 0x01;		//I2S interface
	}
	else
	{	regValue &= ~0x01;	//PCM interface
	}
	if(pParam.msMode)
	{	regValue |= 0x02;		//Set Audio interface work in Master Mode   
	}   
	else
	{	regValue &= ~0x02;	//Set Audio interface work in Slave Mode
	}
	if(pParam.edgemode)
	{	regValue |= 0x04;		// 1: send out data at rising edge of sck & capture data  at falling edge of sck
	}
	else
	{	regValue &= ~0x04;	// 0: send out data at falling edge of sck & capture data  at rising  edge of sck
	}
   	HAL_WRITE_UINT32(V5_REG_AUD_SHARE_CFG,regValue); 

	HAL_READ_UINT32(V5_REG_AUD_I2S_TX_CFG,regValue);	
	regValue &= ~0x7;
	regValue |= pParam.wdLen;	//Set audio data word length for DAC
	if(pParam.msbFst)
	{	regValue |= 0x10;			//MSB First
	}
	else
	{	regValue &= ~0x10;		//LSB First
	}
	if(pParam.fstWs)
	{	regValue |= 0x20;		//send out sd on first SCLK edge after WS transition
	}
	else
	{	regValue &= ~0x20;	//send out sd on second  SCLK edge after WS transition
	}
	HAL_WRITE_UINT32(V5_REG_AUD_I2S_TX_CFG,regValue);	
		
	HAL_READ_UINT32(V5_REG_AUD_FIFO_CFG,regValue);  
	regValue &= ~0x1F0000;
	regValue |= (pParam.fifoDepth<<16);		//Set  PFIFO_DEPTH
	HAL_WRITE_UINT32(V5_REG_AUD_FIFO_CFG,regValue);
	
	HAL_READ_UINT32(V5_REG_AUD_MEM_FORMAT,regValue); 	
	regValue &= ~0x70000;
    	if(pParam.mono)
	{	regValue |= 0x40000;	//Mono for PCM data stored in memory transmitted to  DAC
	}/*
	else
	{	regValue &= ~0x40000;	//Stereo for PCM data stored in memory transmitted to  DAC
	}*/
    	regValue |= (pParam.pcmDataFmt<<16);	//Set source pcm data format (16bit,8bit,32bit)
    	HAL_WRITE_UINT32(V5_REG_AUD_MEM_FORMAT,regValue);

	HAL_READ_UINT32(V5_REG_AUD_WS_DIV,regValue); 	
	regValue &= ~ CREATE_MASK(31,16);
	
	//目前只支持64bit采样位宽(不是ADC/DAC的字长)
	if(pParam.I2SMode)
	{	regValue |= (0x1F1F<<16);	//WS WIDTH for Tx is 64bit
	}
	else
	{	regValue |= (0x003E<<16);	//WS WIDTH for Tx is 64bit,单声道时只有左声道
	}
	/*支持64bit,32bit,16bit 采样位宽时打开
	if(pParam.wsWidth==FS64)
	{   	if(pParam.I2SMode)
		{	regValue |= (0x1F1F<<16);	//WS WIDTH for Tx is 64bit
		}
		else
		{	regValue |= (0x003E<<16);	//WS WIDTH for Tx is 64bit,单声道时只有左声道
		}
	}   
	else if(pParam.wsWidth==FS16)
	{   	if(pParam.I2SMode)
		{	regValue |= (0x0F0F<<16);	//WS WIDTH for Tx is 32bit
		}
		else
		{	regValue |= (0x001E<<16);	//WS WIDTH for Tx is 32bit,单声道时只有左声道
		}
	}  
	else
	{   	if(pParam.I2SMode)
		{	regValue |= (0x0707<<16);	//WS WIDTH for Tx is 16bit
		}
		else
		{	regValue |= (0x000E<<16);	//WS WIDTH for Tx is 16bit,单声道时只有左声道
		}
	}*/
	HAL_WRITE_UINT32(V5_REG_AUD_WS_DIV,regValue);
	//-------//
	HAL_WRITE_UINT32(V5_REG_AUD_WS_SRC,0x4);	//Source of ws for embedded codec is from div_ws_tx.
	//-------//
	HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
	regValue &= ~ CREATE_MASK(14,14);	//Clear bit14, MUX4 ,data with no mixer
	regValue &= ~ CREATE_MASK(5,4);		//Clear bit5~bit4 , TX ChannelSwitch Setup
	if(pParam.I2SMode==0)
	{//PCM输出模式,只用于蓝牙耳机
		regValue &= ~0x40000;	//MUX_PAD = 0,serial data from I2S_TX/PCM_TX
		regValue |= 0x3000;	//PCM接口时，打开硬件MIX功能(MUX3_L=1, MUX3_R=1).
	}
	else
	{	if(PlayMixFlag)
		{	regValue |= 0x3000;	//I2S接口时，打开硬件MIX功能(MUX3_L=1, MUX3_R=1).
		}
		else	
		{	regValue &= ~0x3000;	//I2S接口时，关闭硬件MIX功能
		}
	}	
	HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
	//----- additional test -------//
	#if 0	//使用codec测试PCM输出时将其置1
	if(pParam.I2SMode)
	{
		HAL_WRITE_UINT32(V5_REG_AUD_CODEC_DIG_IF, 0x02);		//Codec Digital-Interface is  I2S justified mode
   		audio_codec_latch();
	}
	else
	{
		HAL_WRITE_UINT32(V5_REG_AUD_CODEC_DIG_IF, 0x0A);	//Codec Digital-Interface is Mono Burst 1 format mode.
   		audio_codec_latch();
	}
	#endif
}

/*******************************************************************************************
Description:
	Config audio interface mode, I2S or PCM interface.
Parameters:	0: PCM-Interface, 1: I2S-Interface
Return:		void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Hao Baopeng			2009.02.05		create first version
********************************************************************************************/
void VIM_Audio_PlayModeIf_Cfg(MMD_U32 IfMode)
{
	unsigned int regValue;
	
	if(IfMode)
	{	
		//------配置为GPIO------//
		HAL_READ_UINT32(V5_REG_GPIO_A_CFG,regValue);
		regValue |= 0x1C000C20;	// 1: Indication this pin is used as a GPIO pin. 
		HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG,regValue);
		HAL_WRITE_UINT32(V5_REG_FS_LEDM,0x00000000);
	      HAL_WRITE_UINT32(V5_REG_FS_SD_GPIO ,0x00000000);
		  //--------------//
	   	HAL_READ_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
		regValue |= 0x01;					//I2S接口
   		HAL_WRITE_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
		HAL_READ_UINT32(V5_REG_AUD_WS_DIV,regValue);
		regValue &= 0x0000FFFF;
		regValue |= (0x1F1F<<16);	//WS WIDTH for Tx is 64bit,目前只支持64bit采样位宽(不是ADC/DAC的字长)
		HAL_WRITE_UINT32(V5_REG_AUD_WS_DIV,regValue);
		HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
		regValue &= ~0x3000;				//I2S接口时，关闭硬件MIX功能
   		HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
		//----- additional test -------//
		#if 0	//使用codec测试PCM输出时将其置1
		HAL_WRITE_UINT32(V5_REG_AUD_CODEC_DIG_IF, 0x02);	//Codec Digital-Interface is  I2S justified mode
   		audio_codec_latch();
		#endif
	}
	else
	{			
		//------配置GPIO为PCM接口------//
		HAL_READ_UINT32(V5_REG_GPIO_A_CFG,regValue);
		regValue &= ~0x1C000C20;	// 0: Indicates the pin is used for its normal function. 
		HAL_WRITE_UINT32(V5_REG_GPIO_A_CFG,regValue);		
	      HAL_WRITE_UINT32(V5_REG_FS_LEDM,0x00001111);	//Selected AUD_SDI_ADC,AUD_WS_ADC ,AUD_WS_DAC,AUD_SDO_DAC 
	      HAL_WRITE_UINT32(V5_REG_FS_SD_GPIO ,0x00000011);	//Selected AUD_CLK,AUD_SCLK.
	      //----------------//
	   	HAL_READ_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
		regValue &= ~0x01;				//PCM接口
   		HAL_WRITE_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
		HAL_READ_UINT32(V5_REG_AUD_WS_DIV,regValue);
		regValue &= 0x0000FFFF;
		regValue |= (0x003E<<16);	//WS WIDTH for Tx is 64bit,单声道时只有左声道,目前只支持64bit采样位宽(不是ADC/DAC的字长)
		HAL_WRITE_UINT32(V5_REG_AUD_WS_DIV,regValue);		
		HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
		regValue |= 0x3000;				//PCM接口时，使用硬件MIX功能(MUX3_L=1, MUX3_R=1).		
   		HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
		//----- additional test -------//
		#if 0	//使用codec测试PCM输出时将其置1
		HAL_WRITE_UINT32(V5_REG_AUD_CODEC_DIG_IF, 0x0A);	//Codec Digital-Interface is Mono Burst 1 format mode.
   		audio_codec_latch();
		#endif
	}
}


/*******************************************************************************************
Description:
 	audio interface record mode config (I2S module parameter set)
Parameters:	
      	void;
Return:	
	void;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.02.08		create first version(transplant form SW code)
	   0.2			Zhang Xuecheng		2007.07.25		modified for mp platform
********************************************************************************************/
void VIM_Audio_RecordMode_Cfg(audio_input_param_t  rParam)
{
	unsigned int regValue;
	
    	VIM_Audio_ClkConfig(rParam.samRate);	//根据采样频率配置Audio接口的时钟(PLL2)
    	//--------------------//
	HAL_READ_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
	regValue |= 0x01;		//I2S interface	
	if(rParam.msMode)
	{	regValue |= 0x02;		//Set Audio interface work in Master Mode   
	}   
	else
	{	regValue &= ~0x02;	//Set Audio interface work in Slave Mode
	}
	if(rParam.edgemode)
	{	regValue |= 0x04;		// 1: send out data at rising edge of sck & capture data  at falling edge of sck
	}
	else
	{	regValue &= ~0x04;	// 0: send out data at falling edge of sck & capture data  at rising  edge of sck
	}
	HAL_WRITE_UINT32(V5_REG_AUD_SHARE_CFG,regValue);

	HAL_READ_UINT32(V5_REG_AUD_I2S_RX_CFG,regValue);	
	regValue &= ~0x7;
	regValue |= rParam.wdLen;	//Set audio data word length for ADC
	if(rParam.msbFst)
	{	regValue |= 0x10;			//MSB First
	}
	else
	{	regValue &= ~0x10;		//LSB First
	}
	if(rParam.fstWs)
	{	regValue |= 0x20;		//send out sd on first SCLK edge after WS transition
	}
	else
	{	regValue &= ~0x20;	//send out sd on second  SCLK edge after WS transition
	}	
	if(rParam.align == LEFT)
	{	regValue |= 0x40;		// 1, left alignment
	}
	else
	{	regValue &= ~0x40;		// 0, right alignment
	}
	HAL_WRITE_UINT32(V5_REG_AUD_I2S_RX_CFG,regValue);

	HAL_READ_UINT32(V5_REG_AUD_FIFO_CFG,regValue);  
	regValue &= ~0x1F;
	regValue |= (rParam.fifoDepth&0x1F);
	HAL_WRITE_UINT32(V5_REG_AUD_FIFO_CFG,regValue);
	
	HAL_READ_UINT32(V5_REG_AUD_MEM_FORMAT,regValue); 	
	regValue &= ~0x7;
    	if(rParam.mono)
	{	regValue |= 0x4;	//Mono for PCM data stored in memory transmitted to  ADC
	}/*
	else
	{	regValue &= ~0x4;	//Stereo for PCM data stored in memory transmitted to  ADC
	}*/
    	regValue |= (rParam.pcmDataFmt & 0x7);	//Set source pcm data format (16bit,8bit,32bit)
    	HAL_WRITE_UINT32(V5_REG_AUD_MEM_FORMAT,regValue);

	HAL_READ_UINT32(V5_REG_AUD_WS_DIV,regValue);
	regValue &= ~ CREATE_MASK(15,0);
	//目前只支持64bit采样位宽(不是ADC/DAC的字长)
	regValue |= 0x1F1F;	//WS WIDTH for Rx is 64bit
	/*支持64bit,32bit,16bit 采样位宽时打开
	if(pParam.wsWidth==FS64)
	{   	regValue |= (0x1F1F<<16);	//WS WIDTH for Rx is 64bit
	}   
	else if(pParam.wsWidth==FS16)
	{   	regValue |= (0x0F0F<<16);	//WS WIDTH for Rx is 32bit
	}
	else
	{   regValue |= (0x0707<<16);	//WS WIDTH forRx is 16bit
	}*/
	HAL_WRITE_UINT32(V5_REG_AUD_WS_DIV,regValue);
	//-------//
	HAL_WRITE_UINT32(V5_REG_AUD_WS_SRC,0x0);	//Source of ws for embedded codec is from from div_ws_rx.
	//-------//	
	HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
	if(rParam.mono == 1)
	{//单声道录音
		if(gAudio_RecChSel == MMD_AUDIOCODEC_RCHANNEL)
		{	regValue |= 0x01;		//录右声道
		}
		else
		{	regValue &= ~0x01;	//录左声道
		}
	}
	else
	{//立体声录音
		regValue &= ~ CREATE_MASK(3,0);		//Clear bit3~bit0 , RX0/1 ChannelSwitch Set
	}
	regValue &= ~ CREATE_MASK(11,8);	//Clear bit11~bit8, MUX2 Set
	regValue &= ~ CREATE_MASK(16,15);	//Clear bit16~bit15, MUX_Rx,MUX1 Set
	HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
}

/*******************************************************************************************
Description: 设置AUDIO接口和CODEC,用于FM模式
Parameters:
	sampleRate	: 采样频率(例8000(8KHz))
	MixFlag		: 混音标志(1.使能混音,0关闭混音)
Return:	None
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Hao Baopen			2000.02.10		create first version
********************************************************************************************/
void VIM_Audio_FM_Cfg(unsigned int sampleRate, unsigned int MixFlag)
{
	unsigned int regValue;
	
    	VIM_Audio_ClkConfig(sampleRate);		//FM播放或录音采样频率
	//----------- Codec Config ---------------//
	//audio_codec.codec_DAC_volume(60,60);		//设置初始输出音量
	//audio_codec.codec_ADC_volume(70,70);		//设置初始输入音量
	audio_codec.codec_set_playmode(CODEC_STEROECHANNEL,CODEC_HEADPHONEOUT);
	audio_codec.codec_set_recordmode(CODEC_STEROECHANNEL,CODEC_LINEIN);
	//----------- Audio Config ---------------//
	HAL_READ_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
	regValue |= 0x01;		//I2S interface	
	regValue |= 0x02;		//Set Audio interface work in Master Mode   
	regValue &= ~0x04;	// 0: send out data at falling edge of sck & capture data  at rising  edge of sck	
	HAL_WRITE_UINT32(V5_REG_AUD_SHARE_CFG,regValue);
	
	HAL_READ_UINT32(V5_REG_AUD_I2S_RX_CFG,regValue);
	regValue &= ~0x7;
	regValue |= B24;		//Set audio data word length for ADC
	regValue |= 0x10;		//MSB First
	regValue &= ~0x20;	//send out sd on second  SCLK edge after WS transition
	regValue |= 0x40;		// 1, left alignment
	HAL_WRITE_UINT32(V5_REG_AUD_I2S_RX_CFG,regValue);

	HAL_READ_UINT32(V5_REG_AUD_I2S_TX_CFG,regValue);
	regValue &= ~0x7;
	regValue |= B24;		//Set audio data word length for DAC
	regValue |= 0x10;		//MSB First
	regValue &= ~0x20;	//send out sd on second  SCLK edge after WS transition
	HAL_WRITE_UINT32(V5_REG_AUD_I2S_TX_CFG,regValue);
	
	HAL_READ_UINT32(V5_REG_AUD_FIFO_CFG,regValue);  
	regValue &= ~0x1F;
	regValue |=0x0F;		//RFIFO_DEPTH = 15 +1
	HAL_WRITE_UINT32(V5_REG_AUD_FIFO_CFG,regValue);
	
	HAL_READ_UINT32(V5_REG_AUD_MEM_FORMAT,regValue);
	regValue &= ~0x7;
	regValue &= ~0x4;			//Stereo for PCM data stored in memory transmitted to  ADC
    	regValue |= (PCM16 & 0x7);	//Set source pcm data format (16bit,8bit,32bit)
    	HAL_WRITE_UINT32(V5_REG_AUD_MEM_FORMAT,regValue);
    	
	HAL_READ_UINT32(V5_REG_AUD_WS_DIV,regValue);
	regValue &= ~ CREATE_MASK(31,16);
	regValue |= (0x1F1F<<16);	//WS WIDTH for Tx is 64bit
	regValue &= ~ CREATE_MASK(15,0);
	regValue |= 0x1F1F;			//WS WIDTH for Rx is 64bit
	HAL_WRITE_UINT32(V5_REG_AUD_WS_DIV,regValue);
	//-------//
	HAL_WRITE_UINT32(V5_REG_AUD_WS_SRC,0x2);	//Source of ws for all is from from div_ws_rx.
	//-------//	
	HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
	regValue &= ~ CREATE_MASK(3,0);		//Clear bit3~bit0 , RX0/1 ChannelSwitch Setup
	regValue &= ~ CREATE_MASK(11,8);	//Clear bit11~bit8, MUX2 Setup
	if(MixFlag)
	{//使能混音
		//Rx Mixer Source
		regValue |= 0x02;		//RX0 Left&Right Cahnnel is from left channel of RX0
		regValue |= 0x04;		//RX1 Left&Right Cahnnel is from right channel of RX1
		//Rx Select Mix
		regValue |= 0x5<<8;	//Source of left&right channel to RFIFO is left&right channel of  ldigital mixer
	}
	regValue &= ~ CREATE_MASK(14,14);	//Clear bit14, MUX4 Setup
	regValue |= 0x1<<14;					//data source for TX is after mixer
	regValue &= ~ CREATE_MASK(16,15);	//Clear bit16~bit15, MUX_Rx,MUX1 Setup
	HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
	HAL_WRITE_UINT32(V5_REG_AUD_START,0x3);	//Start Record&Playback
}

