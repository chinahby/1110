
/*************************************************************************
*                                                                       
*             Copyright (C) 2005 Vimicro CO.,LTD     		 
*
* FILE NAME                                  VERSION                
*                                                                       
* VIM_SENSOR_Info.c				0.1                  
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*    578B's  sensor information file                      
*
*	Version		Author			Date		Description		
*  ---------------------------------------------------------------						 
*   0.1			yaoweiquan  		2007-4-19	The first version. 
*  ---------------------------------------------------------------
*                                                                       
*************************************************************************/
/****************************************************************************
This source code has been made available to you by VIMICRO on an
AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
copyrights to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or without
modifications. Any person who transfers this source code or any derivative
work must include the VIMICRO copyright notice and this paragraph in
the transferred software.
****************************************************************************/
#include "VIM_COMMON.h"
#include "msg.h"
#define VIM_SURPPORT_VGA_30FRAME  0

/*****************************************************************************************************
		SENSOR/SIF/ISP information
*****************************************************************************************************/
//0.3M
#if V5_DEF_SNR_DUMMY

void SensorSetReg_DUMMY(UINT32 uAddr, UINT16 uVal)
{
}

void SensorGetReg_DUMMY(UINT32 uAddr, UINT16 *uVal)
{
}

UINT8 SensorIdChk_DUMMY(void)
{
	return SUCCEED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_DUMMY(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor Brightness  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetBrightness_DUMMY(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor contrast  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetContrast_DUMMY(UINT8 val)
{
	
}

/******************************************************************
         Desc: set sensor mod  callback 
         Para:  val: 	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetMode_DUMMY(UINT8 val)
{
	
}

void SensorGetET_DUMMY(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
	
}

void SensorSetET_DUMMY(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
	
}

TSnrSizeCfg gSnrSizeCfg_DUMMY[] = 
{
	{
		{0, NULL},
		{0, NULL},
		{1280, 1024},
		2,
		0,
		0
	},
	{
		{0, NULL},
		{0, NULL},
		{640, 480},
		2,
		1,
		1
	}
};

TSnrInfo gSensorInfo_DUMMY = 
{
	"DUMMY YUV",

	1,																										//yuv mode
	1,																										//not use PCLK of sensor
	0,																										
	0,																										//0 -- I2C, 1 -- Serial bus;
	0,																										//[0]: 1 -- OV I2C bus
	80,																										//I2C Rate : 100KHz
	0x5d,//0x48,				//0x5d for 5xx sensor, 0x48 for 3xx sensor 		0x48,//																			//I2C address
	0,																									//ISP I2C address for special sensor
	0x01,																									//power config
	0,																										//reset sensor

	2,																										//brightness 
	2,																										//contrast
	2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
   	},


	{0,NULL},																								
	{0,NULL},																								
	{0, NULL},	
	{{0,NULL}},																								
	{0,NULL},

	{0,NULL},//{sizeof(gSnrSizeRegValInit_MT9M112) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M112},
	{sizeof(gSnrSizeCfg_DUMMY) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_DUMMY},	
	{0,NULL},


	SensorSetReg_DUMMY,																				
	SensorGetReg_DUMMY,																				
	SensorIdChk_DUMMY,	
	SensorSetMirrorFlip_DUMMY,
	NULL,
	SensorSetContrast_DUMMY,
	SensorSetBrightness_DUMMY,
	SensorSetMode_DUMMY,																									//Set Sensor Mirror Flip Call
	NULL,//SnrGetEt_MT9M112,																				
	NULL,//SnrSetEt_MT9M112,																			

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};

#endif


#if V5_DEF_SNR_MT9V011_BAYER

void SensorSetReg_MT9V011_BAYER(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord((UINT8)uAddr, (UINT16)uVal,1);
}

void SensorGetReg_MT9V011_BAYER(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord((UINT8)uAddr, (UINT16*)uVal,1);
}

UINT8 SensorIdChk_MT9V011_BAYER(void)
{
	UINT16 val = 0;
	VIM_SIF_I2cReadWord(0x00, &val,1);
       if(val == 0x8243)   
		return SUCCEED;	
	return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9V011_BAYER(UINT8 val)
{
 
}

/******************************************************************************
**
Description:	Write sensor register to change exposure time
Parameter: t -- exposure time clock num 
freqtype -- frequence type 50 or 60
*******************************************************************************
**/
void SensorSetExposeTime_MT9V011_BAYER(UINT8 freqtype, UINT32 t, TSize size, UINT32 snrclock)
{

	UINT16 uTemp = 0;
	TI2cBatch AeData;
	uTemp = (t+112)/853;
 //       uTemp = (t+112)/(0x37a);
	if(uTemp<=1)  uTemp=1;

    
	AeData.RegBytes  = 2;
	AeData.RegAddr   = 0x9;
	AeData.RegVal[0] = (UINT8)(uTemp>>8);
	AeData.RegVal[1] = (UINT8)(uTemp);

        VIM_SIF_I2cAeBatch(1, &AeData,1); 

   
	//VIM_SIF_I2cWriteWord(0x09, uTemp,1);
}

const TReg gSifPwrOnSeqRegVal_MT9V011_BAYER[] =
{
	{0x00, 0x27, 1},
	{0x00, 0x26, 100},
	{0x00, 0x27, 10}
};

const TReg gSifRegVal_MT9V011_BAYER[] =
{
	{0x0c, 0x08, 0},
	{0x10, 0x04, 0},
	{0x04, 0x41, 0},
	{0x50, 0x02, 0},
	{0x100, 0x1d0, 0},	//AE start/stop
	{0x108, 0x1e0, 0},
};

const TReg gIspRegVal_MT9V011_BAYER[] =
{
	{0x00, 0x01, 0},
	{0x01, 0x79, 0},
       {0x14, 0x08, 0}, //0.5x
       {0x15, 0x0c, 0},
	{0x40, 0x40, 0},	//digital gain R
	{0x41, 0x40, 0},    //B
	{0x42, 0x40, 0},    //G
	{0x48, 0x60, 0},	//global gain
	{0x4c, 0x20, 0},

	{0x58, 0x5f, 0}, 	//color matrix
	{0x59, 0xec, 0},
	{0x5a, 0xf5, 0},
	{0x5c, 0xfc, 0},
	{0x5d, 0x60, 0},
	{0x5e, 0xe4, 0},
	{0x60, 0x00, 0},
	{0x61, 0xe0, 0},
	{0x62, 0x60, 0},
	
	{0xd0, 0x00, 0},
 	{0xd1, 0x09, 0},	//noise remove & awb en
 	{0xd8, 0xff, 0},	//update
 	
	{0xe4, 0x10, 0},	//awb
	{0xe8, 0x18, 0},  
	{0xec, 0x14, 0},
	{0xf0, 0xd0, 0},

	{0x19c, 0x01, 0},	//saturation en
//	{0x1a8, 0x40, 0},

};

const TReg gSnrSizeRegValVGA_MT9V011_BAYER[] = 
{   
   	{0x01,	0x0006, 1},
	{0x02,	0x0011, 1},
	{0x03,	0x01e5, 1},
	{0x04,	0x0285, 1},
       {0x05,    0x005c, 1},
	{0x06,	0x0003, 1},
	{0x07,	0x3002, 1},
	{0x20,	0x1100, 1},
	{0x30,	0x0005, 1},
	{0x31,	0x0000, 1},
	{0x34,	0x0100, 1},
       {0x35,    0x0050, 1},
	{0x3d,	0x068f, 1},
	{0x40,	0x01e0, 1},
	
//	{0x58,	0x0078, 1},
	
	{0x62,	0x0411, 1},
		
   
	{0x2b,	0x0040, 1},
	{0x2c,	0x0047, 1},
	{0x2d,	0x006e, 1},
	{0x2e,	0x0040, 1},
/*  
       {0x2b,	0x0040, 1},
	{0x2c,	0x0040, 1},
	{0x2d,	0x0040, 1},
	{0x2e,	0x0040, 1},
*/
};




const TSnrSizeCfg gSnrSizeCfg_MT9V011_BAYER[] = 
{
	{
		{sizeof(gSnrSizeRegValVGA_MT9V011_BAYER) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9V011_BAYER},
		{0, NULL},//{sizeof(gVispRegValQXGA_MT9V011_BAYER) / sizeof(TReg), (PTReg)&gVispRegValQXGA_MT9V011_BAYER},
		{640, 480},
		30,				// maxet
		0,				// has to use sensor pll
		1
	},
	{
		{sizeof(gSnrSizeRegValVGA_MT9V011_BAYER) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9V011_BAYER},
		{0, NULL},//{sizeof(gVispRegValQXGA_MT9V011_BAYER) / sizeof(TReg), (PTReg)&gVispRegValQXGA_MT9V011_BAYER},
		{640, 480},
		30,				// maxet
		0,				// has to use sensor pll
		1
	}
};

const TSnrInfo gSensorInfo_MT9V011_BAYER = 
{
	"MT9V011 Bayer",

	0,													//bayer mode
	1,													//use PCLK of sensor
	0,													
	0,													//0 -- I2C, 1 -- Serial bus;
	0,													//[0]: 1 -- OV I2C bus
	80,													//I2C Rate : 100KHz
	0xba,												//I2C address
	0,													//ISP I2C address for special sensor
	0x07,												//power config
	0,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0x50,													//saturation
	{
	//	0x00,0x06,0x1c,0x38,0x57,0x71,0x88,0x9f,0xb3,0xc3,0xd1,0xdd,0xe6,0xee,0xf6,0xfb,0xff
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
	//	0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
    
            0x00,0x13,0x38,0x59,0x79,0x92,0xa7,0xb9,0xc8,0xd4,0xdf,0xe7,0xee,0xf4,0xf9,0xfc,0xff //gamma table
    //0x00,0x04,0x15,0x30,0x55,0x71,0x88,0x9f,0xb3,0xc3,0xd1,0xdd,0xe6,0xee,0xf6,0xfb,0xff//new
	//0x00,0x04,0x16,0x30,0x4E,0x68,0x81,0x98,0xAC,0xBE,0xCD,0xDA,0xE4,0xED,0xF5,0xfB,0xff
	//	0x00,0x02,0x0f,0x25,0x41,0x5b,0x74,0x8c,0xa2,0xb6,0xc6,0xd3,0xde,0xe5,0xee,0xf6,0xff 	
	//	0x00,0x00,0x09,0x19,0x32,0x4a,0x64,0x7d,0x95,0xaa,0xbd,0xcd,0xdb,0xe7,0xf1,0xf9,0xff 	

	},
	{sizeof(gSifPwrOnSeqRegVal_MT9V011_BAYER) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_MT9V011_BAYER},	
	{0, NULL},												//sensor standby
	{0, NULL},	    										//Sif config sequence(Reg.800 bit0~2) 
	{{sizeof(gSifRegVal_MT9V011_BAYER) / sizeof(TReg), (PTReg)&gSifRegVal_MT9V011_BAYER}},			//sif initial value
	{sizeof(gIspRegVal_MT9V011_BAYER) / sizeof(TReg), (PTReg)&gIspRegVal_MT9V011_BAYER},			//isp initial value
	{0, NULL},//{sizeof(gSensorInit_MT9V011_BAYER) / sizeof(TReg), (PTReg)&gSensorInit_MT9V011_BAYER},			//sensor initial configure
	{sizeof(gSnrSizeCfg_MT9V011_BAYER) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_MT9V011_BAYER},	//size configure
	{0,NULL},

	SensorSetReg_MT9V011_BAYER,										//set reg callback
	SensorGetReg_MT9V011_BAYER,										//get reg callback
	SensorIdChk_MT9V011_BAYER,										//Sensor ID Check Call
	SensorSetMirrorFlip_MT9V011_BAYER,									//Set Sensor Mirror Flip Call
	SensorSetExposeTime_MT9V011_BAYER,									//set et callback
	NULL,													        //adjust contrast	
	NULL,                                      //adjust brightness     
       NULL,                                      //adjust work mode      
	NULL,//SensorGetET_MT9V011_BAYER,      //get ae/awb para       
	NULL,//SensorSetET_MT9V011_BAYER,       //set ae/awb para       
	
	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x74, 0x8, 0x9f, 0x60, 0x20, 1, 0x60, 1, 0xe0, 0x40, 0					
	},
	{//	ythd of flash,et of flash
		0x20,			10
	},
	1													//Histogram ratio number, 1 is recommend value
};

#endif


// 2M

#if V5_DEF_SNR_OV2640_BAYER

void SensorSetReg_OV2640_BAYER(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteByte(uAddr, (UINT8)uVal,1);
}

void SensorGetReg_OV2640_BAYER(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadByte(uAddr, (UINT8*)uVal,1);
}

UINT8 SensorIdChk_OV2640_BAYER(void)
{
	UINT8 val = 0;
	VIM_SIF_I2cWriteByte(0xff, 0x01,1); 
	VIM_SIF_I2cReadByte(0x0a, &val,1);
	if(val != 0x26)
		return FAILED;
	VIM_SIF_I2cReadByte(0x0b, &val,1);
	if(val == 0x42)
		return SUCCEED;
	return FAILED;
}
/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_OV2640_BAYER(UINT8 val)
{
	
}

/********************************************************************************
Description:	Write sensor register to change exposure time
Parameter: t -- exposure time clock num 
freqtype -- frequence type 50 or 60
*********************************************************************************/
void SensorSetExposeTime_OV2640_BAYER(UINT8 freqtype, UINT32 t, TSize size, UINT32 snrclock)
{
	UINT16 uTemp = 0, vBlank = 0;
	UINT8 uVal1,uVal2;
	TI2cBatch AeData[3];
	switch(size.cx)
	{
	case 1600:
		if(t/1922>1248)
		{	
			uTemp = 1248;
			vBlank = (UINT16)(t/1922-1248);
		}
		else
		{
			uTemp = (UINT16)(t/1922);	
			vBlank = 0;
		}
		break;
	case 800:
		if(t/1190>672)
		{	
			uTemp = 672;
			vBlank = (UINT16)(t/1190-672);
		}
		else
		{
			uTemp = (UINT16)(t/1190);	
			vBlank = 0;
		}
		break;
	default:
		break;
	}

	if(uTemp<=1)  uTemp=1;
	VIM_SIF_I2cWriteByte(0xff, 1,1);
	VIM_SIF_I2cWriteByte(0x2d, (UINT8)vBlank,1);
	VIM_SIF_I2cWriteByte(0x2e, (UINT8)(vBlank>>8),1);

	VIM_SIF_I2cReadByte(0x04, &uVal1,1);
	VIM_SIF_I2cReadByte(0x45, &uVal2,1);
		
	AeData[0].RegBytes  = 1;
	AeData[0].RegAddr   = 0x04;	
	AeData[0].RegVal[0] = (UINT8)((uTemp&0x03)|(uVal1&0xfc));			//low 2 bits

	AeData[1].RegBytes  = 1;
	AeData[1].RegAddr   = 0x10;
	AeData[1].RegVal[0]= (UINT8)(uTemp>>2);							//middle 8 bits

	AeData[2].RegBytes  = 1;
	AeData[2].RegAddr   = 0x45;
	AeData[2].RegVal[0] = (UINT8)(((uTemp>>10)&0x3f)|(uVal2&0xc0));		//high 6 bits

	VIM_SIF_I2cAeBatch(3, AeData,1); 
	
}

void SensorGetET_OV2640_BAYER(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
    UINT8 i,uTemp1,uTemp2,uTemp3,vBlank1,vBlank2;
    UINT16 uTemp,vBlank;


    VIM_SIF_I2cWriteByte(0xff, 1,1);    //READ AE
    
    VIM_SIF_I2cReadByte(0x04,&uTemp1,1);
    VIM_SIF_I2cReadByte(0x10,&uTemp2,1);
    VIM_SIF_I2cReadByte(0x45,&uTemp3,1);
    VIM_SIF_I2cReadByte(0x2d,&vBlank1,1);
    VIM_SIF_I2cReadByte(0x2e,&vBlank2,1);
    VIM_USER_DelayMs(20);
    
    *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
    for (i=0;i<3;i++)
        AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);

    switch (index)
        {
        case 0:
           break;
        case 1:
    
            uTemp=(UINT16)(((uTemp3&0x3f)<<10)|(uTemp2<<2)|(uTemp1&0x03));
            vBlank=(UINT16)((vBlank2<<8)|vBlank1);
            if (uTemp<672)
                    *t=uTemp*1190;
                else
                    *t=(vBlank+672)*1190;
            
            (*t)=(*t)*4;
            break;
         }
    
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
    VIM_USER_PrintDec("*Ggain=\n",*Ggain);
    VIM_USER_PrintDec("*t=\n",*t);
#endif	

}
    
void SensorSetET_OV2640_BAYER(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
    UINT8 i,uVal1,uVal2;
    UINT16 uTemp, vBlank;
    TI2cBatch AeData[3];
    switch (index)
        {
        case 0:
		if(t/1922<1248)
		{	
			uTemp = (UINT16)(t/1922);
			vBlank = 0;
		}
		else
		{
			uTemp = 1248;
			vBlank = (UINT16)(t/1922-1248);
		}
		break;
	case 1:
		break;
	default:
		break;
        }   
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
                VIM_USER_PrintDec("t=\n",t);
                VIM_USER_PrintDec("Ggain=\n",Ggain);
#endif	

	if(uTemp<=1)  uTemp=1;
    
	VIM_SIF_I2cWriteByte(0xff, 1,1);
	VIM_SIF_I2cWriteByte(0x2d, (UINT8)vBlank,1);
	VIM_SIF_I2cWriteByte(0x2e, (UINT8)(vBlank>>8),1);
        VIM_USER_DelayMs(50);
	VIM_SIF_I2cReadByte(0x04, &uVal1,1);
	VIM_SIF_I2cReadByte(0x45, &uVal2,1);
        VIM_USER_DelayMs(50);
		
	AeData[0].RegBytes  = 1;
	AeData[0].RegAddr   = 0x04;	
	AeData[0].RegVal[0] = (UINT8)((uTemp&0x03)|(uVal1&0xfc));		//low 2 bits

	AeData[1].RegBytes  = 1;
	AeData[1].RegAddr   = 0x10;
	AeData[1].RegVal[0]= (UINT8)(uTemp>>2);					        //middle 8 bits

	AeData[2].RegBytes  = 1;
	AeData[2].RegAddr   = 0x45;
	AeData[2].RegVal[0] = (UINT8)(((uTemp>>10)&0x3f)|(uVal2&0xc0));	//high 6 bits

	VIM_SIF_I2cAeBatch(3, AeData,1); 
        VIM_USER_DelayMs(100);
    
        VIM_HIF_SetReg8(V5_REG_ISP_GLBGAIN,Ggain);    
        for (i=0;i<3;i++)
            VIM_HIF_SetReg8(V5_REG_ISP_RGBGAIN+i, AWB[i]);
    
        VIM_USER_DelayMs(50);
}

const TReg gSifPwrOnSeqRegVal_OV2640_BAYER[] =
{
	{0x00, 0x21, 1},
	{0x00, 0x20, 1},
	{0x00, 0x21, 1}
};

const TReg	gSifRegVal_OV2640_BAYER[] =
{
	{0x0c, 0x0a, 0},
	{0x10, 0x04, 0},
};

const TReg	gIspRegVal_OV2640_BAYER[] =
{
	{0x00, 0xa1, 0},
       {0x01, 0x7d, 0},
       {0x14, 0x08, 0},     // 0.5x
       {0x15, 0x0c, 0},
	{0x40, 0x40, 0},	//digital gain
	{0x41, 0x40, 0},  
	{0x42, 0x40, 0},
	{0x48, 0x60, 0},	//global gain
	{0x4c, 0x20, 0},
 	
 	{0x58, 0x4f, 0},  	//color matrix
	{0x59, 0xf8, 0},
	{0x5a, 0xf9, 0},
	{0x5c, 0xf8, 0},  
	{0x5d, 0x5f, 0},
	{0x5e, 0xe9, 0},
	{0x60, 0x00, 0},  
	{0x61, 0xe0, 0},
	{0x62, 0x60, 0},
 	
 	{0xd0, 0x00, 0},
 	{0xd1, 0x49, 0},	//noise remove & awb en
 	{0xd8, 0xff, 0},	//update
 	
	{0xe4, 0x10, 0},	//awb
	{0xe8, 0x18, 0},  
	{0xec, 0x14, 0},
	{0xf0, 0xd0, 0},

       {0x11c, 0x8, 0},
	{0x19c, 0x01, 0},	//saturation en
};

const TReg gVispRegValSVGA_OV2640_BAYER[] =
{		
    {0x600a0100, 0x240, 0},    //AE start/stop
    {0x600a0108, 0x250, 0},
};

const TReg gVispRegValUXGA_OV2640_BAYER[] =
{
    {0x600a0100, 0x4a0, 0},    //AE start/stop
    {0x600a0108, 0x4b0, 0},
};

const TReg gSnrSizeRegValSVGA_OV2640_BAYER[] = 
{	
	{0xff, 0x01, 1},
	{0x12, 0x80, 1},
	{0xff, 0x00, 1},
	{0x2c, 0xff, 1},
	{0x2e, 0xdf, 1},
	{0x05, 0x01, 1},
	{0xff, 0x01, 1},
	{0x3c, 0x32, 1},
	{0x11, 0x00, 1},    //mclk/2
	{0x09, 0x02, 1},
	{0x04, 0x28, 1},
	{0x13, 0xe0, 1},
	{0x14, 0x48, 1},
	{0x2c, 0x0c, 1},
	{0x33, 0x78, 1},
	{0x3a, 0x33, 1},
	{0x3b, 0xfB, 1},
	{0x3e, 0x00, 1},
	{0x43, 0x11, 1},
	{0x16, 0x10, 1},
	{0x12, 0x40, 1},
	{0x03, 0x0a, 1},
	{0x17, 0x11, 1},
	{0x18, 0x43, 1},
	{0x19, 0x00, 1},
	{0x1a, 0x4b, 1},
	{0x32, 0x09, 1},    //pixel clock
	{0x37, 0xc0, 1},
	{0x39, 0x12, 1},
	{0x35, 0xda, 1},
	{0x22, 0x1a, 1},
	{0x37, 0xc3, 1},
	{0x23, 0x00, 1},
	{0x34, 0xc0, 1},
	{0x36, 0x1a, 1},
	{0x06, 0x88, 1},
	{0x07, 0xc0, 1},
	{0x0d, 0x87, 1},
	{0x0e, 0x41, 1},
	{0x4c, 0x00, 1},
	{0x4a, 0x81, 1},
	{0x21, 0x99, 1},
	{0x24, 0x38, 1},
	{0x25, 0x30, 1},
	{0x26, 0x82, 1},
	{0x5c, 0x00, 1},
	{0x63, 0x00, 1},
	{0x61, 0x70, 1},
	{0x62, 0x80, 1},
	{0x7c, 0x05, 1},
	{0x20, 0x80, 1},
	{0x28, 0x30, 1},
	{0x6c, 0x00, 1},
	{0x6d, 0x00, 1},
	{0x6e, 0x00, 1},
	{0x70, 0x02, 1},
	{0x71, 0x94, 1},
	{0x73, 0xc1, 1},
	{0x3d, 0x38, 1},    //PLL
	{0x5a, 0x23, 1},
	{0x4f, 0xca, 1},
	{0x50, 0xa8, 1},
	{0xff, 0x00, 1},
	{0xe5, 0x7f, 1},
	{0xf9, 0xc0, 1},
	{0x41, 0x24, 1},
	{0xe0, 0x14, 1},
	{0x76, 0xff, 1},
	{0x33, 0xa0, 1},
	{0x42, 0x20, 1},
	{0x43, 0x18, 1},
	{0x4c, 0x00, 1},
	{0x87, 0xd0, 1},
	{0x88, 0x3f, 1},
	{0xd7, 0x03, 1},
	{0xd9, 0x10, 1},
	{0xc0, 0x64, 1},
	{0xc1, 0x4B, 1},
	{0x8c, 0x00, 1},
	{0x86, 0x1D, 1},
	{0x50, 0x00, 1},
	{0x51, 0xC8, 1},
	{0x52, 0x96, 1},
	{0x53, 0x00, 1},
	{0x54, 0x00, 1},
	{0x55, 0x00, 1},
	{0x5a, 0xA0, 1},
	{0x5b, 0x78, 1},
	{0x5c, 0x00, 1},
	{0xd3, 0x82, 1},
	{0xe5, 0x1f, 1},
	{0xe1, 0x67, 1},
	{0xe0, 0x00, 1},
	{0xdd, 0x7f, 1},
	{0x05, 0x01, 1},


};

const TReg gSnrSizeRegValUXGA_OV2640_BAYER[] = 
{	
	{0xff, 0x01, 1},
	{0x12, 0x80, 1},
	{0xff, 0x00, 1},
	{0x2c, 0xff, 1},
	{0x2e, 0xdf, 1},
	{0x05, 0x01, 1},
	{0xff, 0x01, 1},
	{0x3c, 0x32, 1},
//	{0x11, 0x01, 1},
        {0x11, 0x00, 1},

	{0x09, 0x02, 1},
	{0x04, 0x28, 1},
	{0x13, 0xe0, 1},
	{0x14, 0x48, 1},
	{0x2c, 0x0c, 1},
	{0x33, 0x78, 1},
	{0x3a, 0x33, 1},
	{0x3b, 0xfB, 1},
	{0x3e, 0x00, 1},
	{0x43, 0x11, 1},
	{0x16, 0x10, 1},
	{0x12, 0x00, 1},
	{0x17, 0x11, 1},
	{0x18, 0x75, 1},
	{0x19, 0x01, 1},
	{0x1a, 0x97, 1},
	{0x32, 0x36, 1},    //pixel clock
	{0x37, 0x40, 1},
	{0x39, 0x02, 1},
	{0x35, 0x88, 1},
	{0x22, 0x00, 1},
	{0x37, 0x40, 1},
	{0x23, 0x00, 1},
	{0x34, 0xa0, 1},
	{0x36, 0x1a, 1},
	{0x06, 0x02, 1},
	{0x07, 0xc0, 1},
	{0x0d, 0xb7, 1},
	{0x0e, 0x01, 1},
	{0x4c, 0x00, 1},
	{0x4a, 0x81, 1},
	{0x21, 0x99, 1},
	{0x24, 0x38, 1},
	{0x25, 0x30, 1},
	{0x26, 0x82, 1},
	{0x5c, 0x00, 1},
	{0x63, 0x00, 1},
	{0x61, 0x70, 1},
	{0x62, 0x80, 1},
	{0x7c, 0x05, 1},
	{0x20, 0x80, 1},
	{0x28, 0x30, 1},
	{0x6c, 0x00, 1},
	{0x6d, 0x80, 1},
	{0x6e, 0x00, 1},
	{0x70, 0x02, 1},
	{0x71, 0x94, 1},
	{0x73, 0xc1, 1},
	{0x3d, 0x38, 1},    //PLL
	{0x5a, 0x57, 1},
	{0x4f, 0xbb, 1},
	{0x50, 0x9c, 1},
	{0xff, 0x00, 1},
	{0xe5, 0x7f, 1},
	{0xf9, 0xc0, 1},
	{0x41, 0x24, 1},
	{0xe0, 0x14, 1},
	{0x76, 0xff, 1},
	{0x33, 0xa0, 1},
	{0x42, 0x20, 1},
	{0x43, 0x18, 1},
	{0x4c, 0x00, 1},
	{0x87, 0xd0, 1},
	{0x88, 0x3f, 1},
	{0xd7, 0x03, 1},
	{0xd9, 0x10, 1},
	{0xd3, 0x82, 1},
	{0xc0, 0xc8, 1},
	{0xc1, 0x96, 1},
	{0x86, 0x1d, 1},
	{0x50, 0x00, 1},
	{0x51, 0x90, 1},
	{0x52, 0x2c, 1},
	{0x53, 0x00, 1},
	{0x54, 0x00, 1},
	{0x55, 0x88, 1},
	{0x57, 0x00, 1},
	{0x5a, 0x90, 1},
	{0x5b, 0x2c, 1},
	{0x5c, 0x05, 1},
	{0xe5, 0x1f, 1},
	{0xe1, 0x67, 1},
	{0xe0, 0x00, 1},
	{0xdd, 0x7f, 1},
	{0x05, 0x01, 1},
	{0xff, 0x01, 1},
	{0x10, 0xc0, 1},                                                          
};

const TSnrSizeCfg gSnrSizeCfg_OV2640_BAYER[] = 
{
	{
		{sizeof(gSnrSizeRegValUXGA_OV2640_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValUXGA_OV2640_BAYER},
		{sizeof(gVispRegValUXGA_OV2640_BAYER) / sizeof(TReg), (PTReg)&gVispRegValUXGA_OV2640_BAYER},
		{1600, 1200},
		30,				// maxet
		1,
		1
	},
	{
		{sizeof(gSnrSizeRegValSVGA_OV2640_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValSVGA_OV2640_BAYER},
		{sizeof(gVispRegValSVGA_OV2640_BAYER) / sizeof(TReg), (PTReg)&gVispRegValSVGA_OV2640_BAYER},
		{800,600},
		30,				// maxet
		1,
		1
	}
};

const TSnrInfo gSensorInfo_OV2640_BAYER = 
{
	"OV2640 Bayer",

	0,													//bayer mode
	1,													//use PCLK of sensor
	0,													//[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
	0,													//0 -- I2C, 1 -- Serial bus;
	0x01,													//[0]: 1 -- OV I2C bus
	80,													//I2C Rate : 100KHz
	0x60,													//I2C address
	0,													//ISP I2C address for special sensor
	1,													//power config
	1,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0x50,												//saturation
	{
		0x00,0x13,0x38,0x59,0x79,0x92,0xa7,0xb9,0xc8,0xd4,0xdf,0xe7,0xee,0xf4,0xf9,0xfc,0xff
	},

	{sizeof(gSifPwrOnSeqRegVal_OV2640_BAYER) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_OV2640_BAYER},	//Sif config sequence(Reg.800 bit0~2) 
	// when sensor power on
	{0,NULL},												//sensor standby
	{0, NULL},												//Sif config sequence(Reg.800 bit0~2) when sensor standby
	{{sizeof(gSifRegVal_OV2640_BAYER) / sizeof(TReg), (PTReg)&gSifRegVal_OV2640_BAYER}},			//sif initial value
	{sizeof(gIspRegVal_OV2640_BAYER) / sizeof(TReg), (PTReg)&gIspRegVal_OV2640_BAYER},			//isp initial value
	{0,NULL},//{sizeof(gSensorInit_OV2640_BAYER) / sizeof(TReg), (PTReg)&gSensorInit_OV2640_BAYER},
	{sizeof(gSnrSizeCfg_OV2640_BAYER) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_OV2640_BAYER},//size configure 
	{0,NULL},
	
	SensorSetReg_OV2640_BAYER,										//set reg callback
	SensorGetReg_OV2640_BAYER,										//get reg callback
	SensorIdChk_OV2640_BAYER,										        //Sensor ID Check Call
	SensorSetMirrorFlip_OV2640_BAYER,									//Set Sensor Mirror Flip Call
	NULL,//SensorSetExposeTime_OV2640_BAYER,								//set et callback
	NULL,													                    //adjust contrast											
	NULL,													                    //adjust brightness
	NULL,													                    //adjust work mode
        SensorGetET_OV2640_BAYER,                                                                     //get ae/awb para
        SensorSetET_OV2640_BAYER,                                                                     //set ae/awb para

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x70, 0x0a, 0x9f, 0x60, 0x20, 1, 0x40, 1, 0xe0, 0x40, 0						//should be add to other sensor configure
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1													//Histogram ratio number, 1 is recommend value
};

#endif



#if V5_DEF_SNR_OV2640_YUV
#if 1
void SensorSetReg_OV2640_YUV(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteByte(uAddr, (UINT8)uVal,1);
}

void SensorGetReg_OV2640_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadByte(uAddr, (UINT8*)uVal,1);
}

UINT8 SensorIdChk_OV2640_YUV(void)
{
	UINT8 val = 0;
	VIM_SIF_I2cWriteByte(0xff, 0x01,1); 
	VIM_SIF_I2cReadByte(0x0a, &val,1);
	if(val != 0x26)
		return FAILED;
	VIM_SIF_I2cReadByte(0x0b, &val,1);
	if(val == 0x41||val == 0x42)
		return SUCCEED;
	return FAILED;
}
/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_OV2640_YUV(UINT8 val)
{   
    UINT8 uTemp=0;
    VIM_SIF_I2cWriteByte(0xff, 0x01,1); 
    VIM_SIF_I2cReadByte(0x04, &uTemp,1);
    if (val==0)
        val=3;
    else if (val==3)
        val=0;
    switch(val)
        {
            case 0:
                VIM_SIF_I2cWriteByte(0x04, (0x20 |(uTemp&0x0f)),1); 
                break;
            case 1:
                VIM_SIF_I2cWriteByte(0x04, (0x80|(uTemp&0x0f)),1); 
                break;
            case 2:
                VIM_SIF_I2cWriteByte(0x04, (0x70|(uTemp&0x0f)),1); 
                break;
            case 3:
                VIM_SIF_I2cWriteByte(0x04, (0xd0|(uTemp&0x0f)),1); 
                break;
        }
    
}

void SensorGetET_OV2640_YUV(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{

#if 1
	UINT16 wPrevExpVal = 0;
	UINT32 wPrevGainReal = 0;
	UINT8 reg00 = 0;
	UINT8 reg04 = 0;
        UINT8 reg10 = 0; 
        UINT8 reg45 = 0;
	
        
        VIM_SIF_I2cWriteByte(0xff, 0x01,1);
        VIM_SIF_I2cWriteByte(0x13, 0xe2,1);
        VIM_SIF_I2cReadByte(0x00, &reg00,1);
        VIM_SIF_I2cReadByte(0x04, &reg04,1);
        VIM_SIF_I2cReadByte(0x10, &reg10,1);	
        VIM_SIF_I2cReadByte(0x45, &reg45,1);
        
        wPrevExpVal = reg10 << 2;
        wPrevExpVal += (reg04 & 0x03);
        wPrevExpVal += ((reg45 & 0x3f) << 10);
		
	wPrevGainReal = reg00 & 0x000f;
	wPrevGainReal+=16;

       if( reg00 & 0x10 )
       {
       	wPrevGainReal = wPrevGainReal <<1;
       }
       
       if( reg00 & 0x20 )
       {
       	wPrevGainReal = wPrevGainReal <<1;
       }
       
       if( reg00 & 0x40 )
       {
       	wPrevGainReal = wPrevGainReal <<1;
       }
       
       if( reg00 & 0x80 )
       {
       	wPrevGainReal = wPrevGainReal <<1;
       }
       wPrevGainReal /= 2;
	*Ggain=wPrevGainReal;
	*t=(UINT32)wPrevExpVal;
#else
    UINT8 uTemp1,uTemp2,uTemp3,vBlank1,vBlank2;
    UINT16 uTemp,vBlank;


    VIM_SIF_I2cWriteByte(0xff, 1,1);    //READ AE
    VIM_SIF_I2cWriteByte(0x13,0xe0,1);
    
    VIM_SIF_I2cReadByte(0x00,Ggain,1);
    VIM_SIF_I2cReadByte(0x04,&uTemp1,1);
    VIM_SIF_I2cReadByte(0x10,&uTemp2,1);
    VIM_SIF_I2cReadByte(0x45,&uTemp3,1);
    VIM_SIF_I2cReadByte(0x2d,&vBlank1,1);
    VIM_SIF_I2cReadByte(0x2e,&vBlank2,1);
    VIM_USER_DelayMs(20);
    
    VIM_SIF_I2cWriteByte(0xff, 0,1);    //READ AWB
    VIM_SIF_I2cWriteByte(0xca, 1,1);
    VIM_SIF_I2cReadByte(0xcb,&AWB[0],1);
    VIM_SIF_I2cWriteByte(0xca, 2,1);
    VIM_SIF_I2cReadByte(0xcb,&AWB[1],1);
    VIM_SIF_I2cWriteByte(0xca, 3,1);
    VIM_SIF_I2cReadByte(0xcb,&AWB[2],1);
    VIM_USER_DelayMs(20);

    switch (index)
        {
        case 0:
           break;
        case 1:
    
            uTemp=(UINT16)(((uTemp3&0x3f)<<10)|(uTemp2<<2)|(uTemp1&0x03));
            vBlank=(UINT16)((vBlank2<<8)|vBlank1);
            if (uTemp<672)
                    *t=uTemp*1190;
                else
                    *t=(vBlank+672)*1190;

            break;
         }
#endif
    
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
    VIM_USER_PrintDec("*Ggain=\n",*Ggain);
    VIM_USER_PrintDec("*t=\n",*t);
#endif	

}
    
void SensorSetET_OV2640_YUV(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
#if 1
	UINT16 wPrevExpVal = (UINT16)t;
	UINT16 wSnapGainReal = (UINT16)Ggain;
	UINT8 reg04 = 0;
        UINT8 reg10 = 0; 
        UINT8 reg45 = 0;
        UINT8 reg_tmp = 0;
        UINT8 reg13 = 0;
	  UINT8 gain_reg = 0;	
	
	gain_reg = 0;
	if (wSnapGainReal > 31)
	{
		gain_reg |= 0x10;
		wSnapGainReal = wSnapGainReal >> 1;
	}
	if (wSnapGainReal > 31)
	{
		gain_reg |= 0x20;
		wSnapGainReal = wSnapGainReal >> 1;
	}

	if (wSnapGainReal > 31)
	{
		gain_reg |= 0x40;
		wSnapGainReal = wSnapGainReal >> 1;
	}
	if (wSnapGainReal > 31)
	{
		gain_reg |= 0x80;
		wSnapGainReal = wSnapGainReal >> 1;
	}

	if (wSnapGainReal > 16)
	{
		gain_reg |= ((wSnapGainReal -16) & 0x0f);
	}
	VIM_SIF_I2cWriteByte(0x00, (gain_reg & 0xff),1);
	
        wPrevExpVal /=1;
        VIM_SIF_I2cWriteByte(0xff, 0x01,1);
	 VIM_SIF_I2cWriteByte(0x13, 0xe2,1);	
       // VIM_SIF_I2cReadByte(0x13, &reg13,1);
	//mp_printf("sensor reg 0x13 value is 0x%x\n",reg13);
	
        VIM_SIF_I2cReadByte(0x45, &reg45,1);	
        reg45 = (reg45 & 0xc0);
        reg45 += (wPrevExpVal >> 10);
        VIM_SIF_I2cWriteByte(0x45, reg45,1);
       // VIM_SIF_I2cReadByte(0x45, &reg45,1);
	// mp_printf("sensor reg 0x45 value is 0x%x\n",reg45);
	 
        VIM_SIF_I2cWriteByte(0x10, (wPrevExpVal >> 2),1);
	// VIM_SIF_I2cReadByte(0x10, &reg10,1);
	// mp_printf("sensor reg 0x10 value is 0x%x\n",reg10);
        
        VIM_SIF_I2cReadByte(0x04, &reg04,1);
	 reg_tmp = reg04 & 0xFC;
        reg04 = wPrevExpVal & 0x03;
        reg04 += reg_tmp;
        VIM_SIF_I2cWriteByte(0x04, reg04,1);
	// VIM_SIF_I2cReadByte(0x04, &reg04,1);
	// mp_printf("sensor reg 0x04 value is 0x%x\n",reg04);
	//VIM_SIF_I2cWriteByte(0x00, Ggain,1);	
#else
	UINT8 uVal1,uVal2,Mwb,Awb;
    UINT16 uTemp, vBlank;
    TI2cBatch AeData[3];
    switch (index)
        {
        case 0:
		if(t/1922<1248)
		{	
			uTemp = (UINT16)(t/1922);
			vBlank = 0;
		}
		else
		{
			uTemp = 1248;
			vBlank = (UINT16)(t/1922-1248);
		}
		break;
	case 1:
		break;
	default:
		break;
        }   
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
                VIM_USER_PrintDec("t=\n",t);
                VIM_USER_PrintDec("Ggain=\n",Ggain);
#endif	

	if(uTemp<=1)  uTemp=1;
    
	VIM_SIF_I2cWriteByte(0xff, 1,1);
	VIM_SIF_I2cWriteByte(0x2d, (UINT8)vBlank,1);
	VIM_SIF_I2cWriteByte(0x2e, (UINT8)(vBlank>>8),1);
        VIM_USER_DelayMs(50);
	VIM_SIF_I2cReadByte(0x04, &uVal1,1);
	VIM_SIF_I2cReadByte(0x45, &uVal2,1);
        VIM_USER_DelayMs(50);
		
	AeData[0].RegBytes  = 1;
	AeData[0].RegAddr   = 0x04;	
	AeData[0].RegVal[0] = (UINT8)((uTemp&0x03)|(uVal1&0xfc));		//low 2 bits

	AeData[1].RegBytes  = 1;
	AeData[1].RegAddr   = 0x10;
	AeData[1].RegVal[0]= (UINT8)(uTemp>>2);					        //middle 8 bits

	AeData[2].RegBytes  = 1;
	AeData[2].RegAddr   = 0x45;
	AeData[2].RegVal[0] = (UINT8)(((uTemp>>10)&0x3f)|(uVal2&0xc0));	//high 6 bits

	VIM_SIF_I2cAeBatch(3, AeData,1); 
        VIM_USER_DelayMs(100);
    
	VIM_SIF_I2cWriteByte(0x00, (UINT8)Ggain,1);
	VIM_SIF_I2cWriteByte(0xff, 0,1);    //write AWB gain
        VIM_SIF_I2cReadByte(0xc3,&Awb,1);
        VIM_SIF_I2cWriteByte(0xc3,(Awb&0xf7),1);
        VIM_SIF_I2cReadByte(0xc7,&Mwb,1);
        VIM_SIF_I2cWriteByte(0xc7,(0x40|(Mwb&0xbf)),1);
        
        VIM_USER_DelayMs(50);
        VIM_SIF_I2cWriteByte(0xcc,AWB[0],1);
        VIM_SIF_I2cWriteByte(0xcd,AWB[1],1);
        VIM_SIF_I2cWriteByte(0xce,AWB[2],1);
            
        VIM_USER_DelayMs(50);
      VIM_SIF_I2cWriteByte(0xff,0x1,1);
      VIM_SIF_I2cWriteByte(0x13,0xe5,1);
      VIM_SIF_I2cWriteByte(0xff,0x0,1);
      VIM_SIF_I2cWriteByte(0xc3,0xed,1);
    
        VIM_USER_DelayMs(50);
#endif
}

const TReg gSifPwrOnSeqRegVal_OV2640_YUV[] =
{
	{0x00, 0x61, 1},
	{0x00, 0x60, 1},
	{0x00, 0x61, 1}
};

/*const TReg gSifRegVal_OV2640_YUV[] =
{
	{0x0c, 0x0a, 0},
	{0x10, 0x04, 0},
};*/
const TReg gSifRegVal_OV2640_800X600_YUV[] =
{
	{0x24, 0x031f0000, 0},
	{0x0c, 0x40a, 0},
	{0x10, 0x04, 0},
	{0x84, 0x2, 0},
	{0x08, 0x1b00, 0},
	{0x30, 0x02580000, 0},	
};

const TReg gSifRegVal_OV2640_1600X1200_YUV[] =
{
	{0x24, 0x063f0000, 0},
	{0x08, 0x1b02, 0},
	{0x0c, 0x40a, 0},
	{0x84, 0x2, 0},
	{0x10, 0x04, 0},
	{0x30, 0x04b00000, 0},	
};

const TReg gVispRegValSVGA_OV2640_YUV[] =
{		
	{0x600a0008, 0xb2, 0},	
};

const TReg gVispRegValUXGA_OV2640_YUV[] =
{
	{0x600a0008, 0xb0, 0},	
};

const TReg gSnrSizeRegValSVGA_OV2640_YUV[] = 
{	
#if 0	    
   {0xff, 0x01, 3},
   {0x12, 0x80, 3},
    // {0x46, 0x80, 3},	
     {0xff, 0x00, 3},
     {0x2c, 0xff, 3},
     {0x2e, 0xdf, 3},
     {0xff, 0x01, 3},
     {0x3c, 0x32, 3},
     {0x11, 0x00, 3},
      {0x0f, 0x4b, 0}, //turn on night mode
     {0x03, 0x4f, 0},//turn on night mode   0xcf=1/8  0x8f=1/4  0x4f=1/2
      {0x09, 0x02, 3},
     {0x04, 0x28, 3},
     {0x13, 0xe5, 3},
     {0x14, 0x48, 3},
     {0x2c, 0x0c, 3},
     {0x33, 0x78, 3},
     {0x3a, 0x33, 3},
     {0x3b, 0xfB, 3},
     {0x3e, 0x00, 3},
     {0x43, 0x11, 3},
     {0x16, 0x10, 3},
     {0x39, 0x02, 3},
     {0x35, 0x88, 3},
     {0x22, 0x0a, 3},
     {0x37, 0x40, 3},
     {0x23, 0x00, 3},
     {0x34, 0xa0, 3},
     {0x36, 0x1a, 3},
     {0x06, 0x02, 3},
     {0x07, 0xc0, 3},
     {0x0d, 0xb7, 3},
     {0x0e, 0x01, 3},
     {0x4c, 0x00, 3},
     {0x4a, 0x81, 3},
     {0x21, 0x99, 3},
     {0x24, 0x40, 3},
     {0x25, 0x38, 3},
     {0x26, 0x82, 3},
     {0x5c, 0x00, 3},
     {0x63, 0x00, 3},
     {0x46, 0xdd, 3},  //anti-flicker for 50hz
     {0x61, 0x70, 3},
     {0x62, 0x80, 3},
     {0x7c, 0x05, 3},
     {0x20, 0x80, 3},
     {0x28, 0x30, 3},
     {0x6c, 0x00, 3},
     {0x6d, 0x80, 3},
     {0x6e, 0x00, 3},
     {0x70, 0x02, 3},
     {0x71, 0x94, 3},
     {0x73, 0xc1, 3},
     {0x3d, 0x38, 3},
     {0x5a, 0x57, 3},
     {0x4f, 0xa7, 3},    //anti-flicker for 50hz
     {0x50, 0xa7, 3},    //anti-flicker for 50hz
     {0x12, 0x40, 3},
     {0x17, 0x11, 3},
     {0x18, 0x43, 3},
     {0x19, 0x00, 3},
     {0x1a, 0x4b, 3},
     {0x32, 0x09, 3},
     {0x37, 0xc0, 3},
     {0x4f, 0xca, 3},
     {0x50, 0xa8, 3},
     {0x6d, 0x00, 3},
     {0x3d, 0x38, 3},
     {0xff, 0x00, 3},
     {0xe5, 0x7f, 3},
     {0xf9, 0xc0, 3},
     {0x41, 0x24, 3},
     {0xe0, 0x14, 3},
     {0x76, 0xff, 3},
     {0x33, 0xa0, 3},
     {0x42, 0x20, 3},
     {0x43, 0x18, 3},
     {0x4c, 0x00, 3},
     {0x87, 0xd0, 3},
     {0x88, 0x3f, 3},
     {0xd7, 0x03, 3},
     {0xd9, 0x10, 3},
     {0xd3, 0x82, 3},
     {0xc8, 0x08, 3},
     {0xc9, 0x80, 3},
     {0x7c, 0x00, 3},
     {0x7d, 0x00, 3},
     {0x7c, 0x03, 3},
     {0x7d, 0x48, 3},
     {0x7d, 0x48, 3},
     {0x7c, 0x08, 3},
     {0x7d, 0x20, 3},
     {0x7d, 0x10, 3},
     {0x7d, 0x0e, 3},
     {0x90, 0x00, 3},
     {0x91, 0x0e, 3},
     {0x91, 0x1a, 3},
     {0x91, 0x31, 3},
     {0x91, 0x5a, 3},
     {0x91, 0x69, 3},
     {0x91, 0x75, 3},
     {0x91, 0x7e, 3},
     {0x91, 0x88, 3},
     {0x91, 0x8f, 3},
     {0x91, 0x96, 3},
     {0x91, 0xa3, 3},
     {0x91, 0xaf, 3},
     {0x91, 0xc4, 3},
     {0x91, 0xd7, 3},
     {0x91, 0xe8, 3},
     {0x91, 0x20, 3},
     {0x92, 0x00, 3},
     {0x93, 0x06, 3},
     {0x93, 0xe3, 3},
     {0x93, 0x05, 3},
     {0x93, 0x05, 3},
     {0x93, 0x00, 3},
     {0x93, 0x04, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x96, 0x00, 3},
     {0x97, 0x08, 3},
     {0x97, 0x19, 3},
     {0x97, 0x02, 3},
     {0x97, 0x0c, 3},
     {0x97, 0x24, 3},
     {0x97, 0x30, 3},
     {0x97, 0x28, 3},
     {0x97, 0x26, 3},
     {0x97, 0x02, 3},
     {0x97, 0x98, 3},
     {0x97, 0x80, 3},
     {0x97, 0x00, 3},
     {0x97, 0x00, 3},
     {0xc3, 0xed, 3},
     {0xa4, 0x00, 3},
     {0xa8, 0x00, 3},
     {0xc5, 0x11, 3},
     {0xc6, 0x51, 3},
     {0xbf, 0x80, 3},
     //{0xc7, 0x10, 3},
     {0xb6, 0x66, 3},
     {0xb8, 0xA5, 3},
     {0xb7, 0x64, 3},
     {0xb9, 0x7C, 3},
     {0xb3, 0xaf, 3},
     {0xb4, 0x97, 3},
     {0xb5, 0xFF, 3},
     {0xb0, 0xC5, 3},
     {0xb1, 0x94, 3},
     {0xb2, 0x0f, 3},
     {0xc4, 0x5c, 3},
     {0xc0, 0x64, 3},
     {0xc1, 0x4B, 3},
     {0x8c, 0x00, 3},
     {0x86, 0x3D, 3},
     {0x50, 0x00, 3},
     {0x51, 0xC8, 3},
     {0x52, 0x96, 3},
     {0x53, 0x00, 3},
     {0x54, 0x00, 3},
     {0x55, 0x00, 3},
     {0x5a, 0xC8, 3},
     {0x5b, 0x96, 3},
     {0x5c, 0x00, 3},
     {0xd3, 0x82, 3},
     {0xc3, 0xed, 3},
     {0x7f, 0x00, 3},
     {0xda, 0x01, 3},  //YUV output swap
     {0xe5, 0x1f, 3},
     {0xe1, 0x77, 3},
     {0xe0, 0x00, 3},
     {0xdd, 0x7f, 3},
     {0x05, 0x00, 3},

	 //{0xff, 0x01, 3},  //anti-flicker 50hz
	 //{0x0c, 0x38, 3},
	 
   	{0xff, 0x01, 3},  //anti-flicker 50hz
	{0x0c, 0x3c, 3},
	{0x2e, 0x01, 3},  //25fps preview
	{0x2d, 0x3d, 3},

   	/*{0xff, 0x01, 3},
	{0x0f, 0x4b, 3},
	{0x03, 0x8f, 3},*/
#else
 {0xff, 0x01, 0},
     {0x12, 0x80, 0},
     {0xff, 0x00, 0},
     {0x2c, 0xff, 0},
     {0x2e, 0xdf, 0},
     {0xff, 0x01, 0},
     {0x3c, 0x32, 0},
     {0x11, 0x00, 0},
     {0x0f, 0x4b, 0}, //turn on night mode
     {0x03, 0x4f, 0},//turn on night mode   0xcf=1/8  0x8f=1/4  0x4f=1/2
     
     {0x09, 0x02, 0},
     {0x04, 0x28, 0}, //weiyunlong, we don't need rotate 180 degree when using new sensor module
//     {0x04, 0xf8, 0},
     {0x13, 0xe5, 0},
     {0x14, 0x28, 0},
     {0x2c, 0x0c, 0},
     {0x33, 0x78, 0},
     {0x3a, 0x33, 0},
     {0x3b, 0xfB, 0},
     {0x3e, 0x00, 0},
     {0x43, 0x11, 0},
     {0x16, 0x10, 0},
     {0x39, 0x02, 0},
     {0x35, 0x88, 0},
     {0x22, 0x0a, 0},
     {0x37, 0x40, 0},
     {0x23, 0x00, 0},
     {0x34, 0xa0, 0},
     {0x36, 0x1a, 0},
     {0x06, 0x02, 0},
     {0x07, 0xc0, 0},
     {0x0d, 0xb7, 0},
     {0x0e, 0x01, 0},
     {0x4c, 0x00, 0},
     {0x4a, 0x81, 0},
     {0x21, 0x99, 0},
     {0x24, 0x40, 0},
     {0x25, 0x38, 0},
     {0x26, 0x82, 0},
     {0x5c, 0x00, 0},
     {0x63, 0x00, 0},     
     //{0x46, 0x1b, 0},  //anti-flicker for 50hz
     {0x46, 0x87, 0},  //anti-flicker for 50hz
     //{0x47, 0x02, 0},
     {0x47, 0x00, 0},
     {0x61, 0x70, 0},
     {0x62, 0x80, 0},
     {0x7c, 0x05, 0},
     {0x20, 0x80, 0},
     {0x28, 0x30, 0},
     {0x6c, 0x00, 0},
     {0x6d, 0x80, 0},
     {0x6e, 0x00, 0},
     {0x70, 0x02, 0},
     {0x71, 0x94, 0},
     {0x73, 0xc1, 0},
     {0x3d, 0x38, 0},
     {0x5a, 0x57, 0},
     {0x4f, 0xd0, 0},    //anti-flicker for 50hz
     {0x50, 0xa7, 0},    //anti-flicker for 50hz
     {0x12, 0x40, 0},
     {0x17, 0x11, 0},
     {0x18, 0x43, 0},
     {0x19, 0x00, 0},
     {0x1a, 0x4b, 0},
     {0x32, 0x09, 0},
     {0x37, 0xc0, 0},
     {0x4f, 0xca, 0},
     {0x50, 0xa8, 0},
     {0x6d, 0x00, 0},
     {0x3d, 0x38, 0},
     {0xff, 0x00, 0},
     {0xe5, 0x7f, 0},
     {0xf9, 0xc0, 0},
     {0x41, 0x24, 0},
     {0xe0, 0x14, 0},
     {0x76, 0xff, 0},
     {0x33, 0xa0, 0},
     {0x42, 0x20, 0},
     {0x43, 0x18, 0},
     {0x4c, 0x00, 0},
     {0x87, 0xd0, 0},
     {0x88, 0x3f, 0},
     {0xd7, 0x03, 0},
     {0xd9, 0x10, 0},
     {0xd3, 0x82, 0},
     {0xc8, 0x08, 0},
     {0xc9, 0x80, 0},
     {0x7c, 0x00, 0},
     {0x7d, 0x00, 0},
     {0x7c, 0x03, 0},
     {0x7d, 0x48, 0},
     {0x7d, 0x48, 0},
     {0x7c, 0x08, 0},
     {0x7d, 0x20, 0},
     {0x7d, 0x10, 0},
     {0x7d, 0x0e, 0},
     {0x90, 0x00, 0},
     {0x91, 0x0e, 0},
     {0x91, 0x1a, 0},
     {0x91, 0x31, 0},
     {0x91, 0x5a, 0},
     {0x91, 0x69, 0},
     {0x91, 0x75, 0},
     {0x91, 0x7e, 0},
     {0x91, 0x88, 0},
     {0x91, 0x8f, 0},
     {0x91, 0x96, 0},
     {0x91, 0xa3, 0},
     {0x91, 0xaf, 0},
     {0x91, 0xc4, 0},
     {0x91, 0xd7, 0},
     {0x91, 0xe8, 0},
     {0x91, 0x20, 0},
     {0x92, 0x00, 0},
     {0x93, 0x06, 0},
     {0x93, 0xe3, 0},
     {0x93, 0x05, 0},
     {0x93, 0x05, 0},
     {0x93, 0x00, 0},
     {0x93, 0x04, 0},
     {0x93, 0x00, 0},
     {0x93, 0x00, 0},
     {0x93, 0x00, 0},
     {0x93, 0x00, 0},
     {0x93, 0x00, 0},
     {0x93, 0x00, 0},
     {0x93, 0x00, 0},
     {0x96, 0x00, 0},
     {0x97, 0x08, 0},
     {0x97, 0x19, 0},
     {0x97, 0x02, 0},
     {0x97, 0x0c, 0},
     {0x97, 0x24, 0},
     {0x97, 0x30, 0},
     {0x97, 0x28, 0},
     {0x97, 0x26, 0},
     {0x97, 0x02, 0},
     {0x97, 0x98, 0},
     {0x97, 0x80, 0},
     {0x97, 0x00, 0},
     {0x97, 0x00, 0},
     {0xc3, 0xed, 0},
     {0xa4, 0x00, 0},
     {0xa8, 0x00, 0},
     {0xc5, 0x11, 0},
     {0xc6, 0x51, 0},
     {0xbf, 0x80, 0},
     {0xc7, 0x10, 0},  //turn off simply awb
     {0xb6, 0x66, 0},
     {0xb8, 0xA5, 0},
     {0xb7, 0x64, 0},
     {0xb9, 0x7C, 0},
     {0xb3, 0xaf, 0},
     {0xb4, 0x97, 0},
     {0xb5, 0xFF, 0},
     {0xb0, 0xC5, 0},
     {0xb1, 0x94, 0},
     {0xb2, 0x0f, 0},
     {0xc4, 0x5c, 0},
     {0xc0, 0x64, 0},
     {0xc1, 0x4B, 0},
     {0x8c, 0x00, 0},
     {0x86, 0x3D, 0},
     {0x50, 0x00, 0},
     {0x51, 0xC8, 0},
     {0x52, 0x96, 0},
     {0x53, 0x00, 0},
     {0x54, 0x00, 0},
     {0x55, 0x00, 0},
     {0x5a, 0xC8, 0},
     {0x5b, 0x96, 0},
     {0x5c, 0x00, 0},
     {0xd3, 0x82, 0},
     {0xc3, 0xed, 0},
     {0x7f, 0x00, 0},
     {0xda, 0x01, 0},  //YUV output swap
     {0xe5, 0x1f, 0},
     {0xe1, 0x77, 0},
     {0xe0, 0x00, 0},
     {0xdd, 0x7f, 0},
     {0x05, 0x00, 0},

     //lens shading begin
     {0xC3,0xef,0},
     {0x87,0xd5,0},
     {0xA6,0x00,0},
 
     {0xA7,0xD2,0},
     {0xA7,0x52,0},
     {0xA7,0x10,0},
     {0xA7,0x22,0},
     {0xA7,0x80,0},
     {0xA7,0x25,0},
 
     {0xA7,0xD8,0},
     {0xA7,0x62,0},
     {0xA7,0x0A,0},
     {0xA7,0x22,0},
     {0xA7,0x6C,0},
     {0xA7,0x25,0},
 
     {0xA7,0xCD,0},
     {0xA7,0x72,0},
     {0xA7,0x08,0},
     {0xA7,0x22,0},
     {0xA7,0x14,0},
     {0xA7,0x25,0},
     //lens shading end

     {0xff, 0x01, 0},  //anti-flicker 50hz
     {0x0c, 0x3c, 0},
#endif
	      
};
const TReg gSnrSizeRegValSVGA_OV2640_YUV_VideoRecorder[] = 
{	
	    
   {0xff, 0x01, 3},
   {0x12, 0x80, 3},
    // {0x46, 0x80, 3},	
     {0xff, 0x00, 3},
     {0x2c, 0xff, 3},
     {0x2e, 0xdf, 3},
     {0xff, 0x01, 3},
     {0x3c, 0x32, 3},
     {0x11, 0x00, 3},
     {0x09, 0x02, 3},
     {0x04, 0x28, 3},
     {0x13, 0xe5, 3},
     {0x14, 0x48, 3},
     {0x2c, 0x0c, 3},
     {0x33, 0x78, 3},
     {0x3a, 0x33, 3},
     {0x3b, 0xfB, 3},
     {0x3e, 0x00, 3},
     {0x43, 0x11, 3},
     {0x16, 0x10, 3},
     {0x39, 0x02, 3},
     {0x35, 0x88, 3},
     {0x22, 0x0a, 3},
     {0x37, 0x40, 3},
     {0x23, 0x00, 3},
     {0x34, 0xa0, 3},
     {0x36, 0x1a, 3},
     {0x06, 0x02, 3},
     {0x07, 0xc0, 3},
     {0x0d, 0xb7, 3},
     {0x0e, 0x01, 3},
     {0x4c, 0x00, 3},
     {0x4a, 0x81, 3},
     {0x21, 0x99, 3},
     {0x24, 0x40, 3},
     {0x25, 0x38, 3},
     {0x26, 0x82, 3},
     {0x5c, 0x00, 3},
     {0x63, 0x00, 3},
     //{0x46, 0xdd, 3},  //anti-flicker for 50hz
     {0x46, 0x87, 3},  //anti-flicker for 50hz
     {0x61, 0x70, 3},
     {0x62, 0x80, 3},
     {0x7c, 0x05, 3},
     {0x20, 0x80, 3},
     {0x28, 0x30, 3},
     {0x6c, 0x00, 3},
     {0x6d, 0x80, 3},
     {0x6e, 0x00, 3},
     {0x70, 0x02, 3},
     {0x71, 0x94, 3},
     {0x73, 0xc1, 3},
     {0x3d, 0x38, 3},
     {0x5a, 0x57, 3},
     {0x4f, 0xa7, 3},    //anti-flicker for 50hz
     {0x50, 0xa7, 3},    //anti-flicker for 50hz
     {0x12, 0x40, 3},
     {0x17, 0x11, 3},
     {0x18, 0x43, 3},
     {0x19, 0x00, 3},
     {0x1a, 0x4b, 3},
     {0x32, 0x09, 3},
     {0x37, 0xc0, 3},
     {0x4f, 0xca, 3},
     {0x50, 0xa8, 3},
     {0x6d, 0x00, 3},
     {0x3d, 0x38, 3},
     {0xff, 0x00, 3},
     {0xe5, 0x7f, 3},
     {0xf9, 0xc0, 3},
     {0x41, 0x24, 3},
     {0xe0, 0x14, 3},
     {0x76, 0xff, 3},
     {0x33, 0xa0, 3},
     {0x42, 0x20, 3},
     {0x43, 0x18, 3},
     {0x4c, 0x00, 3},
     {0x87, 0xd0, 3},
     {0x88, 0x3f, 3},
     {0xd7, 0x03, 3},
     {0xd9, 0x10, 3},
     {0xd3, 0x82, 3},
     {0xc8, 0x08, 3},
     {0xc9, 0x80, 3},
     {0x7c, 0x00, 3},
     {0x7d, 0x00, 3},
     {0x7c, 0x03, 3},
     {0x7d, 0x48, 3},
     {0x7d, 0x48, 3},
     {0x7c, 0x08, 3},
     {0x7d, 0x20, 3},
     {0x7d, 0x10, 3},
     {0x7d, 0x0e, 3},
     {0x90, 0x00, 3},
     {0x91, 0x0e, 3},
     {0x91, 0x1a, 3},
     {0x91, 0x31, 3},
     {0x91, 0x5a, 3},
     {0x91, 0x69, 3},
     {0x91, 0x75, 3},
     {0x91, 0x7e, 3},
     {0x91, 0x88, 3},
     {0x91, 0x8f, 3},
     {0x91, 0x96, 3},
     {0x91, 0xa3, 3},
     {0x91, 0xaf, 3},
     {0x91, 0xc4, 3},
     {0x91, 0xd7, 3},
     {0x91, 0xe8, 3},
     {0x91, 0x20, 3},
     {0x92, 0x00, 3},
     {0x93, 0x06, 3},
     {0x93, 0xe3, 3},
     {0x93, 0x05, 3},
     {0x93, 0x05, 3},
     {0x93, 0x00, 3},
     {0x93, 0x04, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x93, 0x00, 3},
     {0x96, 0x00, 3},
     {0x97, 0x08, 3},
     {0x97, 0x19, 3},
     {0x97, 0x02, 3},
     {0x97, 0x0c, 3},
     {0x97, 0x24, 3},
     {0x97, 0x30, 3},
     {0x97, 0x28, 3},
     {0x97, 0x26, 3},
     {0x97, 0x02, 3},
     {0x97, 0x98, 3},
     {0x97, 0x80, 3},
     {0x97, 0x00, 3},
     {0x97, 0x00, 3},
     {0xc3, 0xed, 3},
     {0xa4, 0x00, 3},
     {0xa8, 0x00, 3},
     {0xc5, 0x11, 3},
     {0xc6, 0x51, 3},
     {0xbf, 0x80, 3},
     //{0xc7, 0x10, 3},
     {0xb6, 0x66, 3},
     {0xb8, 0xA5, 3},
     {0xb7, 0x64, 3},
     {0xb9, 0x7C, 3},
     {0xb3, 0xaf, 3},
     {0xb4, 0x97, 3},
     {0xb5, 0xFF, 3},
     {0xb0, 0xC5, 3},
     {0xb1, 0x94, 3},
     {0xb2, 0x0f, 3},
     {0xc4, 0x5c, 3},
     {0xc0, 0x64, 3},
     {0xc1, 0x4B, 3},
     {0x8c, 0x00, 3},
     {0x86, 0x3D, 3},
     {0x50, 0x00, 3},
     {0x51, 0xC8, 3},
     {0x52, 0x96, 3},
     {0x53, 0x00, 3},
     {0x54, 0x00, 3},
     {0x55, 0x00, 3},
     {0x5a, 0xC8, 3},
     {0x5b, 0x96, 3},
     {0x5c, 0x00, 3},
     {0xd3, 0x82, 3},
     {0xc3, 0xed, 3},
     {0x7f, 0x00, 3},
     {0xda, 0x01, 3},  //YUV output swap
     {0xe5, 0x1f, 3},
     {0xe1, 0x77, 3},
     {0xe0, 0x00, 3},
     {0xdd, 0x7f, 3},
     {0x05, 0x00, 3},

	 //{0xff, 0x01, 3},  //anti-flicker 50hz
	 //{0x0c, 0x38, 3},
	 
   	{0xff, 0x01, 3},  //anti-flicker 50hz
	{0x0c, 0x3c, 3},
	//{0x2e, 0x01, 3},  //25fps preview
	//{0x2d, 0x3d, 3},
	      
};

#if 1
const TReg gSnrSizeRegValUXGA_OV2640_YUV[] = 
{
		
		/*{0xff, 0x01, 3}, 
			{0x12, 0x80, 3}, 
			{0xff, 0x00, 3},
			{0x2c, 0xff, 3}, 
			{0x2e, 0xdf, 3}, 
			{0xff, 0x01, 3}, 
			{0x3c, 0x32, 3}, 
			{0x11, 0x01, 3},  
			{0x09, 0x02, 3}, 
			{0x04, 0x28, 3},  
			{0x13, 0xe5, 3},  
			{0x14, 0x48, 3}, 
			{0x2c, 0x0c, 3},  
			{0x33, 0x78, 3},  
			{0x3a, 0x33, 3}, 
			{0x3b, 0xfB, 3}, 
			{0x3e, 0x00, 3},  
			{0x43, 0x11, 3}, 
			{0x16, 0x10, 3}, 
			{0x39, 0x02, 3}, 
			{0x35, 0x88, 3}, 
			{0x22, 0x0a, 3}, 
			{0x37, 0x40, 3}, 
			{0x23, 0x00, 3}, 
			{0x34, 0xa0, 3}, 
			{0x36, 0x1a, 3}, 
			{0x06, 0x02, 3}, 
			{0x07, 0xc0, 3}, 
			{0x0d, 0xb7, 3}, 
			{0x0e, 0x01, 3}, 
			{0x4c, 0x00, 3}, 
			{0x4a, 0x81, 3}, 
			{0x21, 0x99, 3}, 
			{0x24, 0x40, 3},  
			{0x25, 0x38, 3}, 
			{0x26, 0x82, 3}, 
			{0x5c, 0x00, 3}, 
			{0x63, 0x00, 3}, 
			{0x46, 0xa0, 3},  //antiflicker for 50hz
			{0x61, 0x70, 3}, 
			{0x62, 0x80, 3}, 
			{0x7c, 0x05, 3},	
			{0x20, 0x80, 3}, 
			{0x28, 0x30, 3}, 
			{0x6c, 0x00, 3}, 
			{0x6d, 0x80, 3}, 
			{0x6e, 0x00, 3}, 
			{0x70, 0x02, 3}, 
			{0x71, 0x94, 3}, 
			{0x73, 0xc1, 3}, 
			{0x3d, 0x34, 3}, 
			{0x5a, 0x57, 3}, 
			{0x4f, 0xfa, 3},  //antiflicker for 50hz
			{0x50, 0x4e, 3},  //antiflicker for 50hz
			{0xff, 0x00, 3}, 
			{0xe5, 0x7f, 3}, 
			{0xf9, 0xc0, 3}, 
			{0x41, 0x24, 3}, 
			{0xe0, 0x14, 3}, 
			{0x76, 0xff, 3}, 
			{0x33, 0xa0, 3},
			{0x42, 0x20, 3},
			{0x43, 0x18, 3},
			{0x4c, 0x00, 3},
			{0x87, 0xd0, 3},
			{0x88, 0x3f, 3}, 
			{0xd7, 0x03, 3},
			{0xd9, 0x10, 3},
			{0xd3, 0x82, 3}, 
			{0xc8, 0x08, 3}, 
			{0xc9, 0x80, 3}, 
			{0x7c, 0x00, 3}, 
			{0x7d, 0x00, 3}, 
			{0x7c, 0x03, 3}, 
			{0x7d, 0x48, 3}, 
			{0x7d, 0x48, 3}, 
			{0x7c, 0x08, 3}, 
			{0x7d, 0x20, 3}, 
			{0x7d, 0x10, 3}, 
			{0x7d, 0x0e, 3}, 
			{0x90, 0x00, 3}, 
			{0x91, 0x0e, 3},
			{0x91, 0x1a, 3},
			{0x91, 0x31, 3},
			{0x91, 0x5a, 3},
			{0x91, 0x69, 3},
			{0x91, 0x75, 3},
			{0x91, 0x7e, 3},
			{0x91, 0x88, 3},
			{0x91, 0x8f, 3},
			{0x91, 0x96, 3},
			{0x91, 0xa3, 3},
			{0x91, 0xaf, 3},
			{0x91, 0xc4, 3},
			{0x91, 0xd7, 3},
			{0x91, 0xe8, 3},
			{0x91, 0x20, 3},
			{0x92, 0x00, 3}, 
			{0x93, 0x06, 3}, 
			{0x93, 0xe3, 3},
			{0x93, 0x05, 3},
			{0x93, 0x05, 3},
			{0x93, 0x00, 3},
			{0x93, 0x04, 3},
			{0x93, 0x00, 3},
			{0x93, 0x00, 3},
			{0x93, 0x00, 3},
			{0x93, 0x00, 3},
			{0x93, 0x00, 3},
			{0x93, 0x00, 3},
			{0x93, 0x00, 3},
			{0x96, 0x00, 3}, 
			{0x97, 0x08, 3}, 
			{0x97, 0x19, 3}, 
			{0x97, 0x02, 3}, 
			{0x97, 0x0c, 3}, 
			{0x97, 0x24, 3}, 
			{0x97, 0x30, 3}, 
			{0x97, 0x28, 3}, 
			{0x97, 0x26, 3}, 
			{0x97, 0x02, 3}, 
			{0x97, 0x98, 3}, 
			{0x97, 0x80, 3}, 
			{0x97, 0x00, 3}, 
			{0x97, 0x00, 3}, 
			{0xc3, 0xed, 3}, 
			{0xa4, 0x00, 3}, 
			{0xa8, 0x00, 3}, 
			{0xc5, 0x11, 3},
			{0xc6, 0x51, 3}, 
			{0xbf, 0x80, 3}, 
			//{0xc7, 0x10, 3}, 
			{0xb6, 0x66, 3}, 
			{0xb8, 0xA5, 3},
			{0xb7, 0x64, 3},
			{0xb9, 0x7C, 3},
			{0xb3, 0xaf, 3},
			{0xb4, 0x97, 3},
			{0xb5, 0xFF, 3},
			{0xb0, 0xC5, 3},
			{0xb1, 0x94, 3},
			{0xb2, 0x0f, 3},
			{0xc4, 0x5c, 3}, 
			{0xc0, 0xc8, 3}, 
			{0xc1, 0x96, 3}, 
			{0x86, 0x1d, 3}, 
			{0x50, 0x00, 3},
			{0x51, 0x90, 3},
			{0x52, 0x18, 3},
			{0x53, 0x00, 3},
			{0x54, 0x00, 3},
			{0x55, 0x88, 3},
			{0x57, 0x00, 3},
			{0x5a, 0x90, 3},
			{0x5b, 0x18, 3},
			{0x5c, 0x05, 3}, 
			{0xc3, 0xed, 3}, 
			{0x7f, 0x00, 3}, 
			{0xda, 0x01, 3}, 
			{0xe5, 0x1f, 3}, 
			{0xe1, 0x77, 3}, 
			{0xe0, 0x00, 3}, 
			{0xdd, 0x7f, 3}, 
			{0x05, 0x00, 3}, 
		
			{0xff, 0x01, 3},  //anti-flicker 50hz
			{0x0c, 0x3c, 3},*/
			 {0xFF,0x01,0},
 //{0x46, 0xe8, 0},  //anti-flicker for 50hz
 //{0x47, 0x03, 0},
 {0x13, 0xe0, 0}, 
 {0x3D,0x34,1},
 {0x11,0x01,1},
 {0x12,0x00,1},
 {0x17,0x11,1},
 {0x18,0x75,1},
 {0x19,0x01,1},
 {0x1A,0x97,1},
 {0x32,0x36,1},
 {0x39,0x02,1},
 {0x35,0x88,1},
 {0x22,0x0a,1},
 {0x37,0x40,1},
 {0x23,0x00,1},
 {0x34,0xa0,1},
 {0x36,0x1a,1},
 {0x06,0x02,1},
 {0x07,0xc0,1},
 {0x0d,0xb7,1},
 {0x0e,0x01,1},
 {0x4c,0x00,1},
 {0xFF,0x00,1},
 {0xc0,0xc8,1},
 {0xc1,0x96,1},
 {0x8C,0x00,1},
 {0x86,0x3d,1},
 {0x50,0x00,1},
 {0x51,0x90,1},
 {0x52,0x2c,1},
 {0x53,0x00,1},
 {0x54,0x00,1},
 {0x55,0x88,1},
 {0x57,0x00,1},
 {0x5a,0x90,1},
 {0x5b,0x2c,1},
 {0x5c,0x05,1},
 {0xd3,0x82,1},
     //lens shading begin
 {0x87,0xd0,1},
     //lens shading end

		
			
    
};
#endif

const TSnrSizeCfg gSnrSizeCfg_OV2640_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegValUXGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValUXGA_OV2640_YUV},
		{sizeof(gVispRegValUXGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gVispRegValUXGA_OV2640_YUV},
		{1600, 1200},
		30, 			// maxet
		1,	//25M mclk	
		0	//50M pclk
	},

	{
		{sizeof(gSnrSizeRegValSVGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValSVGA_OV2640_YUV},
		{sizeof(gVispRegValSVGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gVispRegValSVGA_OV2640_YUV},
		{800, 600},
		30,				// maxet
		0,  //50M mclk  31fps
		0   //50M pclk
	}
	
};
const TSnrSizeCfg gSnrSizeCfg_OV2640_YUV_VideoRecorder[] = 
{
	{
		{sizeof(gSnrSizeRegValSVGA_OV2640_YUV_VideoRecorder) / sizeof(TReg), (PTReg)&gSnrSizeRegValSVGA_OV2640_YUV_VideoRecorder},
		{sizeof(gSnrSizeRegValSVGA_OV2640_YUV_VideoRecorder) / sizeof(TReg), (PTReg)&gSnrSizeRegValSVGA_OV2640_YUV_VideoRecorder},
		{800, 600},
		30,				// maxet
		0,  //50M mclk  31fps
		0   //50M pclk
	}
	
};

const TSnrInfo gSensorInfo_OV2640_YUV = 
{
	"OV2640 YUV",

	1,													//YUV mode
	1,													//use PCLK of sensor
	0,													//[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
	0,													//0 -- I2C, 1 -- Serial bus;
	0x01,												//[0]: 1 -- OV I2C bus
	80,												       //I2C Rate : 100KHz
	0x30,												//I2C address
	0,													//ISP I2C address for special sensor
	0x01,												//power config
	0,//1,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0,													//saturation
	{
		0x00,0x06,0x1c,0x38,0x57,0x71,0x88,0x9f,0xb3,0xc3,0xcf,0xd7,0xde,0xe5,0xee,0xf6,0xff		//gamma table  
	},
	{sizeof(gSifPwrOnSeqRegVal_OV2640_YUV) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_OV2640_YUV},		//sensor power  on
	{0,NULL},												//sensor standby
	{0, NULL},												//Sif config sequence(Reg.800 bit0~2) when sensor standby
	{{sizeof(gSifRegVal_OV2640_800X600_YUV) / sizeof(TReg), (PTReg)&gSifRegVal_OV2640_800X600_YUV},{sizeof(gSifRegVal_OV2640_1600X1200_YUV) / sizeof(TReg), (PTReg)&gSifRegVal_OV2640_1600X1200_YUV}},						//sif initial value		//sif initial value
	{0,NULL},												//isp initial value
	{0,NULL},//{sizeof(gSensorInit_OV2640_YUV) / sizeof(TReg), (PTReg)&gSensorInit_OV2640_YUV},		//sensor initial configure
	{sizeof(gSnrSizeCfg_OV2640_YUV) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_OV2640_YUV},
	{sizeof(gSnrSizeCfg_OV2640_YUV_VideoRecorder) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_OV2640_YUV_VideoRecorder},	//size configure 

	SensorSetReg_OV2640_YUV,										//set reg callback
	SensorGetReg_OV2640_YUV,										//get reg callback
	SensorIdChk_OV2640_YUV,										//Sensor ID Check Call
       SensorSetMirrorFlip_OV2640_YUV,								//Set Sensor Mirror Flip Call
	NULL,													            //set et callback
	NULL,													            //adjust contrast											
	NULL,													            //adjust brightness
	NULL,													            //adjust work mode
	SensorGetET_OV2640_YUV,										//get ae/awb para
	SensorSetET_OV2640_YUV,										//set ae/awb para
	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x70, 0x0a, 0x9f, 0x60, 0x20, 1, 0x40, 1, 0xe0, 0x40, 0				//should be add to other sensor configure
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1																										//Histogram ratio number, 1 is recommend value
};

#else

void SensorSetReg_OV2640_YUV(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteByte(uAddr, (UINT8)uVal,1);
}

void SensorGetReg_OV2640_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadByte(uAddr, (UINT8*)uVal,1);
}

UINT8 SensorIdChk_OV2640_YUV(void)
{
	UINT8 val = 0;
	VIM_SIF_I2cWriteByte(0xff, 0x01,1); 
	VIM_SIF_I2cReadByte(0x0a, &val,1);
	if(val != 0x26)
		return FAILED;
	VIM_SIF_I2cReadByte(0x0b, &val,1);
	if(val == 0x42)
		return SUCCEED;
	return FAILED;
}
/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_OV2640_YUV(UINT8 val)
{   
    UINT8 uTemp=0;
    VIM_SIF_I2cWriteByte(0xff, 0x01,1); 
    VIM_SIF_I2cReadByte(0x04, &uTemp,1);
    if (val==0)
        val=3;
    else if (val==3)
        val=0;
    switch(val)
        {
            case 0:
                VIM_SIF_I2cWriteByte(0x04, (0x20 |(uTemp&0x0f)),1); 
                break;
            case 1:
                VIM_SIF_I2cWriteByte(0x04, (0x80|(uTemp&0x0f)),1); 
                break;
            case 2:
                VIM_SIF_I2cWriteByte(0x04, (0x70|(uTemp&0x0f)),1); 
                break;
            case 3:
                VIM_SIF_I2cWriteByte(0x04, (0xd0|(uTemp&0x0f)),1); 
                break;
        }
    
}

void SensorGetET_OV2640_YUV(UINT8 index,UINT32 *t,UINT8 *Ggain, UINT8 *AWB)
{
    UINT8 uTemp1,uTemp2,uTemp3,vBlank1,vBlank2;
    UINT16 uTemp,vBlank;


    VIM_SIF_I2cWriteByte(0xff, 1,1);    //READ AE
    VIM_SIF_I2cWriteByte(0x13,0xe0,1);
    
    VIM_SIF_I2cReadByte(0x00,Ggain,1);
    VIM_SIF_I2cReadByte(0x04,&uTemp1,1);
    VIM_SIF_I2cReadByte(0x10,&uTemp2,1);
    VIM_SIF_I2cReadByte(0x45,&uTemp3,1);
    VIM_SIF_I2cReadByte(0x2d,&vBlank1,1);
    VIM_SIF_I2cReadByte(0x2e,&vBlank2,1);
    VIM_USER_DelayMs(20);
    
    VIM_SIF_I2cWriteByte(0xff, 0,1);    //READ AWB
    VIM_SIF_I2cWriteByte(0xca, 1,1);
    VIM_SIF_I2cReadByte(0xcb,&AWB[0],1);
    VIM_SIF_I2cWriteByte(0xca, 2,1);
    VIM_SIF_I2cReadByte(0xcb,&AWB[1],1);
    VIM_SIF_I2cWriteByte(0xca, 3,1);
    VIM_SIF_I2cReadByte(0xcb,&AWB[2],1);
    VIM_USER_DelayMs(20);

    switch (index)
        {
        case 0:
           break;
        case 1:
    
            uTemp=(UINT16)(((uTemp3&0x3f)<<10)|(uTemp2<<2)|(uTemp1&0x03));
            vBlank=(UINT16)((vBlank2<<8)|vBlank1);
            if (uTemp<672)
                    *t=uTemp*1190;
                else
                    *t=(vBlank+672)*1190;

            break;
         }
    
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
    VIM_USER_PrintDec("*Ggain=\n",*Ggain);
    VIM_USER_PrintDec("*t=\n",*t);
#endif	

}
    
void SensorSetET_OV2640_YUV(UINT8 index,UINT32 t,UINT8 Ggain,UINT8 *AWB)
{
    UINT8 uVal1,uVal2,Mwb,Awb;
    UINT16 uTemp, vBlank;
    TI2cBatch AeData[3];
    switch (index)
        {
        case 0:
		if(t/1922<1248)
		{	
			uTemp = (UINT16)(t/1922);
			vBlank = 0;
		}
		else
		{
			uTemp = 1248;
			vBlank = (UINT16)(t/1922-1248);
		}
		break;
	case 1:
		break;
	default:
		break;
        }   
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
                VIM_USER_PrintDec("t=\n",t);
                VIM_USER_PrintDec("Ggain=\n",Ggain);
#endif	

	if(uTemp<=1)  uTemp=1;
    
	VIM_SIF_I2cWriteByte(0xff, 1,1);
	VIM_SIF_I2cWriteByte(0x2d, (UINT8)vBlank,1);
	VIM_SIF_I2cWriteByte(0x2e, (UINT8)(vBlank>>8),1);
        VIM_USER_DelayMs(50);
	VIM_SIF_I2cReadByte(0x04, &uVal1,1);
	VIM_SIF_I2cReadByte(0x45, &uVal2,1);
        VIM_USER_DelayMs(50);
		
	AeData[0].RegBytes  = 1;
	AeData[0].RegAddr   = 0x04;	
	AeData[0].RegVal[0] = (UINT8)((uTemp&0x03)|(uVal1&0xfc));		//low 2 bits

	AeData[1].RegBytes  = 1;
	AeData[1].RegAddr   = 0x10;
	AeData[1].RegVal[0]= (UINT8)(uTemp>>2);					        //middle 8 bits

	AeData[2].RegBytes  = 1;
	AeData[2].RegAddr   = 0x45;
	AeData[2].RegVal[0] = (UINT8)(((uTemp>>10)&0x3f)|(uVal2&0xc0));	//high 6 bits

	VIM_SIF_I2cAeBatch(3, AeData,1); 
        VIM_USER_DelayMs(100);
    
	VIM_SIF_I2cWriteByte(0x00, (UINT8)Ggain,1);
	VIM_SIF_I2cWriteByte(0xff, 0,1);    //write AWB gain
        VIM_SIF_I2cReadByte(0xc3,&Awb,1);
        VIM_SIF_I2cWriteByte(0xc3,(Awb&0xf7),1);
        VIM_SIF_I2cReadByte(0xc7,&Mwb,1);
        VIM_SIF_I2cWriteByte(0xc7,(0x40|(Mwb&0xbf)),1);
        
        VIM_USER_DelayMs(50);
        VIM_SIF_I2cWriteByte(0xcc,AWB[0],1);
        VIM_SIF_I2cWriteByte(0xcd,AWB[1],1);
        VIM_SIF_I2cWriteByte(0xce,AWB[2],1);
            
        VIM_USER_DelayMs(50);
      VIM_SIF_I2cWriteByte(0xff,0x1,1);
      VIM_SIF_I2cWriteByte(0x13,0xe5,1);
      VIM_SIF_I2cWriteByte(0xff,0x0,1);
      VIM_SIF_I2cWriteByte(0xc3,0xed,1);
    
        VIM_USER_DelayMs(50);
}

const TReg gSifPwrOnSeqRegVal_OV2640_YUV[] =
{
	{0x00, 0x61, 1},
	{0x00, 0x60, 1},
	{0x00, 0x61, 1}
};

const TReg gSifRegVal_OV2640_YUV[] =
{
	{0x0c, 0x0a, 0},
	{0x10, 0x04, 0},
};

const TReg gVispRegValSVGA_OV2640_YUV[] =
{		
	{0x600a0008, 0xb2, 0},	
};

const TReg gVispRegValUXGA_OV2640_YUV[] =
{
	{0x600a0008, 0xb0, 0},	
};

const TReg gSnrSizeRegValSVGA_OV2640_YUV[] = 
{	
	    
    {0xff, 0x01, 0},	//page 1 
    {0x12, 0x80, 0}, 

    {0xff, 0x00, 0},	//page 0
    {0x2c, 0xff, 0}, 
    {0x2e, 0xdf, 0}, 

    {0xff, 0x01, 0},	//page 1 
    {0x3c, 0x32, 0}, 
    {0x11, 0x01, 0},	//clk=xvclk/[5:0] 
    {0x09, 0x02, 0}, 
    {0x04, 0x28, 0}, 
    {0x13, 0xe5, 0}, 	//AEC on
    {0x14, 0x48, 0},	//AGC 8x 
    {0x2c, 0x0c, 0}, 
    {0x33, 0x78, 0}, 
    {0x3a, 0x33, 0}, 
    {0x3b, 0xfB, 0}, 
    {0x3e, 0x00, 0}, 
    {0x43, 0x11, 0}, 
    {0x16, 0x10, 0}, 
    {0x39, 0x02, 0}, 
    {0x35, 0x88, 0}, 
    {0x22, 0x0a, 0}, 
    {0x37, 0x40, 0}, 
    {0x23, 0x00, 0}, 
    {0x34, 0xa0, 0}, 
    {0x36, 0x1a, 0}, 
    {0x06, 0x02, 0}, 
    {0x07, 0xc0, 0}, 
    {0x0d, 0xb7, 0}, 
    {0x0e, 0x01, 0}, 
    {0x4c, 0x00, 0}, 
    {0x4a, 0x81, 0}, 
    {0x21, 0x99, 0}, 
    {0x24, 0x40, 0},	//AE target high  
    {0x25, 0x38, 0},        //AE targer low  
    {0x26, 0x82, 0}, 
    {0x5c, 0x00, 0}, 
    {0x63, 0x00, 0}, 
    {0x46, 0x22, 0},
    {0x61, 0x70, 0}, 	//Histogram algorithm low  
    {0x62, 0x80, 0},        //Histogram algorithm high 
    {0x7c, 0x05, 0}, 
    {0x20, 0x80, 0}, 
    {0x28, 0x30, 0}, 
    {0x6c, 0x00, 0}, 
    {0x6d, 0x80, 0}, 
    {0x6e, 0x00, 0}, 
    {0x70, 0x02, 0}, 
    {0x71, 0x94, 0}, 
    {0x73, 0xc1, 0}, 
    {0x3d, 0x34, 0}, 
    {0x5a, 0x57, 0}, 
    {0x12, 0x40, 0},        //svga          
    {0x17, 0x11, 0},
    {0x18, 0x43, 0},
    {0x19, 0x00, 0},
    {0x1a, 0x4b, 0},
    {0x32, 0x09, 0},        //pclk divider
    {0x37, 0xc0, 0},
    {0x6d, 0x00, 0},
    {0x3d, 0x38, 0},        //PLL

    {0xff, 0x00, 0}, 	//page 0
    {0xe5, 0x7f, 0}, 
    {0xf9, 0xc0, 0}, 
    {0x41, 0x24, 0}, 
    {0xe0, 0x14, 0}, 
    {0x76, 0xff, 0}, 
    {0x33, 0xa0, 0},
    {0x42, 0x20, 0},
    {0x43, 0x18, 0},
    {0x4c, 0x00, 0},
    {0x87, 0xd0, 0},
    {0x88, 0x3f, 0}, 
    {0xd7, 0x03, 0},
    {0xd9, 0x10, 0},
    {0xd3, 0x82, 0}, 
    {0xc8, 0x08, 0}, 
    {0xc9, 0x80, 0}, 
    {0x7c, 0x00, 0}, 
    {0x7d, 0x00, 0}, 
    {0x7c, 0x03, 0}, 
    {0x7d, 0x48, 0}, 
    {0x7d, 0x48, 0}, 
    {0x7c, 0x08, 0}, 
    {0x7d, 0x20, 0}, 
    {0x7d, 0x10, 0}, 
    {0x7d, 0x0e, 0}, 
    {0x90, 0x00, 0}, 
    {0x91, 0x0e, 0},
    {0x91, 0x1a, 0},
    {0x91, 0x31, 0},
    {0x91, 0x5a, 0},
    {0x91, 0x69, 0},
    {0x91, 0x75, 0},
    {0x91, 0x7e, 0},
    {0x91, 0x88, 0},
    {0x91, 0x8f, 0},
    {0x91, 0x96, 0},
    {0x91, 0xa3, 0},
    {0x91, 0xaf, 0},
    {0x91, 0xc4, 0},
    {0x91, 0xd7, 0},
    {0x91, 0xe8, 0},
    {0x91, 0x20, 0},
    {0x92, 0x00, 0}, 
    {0x93, 0x06, 0}, 
    {0x93, 0xe3, 0},
    {0x93, 0x05, 0},
    {0x93, 0x05, 0},
    {0x93, 0x00, 0},
    {0x93, 0x04, 0},
    {0x93, 0x00, 0},
    {0x93, 0x00, 0},
    {0x93, 0x00, 0},
    {0x93, 0x00, 0},
    {0x93, 0x00, 0},
    {0x93, 0x00, 0},
    {0x93, 0x00, 0},
    {0x96, 0x00, 0}, 
    {0x97, 0x08, 0}, 
    {0x97, 0x19, 0}, 
    {0x97, 0x02, 0}, 
    {0x97, 0x0c, 0}, 
    {0x97, 0x24, 0}, 
    {0x97, 0x30, 0}, 
    {0x97, 0x28, 0}, 
    {0x97, 0x26, 0}, 
    {0x97, 0x02, 0}, 
    {0x97, 0x98, 0}, 
    {0x97, 0x80, 0}, 
    {0x97, 0x00, 0}, 
    {0x97, 0x00, 0}, 
    {0xc3, 0xed, 0}, 
    {0xa4, 0x00, 0}, 
    {0xa8, 0x00, 0}, 
    {0xc5, 0x11, 0},
    {0xc6, 0x51, 0}, 
    {0xbf, 0x80, 0}, 
    {0xc7, 0x10, 0}, 
    {0xb6, 0x66, 0}, 
    {0xb8, 0xA5, 0},
    {0xb7, 0x64, 0},
    {0xb9, 0x7C, 0},
    {0xb3, 0xaf, 0},
    {0xb4, 0x97, 0},
    {0xb5, 0xFF, 0},
    {0xb0, 0xC5, 0},
    {0xb1, 0x94, 0},
    {0xb2, 0x0f, 0},
    {0xc4, 0x5c, 0}, 
    {0xc0, 0x64, 0},
    {0xc1, 0x4B, 0},
    {0x8c, 0x00, 0},
    {0x86, 0x3D, 0},
    {0x50, 0x00, 0},
    {0x51, 0xC8, 0},
    {0x52, 0x96, 0},
    {0x53, 0x00, 0},
    {0x54, 0x00, 0},
    {0x55, 0x00, 0},
    {0x5a, 0xC8, 0},
    {0x5b, 0x96, 0},
    {0x5c, 0x00, 0},
    {0xd3, 0x82, 0},
    {0xc3, 0xed, 0}, 
    {0x7f, 0x00, 0}, 
    {0xda, 0x00, 0}, 
    {0xe5, 0x1f, 0}, 
    {0xe1, 0x77, 0}, 
    {0xe0, 0x00, 0}, 
    {0xdd, 0x7f, 0}, 
    {0x05, 0x00, 0}, 

    //add by ywq 
    {0xff, 0x01, 1},	//page 1
//    {0x0c, 0x38, 1},	//default set 60Hz manual banding 
    {0x0c, 0x3c, 1},	//set 50Hz manual banding         
//    {0x4e, 0x00, 1},	//50/60Hz banding AEC   @ 25M pclk
//    {0x4f, 0xd2, 1},         //50Hz banding AEC 
//    {0x50, 0xaf, 1},        //60Hz banding AEC
  

    {0x4e, 0x50, 1},	//50/60Hz banding AEC   @ 50M pclk
    {0x4f, 0xa4, 1},        //50Hz banding AEC 
    {0x50, 0x5e, 1},        //60Hz banding AEC

    {0x46, 0xa8, 1},    //FLL   avoid flicker framerate=25fps
    {0x47, 0x00, 1},    //FLH
        
//erase FPN   
    {0xff, 0x01, 0},
    {0x39, 0x12, 0}, 
    {0x35, 0xda, 0}, 
    {0x22, 0x1a, 0}, 
    {0x37, 0xc3, 0}, 
    {0x23, 0x00, 0},
    {0x34, 0xc0, 0}, 
    {0x36, 0x1a, 0},
    {0x06, 0x88, 0}, 
    {0x07, 0xc0, 0},
    {0x0d, 0x87, 0}, 
    {0x0e, 0x41, 0}, 
    {0x4c, 0x00, 0},
    
    {0x0f, 0x4b, 0}, 
    {0x03, 0xcf, 0}, 
	      
};

const TReg gSnrSizeRegValUXGA_OV2640_YUV[] = 
{
		
	{0xff, 0x01, 1},
	{0x12, 0x80, 1},

	{0xff, 0x00, 1},    //page 0
	{0x2c, 0xff, 1},
	{0x2e, 0xdf, 1},
	{0xc2, 0x0c, 1},

	{0xff, 0x01, 1},    //page 1
	{0x03, 0x0f, 1},
	{0x17, 0x11, 1},
	{0x18, 0x75, 1},
	{0x19, 0x01, 1},
	{0x1a, 0x97, 1},
	{0x32, 0x36, 1},    //pclk divider
	{0x3c, 0x32, 1},
	{0x09, 0x02, 1},
	{0x04, 0x28, 1},
	{0x13, 0xe5, 1},
	{0x14, 0x48, 1},
	{0x2c, 0x0c, 1},
	{0x33, 0x78, 1},
	{0x3a, 0x33, 1},
	{0x3b, 0xfb, 1},
	{0x3e, 0x00, 1},
	{0x43, 0x11, 1},
	{0x16, 0x10, 1},
	{0x39, 0x02, 1},
	{0x35, 0x88, 1},
	{0x22, 0x0a, 1},
	{0x37, 0x40, 1},
	{0x23, 0x00, 1},
	{0x34, 0xa0, 1},
	{0x36, 0x1a, 1},
	{0x06, 0x02, 1},
	{0x07, 0xc0, 1},
	{0x0d, 0xb7, 1},
	{0x0e, 0x01, 1},
	{0x4c, 0x00, 1},
	{0x4a, 0x81, 1},
	{0x21, 0x99, 1},
	{0x24, 0x40, 1},
	{0x25, 0x38, 1},
	{0x26, 0x82, 1},
	{0x5c, 0x00, 1},
	{0x63, 0x00, 1},
	{0x61, 0x70, 1},
	{0x62, 0x80, 1},
	{0x7c, 0x05, 1},
	{0x20, 0x80, 1},
	{0x28, 0x30, 1},
	{0x6c, 0x00, 1},
	{0x6d, 0x80, 1},
	{0x6e, 0x00, 1},
	{0x70, 0x02, 1},
	{0x71, 0x94, 1},
	{0x73, 0xc1, 1},
	{0x3d, 0x38, 1},    //PLL
	{0x5a, 0x57, 1},
	{0x4f, 0xbb, 1},
	{0x50, 0x9c, 1},

	{0xff, 0x00, 1},    //page 0
	{0xe5, 0x7f, 1},
	{0xf9, 0xc0, 1},
	{0x41, 0x24, 1},
	{0xe0, 0x14, 1},
	{0x76, 0xff, 1},
	{0x33, 0xa0, 1},
	{0x42, 0x20, 1},
	{0x43, 0x18, 1},
	{0x4c, 0x00, 1},
	{0x87, 0xd0, 1},
	{0x88, 0x3f, 1},
	{0xd7, 0x03, 1},
	{0xd9, 0x10, 1},
	{0xd3, 0x82, 1},
	{0xc8, 0x08, 1},
	{0xc9, 0x80, 1},
	{0x7c, 0x00, 1},
	{0x7d, 0x00, 1},
	{0x7c, 0x03, 1},
	{0x7d, 0x48, 1},
	{0x7d, 0x48, 1},
	{0x7c, 0x08, 1},
	{0x7d, 0x20, 1},
	{0x7d, 0x10, 1},
	{0x7d, 0x0e, 1},
	{0x90, 0x00, 1},
	{0x91, 0x0e, 1},
	{0x91, 0x1a, 1},
	{0x91, 0x31, 1},
	{0x91, 0x5a, 1},
	{0x91, 0x69, 1},
	{0x91, 0x75, 1},
	{0x91, 0x7e, 1},
	{0x91, 0x88, 1},
	{0x91, 0x8f, 1},
	{0x91, 0x96, 1},
	{0x91, 0xa3, 1},
	{0x91, 0xaf, 1},
	{0x91, 0xc4, 1},
	{0x91, 0xd7, 1},
	{0x91, 0xe8, 1},
	{0x91, 0x20, 1},
	{0x92, 0x00, 1},
	{0x93, 0x03, 1},
	{0x93, 0xe3, 1},
	{0x93, 0x03, 1},
	{0x93, 0x03, 1},
	{0x93, 0x00, 1},
	{0x93, 0x01, 1},
	{0x93, 0x00, 1},
	{0x93, 0x00, 1},
	{0x93, 0x00, 1},
	{0x93, 0x00, 1},
	{0x93, 0x00, 1},
	{0x93, 0x00, 1},
	{0x93, 0x00, 1},
	{0x96, 0x00, 1},
	{0x97, 0x08, 1},
	{0x97, 0x19, 1},
	{0x97, 0x02, 1},
	{0x97, 0x0c, 1},
	{0x97, 0x24, 1},
	{0x97, 0x30, 1},
	{0x97, 0x28, 1},
	{0x97, 0x26, 1},
	{0x97, 0x02, 1},
	{0x97, 0x98, 1},
	{0x97, 0x80, 1},
	{0x97, 0x00, 1},
	{0x97, 0x00, 1},
	{0xc3, 0xed, 1},
	{0xa4, 0x00, 1},
	{0xa8, 0x00, 1},
	{0xc5, 0x11, 1},
	{0xc6, 0x51, 1},
	{0xbf, 0x80, 1},
	{0xc7, 0x10, 1},
	{0xb6, 0x66, 1},
	{0xb8, 0xa5, 1},
	{0xb7, 0x64, 1},
	{0xb9, 0x7c, 1},
	{0xb3, 0xaf, 1},
	{0xb4, 0x97, 1},
	{0xb5, 0xff, 1},
	{0xb0, 0xc5, 1},
	{0xb1, 0x94, 1},
	{0xb2, 0x0f, 1},
	{0xc4, 0x5c, 1},
	{0xc0, 0xc8, 1},
	{0xc1, 0x96, 1},
	{0x86, 0x1d, 1},
	{0x50, 0x00, 1},
	{0x51, 0x90, 1},
	{0x52, 0x2c, 1},
	{0x53, 0x00, 1},
	{0x54, 0x00, 1},
	{0x55, 0x88, 1},
	{0x57, 0x00, 1},
	{0x5a, 0x90, 1},
	{0x5b, 0x2c, 1},
	{0x5c, 0x05, 1},
	{0xc3, 0xed, 1},
	{0x7f, 0x00, 1},
	{0xda, 0x01, 1},
	{0xe5, 0x1f, 1},
	{0xe1, 0x77, 1},
	{0xe0, 0x00, 1},
	{0xdd, 0x7f, 1},
	{0x05, 0x00, 1},
	
	{0xff, 0x01, 1},                   

        //add by ywq 
        {0xff, 0x01, 1},    //page 1
//    {0x0c, 0x38, 1},  //default set 60Hz manual banding 
        {0x0c, 0x3c, 1},    //set 50Hz manual banding         
//    {0x4e, 0x00, 1},    //50/60Hz banding AEC   @ 25M pclk
//    {0x4f, 0x82, 1},         //50Hz banding AEC 
//    {0x50, 0x6c, 1},        //60Hz banding AEC
      
    
        {0x4e, 0x40, 1},  //50/60Hz banding AEC   @ 50M pclk
        {0x4f, 0x04, 1},    //50Hz banding AEC 
        {0x50, 0xd8, 1},        //60Hz banding AEC
    
        {0x46, 0x34, 1},    //FLL   avoid flicker framerate=10fps
        {0x47, 0x00, 1},    //FLH

 //erase FPN   
        {0xff, 0x01, 0},
        {0x39, 0x02, 0},
        {0x35, 0x88, 0}, 
        {0x22, 0x0a, 0}, 
        {0x37, 0x40, 0}, 
        {0x23, 0x00, 0}, 
        {0x34, 0xa0, 0}, 
        {0x36, 0x1a, 0}, 
        {0x06, 0x02, 0}, 
        {0x07, 0xc0, 0}, 
        {0x0d, 0xb7, 0}, 
        {0x0e, 0x01, 0}, 
        {0x4c, 0x00, 0}, 

        {0x0f, 0x4b, 0}, 
        {0x03, 0xcf, 0}, 
    
};

const TSnrSizeCfg gSnrSizeCfg_OV2640_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegValUXGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValUXGA_OV2640_YUV},
		{sizeof(gVispRegValUXGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gVispRegValUXGA_OV2640_YUV},
		{1600, 1200},
		30,				// maxet
		1,  //25M mclk  
		0   //50M pclk
	},
	{
		{sizeof(gSnrSizeRegValSVGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValSVGA_OV2640_YUV},
		{sizeof(gVispRegValSVGA_OV2640_YUV) / sizeof(TReg), (PTReg)&gVispRegValSVGA_OV2640_YUV},
		{800,600},
		30,				// maxet
		0,  //50M mclk  31fps
		0   //50M pclk
	}
};

const TSnrInfo gSensorInfo_OV2640_YUV = 
{
	"OV2640 YUV",

	1,													//YUV mode
	1,													//use PCLK of sensor
	0,													//[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
	0,													//0 -- I2C, 1 -- Serial bus;
	0x01,												//[0]: 1 -- OV I2C bus
	80,												       //I2C Rate : 100KHz
	0x60,												//I2C address
	0,													//ISP I2C address for special sensor
	1,													//power config
	0,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0,													//saturation
	{
		0x00,0x06,0x1c,0x38,0x57,0x71,0x88,0x9f,0xb3,0xc3,0xcf,0xd7,0xde,0xe5,0xee,0xf6,0xff		//gamma table  
	},
	{sizeof(gSifPwrOnSeqRegVal_OV2640_YUV) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_OV2640_YUV},		//sensor power  on
	{0,NULL},												//sensor standby
	{0, NULL},												//Sif config sequence(Reg.800 bit0~2) when sensor standby
	{sizeof(gSifRegVal_OV2640_YUV) / sizeof(TReg), (PTReg)&gSifRegVal_OV2640_YUV},				//sif initial value
	{0,NULL},												//isp initial value
	{0,NULL},//{sizeof(gSensorInit_OV2640_YUV) / sizeof(TReg), (PTReg)&gSensorInit_OV2640_YUV},		//sensor initial configure
	{sizeof(gSnrSizeCfg_OV2640_YUV) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_OV2640_YUV},		//size configure 

	SensorSetReg_OV2640_YUV,										//set reg callback
	SensorGetReg_OV2640_YUV,										//get reg callback
	SensorIdChk_OV2640_YUV,										//Sensor ID Check Call
       SensorSetMirrorFlip_OV2640_YUV,								//Set Sensor Mirror Flip Call
	NULL,													            //set et callback
	NULL,													            //adjust contrast											
	NULL,													            //adjust brightness
	NULL,													            //adjust work mode
	SensorGetET_OV2640_YUV,										//get ae/awb para
	SensorSetET_OV2640_YUV,										//set ae/awb para
	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x70, 0x0a, 0x9f, 0x60, 0x20, 1, 0x40, 1, 0xe0, 0x40, 0				//should be add to other sensor configure
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1																										//Histogram ratio number, 1 is recommend value
};
#endif
#endif

#if V5_DEF_SNR_OV2650_YUV
void SensorSetReg_OV2650_YUV(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteByte((UINT16)uAddr, (UINT8)uVal,2);
}

void SensorGetReg_OV2650_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadByte((UINT16)uAddr, (UINT8*)uVal,2);
}

