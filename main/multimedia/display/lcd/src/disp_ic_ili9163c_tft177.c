#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   128

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA IlI9163C driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_IlI9163C[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_IlI9163C[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
#endif
{
    NOP,               //0
    NOP,               //1
    SET_LCD_CNTL_ADDR, //2 set LCD command port address
    LCD_CMD_WH,    //3
    
    SET_LCD_DATA_ADDR, //4 set LCD data port address
    LCD_DATA_WH,   //5
    
    SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR)),//6
    NULL,              //7
    NULL,              //8
    NULL,              //9
    NULL,              //10
    SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR)),//11
    NULL,              //12
    NULL,              //13
    NULL,              //14
    NULL,              //15
    SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_RAMWR)),//16
    
    RETURN            //17
};

static uint8 disp_ic_mdp_getformat(void)
{
    return LCD_16BPP_88_II; //8 16BPP 0 //18BPP
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_IlI9163C[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP((start_col)>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP((end_col)>>8));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[12] = SEND_LCD_DATA(DISP_LCD_18BPP((start_row)>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP((end_row)>>8));
    scr[15] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
}
#endif

void disp_ic_init_w021_ct100_9163C_F1N9_AGA22_2013_07_22(void)
{
    LCD_WRITE_CMD(0x11); //Exit Sleep
    LCD_DELAY(120);

    LCD_WRITE_CMD(0xEC);
    LCD_WRITE_DATA(0x02);  //0x0C

    LCD_WRITE_CMD(0x26); //Set Default Gamma
    LCD_WRITE_DATA(0x04);

    LCD_WRITE_CMD(0xB1);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x14);

    LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_WRITE_DATA(0x05);

    LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_WRITE_DATA(0x46);//0x46
    LCD_WRITE_DATA(0x40);//0x40

    LCD_WRITE_CMD(0xC7); 
    LCD_WRITE_DATA(0xBD);  //0xC2




    LCD_WRITE_CMD(0x3a); //Set Color Format
    LCD_WRITE_DATA(0x05);

    LCD_WRITE_CMD(0x2A); //Set Column Address
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x7F);
    LCD_WRITE_CMD(0x2B); //Set Page Address
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x9F);

    LCD_WRITE_CMD(0xB4); //Set Source Output Direction
    LCD_WRITE_DATA(0x06);

    LCD_WRITE_CMD(0xf2); //Enable Gamma bit
    LCD_WRITE_DATA(0x01);


    LCD_WRITE_CMD(0x36); //Set Scanning Direction
    LCD_WRITE_DATA(0xC8);  //0xc0

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x3F);//p1
    LCD_WRITE_DATA(0x24);//p2
    LCD_WRITE_DATA(0x22);//p3
    LCD_WRITE_DATA(0x2f);//p4
    LCD_WRITE_DATA(0x28);//p5
    LCD_WRITE_DATA(0x0b);//p6
    LCD_WRITE_DATA(0x4f);//p7
    LCD_WRITE_DATA(0xB7);//p8
    LCD_WRITE_DATA(0x3c);//p9
    LCD_WRITE_DATA(0x19);//p10
    LCD_WRITE_DATA(0x15);//p11
    LCD_WRITE_DATA(0x08);//p12
    LCD_WRITE_DATA(0x02);//p13
    LCD_WRITE_DATA(0x01);//p14
    LCD_WRITE_DATA(0x00);//p15

    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00);//p1
    LCD_WRITE_DATA(0x1b);//p2
    LCD_WRITE_DATA(0x1d);//p3
    LCD_WRITE_DATA(0x10);//p4
    LCD_WRITE_DATA(0x17);//p5
    LCD_WRITE_DATA(0x14);//p6
    LCD_WRITE_DATA(0x30);//p7
    LCD_WRITE_DATA(0x48);//p8
    LCD_WRITE_DATA(0x43);//p9
    LCD_WRITE_DATA(0x07);//p10
    LCD_WRITE_DATA(0x1a);//p11
    LCD_WRITE_DATA(0x37);//p12
    LCD_WRITE_DATA(0x3D);//p13
    LCD_WRITE_DATA(0x3E);//p14
    LCD_WRITE_DATA(0x3F);//p15

    LCD_WRITE_CMD(0x29); // Display On
}

