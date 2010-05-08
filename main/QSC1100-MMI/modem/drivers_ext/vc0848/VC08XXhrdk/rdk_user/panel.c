
#include "mmp_rdk.h"


typedef void (*LcdSetReg)(MMD_U08 Status, MMD_U32 Byte_Length);
typedef void (*LcdSetRS)(MMD_U16 val);

LcdSetRS LCD_SetRSHigh;
LcdSetRS LCD_SetRSLow;

#if(MMD_LCD_PANEL_TYPE == LCD_ILI9325_CPT24_16BIT)  
void  MLcdSetReg(int adr,int val)
{
	if(adr==0xffff)
	{
		LCD_SetRSHigh((MMD_U16)val);		

		return;
	}	
	if(val==0xffff)
	{
		LCD_SetRSLow((MMD_U16)adr);

		return;
	}
	LCD_SetRSLow((MMD_U16)adr);

	LCD_SetRSHigh((MMD_U16)val);

}


static LCDReg gMLCD_powon[] =
{

	{0x00E3, 0x3008,0},       
	{0x00E7, 0x0012,0},       
	{0x00EF, 0x1231,0},       
	{0x0001, 0x0100,0},       
	{0x0002, 0x0700,0},       
	{0x0003, 0x1030,0},       
	{0x0004, 0x0000,0},       
	{0x0008, 0x0202,0},       
	{0x0009, 0x0000,0},       
	{0x000A, 0x0000,0},       
	{0x000C, 0x0000,0},       
	{0x000D, 0x0000,0},       
	{0x000F, 0x0000,0},       
	                          
	{0x0010, 0x0000,0},       
	{0x0011, 0x0007,0},       
	{0x0012, 0x0000,0},       
	{0x0013, 0x0000,200},       
	                          
	{0x0010, 0x1690,0},       
	{0x0011, 0x0227,50},       
	                          
	{0x0012, 0x001b,50},       
	                          
	{0x0013, 0x1800,0},       
	{0x0029, 0x0021,0},       
	//{0x002B, 0x0008,50},       
	{0x002B, 0x000f,0},
	                          
	{0x0020, 0x0000,0},       
	{0x0021, 0x0000,0},  
	                          
	{0x0030, 0x0000,0},       
	{0x0031, 0x0507,0},       
	{0x0032, 0x0004,0},       
	{0x0035, 0x0205,0},       
	{0x0036, 0x0004,0},       
	{0x0037, 0x0307,0},       
	{0x0038, 0x0002,0},       
	{0x0039, 0x0707,0},       
	{0x003C, 0x0502,0},       
	{0x003D, 0x0004,0},       


//-------------------//
/*	{0x0030, 0x0000,0},       
	{0x0031, 0x0607,0},       
	{0x0032, 0x0305,0},       
	{0x0035, 0x0000,0},       
	{0x0036, 0x1604,0},       
	{0x0037, 0x0204,0},       
	{0x0038, 0x0001,0},       
	{0x0039, 0x0707,0},       
	{0x003C, 0x0000,0},       
	{0x003D, 0x000F,0},       */

//-------------------//

	{0x0050, 0x0000,0},       
	{0x0051, 0x00EF,0},       
	{0x0052, 0x0000,0},       
	{0x0053, 0x013F,0},       
	{0x0060, 0xA700,0},   
	{0x0061, 0x0001,0},       
	{0x006A, 0x0000,0},       

	{0x0080, 0x0000,0},  
	{0x0081, 0x0000,0},       
	{0x0082, 0x0000,0},       
	{0x0083, 0x0000,0},       
	{0x0084, 0x0000,0},       
	{0x0085, 0x0000,0},       
	                          
	{0x0090, 0x0010,0},       
	{0x0092, 0x0000,0},       
	{0x0093, 0x0003,0},       
	{0x0095, 0x0110,0},       	
	{0x0097, 0x0000,0},       
	{0x0098, 0x0000,0},       
	
	{0x0007, 0x0133,0}        


};

LCDParm MLcdParm =
{
	{ sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},
	{ 0, NULL },
	{ 0, NULL },			
	{ 0, NULL }	
 };

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
	unsigned short  i = 0, j = 0, pos = 0;

	pos = (( sy << 8 ) & 0xff00 ) | ( sx & 0x00ff );


	MLcdSetReg(0x20,0x0000);	
	MLcdSetReg(0x21,0x0000);

	MLcdSetReg(0x0022,0xffff);

	for( j = 0; j < height; j++ )
	{	


		for( i = 0; i < width; i++ )
		{
  			LCD_SetRSHigh( ((unsigned int)color ));
		}
		sy ++;
	}
}

void pnl_draw_hline( unsigned short line, unsigned int color )
{
	unsigned short i = 0;
		
	MLcdSetReg(0x20,0x00);
	
	MLcdSetReg(0x21,line);
	
	MLcdSetReg(0x22,0xffff);
	for( i = 0; i < (240); i++ )
	{	
  		LCD_SetRSHigh( ((unsigned int)color));
		//vm_set_reg( REG_LCDIF_EXRW_EN, BIT1 );
	}
}

void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
	unsigned short  i = 0, j = 0, pos = 0;
	unsigned int color;

	pos = (( sy << 8 ) & 0xff00 ) | ( sx & 0x00ff );


	MLcdSetReg(0x20,0x0000);	
	MLcdSetReg(0x21,0x0000);

	MLcdSetReg(0x0022,0xffff);


	for( j = 0; j < height; j++ )
	{
		
		for( i = 0; i < width; i++ )
		{
	
		
			color= (pbuf[1]<<8)|pbuf[0];
			pbuf+=0x02;		
  			LCD_SetRSHigh( ((unsigned int)color ));

	 			
		}
		sy ++;
	}
	return;

}

//void LcdRotate_ILI9325_CPT24_16BIT(UINT8 degree, UINT8 WorkMode) 
void Lcd_Rotate(UINT8 degree)
{ 
       switch(degree) 
        { 
            case VIM_HAPI_ROTATE_0: 
	               VIM_LCDIF_RsLExW((UINT16)0x0001); 
	               VIM_LCDIF_RsHExW((UINT16)0x0100); 
	            	
	               VIM_LCDIF_RsLExW((UINT16)0x0003); 
	               VIM_LCDIF_RsHExW((UINT16)0x1030); 

			 VIM_LCDIF_RsLExW((UINT16)0x0060); 
	               VIM_LCDIF_RsHExW((UINT16)0xa700);		

	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );            
                break; 
                
            case VIM_HAPI_ROTATE_90: 
            		VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//0x60=0xa700, gs=1
           	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x1028);		//效果是逆时针转90度的图像

               	VIM_LCDIF_RsLExW((UINT16)0x0060); 
                     VIM_LCDIF_RsHExW((UINT16)0xa700);		
       		
                     VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
                     
   
                break; 
                
               case VIM_HAPI_ROTATE_180: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//0x60=0xa700, gs=1
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1010);		
  
            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                    	VIM_LCDIF_RsHExW((UINT16)0xa700);	
                    	
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );       

   		 break; 
                
 		case VIM_HAPI_ROTATE_270: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//0x60=0xa700, gs=1
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x1028);		//ok,but 90 mirror
               		

            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                 	VIM_LCDIF_RsHExW((UINT16)0x2700);	

                	
                	VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

                break; 

  		case VIM_HAPI_MIRROR_0: 
			VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//0x60=0xa700, gs=1
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1030);		

            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                    	VIM_LCDIF_RsHExW((UINT16)0x2700);	

	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );                     	
		 break; 

               case VIM_HAPI_MIRROR_90: 
            		VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//0x60=0xa700, gs=1
           	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x1028);		//效果是逆时针转90度的图像

               	VIM_LCDIF_RsLExW((UINT16)0x0060); 
                     VIM_LCDIF_RsHExW((UINT16)0x2700);		//上下颠倒	
       		
                     VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
  
                break;   

               case VIM_HAPI_MIRROR_180: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)	//左右颠倒
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//0x60=0xa700, gs=1
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1010);		
  
            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                    	VIM_LCDIF_RsHExW((UINT16)0x2700);	
                    	
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );       
               	
                 break; 
        
  		case VIM_HAPI_MIRROR_270: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ss=0(bit8),sm=0(bit10)
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//0x60=0xa700, gs=1
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x1028);		//ok,but 90 mirror
               		

            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                 	VIM_LCDIF_RsHExW((UINT16)0xa700);		//上下颠倒

                	
                	VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

                break; 


               
             
            default: 
                break; 
        }
return; 

} 

#endif

#if(MMD_LCD_PANEL_TYPE == LCD_ILI9320_16BIT)	

