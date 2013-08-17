#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   160

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ST7735S driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ST7735S[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ST7735S[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
    return LCD_16BPP_88_II;
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_ST7735S[0][0];
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

static void disp_ic_init(void)
{

#if defined(FEATURE_VERSION_ESIA)||defined(FEATURE_VERSION_W021_GPLUS)
	// 这是力合3月份的玻璃参数
    LCD_WRITE_CMD(0x11); //Sleep out
    LCD_DELAY(120); 	            // Must Delay At Least 100 ms
    LCD_WRITE_CMD (0xB1);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x05);
	LCD_WRITE_CMD (0xB2);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x2C);
	LCD_WRITE_DATA(0x2D);
	LCD_WRITE_CMD (0xB3);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x2C);
	LCD_WRITE_DATA(0x2D);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x2C);
	LCD_WRITE_DATA(0x2D);
	LCD_WRITE_CMD (0xB4); //Column inversion
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD (0xC0);
	LCD_WRITE_DATA(0xA2);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x84);
	LCD_WRITE_CMD (0xC1);
	LCD_WRITE_DATA(0xCF);
	LCD_WRITE_CMD (0xC2);
	LCD_WRITE_DATA(0x0D);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD (0xC3);
	LCD_WRITE_DATA(0x8A);
	LCD_WRITE_DATA(0x2A);
	LCD_WRITE_CMD (0xC4);
	LCD_WRITE_DATA(0x8A);
	LCD_WRITE_DATA(0xEE);
	LCD_WRITE_CMD (0xC5); //VCOM
	LCD_WRITE_DATA(0x2c);    
	LCD_WRITE_CMD (0x36); //MX, MY, RGB mode	
	LCD_WRITE_DATA(0xC8);	
	LCD_WRITE_CMD (0xe0);
	LCD_WRITE_DATA(0x05);    
	LCD_WRITE_DATA(0x11);
	LCD_WRITE_DATA(0x0e);
	LCD_WRITE_DATA(0x19);
	LCD_WRITE_DATA(0x3a);
	LCD_WRITE_DATA(0x33);
	LCD_WRITE_DATA(0x29);
	LCD_WRITE_DATA(0x2a);
	LCD_WRITE_DATA(0x26);
	LCD_WRITE_DATA(0x21);
	LCD_WRITE_DATA(0x26);
	LCD_WRITE_DATA(0x3b);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_CMD (0xe1);
	LCD_WRITE_DATA(0x05);    
	LCD_WRITE_DATA(0x11);
	LCD_WRITE_DATA(0x0e);
	LCD_WRITE_DATA(0x19);
	LCD_WRITE_DATA(0x36);
	LCD_WRITE_DATA(0x30);
	LCD_WRITE_DATA(0x2b);
	LCD_WRITE_DATA(0x30);
	LCD_WRITE_DATA(0x31);
	LCD_WRITE_DATA(0x31);
	LCD_WRITE_DATA(0x3b);
	LCD_WRITE_DATA(0x3f);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_CMD (0x3A); //65k mode
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_CMD (0x29); //Display on
	LCD_WRITE_CMD(0x2c);
#elif defined(FEATURE_VERSION_V3CM301)
//财富十二部，换新屏
	LCD_WRITE_CMD(0x11); //Sleep out
    LCD_DELAY(120); 	            // Must Delay At Least 100 ms
    LCD_WRITE_CMD (0xB1);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x05);
	LCD_WRITE_CMD (0xB2);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x2C);
	LCD_WRITE_DATA(0x2D);
	LCD_WRITE_CMD (0xB3);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x2C);
	LCD_WRITE_DATA(0x2D);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x2C);
	LCD_WRITE_DATA(0x2D);
	LCD_WRITE_CMD (0xB4); //Column inversion
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD (0xC0);
	LCD_WRITE_DATA(0xA2);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x84);
	LCD_WRITE_CMD (0xC1);
	LCD_WRITE_DATA(0xCF);
	LCD_WRITE_CMD (0xC2);
	LCD_WRITE_DATA(0x0D);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD (0xC3);
	LCD_WRITE_DATA(0x8A);
	LCD_WRITE_DATA(0x2A);
	LCD_WRITE_CMD (0xC4);
	LCD_WRITE_DATA(0x8A);
	LCD_WRITE_DATA(0xEE);
	LCD_WRITE_CMD (0xC5); //VCOM
	LCD_WRITE_DATA(0x2c);    
	LCD_WRITE_CMD (0x36); //MX, MY, RGB mode	
	LCD_WRITE_DATA(0xC0);	
	LCD_WRITE_CMD (0xe0);
	LCD_WRITE_DATA(0x05);    
	LCD_WRITE_DATA(0x11);
	LCD_WRITE_DATA(0x0e);
	LCD_WRITE_DATA(0x19);
	LCD_WRITE_DATA(0x3a);
	LCD_WRITE_DATA(0x33);
	LCD_WRITE_DATA(0x29);
	LCD_WRITE_DATA(0x2a);
	LCD_WRITE_DATA(0x26);
	LCD_WRITE_DATA(0x21);
	LCD_WRITE_DATA(0x26);
	LCD_WRITE_DATA(0x3b);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_CMD (0xe1);
	LCD_WRITE_DATA(0x05);    
	LCD_WRITE_DATA(0x11);
	LCD_WRITE_DATA(0x0e);
	LCD_WRITE_DATA(0x19);
	LCD_WRITE_DATA(0x36);
	LCD_WRITE_DATA(0x30);
	LCD_WRITE_DATA(0x2b);
	LCD_WRITE_DATA(0x30);
	LCD_WRITE_DATA(0x31);
	LCD_WRITE_DATA(0x31);
	LCD_WRITE_DATA(0x3b);
	LCD_WRITE_DATA(0x3f);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_CMD (0x3A); //65k mode
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_CMD (0x29); //Display on
	LCD_WRITE_CMD(0x2c);