void disp_ic_init_w021_ct100_9163C_cpt177_2013_06_20(void)
{
    //2013-06-20,力拓9163C+CMO更新为9163C+CPT
    LCD_WRITE_CMD(0x11); //Exit Sleep
    LCD_DELAY(120);
    LCD_WRITE_CMD(0x26); //Set Default Gamma
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_CMD(0xB1);//Set Frame Rate
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_WRITE_DATA(0x46);
    LCD_WRITE_DATA(0x40);
    LCD_WRITE_CMD(0xC7);// Set VMF
    LCD_WRITE_DATA(0xC2);
    LCD_WRITE_CMD(0x3a); //Set Color Format
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_CMD(0x2A); //Set Column Address
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x7F);
    LCD_WRITE_CMD(0x2B); //Set Page Address
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x9F);
    LCD_WRITE_CMD(0xB4);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xEC); //Set pumping clock frequence
    LCD_WRITE_DATA(0x0C);
    LCD_WRITE_CMD(0xf2); //Enable Gamma bit
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_CMD(0x36);
    LCD_WRITE_DATA(0xC8);	   //08

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x3F);//p1
    LCD_WRITE_DATA(0x26);//p2
    LCD_WRITE_DATA(0x23);//p3
    LCD_WRITE_DATA(0x30);//p4
    LCD_WRITE_DATA(0x28);//p5
    LCD_WRITE_DATA(0x10);//p6
    LCD_WRITE_DATA(0x55);//p7
    LCD_WRITE_DATA(0xB7);//p8
    LCD_WRITE_DATA(0x40);//p9
    LCD_WRITE_DATA(0x19);//p10
    LCD_WRITE_DATA(0x10);//p11
    LCD_WRITE_DATA(0x1E);//p12
    LCD_WRITE_DATA(0x02);//p13
    LCD_WRITE_DATA(0x01);//p14
    LCD_WRITE_DATA(0x00);//p15

    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00);//p1
    LCD_WRITE_DATA(0x19);//p2
    LCD_WRITE_DATA(0x1C);//p3
    LCD_WRITE_DATA(0x0F);//p4
    LCD_WRITE_DATA(0x14);//p5
    LCD_WRITE_DATA(0x0F);//p6
    LCD_WRITE_DATA(0x2A);//p7
    LCD_WRITE_DATA(0x48);//p8
    LCD_WRITE_DATA(0x3F);//p9
    LCD_WRITE_DATA(0x06);//p10
    LCD_WRITE_DATA(0x1D);//p11
    LCD_WRITE_DATA(0x21);//p12
    LCD_WRITE_DATA(0x3d);//p13
    LCD_WRITE_DATA(0x3e);//p14
    LCD_WRITE_DATA(0x3f);//p15

    LCD_WRITE_CMD(0x29); // Display On
}

static void disp_ic_init(void)
{

#if 0    //add for W516 GC9101_TFT177
	LCD_WRITE_CMD(0x3a);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA(0xC8);

	LCD_WRITE_CMD(0xfe);
	LCD_WRITE_CMD(0xef);

	LCD_WRITE_CMD(0xfd);	 //vcm
	LCD_WRITE_DATA(0x20);   //2D

	LCD_WRITE_CMD(0xff);	 //vdv
	LCD_WRITE_DATA(0x16);

	LCD_WRITE_CMD(0xb4);//line inversion
	LCD_WRITE_DATA(0x00);	

	LCD_WRITE_CMD(0xa3);
	LCD_WRITE_DATA(0x09);	//10
	LCD_WRITE_CMD(0xa4);	 
	LCD_WRITE_DATA (0x96);

	LCD_WRITE_CMD(0xe7);    
	LCD_WRITE_DATA(0x94);	
	LCD_WRITE_DATA(0x88);
	LCD_WRITE_CMD(0xed);	 
	LCD_WRITE_DATA(0x11);
	LCD_WRITE_CMD(0xe4);	 
	LCD_WRITE_DATA(0xc5);
	LCD_WRITE_CMD(0xe2);
	LCD_WRITE_DATA(0x80);//80	  
	LCD_WRITE_CMD(0xe3);
	LCD_WRITE_DATA(0x07);	 
	LCD_WRITE_CMD(0xe5);	 
	LCD_WRITE_DATA(0x10);	
	//*****************GAMMA***************************//
	LCD_WRITE_CMD(0xF0);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0xF1);
	LCD_WRITE_DATA(0x55);
	LCD_WRITE_CMD(0xF2);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_CMD(0xF3);
	LCD_WRITE_DATA(0x52);
	LCD_WRITE_CMD(0xF4);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0xF5);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0xF7);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_CMD(0xF8);
	LCD_WRITE_DATA(0x22);
	LCD_WRITE_CMD(0xF9);
	LCD_WRITE_DATA(0x77);
	LCD_WRITE_CMD(0xFA);
	LCD_WRITE_DATA(0x25);
	LCD_WRITE_CMD(0xFB);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0xFC);
	LCD_WRITE_DATA(0x00);
	//********************************************//
	LCD_WRITE_CMD(0x11);//Exit sleep
	LCD_DELAY(120);	
	LCD_WRITE_CMD(0x29);//display on