void  MLcdSetReg(int adr,int val)
{
	if(adr==0xffff)
	{
		LCD_SetRSHigh((MMD_U16)val);

		return;
	}	
	if(val==0xffff)
	{
		LCD_SetRSLow((MMD_U16)adr);

		return;
	}
	LCD_SetRSLow((MMD_U16)adr);

	LCD_SetRSHigh((MMD_U16)val);

}



void Lcd_Rotate(UINT8 degree)
{
	switch(degree)
	{
		case VIM_HAPI_ROTATE_0:
                     LCDIF_SetRSLow_normal((UINT16)0x0003); 
			LCDIF_SetRSHigh_normal((UINT16)0x1030);
                     LCDIF_SetRSLow_normal((UINT16)0x0001); 
                     LCDIF_SetRSHigh_normal((UINT16)0x011c); 
                     MMD_USER_WaitMICRS(100);
	
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (3<<16)|0x0807);
                     MMD_USER_WaitMICRS(100);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
                     MMD_USER_WaitMICRS(200);
			break;
              #if 0
		case VIM_HAPI_ROTATE_90:
			LCDIF_SetRSLow_normal((UINT16)0x03);
			LCDIF_SetRSHigh_normal((UINT16)0x1028);
			
			LCDIF_SetRSLow_normal((UINT16)0x60);
			LCDIF_SetRSHigh_normal((UINT16)0xa700);// for 320x240 set 0xa700, for 240x320 set 0x2700, by lizg


			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x3);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

	
			break;
		#endif
            case VIM_HAPI_ROTATE_90:
                     LCDIF_SetRSLow_normal((UINT16)0x0003); 
                     LCDIF_SetRSHigh_normal((UINT16)0x1028); 
                     
                     LCDIF_SetRSLow_normal((UINT16)0x0001); 
                     LCDIF_SetRSHigh_normal((UINT16)0x001c); 
                     VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS, (0x0807<<16)|0x3);      
                     VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, (0<<16)|0x0);  
                     break;

		case VIM_HAPI_ROTATE_180:
			LCDIF_SetRSLow_normal((UINT16)0xbc<<8);
			LCDIF_SetRSHigh_normal((UINT16)0x03<<8);

			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (4<<16)|0x0608);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (12<<16)|0x0e10);

			
			break;
		case VIM_HAPI_MIRROR_180:
			LCDIF_SetRSLow_normal((UINT16)0xbc<<8);
			LCDIF_SetRSHigh_normal((UINT16)0x01<<8);
	
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (4<<16)|0x0608);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (12<<16)|0x0e10);

			break;
		case VIM_HAPI_ROTATE_270:
			LCDIF_SetRSLow_normal((UINT16)0xbc<<8);
			LCDIF_SetRSHigh_normal((UINT16)0x0a<<8);

			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0608<<16)|4);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0x0e10<<16)|12);

			
			break;
			
		case VIM_HAPI_MIRROR_0:
			LCDIF_SetRSLow_normal((UINT16)0xbc<<8);
			LCDIF_SetRSHigh_normal((UINT16)0x02<<8);

			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (4<<16)|0x0608);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (12<<16)|0x0e10);

			break;
			
		case VIM_HAPI_MIRROR_90:
			LCDIF_SetRSLow_normal((UINT16)0xbc<<8);
			LCDIF_SetRSHigh_normal((UINT16)0x08<<8);

			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0608<<16)|4);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0x0e10<<16)|12);			
			break;
			
		
			
		case VIM_HAPI_MIRROR_270:
			LCDIF_SetRSLow_normal((UINT16)0xbc<<8);
			LCDIF_SetRSHigh_normal((UINT16)0x0b<<8);
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0608<<16)|4);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0x0e10<<16)|12);

			break;
			
		default:
			break;
	}

	return;	
}




static LCDReg gMLCD_powon[] =
{
#if 0 //屏亮参数
	{0x0000, 0x0001,50},		// Start internal OSC.
	{0x0001, 0x0100,0},		// bit 8:the shift direction  of outputs is from s720 to s0;bit10:SM combined with GS(0x60H)
	{0x0002, 0x0400,0},		// set frame/field inversion
	{0x0003, 0x1030,0},		// order:B-G-R,16bit system interface,16bits/pixel
	
	{0x0004, 0x0000,0},		//
	{0x0008, 0x0202,0},		// set the back porch and front porch
	{0x0009, 0x0000,0},		// normal scan
	{0x000A, 0x0000,0},		// FMARK function disable
	{0x000C, 0x0000,0},		// RGB interface setting:display still pictures
	{0x000D, 0x0000,0},		// Frame marker Position
	{0x000F, 0x0000,0},		// RGB interface polarity

	{0x0010, 0x0000,0},		// SAP, BT[3:0], AP, DSTB, SLP, STB
	{0x0011, 0x0007,0},		// DC1[2:0], DC0[2:0], VC[2:0]
	{0x0012, 0x0000,0},		// VREG1OUT voltage
	{0x0013, 0x0000,50},		// VDV[4:0] for VCOM amplitude
	
	{0x0010, 0x17B0,0},		// SAP, BT[3:0], AP, DSTB, SLP, STB
	{0x0011, 0x0001,50},		// DC1[2:0], DC0[2:0], VC[2:0]
	
	{0x0012, 0x013C,50},		// VREG1OUT voltage
	
	{0x0013, 0x1300,0},		// VDV[4:0] for VCOM amplitude
	{0x0029, 0x0004,50},		// VCM[4:0] for VCOMH
	

	{0x0020, 0x0000,0},		// GRAM horizontal Address
	{0x0021, 0x0000,0},		// GRAM Vertical Address
	{0x002B, 0x0020,0},		// 110hz 262k color

	{0x0030, 0x0000,0},		// Gamma Control
	{0x0031, 0x0306,0},		// Gamma Control
	{0x0032, 0x0200,0},		// Gamma Control
	{0x0035, 0x0107,0},		// Gamma Control
	{0x0036, 0x0404,0},		// Gamma Control
	{0x0037, 0x0606,0},		// Gamma Control
	{0x0038, 0x0105,0},		// Gamma Control
	{0x0039, 0x0707,0},		// Gamma Control
	{0x003C, 0x0600,0},		// Gamma Control
	{0x003D, 0x0807,0},		// Gamma Control

	{0x0050, 0x0000,0},		// Horizontal GRAM Start Address-----HSA[7:0]
	{0x0051, 0x00EF,0},		// Horizontal GRAM End Address-----HEA[7:0]
	{0x0052, 0x0000,0},		// Vertical GRAM Start Address-----VSA[8:0]
	{0x0053, 0x013F,0},		// Vertical GRAM Start Address-----VEA[8:0]
	
	{0x0060, 0x2700,0},		//320lines GS=0
	
	{0x0061, 0x0001,0},		// NDL,VLE, REV
	{0x006A, 0x0000,0},		// VL[8:0]
	
	{0x0080, 0x0000,0},		// Partial Image 1 Display Position-----PTDP0[8:0]
	{0x0081, 0x0000,0},		// Partial Image 1 Start Address-----PTSA0[8:0]
	{0x0082, 0x0000,0},		// Partial Image 1 End Address-----PTEA0[8:0]
	{0x0083, 0x0000,0},		// Partial Image 2 Display Position-----PTDP1[8:0]
	{0x0084, 0x0000,0},		// Partial Image 2 Start Address-----PTSA1[8:0]
	{0x0085, 0x0000,0},		// Partial Image 2 Start Address-----PTEA1[8:0]
	
	{0x0090, 0x0013,0},		// Panel Interface Control 1-----DIVI[1:0], RTNI[4:0]
	{0x0092, 0x0000,0},		// Panel Interface Control 2-----NOWI[2:0]
	{0x0093, 0x0003,0},		// Panel Interface Control 3-----MCPI[2:0]
	{0x0095, 0x0110,0},		// Panel Interface Control 4-----DIVE[1:0], RTNE[5:0]
	{0x0097, 0x0000,0},		// Panel Interface Control 5-----NOWE[3:0]
	{0x0098, 0x0000,0},		// Panel Interface Control 6-----MCPE[2:0]
	
	{0x0007, 0x0001,50},		// Display Control 1
	
	{0x0007, 0x0021,0},		// Display Control 1
	{0x0007, 0x0023,0},		// Display Control 1
	
	{0x0007, 0x0173,0},		// 262K color and display ON
#else
    {0x00e5, 0x8000,0},
    {0x0000, 0x0001,0},     // Start internal OSC.
    {0x002b, 0x0010,0},
    {0x0001, 0x0100,0},     // bit 8:the shift direction  of outputs is from s720 to s0;bit10:SM combined with GS(0x60H)
    {0x0002, 0x0700,0},     // set frame/field inversion
    //{0x0003, 0x1030,0},       // order:B-G-R,16bit system interface,16bits/pixel
    {0x0003, 0x1230,0},     //set GRAM write direction and BGR=1. 
    {0x0004, 0x0000,0},     //// Resize register 
    {0x0008, 0x0202,0},     // set the back porch and front porch
    {0x0009, 0x0000,0},     // set non-display area refresh cycle ISC[3:0]
    {0x000A, 0x0000,0},     // FMARK function disable
    {0x000C, 0x0000,0},     // RGB interface setting:display still pictures
    {0x000D, 0x0000,0},     // Frame marker Position
    {0x000F, 0x0000,0},     // RGB interface polarity
    {0x0050, 0x0000,0},     // Horizontal GRAM Start Address-----HSA[7:0]
    {0x0051, 0x00EF,0},     // Horizontal GRAM End Address-----HEA[7:0]
    {0x0052, 0x0000,0},     // Vertical GRAM Start Address-----VSA[8:0]
    {0x0053, 0x013F,0},     // Vertical GRAM Start Address-----VEA[8:0]
    {0x0060, 0x2700,0},     //320lines GS=0
    {0x0061, 0x0001,0},     // NDL,VLE, REV
    {0x006A, 0x0000,0},     // set scrolling line
    {0x0080, 0x0000,0},     // Partial Image 1 Display Position-----PTDP0[8:0]
    {0x0081, 0x0000,0},     // Partial Image 1 Start Address-----PTSA0[8:0]
    {0x0082, 0x0000,0},     // Partial Image 1 End Address-----PTEA0[8:0]
    {0x0083, 0x0000,0},     // Partial Image 2 Display Position-----PTDP1[8:0]
    {0x0084, 0x0000,0},     // Partial Image 2 Start Address-----PTSA1[8:0]
    {0x0085, 0x0000,0},     // Partial Image 2 Start Address-----PTEA1[8:0]
    {0x0090, 0x0010,0},     // Panel Interface Control 1-----DIVI[1:0], RTNI[4:0]
    {0x0092, 0x0000,0},     // Panel Interface Control 2-----NOWI[2:0]
    {0x0093, 0x0003,0},     // Panel Interface Control 3-----MCPI[2:0]
    {0x0095, 0x0110,0},     // Panel Interface Control 4-----DIVE[1:0], RTNE[5:0]
    {0x0097, 0x0000,0},     // Panel Interface Control 5-----NOWE[3:0]
    {0x0098, 0x0000,0},     // Panel Interface Control 6-----MCPE[2:0]
    {0x0010, 0x0000,0},     // SAP, BT[3:0], AP, DSTB, SLP, STB
    {0x0011, 0x0000,0},     // DC1[2:0], DC0[2:0], VC[2:0]
    {0x0012, 0x0000,0},     // VREG1OUT voltage
    {0x0013, 0x0000,200},       // VDV[4:0] for VCOM amplitude
    {0x0010, 0x17B0,0},     // SAP, BT[3:0], AP, DSTB, SLP, STB
    {0x0011, 0x0000,50},        // DC1[2:0], DC0[2:0], VC[2:0]
    {0x0012, 0x013e,50},        // VREG1OUT voltage
    {0x0013, 0x1c00,0},     // VDV[4:0] for VCOM amplitude
    {0x0029, 0x000a,50},        // VCM[4:0] for VCOMH
    {0x0020, 0x0000,0},     // GRAM horizontal Address
    {0x0021, 0x0000,0},     // GRAM Vertical Address
    {0x0030, 0x0004,0},     // Gamma Control
    {0x0031, 0x0007,0},     // Gamma Control
    {0x0032, 0x0100,0},     // Gamma Control
    {0x0035, 0x0007,0},     // Gamma Control
    {0x0036, 0x0400,0},     // Gamma Control
    {0x0037, 0x0700,0},     // Gamma Control
    {0x0038, 0x0005,0},     // Gamma Control
    {0x0039, 0x0707,0},     // Gamma Control
    {0x003C, 0x0000,0},     // Gamma Control
    {0x003D, 0x0400,0},     // Gamma Control

    {0x0007, 0x0173,0},     // 262K color and display ON

#endif
};

