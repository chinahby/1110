/**
 * @file A8_panel.c
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


sPanel_Ctrl_Attrib gMainPanelControlAttrib;


#ifndef A8_DISABLE_SUB_LCD_FUNCTION
sPanel_Ctrl_Attrib gMainPanelControlAttrib;
#endif

// Initial the both Main and Sub LCD. The shared portion is initialized for Main LCD.
u_char A800_HostInitLCDCommon(void)
{
	if(gMainPanelControlAttrib.PIXEL_CLOCK_DIV>0x0f)
	{
//		AIT_Message_Error("[A800_HostInitLCDCommon] Pixel cleck setting error!!%s,%d",(__FILE__,__LINE__));
		return A8_SYSTEM_ERROR;
	}

	SetA8RegB(0x5000,0x02);
	 	
   SetA8RegB(0x5002,(unsigned char)gMainPanelControlAttrib.PIXEL_CLOCK_DIV);
   SetA8RegW(0x500C,gMainPanelControlAttrib.ADDRESS_INDEX);
   SetA8RegW(0x500E,gMainPanelControlAttrib.ADDRESS_DATA);
   SetA8RegW(0x5010,gMainPanelControlAttrib.DATA_INDEX);
#ifndef A8_DISABLE_SUB_LCD_FUNCTION	

   SetA8RegW(0x510C,gMainPanelControlAttrib.ADDRESS_INDEX);		// command and index use...		
   SetA8RegW(0x510E,gMainPanelControlAttrib.ADDRESS_DATA);		// command and index use...
   SetA8RegW(0x5110,gMainPanelControlAttrib.DATA_INDEX);		// command and index use...
#endif
   SetA8RegB(0x5042,NO_DUAL_PANEL_PREVIEW|
            LCD1ACTIVE |
            PANEL1_RGB);
   if (gMainPanelControlAttrib.LCD_BusCtl.BusWidth==16)
   {
      SetA8RegB(0x5026, TYPE_80|
            POLARITY_ENABLE|
            PHASE_ENABLE|
            NON_BURST_MODE|
         //		   BURST_MODE|
            DATA_BUS_16);

      SetA8RegB(0x5027, RGB56516BIT);
   }
   if (gMainPanelControlAttrib.LCD_BusCtl.BusWidth==8)
   {
#if 1				
//	add for speed up for LCD refresh
	SetA8RegB(0x5028,GetA8RegB(0x5028)|0xc0);
	SetA8RegB(0x5026, TYPE_80|
            POLARITY_ENABLE|
            PHASE_ENABLE|
            NON_BURST_MODE|
            DATA_BUS_16);

      SetA8RegB(0x5027, RGB56516BIT);

	if(gMainPanelControlAttrib.PIXEL_CLOCK_DIV!=3)
	{
//		AIT_Message_Error("!!!Warring!!! Please set PIXEL_CLOCK_DIV to 3, or LCD refresh will abnormal:%s %d" ,(__FILE__,__LINE__));
	}

	  
#else	//Normal Setting		
   
      SetA8RegB(0x5026, TYPE_80|
            POLARITY_ENABLE|
            PHASE_ENABLE|
            NON_BURST_MODE|
            DATA_BUS_8);

      SetA8RegB(0x5027, RGB56508BIT);
#endif      
   }
#ifndef A8_DISABLE_SUB_LCD_FUNCTION	

   if (gMainPanelControlAttrib.LCD_BusCtl.BusWidth==16)
   {
      SetA8RegB(0x503A, TYPE_80|
            POLARITY_ENABLE|
            PHASE_ENABLE|
            NON_BURST_MODE|
            DATA_BUS_16);

      SetA8RegB(0x503B, RGB56516BIT);
   }
   if (gMainPanelControlAttrib.LCD_BusCtl.BusWidth==8)
   {
      SetA8RegB(0x503A, TYPE_80|
            POLARITY_ENABLE|
            PHASE_ENABLE|
            NON_BURST_MODE|
            DATA_BUS_8);

      SetA8RegB(0x503B, RGB56508BIT);
   }
#endif
   SetA8RegW(0x5044,gMainPanelControlAttrib.LCD_BusCtl.Width);
   SetA8RegW(0x5046,gMainPanelControlAttrib.LCD_BusCtl.Height);
   SetA8RegB(0x5048,0x00);
   SetA8RegB(0x5040,0xF1);

   SetA8RegW(0x5020, ((gMainPanelControlAttrib.LCD_BusCtl.Width*gMainPanelControlAttrib.LCD_BusCtl.Height) & 0xFFFF));
   SetA8RegB(0x5022,(unsigned char) ((gMainPanelControlAttrib.LCD_BusCtl.Width*gMainPanelControlAttrib.LCD_BusCtl.Height) >>16));

   SetA8RegB(0x50e0,0x00);
   SetA8RegB(0x50e1,0x00);
   SetA8RegB(0x50e2,0x00);

   SetA8RegW(0x5062, 0x00);
   SetA8RegW(0x5082, 0x00);

   return A8_NO_ERROR;

}

u_char A800_HostInitLCD(void)
{
   SetA8RegB(0x5002,(unsigned char)gMainPanelControlAttrib.PIXEL_CLOCK_DIV);

   SetA8RegB(0x5042,NO_DUAL_PANEL_PREVIEW|
            LCD1ACTIVE |
            PANEL1_RGB);
   
   SetA8RegW(0x5044, gMainPanelControlAttrib.LCD_BusCtl.Width);
   SetA8RegW(0x5046, gMainPanelControlAttrib.LCD_BusCtl.Height);

   return A8_NO_ERROR;

}


//======================================================================//

void A8_WriteLcdCommand28(u_short command)
{
	u_short val;
	SetA8RegW(0x5106, command>>8);
	SetA8RegB(0x500a, 0x04);
	do
	{
        val=GetA8RegB(0x500a);
        }while(val & 0x0004);
	
	SetA8RegW(0x5106, command &0xFF);
	SetA8RegB(0x500a, 0x04);
	do
	{
        val=GetA8RegB(0x500a);
       }while(val & 0x0004);
}

void A8_WriteLcdData28(u_short Data)
{
	//u_short val;
	SetA8RegW(0x5104, Data>>8);
	SetA8RegB(0x500a, 0x08);
	SetA8RegW(0x5104, Data &0xFF);
	SetA8RegB(0x500a,  0x08);
}


u_char A701_MainPreviewSetting()
{
	A8L_SetWindowAttribute( A8_MAIN_WINDOW, gMainPanelControlAttrib.LCD_BusCtl.Width, gMainPanelControlAttrib.LCD_BusCtl.Height, A8_DEPTH_16B, 0xA000);
	A8L_SetWindowToDisplay( A8_MAIN_WINDOW, 0, 0, gMainPanelControlAttrib.LCD_BusCtl.Width, gMainPanelControlAttrib.LCD_BusCtl.Height, 0, 0, A8_RT_NORMAL );

	A8L_SetWindowPriority( A8_MAIN_WINDOW, A8_PIP_WINDOW, 0, 0 );
	A8L_SetWindowActive( A8_MAIN_WINDOW, 1 );
	
	return A8_NO_ERROR;
}


u_char A800_UsingLCD1()
{
	u_short temp;
	s_int timeout;
	
	temp = GetA8RegB(0x500A);
	if ( (temp & 0x01) )
		{
		SetA8RegB(0x5013, 1);
		SetA8RegB(0x500A,(temp & 0xFE));
		timeout = 0;
		while ((GetA8RegB(0x5013)&0x01) && timeout < 0x10000 )
			timeout++;
		}
	else if ( (temp & 0x02) )
		{
		timeout = 0;
		while ((GetA8RegB(0x500A)&0x02) && timeout < 0x10000 )
			timeout++;
		if (timeout>= 0x10000)
			SetA8RegB(0x500A, 0x00);
		}
	SetA8RegB(0x5042,NO_DUAL_PANEL_PREVIEW|LCD1ACTIVE|PANEL1_RGB);
	//SendA8Cmd(A8_HOST_CMD_SET_ACTIVE_LCD + (0<<8) );

	return A8_NO_ERROR;
}



#ifndef A8_DISABLE_SUB_LCD_FUNCTION
u_char A800_HostInitLCD_P2(void)
{

	SetA8RegB(0x5002,(unsigned char)gMainPanelControlAttrib.PIXEL_CLOCK_DIV);		// grobal

	SetA8RegB(0x5042,NO_DUAL_PANEL_PREVIEW|
	      LCD2ACTIVE |
	      PANEL1_RGB);

	SetA8RegW(0x5044,gMainPanelControlAttrib.LCD_BusCtl.Width);
	SetA8RegW(0x5046,gMainPanelControlAttrib.LCD_BusCtl.Height);

	return A8_NO_ERROR;

}

u_char A800_SubPreviewSetting()
{

	
	SetA8RegB(0x5150, 0x00);
	SetA8RegB(0x5151, 0xD0);
	SetA8RegB(0x5152, 0x00);

	SetA8RegW(0x5162,0x00);		// start offset
	SetA8RegB(0x5164 ,0x00);

	SetA8RegW(0x5156, 0x00);	// x, y
	SetA8RegW(0x5158, 0x00);

	SetA8RegW(0x515E, gMainPanelControlAttrib.LCD_BusCtl.Width);
	SetA8RegW(0x5160, gMainPanelControlAttrib.LCD_BusCtl.Height);


	SetA8RegW(0x515C, gMainPanelControlAttrib.LCD_BusCtl.Width);	// p, r off
	SetA8RegW(0x515A, 1);

	SetA8RegB(0x5170, 0x00); 
	SetA8RegB(0x5171, 0x00);
	SetA8RegB(0x5172, 0x00);

	SetA8RegW(0x5182, 0x00);	// start off
	SetA8RegB(0x5184 ,0x00);

	SetA8RegW(0x5176, 0x00);	// x, y
	SetA8RegW(0x5178, 0x00);

	SetA8RegW(0x517E, gMainPanelControlAttrib.LCD_BusCtl.Width);
	SetA8RegW(0x5180, gMainPanelControlAttrib.LCD_BusCtl.Height);

	SetA8RegW(0x517C,gMainPanelControlAttrib.LCD_BusCtl.Width);

	SetA8RegW(0x517A, 1);
	
  	SetA8RegB(0x5141, 0x01);
  	SetA8RegB(0x5143, 0x04);

   return A8_NO_ERROR;

}


u_char A800_SubRotatePreviewSetting()
{

	
	SetA8RegB(0x5150, 0x00);
	SetA8RegB(0x5151, 0xD0);
	SetA8RegB(0x5152, 0x00);

	SetA8RegW(0x5162, 0x17A0 + 95);
	SetA8RegB(0x5164 ,0x00);

	SetA8RegW(0x5156, 0x00);
	SetA8RegW(0x5158, 0x00);

	SetA8RegW(0x515E, gMainPanelControlAttrib.LCD_BusCtl.Width);
	SetA8RegW(0x5160, gMainPanelControlAttrib.LCD_BusCtl.Height);


	SetA8RegW(0x515C, gMainPanelControlAttrib.LCD_BusCtl.Width + 0x8000);

	SetA8RegW(0x515A, 1 + 0x8000);

	SetA8RegB(0x5170, 0x00); 
	SetA8RegB(0x5171, 0x00);
	SetA8RegB(0x5172, 0x00);

	SetA8RegW(0x5182, 0x17A0 + 95);
	SetA8RegB(0x5184 ,0x00);

	SetA8RegW(0x5176, 0x00);
	SetA8RegW(0x5178, 0x00);

	SetA8RegW(0x517E, gMainPanelControlAttrib.LCD_BusCtl.Width);
	SetA8RegW(0x5180, gMainPanelControlAttrib.LCD_BusCtl.Height);


	SetA8RegW(0x517C, gMainPanelControlAttrib.LCD_BusCtl.Width + 0x8000);

	SetA8RegW(0x517A, 1 + 0x8000);

    return A8_NO_ERROR;

}

u_char A701_SubPreviewSetting()
{

	A8L_SetWindowAttribute( A8_MAIN_WINDOW, gMainPanelControlAttrib.LCD_BusCtl.Width, gMainPanelControlAttrib.LCD_BusCtl.Height, A8_DEPTH_16B, 0xD000);
	A8L_SetWindowToDisplay( A8_MAIN_WINDOW, 0, 0, gMainPanelControlAttrib.LCD_BusCtl.Width, gMainPanelControlAttrib.LCD_BusCtl.Height, 0, 0, A8_RT_NORMAL );

	A8L_SetWindowPriority( A8_MAIN_WINDOW, A8_PIP_WINDOW, 0, 0 );
	A8L_SetWindowActive( A8_MAIN_WINDOW, 1 );
	A8L_SetWindowActive( A8_PIP_WINDOW, 1 );
	
	return A8_NO_ERROR;

}


u_char A701_SubRotatePreviewSetting()
{


	
	A8L_SetWindowAttribute( A8_MAIN_WINDOW, gMainPanelControlAttrib.LCD_BusCtl.Width, gMainPanelControlAttrib.LCD_BusCtl.Height, A8_DEPTH_16B, 0xD000);
	A8L_SetWindowToDisplay( A8_MAIN_WINDOW, 0, 0, gMainPanelControlAttrib.LCD_BusCtl.Width, gMainPanelControlAttrib.LCD_BusCtl.Height, 0, 0, A8_RT_RIGHT_180 );

	A8L_SetWindowPriority( A8_MAIN_WINDOW, A8_PIP_WINDOW, 0, 0 );
	A8L_SetWindowActive( A8_MAIN_WINDOW, 1 );
	A8L_SetWindowActive( A8_PIP_WINDOW, 1 );
	
	return A8_NO_ERROR;

}


u_char 	A800_PreviewSub(s_short VFlip)
{

 	
	if ( VFlip == 0 )
		A701_SubPreviewSetting();	
	else
		A701_SubRotatePreviewSetting();	
 	A8L_ConfigPreviewZoomParam( 4, 1 );
	A8L_SetCdspPreviewResolution(640, 480, 96, 96,
				4,16, 10);	// set AIT800 scale
 	SetA8RegW( 0x5186, 0x0000 & 0x0FFFF );
	SetA8RegB( 0x5188, 0xA000 >> 16 );
	
 	SetA8RegW(0x6500, 0xD000 >> 3);	// JPEG start address
	SetA8RegW(0x6502, 0xA000 >> 3);

 	A8L_SetCamMode(A8_STATE_STILL_PREVIEW);
	A8L_StartLCDPreview();
	
 	return A8_NO_ERROR;
}
u_char A800_UsingLCD2()
{
	u_short temp;//,x,y;
	s_int timeout;
	
	temp = GetA8RegB(0x500A);
	if ( (temp & 0x01) )
		{
		SetA8RegB(0x5013, 1);
		SetA8RegB(0x500A,(temp & 0xFE));
		timeout = 0;
		while ((GetA8RegB(0x5013)&0x01) && timeout < 0x10000 )
			timeout++;
		}
	else if ( (temp & 0x02) )
		{
		timeout = 0;
		while ((GetA8RegB(0x500A)&0x02) && timeout < 0x10000 )
			timeout++;
		if (timeout>= 0x10000)
			SetA8RegB(0x500A, 0x00);
		}
	
	SetA8RegB(0x5042,NO_DUAL_PANEL_PREVIEW|LCD2ACTIVE|PANEL1_RGB);

	SetA8RegB(0x500a, 0x00);
	SetA8RegB(0x503a, TYPE_80 | POLARITY_ENABLE | PHASE_ENABLE | NON_BURST_MODE | DATA_BUS_16);
	SetA8RegB(0x503b, 0x00);

    
	return A8_NO_ERROR;

}
#endif

u_char A800_UsingDualLCD()
{
	SetA8RegB(0x5042,DUAL_PANEL_PREVIEW|
					  LCD2ACTIVE |
					  PANEL1_RGB);
	return A8_NO_ERROR;
}


//------------------------------------------------------------------------------------------------
// The following functions are some specific panels control functions and will be customized.
//
//------------------------------------------------------------------------------------------------
void A8L_SendLCDCommand(u_short Command)
{
   s_int timeout;
   u_short val;

   SetA8RegW(0x5004, Command);
   SetA8RegB(0x500a, 0x08);
   timeout = 0;
   do
   {
      val=GetA8RegB(0x500a);
      timeout++;
   }while((val & 0x08) &&(timeout < A8_CMD_READY_TIME_OUT*2));
  sys_IF_ait_delay1ms(10);
}
void A8L_SendLCDIndex(u_short Index)
{
   s_int timeout;
   u_short val;

   SetA8RegW(0x5006, Index);
   SetA8RegB(0x500a, 0x04);
   timeout = 0;
   do
   {
      val=GetA8RegB(0x500a);
      timeout++;
   }while((val & 0x08) &&(timeout < A8_CMD_READY_TIME_OUT*2));
   sys_IF_ait_delay1ms(10);
}
s_short A8L_SendLCDIndexCommand8B( u_short index, u_short command )
{
	return A8_NO_ERROR;
}


s_short A8L_SendLCDIndexCommand( u_short index, u_short command )
{
	s_int timeout;
	u_short val;
	
	SetA8RegW( 0x5006, index );
	SetA8RegB( 0x500A, 0x04 );
	timeout = 0;
	do
	{
	   val=GetA8RegB(0x500a);
	   timeout++;
	}while ((val & 0x04)&&(timeout < A8_CMD_READY_TIME_OUT*2 ));

	SetA8RegW( 0x5004, command );
	SetA8RegB( 0x500A, 0x08 );
	timeout = 0;
	do
	{
	   val=GetA8RegB(0x500a);
	   timeout++;
	}while ((val & 0x08)&&(timeout < A8_CMD_READY_TIME_OUT*2 ));
	sys_IF_ait_delay1ms(10);
	
	return A8_NO_ERROR;
}