#endif

#if 1
	#if defined( FEATURE_VERSION_C316)||defined(FEATURE_VERSION_15E)
	//CPT  TXDT180CA-68v4
	
	LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(120);

	LCD_WRITE_CMD(0x26); //Set Default Gamma
	LCD_WRITE_DATA(0x04);

	LCD_WRITE_CMD(0xB1);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x10);

	LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_WRITE_DATA(0x03);

	LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_WRITE_DATA(0x4D);
	LCD_WRITE_DATA(0x40);

	LCD_WRITE_CMD(0xC7);
	LCD_WRITE_DATA(0xB7);    //0xBA

	LCD_WRITE_CMD(0x2A); //Set Column Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x7F);

	LCD_WRITE_CMD(0x2B); //Set Page Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x9F);
    
	LCD_WRITE_CMD(0xEC);
	LCD_WRITE_DATA(0x1B);
    
	LCD_WRITE_CMD(0xB7);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x3a); //Set Color Format
	LCD_WRITE_DATA(0x55);

	LCD_WRITE_CMD(0x36); //Set Scanning Direction
	LCD_WRITE_DATA(0xC8);

	LCD_WRITE_CMD(0xB4); //Set Source Output Direction
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0xf2); //Enable Gamma bit
	LCD_WRITE_DATA(0x01);

	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x3F);//p1
	LCD_WRITE_DATA(0x29);//p2
	LCD_WRITE_DATA(0x27);//p3
	LCD_WRITE_DATA(0x2F);//p4
	LCD_WRITE_DATA(0x27);//p5
	LCD_WRITE_DATA(0x0E);//p6
	LCD_WRITE_DATA(0x51);//p7
	LCD_WRITE_DATA(0xC8);//p8
	LCD_WRITE_DATA(0x3C);//p9
	LCD_WRITE_DATA(0x18);//p10
	LCD_WRITE_DATA(0x17);//p11
	LCD_WRITE_DATA(0x16);//p12
	LCD_WRITE_DATA(0x08);//p13
	LCD_WRITE_DATA(0x05);//p14
	LCD_WRITE_DATA(0x00);//p15

	LCD_WRITE_CMD(0xE1);
	LCD_WRITE_DATA(0x00);//p1
	LCD_WRITE_DATA(0x16);//p2
	LCD_WRITE_DATA(0x18);//p3
	LCD_WRITE_DATA(0x10);//p4
	LCD_WRITE_DATA(0x18);//p5
	LCD_WRITE_DATA(0x11);//p6
	LCD_WRITE_DATA(0x2E);//p7
	LCD_WRITE_DATA(0x37);//p8
	LCD_WRITE_DATA(0x43);//p9
	LCD_WRITE_DATA(0x07);//p10
	LCD_WRITE_DATA(0x18);//p11
	LCD_WRITE_DATA(0x29);//p12
	LCD_WRITE_DATA(0x37);//p13
	LCD_WRITE_DATA(0x3A);//p14
	LCD_WRITE_DATA(0x3F);//p15
	
	//648V4
	/*
	//TXDT180TA-98  //天马
	LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(120);

	LCD_WRITE_CMD(0x26); //Set Default Gamma
	LCD_WRITE_DATA(0x04);

	LCD_WRITE_CMD(0xB1);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x10);

	LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x05);

	LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_WRITE_DATA(0x48);
	LCD_WRITE_DATA(0x44);

	LCD_WRITE_CMD(0xC7);
	LCD_WRITE_DATA(0xC7);    //0xBA

	LCD_WRITE_CMD(0x2A); //Set Column Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x7F);

	LCD_WRITE_CMD(0x2B); //Set Page Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x9F);
    
	LCD_WRITE_CMD(0xEC);
	LCD_WRITE_DATA(0x0C);
    
	LCD_WRITE_CMD(0xB7);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x3a); //Set Color Format
	LCD_WRITE_DATA(0x55);

	LCD_WRITE_CMD(0x36); //Set Scanning Direction
	LCD_WRITE_DATA(0xC8);

	LCD_WRITE_CMD(0xB4); //Set Source Output Direction
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0xF2); //Enable Gamma bit
	LCD_WRITE_DATA(0x01);

	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x3F);//p1
	LCD_WRITE_DATA(0x20);//p2
	LCD_WRITE_DATA(0x1C);//p3
	LCD_WRITE_DATA(0x24);//p4
	LCD_WRITE_DATA(0x1D);//p5
	LCD_WRITE_DATA(0x0B);//p6
	LCD_WRITE_DATA(0x48);//p7
	LCD_WRITE_DATA(0xB7);//p8
	LCD_WRITE_DATA(0x33);//p9
	LCD_WRITE_DATA(0x11);//p10
	LCD_WRITE_DATA(0x00);//p11
	LCD_WRITE_DATA(0x03);//p12
	LCD_WRITE_DATA(0x00);//p13
	LCD_WRITE_DATA(0x02);//p14
	LCD_WRITE_DATA(0x00);//p15

	LCD_WRITE_CMD(0xE1);
	LCD_WRITE_DATA(0x00);//p1
	LCD_WRITE_DATA(0x1F);//p2
	LCD_WRITE_DATA(0x23);//p3
	LCD_WRITE_DATA(0x0B);//p4
	LCD_WRITE_DATA(0x12);//p5
	LCD_WRITE_DATA(0x15);//p6
	LCD_WRITE_DATA(0x37);//p7
	LCD_WRITE_DATA(0x48);//p8
	LCD_WRITE_DATA(0x4C);//p9
	LCD_WRITE_DATA(0x0E);//p10
	LCD_WRITE_DATA(0x3F);//p11
	LCD_WRITE_DATA(0x3C);//p12
	LCD_WRITE_DATA(0x3F);//p13
	LCD_WRITE_DATA(0x3F);//p14
	LCD_WRITE_DATA(0x3D);//p15
	*/
	#elif defined(FEATURE_VERSION_W450_E200) //xxzehn for w450
	
    // VCI=2.6V
    //************* Reset LCD Driver ****************//
    #if 0
    LCD_nRESET = 1;
    LCD_DELAY(1); // Delay 1ms
    LCD_nRESET = 0;
    LCD_DELAY(10); // Delay 10ms // This delay time is necessary
    LCD_nRESET = 1;
	#endif
    LCD_DELAY(120); // Delay 50 ms
    //************* Start Initial Sequence **********//
    LCD_WRITE_CMD(0xEC);   
    LCD_WRITE_DATA(0x0C); 
    
    LCD_WRITE_CMD(0xc0);  
    LCD_WRITE_DATA(0x16); 
    LCD_WRITE_DATA(0x05); 
    
    LCD_WRITE_CMD(0xc1);  
    LCD_WRITE_DATA(0x02); 
    
    LCD_WRITE_CMD(0xc5);  
    LCD_WRITE_DATA(0x46); 
    LCD_WRITE_DATA(0x40); 
    
    LCD_WRITE_CMD(0xc7);  
    LCD_WRITE_DATA(0xa4); 
    
    LCD_WRITE_CMD(0x36);  
    LCD_WRITE_DATA(0xc0); //0xc0
     
    LCD_WRITE_CMD(0x3A);   //Set Pixel Format
    LCD_WRITE_DATA(0x55); 
    
    LCD_WRITE_CMD(0xF2);  
    LCD_WRITE_DATA(0x01); 
    
    LCD_WRITE_CMD(0xE0);    //Set Gamma 
    LCD_WRITE_DATA(0x3f); 
    LCD_WRITE_DATA(0x28); 
    LCD_WRITE_DATA(0x24); 
    LCD_WRITE_DATA(0x2d); 
    LCD_WRITE_DATA(0x26); 
    LCD_WRITE_DATA(0x0b); 
    LCD_WRITE_DATA(0x4f); 
    LCD_WRITE_DATA(0xc4); 
    LCD_WRITE_DATA(0x3a); 
    LCD_WRITE_DATA(0x18); 
    LCD_WRITE_DATA(0x19); 
    LCD_WRITE_DATA(0x08); 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_DATA(0x04); 
    LCD_WRITE_DATA(0x01); 
     
    LCD_WRITE_CMD(0xE1);    //Set Gamma 
    LCD_WRITE_DATA(0x00); 
    LCD_WRITE_DATA(0x17); 
    LCD_WRITE_DATA(0x1b); 
    LCD_WRITE_DATA(0x12); 
    LCD_WRITE_DATA(0x19); 
    LCD_WRITE_DATA(0x14); 
    LCD_WRITE_DATA(0x30); 
    LCD_WRITE_DATA(0x3b); 
    LCD_WRITE_DATA(0x45); 
    LCD_WRITE_DATA(0x07); 
    LCD_WRITE_DATA(0x26); 
    LCD_WRITE_DATA(0x2e); 
    LCD_WRITE_DATA(0x3a); 
    LCD_WRITE_DATA(0x3b); 
    LCD_WRITE_DATA(0x3e); 
     
    
    
    LCD_WRITE_CMD(0xB4);   
    LCD_WRITE_DATA(0x06); 
    
    LCD_WRITE_CMD(0xB1);    //Set Frame Rate
    LCD_WRITE_DATA(0x08); 
    LCD_WRITE_DATA(0x12); 
    
    LCD_WRITE_CMD(0x11);    //Exit Sleep 
    LCD_DELAY(120);
    LCD_WRITE_CMD(0x29);    //Display on 
    LCD_WRITE_CMD(0x2C);     
    #elif defined(FEATURE_VERSION_W022_CT100)    
    disp_ic_init_w021_ct100_9163C_F1N9_AGA22_2013_07_22();
    #elif defined(FEATURE_VERSION_W021_CT100_X2)||defined(FEATURE_VERSION_W021_CT100_VERSION_02)
    disp_ic_init_w021_ct100_9163C_F1N9_AGA22_2013_07_22();
    #elif defined(FEATURE_VERSION_W021_CT100)&&!defined(FEATURE_VERSION_W021_CT100_VERSION_02)
    disp_ic_init_w021_ct100_9163C_cpt177_2013_06_20();
   
    #elif defined(FEATURE_VERSION_W027_IC18)
    LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(50);	
    
    LCD_WRITE_CMD(0x26); //Set Default Gamma
    LCD_WRITE_DATA(0x04);
    
    LCD_WRITE_CMD(0xB1);//频率
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x14);

    LCD_WRITE_CMD(0xB4);
	LCD_WRITE_DATA(0x07);
    
    LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x00);
    
    LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_WRITE_DATA(0x03);

    LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_WRITE_DATA(0x32);  //0x39
    LCD_WRITE_DATA(0x40);
    
    LCD_WRITE_CMD(0xC7);
    LCD_WRITE_DATA(0xB7);    
    
    LCD_WRITE_CMD(0xEC);
    LCD_WRITE_DATA(0x0C);
    
    LCD_WRITE_CMD(0x3a); //Set Color Format
    LCD_WRITE_DATA(0x05);
    
    LCD_WRITE_CMD(0x2A); //Set Column Address
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x7F);
    
    LCD_WRITE_CMD(0x2B); //Set Page Address
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x9F);
    
    LCD_WRITE_CMD(0x36); //Set Scanning Direction
    LCD_WRITE_DATA(0xC0);//60
    
    LCD_WRITE_CMD(0xB7); //Set Source Output Direction
    LCD_WRITE_DATA(0x00);
    
    LCD_WRITE_CMD(0xf2); //Enable Gamma bit
    LCD_WRITE_DATA(0x01);
    
    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x3F);//p1
    LCD_WRITE_DATA(0x25);//p2
    LCD_WRITE_DATA(0x22);//p3
    LCD_WRITE_DATA(0x2F);//p4
    LCD_WRITE_DATA(0x28);//p5
    LCD_WRITE_DATA(0x0C);//p6
    LCD_WRITE_DATA(0x4F);//p7
    LCD_WRITE_DATA(0xB7);//p8
    LCD_WRITE_DATA(0x3C);//p9
    LCD_WRITE_DATA(0x19);//p10
    LCD_WRITE_DATA(0x16);//p11
    LCD_WRITE_DATA(0x1E);//p12
    LCD_WRITE_DATA(0x02);//p13
    LCD_WRITE_DATA(0x01);//p14
    LCD_WRITE_DATA(0x00);//p15
    
    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00);//p1
    LCD_WRITE_DATA(0x1A);//p2
    LCD_WRITE_DATA(0x1D);//p3
    LCD_WRITE_DATA(0x10);//p4
    LCD_WRITE_DATA(0x17);//p5
    LCD_WRITE_DATA(0x13);//p6
    LCD_WRITE_DATA(0x30);//p7
    LCD_WRITE_DATA(0x48);//p8
    LCD_WRITE_DATA(0x43);//p9
    LCD_WRITE_DATA(0x06);//p10
    LCD_WRITE_DATA(0x19);//p11
    LCD_WRITE_DATA(0x21);//p12
    LCD_WRITE_DATA(0x3D);//p13
	LCD_WRITE_DATA(0x3E);//p14
	LCD_WRITE_DATA(0x3F);//p15
	
	#else
	LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(50);	 

	LCD_WRITE_CMD(0x26); //Set Default Gamma
	LCD_WRITE_DATA(0x04);

	LCD_WRITE_CMD(0xB1);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x10);

	LCD_WRITE_CMD(0xB4);
	LCD_WRITE_DATA(0x07);


	LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_WRITE_DATA(0x03);

	LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_WRITE_DATA(0x34);
	LCD_WRITE_DATA(0x39);

	LCD_WRITE_CMD(0xC7);
	LCD_WRITE_DATA(0xB4);

	LCD_WRITE_CMD(0xEC);
	LCD_WRITE_DATA(0x0C);

	LCD_WRITE_CMD(0x3a); //Set Color Format
	LCD_WRITE_DATA(0x05);

	LCD_WRITE_CMD(0x2A); //Set Column Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x7F);

	LCD_WRITE_CMD(0x2B); //Set Page Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x9F);

	LCD_WRITE_CMD(0x36); //Set Scanning Direction
	LCD_WRITE_DATA(0xC0);
	
	LCD_WRITE_CMD(0xB7); //Set Source Output Direction
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xf2); //Enable Gamma bit
	LCD_WRITE_DATA(0x01);

	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x3F);//p1
	LCD_WRITE_DATA(0x25);//p2
	LCD_WRITE_DATA(0x22);//p3
	LCD_WRITE_DATA(0x2F);//p4
	LCD_WRITE_DATA(0x28);//p5
	LCD_WRITE_DATA(0x0C);//p6
	LCD_WRITE_DATA(0x4F);//p7
	LCD_WRITE_DATA(0xB7);//p8
	LCD_WRITE_DATA(0x3C);//p9
	LCD_WRITE_DATA(0x19);//p10
	LCD_WRITE_DATA(0x16);//p11
	LCD_WRITE_DATA(0x1E);//p12
	LCD_WRITE_DATA(0x02);//p13
	LCD_WRITE_DATA(0x01);//p14
	LCD_WRITE_DATA(0x00);//p15

	LCD_WRITE_CMD(0xE1);
	LCD_WRITE_DATA(0x00);//p1
	LCD_WRITE_DATA(0x1A);//p2
	LCD_WRITE_DATA(0x1D);//p3
	LCD_WRITE_DATA(0x10);//p4
	LCD_WRITE_DATA(0x17);//p5
	LCD_WRITE_DATA(0x13);//p6
	LCD_WRITE_DATA(0x30);//p7
	LCD_WRITE_DATA(0x48);//p8
	LCD_WRITE_DATA(0x43);//p9
	LCD_WRITE_DATA(0x06);//p10
	LCD_WRITE_DATA(0x19);//p11
	LCD_WRITE_DATA(0x21);//p12
	LCD_WRITE_DATA(0x3D);//p13
	LCD_WRITE_DATA(0x3E);//p14
	LCD_WRITE_DATA(0x3F);//p15