UINT8 SensorIdChk_OV2650_YUV(void)
{
	UINT8 val;

	VIM_SIF_I2cReadByte(0x300a, &val,2);

	if(val != 0x26)
		return FAILED;
	VIM_SIF_I2cReadByte(0x300b, &val,2);
	if(val == 0x52)
		{
		
		return SUCCEED;
		}
	return FAILED;
	
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_OV2650_YUV(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor Brightness  callback 

         Para: 1-5 step

*******************************************************************/
void SensorSetBrightness_OV2650_YUV(UINT8 val)   //add by zhangxiang 20090106
{ //modify by xinggang.xu 20090106 
   MMD_USER_WaitMICRS(300);
   VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	//angela 2006-12-12
   switch(val)
   {
      case BRIGHTNESS_LEVE0:   //brightness -2
	     SensorSetReg_OV2650_YUV(0x3391,0x06);
         SensorSetReg_OV2650_YUV(0x3390,0x49);
         SensorSetReg_OV2650_YUV(0x339a,0x20);
         break;
      case BRIGHTNESS_LEVE1:   //brightness -1
	      SensorSetReg_OV2650_YUV(0x3391,0x06);
         SensorSetReg_OV2650_YUV(0x3390,0x49);
         SensorSetReg_OV2650_YUV(0x339a,0x10);
         break;
      case BRIGHTNESS_LEVE2:   //brightness 0
	     SensorSetReg_OV2650_YUV(0x3391,0x06);
         SensorSetReg_OV2650_YUV(0x3390,0x41);
         SensorSetReg_OV2650_YUV(0x339a,0x00);
         break;
      case BRIGHTNESS_LEVE3:   //brightness 1
	      SensorSetReg_OV2650_YUV(0x3391,0x06);
         SensorSetReg_OV2650_YUV(0x3390,0x41);
         SensorSetReg_OV2650_YUV(0x339a,0x10);
         break;
      case BRIGHTNESS_LEVE4:  //brightness 2
	      SensorSetReg_OV2650_YUV(0x3391,0x06);
         SensorSetReg_OV2650_YUV(0x3390,0x41);
         SensorSetReg_OV2650_YUV(0x339a,0x20);
         break; 
      
      
       default:
         break;
   }
   MMD_USER_WaitMICRS(300);
   VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
}

/******************************************************************

         Desc: set sensor contrast  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetContrast_OV2650_YUV(UINT8 val)
{
	
}

/******************************************************************
         Desc: set sensor mod  callback 
         Para:  val: 	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetBanding_OV2650_YUV(UINT8 val)  //add by zhangxiang 20090106
{  // modify by xinggang.xu 20090106
	switch(val)
   {
      case BANDING_50HZ:   //banding 50Hz, svga-30fps-24m(mclk)-36m(pclk)
      #if 0
	      SensorSetReg_OV2650_YUV(0x3014,0x84);
         SensorSetReg_OV2650_YUV(0x3013,0xf7);
         SensorSetReg_OV2650_YUV(0x3070,0xb9);
         SensorSetReg_OV2650_YUV(0x3071,0x00);
         SensorSetReg_OV2650_YUV(0x301c,0x02);
     #else
          SensorSetReg_OV2650_YUV(0x3014,0x8c);         
         SensorSetReg_OV2650_YUV(0x3070,0xb9);
         SensorSetReg_OV2650_YUV(0x3071,0x00);
         SensorSetReg_OV2650_YUV(0x301c,0x02);
      #endif
         break;
      case BANDING_60HZ:   //banding 60Hz
      #if 0
	      SensorSetReg_OV2650_YUV(0x3014,0x04);
         SensorSetReg_OV2650_YUV(0x3013,0xf7);
         SensorSetReg_OV2650_YUV(0x3070,0x9a);
         SensorSetReg_OV2650_YUV(0x3071,0x00);
         SensorSetReg_OV2650_YUV(0x301c,0x03);
    #else
         SensorSetReg_OV2650_YUV(0x3014,0x0c);         
         SensorSetReg_OV2650_YUV(0x3072,0x9a);
         SensorSetReg_OV2650_YUV(0x3073,0x00);
         SensorSetReg_OV2650_YUV(0x301d,0x03);
    #endif
         break;
#if 0
      case environment_Auto:  //Auto
	      SensorSetReg_OV2650_YUV(0x3306,0x00);
         break; 
      case environment_Sunny:   //sunny 
	      SensorSetReg_OV2650_YUV(0x3306,0x02); 
         SensorSetReg_OV2650_YUV(0x3337,0x5e);
         SensorSetReg_OV2650_YUV(0x3338,0x40);
         SensorSetReg_OV2650_YUV(0x3339,0x46);
         break;
      case environment_Cloudy:   //cloudy
	      SensorSetReg_OV2650_YUV(0x3306,0x02);
         SensorSetReg_OV2650_YUV(0x3337,0x68);
         SensorSetReg_OV2650_YUV(0x3338,0x40);
         SensorSetReg_OV2650_YUV(0x3339,0x4e);
         break;
      case environment_Night:   //home (night)
	      SensorSetReg_OV2650_YUV(0x3306,0x02);
         SensorSetReg_OV2650_YUV(0x3337,0x44);
         SensorSetReg_OV2650_YUV(0x3338,0x40);
         SensorSetReg_OV2650_YUV(0x3339,0x70);
         break;
#endif
       default:
         break;
   }
}
UINT8 last_environment_mode = ENVIRONMENT_MAX;

void SensorSetEnvironment_OV2650_YUV(UINT8 val)  // modify by xinggang.xu 20090106
{
  // static UINT8 last_environment_mode = ENVIRONMENT_MAX;
   switch(val)
   {
      case ENVIRONMENT_AUTIO:  //Auto
	      SensorSetReg_OV2650_YUV(0x3306,0x00);
         if(last_environment_mode == ENVIRONMENT_NIGHT)
         {
            MSG_FATAL("the last mode if night %d", last_environment_mode, 0, 0);
            //add by xinggang.xu 20090114 start
            SensorSetReg_OV2650_YUV(0x300e,0x34);
            SensorSetReg_OV2650_YUV(0x3011,0x00);
            //SensorSetReg_OV2650_YUV(0x302c,0x00);
           /* SensorSetReg_OV2650_YUV(0x3071,0x00);
            SensorSetReg_OV2650_YUV(0x3070,0x5d);
            SensorSetReg_OV2650_YUV(0x301c,0x05);
            SensorSetReg_OV2650_YUV(0x3073,0x00);
            SensorSetReg_OV2650_YUV(0x3072,0x4d);
            SensorSetReg_OV2650_YUV(0x301d,0x06);*/
         //end 20090114
         }
         last_environment_mode = ENVIRONMENT_AUTIO;
         break; 
      case ENVIRONMENT_SUNNY:   //sunny 
	      SensorSetReg_OV2650_YUV(0x3306,0x02); 
         SensorSetReg_OV2650_YUV(0x3337,0x5e);
         SensorSetReg_OV2650_YUV(0x3338,0x40);
         SensorSetReg_OV2650_YUV(0x3339,0x46);
         if(last_environment_mode == ENVIRONMENT_NIGHT)
         {
             MSG_FATAL("the last mode if night %d", last_environment_mode, 0, 0);
            //add by xinggang.xu 20090114 start
            SensorSetReg_OV2650_YUV(0x300e,0x34);
            SensorSetReg_OV2650_YUV(0x3011,0x00);
            //SensorSetReg_OV2650_YUV(0x302c,0x00);
            /*SensorSetReg_OV2650_YUV(0x3071,0x00);
            SensorSetReg_OV2650_YUV(0x3070,0x5d);
            SensorSetReg_OV2650_YUV(0x301c,0x05);
            SensorSetReg_OV2650_YUV(0x3073,0x00);
            SensorSetReg_OV2650_YUV(0x3072,0x4d);
            SensorSetReg_OV2650_YUV(0x301d,0x06);*/
            //end 20090114
         }
         last_environment_mode = ENVIRONMENT_SUNNY;
         break;
      case ENVIRONMENT_CLOUDY:   //cloudy
	      SensorSetReg_OV2650_YUV(0x3306,0x02);
         SensorSetReg_OV2650_YUV(0x3337,0x68);
         SensorSetReg_OV2650_YUV(0x3338,0x40);
         SensorSetReg_OV2650_YUV(0x3339,0x4e);
         if(last_environment_mode == ENVIRONMENT_NIGHT)
         {
            MSG_FATAL("the last mode if night %d", last_environment_mode, 0, 0);
            //add by xinggang.xu 20090114 start
            SensorSetReg_OV2650_YUV(0x300e,0x34);
            SensorSetReg_OV2650_YUV(0x3011,0x00);
            //SensorSetReg_OV2650_YUV(0x302c,0x00);
            /*SensorSetReg_OV2650_YUV(0x3071,0x00);
            SensorSetReg_OV2650_YUV(0x3070,0x5d);
            SensorSetReg_OV2650_YUV(0x301c,0x05);
            SensorSetReg_OV2650_YUV(0x3073,0x00);
            SensorSetReg_OV2650_YUV(0x3072,0x4d);
            SensorSetReg_OV2650_YUV(0x301d,0x06);*/
         //end 20090114
         }
         last_environment_mode = ENVIRONMENT_CLOUDY;
         break;
      case ENVIRONMENT_NIGHT:   //home (night)
      //set the fps as 5 modify by xinggang.xu 20090114 start
      #if 0
	      SensorSetReg_OV2650_YUV(0x3306,0x02);
         SensorSetReg_OV2650_YUV(0x3337,0x44);
         SensorSetReg_OV2650_YUV(0x3338,0x40);
         SensorSetReg_OV2650_YUV(0x3339,0x70);
      #endif
         //SensorSetReg_OV2650_YUV(0x300e,0x34);
         SensorSetReg_OV2650_YUV(0x3011,0x01); //0x5
        /* SensorSetReg_OV2650_YUV(0x302c,0x00);
         SensorSetReg_OV2650_YUV(0x3071,0x00);
         SensorSetReg_OV2650_YUV(0x3070,0x31);
         SensorSetReg_OV2650_YUV(0x301c,0x13);
         SensorSetReg_OV2650_YUV(0x3073,0x00);
         SensorSetReg_OV2650_YUV(0x3072,0x1a);
         SensorSetReg_OV2650_YUV(0x301d,0x17);*/
       //end 20090114 
         last_environment_mode = ENVIRONMENT_NIGHT;
         MSG_FATAL("current mode is night %d", last_environment_mode, 0, 0);
         break;
       default:
         break;
   }
}


