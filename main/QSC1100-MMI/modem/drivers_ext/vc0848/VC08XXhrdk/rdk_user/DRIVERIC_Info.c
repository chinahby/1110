/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : DRIVERIC_Info.c
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
#include "MMP_rdk.h"

//#include "DRIVERIC_Info.h"

#if MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_ILI9320_16BIT_240x320	//天宇LCD ST7712
const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x20,0,0x21,0,0x22},
  0x03,0x0807,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 //0x0ca2, 0x0ca2,
 0x0461, 0x0ca2,
 //0x20, 0x8, 0x8
  0x25, 0x4, 0x02
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};

#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_ILI9320_16BIT_320x240	//天宇LCD ST7712
const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x20,0,0x21,0,0x22},
 0x07,0x03,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 //0x0ca2, 0x0ca2,
 0x0461, 0x0ca2,
 0x40, 0x10, 0x10
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};
#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_ILI9325_CPT24_16BIT  //to xiang zhi hong 10/17/2008

const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x20,0,0x21,0,0x22},
 0x03, 0x0807,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 //0x0461, 0x0461,	//用这组参数不可以完成视频的旋转，会慢慢地白屏
 //0x3c, 0xd, 0x0b
// 0x0461, 0x0ca2,		//用这组参数就可以完成视频文件的旋转
 //0x25, 0x4, 0x02
 0x421,0x421,
 45, 3, 14
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};


#elif (MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_HX8347_18BIT)
const  TDBI_Parm DBIgroup=

//for 16+2 rgb data transfer mode 5/5/2008

{
     0x00,
     0xeeaa,
     9,
     {0x0003,0x0000,0x0002,0x0000,       //start x
       0x0007,0x0000,0x0006,0x0000,      //start y
      // 0x0005,0x00ef,//0x0004,0x0000,      //end x
      // 0x0009,0x003f,0x0008,0x0001,        //end y
       0x0022
     },
     0x0703, 0x0f0b,0x0000, 0x0000,//0x1b17,//0x1713,  //stxp, styp, endxp, endyp
     0, 0, 0, 0,   //Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos,
     COMMAND_ONLY,
     0x0, 0x0, 0x0, 0x0,
   
     MAINPANEL,                 //config

  //   PANEL_FMT666_2TIMES_BIG|PANEL_RGB,              //fmt : 16bit bus/RGB565,16+2 mode

      PANEL_FMT565_1TIMES|PANEL_RGB,    
       0x460, 0x460,
        40,4,1
};

const TLcdifParm gDriverIc_LcdIF =
{

    VIM_LCDIF_DBI_B,            //LCD IF type enable

    (PTDBI_Parm)&DBIgroup,

    NULL

};
#elif (MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_LPFD5420A)
const  TDBI_Parm DBIgroup=

{
	 0x00,			//special LCD flag
	
	 0xffea,		//rsflag
	
	 5, 		//head num
	
	{0x200,0,0x201,0,0x202},
	
	 0x03, 0x0807,0,0,
	
	0, 0, 0, 0, //stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
	
	 COMMAND_ONLY,
	
	 0x0, 0x0, 0x0, 0x0,
	
	  
	
	 MAINPANEL, 	 //main panel or sub panel
	
	 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
	
	 //PANEL_FMT666_2TIMES_BIG|PANEL_RGB,  //fmt
	
	 
	
	 0x0461, 0x0ca2,
	
	 0x3c, 0x05, 0x05

};

const TLcdifParm gDriverIc_LcdIF =

{

    VIM_LCDIF_DBI_B,            //LCD IF type enable

    (PTDBI_Parm)&DBIgroup,

    NULL

};
#elif (MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_S6D0170)
const  TDBI_Parm DBIgroup=

{
	0x00,
	
	0xfeaa,  //rsflag
	
	9,	 //head num
	
	{0x47, 0x00, 0x46, 0xef, 0x49,0x00, 0x48, 0x13f, 0x22},
	
	3,0x0c0b, 7, 0x100f, //startx starty endx endy
	
	 
	
	  0, 0, 0, 0,	//Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos,
	
	   COMMAND_ONLY,
	
	   0x0, 0x0, 0x0, 0x0,
	
	 
	
	MAINPANEL,	//main panel or sub panel
	
	PANEL_FMT565_1TIMES|PANEL_RGB,	//fmt
	
	0x461, 0x460,
	
	0x3C,0x0d,0x0b


};