LCDParm MLcdParm =
{
	{ sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},
	{ 0, NULL },
	{ 0, NULL },			
	{ 0, NULL }	
 };

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
	unsigned short  i = 0, j = 0, pos = 0;

	pos = (( sy << 8 ) & 0xff00 ) | ( sx & 0x00ff );

	MLcdSetReg(0x20,sx);
	MLcdSetReg(0x21,sy);

	for( j = 0; j < height; j++ )
	{		
		MLcdSetReg(0x20,sx);
		MLcdSetReg(0x21,sy);
		MLcdSetReg(0x22,0xffff);
		
		for( i = 0; i < width; i++ )
		{
  			LCD_SetRSHigh( ((unsigned int)color ));
		}
		sy ++;
	}
}

void pnl_draw_hline( unsigned short line, unsigned int color )
{
	unsigned short i = 0;
		
	MLcdSetReg(0x20,0x00);
	
	MLcdSetReg(0x21,line);
	
	MLcdSetReg(0x22,0xffff);
	for( i = 0; i < (240); i++ )
	{	
  		LCD_SetRSHigh( ((unsigned int)color));
		//vm_set_reg( REG_LCDIF_EXRW_EN, BIT1 );
	}
}

void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
	unsigned short  i = 0, j = 0, pos = 0;
	unsigned int color;

	pos = (( sy << 8 ) & 0xff00 ) | ( sx & 0x00ff );

	MLcdSetReg(0x20,sx);
	MLcdSetReg(0x21,sy);

	for( j = 0; j < height; j++ )
	{
		MLcdSetReg(0x20,sx);
		MLcdSetReg(0x21,sy);
		MLcdSetReg(0x22,0xffff);
		
		for( i = 0; i < width; i++ )
		{
			color= (pbuf[1]<<8)|pbuf[0];
			pbuf+=0x02;		
  			LCD_SetRSHigh( ((unsigned int)color ));
		}
		sy ++;
	}
	return;

}


#elif(MMD_LCD_PANEL_TYPE == LCD_HX8347_18BIT)	
void  MLcdSetReg(int adr,int val)

{

    if(adr==0xffff)

    {

        LCD_SetRSHigh((MMD_U16)val);        

 

        return;

    }   

    if(val==0xffff)

    {

        LCD_SetRSLow((MMD_U16)adr);

 

        return;

    }

    LCD_SetRSLow((MMD_U16)adr);

 

    LCD_SetRSHigh((MMD_U16)val);

 

}

 
 void Lcd_Rotate(UINT8 degree)
 {
	 switch (degree) {	 
	 case VIM_HAPI_ROTATE_180: 
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0008);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 break;  
	 case VIM_HAPI_ROTATE_90:	 
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0068);	 // 0x38
		 LCDIF_SetRSLow_normal((unsigned short)0x04); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 break;  
	 case VIM_HAPI_ROTATE_0:	 
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00c8);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04);
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05);
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 break;  
	 case VIM_HAPI_ROTATE_270:	 
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00a8);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 break;  
	 case VIM_HAPI_MIRROR_0: 
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0048);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08);
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 break;  
	 case VIM_HAPI_MIRROR_90:
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00e8);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04);
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 break;  
	 case VIM_HAPI_MIRROR_180:	 
		 LCDIF_SetRSLow_normal((unsigned short)0x16); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0088);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 break;  
	 case VIM_HAPI_MIRROR_270:	 
		 LCDIF_SetRSLow_normal((unsigned short)0x16);
		 LCDIF_SetRSHigh_normal((unsigned short)0x0028);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x04); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0001);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x05); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x003f);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x08); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x0000);	 
		 LCDIF_SetRSLow_normal((unsigned short)0x09); 
		 LCDIF_SetRSHigh_normal((unsigned short)0x00ef);	 
		 break;  
	 default:	 
		 break;  
	 }
	 
	 return; 
 }

 

static LCDReg gMLCD_powon[] =