#else    
    LCD_WRITE_CMD(0x11); 
    //----------------------------------
    LCD_WRITE_CMD(0xB1); 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_CMD(0xB2); 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_CMD(0xB3); 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_DATA(0x3A); 
    //------------------------------------End ST7735S Fra
    LCD_WRITE_CMD(0xB4); //Dot inversion 
    LCD_WRITE_DATA(0x03); 
     //------------------------------------ST7735S P
    LCD_WRITE_CMD(0xC0); 
    LCD_WRITE_DATA(0x28); 
    LCD_WRITE_DATA(0x08); 
    LCD_WRITE_DATA(0x84); 
    LCD_WRITE_CMD(0xC1); 
    LCD_WRITE_DATA(0XC0); 
    LCD_WRITE_CMD(0xC2); 
    LCD_WRITE_DATA(0x0D); 
    LCD_WRITE_DATA(0x00); 
    LCD_WRITE_CMD(0xC3); 
    LCD_WRITE_DATA(0x8D); 
    LCD_WRITE_DATA(0x2A); 
    LCD_WRITE_CMD(0xC4); 
    LCD_WRITE_DATA(0x8D); 
    LCD_WRITE_DATA(0xEE); 
    //---------------------------------End ST7
     
    LCD_WRITE_CMD(0xC5); //VCOM 
    LCD_WRITE_DATA(0x0C); 
     
    LCD_WRITE_CMD(0x36); //MX, MY,
    LCD_WRITE_DATA(0xC8); 
    //------------------------------------ST773
     
    LCD_WRITE_CMD(0xE0); 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_DATA(0x1A); 
    LCD_WRITE_DATA(0x0C); 
    LCD_WRITE_DATA(0x0E); 
    LCD_WRITE_DATA(0x3A); 
    LCD_WRITE_DATA(0x34); 
    LCD_WRITE_DATA(0x2D); 
    LCD_WRITE_DATA(0x2F); 
    LCD_WRITE_DATA(0x2D); 
    LCD_WRITE_DATA(0x2A); 
    LCD_WRITE_DATA(0x2F); 
    LCD_WRITE_DATA(0x3C); 
    LCD_WRITE_DATA(0x00); 
    LCD_WRITE_DATA(0x01); 
    LCD_WRITE_DATA(0x02); 
    LCD_WRITE_DATA(0x10); 
    LCD_WRITE_CMD(0xE1); 
    LCD_WRITE_DATA(0x04); 
    LCD_WRITE_DATA(0x1B); 
    LCD_WRITE_DATA(0x0D); 
    LCD_WRITE_DATA(0x0E); 
    LCD_WRITE_DATA(0x2D); 
    LCD_WRITE_DATA(0x29); 
    LCD_WRITE_DATA(0x24); 
    LCD_WRITE_DATA(0x29); 
    LCD_WRITE_DATA(0x28); 
    LCD_WRITE_DATA(0x26); 
    LCD_WRITE_DATA(0x31); 
    LCD_WRITE_DATA(0x3B); 
    LCD_WRITE_DATA(0x00); 
    LCD_WRITE_DATA(0x00); 
    LCD_WRITE_DATA(0x03); 
    LCD_WRITE_DATA(0x12); 
    //------------------------------------End ST7735S G
     
    LCD_WRITE_CMD(0xFC); //Enable Gate Pum
    LCD_WRITE_DATA(0x8C); 
     
    LCD_WRITE_CMD(0x3A); //65k mode 
    LCD_WRITE_DATA(0x05); 
    LCD_WRITE_CMD(0x21); 
    LCD_WRITE_CMD(0x29); //Display on 
    LCD_WRITE_CMD(0x2c);	
	
	#endif

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
    return;
}

boolean disp_st7735s_tft177(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2;
	uint8 id3;
    
    // Read ID
    LCD_WRITE_CMD(0xDA);  //5C
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDB);  //89
    id2 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDC);   //F0
    id3 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
    // 5C 89 F0
    if(id1 == 0x7C) //|| id3 == 0xF0
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
