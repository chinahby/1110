#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   320

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ILI9341 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ILI9341[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ILI9341[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_ILI9341[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[12] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[15] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
}
#endif
//int reg=0xAA;
static void disp_ic_init(void)
{
  
    
    //--************ Start Initial Sequence **********--/
 #if defined(FEATURE_VERSION_K212_HUALU)
 	LCD_DELAY(120);      // Delay 120 ms 
 	#ifdef FEATURE_VERSION_K232_JST    //瀚彩 9341v
    LCD_WRITE_CMD(0xEF);  
	LCD_WRITE_DATA(0x03); 
	LCD_WRITE_DATA(0x80); 
	LCD_WRITE_DATA(0x02); 
	
	LCD_WRITE_CMD(0xCF);  
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0xC1); //0x81
	LCD_WRITE_DATA(0X30); 
	
	LCD_WRITE_CMD(0xED);  
	LCD_WRITE_DATA(0x67); 
	LCD_WRITE_DATA(0x03); 
	LCD_WRITE_DATA(0X12); 
	LCD_WRITE_DATA(0X81); 
	
	LCD_WRITE_CMD(0xE8);  
	LCD_WRITE_DATA(0x85); 
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x79); 
	
	LCD_WRITE_CMD(0xCB);  
	LCD_WRITE_DATA(0x39); 
	LCD_WRITE_DATA(0x2C); 
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x34); 
	LCD_WRITE_DATA(0x02); 
	
	LCD_WRITE_CMD(0xF7);  
	LCD_WRITE_DATA(0x20); 
	
	LCD_WRITE_CMD(0xEA);  
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x00); 
	
	LCD_WRITE_CMD(0xC0);       //Power control 
	LCD_WRITE_DATA(0x23);     //VRH[5:0] 
	
	LCD_WRITE_CMD(0xC1);       //Power control 
	LCD_WRITE_DATA(0x12);     //SAP[2:0];BT[3:0] 
	
	LCD_WRITE_CMD(0xC5);       //VCM control 
	LCD_WRITE_DATA(0x30); 
	LCD_WRITE_DATA(0x3F); 
	
	LCD_WRITE_CMD(0xC7);       //VCM control2 
	LCD_WRITE_DATA(0x92); 
	
	LCD_WRITE_CMD(0x36);       // Memory Access Control 
	LCD_WRITE_DATA(0x48); 
	
	LCD_WRITE_CMD(0xB1);       //VCM control 
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x10); 

    LCD_WRITE_CMD(0x3A);   
	LCD_WRITE_DATA(0x55); 
	
	LCD_WRITE_CMD(0xF2);       // 3Gamma Function Disable 
	LCD_WRITE_DATA(0x00); 
	
	LCD_WRITE_CMD(0x26);       //Gamma curve selected 
	LCD_WRITE_DATA(0x01); 
	
	LCD_WRITE_CMD(0xE0);       //Set Gamma 
	LCD_WRITE_DATA(0x0F); 
	LCD_WRITE_DATA(0x22); 
	LCD_WRITE_DATA(0x1C); 
	LCD_WRITE_DATA(0x1B); 
	LCD_WRITE_DATA(0x08); 
	LCD_WRITE_DATA(0x0F); 
	LCD_WRITE_DATA(0x48); 
	LCD_WRITE_DATA(0xB8); 
	LCD_WRITE_DATA(0x34); 
	LCD_WRITE_DATA(0x05); 
	LCD_WRITE_DATA(0x0C); 
	LCD_WRITE_DATA(0x09); 
	LCD_WRITE_DATA(0x0F); 
	LCD_WRITE_DATA(0x07); 
	LCD_WRITE_DATA(0x00); 
	
	LCD_WRITE_CMD(0XE1);       //Set Gamma 
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x23); 
	LCD_WRITE_DATA(0x24); 
	LCD_WRITE_DATA(0x07); 
	LCD_WRITE_DATA(0x10); 
	LCD_WRITE_DATA(0x07); 
	LCD_WRITE_DATA(0x38); 
	LCD_WRITE_DATA(0x47); 
	LCD_WRITE_DATA(0x4B); 
	LCD_WRITE_DATA(0x0A); 
	LCD_WRITE_DATA(0x13); 
	LCD_WRITE_DATA(0x06); 
	LCD_WRITE_DATA(0x30); 
	LCD_WRITE_DATA(0x38); 
	LCD_WRITE_DATA(0x0F);
	#elif defined(FEATURE_VERSION_K212_BH)
	LCD_WRITE_CMD(0xCF);  
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0xD9); 
    LCD_WRITE_DATA (0X30); 
     
    LCD_WRITE_CMD(0xED);  
    LCD_WRITE_DATA (0x64); 
    LCD_WRITE_DATA (0x03); 
    LCD_WRITE_DATA (0X12); 
    LCD_WRITE_DATA (0X81); 
     
    LCD_WRITE_CMD(0xE8);  
    LCD_WRITE_DATA (0x85); 
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x78); 
     
    LCD_WRITE_CMD(0xCB);  
    LCD_WRITE_DATA (0x39); 
    LCD_WRITE_DATA (0x2C); 
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x34); 
    LCD_WRITE_DATA (0x02); 
     
    LCD_WRITE_CMD(0xF7);  
    LCD_WRITE_DATA (0x20); 
     
    LCD_WRITE_CMD(0xEA);  
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x00); 
     
    LCD_WRITE_CMD(0xC0);    //Power control 
    LCD_WRITE_DATA (0x1B);   //VRH[5:0] 
     
    LCD_WRITE_CMD(0xC1);    //Power control 
    LCD_WRITE_DATA (0x12);   //SAP[2:0];BT[3:0] 
     
    LCD_WRITE_CMD(0xC5);    //VCM control 
    LCD_WRITE_DATA (0x32); 
    LCD_WRITE_DATA (0x3C); 
     
    LCD_WRITE_CMD(0xC7);    //VCM control2 
    LCD_WRITE_DATA (0X9D); 
     
    LCD_WRITE_CMD(0x36);    // Memory Access Control 
    LCD_WRITE_DATA (0x08); 
     
    LCD_WRITE_CMD(0x3A);   
    LCD_WRITE_DATA (0x55); 

    LCD_WRITE_CMD(0xB1);   
    LCD_WRITE_DATA (0x00);   
    LCD_WRITE_DATA (0x1B); 
     
    LCD_WRITE_CMD(0xB6);    // Display Function Control 
    LCD_WRITE_DATA (0x0A); 
    LCD_WRITE_DATA (0xA2); 

    LCD_WRITE_CMD(0xF6);    
    LCD_WRITE_DATA (0x01); 
    LCD_WRITE_DATA (0x30); 
     
    LCD_WRITE_CMD(0xF2);    // 3Gamma Function Disable 
    LCD_WRITE_DATA (0x00); 
     
    LCD_WRITE_CMD(0x26);    //Gamma curve selected 
    LCD_WRITE_DATA (0x01); 
     
    LCD_WRITE_CMD(0xE0);    //Set Gamma 
    LCD_WRITE_DATA (0x0F); 
    LCD_WRITE_DATA (0x24); 
    LCD_WRITE_DATA (0x1F); 
    LCD_WRITE_DATA (0x0B); 
    LCD_WRITE_DATA (0x0F); 
    LCD_WRITE_DATA (0x05); 
    LCD_WRITE_DATA (0x4A); 
    LCD_WRITE_DATA (0X96); 
    LCD_WRITE_DATA (0x39); 
    LCD_WRITE_DATA (0x07); 
    LCD_WRITE_DATA (0x11); 
    LCD_WRITE_DATA (0x03); 
    LCD_WRITE_DATA (0x11); 
    LCD_WRITE_DATA (0x0D); 
    LCD_WRITE_DATA (0x04);
	
    LCD_WRITE_CMD(0XE1);    //Set Gamma 
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x1B); 
    LCD_WRITE_DATA (0x20); 
    LCD_WRITE_DATA (0x04); 
    LCD_WRITE_DATA (0x10); 
    LCD_WRITE_DATA (0x02); 
    LCD_WRITE_DATA (0x35); 
    LCD_WRITE_DATA (0x23); 
    LCD_WRITE_DATA (0x46); 
    LCD_WRITE_DATA (0x04); 
    LCD_WRITE_DATA (0x0E); 
    LCD_WRITE_DATA (0x0C); 
    LCD_WRITE_DATA (0x2E); 
    LCD_WRITE_DATA (0x32); 
    LCD_WRITE_DATA (0x05); 
    #else
	//************* Start Initial Sequence **********// 
	LCD_WRITE_CMD(0xCF);  
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0xC1); 
	LCD_WRITE_DATA(0X30); 
	 
	LCD_WRITE_CMD(0xED);  
	LCD_WRITE_DATA(0x64); 
	LCD_WRITE_DATA(0x03); 
	LCD_WRITE_DATA(0X12); 
	LCD_WRITE_DATA(0X81); 
	 
	LCD_WRITE_CMD(0xE8);  
	LCD_WRITE_DATA(0x85); 
	LCD_WRITE_DATA(0x10); 
	LCD_WRITE_DATA(0x78); 
	 
	LCD_WRITE_CMD(0xCB);  
	LCD_WRITE_DATA(0x39); 
	LCD_WRITE_DATA(0x2C); 
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x34); 
	LCD_WRITE_DATA(0x02); 
	 
	LCD_WRITE_CMD(0xF7);  
	LCD_WRITE_DATA(0x20); 
	 
	LCD_WRITE_CMD(0xEA);  
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x00); 
	 
	LCD_WRITE_CMD(0xC0);    //Power control 
	LCD_WRITE_DATA(0x21);   //VRH[5:0] 
	 
	LCD_WRITE_CMD(0xC1);    //Power control 
	LCD_WRITE_DATA(0x12);   //SAP[2:0];BT[3:0] 
	 
	LCD_WRITE_CMD(0xC5);    //VCM control 
	LCD_WRITE_DATA(0x32); 
	LCD_WRITE_DATA(0x3C); 
	 
	LCD_WRITE_CMD(0xC7);    //VCM control2 
	LCD_WRITE_DATA(0XB2); 
	 
	LCD_WRITE_CMD(0x36);    // Memory Access Control 
	LCD_WRITE_DATA(0x08); 
	 
	LCD_WRITE_CMD(0x3A);   
	LCD_WRITE_DATA(0x55); 

	LCD_WRITE_CMD(0xB1);   
	LCD_WRITE_DATA(0x00);   
	LCD_WRITE_DATA(0x17); 
	 
	LCD_WRITE_CMD(0xB6);    // Display Function Control 
	LCD_WRITE_DATA(0x0A); 
	LCD_WRITE_DATA(0xA2); 

	LCD_WRITE_CMD(0xF6);    
	LCD_WRITE_DATA(0x01); 
	LCD_WRITE_DATA(0x30); 
	 
	LCD_WRITE_CMD(0xF2);    // 3Gamma Function Disable 
	LCD_WRITE_DATA(0x00); 
	 
	LCD_WRITE_CMD(0x26);    //Gamma curve selected 
	LCD_WRITE_DATA(0x01); 
	 
	LCD_WRITE_CMD(0xE0);    //Set Gamma 
	LCD_WRITE_DATA(0x0F); 
	LCD_WRITE_DATA(0x20); 
	LCD_WRITE_DATA(0x1E); 
	LCD_WRITE_DATA(0x07); 
	LCD_WRITE_DATA(0x0A); 
	LCD_WRITE_DATA(0x03); 
	LCD_WRITE_DATA(0x52); 
	LCD_WRITE_DATA(0X63); 
	LCD_WRITE_DATA(0x44); 
	LCD_WRITE_DATA(0x08); 
	LCD_WRITE_DATA(0x17); 
	LCD_WRITE_DATA(0x09); 
	LCD_WRITE_DATA(0x19); 
	LCD_WRITE_DATA(0x13); 
	LCD_WRITE_DATA(0x00); 
	 
	LCD_WRITE_CMD(0XE1);    //Set Gamma 
	LCD_WRITE_DATA(0x00); 
	LCD_WRITE_DATA(0x16); 
	LCD_WRITE_DATA(0x19); 
	LCD_WRITE_DATA(0x02); 
	LCD_WRITE_DATA(0x0F); 
	LCD_WRITE_DATA(0x03); 
	LCD_WRITE_DATA(0x2F); 
	LCD_WRITE_DATA(0x13); 
	LCD_WRITE_DATA(0x40); 
	LCD_WRITE_DATA(0x01); 
	LCD_WRITE_DATA(0x08); 
	LCD_WRITE_DATA(0x07); 
	LCD_WRITE_DATA(0x2E); 
	LCD_WRITE_DATA(0x3C); 
	LCD_WRITE_DATA(0x0F); 
    #endif
 #elif defined(FEATURE_VERSION_K212)
    LCD_DELAY(120);       // Delay 120 ms 
 
    //************* Start Initial Sequence **********// 
    LCD_WRITE_CMD(0xCF);  
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0xD9); 
    LCD_WRITE_DATA (0X30); 
     
    LCD_WRITE_CMD(0xED);  
    LCD_WRITE_DATA (0x64); 
    LCD_WRITE_DATA (0x03); 
    LCD_WRITE_DATA (0X12); 
    LCD_WRITE_DATA (0X81); 
     
    LCD_WRITE_CMD(0xE8);  
    LCD_WRITE_DATA (0x85); 
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x78); 
     
    LCD_WRITE_CMD(0xCB);  
    LCD_WRITE_DATA (0x39); 
    LCD_WRITE_DATA (0x2C); 
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x34); 
    LCD_WRITE_DATA (0x02); 
     
    LCD_WRITE_CMD(0xF7);  
    LCD_WRITE_DATA (0x20); 
     
    LCD_WRITE_CMD(0xEA);  
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x00); 
     
    LCD_WRITE_CMD(0xC0);    //Power control 
    LCD_WRITE_DATA (0x1B);   //VRH[5:0] 
     
    LCD_WRITE_CMD(0xC1);    //Power control 
    LCD_WRITE_DATA (0x12);   //SAP[2:0];BT[3:0] 
     
    LCD_WRITE_CMD(0xC5);    //VCM control 
    LCD_WRITE_DATA (0x32); 
    LCD_WRITE_DATA (0x3C); 
     
    LCD_WRITE_CMD(0xC7);    //VCM control2 
    LCD_WRITE_DATA (0X9D); 
     
    LCD_WRITE_CMD(0x36);    // Memory Access Control 
    LCD_WRITE_DATA (0x08); 
     
    LCD_WRITE_CMD(0x3A);   
    LCD_WRITE_DATA (0x55); 

    LCD_WRITE_CMD(0xB1);   
    LCD_WRITE_DATA (0x00);   
    LCD_WRITE_DATA (0x1B); 
     
    LCD_WRITE_CMD(0xB6);    // Display Function Control 
    LCD_WRITE_DATA (0x0A); 
    LCD_WRITE_DATA (0xA2); 

    LCD_WRITE_CMD(0xF6);    
    LCD_WRITE_DATA (0x01); 
    LCD_WRITE_DATA (0x30); 
     
    LCD_WRITE_CMD(0xF2);    // 3Gamma Function Disable 
    LCD_WRITE_DATA (0x00); 
     
    LCD_WRITE_CMD(0x26);    //Gamma curve selected 
    LCD_WRITE_DATA (0x01); 
     
    LCD_WRITE_CMD(0xE0);    //Set Gamma 
    LCD_WRITE_DATA (0x0F); 
    LCD_WRITE_DATA (0x24); 
    LCD_WRITE_DATA (0x1F); 
    LCD_WRITE_DATA (0x0B); 
    LCD_WRITE_DATA (0x0F); 
    LCD_WRITE_DATA (0x05); 
    LCD_WRITE_DATA (0x4A); 
    LCD_WRITE_DATA (0X96); 
    LCD_WRITE_DATA (0x39); 
    LCD_WRITE_DATA (0x07); 
    LCD_WRITE_DATA (0x11); 
    LCD_WRITE_DATA (0x03); 
    LCD_WRITE_DATA (0x11); 
    LCD_WRITE_DATA (0x0D); 
    LCD_WRITE_DATA (0x04); 
     
    LCD_WRITE_CMD(0XE1);    //Set Gamma 
    LCD_WRITE_DATA (0x00); 
    LCD_WRITE_DATA (0x1B); 
    LCD_WRITE_DATA (0x20); 
    LCD_WRITE_DATA (0x04); 
    LCD_WRITE_DATA (0x10); 
    LCD_WRITE_DATA (0x02); 
    LCD_WRITE_DATA (0x35); 
    LCD_WRITE_DATA (0x23); 
    LCD_WRITE_DATA (0x46); 
    LCD_WRITE_DATA (0x04); 
    LCD_WRITE_DATA (0x0E); 
    LCD_WRITE_DATA (0x0C); 
    LCD_WRITE_DATA (0x2E); 
    LCD_WRITE_DATA (0x32); 
    LCD_WRITE_DATA (0x05); 
 
 #else
	LCD_WRITE_CMD(0xCB);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x2C); 
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x34);
    LCD_WRITE_DATA(0x02);

    LCD_WRITE_CMD(0xCF);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0xC1);
    LCD_WRITE_DATA(0x30);

    LCD_WRITE_CMD(0xE8);
    LCD_WRITE_DATA(0x85);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x7A);

    LCD_WRITE_CMD(0xEA);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xED);
    LCD_WRITE_DATA(0x64);
    LCD_WRITE_DATA(0x03); 
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x81);

    LCD_WRITE_CMD(0xF7);
    LCD_WRITE_DATA(0x20);

    LCD_WRITE_CMD(0xC0);
    LCD_WRITE_DATA(0x21); //  GVDD=4.50V

    LCD_WRITE_CMD(0xC1);
    LCD_WRITE_DATA(0x11); // AVDD=VCIx2, VGH=VCIx7, VGL=-VCIx3

    LCD_WRITE_CMD(0xC5); //改善对比度
    LCD_WRITE_DATA(0x41); 
    LCD_WRITE_DATA(0x2C); 

    LCD_WRITE_CMD(0xC7);
    LCD_WRITE_DATA(0xAA); // 调FILCK

    LCD_WRITE_CMD(0x36);