{

#if 1

    {0x0046,0x0095,0},

    {0x0047,0x0051,0},

    {0x0048,0x0000,0},

    {0x0049,0x0036,0},

    {0x004A,0x0011,0},

    {0x004B,0x0066,0},

    {0x004C,0x0014,0}, 

    {0x004D,0x0077,0}, 

    {0x004E,0x0013,0}, 

    {0x004F,0x004C,0}, 

    {0x0050,0x0046,0}, 

    {0x0051,0x0046,0}, 

            

     

    {0x0002,0x0000,0},  

    {0x0003,0x0000,0},  

    {0x0004,0x0000,0},  

    {0x0005,0x00EF,0}, 

    {0x0006,0x0000,0}, 

    {0x0007,0x0000,0}, 

    {0x0008,0x0001,0}, 

    {0x0009,0x003F,0}, 

 

 

    {0x0001,0x0006,0},  

    //{0x0016,0x0048,0},  //0x00c8

    {0x0016,0x00c8,0},

 

    {0x0021,0x0000,0},

    {0x0023,0x0095,0},

    {0x0024,0x0095,0},

    {0x0025,0x00FF,0},

    {0x0027,0x0006,0},

    {0x0028,0x0006,0},

    {0x0029,0x0006,0},

    {0x002A,0x0006,0},

    {0x002C,0x0006,0},

    {0x002D,0x0006,0},

 

    {0x003A,0x0001,0}, 

    {0x003B,0x0001,0}, 

    {0x003C,0x00F0,0}, 

    {0x003D,0x0000,50}, 

 

    {0x0010,0x00A6,0}, 

 

    {0x0035,0x0038,0},

    {0x0036,0x0078,0},

    {0x003E,0x0038,0},

    {0x0040,0x000F,0},

    {0x0041,0x00F0,0},

 

    //{0x0070,0x0048,0},//#ifdef __SYNC_LCM_SUPPORT__

    //{0x0070,0x000e,0},

    {0x0070,0x000d,0},

    

    {0x0019,0x0039,0},  

    {0x0093,0x000C,30},  

 

    {0x0020,0x0040,0},

    {0x001D,0x0007,0},

    {0x001E,0x0000,0},

    {0x001F,0x0003,0},

 

    {0x0044,0x004b,0},   

    {0x0045,0x0013,30},  

 

    {0x0057,0x0002,0},

    {0x0055,0x0000,0},

    {0x0057,0x0000,30},    

 

    {0x001c,0x0004,50},

 

    {0x0043,0x0080,20},

 

    {0x001b,0x0018,100},

 

    {0x001b,0x0010,100},

 

 

    {0x0090,0x007F,0},

    {0x0026,0x0004,120},

 

    {0x0026,0x0024,120},

    {0x0026,0x002c,120},

    {0x0026,0x003c,120},

 

    {0x0022,0xffff,0}

#else

    {0x0046,0x00a4,0},

    {0x0047,0x0053,0},

    {0x0048,0x0000,0},

    {0x0049,0x0044,0},

    {0x004A,0x0004,0},

    {0x004B,0x0067,0},

    {0x004C,0x0033,0}, 

    {0x004D,0x0077,0}, 

    {0x004E,0x0012,0}, 

    {0x004F,0x004C,0}, 

    {0x0050,0x0046,0}, 

    {0x0051,0x0044,0}, 

            

     

    {0x0002,0x0000,0},  

    {0x0003,0x0000,0},  

    {0x0004,0x0000,0},  

    {0x0005,0x00EF,0}, 

    {0x0006,0x0000,0}, 

    {0x0007,0x0000,0}, 

    {0x0008,0x0001,0}, 

    {0x0009,0x003F,0}, 

 

 

    {0x0001,0x0006,0},  

    //{0x0016,0x0048,0},  //0x00c8

    {0x0016,0x00c8,0},

 

    //{0x0021,0x0000,0},

    {0x0023,0x0095,0},

    {0x0024,0x0095,0},

    {0x0025,0x00FF,0},

    {0x0027,0x0002,0},

    {0x0028,0x0002,0},

    {0x0029,0x0002,0},

    {0x002A,0x0002,0},

    {0x002C,0x0002,0},

    {0x002D,0x0002,0},

 

    {0x003A,0x0001,0}, 

    {0x003B,0x0001,0}, 

    {0x003C,0x00F0,0}, 

    {0x003D,0x0000,20}, 

 

    //{0x0010,0x00A6,0}, 

 

    {0x0035,0x0038,0},

    {0x0036,0x0078,0},

    {0x003E,0x0038,0},

    {0x0040,0x000F,0},

    {0x0041,0x00F0,0},

 

    //{0x0070,0x0048,0},//#ifdef __SYNC_LCM_SUPPORT__

    //{0x0070,0x000e,0},

    //{0x0070,0x000d,0},

    

    {0x0019,0x0087,0},  

    {0x0093,0x000b,10},  

 

    {0x0020,0x0040,0},

    {0x001D,0x0007,0},

    {0x001E,0x0000,0},

    {0x001F,0x0004,0},

 

    {0x0044,0x0047,0},   

    {0x0045,0x0010,10},  

 

    //  

 

    {0x001c,0x0006,20},

 

    {0x0043,0x0080,5},

 

    {0x001b,0x0008,40},

 

    {0x001b,0x0010,40},

 

    {0x0070, 0x000e, 0},

    

    {0x0090,0x007F,0},

    {0x0026,0x0004,40},

 

    {0x0026,0x0024,0},

    {0x0026,0x002c,40},

    {0x0026,0x003c,0},

    

    {0x0057,0x0002,0},

    {0x0055,0x0000,0},

    {0x0057,0x0000,30},  

    

    {0x0022,0xffff,0}
#endif

};

 

LCDParm MLcdParm =

{

    { sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},

    { 0, NULL },

    { 0, NULL },            

    { 0, NULL } 

 };

 

LcdSetReg  SLcdSetReg =NULL;

 

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )

{

    unsigned short  i = 0, j = 0, pos = 0;

 

    pos = (( sy << 8 ) & 0xff00 ) | ( sx & 0x00ff );

 

    MLcdSetReg(0x0002,0x0000);  //start x high

    MLcdSetReg(0x0003,0x0000);

 

    MLcdSetReg(0x0004,0x0000);  //end x high

    MLcdSetReg(0x0005,0x00ef);

 

    MLcdSetReg(0x0006,0x0000);  //start y high

    MLcdSetReg(0x0007,0x0000);

 

    MMD_USER_WaitMICRS(500);

    MLcdSetReg(0x0008,0x0001);  //end y high

    MMD_USER_WaitMICRS(500);

    MLcdSetReg(0x0009,0x003f);

 

    MLcdSetReg(0x0022,0xffff);

 

    for( j = 0; j < height; j++ )

    {   

 

 

        for( i = 0; i < width; i++ )

        {

            LCD_SetRSHigh( ((unsigned int)color ));

        }

        sy ++;

    }

}

 

void pnl_draw_hline( unsigned short line, unsigned int color )

{

    unsigned short i = 0;

        

    MLcdSetReg(0x20,0x00);

    

    MLcdSetReg(0x21,line);

    

    MLcdSetReg(0x22,0xffff);

    for( i = 0; i < (240); i++ )

    {   

        LCD_SetRSHigh( ((unsigned int)color));

        //vm_set_reg( REG_LCDIF_EXRW_EN, BIT1 );

    }

}

 

void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )

{

    unsigned short  i = 0, j = 0, pos = 0;

    unsigned int color;

 

    pos = (( sy << 8 ) & 0xff00 ) | ( sx & 0x00ff );

 

    MLcdSetReg(0x0002,0x0000);  //start x high

    MLcdSetReg(0x0003,0x0000);

 

    MLcdSetReg(0x0004,0x0000);  //end x high

    MLcdSetReg(0x0005,0x00ef);

 

    MLcdSetReg(0x0006,0x0000);  //start y high

    MLcdSetReg(0x0007,0x0000);

 

    MMD_USER_WaitMICRS(500);

    MLcdSetReg(0x0008,0x0001);  //end y high

    MMD_USER_WaitMICRS(500);

    MLcdSetReg(0x0009,0x003f);

 

    MLcdSetReg(0x0022,0xffff);

 

 

    for( j = 0; j < height; j++ )

    {

    

        

        for( i = 0; i < width; i++ )

        {

    

        

            color= (pbuf[1]<<8)|pbuf[0];

            pbuf+=0x02;     

            LCD_SetRSHigh( ((unsigned int)color ));

 

                

        }

        sy ++;

    }

    return;

 

}
#elif (MMD_LCD_PANEL_TYPE == LCD_LPFD5420A)	
void  MLcdSetReg(int adr,int val)

{

    if(adr==0xffff)

    {

        LCD_SetRSHigh((MMD_U16)val);        

 

        return;

    }   

    if(val==0xffff)

    {

        LCD_SetRSLow((MMD_U16)adr);

 

        return;

    }

    LCD_SetRSLow((MMD_U16)adr);

 

    LCD_SetRSHigh((MMD_U16)val);

 

}