const TLcdifParm gDriverIc_LcdIF =

{

    VIM_LCDIF_DBI_B,            //LCD IF type enable

    (PTDBI_Parm)&DBIgroup,

    NULL

};


#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_R61509_16BIT	//to xiang zhi hong 11/4/2008
const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x200,0,0x201,0,0x202},
 0x03, 0x0807,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 0x0461, 0x0461,
 0x25, 0x4, 0x02
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};


#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_R61505U_16BIT	//to xiang zhi hong 11/13/2008 精佑
const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x20,0,0x21,0,0x22},
 0x03, 0x0807,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 0x0461, 0x0461,
 0x25, 0x4, 0x02
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};

#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_R61509_16BIT	//to xiang zhi hong 11/4/2008
const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x200,0,0x201,0,0x202},
 0x03, 0x0807,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 0x0461, 0x0461,
 0x25, 0x4, 0x02
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};

#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_S6D0164_16BIT  //to xiang zhi hong 10/17/2008

const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xffea,		//rsflag
 5,			//head num
{0x20,0,0x21,0,0x22},
 0x03, 0x0807,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 //0x0461, 0x0461,	//用这组参数不可以完成视频的旋转，会慢慢地白屏
 //0x3c, 0xd, 0x0b
// 0x0461, 0x0ca2,		//用这组参数就可以完成视频文件的旋转
 //0x25, 0x4, 0x02
 0x421,0x421,
 45, 3, 14
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};
#elif MMD_MAINDRIVERIC_TYPE == MMD_DEF_LCD_LGDP4525_16BIT

const  TDBI_Parm DBIgroup=
{
 0x00,			//special LCD flag
 0xfffa,		//rsflag
 3,			//head num
{0x21,0,0x22},
 0x03, 0x4,0,0,
 0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
 COMMAND_ONLY,
 0x0, 0x0, 0x0, 0x0,
  
 MAINPANEL, 		 //main panel or sub panel
 PANEL_FMT565_1TIMES|PANEL_RGB,  //fmt
 
 //0x0461, 0x0461,	//用这组参数不可以完成视频的旋转，会慢慢地白屏
 //0x3c, 0xd, 0x0b
// 0x0461, 0x0ca2,		//用这组参数就可以完成视频文件的旋转
 //0x25, 0x4, 0x02
 0x421,0x421,
 45, 3, 14
};

const TLcdifParm gDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};

#else
#endif



/*--------------------------sub panel driver---------*/

#if MMD_SUBDRIVERIC_TYPE == MMD_DEF_LCD_NOSUB
const TLcdifParm gSubDriverIc_LcdIF ={ VIM_LCDIF_DBI_B,NULL,NULL };

#elif MMD_SUBDRIVERIC_TYPE == MMD_DEF_LCD_ILI9320_16BIT_320x240
const  TDBI_Parm DBIgroup=
{
	0xffea,		//rsflag
	5,			//head num
	{0x20, 0, 0x21, 0, 0x22},
	3, 7, 0, 0,	//

	0, 0, 0, 0,	//stxp, styp, endxp, endyp,Repeat_start_x_pos,Repeat_start_y_pos,Repeat_end_x_pos,Repeat_end_y_pos
	 COMMAND_ONLY,
	 0x0, 0x0, 0x0, 0x0,
	
	MAINPANEL,		//main panel or sub panel		??why this is MAINPANEL?? wendy guo 4/4/2007
	PANEL_FMT565_1TIMES|PANEL_RGB,		//fmt
	0x460, 0x460,
	0x47,0x1d,0x1b,
};
const TLcdifParm gSubDriverIc_LcdIF =
{
	VIM_LCDIF_DBI_B,			//LCD IF type enable
	(PTDBI_Parm)&DBIgroup,
	NULL
};

#else

#endif


const TPanelInfo gDriverIcInfo=
{
	(PTLcdifParm)&gDriverIc_LcdIF,
	(PTLcdifParm)&gSubDriverIc_LcdIF,
};