void SensorGetET_OV2650_YUV(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
	 UINT16 wPrevExpVal = 0;
         UINT32 wPrevGainReal = 0;
         UINT8 reg00 = 0;
         UINT8 reg02 = 0;
        UINT8 reg03 = 0; 

        VIM_SIF_I2cWriteByte(0x3013, 0xe2,2);

        VIM_SIF_I2cReadByte(0x3000, &reg00,2);

        VIM_SIF_I2cReadByte(0x3002, &reg02,2);

        VIM_SIF_I2cReadByte(0x3003, &reg03,2);     


        wPrevExpVal = reg02 << 8;

        wPrevExpVal += reg03;


         wPrevGainReal = reg00 & 0x000f;

         wPrevGainReal+=16;

 

       if( reg00 & 0x10 )

       {

         wPrevGainReal = wPrevGainReal <<1;

       }

       

       if( reg00 & 0x20 )

       {

         wPrevGainReal = wPrevGainReal <<1;

       }

       

       if( reg00 & 0x40 )

       {

         wPrevGainReal = wPrevGainReal <<1;

       }

       

       if( reg00 & 0x80 )

       {

         wPrevGainReal = wPrevGainReal <<1;

       }

       wPrevGainReal /= 2;

         *Ggain=wPrevGainReal;

         *t=(UINT32)wPrevExpVal;
}