void Lcd_Rotate(UINT8 degree) 
{ 
	//MMD_USER_WaitMSEC(20);
	switch(degree) 
	{ 
		case VIM_HAPI_ROTATE_0: 
			LCDIF_SetRSLow_normal((UINT16)0x0003); 
			LCDIF_SetRSHigh_normal((UINT16)0x1030); 
			MMD_USER_WaitMICRS(100);
			LCDIF_SetRSLow_normal((UINT16)0x0400);
			LCDIF_SetRSHigh_normal((UINT16)0x3104);	
			MMD_USER_WaitMICRS(100);
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			MMD_USER_WaitMICRS(100);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, 0 );		
			MMD_USER_WaitMICRS(200);
		break; 

		case VIM_HAPI_ROTATE_90: 
			LCDIF_SetRSLow_normal((UINT16)0x0003); 
			LCDIF_SetRSHigh_normal((UINT16)0x1028);
			//MMD_USER_WaitMSEC(10);
			LCDIF_SetRSLow_normal((UINT16)0x0400);
			LCDIF_SetRSHigh_normal((UINT16)0xb104);		//400x240
			//MMD_USER_WaitMSEC(10);
			LCDIF_SetRSLow_normal((UINT16)0x0001);
			LCDIF_SetRSHigh_normal((UINT16)0x0100);
			//MMD_USER_WaitMSEC(10);
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x807<<16)|(0x03));
			//MMD_USER_WaitMSEC(10);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, 0); 		
			//MMD_USER_WaitMSEC(20);
			break; 
				

		default: 
			break; 
	}

} 


static LCDReg gMLCD_powon[] =
{
	
	{0x0606,0x0000,5},	  // 

	{0x0007,0x0001,5},    // Display control 1 

	{0x0110,0x0001,5},    // Power control 6 

	{0x0100,0x17B0,5},    // Power control 1 

	{0x0101,0x0147,5},    // Power control 2 

	{0x0102,0x019D,5},    // Power control 3 

	{0x0103,0x3600,5},    // Power control 4 

	//{0x0103,0x3900,5},	 // Power control 4 

	{0x0281,0x0010,5},    // NVM read data 2		

	{0x0102,0x01BD,5},    // Power control 3

	{0x0000,0x0000,5},    // ID read register 



	{0x0001, 0x0100,5},   // set SS and SM bit  //scan direction rotate 180 need



	{0x0002,0x0100,5},    // LCD driving waveform control 

	{0x0003,0x1030,5},    //no rotation: when 0x003=0x1030; SS=1(0x001=0x0100,bit8=1); SM=0,GS=0(0x400:bit15=0;0x001:bit10=0)
	

	{0x0008,0x0303,5},    // Display control 2 

	{0x0009,0x0001,5},    // Display control 3 

	{0x000B,0x0010,5},    // Low power control 

	{0x000C,0x0000,5},    // External display interface control 1 

	{0x000F,0x0000,5},    // External display interface control 2 

	{0x0007,0x0001,5},    // Display control 1 

	//	   {0x0010,0x0014,5},	 // Panel interface control 1 (Frame rate control) 

	{0x0010,0x0010,5},    // Panel interface control 1 (Frame rate control) 

	{0x0011,0x0202,5},    // Panel interface control 2 

	{0x0012,0x0300,5},    // Panel interface control 3 

	{0x0020,0x021E,5},    // Panel interface control 4 

	{0x0021,0x0202,5},    // Panel interface control 5 

	{0x0022,0x0100,5},    // Panel interface control 6 

	{0x0090,0x8000,5},    // Frame marker control 

	{0x0100,0x14B0,5},    // Power control 1 

	{0x0101,0x0147,5},    // Power control 2 

	{0x0102,0x01BD,5},    // Power control 3 

	//{0x0103,0x3000,5},	 // Power control 4//2d00 

	{0x0103,0x0d00,5},    // Power control 4//2d00 

	{0x0107,0x0000,5},    // Power control 5 

	{0x0110,0x0001,5},    // Power control 6 



	{0x0210,0x0000,5},    // Window Horizontal RAM address start 

	{0x0211,0x00EF,5},    // Window Horizontal RAM address end 

	{0x0212,0x0000,5},    // Window vertical RAM address start 

	{0x0213,0x018F,5},    // Window vertical RAM address end	  



	{0x0280,0x0000,5},    // NVM read data 1	  

	//{0x0281,0x0004,5},	 // NVM read data 2//0002 

	{0x0281,0x0000,5},    // NVM read data 2//0002    0000-0010



	{0x0282,0x0000,5},    // NVM read data 3 

	{0x0300,0x0101,5},    // Gamma curve set 

	{0x0301,0x0024,5},    // Gamma curve set 

	{0x0302,0x1326,5},    // Gamma curve set 

	{0x0303,0x2613,5},    // Gamma curve set 

	{0x0304,0x2400,5},    // Gamma curve set 

	{0x0305,0x0100,5},    // Gamma curve set 

	{0x0306,0x1704,5},    // Gamma curve set 

	{0x0307,0x0417,5},    // Gamma curve set 

	{0x0308,0x0007,5},    // Gamma curve set 

	{0x0309,0x0105,5},    // Gamma curve set 

	{0x030A,0x0F05,5},    // Gamma curve set 

	{0x030B,0x0F01,5},    // Gamma curve set 

	{0x030C,0x010F,5},    // Gamma curve set 

	{0x030D,0x050F,5},    // Gamma curve set 

	{0x030E,0x0501,5},    // Gamma curve set 

	{0x030F,0x0700,5},    // Gamma curve set 





	//{0x0400,0xb500,5},

	//{0x0400,0x3500,5},
	{0x0400,0x3104,5}, 


	{0x0401,0x0001,5},    // Base image display control 

	{0x0404,0x0000,5},    // Base image vertical scroll control 

	{0x0500,0x0000,5},    // Display position - partial display 1 

	{0x0501,0x0000,5},    // RAM address start - partial display 1 

	{0x0502,0x0000,5},    // RAM address end - partial display 1 

	{0x0503,0x0000,5},    // Display position - partial display 2 

	{0x0504,0x0000,5},    // RAM address start - partial display 2 

	{0x0505,0x0000,5},    // RAM address end - partial display 2 

	{0x0600,0x0000,5},    // 

	{0x0606,0x0000,5},    // Pin control 

	{0x06F0,0x0000,5},    // NVM access control 

	{0x07F0,0x5420,5},    // 

	{0x07F3,0x288d,5},    // 

	{0x07F4,0x0022,5},    // 

	{0x07F5,0x0001,5},    // 

	{0x07F0,0x0000,5},    //	   

	{0x0007,0x0173,5},    // 

	{0x0202,0xffff,5}//Write Data to GRAM 	 


};
LCDParm MLcdParm =

{

    { sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},

    { 0, NULL },

    { 0, NULL },            

    { 0, NULL } 

 };

 

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
}
void pnl_draw_hline( unsigned short line, unsigned int color )
{
}
void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
}

#elif (MMD_LCD_PANEL_TYPE == LCD_S6D0170)	
void  MLcdSetReg(int adr,int val)

{

    if(adr==0xffff)

    {

        LCD_SetRSHigh((MMD_U16)val);        

 

        return;

    }   

    if(val==0xffff)

    {

        LCD_SetRSLow((MMD_U16)adr);

 

        return;

    }

    LCD_SetRSLow((MMD_U16)adr);

 

    LCD_SetRSHigh((MMD_U16)val);

 

}


void Lcd_Rotate(UINT8 degree) 
{ 
	MMDPrintf("ROTATE LCD=%d\n", degree);
	//MMD_USER_WaitMSEC(20);
	switch(degree) 
	{ 
		case VIM_HAPI_ROTATE_0: 
			LCDIF_SetRSLow_normal((UINT16)0x0003); 
			LCDIF_SetRSHigh_normal((UINT16)0x1000); 
			MMD_USER_WaitMICRS(100);
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS,( 0x03<<16)|(0x0c0b));
			MMD_USER_WaitMICRS(100);
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, ( 0x07<<16)|(0x100f));   
			MMD_USER_WaitMICRS(200);
			break; 

		case VIM_HAPI_ROTATE_90: 

			LCDIF_SetRSLow_normal((UINT16)0x0003); 
			LCDIF_SetRSHigh_normal((UINT16)0x1060);	

			LCDIF_SetRSLow_normal((UINT16)0x0047); 
			LCDIF_SetRSHigh_normal((UINT16)0x0000);

			LCDIF_SetRSLow_normal((UINT16)0x0046); 
			LCDIF_SetRSHigh_normal((UINT16)0x013f);  

			LCDIF_SetRSLow_normal((UINT16)0x0049); 
			LCDIF_SetRSHigh_normal((UINT16)0x0000);		

			LCDIF_SetRSLow_normal((UINT16)0x0048); 
			LCDIF_SetRSHigh_normal((UINT16)0x00ef);	
	
			VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS, (0x0403<<16)|(0x0c0b));		//MUST SET 0x0c0b, else can't dispaly preview image! I don't know the reason!!
			VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, ( 0x0807<<16)|(0x100f));	 

			break; 
				

		default: 
			break; 
	}

} 


