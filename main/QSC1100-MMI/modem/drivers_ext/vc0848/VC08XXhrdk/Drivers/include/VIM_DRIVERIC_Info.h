/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_DRIVERIC_Info.h
 * [Description]        : 适用于578b.
 *				    LCD panel parameter setting structure.
 *				    				
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-04-04 16:01
 * [Platform]               : not be limited by platform
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-04-04  		WendyGuo     		 Created
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



#ifndef _VIM_5XRDK_DRIVERINFO_H_
#define _VIM_5XRDK_DRIVERINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

enum PANELCSenum {
	MAINPANEL=0,
	SUBPANEL=0x10,
	PANEL_ERROR = 0x7fffffff
};

/*
enum PANELFMTenum {
	PANEL_FMT565_1TIMES=0,
	PANEL_FMT565_2TIMES=4,
	
	PANEL_FMT555_1TIMES=1,

	PANEL_FMT666_1TIMES=3,
	PANEL_FMT666_2TIMES_LITTLE=5,
	PANEL_FMT666_2TIMES_BIG=6,
	
	PANEL_FMT444_1TIMES=2,
	PANEL_FMT444_2TIMES_BIG=8,
	PANEL_FMT444_2TIMES_LITTLE=9,
	PANEL_FMT444_3TIMES_BIG=12,
	PANEL_FMT444_3TIMES_LITTLE=13	

};
enum PANELRGBenum {
	PANEL_RGB=0<<4,
	PANEL_RBG=1<<4,
	PANEL_GRB=2<<4,
	PANEL_GBR=3<<4,
	PANEL_BRG=4<<4,
	PANEL_BGR=5<<4
};*/

enum PANELFMTenum {
	PANEL_FMT565_1TIMES=0,
	PANEL_FMT565_2TIMES=4,
	
	PANEL_FMT555_1TIMES=1,

	PANEL_FMT666_1TIMES=3,
	PANEL_FMT666_2TIMES_LITTLE=5,
	PANEL_FMT666_2TIMES_BIG=6,
	PANEL_FMT666_1TIMES_BIG=7,		//surport ??
	
	PANEL_FMT444_1TIMES=2,
	PANEL_FMT444_2TIMES_BIG=8,
	PANEL_FMT444_2TIMES_LITTLE=9,
	PANEL_FMT444_3TIMES_BIG=12,
	PANEL_FMT444_3TIMES_LITTLE=13,

	PANEL_FMT888_2TIMES_BIG=10,		//?
	//PANEL_FMT666_2TIMES_LITTLE=11,	//?					//same as PANEL_FMT666_2TIMES_LITTLE=5,
	
	PANEL_FMT888_3TIMES_BIG=14,		//?
	PANEL_FMT888_3TIMES_LITTLE=15,	//?

	PANEL_FMT666_2TIMES=16,		//?	9+9
	PANEL_FMT888_3TIMES=24,		//? 8+8+8
	PANEL_FMT666_3TIMES=25,		//? 8+8+2
	PANEL_FMT_ERROR = 0x7fffffff

};
enum PANELRGBenum {
	PANEL_RGB=0<<20,				//MPPAT set from b20--b22
	PANEL_RBG=1<<20,
	PANEL_GRB=2<<20,
	PANEL_GBR=3<<20,
	PANEL_BRG=4<<20,
	PANEL_BGR=5<<20,
	PANEL_RGB_ERROR = 0x7fffffff
};

enum DRIVERICFLAGenum {
	COMMAND_ONLY=0,
	COMMAND_VALUE_ONEBYTE=BIT1,
	REPEAT_COORDINATE=BIT0,
	DRIVERICFLAG_ERROR = 0x7fffffff
};

#if 0
enum PC_Polarization_emum{
	PixelClock_Positive=BIT4,
	PixelClock_Negative=~BIT4
};
enum DE_Polarization_emum{
	DataEn_Positive=BIT5,
	DataEn_Negative=~BIT5
};
enum HS_Polarization_emum{
	HS_Positive=BIT6,
	HS_Negative=~BIT6
};
enum VS_Polarization_emum{
	VS_Positive=BIT7,
	VS_Negative=~BIT7
};


enum Head_Config_emum{
	Hen_head_En=BIT0,
	Head_test=BIT2,			//set the RGB bit order in the data path according to the format definition of DFMT
	Update_head=BIT3
	
};
#endif
typedef enum _VIM_PC_POLARIZATION{
	VIM_PIXCLOCK_POSITIVE=BIT4,
	VIM_PIXCLOCK_NEGATIVE=0,//~BIT4,
	VIM_PC_POLARIZATION_ERROR=0x7fffffff
}VIM_PC_POLARIZATION;