#endif
	LCD_WRITE_CMD(0x29); // Display On

#else
    //--************ Start Initial Sequence **********--//
            
  LCD_WRITE_CMD(0x01); //Exit Sleep
	LCD_DELAY(120);
	LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(120);

	LCD_WRITE_CMD(0x26); //Set Default Gamma
	LCD_WRITE_DATA(0x04);

	LCD_WRITE_CMD(0xB1);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x10);

	LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_WRITE_DATA(0x35);
	LCD_WRITE_DATA(0x57);

	LCD_WRITE_CMD(0xC7);
	LCD_WRITE_DATA(0xb8);    //0xBA

	LCD_WRITE_CMD(0xEC);
	LCD_WRITE_DATA(0x0C);

	LCD_WRITE_CMD(0x3a); //Set Color Format
	LCD_WRITE_DATA(0x05);

	LCD_WRITE_CMD(0x2A); //Set Column Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x7F);

	LCD_WRITE_CMD(0x2B); //Set Page Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x9F);

	LCD_WRITE_CMD(0x36); //Set Scanning Direction
	LCD_WRITE_DATA(0xC8);

	LCD_WRITE_CMD(0xB7); //Set Source Output Direction
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xB4); //Set Source Output Direction
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0xf2); //Enable Gamma bit
	LCD_WRITE_DATA(0x01);

	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x3F);//p1
	LCD_WRITE_DATA(0x20);//p2
	LCD_WRITE_DATA(0x1d);//p3
	LCD_WRITE_DATA(0x24);//p4
	LCD_WRITE_DATA(0x1d);//p5
	LCD_WRITE_DATA(0x0d);//p6
	LCD_WRITE_DATA(0x45);//p7
	LCD_WRITE_DATA(0xBa);//p8
	LCD_WRITE_DATA(0x30);//p9
	LCD_WRITE_DATA(0x17);//p10
	LCD_WRITE_DATA(0x0f);//p11
	LCD_WRITE_DATA(0x08);//p12
	LCD_WRITE_DATA(0x04);//p13
	LCD_WRITE_DATA(0x02);//p14
	LCD_WRITE_DATA(0x00);//p15

	LCD_WRITE_CMD(0xE1);
	LCD_WRITE_DATA(0x00);//p1
	LCD_WRITE_DATA(0x1f);//p2
	LCD_WRITE_DATA(0x22);//p3
	LCD_WRITE_DATA(0x0c);//p4
	LCD_WRITE_DATA(0x12);//p5
	LCD_WRITE_DATA(0x12);//p6
	LCD_WRITE_DATA(0x3a);//p7
	LCD_WRITE_DATA(0x45);//p8
	LCD_WRITE_DATA(0x4E);//p9
	LCD_WRITE_DATA(0x08);//p10
	LCD_WRITE_DATA(0x20);//p11
	LCD_WRITE_DATA(0x27);//p12
	LCD_WRITE_DATA(0x3c);//p13
	LCD_WRITE_DATA(0x3d);//p14
	LCD_WRITE_DATA(0x3F);//p15