static LCDReg gMLCD_powon[] =
{
	
		{0x2300,0xffff,0},			   //select 16 bit data interface	   
	
	//================= Power Supply setting ===================//	   
	
		 {0x0011,0x0000,0},    
	
		 {0x0012,0x0000,0},  
	
		 {0x0013,0x0000,0},  
	
		 {0x0014,0x0000,10}, 
	
		 
	
		 {0x0011,0x0010,0},  
	
		 {0x0012,0x4222,0},  
	
		 {0x0013,0x2060,0},  
	
		 //{0x0014,0x0448,0},	   
	
		 {0x0014,0x1048,0}, 		//new
	
		 {0x0010,0x0700,10},  
	
		 
	
		 {0x0011,0x0112,10},  
	
		 {0x0011,0x0312,10},  
	
		 {0x0011,0x0712,10},  
	
		 {0x0011,0x0F1C,10},  
	
		 {0x0011,0x0F38,30},  
	
	 
	
	//===================Display Control Register Setting ================//	 
	
		 {0x0001,0x0136,0},  
	
		 {0x0002,0x0000,0},  
	
		 {0x0003,0x1000,0}, 	  
	
		//{0x0003,0x1060,0},		//rotate 90
	
		 
	
		 //{0x0007,0x0004,0},  
	
		 {0x0007,0x1004,0}, 	//new
	
		 {0x0008,0x0088,0},  
	
		 {0x000B,0x3300,0},  
	
		 {0x000C,0x0000,0},  
	
		// {0x000F,0x0801,0},  
	
		 {0x000F,0x1101,0}, 	//new
	
		 {0x0015,0x0011,0},  
	
		 {0x0046,0x00EF,0},  
	
		 {0x0047,0x0000,0},  
	
		 {0x0048,0x01AF,0}, 	  
	
		 {0x0049,0x0000,0},  
	
	 
	
		 //gamma function
	
		 {0x0050,0x1F00,0},  
	
		 {0x0051,0x001F,0},  
	
		 {0x0052,0x0200,0},  
	
		 {0x0053,0x0000,0},  
	
		 {0x0054,0x0306,0},  
	
		 {0x0055,0x0603,0},  
	
		 {0x0056,0x0905,0},  
	
		 {0x0057,0x0302,0}, 	  
	
		 {0x0058,0x0001,0},  
	
		 {0x0059,0x0602,0},  

		 {0x0080,0x0101,0},  
	
	  //==================== display on Sequence ==================//
	
		// {0x0007,0x0016,40}, 
	
		//{0x0007,0x0117,0},  
	
		  {0x0007,0x1016,40},  //new
	
		 {0x0007,0x1117,0}, 	//new
	
	 
	
	  /*	{0x0046,0x00f0,0},	
	
		 {0x0047,0x0000,0},  
	
		 {0x0048,0x01b0,0}, 	  
	
		 {0x0049,0x0000,0},  
	
	   */
	
		{0x0046,0x0140,0},	
	
		 {0x0047,0x0000,0},  
	
		 {0x0048,0x0f0,0},		 
	
		 {0x0049,0x0000,0},  
	
		 
	
		 {0x0022,0xffff,0}
};

LCDParm MLcdParm =

{

    { sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},

    { 0, NULL },

    { 0, NULL },            

    { 0, NULL } 

 };

 

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
}
void pnl_draw_hline( unsigned short line, unsigned int color )
{
}
void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
}
#elif (MMD_LCD_PANEL_TYPE == LCD_R61509_16BIT)	
void  MLcdSetReg(int adr,int val)

{

    if(adr==0xffff)
    {
        LCD_SetRSHigh((MMD_U16)val);        
        return;

    }   

    if(val==0xffff)
    {
       LCD_SetRSLow((MMD_U16)adr);
       return;

    }

    LCD_SetRSLow((MMD_U16)adr);
    LCD_SetRSHigh((MMD_U16)val);
}


void Lcd_Rotate(UINT8 degree) 
{ 

        switch(degree) 
        { 
            case VIM_HAPI_ROTATE_0: 
	               VIM_LCDIF_RsLExW((UINT16)0x0001); 
	               VIM_LCDIF_RsHExW((UINT16)0x0100); 
	            	
	               VIM_LCDIF_RsLExW((UINT16)0x0003); 
	               VIM_LCDIF_RsHExW((UINT16)0x1032); 

			 VIM_LCDIF_RsLExW((UINT16)0x0400); 
	               VIM_LCDIF_RsHExW((UINT16)0x3100);		

	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );            
                break; 
                
            case VIM_HAPI_ROTATE_90: 
            		VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ok!
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//SS=0,SM=0	
           	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x102a);		//效果是逆时针转90度的图像

               	VIM_LCDIF_RsLExW((UINT16)0x0400); 
                     VIM_LCDIF_RsHExW((UINT16)0x3100);		//GS=0	
       		
                     VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
                     
   
                break; 
                
 		case VIM_HAPI_ROTATE_270: 					//效果是逆时针转270度的图像
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//SS=1,SM=0
            		
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x103a);		
               		

            		VIM_LCDIF_RsLExW((UINT16)0x0400); 
                 	VIM_LCDIF_RsHExW((UINT16)0xb104);		//GS=1

                	
                	VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

                break; 

                case VIM_HAPI_ROTATE_180: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//SS=0, SM=0
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1012);		
  
            		VIM_LCDIF_RsLExW((UINT16)0x0400); 
                    	VIM_LCDIF_RsHExW((UINT16)0xb104);		//GS=1, SCN0=0x04 (G400)
                    	
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );       
               	
                 break; 
  //----------------------------------------------------------------------------------//               
  		case VIM_HAPI_MIRROR_0: 
	               VIM_LCDIF_RsLExW((UINT16)0x0001); 
	               VIM_LCDIF_RsHExW((UINT16)0x0000); 
	            	
	               VIM_LCDIF_RsLExW((UINT16)0x0003); 
	               VIM_LCDIF_RsHExW((UINT16)0x1012); 

			 VIM_LCDIF_RsLExW((UINT16)0x0400); 
	               VIM_LCDIF_RsHExW((UINT16)0x3100);		
	               
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );                     	
		 break; 
		 
                case VIM_HAPI_MIRROR_180: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//SS=1, SM=0
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1012);		
  
            		VIM_LCDIF_RsLExW((UINT16)0x0400); 
                    	VIM_LCDIF_RsHExW((UINT16)0xb104);		//GS=1, SCN0=0x04 (G400)
                    	
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );  
                 break;

            case VIM_HAPI_MIRROR_90: 
            		VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ok!
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//SS=1,SM=0	
           	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x102a);		 

               	VIM_LCDIF_RsLExW((UINT16)0x0400); 
                     VIM_LCDIF_RsHExW((UINT16)0xb104);		//GS=0	
       		
                     VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
                     
   
                break; 

 		case VIM_HAPI_MIRROR_270: 					 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//SS=0,SM=0
            		
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x103a);		
               		

            		VIM_LCDIF_RsLExW((UINT16)0x0400); 
                 	VIM_LCDIF_RsHExW((UINT16)0x3100);		//GS=0

                	
                	VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

                break; 
                 
            default: 
                break; 
        }
      
        
return; 
} 