void SensorSetET_OV2650_YUV(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
	 UINT16 wPrevExpVal = (UINT16)t;
         UINT16 wSnapGainReal = (UINT16)Ggain;
         UINT8 reg02 = 0;
         UINT8 reg03 = 0;
           UINT8 gain_reg = 0;        
        VIM_SIF_I2cWriteByte(0x3013, 0xe2,2);        
         gain_reg = 0;
         if (wSnapGainReal > 31)

         {

                   gain_reg |= 0x10;

                   wSnapGainReal = wSnapGainReal >> 1;

         }

         if (wSnapGainReal > 31)

         {

                   gain_reg |= 0x20;

                   wSnapGainReal = wSnapGainReal >> 1;

         }

 

         if (wSnapGainReal > 31)

         {

                   gain_reg |= 0x40;

                   wSnapGainReal = wSnapGainReal >> 1;

         }

         if (wSnapGainReal > 31)

         {

                   gain_reg |= 0x80;

                   wSnapGainReal = wSnapGainReal >> 1;

         }

 

         if (wSnapGainReal > 16)

         {

                   gain_reg |= ((wSnapGainReal -16) & 0x0f);

         }

         VIM_SIF_I2cWriteByte(0x3000, (gain_reg & 0xff),2);

         

        wPrevExpVal /=1;

        reg02 += (wPrevExpVal >> 8);

        VIM_SIF_I2cWriteByte(0x3002, reg02,2);


        reg03 = wPrevExpVal & 0xff;

        VIM_SIF_I2cWriteByte(0x3003, reg03,2);
}

const TReg gSifPwrOnSeqRegVal_OV2650_YUV[] =
{

	{0x00, 0x63, 1},
	{0x00, 0x62, 10},
	{0x00, 0x63, 10}
};

const TReg gSifRegVal_OV2650_YUV[] =
{
	
	{0x0008, 0x1b02, 3},	
	{0x000c, 0x040a, 3},
    {0x0004, 0x0041, 3},
    //{0x0010, 0x0301, 3}
		
};


const TReg gSnrSizeRegVal_OV2650_SVGA_YUV[] =
{
    #if 0
	{0x3012,0x80,10},//Soft Reset;Add some delay after register reset
	
	{0x308c,0x80,0},
	{0x308d,0x0e,0},
	{0x360b,0x00,0},
	{0x30b0,0xff,0},
	{0x30b1,0xff,0},
	{0x30b2,0x24,0},
	{0x300e,0x34,0},
	{0x300f,0xa6,0},
	{0x3010,0x81,0},
	{0x3082,0x01,0},
	{0x30f4,0x01,0},
	{0x3090,0x33,0},
	{0x3091,0xc0,0},
	{0x30ac,0x42,0},
	{0x30d1,0x08,0},
	{0x30a8,0x56,0},
	{0x3015,0x03,0},
	{0x3093,0x00,0},
	{0x307e,0xe5,0},
	{0x3079,0x00,0},
	{0x30aa,0x42,0},
	{0x3017,0x40,0},
	{0x30f3,0x82,0},
	{0x306a,0x0c,0},
	{0x306d,0x00,0},
	{0x336a,0x3c,0},
	{0x3076,0x6a,0},
	{0x30d9,0x8c,0},
	{0x3016,0x82,0},
	{0x3601,0x30,0},
	{0x304e,0x88,0},
	{0x30f1,0x82,0},
	{0x306f,0x14,0},
	{0x302a,0x02,0},
	{0x302b,0x6a,0},
	{0x3012,0x10,0},
	{0x3011,0x01,0},
	
	{0x3013,0xf7,0},//AEC/AGC
	{0x301c,0x13,0},
	{0x301d,0x17,0},
	{0x3070,0x5d,0},
	{0x3072,0x4d,0},

	{0x3018,0x88,0},//AE Target 08.11.14
 	{0x3019,0x78,0},

	{0x30af,0x00,0},//D5060
	{0x3048,0x1f,0},
	{0x3049,0x4e,0},
	{0x304a,0x20,0},
	{0x304f,0x20,0},
	{0x304b,0x02,0},
	{0x304c,0x00,0},
	{0x304d,0x02,0},
	{0x304f,0x20,0},
	{0x30a3,0x10,0},
	{0x3013,0xf7,0},
	//{0x3014,0x44,0},
	{0x3014,0x84,0},//44 change to 84 for 50Hz manual 08.11.14
	{0x3071,0x00,0},
	{0x3070,0x5d,0},
	{0x3073,0x00,0},
	{0x3072,0x4d,0},
	{0x301c,0x05,0},
	{0x301d,0x06,0},
	{0x304d,0x42,0},
	{0x304a,0x40,0},
	{0x304f,0x40,0},
	{0x3095,0x07,0},
	{0x3096,0x16,0},
	{0x3097,0x1d,0},
	
	{0x300e,0x38,0},//Window Setup
	{0x3020,0x01,0},
	{0x3021,0x18,0},
	{0x3022,0x00,0},
	{0x3023,0x06,0},
	{0x3024,0x06,0},
	{0x3025,0x58,0},
	{0x3026,0x02,0},
	{0x3027,0x61,0},
	{0x3088,0x03,0},
	{0x3089,0x20,0},
	{0x308a,0x02,0},
	{0x308b,0x58,0},
	{0x3316,0x64,0},
	{0x3317,0x25,0},
	{0x3318,0x80,0},
	{0x3319,0x08,0},
	{0x331a,0x64,0},
	{0x331b,0x4b,0},
	{0x331c,0x00,0},
	{0x331d,0x38,0},
	{0x3100,0x00,0},
	
	{0x3320,0xfa,0},//AWB
	{0x3321,0x11,0},
	{0x3322,0x92,0},
	{0x3323,0x01,0},
	{0x3324,0x97,0},
	{0x3325,0x02,0},
	{0x3326,0xff,0},
	{0x3327,0x0c,0},
	{0x3328,0x10,0},
	{0x3329,0x10,0},
	{0x332a,0x58,0},
	{0x332b,0x50,0},
	{0x332c,0xbe,0},
	{0x332d,0xe1,0},
	{0x332e,0x43,0},
	{0x332f,0x36,0},
	{0x3330,0x4d,0},
	{0x3331,0x44,0},
	{0x3332,0xf8,0},
	{0x3333,0x0a,0},
	{0x3334,0xf0,0},
	{0x3335,0xf0,0},
	{0x3336,0xf0,0},
	{0x3337,0x40,0},
	{0x3338,0x40,0},
	{0x3339,0x40,0},
	{0x333a,0x00,0},
	{0x333b,0x00,0},
	
	{0x3380,0x28,0},//Color Matrix
	{0x3381,0x48,0},
	{0x3382,0x10,0},
	{0x3383,0x23,0},
	{0x3384,0xc0,0},
	{0x3385,0xe5,0},
	{0x3386,0xc2,0},
	{0x3387,0xb3,0},
	{0x3388,0x0e,0},
	{0x3389,0x98,0},
	{0x338a,0x01,0},
	
	{0x3340,0x0e,0},//Gamma
	{0x3341,0x1a,0},
	{0x3342,0x31,0},
	{0x3343,0x45,0},
	{0x3344,0x5a,0},
	{0x3345,0x69,0},
	{0x3346,0x75,0},
	{0x3347,0x7e,0},
	{0x3348,0x88,0},
	{0x3349,0x96,0},
	{0x334a,0xa3,0},
	{0x334b,0xaf,0},
	{0x334c,0xc4,0},
	{0x334d,0xd7,0},
	{0x334e,0xe8,0},
	{0x334f,0x20,0},
	
	{0x3350,0x32,0},//Lens correction
	{0x3351,0x25,0},
	{0x3352,0x80,0},
	{0x3353,0x1e,0},
	{0x3354,0x00,0},
	{0x3355,0x85,0},
	{0x3356,0x32,0},
	{0x3357,0x25,0},
	{0x3358,0x80,0},
	{0x3359,0x1b,0},
	{0x335a,0x00,0},
	{0x335b,0x85,0},
	{0x335c,0x32,0},
	{0x335d,0x25,0},
	{0x335e,0x80,0},
	{0x335f,0x1b,0},
	{0x3360,0x00,0},
	{0x3361,0x85,0},
	{0x3363,0x70,0},
	{0x3364,0x7f,0},
	{0x3365,0x00,0},
	{0x3366,0x00,0},
	
	{0x3301,0xff,0},//UVadjust	
	{0x338b,0x11,0},
	{0x338c,0x10,0},
	{0x338d,0x40,0},
	
	{0x3370,0xd0,0},//Sharpness/De-noise
	{0x3371,0x00,0},
	{0x3372,0x00,0},
	{0x3373,0x40,0},
	{0x3374,0x10,0},
	{0x3375,0x10,0},
	{0x3376,0x04,0},
	{0x3377,0x00,0},
	{0x3378,0x04,0},
	{0x3379,0x80,0},
	
	{0x3069,0x84,0},//BLC
	{0x307c,0x10,0},
	{0x3087,0x02,0},
	
	{0x3300,0xfc,0},//Other functions
	{0x3302,0x11,0},
	{0x3400,0x00,0},
	{0x3606,0x20,0},
	{0x3601,0x30,0},
	{0x300e,0x34,0},
	{0x3011,0x00,0},
	{0x30f3,0x83,0},
	{0x304e,0x88,0},
	{0x3086,0x0f,0},
	{0x3086,0x00,0},
#else
    {0x3012,0x80,10},//Soft Reset;Add some delay after register reset
	
	{0x308c,0x80,0},
	{0x308d,0x0e,0},
	{0x360b,0x00,0},
	{0x30b0,0xff,0},
	{0x30b1,0xff,0},
	{0x30b2,0x24,0},
	{0x300e,0x34,0},
	{0x300f,0xa6,0},
	{0x3010,0x81,0},
	{0x3082,0x01,0},
	{0x30f4,0x01,0},
	{0x3090,0x33,0},
	{0x3091,0xc0,0},
	{0x30ac,0x42,0},
	{0x30d1,0x08,0},
	{0x30a8,0x56,0},
	{0x3015,0x03,0},
	{0x3093,0x00,0},
	{0x307e,0xe5,0},
	{0x3079,0x00,0},
	{0x30aa,0x42,0},
	{0x3017,0x40,0},
	{0x30f3,0x82,0},
	{0x306a,0x0c,0},
	{0x306d,0x00,0},
	{0x336a,0x3c,0},
	{0x3076,0x6a,0},
	{0x30d9,0x8c,0},
	{0x3016,0x82,0},
	{0x3601,0x30,0},
	{0x304e,0x88,0},
	{0x30f1,0x82,0},
	{0x306f,0x14,0},
	{0x302a,0x02,0},
	{0x302b,0x6a,0},
	{0x3012,0x10,0},
	{0x3011,0x01,0},
	
	{0x3013,0xf7,0},//AEC/AGC
	{0x301c,0x13,0},
	{0x301d,0x17,0},
	{0x3070,0x5d,0},
	{0x3072,0x4d,0},

	{0x3018,0x88,0},//AE Target 08.11.14
 	{0x3019,0x78,0},

	{0x30af,0x00,0},//D5060
	{0x3048,0x1f,0},
	{0x3049,0x4e,0},
	{0x304a,0x20,0},
	{0x304f,0x20,0},
	{0x304b,0x02,0},
	{0x304c,0x00,0},
	{0x304d,0x02,0},
	{0x304f,0x20,0},
	{0x30a3,0x10,0},
	{0x3013,0xf7,0},
	//{0x3014,0x44,0},
	{0x3014,0x84,0},//44 change to 84 for 50Hz manual 08.11.14
	{0x3071,0x00,0},
	{0x3070,0x5d,0},
	{0x3073,0x00,0},
	{0x3072,0x4d,0},
	{0x301c,0x05,0},
	{0x301d,0x06,0},
	{0x304d,0x42,0},
	{0x304a,0x40,0},
	{0x304f,0x40,0},
	{0x3095,0x07,0},
	{0x3096,0x16,0},
	{0x3097,0x1d,0},
	
	//Window Setup
	{0x3020,0x01,0},
	{0x3021,0x18,0},
	{0x3022,0x00,0},
	{0x3023,0x06,0},
	{0x3024,0x06,0},
	{0x3025,0x58,0},
	{0x3026,0x02,0},
	{0x3027,0x61,0},
	{0x3088,0x03,0},
	{0x3089,0x20,0},
	{0x308a,0x02,0},
	{0x308b,0x58,0},
	{0x3316,0x64,0},
	{0x3317,0x25,0},
	{0x3318,0x80,0},
	{0x3319,0x08,0},
	{0x331a,0x64,0},
	{0x331b,0x4b,0},
	{0x331c,0x00,0},
	{0x331d,0x38,0},
	{0x3100,0x00,0},
	
	{0x3320,0xfa,0},//AWB
	{0x3321,0x11,0},
	{0x3322,0x92,0},
	{0x3323,0x01,0},
	{0x3324,0x97,0},
	{0x3325,0x02,0},
	{0x3326,0xff,0},
	{0x3327,0x0c,0},
	{0x3328,0x10,0},
	{0x3329,0x10,0},
	{0x332a,0x58,0},
	{0x332b,0x50,0},
	{0x332c,0xbe,0},
	{0x332d,0xe1,0},
	{0x332e,0x43,0},
	{0x332f,0x36,0},
	{0x3330,0x4d,0},
	{0x3331,0x44,0},
	{0x3332,0xf8,0},
	{0x3333,0x0a,0},
	{0x3334,0xf0,0},
	{0x3335,0xf0,0},
	{0x3336,0xf0,0},
	{0x3337,0x40,0},
	{0x3338,0x40,0},
	{0x3339,0x40,0},
	{0x333a,0x00,0},
	{0x333b,0x00,0},
	
	{0x3380,0x28,0},//Color Matrix
	{0x3381,0x48,0},
	{0x3382,0x10,0},
	{0x3383,0x22,0},
	{0x3384,0xc0,0},
	{0x3385,0xe2,0},
	{0x3386,0xd2,0},
	{0x3387,0xf2,0},
	{0x3388,0x20,0},
	{0x3389,0x98,0},
	{0x338a,0x00,0},
	
	{0x3340,0x02,0},//Gamma
	{0x3341,0x0e,0},
	{0x3342,0x25,0},
	{0x3343,0x39,0},
	{0x3344,0x4e,0},
	{0x3345,0x5d,0},
	{0x3346,0x69,0},
	{0x3347,0x72,0},
	{0x3348,0x7c,0},
	{0x3349,0x8a,0},
	{0x334a,0x97,0},
	{0x334b,0xa3,0},
	{0x334c,0xb8,0},
	{0x334d,0xcb,0},
	{0x334e,0xdc,0},
	{0x334f,0x30,0},
	
	{0x3350,0x32,0},//Lens correction
	{0x3351,0x25,0},
	{0x3352,0x80,0},
	{0x3353,0x1e,0},
	{0x3354,0x00,0},
	{0x3355,0x85,0},
	{0x3356,0x32,0},
	{0x3357,0x25,0},
	{0x3358,0x80,0},
	{0x3359,0x1b,0},
	{0x335a,0x00,0},
	{0x335b,0x85,0},
	{0x335c,0x32,0},
	{0x335d,0x25,0},
	{0x335e,0x80,0},
	{0x335f,0x1b,0},
	{0x3360,0x00,0},
	{0x3361,0x85,0},
	{0x3363,0x70,0},
	{0x3364,0x7f,0},
	{0x3365,0x00,0},
	{0x3366,0x00,0},
	
	{0x3301,0xff,0},//UVadjust	
	{0x338b,0x11,0},
	{0x338c,0x10,0},
	{0x338d,0x40,0},
	
	{0x3370,0xd0,0},//Sharpness/De-noise
	{0x3371,0x00,0},
	{0x3372,0x00,0},
	{0x3373,0x40,0},
	{0x3374,0x10,0},
	{0x3375,0x10,0},
	{0x3376,0x04,0},
	{0x3377,0x00,0},
	{0x3378,0x04,0},
	{0x3379,0x80,0},
	
	{0x3069,0x84,0},//BLC
	{0x307c,0x10,0},
	{0x3087,0x02,0},
	
	{0x3300,0xfc,0},//Other functions
	{0x3302,0x11,0},
	{0x3400,0x00,0},
	{0x3606,0x20,0},
	{0x3601,0x30,0},
	{0x300e,0x34,0},
	{0x3011,0x00,0},
	{0x30f3,0x83,0},
	{0x304e,0x88,0},
	{0x3086,0x0f,0},

    
	{0x3086,0x00,200},//0
#endif
	
};
const TReg gSnrSizeRegVal_OV2650_SVGA_YUV_VideoRecorder[] =
{
	{0x3012,0x80,10},//Soft Reset;Add some delay after register reset
	
	{0x308c,0x80,0},
	{0x308d,0x0e,0},
	{0x360b,0x00,0},
	{0x30b0,0xff,0},
	{0x30b1,0xff,0},
	{0x30b2,0x24,0},
	{0x300e,0x34,0},
	{0x300f,0xa6,0},
	{0x3010,0x81,0},
	{0x3082,0x01,0},
	{0x30f4,0x01,0},
	{0x3090,0x33,0},
	{0x3091,0xc0,0},
	{0x30ac,0x42,0},
	{0x30d1,0x08,0},
	{0x30a8,0x56,0},
	{0x3015,0x03,0},
	{0x3093,0x00,0},
	{0x307e,0xe5,0},
	{0x3079,0x00,0},
	{0x30aa,0x42,0},
	{0x3017,0x40,0},
	{0x30f3,0x82,0},
	{0x306a,0x0c,0},
	{0x306d,0x00,0},
	{0x336a,0x3c,0},
	{0x3076,0x6a,0},
	{0x30d9,0x8c,0},
	{0x3016,0x82,0},
	{0x3601,0x30,0},
	{0x304e,0x88,0},
	{0x30f1,0x82,0},
	{0x306f,0x14,0},
	{0x302a,0x02,0},
	{0x302b,0x6a,0},
	{0x3012,0x10,0},
	{0x3011,0x01,0},
	
	{0x3013,0xf7,0},//AEC/AGC
	{0x301c,0x13,0},
	{0x301d,0x17,0},
	{0x3070,0x5d,0},
	{0x3072,0x4d,0},
	
    {0x3018,0x68,0},//AE Target 08.11.14
    {0x3019,0x58,0},
    {0x301a,0x74,0},
 	
	{0x30af,0x00,0},//D5060
	{0x3048,0x1f,0},
	{0x3049,0x4e,0},
	{0x304a,0x20,0},
	{0x304f,0x20,0},
	{0x304b,0x02,0},
	{0x304c,0x00,0},
	{0x304d,0x02,0},
	{0x304f,0x20,0},
	{0x30a3,0x10,0},
	{0x3013,0xf7,0},
	//{0x3014,0x44,0},
	{0x3014,0x84,0},//44 change to 84 for 50Hz manual 08.11.14
	{0x3071,0x00,0},
	{0x3070,0x5d,0},
	{0x3073,0x00,0},
	{0x3072,0x4d,0},
	{0x301c,0x05,0},
	{0x301d,0x06,0},
	{0x304d,0x42,0},
	{0x304a,0x40,0},
	{0x304f,0x40,0},
	{0x3095,0x07,0},
	{0x3096,0x16,0},
	{0x3097,0x1d,0},
	
	{0x300e,0x38,0},//Window Setup
	{0x3020,0x01,0},
	{0x3021,0x18,0},
	{0x3022,0x00,0},
	{0x3023,0x06,0},
	{0x3024,0x06,0},
	{0x3025,0x58,0},
	{0x3026,0x02,0},
	{0x3027,0x61,0},
	{0x3088,0x03,0},
	{0x3089,0x20,0},
	{0x308a,0x02,0},
	{0x308b,0x58,0},
	{0x3316,0x64,0},
	{0x3317,0x25,0},
	{0x3318,0x80,0},
	{0x3319,0x08,0},
	{0x331a,0x64,0},
	{0x331b,0x4b,0},
	{0x331c,0x00,0},
	{0x331d,0x38,0},
	{0x3100,0x00,0},
	
	{0x3320,0xfa,0},//AWB
	{0x3321,0x11,0},
	{0x3322,0x92,0},
	{0x3323,0x01,0},
	{0x3324,0x97,0},
	{0x3325,0x02,0},
	{0x3326,0xff,0},
	{0x3327,0x0c,0},
	{0x3328,0x10,0},
	{0x3329,0x10,0},
	{0x332a,0x58,0},
	{0x332b,0x50,0},
	{0x332c,0xbe,0},
	{0x332d,0xe1,0},
	{0x332e,0x43,0},
	{0x332f,0x36,0},
	{0x3330,0x4d,0},
	{0x3331,0x44,0},
	{0x3332,0xf8,0},
	{0x3333,0x0a,0},
	{0x3334,0xf0,0},
	{0x3335,0xf0,0},
	{0x3336,0xf0,0},
	{0x3337,0x40,0},
	{0x3338,0x40,0},
	{0x3339,0x40,0},
	{0x333a,0x00,0},
	{0x333b,0x00,0},
	
	{0x3380,0x28,0},//Color Matrix
	{0x3381,0x48,0},
	{0x3382,0x10,0},
	{0x3383,0x23,0},
	{0x3384,0xc0,0},
	{0x3385,0xe5,0},
	{0x3386,0xc2,0},
	{0x3387,0xb3,0},
	{0x3388,0x0e,0},
	{0x3389,0x98,0},
	{0x338a,0x01,0},
	
	{0x3340,0x0e,0},//Gamma
	{0x3341,0x1a,0},
	{0x3342,0x31,0},
	{0x3343,0x45,0},
	{0x3344,0x5a,0},
	{0x3345,0x69,0},
	{0x3346,0x75,0},
	{0x3347,0x7e,0},
	{0x3348,0x88,0},
	{0x3349,0x96,0},
	{0x334a,0xa3,0},
	{0x334b,0xaf,0},
	{0x334c,0xc4,0},
	{0x334d,0xd7,0},
	{0x334e,0xe8,0},
	{0x334f,0x20,0},
	
	{0x3350,0x32,0},//Lens correction
	{0x3351,0x25,0},
	{0x3352,0x80,0},
	{0x3353,0x1e,0},
	{0x3354,0x00,0},
	{0x3355,0x85,0},
	{0x3356,0x32,0},
	{0x3357,0x25,0},
	{0x3358,0x80,0},
	{0x3359,0x1b,0},
	{0x335a,0x00,0},
	{0x335b,0x85,0},
	{0x335c,0x32,0},
	{0x335d,0x25,0},
	{0x335e,0x80,0},
	{0x335f,0x1b,0},
	{0x3360,0x00,0},
	{0x3361,0x85,0},
	{0x3363,0x70,0},
	{0x3364,0x7f,0},
	{0x3365,0x00,0},
	{0x3366,0x00,0},
	
	{0x3301,0xff,0},//UVadjust	
	{0x338b,0x11,0},
	{0x338c,0x10,0},
	{0x338d,0x40,0},
	
	{0x3370,0xd0,0},//Sharpness/De-noise
	{0x3371,0x00,0},
	{0x3372,0x00,0},
	{0x3373,0x40,0},
	{0x3374,0x10,0},
	{0x3375,0x10,0},
	{0x3376,0x04,0},
	{0x3377,0x00,0},
	{0x3378,0x04,0},
	{0x3379,0x80,0},
	
	{0x3069,0x84,0},//BLC
	{0x307c,0x10,0},
	{0x3087,0x02,0},
	
	{0x3300,0xfc,0},//Other functions
	{0x3302,0x11,0},
	{0x3400,0x00,0},
	{0x3606,0x20,0},
	{0x3601,0x30,0},
	{0x300e,0x34,0},
	{0x3011,0x00,0},
	{0x30f3,0x83,0},
	{0x304e,0x88,0},
	{0x3086,0x0f,0},
	{0x3086,0x00,200},

	
};

const TReg gSnrSizeRegVal_OV2650_UXGA_YUV[] =
{
	{0x300e,0x34,0},
	{0x3011,0x01,0},
	{0x3012,0x00,0},
	{0x302A,0x04,0},
	{0x302B,0xd4,0},
	{0x306f,0x54,0},
	
	{0x3020,0x01,0},
	{0x3021,0x18,0},
	{0x3022,0x00,0},
	{0x3023,0x0a,0},
	{0x3024,0x06,0},
	{0x3025,0x58,0},
	{0x3026,0x04,0},
	{0x3027,0xbc,0},
	{0x3088,0x06,0},
	{0x3089,0x40,0},
	{0x308a,0x04,0},
	{0x308b,0xb0,0},
	{0x3316,0x64,0},
	{0x3317,0x25,0},
	{0x3318,0x80,0},
	{0x3319,0x2c,0},
	{0x331a,0x64,0},
	{0x331b,0x4b,0},
	{0x331c,0x00,0},
	{0x331d,0x4c,0},
	{0x3302,0x01,0},
	
};


const TSnrSizeCfg gSnrSizeCfg_OV2650_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegVal_OV2650_UXGA_YUV) / sizeof(TReg),(PTReg)&gSnrSizeRegVal_OV2650_UXGA_YUV},
		{0, NULL},
		{1600, 1200},
		30,
		0,
		1
	},
	{
		{sizeof(gSnrSizeRegVal_OV2650_SVGA_YUV) / sizeof(TReg),(PTReg)&gSnrSizeRegVal_OV2650_SVGA_YUV},
		{0, NULL},
		{800, 600},
		30,
		0,
		1
	}
};

const TSnrSizeCfg gSnrSizeCfg_OV2650_YUV_VideoRecorder[] = 
{
	{
		{sizeof(gSnrSizeRegVal_OV2650_SVGA_YUV_VideoRecorder) / sizeof(TReg), (PTReg)&gSnrSizeRegVal_OV2650_SVGA_YUV_VideoRecorder},
		{0, NULL},
		{800, 600},
		30,				// maxet
		0,  //50M mclk  31fps
		0   //50M pclk
	}
	
};

const TSnrInfo gSensorInfo_OV2650_YUV = 
{
	"OV2650 YUV",

	1,	//yuv mode
	1,	//modify by zhoujian  1	//not use PCLK of sensor
	0,																										
	0,	//0 -- I2C, 1 -- Serial bus;
	1,	//[0]: 1 -- OV I2C bus
	100,//80,modify by zhoujian		//I2C Rate : 100KHz
	0x30,				//0xba for 5xx sensor, 0x48 for 3xx sensor 		0x48,//	//I2C address
	0x0,				//ISP I2C address for special sensor
	0x07,	//power config
	0x0,	//reset sensor

	32,//2,																										//brightness 
	32,//2,																										//contrast
	32,//2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		//0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
		0x00,0x04,0x16,0x30,0x4E,0x68,0x81,0x98,0xAC,0xBE,0xCD,0xDA,0xE4,0xED,0xF5,0xfB,0xff
   	},


	{sizeof(gSifPwrOnSeqRegVal_OV2650_YUV) / sizeof(TReg),(PTReg)&gSifPwrOnSeqRegVal_OV2650_YUV},		
	{0,NULL},																								
	{0, NULL},	
	{{sizeof(gSifRegVal_OV2650_YUV) /sizeof(TReg),(PTReg)&gSifRegVal_OV2650_YUV}},						
	{0,NULL},

	{0,NULL},//{sizeof(gSnrSizeRegValInit_MT9M112) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M112},
	{sizeof(gSnrSizeCfg_OV2650_YUV) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_OV2650_YUV},		
	{sizeof(gSnrSizeCfg_OV2650_YUV_VideoRecorder) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_OV2650_YUV_VideoRecorder},	//size configure 

	SensorSetReg_OV2650_YUV,																				
	SensorGetReg_OV2650_YUV,																				
	SensorIdChk_OV2650_YUV,	
	SensorSetMirrorFlip_OV2650_YUV,
	NULL,
	SensorSetContrast_OV2650_YUV,
	SensorSetBrightness_OV2650_YUV,
	SensorSetEnvironment_OV2650_YUV,  //add by xinggang.xu 20090106
	SensorSetBanding_OV2650_YUV,      //add by xinggang.xu 20090106
	SensorGetET_OV2650_YUV,																				
	SensorSetET_OV2650_YUV,																			

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};
#endif // V5_DEF_SNR_OV2650_YUV