typedef enum _VIM_DE_POLARIZATION{
	VIM_DATA_POSITIVE=BIT5,
	VIM_DATA_NEGATIVE=0,//~BIT5,
	VIM_DE_POLARIZATION_ERROR=0x7fffffff
}VIM_DE_POLARIZATION;

typedef enum _VIM_HS_POLARIZATION{
	VIM_HS_POSITIVE=BIT6,
	VIM_HS_NEGATIVE=0,//~BIT6,
	VIM_HS_POLARIZATION_ERROR=0x7fffffff
}VIM_HS_POLARIZATION;

typedef enum _VIM_VS_POLARIZATION{
	VIM_VS_POSITIVE=BIT7,
	VIM_VS_NEGATIVE=0,//~BIT7,
	VIM_VS_POLARIZATION_ERROR=0x7fffffff
}VIM_VS_POLARIZATION;

typedef enum _VIM_HEAD_CONFIG{
	VIM_HEN_HEAD=BIT0,
	VIM_HEAD_TEST=BIT2,			//set the RGB bit order in the data path according to the format definition of DFMT
	VIM_UODATE_HEAD=BIT3,
	VIM_HEAD_CONFIG_ERROR=0x7fffffff
}VIM_HEAD_CONFIG;

typedef enum _VIM_LCDIF_TYPE{
	VIM_LCDIF_DBI_B=BIT1,		
	VIM_LCDIF_DBI_A=BIT2,
	VIM_LCDIF_DPI=BIT3,
	VIM_LCDIF_TYPE_ERROR=0x7fffffff
}VIM_LCDIF_TYPE;

#define PANEL_CONVERSE_HIGHLOW 0x80

//define LCD panel type    //angela change
#define NOSUB									1

//Hitachi
#define V5_DEF_LCD_HD66773_16BIT				2
#define V5_DEF_LCD_HD66773_8BIT				3
#define V5_DEF_LCD_HD66777_16BIT	       		4
#define V5_DEF_LCD_HD66768_8BIT				5
#define V5_DEF_LCD_HD66772_HD66774_16BIT		6
#define V5_DEF_LCD_HD66781S_HD66783_16BIT	7

////LDS
#define V5_DEF_LCD_LDS183_16BIT				8
#define V5_DEF_LCD_LDS183_8BIT					9
#define V5_DEF_LCD_LDS506_8BIT					10


//Solomon
#define V5_DEF_LCD_SSD1788_8BIT				11
#define V5_DEF_LCD_SSD1781_8BIT				12
#define V5_DEF_LCD_SSD1773_8BIT				13
#define V5_DEF_LCD_SSD1332_8BIT				14
#define  V5_DEF_LCD_SSD1289_16BIT				15

//Samsung
#define V5_DEF_LCD_S6B33B5_8BIT				16
#define V5_DEF_LCD_S6B33B2_S6B33BG_16BIT		17
#define V5_DEF_LCD_S6B33B2_S6B33BG_8BIT		18
#define V5_DEF_LCD_S6D0110A_8BIT				19
#define  V5_DEF_LCD_S6D0110_16BIT          		20
#define V5_DEF_LCD_S1D19102_S1D17D01_16BIT	21
#define V5_DEF_LCD_S6D0118_8BIT				22
#define V5_DEF_LCD_S1D19105D_16BIT			23
#define V5_DEF_LCD_S1D19111D_8BIT				24
#define V5_DEF_LCD_S6B33B9_16BIT				25
#define V5_DEF_LCD_S6D0123_16BIT				26
#define V5_DEF_LCD_S6D0118_16BIT 				27
#define V5_DEF_LCD_S6B33BC_16BIT				28
#define V5_DEF_LCD_S6B33BC_8BIT				29
#define V5_DEF_LCD_S6D0144_16BIT				30
#define V5_DEF_LCD_S6D0134_16BIT				31	//heju add??
#define V5_DEF_LCD_S1D19120_16BIT				32

//HiMark
#define V5_DEF_LCD_HX8301_HX8609A_16BIT		33
#define V5_DEF_LCD_HX8309_16BIT				34
#define V5_DEF_LCD_HX8312A_16BIT 				35
#define V5_DEF_LCD_HX8309_8BIT				36

//Nec
#define V5_DEF_LCD_uPD161801_uPD161861_16BIT	37
#define V5_DEF_LCD_uPD161963_8BIT					38
#define V5_DEF_LCD_UPD161690_8BIT 				39


//LG
#define V5_DEF_LCD_LGDP4511_16BIT				40
#define V5_DEF_LCD_LGDP4216_16BIT				41