#endif
  
	LCD_WRITE_CMD(0x29); // Display On  
  LCD_WRITE_CMD(0x2C);//Memory Write

}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA(0x0);
    LCD_WRITE_DATA((uint8)start_col);
    LCD_WRITE_DATA(0x0);
    LCD_WRITE_DATA((uint8)end_col);
    
    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA(0x0);        
    LCD_WRITE_DATA((uint8)start_row);
    LCD_WRITE_DATA(0x0);        
    LCD_WRITE_DATA((uint8)end_row);
    
    LCD_WRITE_CMD(0x2C);
}

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x10); //Sleep in
    }
    else
    {
        LCD_WRITE_CMD(0x11); //Exit Sleep
        LCD_DELAY(120);
    }
}

static void disp_ic_rot(uint16 degree)
{
#if 0
    if(degree == 0)
    {
        LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
        LCD_WRITE_DATA(0x3C);
        LCD_WRITE_DATA(0x40);
    }
    else
    {
        LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
        LCD_WRITE_DATA(0x2C);
        LCD_WRITE_DATA(0x40);
    }
#endif
    return;
}

boolean disp_ili9163c_tft177(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
    
    // Read ID
    LCD_WRITE_CMD(0xDA);
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDB);
    id2 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDC);
    id3 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
    
    //if(id1 == 0x54 && id2 == 0x80 && id3 == 0x66)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