// 3M
#if V5_DEF_SNR_OV3630_BAYER

void SensorSetReg_OV3630_BAYER(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteByte(uAddr, (UINT8)uVal,1);
}

void SensorGetReg_OV3630_BAYER(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadByte(uAddr, (UINT8*)uVal,1);
}

UINT8 SensorIdChk_OV3630_BAYER(void)
{
	UINT8 val1 = 0,val2=0;

	VIM_SIF_I2cReadByte(0x0a, &val1,1);
	VIM_SIF_I2cReadByte(0x0b, &val2,1);
	if((val1==0x36)&&(val2==0x32))
		return SUCCEED;
	return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_OV3630_BAYER(UINT8 val)
{   
    UINT8 uTemp;
    VIM_SIF_I2cReadByte(0x04, &uTemp,1);
    val=val&0x03;
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else    
    if (val==0|val==2)
        VIM_HIF_SetReg8(V5_REG_ISP_CTRL,0xa1);
    else 
        VIM_HIF_SetReg8(V5_REG_ISP_CTRL,0x91);
#endif
    VIM_SIF_I2cWriteByte(0x04,(UINT8)((uTemp&0x3f)|(val<<6)),1);
}

/********************************************************************************
Description:	Write sensor register to change exposure time
Parameter: t -- exposure time clock num 
freqtype -- frequence type 50 or 60
*********************************************************************************/
void SensorSetExposeTime_OV3630_BAYER(UINT8 freqtype, UINT32 t, TSize size, UINT32 snrclock)
{
	UINT16 uTemp = 0, vBlank = 0;
	UINT8 uVal1,uVal2;
	TI2cBatch AeData[3];
		
	switch(size.cx)
	{
	case 2048:
		if(t/2320<1567)
		{	
			uTemp = (UINT16)(t/2320);
			vBlank = 0;
		}
		else
		{
			uTemp = 1567;
			vBlank = (UINT16)(t/2320-1567);
		}
		break;
	case 1024:
		if(t/1152<789)
		{	
			uTemp = (UINT16)(t/1152);
			vBlank = 0;
		}
		else
		{
			uTemp = 789;
			vBlank = (UINT16)(t/1152-789);
		}
		break;
	default:
		break;
	}
	
	if(uTemp<=1)  uTemp=1;
	VIM_SIF_I2cWriteByte(0x2d, (UINT8)vBlank,1);
	VIM_SIF_I2cWriteByte(0x2e, (UINT8)(vBlank>>8),1);

	VIM_SIF_I2cReadByte(0x04, &uVal1,1);
	VIM_SIF_I2cReadByte(0x45, &uVal2,1);
		
	AeData[0].RegBytes  = 1;
	AeData[0].RegAddr   = 0x04;	
	AeData[0].RegVal[0] = (UINT8)((uTemp&0x03)|(uVal1&0xfc));		//low 2 bits

	AeData[1].RegBytes  = 1;
	AeData[1].RegAddr   = 0x10;
	AeData[1].RegVal[0]= (UINT8)(uTemp>>2);					//middle 8 bits

	AeData[2].RegBytes  = 1;
	AeData[2].RegAddr   = 0x45;
	AeData[2].RegVal[0] = (UINT8)(((uTemp>>10)&0x3f)|(uVal2&0xc0));		//high 6 bits

	VIM_SIF_I2cAeBatch(3, AeData,1); 
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
                        VIM_USER_PrintDec("t=\n",t);
#endif	
}

void SensorGetET_OV3630_BAYER(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
    UINT8 pclkrate;
#else
    UINT8 i, pclkrate;
#endif    
    UINT8 uTemp1,uTemp2,uTemp3,vBlank1,vBlank2;
    UINT8 uTemp4,uTemp5;
    UINT16 uTemp,vBlank;

    
    VIM_SIF_I2cReadByte(0x04,&uTemp1,1);
    VIM_SIF_I2cReadByte(0x10,&uTemp2,1);
    VIM_SIF_I2cReadByte(0x45,&uTemp3,1);
    
    VIM_SIF_I2cReadByte(0x2d,&vBlank1,1);
    VIM_SIF_I2cReadByte(0x2e,&vBlank2,1);

    switch (index)
        {
        case 0:
/*            uTemp=(UINT16)(((uTemp3&0x3f)<<10)|(uTemp2<<2)|(uTemp1&0x03));
            vBlank=(UINT16)((vBlank2<<8)|vBlank1);
            if (uTemp<1567)
                *t=uTemp*2320;
            else
                *t=(vBlank+1567)*2320;
            
            *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
            for (i=0;i<3;i++)
                AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);
 */           break;
        case 1:
            
            pclkrate = (UINT8)VIM_HIF_GetReg32(V5_REG_SIF_PIXRATE_SEL);
            uTemp=(UINT16)(((uTemp3&0x3f)<<10)|(uTemp2<<2)|(uTemp1&0x03));
            vBlank=(UINT16)((vBlank2<<8)|vBlank1);
            if (uTemp<789)
                    *t=uTemp*1152;
                else
                    *t=(vBlank+789)*1152;
 #if ((BOARD_TYPE==SVBOARD)||(BOARD_TYPE==NEWDEMO))
                (*t)=(*t)*2;		// 当是1:1 关系的时候
  #endif
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else  
            *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
            for (i=0;i<3;i++)
                AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);
#endif                
            break;
        }
    
    VIM_SIF_I2cReadByte(0x2d, &uTemp1,1);
    VIM_SIF_I2cReadByte(0x2e, &uTemp2,1);
    
    VIM_SIF_I2cReadByte(0x04, &uTemp3,1);
    VIM_SIF_I2cReadByte(0x10, &uTemp4,1);
    VIM_SIF_I2cReadByte(0x45, &uTemp5,1);
    
            
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
    VIM_USER_PrintDec("*Ggain=\n",*Ggain);
    VIM_USER_PrintDec("*t=\n",*t);

    VIM_USER_PrintDec("uTemp1=\n",uTemp1);
    VIM_USER_PrintDec("uTemp2=\n",uTemp2);
    VIM_USER_PrintDec("uTemp3=\n",uTemp3);
    VIM_USER_PrintDec("uTemp4=\n",uTemp4);
    VIM_USER_PrintDec("uTemp5=\n",uTemp5);
    
#endif	
}
    
void SensorSetET_OV3630_BAYER(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
    UINT8 uVal1,uVal2;
#else
    UINT8 i, uVal1,uVal2;
#endif    
    UINT16 uTemp, vBlank;
    TI2cBatch AeData[3];
    UINT8 uTemp1,uTemp2,uTemp3,uTemp4,uTemp5;
    switch (index)
        {
        case 0:
		if(t/2320<1567)
		{	
			uTemp = (UINT16)(t/2320);
			vBlank = 0;
		}
		else
		{
			uTemp = 1567;
			vBlank = (UINT16)(t/2320-1567);
		}
		break;
	case 1:
/*		if(t/1152<789)
		{	
			uTemp = (UINT16)(t/1152);
			vBlank = 0;
		}
		else
		{
			uTemp = 789;
			vBlank = (UINT16)(t/1152-789);
		}
*/		break;
	default:
		break;
        }   
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
                VIM_USER_PrintDec("t=\n",t);
                VIM_USER_PrintDec("Ggain=\n",Ggain);
#endif	

	if(uTemp<=1)  uTemp=1;
	VIM_SIF_I2cWriteByte(0x2d, (UINT8)vBlank,1);
	VIM_SIF_I2cWriteByte(0x2e, (UINT8)(vBlank>>8),1);

	VIM_SIF_I2cReadByte(0x04, &uVal1,1);
	VIM_SIF_I2cReadByte(0x45, &uVal2,1);
		
	AeData[0].RegBytes  = 1;
	AeData[0].RegAddr   = 0x04;	
	AeData[0].RegVal[0] = (UINT8)((uTemp&0x03)|(uVal1&0xfc));		//low 2 bits

	AeData[1].RegBytes  = 1;
	AeData[1].RegAddr   = 0x10;
	AeData[1].RegVal[0]= (UINT8)(uTemp>>2);					        //middle 8 bits

	AeData[2].RegBytes  = 1;
	AeData[2].RegAddr   = 0x45;
	AeData[2].RegVal[0] = (UINT8)(((uTemp>>10)&0x3f)|(uVal2&0xc0));	//high 6 bits

	VIM_SIF_I2cAeBatch(3, AeData,1); 
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else        
        VIM_HIF_SetReg8(V5_REG_ISP_GLBGAIN,Ggain);    
        for (i=0;i<3;i++)
                VIM_HIF_SetReg8(V5_REG_ISP_RGBGAIN+i, AWB[i]);
        
        Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN); 
#endif
        VIM_SIF_I2cReadByte(0x2d, &uTemp1,1);
        VIM_SIF_I2cReadByte(0x2e, &uTemp2,1);

        VIM_SIF_I2cReadByte(0x04, &uTemp3,1);
        VIM_SIF_I2cReadByte(0x10, &uTemp4,1);
        VIM_SIF_I2cReadByte(0x45, &uTemp5,1);
        
#if((VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG))
        VIM_USER_PrintDec("Ggain=\n",Ggain);
        VIM_USER_PrintDec("uTemp1=\n",uTemp1);
        VIM_USER_PrintDec("uTemp2=\n",uTemp2);
        VIM_USER_PrintDec("uTemp3=\n",uTemp3);
        VIM_USER_PrintDec("uTemp4=\n",uTemp4);
        VIM_USER_PrintDec("uTemp5=\n",uTemp5);

#endif	
}

const TReg gSifPwrOnSeqRegVal_OV3630_BAYER[] =
{
	{0x00, 0x21,	1},
	{0x00, 0x20,	1},
	{0x00, 0x21,	1}
};

const TReg gSifRegVal_OV3630_BAYER[] =
{
	{0x0c, 0x0a, 0},
	{0x10, 0x04, 0},
	{0x248, 0x00, 0},
};

const TReg gIspRegVal_OV3630_BAYER[] =
{
	{0x00, 0x91, 0},
       {0x01, 0x7d, 0},
       {0x14, 0x08, 0},     // 0.5x
       {0x15, 0x0c, 0},
	{0x40, 0x40, 0},	//digital gain
	{0x41, 0x40, 0},  
	{0x42, 0x40, 0},
	{0x48, 0x60, 0},	//global gain
	{0x4c, 0x20, 0},
 	
 	{0x58, 0x4f, 0},  	//color matrix
	{0x59, 0xf8, 0},
	{0x5a, 0xf9, 0},
	{0x5c, 0xf8, 0},  
	{0x5d, 0x5f, 0},
	{0x5e, 0xe9, 0},
	{0x60, 0x00, 0},  
	{0x61, 0xe0, 0},
	{0x62, 0x60, 0},
 	
 	{0xd0, 0x00, 0},
 	{0xd1, 0x49, 0},	//noise remove & awb en
 	{0xd8, 0xff, 0},	//update
 	
	{0xe4, 0x10, 0},	//awb
	{0xe8, 0x18, 0},  
	{0xec, 0x14, 0},
	{0xf0, 0xd0, 0},

       {0x11c, 0x8, 0},
	{0x19c, 0x01, 0},	//saturation en

};

const TReg gVispRegValXGA_OV3630_BAYER[] =
{
	 {0x600a0100, 0x2c0, 0},	//AE start/stop
	 {0x600a0108, 0x300, 0},
};

const TReg gVispRegValQXGA_OV3630_BAYER[] =
{
	 {0x600a0100, 0x5c0, 0},	//AE start/stop
	 {0x600a0108, 0x600, 0},

};

const TReg gSnrSizeRegValXGA_OV3630_BAYER[] = 
{
	{0x12, 0x80, 1},  
	{0x3C, 0xB7, 1},  
	{0x3C, 0x37, 1},  
	{0x0E, 0x19, 1},  
	{0x14, 0x98, 1},  
	{0x33, 0x78, 1},  
	{0x34, 0x20, 1},  
	{0x37, 0x81, 1},  
	{0x41, 0x40, 1},  
	{0x2C, 0x08, 1},  
	{0x4d, 0x51, 1},  
	{0x11, 0x40, 1},  
	{0x0D, 0x26, 1},  
	{0x44, 0x00, 1},  
	{0x39, 0x02, 1},  
	{0x4A, 0x0A, 1},  
	{0x4C, 0x00, 1},  
	{0x03, 0x0F, 1},  
	{0x42, 0x03, 1},  
	{0x3E, 0x00, 1},  
	{0x48, 0x80, 1},  
	{0x21, 0x90, 1},  
	{0x5C, 0x40, 1},  
	{0x60, 0x00, 1},  
	{0x1E, 0x00, 1},  
	{0x36, 0x12, 1},  
	{0x51, 0x00, 1},  
	{0x52, 0x40, 1},  
	{0x59, 0x00, 1},  
	{0x7E, 0x08, 1},  
	{0x53, 0x10, 1},  
	{0x59, 0x01, 1},  
	{0x7E, 0x20, 1},  
	{0x59, 0x02, 1},  
	{0x7E, 0x03, 1},  
	{0x54, 0x08, 1},  
	{0x55, 0x85, 1},  
	{0x58, 0xC1, 1},  
	{0x56, 0x28, 1},  
	{0x36, 0x52, 1},  
	{0x20, 0x80, 1},  
	{0x28, 0x30, 1},  
	{0x6c, 0x00, 1},  
	{0x6D, 0x80, 1},  
	{0x6E, 0x00, 1},  
	{0x68, 0x00, 1},  
	{0x69, 0x00, 1},  
	{0x6A, 0x00, 1},  
	{0x6B, 0x00, 1},  
	{0x70, 0x02, 1},  
	{0x71, 0x96, 1},  
	{0x73, 0xC1, 1},  
	{0x12, 0x40, 1},  
	{0x17, 0x11, 1},  
	{0x18, 0x01, 1},  
	{0x19, 0x00, 1},  
	{0x1A, 0x60, 1},  
	{0x32, 0xa4, 1},  
	{0x15, 0x04, 1},  
	{0x43, 0x00, 1},  
	{0x38, 0xA4, 1},  
	{0x11, 0x40, 1},    //use mclk
	{0x3d, 0x31, 1},    //  25M mclk,25M pclk
	
//	{0x3d, 0x30, 10},    //  25M mclk, 44M pclk output data is wrong
	{0x13, 0xc0, 10},  
	{0x01, 0x92, 10},  
	{0x02, 0xb8, 10},  
        {0x04, 0xe1, 1},
        {0x10, 0xc5, 1},
	 {0x45, 0x00, 10},  
       {0x2d, 0xda, 1},
	 {0x2e, 0x02, 10}, 
    
};

const TReg gSnrSizeRegValQXGA_OV3630_BAYER[] = 
{	
	{0x12, 0x80, 1},
	{0x3C, 0xB7, 1},
	{0x3C, 0x37, 1},
	{0x0E, 0x19, 1},
	{0x14, 0x98, 1},
	{0x33, 0x78, 1},
	{0x34, 0x20, 1},
	{0x37, 0x81, 1},
	{0x41, 0x40, 1},
	{0x2C, 0x08, 1},
	{0x4d, 0x51, 1},
	{0x11, 0x40, 1},
	{0x0D, 0x26, 1},
	{0x44, 0x00, 1},
	{0x39, 0x02, 1},
	{0x4A, 0x0A, 1},
	{0x4C, 0x00, 1},
	{0x03, 0x0F, 1},
	{0x42, 0x03, 1},
	{0x3E, 0x00, 1},
	{0x48, 0x80, 1},
	{0x21, 0x90, 1},
	{0x5C, 0x40, 1},
	{0x60, 0x00, 1},
	{0x1E, 0x00, 1},
	{0x36, 0x12, 1},
	{0x51, 0x00, 1},
	{0x52, 0x40, 1},
	{0x59, 0x00, 1},
	{0x7E, 0x08, 1},
	{0x53, 0x10, 1},
	{0x59, 0x01, 1},
	{0x7E, 0x20, 1},
	{0x59, 0x02, 1},
	{0x7E, 0x03, 1},
	{0x54, 0x08, 1},
	{0x55, 0x85, 1},
	{0x58, 0xC1, 1},
	{0x56, 0x28, 1},
	{0x36, 0x52, 1},
	{0x20, 0x80, 1},
	{0x28, 0x30, 1},
	{0x6c, 0x00, 1},
	{0x6D, 0x80, 1},
	{0x6E, 0x00, 1},
	{0x68, 0x00, 1},
	{0x69, 0x00, 1},
	{0x6A, 0x00, 1},
	{0x6B, 0x00, 1},
	{0x70, 0x02, 1},
	{0x71, 0x96, 1},
	{0x73, 0xC1, 1},
	{0x12, 0x00, 1},
	{0x17, 0x11, 1},
	{0x18, 0x00, 1},
	{0x19, 0x01, 1},
	{0x1A, 0xC1, 1},
	{0x32, 0x24, 1},
	{0x38, 0xA0, 1},
//  if (sifclock==50M)
//	{0x11, 0x40, 1},    // use mclk, 25M mclk,50M pclk
//  else(sifclock==25M)
	{0x11, 0x00, 1},    //25M mclk,25M pclk
	{0x3d, 0x31, 1},    
	{0x13, 0xc0, 1},
	{0x01, 0x92, 1},
	{0x02, 0xb8, 1},
	{0x04, 0xe2, 1},
	{0x10, 0xbc, 1},
	{0x45, 0x00, 1},
       {0x2d, 0x00, 1},
       {0x2e, 0x00, 10},
       /////////
       //{0x2b, 0xff, 10}, 
//       {0xf, 0x43, 1},     //night mode
};

const TSnrSizeCfg gSnrSizeCfg_OV3630_BAYER[] = 
{

	{
		{sizeof(gSnrSizeRegValQXGA_OV3630_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValQXGA_OV3630_BAYER},
		{sizeof(gVispRegValQXGA_OV3630_BAYER) / sizeof(TReg), (PTReg)&gVispRegValQXGA_OV3630_BAYER},
		{2048, 1536},
		30,				// maxet

//      else(sifclock==25M)        
 #if ((BOARD_TYPE==SVBOARD)||(BOARD_TYPE==NEWDEMO))
         	1,  // 25M mclk  
              3   // 50M pclk  max 12fps
 #else
 //      if (sifclofk==50M)
              1,  // 25M mclk
              0,  // 25M mclk
 #endif
	},
	
	{
		{sizeof(gSnrSizeRegValXGA_OV3630_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValXGA_OV3630_BAYER},
		{sizeof(gVispRegValXGA_OV3630_BAYER) / sizeof(TReg), (PTReg)&gVispRegValXGA_OV3630_BAYER},
		{1024,768},
		30,				// maxet
//      if (sifclofk==50M)
 #if ((BOARD_TYPE==SVBOARD)||(BOARD_TYPE==NEWDEMO))

                1,  // 25M mclk
		  3   // 25M pclk  max 27fps
#else
//      else(sifclock==25M)       
                0,    //25M
                0     //25M
#endif
	},
	
};

const TSnrInfo gSensorInfo_OV3630_BAYER = 
{
	"OV3630 Bayer",

	0,													//bayer mode
	1,													//use PCLK of sensor
	0,													//[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
	0,													//0 -- I2C, 1 -- Serial bus;
	1,													//[0]: 1 -- OV I2C bus
	100,													//I2C Rate : 100KHz
	0x60,													//I2C address
	0,													//ISP I2C address for special sensor
	0x21,													//power config
	0,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0x60,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
	//	0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
	
     //         0x00,0x13,0x38,0x59,0x79,0x92,0xa7,0xb9,0xc8,0xd4,0xdf,0xe7,0xee,0xf4,0xf9,0xfc,0xff //gamma table
              0x00,0xb,0x22,0x43,0x63,0x81,0x98,0xac,0xbd,0xcb,0xd5,0xdd,0xe5,0xed,0xf3,0xf9,0xff
   	},
	
	{sizeof(gSifPwrOnSeqRegVal_OV3630_BAYER) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_OV3630_BAYER},	//Sif config sequence(Reg.800 bit0~2) when sensor power on
	{0,NULL},												//sensor standby																					
	{0,NULL},												//Sif config sequence(Reg.800 bit0~2) when sensor standby
													
	{{sizeof(gSifRegVal_OV3630_BAYER) / sizeof(TReg), (PTReg)&gSifRegVal_OV3630_BAYER}},			//sif initial value
	{sizeof(gIspRegVal_OV3630_BAYER) / sizeof(TReg), (PTReg)&gIspRegVal_OV3630_BAYER},			//isp initial value
	{0, NULL},												//snr initial 
	{sizeof(gSnrSizeCfg_OV3630_BAYER) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_OV3630_BAYER},	//size configure 
	{0, NULL},
	
	SensorSetReg_OV3630_BAYER,										//set reg callback            																			//set reg callback
	SensorGetReg_OV3630_BAYER,										//get reg callback            									//get reg callback
	SensorIdChk_OV3630_BAYER,										//Sensor ID Check Call        									//Sensor ID Check Call
	SensorSetMirrorFlip_OV3630_BAYER,									//Set Sensor Mirror Flip Call 						//Set Sensor Mirror Flip Call
	SensorSetExposeTime_OV3630_BAYER,									//set et callback             						//set et callback
	NULL,													//adjust contrast											
	NULL,													//adjust brightness
	NULL,													//adjust work mode
        SensorGetET_OV3630_BAYER,      //get ae/awb para       
        SensorSetET_OV3630_BAYER,       //set ae/awb para       
	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						//should be add to other sensor configure
	},
	{//	ythd of flash,et of flash
		0x20,			10
	},
	1													//Histogram ratio number, 1 is recommend value
};

#endif




#if V5_DEF_SNR_MT9T012_BAYER

void SensorSetReg_MT9T012_BAYER(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord((UINT16)uAddr, (UINT16)uVal,2);
}

void SensorGetReg_MT9T012_BAYER(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord((UINT16)uAddr, (UINT16*)uVal,2);
}

UINT8 SensorIdChk_MT9T012_BAYER(void)
{
	UINT16 val = 0;

	VIM_SIF_I2cReadWord(0x0000, &val,2);
	
	
        if(val == 0x1600)   
		return SUCCEED;	
	return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9T012_BAYER(UINT8 val)
{
}

/******************************************************************************
**
Description:	Write sensor register to change exposure time
Parameter: t -- exposure time clock num 
freqtype -- frequence type 50 or 60
*******************************************************************************
**/
void SensorSetExposeTime_MT9T012_BAYER(UINT8 freqtype, UINT32 t, TSize size, 
UINT32 snrclock)
{

	UINT16 uTemp = 0, FrameL = 0;
	TI2cBatch AeData;

	switch(size.cx)
	{	
	
/*	case 2048:
		if(((t - 516)/2732) > 1551)
		{	
			uTemp = (UINT16)((t -516)/2732);
			FrameL = (UINT16)((t - 516)/2732 + 1);

		}
		else
		{
			uTemp = (UINT16)((t -516)/2732);
			FrameL = 1552;
		}
		break;
	case 1024:
		if(((t - 516)/2152) > 1551)
		{	
			uTemp = (UINT16)((t - 516)/2152);
			FrameL = (UINT16)((t - 516)/2152 + 1);

		}
		else
		{
			uTemp = (UINT16)((t - 516)/2152);	
			FrameL = 1552;
		}
		break;
*/	case 2048:
		if(((t-516) /2732) > 1551)
		{	
			uTemp = (UINT16)((t-516) /2732);
			FrameL = (UINT16)((t-516) /2732 + 1);

		}
		else
		{
			uTemp = (UINT16)((t-516) /2732);
			FrameL = 1552;
		}
		break;
        case 1024:
		if(((t-516) /1708) > 784)
		{	
			uTemp = (UINT16)((t-516)/1708);
			FrameL = (UINT16)((t-516)/1708 + 1);

		}
		else
		{
			uTemp = (UINT16)((t-516) /1708);
			FrameL = 784;
		}
		break;
        default:
		break;
	}

	if(uTemp<=1)  uTemp=1;
        VIM_SIF_I2cWriteWord(0x0104,1,2);
        VIM_SIF_I2cWriteWord(0x0340,(UINT16)FrameL,2);  
        //has to use Reg 0x340,not 
        // Reg 0x300a
//	VIM_SIF_I2cWriteWord(0x3012,(UINT16)uTemp,2);


	AeData.RegBytes  = 2;
	AeData.RegAddr   = 0x3012;
	AeData.RegVal[0] = (UINT8)(uTemp>>8);
	AeData.RegVal[1] = (UINT8)(uTemp);

        VIM_SIF_I2cAeBatch(1, &AeData,2); 
        VIM_SIF_I2cWriteWord(0x0104,0,2);
}

void SensorGetET_MT9T012_BAYER(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else
    UINT8 i;
#endif    
    UINT16 uTemp;
    switch (index)
        {
        case 0:
            VIM_SIF_I2cReadWord(0x3012,&uTemp,2);
           * t = uTemp*2732+516;
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else           
            *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
            for (i=0;i<3;i++)
                AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);
#endif                
            break;
        case 1:
            VIM_SIF_I2cReadWord(0x3012,&uTemp,2);
            *t = uTemp*1708+516;
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else            
            *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
            for (i=0;i<3;i++)
                AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);
#endif                
            break;
        }
}
    
void SensorSetET_MT9T012_BAYER(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else
    UINT8 i;
#endif    
    UINT16 uTemp, FrameL;
    TI2cBatch AeData;

    switch (index)
        {
        case 0:
            if(((t-516) /2732) > 1551)
		{	
			uTemp = (UINT16)((t-516) /2732);
			FrameL = (UINT16)((t-516) /2732 + 1);

		}
		else
		{
			uTemp = (UINT16)((t-516) /2732);
			FrameL = 1552;
		} 
            break;
        case 1:
            if(((t-516) /1708) > 784)
		{	
			uTemp = (UINT16)((t-516)/1708);
			FrameL = (UINT16)((t-516)/1708 + 1);

		}
		else
		{
			uTemp = (UINT16)((t-516) /1708);
			FrameL = 784;
		}
            break;
        }   
        if(uTemp<=1)  uTemp=1;
        VIM_SIF_I2cWriteWord(0x0104,1,2);
        VIM_SIF_I2cWriteWord(0x0340,(UINT16)FrameL,2); 

	AeData.RegBytes  = 2;
	AeData.RegAddr   = 0x3012;
	AeData.RegVal[0] = (UINT8)(uTemp>>8);
	AeData.RegVal[1] = (UINT8)(uTemp);

        VIM_SIF_I2cAeBatch(1, &AeData,2); 
        VIM_SIF_I2cWriteWord(0x0104,0,2);
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else
        
        VIM_HIF_SetReg8(V5_REG_ISP_GLBGAIN,Ggain);    
        for (i=0;i<3;i++)
                VIM_HIF_SetReg8(V5_REG_ISP_RGBGAIN+i, AWB[i]);
#endif                
}
    
const TReg gSifPwrOnSeqRegVal_MT9T012_BAYER[] =
{
	{0x00, 0x21, 1},
	{0x00, 0x20, 100},
	{0x00, 0x21, 10}
};

const TReg gSifRegVal_MT9T012_BAYER[] =
{
	{0x0C, 0x0a, 0},
	{0x10, 0x04, 0},
	{0x04, 0x41, 0},
};

const TReg gIspRegVal_MT9T012_BAYER[] =
{
        
        {0x00, 0x11, 0},
        {0x01, 0x7d, 0},
    
	{0x40, 0x40, 0},	//digital gain R
	{0x41, 0x40, 0},    //B
	{0x42, 0x40, 0},    //G
	{0x48, 0x60, 0},	//global gain
	{0x4c, 0x20, 0},

	{0x58, 0x5f, 0}, 	//color matrix
	{0x59, 0xe1, 0},
	{0x5a, 0x00, 0},
	{0x5c, 0xfc, 0},
	{0x5d, 0x60, 0},
	{0x5e, 0xe4, 0},
	{0x60, 0x08, 0},
	{0x61, 0xd8, 0},
	{0x62, 0x60, 0},
	
	{0xd0, 0x00, 0},
 	{0xd1, 0x49, 0},	//noise remove & awb en
 	{0xd8, 0xff, 0},	//update
 	
	{0xe4, 0x10, 0},	//awb
	{0xe8, 0x18, 0},  
	{0xec, 0x14, 0},
	{0xf0, 0xd0, 0},

	{0x19c, 0x01, 0},	//saturation en
};

const TReg gVispRegValXGA_MT9T012_BAYER[] =
{
	 {0x600a0100, 0x2c0, 0},	//AE start/stop
	 {0x600a0108, 0x300, 0},

        {0x60060014, 0x1810, 0},
        {0x600600d0, 0x0900, 0},
        {0x6006011c, 0x10, 0},
        {0x60060120, 0x1010, 0},
        {0x60060122, 0x1010, 0},
        {0x60060124, 0x1010, 0},
        {0x60060126, 0x1010, 0},
        {0x60060128, 0x1010, 0},
        {0x6006012a, 0x1010, 0},
        {0x6006012c, 0x1010, 0},
        {0x6006012e, 0x1010, 0},
        {0x60060130, 0x10, 0},
};

const TReg gVispRegValQXGA_MT9T012_BAYER[] =
{
	 {0x600a0100, 0x5c0, 0},	//AE start/stop
	 {0x600a0108, 0x600, 0},

        {0x60060014, 0x3020, 0},
        {0x600600d0, 0x4900, 0},
        {0x6006011c, 0x10, 0},
        {0x60060120, 0x0404, 0},
        {0x60060122, 0x0404, 0},
        {0x60060124, 0x0604, 0},
        {0x60060126, 0x0606, 0},
        {0x60060128, 0x0806, 0},
        {0x6006012a, 0x0a08, 0},
        {0x6006012c, 0x0a0a, 0},
        {0x6006012e, 0x080a, 0},
        {0x60060130, 0x04, 0},

};