//Tomato
#define V5_DEF_LCD_TL1711_16BIT				42
#define V5_DEF_LCD_TL1772_8BIT					43
#define V5_DEF_LCD_TL1763_16BIT				44			//韶丰TL1763


//Renesas
#define V5_DEF_LCD_R61500_16BIT				45
#define V5_DEF_LCD_R61503B_16BIT				46
#define V5_DEF_LCD_R61503B_8BIT				47
#define V5_DEF_LCD_R61508_16BIT				48

//Toppoly
#define V5_DEF_LCD_C1E2_04_16BIT				49
#define V5_DEF_LCD_PL2001A_16BIT				50

//Isron
#define V5_DEF_LCD_IS2320_16BIT				51
#define V5_DEF_LCD_IS2102_IS2202_16BIT		52

//Orise Tech
#define  V5_DEF_LCD_SPFD5414_16BIT			53
#define  V5_DEF_LCD_SPFD54126_16BIT			54


//sitronix
#define V5_DEF_LCD_ST7636_8BIT					55
#define V5_DEF_LCD_ST7712_8BIT					56			//天宇lcd ST7712

//ULTRA
#define V5_DEF_LCD_UC1697_8BIT				57			//凌达lcd 1/23/2007

//KUNSHAN
#define V5_DEF_LCD_ILI9320_16BIT_240x320				58			//add by zhang xuecheng,070910
#define V5_DEF_LCD_ILI9320_16BIT_320x240				59
#define V8_DEF_LCD_DF4827Y_6FLWa                60 
#define V5_DEF_LCD_SPFD5408_16BIT_320x240        61
#define V5_DEF_LCD_YUSHUN_HX8347_S95261_16BIT_320x240       62
	
void	PanelSetReg(UINT8 type, UINT32 adr, UINT16 val);

typedef void (*PPanelCallback)(void);

//void	PanelSetReg(UINT8 type, UINT32 adr, UINT16 val);		//wendy guo 4/4/2007

typedef struct TRegGroup_DBI{
 //UINT8   SpecialLCD_flag;      // this flag =1, means use Repeat_stxp... parameters
 UINT32   SpecialLCD_flag;      // this flag =1, means use Repeat_stxp... parameters, by lizg
 
 UINT32 rsflag;          //SpecialLCD_flag=0
 UINT32 headnum;
 UINT32 head[16];
 UINT32 stxp, styp, endxp, endyp;
 
 UINT32 Repeat_stxp, Repeat_styp, Repeat_endxp, Repeat_endyp;
 UINT32 Head_Process_flag;        //SpecialLCD_flag=1
 UINT32 Start_x_comm_code,End_x_comm_code,Start_y_comm_code,End_y_comm_code;
 
 //UINT8   config;        //only for program to judge the Main panel or Sub panel to set CS1, CS2
 UINT32   config;        //only for program to judge the Main panel or Sub panel to set CS1, CS2
 UINT32 fmt;
 UINT32 wcy, rcy;
 UINT32 dedelay,deinteval,lineinteval;   //?? should write to the LCDC register but I don't know the register's address of them now!
 
}TDBI_Parm, *PTDBI_Parm;
 

typedef struct TRegGroup_DPI{
 UINT32 SerialCOMMD_BIT_NUM, SerialDATA_BIT_NUM, SCL_WIDTH;
 
 UINT32 PixClock_Polarization, DE_Polarization, HS_Polarization, VS_Polarization;     //set the polarization of Pixel Clock Pol and Data Enable Pol and Vertical syn signal Pol and Horizontal syn signal Pol;
 UINT32 PixClock;//MHz
 UINT32 HS_END_X, HBP_END_X, AR_END_X, HFP_END_X; //every parameter have 10bit, 0x00--0x3ff
 UINT32 VS_END_Y, VBP_END_Y, AR_END_Y, VFP_END_Y; //every parameter have 10bit, 0x00--0x3ff
 
 
}TDPI_Parm, *PTDPI_Parm;
 

typedef struct tag_TLcdifParm{
 VIM_LCDIF_TYPE LCDIF_TYPE;						//DBI mode Type B or Type A interface
 
 PTDBI_Parm DBI_Parm;     				 //LCD panel mode is DBI
 PTDPI_Parm DPI_Parm;					//LCD panel mode is DPI
} TLcdifParm, *PTLcdifParm;

typedef struct tag_TPanelInfo {
	PTLcdifParm master;				
	PTLcdifParm slave;
	PPanelCallback	pBacklightOnEx;		//extra backlight on function
} TPanelInfo, *PTPanelInfo;

extern const TPanelInfo gDriverIcInfo;

#ifdef __cplusplus
}
#endif

#endif