static LCDReg gMLCD_powon[] =
{
     {0x0007, 0x0000, 0},//Display Control 1
     {0x0102, 0x0000, 200},//Sets low power VCOM drive period.
     
     {0x0606, 0x0000, 0},//i80-i/F Endian Control
     {0x0008, 0x0808, 0},//Sets the number of lines for front/back porch period
     {0x0400, 0x3100, 0},//Base Image Number of Line
     //===Gamma setting===
     {0x0300, 0x0304, 0},//γ Control
     {0x0301, 0x0304, 0},//γ Control
     {0x0302, 0x0304, 0},//γ Control
     {0x0303, 0x0102, 0},//γ Control
     {0x0304, 0x0102, 0},//γ Control
     {0x0305, 0x0701, 0},//γ Control
     {0x0306, 0x1e00, 0},//γ Control
     {0x0307, 0x0403, 0},//γ Control
     {0x0308, 0x0403, 0},//γ Control
     {0x0309, 0x0403, 0},//γ Control
     {0x030A, 0x0201, 0},//γ Control
     {0x030B, 0x0201, 0},//γ Control
     {0x030C, 0x0107, 0},//γ Control
     {0x030D, 0x001e, 0},//γ Control
     //===Power on sequence===
     {0x0007, 0x0001, 0},//Display Control 1
     {0x0110, 0x0001, 0},//Power supply startup enable bit
     {0x0100, 0x16B0, 0},//Power Control 1
     {0x0101, 0x0100, 0},//Power Control 2
     {0x0102, 0x019b, 0},//Starts VLOUT3,Sets the VREG1OUT.
     {0x0103, 0x3000, 0},//set the amplitude of VCOM
     {0x0107, 0x0004, 0},
     {0x0281, 0x0004, 0},//Selects the factor of VREG1OUT to generate VCOMH.
     {0x0102, 0x01ba, 10},//Starts VLOUT3,Sets the VREG1OUT.
    
     //===User setting===
     {0x0001, 0x0100, 0},//Driver Output Control
     {0x0002, 0x0100, 0},//LCD Driving Wave Control
     {0x0003, 0x1032, 0},//Entry Mode
     {0x0006, 0x0000, 0},//Outline Sharpening Control
     {0x0009, 0x0001, 0},//Display Control 3
     {0x000B, 0x0010, 0},//Low Power Control
     {0x000C, 0x0001, 0},//External Display Interface Control 1 
     {0x0010, 0x0012, 0},//Division Ratio,Clocks per Line
     {0x0011, 0x0101, 0},//Sets the non-overlap period of adjacent gate outputs
     {0x0012, 0x0600, 0},//Sets low power VCOM drive period.
     {0x0021, 0x0101, 0},
     {0x0022, 0x0100, 0},
     {0x0090, 0x1000, 0},//Frame Marker Control
     {0x0092, 0x0002, 0},
     {0x0110, 0x0001, 0},//Power supply startup enable bit
     //Address
     {0x0210, 0x0000, 0},//Window Horizontal RAM Address Start
     {0x0211, 0x00ef, 0},//Window Horizontal RAM Address End
     {0x0212, 0x0000, 0},//Window Vertical RAM Address Start
     {0x0213, 0x018f, 0},//Window Vertical RAM Address End
     {0x0400, 0x3100, 0},//Base Image Number of Line
     {0x0401, 0x0001, 0},//Base Image Display
     {0x0404, 0x0000, 0},//Base Image Vertical Scroll Control
     //partial display control
     {0x0500, 0x0000, 0},
     {0x0501, 0x0000, 0},
     {0x0502, 0x005f, 0},
     //===Display_On_Function===
     {0x0007, 0x0021, 200},//Display Control 1
     
     {0x0007, 0x0061, 200},//Display Control 1
    
     {0x0007, 0x0173, 300},//Display Control 1
    
     {0x0202, 0xffff, 0}//Write Data to GRAM  
};

LCDParm MLcdParm =

{

    { sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},

    { 0, NULL },

    { 0, NULL },            

    { 0, NULL } 

 };

 

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
}
void pnl_draw_hline( unsigned short line, unsigned int color )
{
}
void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
}

#elif (MMD_LCD_PANEL_TYPE == LCD_R61505U_16BIT)
void  MLcdSetReg(int adr,int val)

{

    if(adr==0xffff)
    {
        LCD_SetRSHigh((MMD_U16)val);        
        return;

    }   

    if(val==0xffff)
    {
       LCD_SetRSLow((MMD_U16)adr);
       return;

    }

    LCD_SetRSLow((MMD_U16)adr);
    LCD_SetRSHigh((MMD_U16)val);
}


void Lcd_Rotate(UINT8 degree) 
{ 

        switch(degree) 
        { 
            case VIM_HAPI_ROTATE_0: 
	               VIM_LCDIF_RsLExW((UINT16)0x0001); 
	               VIM_LCDIF_RsHExW((UINT16)0x0100); 
	            	
	               VIM_LCDIF_RsLExW((UINT16)0x0003); 
	               VIM_LCDIF_RsHExW((UINT16)0x1030); 

			 VIM_LCDIF_RsLExW((UINT16)0x0060); 
	               VIM_LCDIF_RsHExW((UINT16)0x2700);		

	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );            
                break; 
                
            case VIM_HAPI_ROTATE_90: 
            		VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ok!
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//SS=0,SM=0	
           	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x102a);		//效果是逆时针转90度的图像

               	VIM_LCDIF_RsLExW((UINT16)0x0060); 
                     VIM_LCDIF_RsHExW((UINT16)0x2700);		//GS=0	
       		
                     VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
                     
   
                break; 
                
 		case VIM_HAPI_ROTATE_270: 					//效果是逆时针转270度的图像
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//SS=1,SM=0
            		
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x103a);		
               		

            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                 	VIM_LCDIF_RsHExW((UINT16)0xa700);		//GS=1

                	
                	VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

                break; 

                case VIM_HAPI_ROTATE_180: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//SS=0, SM=0
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1012);		
  
            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                    	VIM_LCDIF_RsHExW((UINT16)0xa700);		//GS=1, SCN0=0x04 (G400)
                    	
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );       
               	
                 break; 
  //----------------------------------------------------------------------------------//               
  		case VIM_HAPI_MIRROR_0: 
	               VIM_LCDIF_RsLExW((UINT16)0x0001); 
	               VIM_LCDIF_RsHExW((UINT16)0x0000); 
	            	
	               VIM_LCDIF_RsLExW((UINT16)0x0003); 
	               VIM_LCDIF_RsHExW((UINT16)0x1012); 

			 VIM_LCDIF_RsLExW((UINT16)0x0060); 
	               VIM_LCDIF_RsHExW((UINT16)0x2700);		
	               
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );                     	
		 break; 
		 
                case VIM_HAPI_MIRROR_180: 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//SS=1, SM=0
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
                 	VIM_LCDIF_RsHExW((UINT16)0x1012);		
  
            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                    	VIM_LCDIF_RsHExW((UINT16)0xa700);		//GS=1, SCN0=0x04 (G400)
                    	
	  		 VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS,( 0x03<<16)|(0x0807));
			 VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, 0 );  
                 break;

            case VIM_HAPI_MIRROR_90: 
            		VIM_LCDIF_RsLExW((UINT16)0x0001); 		//ok!
            		VIM_LCDIF_RsHExW((UINT16)0x0000);		//SS=1,SM=0	
           	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x102a);		 

               	VIM_LCDIF_RsLExW((UINT16)0x0060); 
                     VIM_LCDIF_RsHExW((UINT16)0xa700);		//GS=1	
       		
                     VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
                     
   
                break; 

 		case VIM_HAPI_MIRROR_270: 					 
 			VIM_LCDIF_RsLExW((UINT16)0x0001); 		
            		VIM_LCDIF_RsHExW((UINT16)0x0100);		//SS=0,SM=0
            		
                	            	          	
   		 	VIM_LCDIF_RsLExW((UINT16)0x0003); 
               	VIM_LCDIF_RsHExW((UINT16)0x103a);		
               		

            		VIM_LCDIF_RsLExW((UINT16)0x0060); 
                 	VIM_LCDIF_RsHExW((UINT16)0x2700);		//GS=0

                	
                	VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (0x0807<<16)|0x03);
                     VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);

                break; 
                 
            default: 
                break; 
        }
      
        
return; 
} 


static LCDReg gMLCD_powon[] =
{
{0x00A4, 0x0001, 20},
//Delayms(20, 0},
{0x0007, 0x0001, 0},
{0x0017, 0x0001, 20},
//Delayms(20, 0},
{0x0019, 0x0000, 0},
{0x0010, 0x17B0, 20},
//Delayms(20, 0},
{0x0011, 0x0117, 20},//0117
//Delayms(20, 0},
{0x0012, 0x0119, 20},//0119
//Delayms(20, 0},
{0x0013, 0x1600, 0},//1400
{0x002A, 0x0000, 0},
{0x0029, 0x000f, 0},//000e
{0x0012, 0x0139, 20},
//Delayms(20, 0},
{0x0001, 0x0100, 0},
{0x0002, 0x0700, 0},
{0x0003, 0x1030, 0},
{0x0004, 0x0000, 0},
{0x000C, 0x0000, 0},
{0x000F, 0x0000, 0},
{0x0008, 0x0704, 0},
{0x000A, 0x0000, 0},
{0x000D, 0x0000, 0},
{0x0030, 0x0707, 0},
{0x0031, 0x0407, 0},
{0x0032, 0x0500, 0},
{0x0033, 0x0000, 0},
{0x0034, 0x0301, 0},
{0x0035, 0x0202, 0},
{0x0036, 0x1510, 0},
{0x0037, 0x0707, 0},
{0x0038, 0x0407, 0},
{0x0039, 0x0203, 0},
{0x003A, 0x0301, 0},
{0x003B, 0x0301, 0},
{0x003C, 0x0202, 0},
{0x003D, 0x1018, 50},
//Delayms(50, 0},
{0x0050, 0x0000, 0},
{0x0051, 0x00EF, 0},
{0x0052, 0x0000, 0},
{0x0053, 0x013F, 0},
{0x0060, 0x2700, 0},
{0x0061, 0x0001, 0},
{0x006A, 0x0000, 0},
{0x0080, 0x0000, 0},
{0x0081, 0x0000, 0},
{0x0082, 0x0000, 0},
{0x0083, 0x0000, 0},
{0x0084, 0x0000, 0},
{0x0085, 0x0000, 0},
{0x0090, 0x0018, 0},
{0x0092, 0x0300, 0},
{0x0093, 0x0005, 0},
{0x0095, 0x0000, 0},
{0x0097, 0x0000, 0},
{0x0098, 0x0000, 0},
{0x0020, 0x0000, 0},
{0x0021, 0x0000, 0},
{0x0007, 0x0021, 20},
//Delayms(20, 0},
{0x0011, 0x0117, 0},
{0x0007, 0x0061, 20},
//Delayms(20, 0},
{0x0007, 0x0173, 0},
{0x0022, 0xffff, 0},
};