#if defined(FEATURE_VERSION_IC241A_MMX) || defined(FEATURE_VERSION_K212_HUALU)
    LCD_WRITE_DATA(0x88);   //0x48
#else
    LCD_WRITE_DATA(0x48);  
#endif    

    LCD_WRITE_CMD(0x3A);
    LCD_WRITE_DATA(0x55);

    LCD_WRITE_CMD(0xB1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x1C); //Frame Rate=73Hz

    LCD_WRITE_CMD(0xB6);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0xe2);



    LCD_WRITE_CMD(0xF2);
    LCD_WRITE_DATA(0x02);

    LCD_WRITE_CMD(0x26);
    LCD_WRITE_DATA(0x01);


    LCD_WRITE_CMD(0xB5);
    LCD_WRITE_DATA(0x11); 
    LCD_WRITE_DATA(0x11);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0x14);

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x0F); 
    LCD_WRITE_DATA(0x2B);
    LCD_WRITE_DATA(0x24);
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x04); 
    LCD_WRITE_DATA(0x46);
    LCD_WRITE_DATA(0x35);
    LCD_WRITE_DATA(0x3A);
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x0C);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00); 
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x23);
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x34);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x0F);
#endif
    LCD_WRITE_CMD(0x11);
    LCD_DELAY(120);  // Delay 60ms

    LCD_WRITE_CMD(0x29);    
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x2A);
    //LCD_WRITE_DATA(0x0);
	LCD_WRITE_DATA16((uint16)start_col);
    //LCD_WRITE_DATA(0x0);
	LCD_WRITE_DATA16((uint16)end_col);
    
	/* Set LCD hardware to set start address */
	/* Transfer command to display hardware */
	LCD_WRITE_CMD(0x2B);
    //LCD_WRITE_DATA(0x0);        
	LCD_WRITE_DATA16((uint16)start_row);
    //LCD_WRITE_DATA(0x0);        
	LCD_WRITE_DATA16((uint16)end_row);
	
	LCD_WRITE_CMD(0x2C);
}


static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x28); //Sleep in
        LCD_WRITE_CMD(0x10);
        LCD_DELAY(120);
    }
    else
    {
        LCD_WRITE_CMD(0x11);	// Sleep out
        LCD_DELAY(120); 
        LCD_WRITE_CMD(0x29);	// Display on
      //reg++;
        disp_ic_init();
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_ili9341_tft23_rot(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
    
    // Read ID
    LCD_WRITE_CMD(0xD3);
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
    
    if(id2 == 0x93 && id3 == 0x41)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}