const TReg gSensorInit_MT9T012_BAYER[] = 
{	
/*{0x0304, 0x0002, 1}, //PLL    4x same as default
    {0x0306, 0x0050, 1},
    {0x0308, 0x000a, 1},
    {0x030a, 0x0001, 1},
    */
    
    {0x301a, 0x00cc, 100},  
    {0x0104, 0x0001, 0}, 	        // GROUPED_PARAMETER_HOLD
/*      //from Lilea
    {0x3600, 0xD168, 0},	// LENS_CORRECTION_CONTROL
    {0x3602, 0x1E2E, 0},	// ZONE_BOUNDS_X1_X2
    {0x3604, 0x4E10, 0},	// ZONE_BOUNDS_X0_X3
    {0x3606, 0x705E, 0},	// ZONE_BOUNDS_X4_X5
    {0x3608, 0x1C2A, 0},	// ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x420E, 0},	// ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x564C, 0},	// ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x383C, 0},	// CENTER_OFFSET
    {0x3610, 0x0198, 0},	// FX_RED
    {0x3618, 0x019B, 0},	// FY_RED
    {0x3620, 0x08E4, 0},	// DF_DX_RED
    {0x3628, 0x095E, 0},	// DF_DY_RED
    {0x3632, 0x2645, 0},	// SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x4B3B, 0},	// SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x3C3C, 0},	// SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x3739, 0},	// SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x3F3E, 0},	// SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x2838, 0},	// SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x3563, 0},	// SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x2FFE, 0},	// SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0400, 0},	// K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0420, 0},	// K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x01B0, 0},	// FX_GREEN
    {0x361A, 0x016A, 0},	// FY_GREEN
    {0x3622, 0x0A00, 0},	// DF_DX_GREEN
    {0x362A, 0x0BDE, 0},	// DF_DY_GREEN
    {0x3634, 0xE21E, 0},	// SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x302D, 0},	// SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x3E38, 0},	// SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x3943, 0},	// SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x3741, 0},	// SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x2F3D, 0},	// SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x2642, 0},	// SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x28CA, 0},	// SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0008, 0},	// K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0410, 0},	// K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0440, 0},	// K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x015F, 0},	// FX_BLUE
    {0x361E, 0x0137, 0},	// FY_BLUE
    {0x3626, 0x0A6D, 0},	// DF_DX_BLUE
    {0x362E, 0x0BF3, 0},	// DF_DY_BLUE
    {0x3638, 0xFC29, 0},	// SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x2D35, 0},	// SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x2D2C, 0},	// SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x3232, 0},	// SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x2A31, 0},	// SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x1E28, 0},	// SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0x2641, 0},	// SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x2CD6, 0},	// SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0408, 0},	// K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0418, 0},	// K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x019A, 0},	// FX_GREEN2
    {0x361C, 0x0154, 0},	// FY_GREEN2
    {0x3624, 0x0A13, 0},	// DF_DX_GREEN2
    {0x362C, 0x0BB1, 0},	// DF_DY_GREEN2
    {0x3636, 0xF222, 0},	// SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x352C, 0},	// SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x333A, 0},	// SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x3C40, 0},	// SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x3A45, 0},	// SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x2B3F, 0},	// SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x2541, 0},	// SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x2CEA, 0},	// SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0438, 0},	// K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 0},	// X2_FACTORS
    {0x3674, 0x0020, 0},	// GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0028, 0},	// CORNER_FACTOR_CONTROL
    {0x318A, 0x8000, 0},	        // LENS_CORRECTION

    //from ywq
 //[Lens Correction 05/14/07 16:03:30]
    {0x3600, 0x5120, 1}, //LENS_CORRECTION_CONTROL
    {0x3602, 0x2030, 1}, //ZONE_BOUNDS_X1_X2
    {0x3604, 0x5010, 1}, //ZONE_BOUNDS_X0_X3
    {0x3606, 0x7060, 1}, //ZONE_BOUNDS_X4_X5
    {0x3608, 0x1622, 1}, //ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x3A0C, 1}, //ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x5446, 1}, //ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x2C40, 1}, //CENTER_OFFSET
    {0x3610, 0x01D8, 1}, //FX_RED
    {0x3618, 0x00F0, 1}, //FY_RED
    {0x3620, 0x0BE2, 1}, //DF_DX_RED
    {0x3628, 0x0D7E, 1}, //DF_DY_RED
    {0x3632, 0x182E, 1}, //SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x1C1F, 1}, //SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x1F18, 1}, //SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x2024, 1}, //SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x2419, 1}, //SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x2018, 1}, //SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x291B, 1}, //SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x1CEA, 1}, //SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0410, 1}, //K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0418, 1}, //K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x024F, 1}, //FX_GREEN
    {0x361A, 0x00D7, 1}, //FY_GREEN
    {0x3622, 0x0BA7, 1}, //DF_DX_GREEN
    {0x362A, 0x0DFC, 1}, //DF_DY_GREEN
    {0x3634, 0x0B1B, 1}, //SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x142A, 1}, //SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x1F1C, 1}, //SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x2229, 1}, //SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x231B, 1}, //SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x231F, 1}, //SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x2315, 1}, //SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x1BEE, 1}, //SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0448, 1}, //K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0030, 1}, //K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x01DF, 1}, //FX_BLUE
    {0x361E, 0x00CB, 1}, //FY_BLUE
    {0x3626, 0x0C7B, 1}, //DF_DX_BLUE
    {0x362E, 0x0DFD, 1}, //DF_DY_BLUE
    {0x3638, 0x1417, 1}, //SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x0F20, 1}, //SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x1917, 1}, //SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x1F22, 1}, //SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x1E17, 1}, //SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x1C15, 1}, //SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0x1F15, 1}, //SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x0FDE, 1}, //SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0438, 1}, //K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0418, 1}, //K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0038, 1}, //K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0018, 1}, //K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x01E4, 1}, //FX_GREEN2
    {0x361C, 0x00CF, 1}, //FY_GREEN2
    {0x3624, 0x0C70, 1}, //DF_DX_GREEN2
    {0x362C, 0x0E39, 1}, //DF_DY_GREEN2
    {0x3636, 0x021A, 1}, //SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x141D, 1}, //SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x1E16, 1}, //SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x2325, 1}, //SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x261C, 1}, //SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x251C, 1}, //SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x2A12, 1}, //SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x2AF2, 1}, //SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0410, 1}, //K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0438, 1}, //K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0028, 1}, //K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 1}, //X2_FACTORS
    {0x3674, 0x0020, 1}, //GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0020, 1}, //CORNER_FACTOR_CONTROL
    //STATE=Lens Correction Falloff, 90
    //STATE=Lens Correction Center X, 1032
    //STATE=Lens Correction Center Y, 704
    //BITFIELD=
    {0x318A, 0x8000, 1}, //ENABLE LENS CORRECTION


    //[Lens Correction 05/14/07 17:03:42]
    {0x3600, 0x5168, 1}, //LENS_CORRECTION_CONTROL
    {0x3602, 0x2030, 1}, //ZONE_BOUNDS_X1_X2
    {0x3604, 0x5010, 1}, //ZONE_BOUNDS_X0_X3
    {0x3606, 0x7060, 1}, //ZONE_BOUNDS_X4_X5
    {0x3608, 0x1824, 1}, //ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x3C0C, 1}, //ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x5448, 1}, //ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x3240, 1}, //CENTER_OFFSET
    {0x3610, 0x01C9, 1}, //FX_RED
    {0x3618, 0x011A, 1}, //FY_RED
    {0x3620, 0x0842, 1}, //DF_DX_RED
    {0x3628, 0x09A6, 1}, //DF_DY_RED
    {0x3632, 0x5C51, 1}, //SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x3A3C, 1}, //SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x3C2B, 1}, //SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x3A4E, 1}, //SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x483D, 1}, //SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x4C2F, 1}, //SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x4C47, 1}, //SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x3A00, 1}, //SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x022C, 1}, //FX_GREEN
    {0x361A, 0x00FC, 1}, //FY_GREEN
    {0x3622, 0x0832, 1}, //DF_DX_GREEN
    {0x362A, 0x0A97, 1}, //DF_DY_GREEN
    {0x3634, 0x4526, 1}, //SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x2F4E, 1}, //SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x3D31, 1}, //SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x3856, 1}, //SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x423E, 1}, //SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x4E39, 1}, //SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x483A, 1}, //SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x2306, 1}, //SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0000, 1}, //K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0430, 1}, //K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0400, 1}, //K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0038, 1}, //K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x01B5, 1}, //FX_BLUE
    {0x361E, 0x00E6, 1}, //FY_BLUE
    {0x3626, 0x097B, 1}, //DF_DX_BLUE
    {0x362E, 0x0AD8, 1}, //DF_DY_BLUE
    {0x3638, 0x512E, 1}, //SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x283C, 1}, //SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x2E23, 1}, //SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x2E48, 1}, //SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x3A33, 1}, //SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x3E22, 1}, //SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0x403A, 1}, //SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x19E5, 1}, //SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0420, 1}, //K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0410, 1}, //K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x01D4, 1}, //FX_GREEN2
    {0x361C, 0x00F0, 1}, //FY_GREEN2
    {0x3624, 0x096C, 1}, //DF_DX_GREEN2
    {0x362C, 0x0B16, 1}, //DF_DY_GREEN2
    {0x3636, 0x3228, 1}, //SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x3036, 1}, //SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x3E29, 1}, //SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x374F, 1}, //SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x4942, 1}, //SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x5135, 1}, //SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x5434, 1}, //SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x4123, 1}, //SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0428, 1}, //K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0400, 1}, //K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0030, 1}, //K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 1}, //X2_FACTORS
    {0x3674, 0x0020, 1}, //GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0020, 1}, //CORNER_FACTOR_CONTROL
    //STATE=Lens Correction Falloff, 90
    //STATE=Lens Correction Center X, 1032
    //STATE=Lens Correction Center Y, 788
    //BITFIELD=
    {0x318A, 0x8000, 1}, //ENABLE LENS CORRECTION    


    
    //[Lens Correction 05/16/07 11:09:12]
    {0x3600, 0x5168, 1}, //LENS_CORRECTION_CONTROL
    {0x3602, 0x1E2C, 1}, //ZONE_BOUNDS_X1_X2
    {0x3604, 0x4C0E, 1}, //ZONE_BOUNDS_X0_X3
    {0x3606, 0x6E5E, 1}, //ZONE_BOUNDS_X4_X5
    {0x3608, 0x1C2A, 1}, //ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x420E, 1}, //ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x564C, 1}, //ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x383C, 1}, //CENTER_OFFSET
    {0x3610, 0x00DD, 1}, //FX_RED
    {0x3618, 0x016C, 1}, //FY_RED
    {0x3620, 0x0C56, 1}, //DF_DX_RED
    {0x3628, 0x08B0, 1}, //DF_DY_RED
    {0x3632, 0x681E, 1}, //SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x3521, 1}, //SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x311A, 1}, //SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x3731, 1}, //SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x3330, 1}, //SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x362F, 1}, //SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x1D46, 1}, //SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x3A28, 1}, //SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0008, 1}, //K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0480, 1}, //K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0008, 1}, //K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x0142, 1}, //FX_GREEN
    {0x361A, 0x014F, 1}, //FY_GREEN
    {0x3622, 0x0C06, 1}, //DF_DX_GREEN
    {0x362A, 0x0A8A, 1}, //DF_DY_GREEN
    {0x3634, 0x2D06, 1}, //SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x231A, 1}, //SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x3E26, 1}, //SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x363E, 1}, //SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x3B38, 1}, //SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x373B, 1}, //SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x1629, 1}, //SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x3A26, 1}, //SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0018, 1}, //K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0418, 1}, //K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0478, 1}, //K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0028, 1}, //K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x00F2, 1}, //FX_BLUE
    {0x361E, 0x0111, 1}, //FY_BLUE
    {0x3626, 0x0C03, 1}, //DF_DX_BLUE
    {0x362E, 0x0B17, 1}, //DF_DY_BLUE
    {0x3638, 0x3D23, 1}, //SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x1B24, 1}, //SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x2B18, 1}, //SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x282D, 1}, //SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x2327, 1}, //SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x3626, 1}, //SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0xFA27, 1}, //SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x3514, 1}, //SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0400, 1}, //K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0448, 1}, //K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0400, 1}, //K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x011E, 1}, //FX_GREEN2
    {0x361C, 0x0140, 1}, //FY_GREEN2
    {0x3624, 0x0C1F, 1}, //DF_DX_GREEN2
    {0x362C, 0x0AA2, 1}, //DF_DY_GREEN2
    {0x3636, 0x3013, 1}, //SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x2516, 1}, //SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x3725, 1}, //SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x3637, 1}, //SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x3D3A, 1}, //SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x363B, 1}, //SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x1D29, 1}, //SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x4721, 1}, //SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0018, 1}, //K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0418, 1}, //K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0480, 1}, //K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0018, 1}, //K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 1}, //X2_FACTORS
    {0x3674, 0x0020, 1}, //GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0028, 1}, //CORNER_FACTOR_CONTROL
    //STATE=Lens Correction Falloff, 85
    //STATE=Lens Correction Center X, 956
    //STATE=Lens Correction Center Y, 884
    //BITFIELD=
    {0x318A, 0x8000, 1}, //ENABLE LENS CORRECTION
    */

    
    
    //[Register Log 05/26/07 11:19:36]
    {0x3600, 0x5168, 1},    // LENS_CORRECTION_CONTROL
    {0x3602, 0x1E2C, 1},    // ZONE_BOUNDS_X1_X2
    {0x3604, 0x4C0E, 1},    // ZONE_BOUNDS_X0_X3
    {0x3606, 0x6E5E, 1},    // ZONE_BOUNDS_X4_X5
    {0x3608, 0x1C2A, 1},    // ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x420E, 1},    // ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x564C, 1},    // ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x383C, 1},    // CENTER_OFFSET
    {0x3610, 0x00DD, 1},    // FX_RED
    {0x3618, 0x016C, 1},    // FY_RED
    {0x3620, 0x0C56, 1},    // DF_DX_RED
    {0x3628, 0x08B0, 1},    // DF_DY_RED
    {0x3632, 0x681E, 1},    // SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x3521, 1},    // SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x311A, 1},    // SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x3731, 1},    // SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x3330, 1},    // SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x362F, 1},    // SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x1D46, 1},    // SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x3A28, 1},    // SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0010, 1},    // K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0008, 1},    // K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0480, 1},    // K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0008, 1},    // K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x0142, 1},    // FX_GREEN
    {0x361A, 0x014F, 1},    // FY_GREEN
    {0x3622, 0x0C06, 1},    // DF_DX_GREEN
    {0x362A, 0x0A8A, 1},    // DF_DY_GREEN
    {0x3634, 0x2D06, 1},    // SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x231A, 1},    // SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x3E26, 1},    // SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x363E, 1},    // SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x3B38, 1},    // SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x373B, 1},    // SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x1629, 1},    // SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x3A26, 1},    // SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0018, 1},    // K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0418, 1},    // K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0478, 1},    // K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0028, 1},    // K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x00F2, 1},    // FX_BLUE
    {0x361E, 0x0111, 1},    // FY_BLUE
    {0x3626, 0x0C03, 1},    // DF_DX_BLUE
    {0x362E, 0x0B17, 1},    // DF_DY_BLUE
    {0x3638, 0x3D23, 1},    // SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x1B24, 1},    // SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x2B18, 1},    // SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x282D, 1},    // SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x2327, 1},    // SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x3626, 1},    // SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0xFA27, 1},    // SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x3514, 1},    // SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0010, 1},    // K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0400, 1},    // K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0448, 1},    // K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0400, 1},    // K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x011E, 1},    // FX_GREEN2
    {0x361C, 0x0140, 1},    // FY_GREEN2
    {0x3624, 0x0C1F, 1},    // DF_DX_GREEN2
    {0x362C, 0x0AA2, 1},    // DF_DY_GREEN2
    {0x3636, 0x3013, 1},    // SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x2516, 1},    // SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x3725, 1},    // SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x3637, 1},    // SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x3D3A, 1},    // SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x363B, 1},    // SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x1D29, 1},    // SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x4721, 1},    // SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0018, 1},    // K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0418, 1},    // K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0480, 1},    // K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0018, 1},    // K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 1},    // X2_FACTORS
    {0x3674, 0x0020, 1},    // GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0028, 1},    // CORNER_FACTOR_CONTROL
    {0x318A, 0x8000, 1},    // LENS_CORRECTION


    {0x3056, 0x0110, 0},        //gain
    {0x3058, 0x0114, 0},
    {0x305a, 0x011c, 0},
    {0x305c, 0x0110, 0},

    {0x0104, 0x0000, 0},         // GROUPED_PARAMETER_HOLD
   
};

const TReg gSnrSizeRegValXGA_MT9T012_BAYER[] = 
{
    {0x301a, 0x80cc, 10},     
    {0x3016, 0x2111, 1}, 
    {0x3006, 0x0601, 1},    // Y_ADDR_END
    {0x3008, 0x0801, 0},    // X_ADDR_END
    {0x0340, 0x0310, 0},    // frame_length_lines
    {0x0342, 0x06ac, 0},    // line_length_pck
    {0x3014, 0x0204, 0},    // fine_integration_time
    {0x3040, 0x006c, 0},    //read_mode subsampling, binning mode doesn't 
    // work in 50M pclk, but can work in 25M pclk???and no frame rate higher
    {0x301a, 0x00cc, 1}, 
};

const TReg gSnrSizeRegValQXGA_MT9T012_BAYER[] = 
{	
    {0x301a, 0x80cc, 10},     
    {0x3016, 0x2111, 1},    // pixel divider
    {0x3006, 0x0603, 0},    // Y_ADDR_END
    {0x3008, 0x0803, 0},    // X_ADDR_END
    {0x300a, 0x0610, 0},    // frame_length_lines
    {0x300c, 0x0aac, 0},    // line_length_pck
    {0x3014, 0x0204, 0},    // fine_integration_time
    {0x3040, 0x0024, 0},    // qxga_mode
    {0x301a, 0x00cc, 1}, 
};