LCDParm MLcdParm =

{

    { sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},

    { 0, NULL },

    { 0, NULL },            

    { 0, NULL } 

 };

 

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
}
void pnl_draw_hline( unsigned short line, unsigned int color )
{
}
void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
}
#elif(MMD_LCD_PANEL_TYPE == LCD_LGDP4525)	
__inline void  MLcdSetReg(int adr,int val)
{
	LCD_SetRSLow((MMD_U16)adr);
	LCD_SetRSHigh((MMD_U16)val);
}

void Lcd_Rotate(UINT8 degree)
{
	switch(degree){
    case VIM_HAPI_ROTATE_0:
        LCDIF_SetRSLow_normal((UINT16)0x0003); 
        LCDIF_SetRSHigh_normal((UINT16)0x1030);
        LCDIF_SetRSLow_normal((UINT16)0x0001); 
        LCDIF_SetRSHigh_normal((UINT16)0x011B); 
        MMD_USER_WaitMICRS(100);
        VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, (3<<16)|0x04);
        MMD_USER_WaitMICRS(100);
        VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, (0<<16)|0x0);
        MMD_USER_WaitMICRS(200);
        break;
        
    case VIM_HAPI_ROTATE_90:
        LCDIF_SetRSLow_normal((UINT16)0x0003); 
        LCDIF_SetRSHigh_normal((UINT16)0x1028); 
        LCDIF_SetRSLow_normal((UINT16)0x0001); 
        LCDIF_SetRSHigh_normal((UINT16)0x001B); 
        VIM_HIF_SetReg32(MMD_REG_LCDIF_START_POS, (0x04<<16)|0x3);      
        VIM_HIF_SetReg32(MMD_REG_LCDIF_END_POS, (0<<16)|0x0);  
        break;
    default:
        break;
	}

	return;	
}




static LCDReg gMLCD_powon[] =
{
    {0X07,0X0000,0},
    {0X12,0X0000,0},
    {0X13,0X0000,0},
    {0X14,0X0331,15},
    {0X12,0X000A,0},
    {0X11,0X0001,0},
    {0X10,0X0004,0},
    {0X13,0X1055,0},
    {0X10,0X0744,0},
    {0X11,0X0331,0},
    {0X12,0X001A,20},
    {0X13,0X3055,0	},
    {0X10,0X0740,20},
    {0X01,0X011B,0},
    {0X02,0X0700,0},
    {0X03,0X1030,0},
    {0X09,0X0000,0},
    {0X0B,0X5D30,0},
    {0X0F,0X0002,0},
    {0X21,0X0000,0},
    {0X30,0X0000,0},
    {0X31,0X0401,0},
    {0X32,0X0104,0},
    {0X33,0X0404,0},
    {0X34,0X0106,0},
    {0X35,0X0604,0},
    {0X36,0X0707,0},
    {0X37,0X0404,0},
    {0X38,0X041E,0},
    {0X39,0X061B,0},
    {0X3A,0X0101,0},
    {0X3B,0X0101,0},
    {0X3C,0X0101,0},
    {0X3D,0X0101,0},
    {0X3E,0X0001,0},
    {0X3F,0X0001,0},
    {0X42,0XDB00,0},
    {0X43,0X0000,0},
    {0X44,0XAF00,0},
    {0X45,0XDB00,0},
    {0X71,0X0040,10},
    {0X10,0X2740,0},
    {0X07,0X0005,30},
    {0X07,0X0025,0},
    {0X07,0X0027,30},
    {0X07,0X0037,0},
};

LCDParm MLcdParm =
{
	{ sizeof(gMLCD_powon)/sizeof(LCDReg), (PLCDReg)&gMLCD_powon},
	{ 0, NULL },
	{ 0, NULL },			
	{ 0, NULL }	
 };

LcdSetReg  SLcdSetReg =NULL;

void pnl_draw_block( unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,unsigned int color )
{
	unsigned short  i;
    unsigned short X,Y,Z;
    unsigned short right = sx + width - 1;
    unsigned short bottom = sy + height - 1;
    
    X=right<<8&0xff00|sx&0x00ff;
	Y=bottom<<8&0xff00|sy&0x00ff;
	Z=sy<<8&0xff00|sx&0x00ff;
    
	MLcdSetReg(0x21,Z);
	MLcdSetReg(0x44,X);
    MLcdSetReg(0x45,Y);
    LCD_SetRSLow(0x22);
    
	for( i = 0; i < height*width; i++ )
	{
  		LCD_SetRSHigh( ((unsigned int)color ));
	}
}

void pnl_draw_hline( unsigned short line, unsigned int color )
{
	unsigned short  i;
    unsigned short X,Y,Z;
    unsigned short right = MMD_LCD_WIDTH - 1;
    unsigned short bottom = line;
    
    X=right<<8&0xff00;
	Y=bottom<<8&0xff00|line&0x00ff;
	Z=line<<8&0xff00;
    
	MLcdSetReg(0x21,Z);
	MLcdSetReg(0x44,X);
    MLcdSetReg(0x45,Y);
    LCD_SetRSLow(0x22);
    
	for( i = 0; i < MMD_LCD_WIDTH; i++ )
	{	
  		LCD_SetRSHigh( ((unsigned int)color));
	}
}
#include "msg.h"
void pnl_draw_bmp(unsigned short  sx, unsigned short  sy, unsigned short  width, unsigned short  height,  unsigned char *pbuf )
{
    unsigned short  i;
    unsigned short X,Y,Z;
    unsigned short right = sx + width - 1;
    unsigned short bottom = sy + height - 1;
    MSG_FATAL("pnl_draw_bmp %d %d %d",sx,sy,width);
    X=right<<8&0xff00|sx&0x00ff;
	Y=bottom<<8&0xff00|sy&0x00ff;
	Z=sy<<8&0xff00|sx&0x00ff;
    
	MLcdSetReg(0x21,Z);
	MLcdSetReg(0x44,X);
    MLcdSetReg(0x45,Y);
    LCD_SetRSLow(0x22);

    if(((unsigned int)pbuf)%2 == 0)
    {
        unsigned short *pBmp = ( unsigned short *)pbuf;
        for( i = 0; i < height*width; i++ )
    	{
      		LCD_SetRSHigh( ((unsigned int)(*pBmp) ));
            pBmp++;	
    	}
    }
    else
    {
        unsigned int color;
    	for( i = 0; i < height*width; i++ )
    	{
            color= (pbuf[1]<<8)|pbuf[0];
      		LCD_SetRSHigh( ((unsigned int)color ));
            pbuf+=0x02;	
    	}
    }
	return;

}

#else
#endif



void Lcd_Panel_SetRSFunc(unsigned long isBypass)
{
	if(isBypass == 0)
	{
		//820 is in normal mode
		LCD_SetRSHigh = LCDIF_SetRSHigh_normal;
		LCD_SetRSLow = LCDIF_SetRSLow_normal;

	}
	else
	{
		//820 is in bypass mode
		LCD_SetRSHigh = Vm_SetRSHigh;
		LCD_SetRSLow = Vm_SetRSLow;
	}
}


// Main LCD Panel register set function
void Lcd_MPanel_SetList(PLCDTRegGroup list,unsigned long isBypass)
{
	MMD_U16 i;

	if(&MLcdSetReg == NULL)
		return;

	Lcd_Panel_SetRSFunc(isBypass);

	for(i = 0; i <list->len; i++)
	{
		MLcdSetReg(list->regval[i].adr, list->regval[i].val);
		MMD_USER_WaitMICRS(list->regval[i].wait*10);
	}
}

// Sub-LCD Panel register set function
void Lcd_SPanel_SetList(PLCDTRegGroup list,unsigned long isBypass)
{
	MMD_U16 i;


	if(&SLcdSetReg == NULL)
		return;
	
	Lcd_Panel_SetRSFunc(isBypass);

	for(i = 0; i <list->len; i++)
	{
		SLcdSetReg(list->regval[i].adr, list->regval[i].val);
		MMD_USER_WaitMICRS(list->regval[i].wait*10);
	}
}