const TSnrSizeCfg gSnrSizeCfg_MT9T012_BAYER[] = 
{
	{
		{sizeof(gSnrSizeRegValQXGA_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValQXGA_MT9T012_BAYER},
		{sizeof(gVispRegValQXGA_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gVispRegValQXGA_MT9T012_BAYER},
		{2048, 1536},
		30,				// maxet
		////4//,				// has to use sensor pll
		4,
		0
	},
	{
		{sizeof(gSnrSizeRegValXGA_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValXGA_MT9T012_BAYER},
		{sizeof(gVispRegValXGA_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gVispRegValXGA_MT9T012_BAYER},
		{1024, 768},
		30,				// maxet
		////3///,
		3,
		0
	}
};

const TSnrInfo gSensorInfo_MT9T012_BAYER = 
{
	"MT9T012 Bayer",

	0,													//bayer mode
	1,													//use PCLK of sensor
	0,													
	0,													//0 -- I2C, 1 -- Serial bus;
	0,													//[0]: 1 -- OV I2C bus
	80,													//I2C Rate : 100KHz
	0x20,												//I2C address
	0,													//ISP I2C address for special sensor
	0x01,												//power config
	0,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0x60,													//saturation
	{
	//	0x00,0x06,0x1c,0x38,0x57,0x71,0x88,0x9f,0xb3,0xc3,0xd1,0xdd,0xe6,0xee,0xf6,0xfb,0xff
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
	//0x00,0x04,0x15,0x30,0x55,0x71,0x88,0x9f,0xb3,0xc3,0xd1,0xdd,0xe6,0xee,0xf6,0xfb,0xff//new
	//0x00,0x04,0x16,0x30,0x4E,0x68,0x81,0x98,0xAC,0xBE,0xCD,0xDA,0xE4,0xED,0xF5,0xfB,0xff
	//	0x00,0x02,0x0f,0x25,0x41,0x5b,0x74,0x8c,0xa2,0xb6,0xc6,0xd3,0xde,0xe5,0xee,0xf6,0xff 
    //new
            0x00,0x02,0x12,0x2a,0x48,0x5e,0x76,0x8c,0xa2,0xb6,0xc6,0xd3,0xde,0xe8,0xf1,0xf8,0xff    
		
	//	0x00,0x00,0x09,0x19,0x32,0x4a,0x64,0x7d,0x95,0xaa,0xbd,0xcd,0xdb,0xe7,0xf1,0xf9,0xff 	

	},
	{sizeof(gSifPwrOnSeqRegVal_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_MT9T012_BAYER},	
	{0, NULL},												//sensor standby
	{0, NULL},	    										//Sif config sequence(Reg.800 bit0~2) 
	{{sizeof(gSifRegVal_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gSifRegVal_MT9T012_BAYER}},			//sif initial value
	{sizeof(gIspRegVal_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gIspRegVal_MT9T012_BAYER},			//isp initial value
	{sizeof(gSensorInit_MT9T012_BAYER) / sizeof(TReg), (PTReg)&gSensorInit_MT9T012_BAYER},			//sensor initial configure
	{sizeof(gSnrSizeCfg_MT9T012_BAYER) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_MT9T012_BAYER},	//size configure 
	{0, NULL},
	
	SensorSetReg_MT9T012_BAYER,										//set reg callback
	SensorGetReg_MT9T012_BAYER,										//get reg callback
	SensorIdChk_MT9T012_BAYER,										//Sensor ID Check Call
	SensorSetMirrorFlip_MT9T012_BAYER,									//Set Sensor Mirror Flip Call
	SensorSetExposeTime_MT9T012_BAYER,									//set et callback
	NULL,													        //adjust contrast	
	NULL,                                      //adjust brightness     
       NULL,                                      //adjust work mode      
	SensorGetET_MT9T012_BAYER,      //get ae/awb para       
	SensorSetET_MT9T012_BAYER,       //set ae/awb para       
	
	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x88, 0xa, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{//	ythd of flash,et of flash
		0x20,			10
	},
	1										//Histogram ratio number, 1 is recommend value
};

#endif







////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////1.3M

#if V5_DEF_SNR_MT9M111_YUV

void SensorSetReg_MT9M111_YUV(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord(uAddr, uVal,1);
#if(VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_SIF_I2cReadWord(uAddr, &uVal,1);

	VIM_USER_PrintHex("uAddr=  ",uAddr);
	VIM_USER_PrintHex("uVal=  ",uVal);
#endif
}

void SensorGetReg_MT9M111_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord(uAddr, (UINT16*)uVal,1);
}

UINT8 SensorIdChk_MT9M111_YUV(void)
{
	UINT16 val;

	VIM_SIF_I2cWriteWord(0xf0, 0,1);

	VIM_SIF_I2cReadWord(0, &val,1);
#if(VIM_SENSOR_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("SensorIdChk_MT9M111=", val);
#endif

	
	if(val == 0x143a)
		return SUCCEED;
	else
		return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9M111_YUV(UINT8 val)
{
	UINT16 x;
	
	VIM_SIF_I2cWriteWord(0xf0, 0,1);
	VIM_SIF_I2cReadWord(0x20, &x,1);
	
	VIM_SIF_I2cWriteWord(0x20, (UINT16)((x&0xfffc)|(val&0x03)),1);
}

/******************************************************************

         Desc: set sensor Brightness  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetBrightness_MT9M111_YUV(UINT8 val)
{
	VIM_SIF_I2cWriteWord(0xf0, 0x02,1);
	switch(val)
	{
	case 1:
		VIM_SIF_I2cWriteWord(0x2e, 0x0c1e,1);
		break;	
	case 2:
		VIM_SIF_I2cWriteWord(0x2e, 0x0c2c,1);
		break;
	case 3:
		VIM_SIF_I2cWriteWord(0x2e, 0x0c3a,1);
		break;
	case 4:
		VIM_SIF_I2cWriteWord(0x2e, 0x0c48,1);
		break;
	case 5:
		VIM_SIF_I2cWriteWord(0x2e, 0x0c56,1);
		break;
	default:
		break;		
	}
}

/******************************************************************

         Desc: set sensor contrast  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetContrast_MT9M111_YUV(UINT8 val)
{
	VIM_SIF_I2cWriteWord(0xf0, 0x01,1); 
	switch(val)
	{	
	case 1:                 
		VIM_SIF_I2cWriteWord(0x53, 0x3514,1);  //gamma0.35
		VIM_SIF_I2cWriteWord(0x54, 0x7a55,1);  
		VIM_SIF_I2cWriteWord(0x55, 0xc3a7,1);  
		VIM_SIF_I2cWriteWord(0x56, 0xe7d7,1);  
		VIM_SIF_I2cWriteWord(0x57, 0xfff4,1);  
		VIM_SIF_I2cWriteWord(0x58, 0x0000,1);  
		
		VIM_SIF_I2cWriteWord(0xdc, 0x3514,1);  
		VIM_SIF_I2cWriteWord(0xdd, 0x7a55,1);  
		VIM_SIF_I2cWriteWord(0xde, 0xc3a7,1);  
		VIM_SIF_I2cWriteWord(0xdf, 0xe7d7,1);  
		VIM_SIF_I2cWriteWord(0xe0, 0xfff4,1);  
		VIM_SIF_I2cWriteWord(0xe1, 0x0000,1);                          

		break;                    
        case 2:                                                    
		VIM_SIF_I2cWriteWord(0x53, 0x220d,1);  //gamma0.45
		VIM_SIF_I2cWriteWord(0x54, 0x5f3d,1);  
		VIM_SIF_I2cWriteWord(0x55, 0xb391,1);  
		VIM_SIF_I2cWriteWord(0x56, 0xe0cc,1);  
		VIM_SIF_I2cWriteWord(0x57, 0xfff1,1);  
		VIM_SIF_I2cWriteWord(0x58, 0x0000,1);  
		
		VIM_SIF_I2cWriteWord(0xdc, 0x220d,1);  
		VIM_SIF_I2cWriteWord(0xdd, 0x5f3d,1);  
		VIM_SIF_I2cWriteWord(0xde, 0xb391,1);  
		VIM_SIF_I2cWriteWord(0xdf, 0xe0cc,1);  
		VIM_SIF_I2cWriteWord(0xe0, 0xfff1,1);  
		VIM_SIF_I2cWriteWord(0xe1, 0x0000,1); 

		break;
	case 3:                                
		VIM_SIF_I2cWriteWord(0x53, 0x1608,1);   //gamma0.55
		VIM_SIF_I2cWriteWord(0x54, 0x4b2c,1);  
		VIM_SIF_I2cWriteWord(0x55, 0xa37c,1);  
		VIM_SIF_I2cWriteWord(0x56, 0xd9c1,1);  
		VIM_SIF_I2cWriteWord(0x57, 0xffee,1);  
		VIM_SIF_I2cWriteWord(0x58, 0x0000,1); 
		
		VIM_SIF_I2cWriteWord(0xdc, 0x1608,1);  
		VIM_SIF_I2cWriteWord(0xdd, 0x4b2c,1);  
		VIM_SIF_I2cWriteWord(0xde, 0xa37c,1);  
		VIM_SIF_I2cWriteWord(0xdf, 0xd9c1,1);  
		VIM_SIF_I2cWriteWord(0xe0, 0xffee,1);  
		VIM_SIF_I2cWriteWord(0xe1, 0x0000,1);  

		break; 
        case 4:                                
		VIM_SIF_I2cWriteWord(0x53, 0x0f05,1);  //gamma0.65
		VIM_SIF_I2cWriteWord(0x54, 0x3b20,1);  
		VIM_SIF_I2cWriteWord(0x55, 0x9469,1);  
		VIM_SIF_I2cWriteWord(0x56, 0xd3b6,1);  
		VIM_SIF_I2cWriteWord(0x57, 0xffea,1);  
		VIM_SIF_I2cWriteWord(0x58, 0x0000,1); 
		
		VIM_SIF_I2cWriteWord(0xdc, 0x0f05,1);  
		VIM_SIF_I2cWriteWord(0xdd, 0x3b20,1);  
		VIM_SIF_I2cWriteWord(0xde, 0x9469,1);  
		VIM_SIF_I2cWriteWord(0xdf, 0xd3b6,1);  
		VIM_SIF_I2cWriteWord(0xe0, 0xffea,1);  
		VIM_SIF_I2cWriteWord(0xe1, 0x0000,1);  

		break;  
	case 5:                                
		VIM_SIF_I2cWriteWord(0x53, 0x0a03,1);  //gamma0.75
		VIM_SIF_I2cWriteWord(0x54, 0x2f18,1);  
		VIM_SIF_I2cWriteWord(0x55, 0x865a,1);  
		VIM_SIF_I2cWriteWord(0x56, 0xccac,1);  
		VIM_SIF_I2cWriteWord(0x57, 0xffe7,1);  
		VIM_SIF_I2cWriteWord(0x58, 0x0000,1);  
		
		VIM_SIF_I2cWriteWord(0xdc, 0x0a03,1);  
		VIM_SIF_I2cWriteWord(0xdd, 0x2f18,1);  
		VIM_SIF_I2cWriteWord(0xde, 0x865a,1);  
		VIM_SIF_I2cWriteWord(0xdf, 0xccac,1);  
		VIM_SIF_I2cWriteWord(0xe0, 0xffe7,1);  
		VIM_SIF_I2cWriteWord(0xe1, 0x0000,1); 

		break; 
	default:
		break;
	}
}

/******************************************************************
         Desc: set sensor mod  callback 
         Para:  val: 	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetMode_MT9M111_YUV(UINT8 val)
{
	VIM_SIF_I2cWriteWord(0xf0, 2,1);
	switch (val)
	{	
		case 1:
			VIM_SIF_I2cWriteWord(0x5b, 0x0001,1);	//50Hz
	//		VIM_SIF_I2cWriteWord(0x5d, 0x8d8d);
			VIM_SIF_I2cWriteWord(0x37, 0x00a0,1);	//eg. min 20-27fps
			VIM_SIF_I2cWriteWord(0xf0, 0x0001,1);
			VIM_SIF_I2cWriteWord(0x06, 0xa40e,1);
			VIM_SIF_I2cWriteWord(0xf0, 0x0000,1);
			VIM_SIF_I2cWriteWord(0x09, 0x0190,1);
			VIM_SIF_I2cWriteWord(0xf0, 0x0001,1);
			VIM_SIF_I2cWriteWord(0x06, 0xe40e,1);
			break;
		case 2:
			VIM_SIF_I2cWriteWord(0x5b, 0x0003,1);	//60Hz
	//		VIM_SIF_I2cWriteWord(0x5c, 0x7676);
			VIM_SIF_I2cWriteWord(0x37, 0x00c0,1);	//eg. min 20-27fps
			VIM_SIF_I2cWriteWord(0xf0, 0x0001,1);
			VIM_SIF_I2cWriteWord(0x06, 0xa40e,1);
			VIM_SIF_I2cWriteWord(0xf0, 0x0000,1);
			VIM_SIF_I2cWriteWord(0x09, 0x01be,1);
			VIM_SIF_I2cWriteWord(0xf0, 0x0001,1);
			VIM_SIF_I2cWriteWord(0x06, 0xe40e,1);
			break;
		case 4:
			VIM_SIF_I2cWriteWord(0x37, 0x0100,1);	//eg. min12.5fps
			VIM_SIF_I2cWriteWord(0xf0, 0x0001,1);
			VIM_SIF_I2cWriteWord(0x06, 0xa40e,1);
			VIM_SIF_I2cWriteWord(0xf0, 0x0000,1);
			VIM_SIF_I2cWriteWord(0x09, 0x042e,1);
			VIM_SIF_I2cWriteWord(0xf0, 0x0001,1);
			VIM_SIF_I2cWriteWord(0x06, 0xe40e,1);
			break;
		case 3:
			VIM_SIF_I2cWriteWord(0x5c, 0x120d,1);	
	//		VIM_SIF_I2cWriteWord(0x5d, 0x1611);	
			break;
		default:
			break;
	}
}


const TReg gSifPwrOnSeqRegVal_MT9M111_YUV[] =
{
	{0x00, 0x63,	1},
	{0x00, 0x62,	1},
	{0x00, 0x63,	1}
};

const TReg gSifRegVal_MT9M111_YUV[] =
{
	{0x08, 0xb0,	0},
	//{0x03, 0x08,	0},
	//{0x04, 0x04,	0},
	{0x0c, 	0x0a,	0},
	{0x10, 	0x0d,	0},
};

const TReg gSnrSizeRegValInit_MT9M111_YUV[] = 
{
	{0xf0, 	0x0000,	 3},
	{0x05,  0x00d2,	 3},
	{0x06,  0x0006,	 3},
	{0x07,  0x00d2,  3}, //erase flicker 2006-12-21 17:17
	{0x08,  0x0006,  3},
	{0x09, 	0x0200,	 3},
	{0x1F,  	0x0000,	 3},
	{0x20, 	0x0303,	 0},
	{0x21, 	0x030c,	 0},
	{0x2B, 	0x0034,	 3},
	{0x2C,  	0x0038,	 3},
	{0x2D,  	0x002E,	 3},
	{0x2E,  	0x0034,	 3},
	{0x2F,  	0x0034,	 3},
	{0x34, 	0xC039,	 3},
	{0x3B, 	0x0018,  3},
	{0x61, 	0x000D,  3},
	{0x62,  	0x0026,  3},
	{0x63,  	0x0026,  3},
	{0x64,  	0x000C,  3},
	{0xF0, 	0x0002,  3},
	{0xF1,  	0x0000,  3},
	{0xF5,  	0x07FF,  3},
	{0xF6,  	0x07FF,  3},
	{0xF9,  	0x4474,  3}, 
	{0xF0, 	0x0001,  3},
	{0x05, 	0x000B,  3},
	{0x06, 	0xE40E,  3},
	{0x25, 	0x002D,  3},
	{0x34, 	0x0000,  0},//
	{0x35,  	0xff00,  0},//
	{0x4C, 	0x0001,  3},
	{0x4D, 	0x0001,  3},
 	{0x53,  0x1608, 3}, //gamma context A
 	{0x54,  0x4b2c, 3},
 	{0x55,  0xa37c, 3},
 	{0x56, 	0xd9c1, 3},
 	{0x57, 	0xffee, 3},
 	{0x58, 	0x0000, 3},
 	{0x80, 	0x0007,  3}, //lens 
 	{0x81, 	0xFF00,  3},
 	{0x82,  	0xFFFF,  3},
 	{0x83,  	0xFFFF,  3},
 	{0x84,  	0xFF00,  3},
 	{0x85, 	0xFFFF,  3},
 	{0x86,  	0xFFFF,  3},
 	{0x87,  	0xFF00,  3},
 	{0x88,  	0xFFFF,  3},
 	{0x89, 	0xFFFF,  3},
 	{0x8A, 	0xD51B,  3},
 	{0x8B, 	0xE3DF,  3},
 	{0x8C, 	0xF5EA,  3},
 	{0x8D,  	0x00FD,  3},
 	{0x8E,  	0xE012,  3},
 	{0x8F,  	0xEDEB,  3},
 	{0x90, 	 0xF8F0,  3},
 	{0x91, 	0x00FE,  3},
 	{0x92, 	0xE210,  3},
 	{0x93, 	0xF3EA,  3},
 	{0x94, 	0xF8F0,  3},
 	{0x95,  	0x00FF,  3},
 	{0xAA, 	0x01E0,  3},
 	{0xAF, 	0x0018,  3},
 	{0xB6,  	0xFFFF,  3},
 	{0xB7, 	0xFFFF,  3},
 	{0xB8, 	0xFFFF,  3},
 	{0xB9, 	0xFFFF,  3},
 	{0xBA, 	0xFFFF,  3},
 	{0xBB, 	0xFFFF,  3},
 	{0xBC, 	0x1107,  3},
 	{0xBD,  	0x1C17,  3},
 	{0xBE,  	0x0025,  3},
 	{0xBF,  	0x0C03,  3},
 	{0xC0, 	0x110F,  3},
 	{0xC1,  	0x001A,  3},
 	{0xC2,  	0x0703,  3},
 	{0xC3,  	0x0F0A,  3},
 	{0xC4, 	0x001D,  3},
 	{0xDC,  0x1608, 3}, //gamma context B
 	{0xDD,  0x4b2c, 3},
 	{0xDE,  0xa37c, 3},
 	{0xDF, 	0xd9c1, 3},
 	{0xE0, 	0xffee, 3},
 	{0xE1,  0x0000, 3},
  
 	{0xF1, 	0x0000,  3}, 
 	{0xF0, 	0x0002,  3},
 	{0x02,  	0x00AA,  3},
 	{0x03,  	0x2922,  3},
 	{0x04,  	0x04E4,  3},
 	{0x09,  	0x0093,  3},
 	{0x0A, 	0x004F,  3},
 	{0x0B, 	0x0020,  3},
 	{0x0C, 	0x0047,  3},
 	{0x0D, 	0x0086,  3},
 	{0x0E, 	0x008D,  3},
 	{0x0F, 	0x0047,  3},
 	{0x10,  	0x008F,  3},
 	{0x11,  	0x00CF,  3},
 	{0x15,  	0x0000,  3},
 	{0x16, 	0x0000,  3},
 	{0x17, 	0x0000,  3},
 	{0x18, 	0x0000,  3},
 	{0x19, 	0x0000,  3},
 	{0x1A,  	0x0000,  3},
 	{0x1B,  	0x0000,  3},
 	{0x1C,  	0x0000,  3},
 	{0x1D,  	0x0000,  3},
 	{0x1E, 	0x0000,  3},
 	{0x1F, 	0x003F,  3},
 	{0x20, 	0xD014,  3},
 	{0x21, 	0x8080,  3},
 	{0x22, 	0xD960,  3},
 	{0x23, 	0xD960,  3},
 	{0x24,  	0x6400,  3},
 	{0x26,  	0x6B22,  3},
 	{0x27,  	0x8000,  3},
 	{0x28, 	0xEF04,  3},
 	{0x29,  	0x847C,  3},
 	{0x2A,  	0x00D0,  3},
 	{0x2B,  	0x5030,  3},
 	{0x2C, 	0x7535,  3},
 	{0x2D, 	0xE0A0,  3},
 	{0x2E, 	0x0c3a,  3},	//changed 2006-12-21 16:34
 	{0x2F, 	0xDF20,  3},
 	{0x33,  	0x1411,  3},
 	{0x36,  	0x7810,  3},
 	{0x37,  	0x0240,  3},
 	{0x38,  	0x0440,  3},
 	{0x39,  	0x0618,  3},
 	{0x3A,  	0x0618,  3},
 	{0x3B,  	0x0618,  3},
 	{0x3C, 	0x0618,  3},
 	{0x3D,  	0x17D9,  3},
 	{0x3E,  	0x1CFF,  3},
 	{0x46,  	0xD900,  3},
	{0x57,  0x01d2,  3}, 	//changed 2006-12-21 17:17 max 26fps vga
	{0x58,  0x022f,  3},
	{0x59,  0x010b,  3},	//max 7fps sxga
	{0x5A,  0x0140,  3},
	{0x5B,  0x0001,  3},	//changed 2006-08-08 for 50Hz
 	{0x5C,  	0x120D,  3},
 	{0x5D,  	0x1611,  3},
 	{0x5E,  	0x4639,  3},
 	{0x5F, 	0x0000,  3},
 	{0x60, 	0x0000,  3},
 	{0x61, 	0x4639,  3},
 	{0x64, 	0x7B5B,  3},
 	{0x65, 	0x0040,  3},
 	{0x67, 	0xA010,  3},
 	{0x82,  	0x03FC,  3},
 	{0x83,  	0x0301,  3},
 	{0x84, 	0x00C1,  3},
 	{0x85, 	0x03A1,  3},
 	{0x86,  	0x03D4,  3},
 	{0x87,  	0x03D7,  3},
 	{0x88,  	0x0399,  3},
 	{0x89, 	0x03F8,  3},
 	{0x8A, 	0x001C,  3},
 	{0x8B, 	0x03BD,  3},
 	{0x8C, 	0x03DB,  3},
 	{0x8D,  	0x03BD,  3},
 	{0x8E,  	0x03FC,  3},
 	{0x8F,  	0x03DE,  3},
 	{0x90,  	0x03DE,  3},
 	{0x91, 	0x03DE,  3},
 	{0x92, 	0x03DE,  3},
 	{0x93, 	0x001F,  3},
 	{0x94, 	0x0041,  3},
 	{0x95, 	0x0363,  3},
 	{0xC8,  	0x0000,  3},
 	{0xCB,  	0x0000,  3},
 	{0xCC, 	0x0004,  3},
 	{0xCD,  	0x21A0,  3},
 	{0xCE,  	0x1E9B,  3},
 	{0xCF,  	0x4A4A,  3},
 	{0xD0, 	0x168D,  3},
 	{0xD1, 	0x004D,  3},
 	{0xD2, 	0x0000,  3},
 	{0xD3, 	0x0000,  3},
 	{0xD4,  	0x0208,  3},
 	{0xD5,  	0x0000,  3},
 	{0xEF,  	0x0008,  3},
 	{0xF1,  	0x0000,  3},
 	{0xF2, 	0x0000,  3},
 	{0xF4, 	0x006E,  3},
 	{0xF5, 	0x0087,  3},
 	{0xF6, 	0x0036,  3},
 	{0xF7, 	0x000D,  3},
 	{0xF8, 	0x00AB,  3},
 	{0xF9,  	0x0088,  3},
 	{0xFA,  	0x0048,  3},
 	{0xFB,  	0x0057,  3},
 	{0xFC, 	0x005E,  3},
 	{0xFD,  	0x007A,  3},
 	{0xFE, 	0x503F,  3},
 	{0xFF,  	0xA880,  3},
};

const TReg gSnrSizeRegValVGA_MT9M111_YUV[] = 
{
	{0xf0, 0x0000, 0},
	{0x09, 0x0200, 0},
	{0x20, 0x0303, 0},
	{0x21, 0x030c, 0},
 	{0xC8, 0x0000, 0},
 	{0xF0,  0x0002,  3},	//changed 2006-09-22 for min 15fps
	{0x37, 0x00a0, 3},	//changed 2006-12-21  for min 20fps
};

const TReg gSnrSizeRegValSXGA_MT9M111_YUV[] = 
{ 
    	{0xF0, 0x0000, 0},
    	{0x20, 0x0303, 0},	
 	{0xc8, 0x1f0b, 0},   
 	{0xF0,  0x0002,  3},	//changed 2006-11-27 with lower framerate to increase performance of low light
 	{0x37,  0x0240,  3},
};

const TSnrSizeCfg gSnrSizeCfg_MT9M111_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegValSXGA_MT9M111_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValSXGA_MT9M111_YUV},
		{0, NULL},
		{1280, 1024},
		0,
		0
	},
	{
		{sizeof(gSnrSizeRegValVGA_MT9M111_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValVGA_MT9M111_YUV},
		{0, NULL},
		{640, 480},
		0,
		0
	}
};

const TSnrInfo gSensorInfo_MT9M111_YUV = 
{
	"MT9M111 YUV",

	1,																										//yuv mode
	1,																										//not use PCLK of sensor
	0,																										//[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
	0,																										//0 -- I2C, 1 -- Serial bus;
	0,																										//[0]: 1 -- OV I2C bus
	80,																										//I2C Rate : 100KHz
	0xba,				//i2c device address if 																						//I2C address
	0xba,																									//ISP I2C address for special sensor
	0x03,																									//power config
	0,																										//reset sensor

	2,																										//brightness 
	2,																										//contrast
	2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
   	},


	{sizeof(gSifPwrOnSeqRegVal_MT9M111_YUV) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_MT9M111_YUV},		//Sif config sequence(Reg.800 bit0~2) when sensor power on
	{0,NULL},																								//sensor standby
	{0, NULL},	
	{{sizeof(gSifRegVal_MT9M111_YUV) / sizeof(TReg), (PTReg)&gSifRegVal_MT9M111_YUV}},						//sif initial value
	{0,NULL},

	{sizeof(gSnrSizeRegValInit_MT9M111_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M111_YUV},
	{sizeof(gSnrSizeCfg_MT9M111_YUV) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_MT9M111_YUV},		//size configure 
	{0,NULL},

	SensorSetReg_MT9M111_YUV,																				//set reg callback
	SensorGetReg_MT9M111_YUV,																				//get reg callback
	SensorIdChk_MT9M111_YUV,	
	SensorSetMirrorFlip_MT9M111_YUV,
	NULL,
	SensorSetContrast_MT9M111_YUV,
	SensorSetBrightness_MT9M111_YUV,
	SensorSetMode_MT9M111_YUV,																									//Set Sensor Mirror Flip Call
	NULL,//SnrGetEt_MT9M111_YUV,																				
	NULL,//SnrSetEt_MT9M111_YUV,																			

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						//should be add to other sensor configure
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};

#endif


#if V5_DEF_SNR_MT9M112_BAYER

void SensorSetReg_MT9M112_BAYER(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord((UINT8)uAddr, (UINT16)uVal,1);
}

void SensorGetReg_MT9M112_BAYER(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord((UINT8)uAddr, (UINT16*)uVal,1);
}

UINT8 SensorIdChk_MT9M112_BAYER(void)
{
	UINT16 val = 0;
	VIM_SIF_I2cWriteWord(0xf0,0,1);
	VIM_SIF_I2cReadWord(0x0000, &val,1);
	
	
        if(val == 0x148c)   
		return SUCCEED;	
	return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9M112_BAYER(UINT8 val)
{
 
}

/******************************************************************************
**
Description:	Write sensor register to change exposure time
Parameter: t -- exposure time clock num 
freqtype -- frequence type 50 or 60
*******************************************************************************
**/
void SensorSetExposeTime_MT9M112_BAYER(UINT8 freqtype, UINT32 t, TSize size, UINT32 snrclock)
{

	UINT16 uTemp = 0;
	TI2cBatch AeData;

	switch(size.cx)
	{
	case 640:
		uTemp =(UINT16)(t/928);
		break;
	case 1280:
		 uTemp =(UINT16)(t/1426);
		break;
	default:
		break;
	}

	if(uTemp<=1)  uTemp=1;
 	VIM_SIF_I2cWriteWord(0xf0,0,1);
	AeData.RegBytes  = 2;
	AeData.RegAddr   = 0x9;
	AeData.RegVal[0] = (UINT8)(uTemp>>8);
	AeData.RegVal[1] = (UINT8)(uTemp);

        VIM_SIF_I2cAeBatch(1, &AeData,1); 
  
}

void SensorGetET_MT9M112_BAYER(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
    UINT8 i;
    UINT16 uTemp;
    VIM_SIF_I2cWriteWord(0xf0,0,1);
    switch (index)
        {
        case 0:
            VIM_SIF_I2cReadWord(0x9,&uTemp,1);
           * t = uTemp*1426;
            *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
            for (i=0;i<3;i++)
                AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);
            break;
        case 1:
            VIM_SIF_I2cReadWord(0x9,&uTemp,1);
            *t = uTemp*928;
            *Ggain=VIM_HIF_GetReg8(V5_REG_ISP_GLBGAIN);
            for (i=0;i<3;i++)
                AWB[i]=VIM_HIF_GetReg8(V5_REG_ISP_RGBGAIN+i);
            break;
        }
}
    
void SensorSetET_MT9M112_BAYER(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
    UINT8 i;
    UINT16 uTemp;
    TI2cBatch AeData;

    switch(index)
	{
	case 0:
		uTemp =(UINT16)(t/1426);
		break;
	case 1:
		 uTemp =(UINT16)(t/928);
		break;
	default:
		break;
	}
        if(uTemp<=1)  uTemp=1;
        VIM_SIF_I2cWriteWord(0xf0,0,1);
   

	AeData.RegBytes  = 2;
	AeData.RegAddr   = 0x9;
	AeData.RegVal[0] = (UINT8)(uTemp>>8);
	AeData.RegVal[1] = (UINT8)(uTemp);

        VIM_SIF_I2cAeBatch(1, &AeData,1); 
 
        VIM_HIF_SetReg8(V5_REG_ISP_GLBGAIN,Ggain);    
        for (i=0;i<3;i++)
                VIM_HIF_SetReg8(V5_REG_ISP_RGBGAIN+i, AWB[i]);
}
    
const TReg gSifPwrOnSeqRegVal_MT9M112_BAYER[] =
{
	{0x00, 0x23, 1},
	{0x00, 0x22, 10},
	{0x00, 0x23, 10}
};

const TReg gSifRegVal_MT9M112_BAYER[] =
{
	{0x0C, 0x0a, 0},
	{0x10, 0x04, 0},
	{0x04, 0x41, 0},
};

const TReg gIspRegVal_MT9M112_BAYER[] =
{
        
        {0x00, 0x11, 0},
        {0x01, 0x7d, 0},
    
	{0x40, 0x40, 0},	//digital gain R
	{0x41, 0x40, 0},    //B
	{0x42, 0x40, 0},    //G
	{0x48, 0x60, 0},	//global gain
	{0x4c, 0x20, 0},

	{0x58, 0x6a, 0}, 	//color matrix
	{0x59, 0xd5, 0},
	{0x5a, 0x00, 0},
	{0x5c, 0xeb, 0},
	{0x5d, 0x6a, 0},
	{0x5e, 0xeb, 0},
	{0x60, 0x00, 0},
	{0x61, 0xdb, 0},
	{0x62, 0x6a, 0},
	
	{0xd0, 0x00, 0},
 	{0xd1, 0x49, 0},	//noise remove & awb en
 	{0xd8, 0xff, 0},	//update
 	
	{0xe4, 0x10, 0},	//awb
	{0xe8, 0x18, 0},  
	{0xec, 0x14, 0},
	{0xf0, 0xd0, 0},

	{0x19c, 0x01, 0},	//saturation en
};

const TReg gVispRegValVGA_MT9M112_BAYER[] =
{
	 {0x600a0100, 0x1d0, 0},	//AE start/stop
	 {0x600a0108, 0x1e0, 0},

        {0x60060014, 0x1810, 0},
};

const TReg gVispRegValSXGA_MT9M112_BAYER[] =
{
	 {0x600a0100, 0x3d0, 0},	//AE start/stop
	 {0x600a0108, 0x400, 0},

        {0x60060014, 0x6040, 0},
};

const TReg gSensorInit_MT9M112_BAYER[] = 
{	
/*{0x0304, 0x0002, 1}, //PLL    4x same as default
    {0x0306, 0x0050, 1},
    {0x0308, 0x000a, 1},
    {0x030a, 0x0001, 1},
    */
    
    {0x301a, 0x00cc, 100},  
    {0x0104, 0x0001, 0}, 	        // GROUPED_PARAMETER_HOLD
/*      //from Lilea
    {0x3600, 0xD168, 0},	// LENS_CORRECTION_CONTROL
    {0x3602, 0x1E2E, 0},	// ZONE_BOUNDS_X1_X2
    {0x3604, 0x4E10, 0},	// ZONE_BOUNDS_X0_X3
    {0x3606, 0x705E, 0},	// ZONE_BOUNDS_X4_X5
    {0x3608, 0x1C2A, 0},	// ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x420E, 0},	// ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x564C, 0},	// ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x383C, 0},	// CENTER_OFFSET
    {0x3610, 0x0198, 0},	// FX_RED
    {0x3618, 0x019B, 0},	// FY_RED
    {0x3620, 0x08E4, 0},	// DF_DX_RED
    {0x3628, 0x095E, 0},	// DF_DY_RED
    {0x3632, 0x2645, 0},	// SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x4B3B, 0},	// SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x3C3C, 0},	// SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x3739, 0},	// SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x3F3E, 0},	// SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x2838, 0},	// SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x3563, 0},	// SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x2FFE, 0},	// SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0400, 0},	// K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0420, 0},	// K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x01B0, 0},	// FX_GREEN
    {0x361A, 0x016A, 0},	// FY_GREEN
    {0x3622, 0x0A00, 0},	// DF_DX_GREEN
    {0x362A, 0x0BDE, 0},	// DF_DY_GREEN
    {0x3634, 0xE21E, 0},	// SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x302D, 0},	// SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x3E38, 0},	// SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x3943, 0},	// SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x3741, 0},	// SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x2F3D, 0},	// SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x2642, 0},	// SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x28CA, 0},	// SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0008, 0},	// K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0410, 0},	// K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0440, 0},	// K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x015F, 0},	// FX_BLUE
    {0x361E, 0x0137, 0},	// FY_BLUE
    {0x3626, 0x0A6D, 0},	// DF_DX_BLUE
    {0x362E, 0x0BF3, 0},	// DF_DY_BLUE
    {0x3638, 0xFC29, 0},	// SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x2D35, 0},	// SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x2D2C, 0},	// SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x3232, 0},	// SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x2A31, 0},	// SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x1E28, 0},	// SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0x2641, 0},	// SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x2CD6, 0},	// SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0408, 0},	// K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0010, 0},	// K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0418, 0},	// K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x019A, 0},	// FX_GREEN2
    {0x361C, 0x0154, 0},	// FY_GREEN2
    {0x3624, 0x0A13, 0},	// DF_DX_GREEN2
    {0x362C, 0x0BB1, 0},	// DF_DY_GREEN2
    {0x3636, 0xF222, 0},	// SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x352C, 0},	// SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x333A, 0},	// SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x3C40, 0},	// SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x3A45, 0},	// SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x2B3F, 0},	// SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x2541, 0},	// SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x2CEA, 0},	// SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0438, 0},	// K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0000, 0},	// K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 0},	// X2_FACTORS
    {0x3674, 0x0020, 0},	// GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0028, 0},	// CORNER_FACTOR_CONTROL
    {0x318A, 0x8000, 0},	        // LENS_CORRECTION

    //from ywq
 //[Lens Correction 05/14/07 16:03:30]
    {0x3600, 0x5120, 1}, //LENS_CORRECTION_CONTROL
    {0x3602, 0x2030, 1}, //ZONE_BOUNDS_X1_X2
    {0x3604, 0x5010, 1}, //ZONE_BOUNDS_X0_X3
    {0x3606, 0x7060, 1}, //ZONE_BOUNDS_X4_X5
    {0x3608, 0x1622, 1}, //ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x3A0C, 1}, //ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x5446, 1}, //ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x2C40, 1}, //CENTER_OFFSET
    {0x3610, 0x01D8, 1}, //FX_RED
    {0x3618, 0x00F0, 1}, //FY_RED
    {0x3620, 0x0BE2, 1}, //DF_DX_RED
    {0x3628, 0x0D7E, 1}, //DF_DY_RED
    {0x3632, 0x182E, 1}, //SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x1C1F, 1}, //SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x1F18, 1}, //SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x2024, 1}, //SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x2419, 1}, //SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x2018, 1}, //SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x291B, 1}, //SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x1CEA, 1}, //SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0410, 1}, //K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0418, 1}, //K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x024F, 1}, //FX_GREEN
    {0x361A, 0x00D7, 1}, //FY_GREEN
    {0x3622, 0x0BA7, 1}, //DF_DX_GREEN
    {0x362A, 0x0DFC, 1}, //DF_DY_GREEN
    {0x3634, 0x0B1B, 1}, //SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x142A, 1}, //SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x1F1C, 1}, //SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x2229, 1}, //SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x231B, 1}, //SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x231F, 1}, //SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x2315, 1}, //SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x1BEE, 1}, //SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0448, 1}, //K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0030, 1}, //K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x01DF, 1}, //FX_BLUE
    {0x361E, 0x00CB, 1}, //FY_BLUE
    {0x3626, 0x0C7B, 1}, //DF_DX_BLUE
    {0x362E, 0x0DFD, 1}, //DF_DY_BLUE
    {0x3638, 0x1417, 1}, //SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x0F20, 1}, //SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x1917, 1}, //SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x1F22, 1}, //SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x1E17, 1}, //SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x1C15, 1}, //SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0x1F15, 1}, //SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x0FDE, 1}, //SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0438, 1}, //K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0418, 1}, //K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0038, 1}, //K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0018, 1}, //K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x01E4, 1}, //FX_GREEN2
    {0x361C, 0x00CF, 1}, //FY_GREEN2
    {0x3624, 0x0C70, 1}, //DF_DX_GREEN2
    {0x362C, 0x0E39, 1}, //DF_DY_GREEN2
    {0x3636, 0x021A, 1}, //SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x141D, 1}, //SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x1E16, 1}, //SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x2325, 1}, //SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x261C, 1}, //SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x251C, 1}, //SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x2A12, 1}, //SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x2AF2, 1}, //SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0410, 1}, //K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0438, 1}, //K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0028, 1}, //K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 1}, //X2_FACTORS
    {0x3674, 0x0020, 1}, //GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0020, 1}, //CORNER_FACTOR_CONTROL
    //STATE=Lens Correction Falloff, 90
    //STATE=Lens Correction Center X, 1032
    //STATE=Lens Correction Center Y, 704
    //BITFIELD=
    {0x318A, 0x8000, 1}, //ENABLE LENS CORRECTION
*/
    //[Lens Correction 05/14/07 17:03:42]
    {0x3600, 0x5168, 1}, //LENS_CORRECTION_CONTROL
    {0x3602, 0x2030, 1}, //ZONE_BOUNDS_X1_X2
    {0x3604, 0x5010, 1}, //ZONE_BOUNDS_X0_X3
    {0x3606, 0x7060, 1}, //ZONE_BOUNDS_X4_X5
    {0x3608, 0x1824, 1}, //ZONE_BOUNDS_Y1_Y2
    {0x360A, 0x3C0C, 1}, //ZONE_BOUNDS_Y0_Y3
    {0x360C, 0x5448, 1}, //ZONE_BOUNDS_Y4_Y5
    {0x360E, 0x3240, 1}, //CENTER_OFFSET
    {0x3610, 0x01C9, 1}, //FX_RED
    {0x3618, 0x011A, 1}, //FY_RED
    {0x3620, 0x0842, 1}, //DF_DX_RED
    {0x3628, 0x09A6, 1}, //DF_DY_RED
    {0x3632, 0x5C51, 1}, //SECOND_DERIV_ZONE_0_RED
    {0x363A, 0x3A3C, 1}, //SECOND_DERIV_ZONE_1_RED
    {0x3642, 0x3C2B, 1}, //SECOND_DERIV_ZONE_2_RED
    {0x364A, 0x3A4E, 1}, //SECOND_DERIV_ZONE_3_RED
    {0x3652, 0x483D, 1}, //SECOND_DERIV_ZONE_4_RED
    {0x365A, 0x4C2F, 1}, //SECOND_DERIV_ZONE_5_RED
    {0x3662, 0x4C47, 1}, //SECOND_DERIV_ZONE_6_RED
    {0x366A, 0x3A00, 1}, //SECOND_DERIV_ZONE_7_RED
    {0x3686, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_R_TL
    {0x368E, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_R_TR
    {0x3676, 0x0010, 1}, //K_FACTOR_IN_K_FX_FY_R_BL
    {0x367E, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_R_BR
    {0x3612, 0x022C, 1}, //FX_GREEN
    {0x361A, 0x00FC, 1}, //FY_GREEN
    {0x3622, 0x0832, 1}, //DF_DX_GREEN
    {0x362A, 0x0A97, 1}, //DF_DY_GREEN
    {0x3634, 0x4526, 1}, //SECOND_DERIV_ZONE_0_GREEN
    {0x363C, 0x2F4E, 1}, //SECOND_DERIV_ZONE_1_GREEN
    {0x3644, 0x3D31, 1}, //SECOND_DERIV_ZONE_2_GREEN
    {0x364C, 0x3856, 1}, //SECOND_DERIV_ZONE_3_GREEN
    {0x3654, 0x423E, 1}, //SECOND_DERIV_ZONE_4_GREEN
    {0x365C, 0x4E39, 1}, //SECOND_DERIV_ZONE_5_GREEN
    {0x3664, 0x483A, 1}, //SECOND_DERIV_ZONE_6_GREEN
    {0x366C, 0x2306, 1}, //SECOND_DERIV_ZONE_7_GREEN
    {0x3688, 0x0000, 1}, //K_FACTOR_IN_K_FX_FY_G1_TL
    {0x3690, 0x0430, 1}, //K_FACTOR_IN_K_FX_FY_G1_TR
    {0x3678, 0x0400, 1}, //K_FACTOR_IN_K_FX_FY_G1_BL
    {0x3680, 0x0038, 1}, //K_FACTOR_IN_K_FX_FY_G1_BR
    {0x3616, 0x01B5, 1}, //FX_BLUE
    {0x361E, 0x00E6, 1}, //FY_BLUE
    {0x3626, 0x097B, 1}, //DF_DX_BLUE
    {0x362E, 0x0AD8, 1}, //DF_DY_BLUE
    {0x3638, 0x512E, 1}, //SECOND_DERIV_ZONE_0_BLUE
    {0x3640, 0x283C, 1}, //SECOND_DERIV_ZONE_1_BLUE
    {0x3648, 0x2E23, 1}, //SECOND_DERIV_ZONE_2_BLUE
    {0x3650, 0x2E48, 1}, //SECOND_DERIV_ZONE_3_BLUE
    {0x3658, 0x3A33, 1}, //SECOND_DERIV_ZONE_4_BLUE
    {0x3660, 0x3E22, 1}, //SECOND_DERIV_ZONE_5_BLUE
    {0x3668, 0x403A, 1}, //SECOND_DERIV_ZONE_6_BLUE
    {0x3670, 0x19E5, 1}, //SECOND_DERIV_ZONE_7_BLUE
    {0x368C, 0x0420, 1}, //K_FACTOR_IN_K_FX_FY_B_TL
    {0x3694, 0x0410, 1}, //K_FACTOR_IN_K_FX_FY_B_TR
    {0x367C, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_B_BL
    {0x3684, 0x0020, 1}, //K_FACTOR_IN_K_FX_FY_B_BR
    {0x3614, 0x01D4, 1}, //FX_GREEN2
    {0x361C, 0x00F0, 1}, //FY_GREEN2
    {0x3624, 0x096C, 1}, //DF_DX_GREEN2
    {0x362C, 0x0B16, 1}, //DF_DY_GREEN2
    {0x3636, 0x3228, 1}, //SECOND_DERIV_ZONE_0_GREEN2
    {0x363E, 0x3036, 1}, //SECOND_DERIV_ZONE_1_GREEN2
    {0x3646, 0x3E29, 1}, //SECOND_DERIV_ZONE_2_GREEN2
    {0x364E, 0x374F, 1}, //SECOND_DERIV_ZONE_3_GREEN2
    {0x3656, 0x4942, 1}, //SECOND_DERIV_ZONE_4_GREEN2
    {0x365E, 0x5135, 1}, //SECOND_DERIV_ZONE_5_GREEN2
    {0x3666, 0x5434, 1}, //SECOND_DERIV_ZONE_6_GREEN2
    {0x366E, 0x4123, 1}, //SECOND_DERIV_ZONE_7_GREEN2
    {0x368A, 0x0408, 1}, //K_FACTOR_IN_K_FX_FY_G2_TL
    {0x3692, 0x0428, 1}, //K_FACTOR_IN_K_FX_FY_G2_TR
    {0x367A, 0x0400, 1}, //K_FACTOR_IN_K_FX_FY_G2_BL
    {0x3682, 0x0030, 1}, //K_FACTOR_IN_K_FX_FY_G2_BR
    {0x3672, 0x0000, 1}, //X2_FACTORS
    {0x3674, 0x0020, 1}, //GLOBAL_OFFSET_FXY_FUNCTION
    {0x3696, 0x0020, 1}, //CORNER_FACTOR_CONTROL
    //STATE=Lens Correction Falloff, 90
    //STATE=Lens Correction Center X, 1032
    //STATE=Lens Correction Center Y, 788
    //BITFIELD=
    {0x318A, 0x8000, 1}, //ENABLE LENS CORRECTION    


    {0x3056, 0x0110, 0},        //gain
    {0x3058, 0x0114, 0},
    {0x305a, 0x011c, 0},
    {0x305c, 0x0110, 0},

    {0x0104, 0x0000, 0},         // GROUPED_PARAMETER_HOLD
   
};

const TReg gSnrSizeRegValVGA_MT9M112_BAYER[] = 
{
    	{0xf0, 0x0001, 1},
	{0x06, 0x0014, 1},
	{0x3a, 0x1000, 1}, //output format context A
	{0x9b, 0x1000, 1}, //output format context B
	{0xf0, 0x0000, 1},
	{0x41, 0x00d7, 1},
	{0x0c, 0x0000, 1},
	{0x05, 0x0092, 1},
	{0x06, 0x0032, 1},
	{0x07, 0x0120, 1},
	{0x08, 0x0019, 1},
	{0x24, 0x806f, 1},
	{0xc8, 0x0000, 1},
	{0x21, 0x8000, 300},
	{0x2c, 0x0021, 1},
	{0x2d, 0x002b, 1},
};

const TReg gSnrSizeRegValSXGA_MT9M112_BAYER[] = 
{	
    	{0xf0, 0x0000, 0},
	{0x20, 0x0100, 0},
	{0xc8, 0x000b, 0},
};

const TSnrSizeCfg gSnrSizeCfg_MT9M112_BAYER[] = 
{
	{
		{sizeof(gSnrSizeRegValSXGA_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValSXGA_MT9M112_BAYER},
		{sizeof(gVispRegValSXGA_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gVispRegValSXGA_MT9M112_BAYER},
		{1280, 1024},
		30,				// maxet
		0,				// has to use sensor pll
		0
	},
	{
		{sizeof(gSnrSizeRegValVGA_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gSnrSizeRegValVGA_MT9M112_BAYER},
		{sizeof(gVispRegValVGA_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gVispRegValVGA_MT9M112_BAYER},
		{640, 480},
		30,				// maxet
		0,
		0
	}
};

const TSnrInfo gSensorInfo_MT9M112_BAYER = 
{
	"MT9M112 Bayer",

	0,													//bayer mode
	1,													//use PCLK of sensor
	0,													
	0,													//0 -- I2C, 1 -- Serial bus;
	0,													//[0]: 1 -- OV I2C bus
	80,													//I2C Rate : 100KHz
	0xba,												//I2C address
	0,													//ISP I2C address for special sensor
	0x01,												//power config
	0,													//reset sensor

	2,													//brightness 
	2,													//contrast
	0x50,													//saturation
	{
	//	0x00,0x06,0x1c,0x38,0x57,0x71,0x88,0x9f,0xb3,0xc3,0xd1,0xdd,0xe6,0xee,0xf6,0xfb,0xff
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
	//0x00,0x04,0x15,0x30,0x55,0x71,0x88,0x9f,0xb3,0xc3,0xd1,0xdd,0xe6,0xee,0xf6,0xfb,0xff//new
	//0x00,0x04,0x16,0x30,0x4E,0x68,0x81,0x98,0xAC,0xBE,0xCD,0xDA,0xE4,0xED,0xF5,0xfB,0xff
	//	0x00,0x02,0x0f,0x25,0x41,0x5b,0x74,0x8c,0xa2,0xb6,0xc6,0xd3,0xde,0xe5,0xee,0xf6,0xff 
    //new
            0x00,0x02,0x12,0x2a,0x48,0x5e,0x76,0x8c,0xa2,0xb6,0xc6,0xd3,0xde,0xe8,0xf1,0xf8,0xff    
		
	//	0x00,0x00,0x09,0x19,0x32,0x4a,0x64,0x7d,0x95,0xaa,0xbd,0xcd,0xdb,0xe7,0xf1,0xf9,0xff 	

	},
	{sizeof(gSifPwrOnSeqRegVal_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gSifPwrOnSeqRegVal_MT9M112_BAYER},	
	{0, NULL},												//sensor standby
	{0, NULL},	    										//Sif config sequence(Reg.800 bit0~2) 
	{{sizeof(gSifRegVal_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gSifRegVal_MT9M112_BAYER}},			//sif initial value
	{sizeof(gIspRegVal_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gIspRegVal_MT9M112_BAYER},			//isp initial value
	{0, NULL},//{sizeof(gSensorInit_MT9M112_BAYER) / sizeof(TReg), (PTReg)&gSensorInit_MT9M112_BAYER},			//sensor initial configure
	{sizeof(gSnrSizeCfg_MT9M112_BAYER) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_MT9M112_BAYER},	//size configure 
	{0,NULL},
	
	SensorSetReg_MT9M112_BAYER,										//set reg callback
	SensorGetReg_MT9M112_BAYER,										//get reg callback
	SensorIdChk_MT9M112_BAYER,										//Sensor ID Check Call
	SensorSetMirrorFlip_MT9M112_BAYER,									//Set Sensor Mirror Flip Call
	SensorSetExposeTime_MT9M112_BAYER,									//set et callback
	NULL,													        //adjust contrast	
	NULL,                                      //adjust brightness     
       NULL,                                      //adjust work mode      
	SensorGetET_MT9M112_BAYER,      //get ae/awb para       
	SensorSetET_MT9M112_BAYER,       //set ae/awb para       
	
	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x88, 0xa, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{//	ythd of flash,et of flash
		0x20,			10
	},
	1										//Histogram ratio number, 1 is recommend value
};

#endif


#if V5_DEF_SNR_MT9M112_YUV

void SensorSetReg_MT9M112_YUV(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord((UINT8)uAddr, (UINT16)uVal,1);
}

void SensorGetReg_MT9M112_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord((UINT8)uAddr, (UINT16*)uVal,1);
}

UINT8 SensorIdChk_MT9M112_YUV(void)
{
	UINT16 val;

	VIM_SIF_I2cWriteWord(0xf0, 0,1);
	VIM_SIF_I2cReadWord(0, &val,1);
	
	if(val == 0x148c)
		return SUCCEED;
	else
		return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9M112_YUV(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor Brightness  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetBrightness_MT9M112_YUV(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor contrast  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetContrast_MT9M112_YUV(UINT8 val)
{
	
}

/******************************************************************
         Desc: set sensor mod  callback 
         Para:  val: 	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetMode_MT9M112_YUV(UINT8 val)
{
	
}

void SensorGetET_MT9M112_YUV(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
	
}

void SensorSetET_MT9M112_YUV(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
	
}

const TReg gSifPwrOnSeqRegVal_MT9M112_YUV[] =
{
	//{0x00,	0x66,	1},
	//{0x00,	0x67,	1},
	//{0x00,	0x65,	1}
	{0x00, 0x67, 1},
	{0x00, 0x66, 10},
	{0x00, 0x67, 10}
};

const TReg gSifRegVal_MT9M112_YUV[] =
{
	//{0x0C, 0x0a, 0},
	//{0x0d, 0x04, 0},
	//{0x04, 0x41, 0},
	{0x0008, 0xb00, 0},
	{0x0024, 0x0, 0},  //Href_start_len
	{0x0026, 0x027f, 0},  //Href_start_len
	{0x000c, 0x050a, 0},
	{0x0010, 0x04, 0},
	{0x0030, 0x0, 0}, //Vref_start
	{0x0032, 0x01e0, 0}, //Vref_start
	{0x0084, 0x03, 0}
};

/*const TReg gSnrSizeRegValVGA_MT9M112_YUV[] = 
{
#if 1
	{0xf0, 0x0000, 1},
	{0x02, 0x0064, 1},	//changed 2006-08-15	
	{0x05, 0x00D0, 1},
	{0x06, 0x0012, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x20, 0x0100, 1},
	{0x21, 0x8000, 1},
	{0x22, 0x0D0F, 1},
	{0x24, 0x8000, 1},
	{0x59, 0x00FF, 1},
                      
	{0xf0, 0x0001, 1},
	{0x05, 0x0009, 1},
	{0x06, 0x600e, 1},
	#if 1 	//add for another sensor 2006-10-19 16:00
	{0x3b, 0x0430, 1},	//black level 
	
	{0x53, 0x0c04, 1},	//context A gamma 
	{0x54, 0x4421, 1},
	{0x55, 0x8f6f, 1},
	{0x56, 0xc2aa, 1},
	{0x57, 0xecd8, 1},
	{0x58, 0xff00, 1},
	{0xdc, 0x0c04, 1},	//context B gamma
	{0xdd, 0x4421, 1},
	{0xde, 0x8f6f, 1},
	{0xdf, 0xc2aa, 1},
	{0xe0, 0xecd8, 1},
	{0xe1, 0xff00, 1},
	#endif 
	{0xf0, 0x0002, 1},
	{0xc8, 0x0000, 1},
	{0x2e, 0x084a, 1},
	{0x2f, 0xd100, 1},
	{0x39, 0x03CA, 1},
	{0x3A, 0x0680, 1},
	{0x3B, 0x0152, 1},
	{0x3C, 0x0540, 1},
	{0x57, 0x019C, 1},	//changed 2006-08-15	
	{0x58, 0x01EE, 1},
	{0x59, 0x010B, 1},
	{0x5A, 0x0140, 1},
	{0x5C, 0x1D17, 1},
	{0x5D, 0x221C, 1},
	{0x64, 0x1E1C, 1},
	{0x5B, 0x0001, 1},
#else
	{0xf0, 0x0002, 1},
	{0xc8, 0x00, 1},	//changed 2006-08-15	
	{0xf0, 0x000, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x21, 0x00c, 1},
	{0x20, 0x00100, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},

#endif
};*/
const TReg gSnrSizeRegValVGA_MT9M112_YUV[] =
{ 
	{0xf0,	0x02,	10},
	{0xc8,	0x00,	30},
	{0xf0,	0x00,	10},
	{0x07,	0xe0,	0},
	{0x08,	0x0b,	0},
	{0x21,	0x0c,	0},
	{0x20,	0x100,	0}
};

const TReg gSnrSizeRegValSXGA_MT9M112_YUV[] = 
{ 
    {0xf0, 0x0002, 1},
	{0xc8, 0x1f0b, 1},	//default SXGA
	{0xf0, 0x0001, 1},
	{0x05, 0x000f, 1},
	{0xf0, 0x0000, 1},
	{0x02, 0x0068, 1},
};

const TSnrSizeCfg gSnrSizeCfg_MT9M112_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegValSXGA_MT9M112_YUV) / sizeof(TReg),(PTReg)&gSnrSizeRegValSXGA_MT9M112_YUV},
		{0, NULL},
		{1280, 1024},
		2,
		2,
		3
	},
	{
		{sizeof(gSnrSizeRegValVGA_MT9M112_YUV) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9M112_YUV},
		{0, NULL},
		{640, 480},
		30,
		0,
		1
	}
};

const TSnrInfo gSensorInfo_MT9M112_YUV = 
{
	"MT9M112 YUV",

	1,																										//yuv mode
	1,	//modify by zhoujian  1																									//not use PCLK of sensor
	0,																										
	0,																										//0 -- I2C, 1 -- Serial bus;
	0,																										//[0]: 1 -- OV I2C bus
	100,//80,modify by zhoujian																										//I2C Rate : 100KHz
	0x48,				//0xba for 5xx sensor, 0x48 for 3xx sensor 		0x48,//																			//I2C address
	0,																									//ISP I2C address for special sensor
	0x07,//01,	modify by zhoujian																										//power config
	0x0,//0,	modify by zhoujian																									//reset sensor

	32,//2,																										//brightness 
	32,//2,																										//contrast
	32,//2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		//0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
		0x00,0x04,0x16,0x30,0x4E,0x68,0x81,0x98,0xAC,0xBE,0xCD,0xDA,0xE4,0xED,0xF5,0xfB,0xff
   	},


	{sizeof(gSifPwrOnSeqRegVal_MT9M112_YUV) / sizeof(TReg),(PTReg)&gSifPwrOnSeqRegVal_MT9M112_YUV},		
	{0,NULL},																								
	{0, NULL},	
	{{sizeof(gSifRegVal_MT9M112_YUV) /sizeof(TReg),(PTReg)&gSifRegVal_MT9M112_YUV}},						
	{0,NULL},

	{0,NULL},//{sizeof(gSnrSizeRegValInit_MT9M112) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M112},
	{sizeof(gSnrSizeCfg_MT9M112_YUV) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_MT9M112_YUV},		
	{0,NULL},

	SensorSetReg_MT9M112_YUV,																				
	SensorGetReg_MT9M112_YUV,																				
	SensorIdChk_MT9M112_YUV,	
	SensorSetMirrorFlip_MT9M112_YUV,
	NULL,
	SensorSetContrast_MT9M112_YUV,
	SensorSetBrightness_MT9M112_YUV,
	SensorSetMode_MT9M112_YUV,																									//Set Sensor Mirror Flip Call
	NULL,//SnrGetEt_MT9M112,																				
	NULL,//SnrSetEt_MT9M112,																			

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};

#endif

#if V5_DEF_SNR_OV9650_YUV

void SensorSetReg_OV9650_YUV(UINT32 uAddr, UINT16 uVal)
{  // iprintf("\nsend data\n");
	VIM_SIF_I2cWriteByte(uAddr, (UINT8)uVal,1);
}

void SensorGetReg_OV9650_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadByte(uAddr, (UINT8*)uVal,1);
}

UINT8 SensorIdChk_OV9650_YUV(void)
{
UINT8 val = 0;
	VIM_SIF_I2cWriteByte(0xff, 0x01,1); 
	VIM_SIF_I2cReadByte(0x0a, &val,1);


	if(val != 0x96)
		return FAILED;
	VIM_SIF_I2cReadByte(0x0b, &val,1);
	if(val == 0x52)
		{
		MMDPrintf("\n********************check id success*********\n");
		return SUCCEED;
		}
	return FAILED;

}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_OV9650_YUV(UINT8 val)
{
	UINT16 x;
	
		SensorGetReg_OV9650_YUV(0x1e, &x);
		x&=0xcf;//clear bit,bit5
		x |= (val&0x03)<<4;
		SensorSetReg_OV9650_YUV(0x1e, x);

	
}

/******************************************************************

         Desc: set sensor Brightness  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetBrightness_OV9650_YUV(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor contrast  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetContrast_OV9650_YUV(UINT8 val)
{
	
}

/******************************************************************
         Desc: set sensor mod  callback 
         Para:  val: 	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetMode_OV9650_YUV(UINT8 val)
{
	
}

void SensorGetET_OV9650_YUV(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
	
}

void SensorSetET_OV9650_YUV(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
	
}

void SensorPowerDown_OV9650(  )
{
		VIM_HIF_SetReg32((UINT32)(0x00| V5_REG_SIF_BASE), 0x66);
		VIM_USER_DelayMs(5);
}

void SensorPowerOn_OV9650(  )
{
		VIM_HIF_SetReg32((UINT32)(0x00| V5_REG_SIF_BASE), 0x62);
		VIM_USER_DelayMs(5);
}
const TReg gSifPwrOnSeqRegVal_OV9650_YUV[] =
{ 
	{0x00, 0x62,	1},
	{0x00, 0x63,	1},
	{0x00, 0x62,	1}



/*	
	{0x00, 0x67,	1},
	{0x00, 0x66,	1},
	{0x00, 0x67,	1}
*/	
};

const TReg	gSifRegVal_OV9650_YUV[] =
{
   {0x0c, 0x040a, 0},
   {0x04, 0x41, 0},

};

const TReg gSnrSizeRegValVGA_OV9650_YUV[] =
{ 
	
     {0x12, 0x80,0},
    // {0x11, 0x80,0},
     {0x11, 0x81,0}, //12.5fps
     {0x6b, 0x0a,0},
     {0x6a, 0x3e,0},//12.5fps
     //{0x6a, 0x7c,0},//fllick 30fps
     
     {0x3b, 0x09,0}, //adopt the light area
    // {0x3b, 0x11,0},
   //  {0x3b, 0xc9,0},
     {0x13, 0xe0,0},
     {0x01, 0x80,0},
     {0x02, 0x80,0},
     {0x00, 0x00,0},
     {0x10, 0x00,0},
     {0x13, 0xe5,0},
     
//     {0x39, 0x50,0},// ;50 for 30fps
     {0x39, 0x43,0},// ;43 for 12.5fps
//  {0x38, 0x92,0},// ;92 for 30fps
      {0x38, 0x12,0},// ;12 for 12.5fps
     {0x37, 0x00,0},
//     {0x35, 0x81,0},// ;81 for 30fps
     {0x35, 0x91,0},// ;91 for 12.5fps
     {0x0e, 0x20,0},
//     {0x1e, 0x04,0},
     {0x1e, 0x34,0},//mirror
     
     {0xA8, 0x80,0}, 
     {0x12, 0x40,0}, 
     {0x04, 0x00,0}, 
     {0x0c, 0x04,0}, 
     {0x0d, 0x80,0}, 
     {0x18, 0xc6,0}, 
     {0x17, 0x26,0}, 
     {0x32, 0xad,0}, 
     {0x03, 0x00,0}, 
     {0x1a, 0x3d,0}, 
     {0x19, 0x01,0}, 
     {0x3f, 0xa6,0}, //edge
    // {0x14, 0x1a,0}, 
     {0x14, 0x2a,0}, //8X gain 
  
     {0x15, 0x02,0}, 
  
     {0x41, 0x02,0},//saturity 
     {0x42, 0x08,0}, 
     
     {0x1b, 0x00,0},
     {0x16, 0x06,0},
     {0x33, 0xe2,0},// ;c0 for internal regulator
     {0x34, 0xbf,0},
     {0x96, 0x04,0},
     {0x3a, 0x0c,0},//guoyuankai, YUV order	
     {0x8e, 0x00,0},
     
     {0x3c, 0x77,0},
     {0x8B, 0x06,0},
     {0x94, 0x88,0},
     {0x95, 0x88,0},
     {0x40, 0xc1,0},
     {0x29, 0x3f,0},// ;2f for internal regulator
     {0x0f, 0x42,0},
     
     {0x3d, 0x92,0},
     {0x69, 0x40,0},
     {0x5C, 0xb9,0},
     {0x5D, 0x96,0},
     {0x5E, 0x10,0},
     {0x59, 0xc0,0},
     {0x5A, 0xaf,0},
     {0x5B, 0x55,0},
     {0x43, 0xf0,0},
     {0x44, 0x10,0},
     {0x45, 0x68,0},
     {0x46, 0x96,0},
     {0x47, 0x60,0},
     {0x48, 0x80,0},
     {0x5F, 0xe0,0},
     {0x60, 0x8c,0},// ;0c for advanced AWB (related to lens)
     {0x61, 0x20,0},
     {0xa5, 0xd9,0},
     {0xa4, 0x74,0},
     {0x8d, 0x02,0},
     {0x13, 0xe7,0},
     
     {0x4f, 0x3a,0},
     {0x50, 0x3d,0},
     {0x51, 0x03,0},
     {0x52, 0x12,0},
     {0x53, 0x26,0},
     {0x54, 0x38,0},
     {0x55, 0x40,0},
     {0x56, 0x40,0},
     {0x57, 0x40,0},
     {0x58, 0x0d,0},
     
     //{0x3f, 0x06,0}, 	//add by Amanda Deng 2008.03.18
     {0x8C, 0x23,0},	
     {0x3E, 0x02,0},
     
     {0xa9, 0xb8,0},
     {0xaa, 0x92,0},
     {0xab, 0x0a,0},
     
     {0x8f, 0xdf,0},
     {0x90, 0x00,0},
     {0x91, 0x00,0},
     {0x9f, 0x00,0},
     {0xa0, 0x00,0},
     {0x3A, 0x0d,0},
     
   //  {0x24, 0xac,0},	//modify by Amanda Deng 2008.03.18
   //  {0x25, 0x9c,0},	//modify by Amanda Deng 2008.03.18

     {0x24, 0x85,0},	//modify by guoyuankai 2008.03.25
     {0x25, 0x76,0},	//modify by guoyuankai  2008.03.25
     {0x26, 0xd3,0},
     
     {0x2a, 0x10,0},// ;10 for 50Hz
     {0x2b, 0x40,0},// ;40 for 50Hz
     
     
     {0x6c, 0x40,0},
     {0x6d, 0x30,0},
     {0x6e, 0x4b,0},
     {0x6f, 0x60,0},
     {0x70, 0x70,0},
     {0x71, 0x70,0},
     {0x72, 0x70,0},
     {0x73, 0x70,0},
     {0x74, 0x60,0},
     {0x75, 0x60,0},
     {0x76, 0x50,0},
     {0x77, 0x48,0},
     {0x78, 0x3a,0},
     {0x79, 0x2e,0},
     {0x7a, 0x28,0},
     {0x7b, 0x22,0},
     {0x7c, 0x04,0},
     {0x7d, 0x07,0},
     {0x7e, 0x10,0},
     {0x7f, 0x28,0},
     {0x80, 0x36,0},
     {0x81, 0x44,0},
     {0x82, 0x52,0},
     {0x83, 0x60,0},  
     {0x84, 0x6c,0},
     {0x85, 0x78,0},
     {0x86, 0x8c,0},
     {0x87, 0x9e,0},
     {0x88, 0xbb,0},
     {0x89, 0xd2,0},
     {0x8a, 0xe6,0},
     //{0x3a, 0x0c,3}, 
 #if VIM_SURPPORT_VGA_30FRAME
//added by youhai
#if 0
#if 1 //50hz
     {0x11,0x80,0},
     {0x6b,0x0a,0},
     {0x39,0x50,0},
     {0x38,0x92,0},
     {0x35,0x81,0},
     {0x2a,0x10,0},
     {0x2b,0x40,0},
     {0x6a,0x7d,0},
#else
     {0x11,0x80,0},
     {0x6b,0x0a,0},
     {0x39,0x50,0},
     {0x38,0x92,0},
     {0x35,0x81,0},
     {0x2a,0x00,0},
     {0x2b,0x00,0},
     {0x6a,0x7d,0},
#endif
#endif
#endif
};

const TReg gSnrSizeRegValSXGA_OV9650_YUV[] = 
{ 
//    for 30W switch to 130W initial necessery registers add by guoyuankai 
	   {0x11, 0x80,2}, 
     {0x6b, 0x0a,2},
     {0x2a, 0x10,2}, 
     {0x2b, 0x34,2}, 
     {0x3b, 0x09,2},
     {0x6a, 0x41,2},
     {0x12, 0x00,2},
     {0x04, 0x00,2},
     {0x0c, 0x00,2},
     {0x0d, 0x00,2},
     {0x18, 0xbd,2},
     {0x17, 0x1d,2},
     {0x32, 0xad,2},
     {0x03, 0x12,2},
     {0x1a, 0x81,2},
     {0x19, 0x01,2},   
};

const TSnrSizeCfg gSnrSizeCfg_OV9650_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegValSXGA_OV9650_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValSXGA_OV9650_YUV},							//snr initial value
		{0, NULL},
		{1280,1024},
		30,				// maxet
		0,
		0
	},
	{
		{sizeof(gSnrSizeRegValVGA_OV9650_YUV) / sizeof(TReg), (PTReg)&gSnrSizeRegValVGA_OV9650_YUV},							//snr initial value
		{0, NULL},
		{640, 480},
		30,				// maxet
		0,
		0
	}
};

const TSnrInfo gSensorInfo_OV9650_YUV = 
{
	"OV9650 YUV",

	1,																										//yuv mode
	1,	//modify by zhoujian  1																									//not use PCLK of sensor
	0,																										
	0,																										//0 -- I2C, 1 -- Serial bus;
	0x1,																										//[0]: 1 -- OV I2C bus
	80,//80,modify by zhoujian																										//I2C Rate : 100KHz
	0x30,//0x18,//0x48,				//0xba for 5xx sensor, 0x48 for 3xx sensor 		0x48,//																			//I2C address
	0x30,																									//ISP I2C address for special sensor
	0x00,//0x07,//01,	modify by zhoujian																										//power config
	0x0,//0x0,//0,	modify by guoyuankai 																								//reset sensor

	32,//2,																										//brightness 
	32,//2,																										//contrast
	32,//2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		//0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
		0x00,0x04,0x16,0x30,0x4E,0x68,0x81,0x98,0xAC,0xBE,0xCD,0xDA,0xE4,0xED,0xF5,0xfB,0xff
	 //   0x00,0x13,0x38,0x59,0x79,0x92,0xa7,0xb9,0xc8,0xd4,0xdf,0xe7,0xee,0xf4,0xf9,0xfc,0xff
   	},


	{sizeof(gSifPwrOnSeqRegVal_OV9650_YUV) / sizeof(TReg),(PTReg)&gSifPwrOnSeqRegVal_OV9650_YUV},		
	{0,NULL},																								
	{0, NULL},	
	{{sizeof(gSifRegVal_OV9650_YUV) /sizeof(TReg),(PTReg)&gSifRegVal_OV9650_YUV}},						
	{0,NULL},

	{0,NULL},//{sizeof(gSnrSizeRegValInit_MT9M112) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M112},
	{sizeof(gSnrSizeCfg_OV9650_YUV) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_OV9650_YUV},		
	{0,NULL},

	SensorSetReg_OV9650_YUV,																				
	SensorGetReg_OV9650_YUV,																				
	SensorIdChk_OV9650_YUV,	
	SensorSetMirrorFlip_OV9650_YUV,
	NULL,
	SensorSetContrast_OV9650_YUV,
	SensorSetBrightness_OV9650_YUV,
	SensorSetMode_OV9650_YUV,																									//Set Sensor Mirror Flip Call
	NULL,//SnrGetEt_OV9650,																				
	NULL,//SnrSetEt_OV9650,																			

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};

#endif

#if V5_DEF_SNR_MT9M112_YUV_5XX

void SensorSetReg_MT9M112_YUV(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord((UINT8)uAddr, (UINT16)uVal,1);
}

void SensorGetReg_MT9M112_YUV(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord((UINT8)uAddr, (UINT16*)uVal,1);
}

UINT8 SensorIdChk_MT9M112_YUV(void)
{
	UINT16 val;
	VIM_SIF_I2cWriteWord(0xf0, 0,1);
	VIM_SIF_I2cReadWord(0, &val,1);
	if(val == 0x148c)
		return SUCCEED;
	else
		return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9M112_YUV(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor Brightness  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetBrightness_MT9M112_YUV(UINT8 val)
{
	
}

/******************************************************************

         Desc: set sensor contrast  callback 

         Para: 1-5 step

*******************************************************************/

void SensorSetContrast_MT9M112_YUV(UINT8 val)
{
	
}

/******************************************************************
         Desc: set sensor mod  callback 
         Para:  val: 	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetMode_MT9M112_YUV(UINT8 val)
{
	
}

void SensorGetET_MT9M112_YUV(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
	
}

void SensorSetET_MT9M112_YUV(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
	
}

const TReg gSifPwrOnSeqRegVal_MT9M112_YUV[] =
{
	{0x00, 0x63, 1},
	{0x00, 0x62, 10},
	{0x00, 0x63, 10}
};

const TReg gSifRegVal_MT9M112_YUV[] =
{
	//{0x0C, 0x0a, 0},
	//{0x0d, 0x04, 0},
	{0x0c, 0x040a, 0},
	{0x04, 0x41, 0},
};
#if 1//add by zhoujian
const TReg gSnrSizeRegValVGA_MT9M112_YUV[] = 
{
#if 1
	{0xf0, 0x0000, 1},
	{0x02, 0x0064, 1},	//changed 2006-08-15	
	{0x05, 0x00D0, 1},
	{0x06, 0x0012, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x20, 0x0100, 1},
	{0x21, 0x8000, 1},
	{0x22, 0x0D0F, 1},
	{0x24, 0x8000, 1},
	{0x59, 0x00FF, 1},
                      
	{0xf0, 0x0001, 1},
	{0x05, 0x0009, 1},
	{0x06, 0x600e, 1},
	#if 0 	//add for another sensor 2006-10-19 16:00
	{0x3b, 0x0430, 1},	//black level 
	
	{0x53, 0x0c04, 1},	//context A gamma 
	{0x54, 0x4421, 1},
	{0x55, 0x8f6f, 1},
	{0x56, 0xc2aa, 1},
	{0x57, 0xecd8, 1},
	{0x58, 0xff00, 1},
	{0xdc, 0x0c04, 1},	//context B gamma
	{0xdd, 0x4421, 1},
	{0xde, 0x8f6f, 1},
	{0xdf, 0xc2aa, 1},
	{0xe0, 0xecd8, 1},
	{0xe1, 0xff00, 1},
	#endif 
	{0xf0, 0x0002, 1},
	{0xc8, 0x0000, 1},
	{0x2e, 0x084a, 1},
	{0x2f, 0xd100, 1},
	{0x39, 0x03CA, 1},
	{0x3A, 0x0680, 1},
	{0x3B, 0x0152, 1},
	{0x3C, 0x0540, 1},
	{0x57, 0x019C, 1},	//changed 2006-08-15	
	{0x58, 0x01EE, 1},
	{0x59, 0x010B, 1},
	{0x5A, 0x0140, 1},
	{0x5C, 0x1D17, 1},
	{0x5D, 0x221C, 1},
	{0x64, 0x1E1C, 1},
	{0x5B, 0x0001, 1},
#else
	{0xf0, 0x0002, 1},
	{0xc8, 0x00, 1},	//changed 2006-08-15	
	{0xf0, 0x000, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x21, 0x00c, 1},
	{0x20, 0x00100, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},

#endif
};
const TReg gSnrSizeRegValVGA_MT9M112_YUV_VideoRecorder[] = 
{
#if 1
	{0xf0, 0x0000, 1},
	{0x02, 0x0064, 1},	//changed 2006-08-15	
	{0x05, 0x00D0, 1},
	{0x06, 0x0012, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x20, 0x0100, 1},
	{0x21, 0x8000, 1},
	{0x22, 0x0D0F, 1},
	{0x24, 0x8000, 1},
	{0x59, 0x00FF, 1},
                      
	{0xf0, 0x0001, 1},
	{0x05, 0x0009, 1},
	{0x06, 0x600e, 1},
	#if 0 	//add for another sensor 2006-10-19 16:00
	{0x3b, 0x0430, 1},	//black level 
	
	{0x53, 0x0c04, 1},	//context A gamma 
	{0x54, 0x4421, 1},
	{0x55, 0x8f6f, 1},
	{0x56, 0xc2aa, 1},
	{0x57, 0xecd8, 1},
	{0x58, 0xff00, 1},
	{0xdc, 0x0c04, 1},	//context B gamma
	{0xdd, 0x4421, 1},
	{0xde, 0x8f6f, 1},
	{0xdf, 0xc2aa, 1},
	{0xe0, 0xecd8, 1},
	{0xe1, 0xff00, 1},
	#endif 
	{0xf0, 0x0002, 1},
	{0xc8, 0x0000, 1},
	{0x2e, 0x084a, 1},
	{0x2f, 0xd100, 1},
	{0x39, 0x03CA, 1},
	{0x3A, 0x0680, 1},
	{0x3B, 0x0152, 1},
	{0x3C, 0x0540, 1},
	{0x57, 0x019C, 1},	//changed 2006-08-15	
	{0x58, 0x01EE, 1},
	{0x59, 0x010B, 1},
	{0x5A, 0x0140, 1},
	{0x5C, 0x1D17, 1},
	{0x5D, 0x221C, 1},
	{0x64, 0x1E1C, 1},
	{0x5B, 0x0001, 1},
#else
	{0xf0, 0x0002, 1},
	{0xc8, 0x00, 1},	//changed 2006-08-15	
	{0xf0, 0x000, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x21, 0x00c, 1},
	{0x20, 0x00100, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},

#endif
};

#else
const TReg gSnrSizeRegValVGA_MT9M112_YUV[] = 
{
#if 1
	{0xf0, 0x0000, 1},
	{0x02, 0x0064, 1},	//changed 2006-08-15	
	{0x05, 0x00D0, 1},
	{0x06, 0x0012, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x20, 0x0100, 1},
	{0x21, 0x8000, 1},
	{0x22, 0x0D0F, 1},
	{0x24, 0x8000, 1},
	{0x59, 0x00FF, 1},
                      
	{0xf0, 0x0001, 1},
	{0x05, 0x0009, 1},
	{0x06, 0x600e, 1},
	#if 1 	//add for another sensor 2006-10-19 16:00
	{0x3b, 0x0430, 1},	//black level 
	
	{0x53, 0x0c04, 1},	//context A gamma 
	{0x54, 0x4421, 1},
	{0x55, 0x8f6f, 1},
	{0x56, 0xc2aa, 1},
	{0x57, 0xecd8, 1},
	{0x58, 0xff00, 1},
	{0xdc, 0x0c04, 1},	//context B gamma
	{0xdd, 0x4421, 1},
	{0xde, 0x8f6f, 1},
	{0xdf, 0xc2aa, 1},
	{0xe0, 0xecd8, 1},
	{0xe1, 0xff00, 1},
	#endif 
	{0xf0, 0x0002, 1},
	{0xc8, 0x0000, 1},
	{0x2e, 0x084a, 1},
	{0x2f, 0xd100, 1},
	{0x39, 0x03CA, 1},
	{0x3A, 0x0680, 1},
	{0x3B, 0x0152, 1},
	{0x3C, 0x0540, 1},
	{0x57, 0x019C, 1},	//changed 2006-08-15	
	{0x58, 0x01EE, 1},
	{0x59, 0x010B, 1},
	{0x5A, 0x0140, 1},
	{0x5C, 0x1D17, 1},
	{0x5D, 0x221C, 1},
	{0x64, 0x1E1C, 1},
	{0x5B, 0x0001, 1},
#else
	{0xf0, 0x0002, 1},
	{0xc8, 0x00, 1},	//changed 2006-08-15	
	{0xf0, 0x000, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x21, 0x00c, 1},
	{0x20, 0x00100, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},

#endif
};
#endif

const TReg gSnrSizeRegValSXGA_MT9M112_YUV[] = 
{ 
    	{0xf0, 0x0002, 1},
	{0xc8, 0x1f0b, 1},	//default SXGA
	{0xf0, 0x0001, 1},
	{0x05, 0x000f, 1},
	{0xf0, 0x0000, 1},
	{0x02, 0x0068, 1},
};

const TSnrSizeCfg gSnrSizeCfg_MT9M112_YUV[] = 
{
	{
		{sizeof(gSnrSizeRegValSXGA_MT9M112_YUV) / sizeof(TReg),(PTReg)&gSnrSizeRegValSXGA_MT9M112_YUV},
		{0, NULL},
		{1280, 1024},
		2,
		0,
		0
	},
	{
		{sizeof(gSnrSizeRegValVGA_MT9M112_YUV) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9M112_YUV},
		{0, NULL},
		{640, 480},
		2,
		1,
		1
	}
};
const TSnrSizeCfg gSnrSizeCfg_MT9M112_YUV_VideoRecorder[] = 
{
	{
		{sizeof(gSnrSizeRegValVGA_MT9M112_YUV_VideoRecorder) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9M112_YUV_VideoRecorder},
		{0, NULL},
		{640, 480},
		2,
		1,
		1
	}
};

const TSnrInfo gSensorInfo_MT9M112_YUV = 
{
	"MT9M112 YUV",

	1,																										//yuv mode
	1,																										//not use PCLK of sensor
	0,																										
	0,																										//0 -- I2C, 1 -- Serial bus;
	0,																										//[0]: 1 -- OV I2C bus
	80,																										//I2C Rate : 100KHz
	0x5d,//0x48,//0x48,				//0x5d for 5xx sensor, 0x48 for 3xx sensor 																					//I2C address
	0,																									//ISP I2C address for special sensor
	0x01,																									//power config
	0,																										//reset sensor

	2,																										//brightness 
	2,																										//contrast
	2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
   	},


	{sizeof(gSifPwrOnSeqRegVal_MT9M112_YUV) / sizeof(TReg),(PTReg)&gSifPwrOnSeqRegVal_MT9M112_YUV},		
	{0,NULL},																								
	{0, NULL},	
	{{sizeof(gSifRegVal_MT9M112_YUV) / sizeof(TReg), (PTReg)&gSifRegVal_MT9M112_YUV}},						//sif initial value
	{0,NULL},

	{0,NULL},//{sizeof(gSnrSizeRegValInit_MT9M112) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M112},
	{sizeof(gSnrSizeCfg_MT9M112_YUV) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_MT9M112_YUV},		
	{sizeof(gSnrSizeCfg_MT9M112_YUV_VideoRecorder) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_MT9M112_YUV_VideoRecorder},		//{0,NULL},

	SensorSetReg_MT9M112_YUV,																				
	SensorGetReg_MT9M112_YUV,																				
	SensorIdChk_MT9M112_YUV,	
	SensorSetMirrorFlip_MT9M112_YUV,
	NULL,
	SensorSetContrast_MT9M112_YUV,
	SensorSetBrightness_MT9M112_YUV,
	SensorSetMode_MT9M112_YUV,																									//Set Sensor Mirror Flip Call
	NULL,//SnrGetEt_MT9M112,																				
	NULL,//SnrSetEt_MT9M112,																			

	{//	ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0						
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};

#endif

#if V5_DEF_SNR_MT9M112_YUV_5XX_NEW

void SensorSetReg_MT9M112_YUV_NEW(UINT32 uAddr, UINT16 uVal)
{
	VIM_SIF_I2cWriteWord((UINT8)uAddr, (UINT16)uVal,1);
}

void SensorGetReg_MT9M112_YUV_NEW(UINT32 uAddr, UINT16 *uVal)
{
	VIM_SIF_I2cReadWord((UINT8)uAddr, (UINT16*)uVal,1);
}

UINT8 SensorIdChk_MT9M112_YUV_NEW(void)
{
	UINT16 val;
	VIM_SIF_I2cWriteWord(0xf0, 0,1);
	VIM_SIF_I2cReadWord(0, &val,1);
	if(val == 0x148c)
		return SUCCEED;
	else
		return FAILED;
}

/******************************************************************
Desc: set sensor mirror and flip callback 
Para: BIT0 -- mirror
BIT1 -- flip
*******************************************************************/
void SensorSetMirrorFlip_MT9M112_YUV_NEW(UINT8 val)
{
	
}

/******************************************************************

		 Desc: set sensor Brightness  callback 

		 Para: 1-5 step

*******************************************************************/

void SensorSetBrightness_MT9M112_YUV_NEW(UINT8 val)
{
	
}

/******************************************************************

		 Desc: set sensor contrast	callback 

		 Para: 1-5 step

*******************************************************************/

void SensorSetContrast_MT9M112_YUV_NEW(UINT8 val)
{
	
}

/******************************************************************
		 Desc: set sensor mod  callback 
		 Para:	val:	1 50hz,   2 60hz, 3 night,4 outdoor
*******************************************************************/
void SensorSetMode_MT9M112_YUV_NEW(UINT8 val)
{
	
}

void SensorGetET_MT9M112_YUV_NEW(UINT8 index,UINT32 *t,UINT32 *Ggain, UINT8 *AWB)
{
	
}

void SensorSetET_MT9M112_YUV_NEW(UINT8 index,UINT32 t,UINT32 Ggain,UINT8 *AWB)
{
	
}

const TReg gSifPwrOnSeqRegVal_MT9M112_YUV_NEW[] =
{
	{0x00, 0x63, 1},
	{0x00, 0x62, 10},
	{0x00, 0x63, 10}
};

const TReg gSifRegVal_MT9M112_YUV_NEW[] =
{
	//{0x0C, 0x0a, 0},
	//{0x0d, 0x04, 0},
	{0x0c, 0x040a, 0},
	{0x04, 0x41, 0},
};

const TReg gSnrSizeRegValVGA_MT9M112_YUV_NEW[] = 
{
#if 1
	{0xf0, 0x0000, 1},
	{0x02, 0x0064, 1},	//changed 2006-08-15	
	{0x05, 0x00D0, 1},
	{0x06, 0x0012, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x20, 0x0100, 1},
	{0x21, 0x8000, 1},
	{0x22, 0x0D0F, 1},
	{0x24, 0x8000, 1},
	{0x59, 0x00FF, 1},
					  
	{0xf0, 0x0001, 1},
	{0x05, 0x0009, 1},
	{0x06, 0x600e, 1},
	#if 0	//add for another sensor 2006-10-19 16:00
	{0x3b, 0x0430, 1},	//black level 
	
	{0x53, 0x0c04, 1},	//context A gamma 
	{0x54, 0x4421, 1},
	{0x55, 0x8f6f, 1},
	{0x56, 0xc2aa, 1},
	{0x57, 0xecd8, 1},
	{0x58, 0xff00, 1},
	{0xdc, 0x0c04, 1},	//context B gamma
	{0xdd, 0x4421, 1},
	{0xde, 0x8f6f, 1},
	{0xdf, 0xc2aa, 1},
	{0xe0, 0xecd8, 1},
	{0xe1, 0xff00, 1},
	#endif 
	{0xf0, 0x0002, 1},
	{0xc8, 0x0000, 1},
	{0x2e, 0x084a, 1},
	{0x2f, 0xd100, 1},
	{0x39, 0x03CA, 1},
	{0x3A, 0x0680, 1},
	{0x3B, 0x0152, 1},
	{0x3C, 0x0540, 1},
	{0x57, 0x019C, 1},	//changed 2006-08-15	
	{0x58, 0x01EE, 1},
	{0x59, 0x010B, 1},
	{0x5A, 0x0140, 1},
	{0x5C, 0x1D17, 1},
	{0x5D, 0x221C, 1},
	{0x64, 0x1E1C, 1},
	{0x5B, 0x0001, 1},
#else
	{0xf0, 0x0002, 1},
	{0xc8, 0x00, 1},	//changed 2006-08-15	
	{0xf0, 0x000, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x21, 0x00c, 1},
	{0x20, 0x00100, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},

#endif
};
const TReg gSnrSizeRegValVGA_MT9M112_YUV_VideoRecorder_NEW[] = 
{
#if 1
	{0xf0, 0x0000, 1},
	{0x02, 0x0064, 1},	//changed 2006-08-15	
	{0x05, 0x00D0, 1},
	{0x06, 0x0012, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x20, 0x0100, 1},
	{0x21, 0x8000, 1},
	{0x22, 0x0D0F, 1},
	{0x24, 0x8000, 1},
	{0x59, 0x00FF, 1},
					  
	{0xf0, 0x0001, 1},
	{0x05, 0x0009, 1},
	{0x06, 0x600e, 1},
	#if 0	//add for another sensor 2006-10-19 16:00
	{0x3b, 0x0430, 1},	//black level 
	
	{0x53, 0x0c04, 1},	//context A gamma 
	{0x54, 0x4421, 1},
	{0x55, 0x8f6f, 1},
	{0x56, 0xc2aa, 1},
	{0x57, 0xecd8, 1},
	{0x58, 0xff00, 1},
	{0xdc, 0x0c04, 1},	//context B gamma
	{0xdd, 0x4421, 1},
	{0xde, 0x8f6f, 1},
	{0xdf, 0xc2aa, 1},
	{0xe0, 0xecd8, 1},
	{0xe1, 0xff00, 1},
	#endif 
	{0xf0, 0x0002, 1},
	{0xc8, 0x0000, 1},
	{0x2e, 0x084a, 1},
	{0x2f, 0xd100, 1},
	{0x39, 0x03CA, 1},
	{0x3A, 0x0680, 1},
	{0x3B, 0x0152, 1},
	{0x3C, 0x0540, 1},
	{0x57, 0x019C, 1},	//changed 2006-08-15	
	{0x58, 0x01EE, 1},
	{0x59, 0x010B, 1},
	{0x5A, 0x0140, 1},
	{0x5C, 0x1D17, 1},
	{0x5D, 0x221C, 1},
	{0x64, 0x1E1C, 1},
	{0x5B, 0x0001, 1},
#else
	{0xf0, 0x0002, 1},
	{0xc8, 0x00, 1},	//changed 2006-08-15	
	{0xf0, 0x000, 1},
	{0x07, 0x0130, 1},
	{0x08, 0x001a, 1},
	
	{0x21, 0x00c, 1},
	{0x20, 0x00100, 1},
	//{0x07, 0x0142, 1},
	//{0x08, 0x0012, 1},

#endif
};


const TReg gSnrSizeRegValSXGA_MT9M112_YUV_NEW[] = 
{ 
		{0xf0, 0x0002, 1},
	{0xc8, 0x1f0b, 1},	//default SXGA
	{0xf0, 0x0001, 1},
	{0x05, 0x000f, 1},
	{0xf0, 0x0000, 1},
	{0x02, 0x0068, 1},
};

const TSnrSizeCfg gSnrSizeCfg_MT9M112_YUV_NEW[] = 
{
	{
		{sizeof(gSnrSizeRegValSXGA_MT9M112_YUV_NEW) / sizeof(TReg),(PTReg)&gSnrSizeRegValSXGA_MT9M112_YUV_NEW},
		{0, NULL},
		{1280, 1024},
		2,
		0,
		0
	},
	{
		{sizeof(gSnrSizeRegValVGA_MT9M112_YUV_NEW) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9M112_YUV_NEW},
		{0, NULL},
		{640, 480},
		2,
		1,
		1
	}
};
const TSnrSizeCfg gSnrSizeCfg_MT9M112_YUV_VideoRecorder_NEW[] = 
{
	{
		{sizeof(gSnrSizeRegValVGA_MT9M112_YUV_VideoRecorder_NEW) / sizeof(TReg),(PTReg)&gSnrSizeRegValVGA_MT9M112_YUV_VideoRecorder_NEW},
		{0, NULL},
		{640, 480},
		2,
		1,
		1
	}
};

const TSnrInfo gSensorInfo_MT9M112_YUV_NEW = 
{
	"MT9M112 YUV NEW",

	1,																										//yuv mode
	1,																										//not use PCLK of sensor
	0,																										
	0,																										//0 -- I2C, 1 -- Serial bus;
	0,																										//[0]: 1 -- OV I2C bus
	80, 																									//I2C Rate : 100KHz
	0x48,//0x5d,//0x48, 			//0x5d for 5xx sensor, 0x48 for 3xx sensor																					//I2C address
	0,																									//ISP I2C address for special sensor
	0x01,																									//power config
	0,																										//reset sensor

	2,																										//brightness 
	2,																										//contrast
	2,													//saturation
	{
	//	0x00,0x08,0x23,0x41,0x61,0x7a,0x92,0xa7,0xb9,0xc8,0xd5,0xe0,0xe9,0xf0,0xf7,0xfb,0xff
		0x00,0x08,0x1c,0x32,0x48,0x5e,0x76,0x8c,0x9e,0xae,0xbe,0xcc,0xd8,0xe1,0xea,0xf6,0xff
	},


	{sizeof(gSifPwrOnSeqRegVal_MT9M112_YUV_NEW) / sizeof(TReg),(PTReg)&gSifPwrOnSeqRegVal_MT9M112_YUV_NEW}, 	
	{0,NULL},																								
	{0, NULL},	
	{{sizeof(gSifRegVal_MT9M112_YUV_NEW) / sizeof(TReg), (PTReg)&gSifRegVal_MT9M112_YUV_NEW}},						//sif initial value
	{0,NULL},

	{0,NULL},//{sizeof(gSnrSizeRegValInit_MT9M112) / sizeof(TReg), (PTReg)&gSnrSizeRegValInit_MT9M112},
	{sizeof(gSnrSizeCfg_MT9M112_YUV_NEW) / sizeof(TSnrSizeCfg), (PTSnrSizeCfg)&gSnrSizeCfg_MT9M112_YUV_NEW},		
	{sizeof(gSnrSizeCfg_MT9M112_YUV_VideoRecorder_NEW) / sizeof(TSnrSizeCfg),	(PTSnrSizeCfg)&gSnrSizeCfg_MT9M112_YUV_VideoRecorder_NEW},		//{0,NULL},

	SensorSetReg_MT9M112_YUV_NEW,																				
	SensorGetReg_MT9M112_YUV_NEW,																				
	SensorIdChk_MT9M112_YUV_NEW,	
	SensorSetMirrorFlip_MT9M112_YUV_NEW,
	NULL,
	SensorSetContrast_MT9M112_YUV_NEW,
	SensorSetBrightness_MT9M112_YUV_NEW,
	SensorSetMode_MT9M112_YUV_NEW,																									//Set Sensor Mirror Flip Call
	NULL,//SnrGetEt_MT9M112,																				
	NULL,//SnrSetEt_MT9M112,																			

	{// ytarget ythd ytop gaintarget gainthd,gaindelay,gain,minet,maxgain,mingain,speed
		0x80, 0x08, 0x9f, 0x60, 0x20, 1, 0x60, 0, 0xe0, 0x40, 0 					
	},
	{	//ythd of flash,et of flash
		0x20,			10
	},
	1
																						
};

#endif




const PTSnrInfo gPSensorInfo[] =
{
#if	V5_DEF_SNR_DUMMY
            (const PTSnrInfo)&gSensorInfo_DUMMY,                  // //V5_DEF_SNR_MT9V011_BAYER
#endif

#if	V5_DEF_SNR_MT9V011_BAYER
            (const PTSnrInfo)&gSensorInfo_MT9V011_BAYER,                  // //V5_DEF_SNR_MT9V011_BAYER
#endif

#if	V5_DEF_SNR_OV2640_BAYER
        (const PTSnrInfo)&gSensorInfo_OV2640_BAYER,         //V5_DEF_SNR_OV2640_BAYER
#endif

#if	V5_DEF_SNR_OV2640_YUV
        (const PTSnrInfo)&gSensorInfo_OV2640_YUV,         //V5_DEF_SNR_OV2640_YUV
#endif

#if	V5_DEF_SNR_OV2650_YUV
	(const PTSnrInfo)&gSensorInfo_OV2650_YUV,
#endif

#if	V5_DEF_SNR_OV3630_BAYER
	(const PTSnrInfo)&gSensorInfo_OV3630_BAYER,			//V5_DEF_SNR_OV3630_BAYER
#endif

#if	V5_DEF_SNR_MT9T012_BAYER
	(const PTSnrInfo)&gSensorInfo_MT9T012_BAYER,			//V5_DEF_SNR_MT9T012_BAYER
#endif


#if	V5_DEF_SNR_MT9M111_YUV
	(const PTSnrInfo)&gSensorInfo_MT9M111_YUV,			//V5_DEF_SNR_MT9M111_YUV
#endif

#if	V5_DEF_SNR_MT9M112_BAYER
       (const PTSnrInfo)&gSensorInfo_MT9M112_BAYER,        //V5_DEF_SNR_MT9M112_BAYER
#endif

#if	V5_DEF_SNR_MT9M112_YUV
            (const PTSnrInfo)&gSensorInfo_MT9M112_YUV,         //V5_DEF_SNR_MT9M112_YUV
#endif

#if	V5_DEF_SNR_MT9M112_YUV_5XX
            (const PTSnrInfo)&gSensorInfo_MT9M112_YUV,         //V5_DEF_SNR_MT9M112_YUV
#endif

#if V5_DEF_SNR_MT9M112_YUV_5XX_NEW
			(const PTSnrInfo)&gSensorInfo_MT9M112_YUV_NEW,		   //V5_DEF_SNR_MT9M112_YUV
#endif


#if	V5_DEF_SNR_OV9650_YUV
				(const PTSnrInfo)&gSensorInfo_OV9650_YUV,		   //V5_DEF_SNR_MT9M112_YUV
#endif
	NULL												//Avoid no sensor define
};

const UINT32 gSensorNum = sizeof(gPSensorInfo) / sizeof(PTSnrInfo);

