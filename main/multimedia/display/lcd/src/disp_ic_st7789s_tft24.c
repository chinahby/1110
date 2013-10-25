#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   240

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ST7789S driver

#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ST7789S[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ST7789S[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_ST7789S[0][0];
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
#if defined (FEATURE_VERSION_IC241A_MMX)
        //--************ Start Initial Sequence **********--/
	    //LCD_WRITE_CMD(0x01);
		LCD_DELAY(1); // Delay 1ms
		LCD_DELAY(10); // Delay 10ms, This delay time is necessary
		LCD_DELAY(120); // Delay 50 ms
		//************* Start Initial Sequence **********//
		//************* Start Initial Sequence **********//


		//************* Start Initial Sequence **********//
		
		//-----------------------------------ST7789S reset sequence------------------------------------//
		
		LCD_WRITE_CMD(0x11);
		LCD_DELAY(120); //Delay 120ms
		//--------------------------------------Display Setting------------------------------------------//
		LCD_WRITE_CMD(0x36);
		LCD_WRITE_DATA(0x88);       
		LCD_WRITE_CMD(0x3a);
		LCD_WRITE_DATA(0x55);
		//--------------------------------ST7789S Frame rate setting----------------------------------//
		LCD_WRITE_CMD(0xb2);
		LCD_WRITE_DATA(0x0c);
		LCD_WRITE_DATA(0x0c);
		LCD_WRITE_DATA(0x00);
		LCD_WRITE_DATA(0x33);
		LCD_WRITE_DATA(0x33);
		LCD_WRITE_CMD(0xb7);
		LCD_WRITE_DATA(0x35);
		//---------------------------------ST7789S Power setting--------------------------------------//
		LCD_WRITE_CMD(0xbb);
		LCD_WRITE_DATA(0x0e);
		LCD_WRITE_CMD(0xc0);
		LCD_WRITE_DATA(0x2c);
		LCD_WRITE_CMD(0xc2);
		LCD_WRITE_DATA(0x01);
		LCD_WRITE_CMD(0xc3);
		LCD_WRITE_DATA(0x12);
		LCD_WRITE_CMD(0xc4);
		LCD_WRITE_DATA(0x20);
		LCD_WRITE_CMD(0xc6);
		LCD_WRITE_DATA(0x0f);
		LCD_WRITE_CMD(0xd0);
		LCD_WRITE_DATA(0xa4);
		LCD_WRITE_DATA(0xa1);
		//--------------------------------ST7789S gamma setting---------------------------------------//
		LCD_WRITE_CMD(0xe0);
		LCD_WRITE_DATA(0xd0);
		LCD_WRITE_DATA(0x00);
		LCD_WRITE_DATA(0x05);
		LCD_WRITE_DATA(0x0d);
		LCD_WRITE_DATA(0x0f);
		LCD_WRITE_DATA(0x2b);
		LCD_WRITE_DATA(0x3b);
		LCD_WRITE_DATA(0x44);
		LCD_WRITE_DATA(0x4a);
		LCD_WRITE_DATA(0x18);
		LCD_WRITE_DATA(0x0f);
		LCD_WRITE_DATA(0x0b);
		LCD_WRITE_DATA(0x1f);
		LCD_WRITE_DATA(0x22);
		
		LCD_WRITE_CMD(0xe1);
		LCD_WRITE_DATA(0xd0);
		LCD_WRITE_DATA(0x00);
		LCD_WRITE_DATA(0x05);
		LCD_WRITE_DATA(0x0d);
		LCD_WRITE_DATA(0x10);
		LCD_WRITE_DATA(0x2b);
		LCD_WRITE_DATA(0x3d);
		LCD_WRITE_DATA(0x54);
		LCD_WRITE_DATA(0x50);
		LCD_WRITE_DATA(0x2f);
		LCD_WRITE_DATA(0x1f);
		LCD_WRITE_DATA(0x1f);
		LCD_WRITE_DATA(0x1f);
		LCD_WRITE_DATA(0x23);
		LCD_WRITE_CMD(0x29);	
#else
	//----------------------------------End ST7775R Reset Sequence ------------------------------------// 
	//------------------------------------Display Control Setting----------------------------------------------// 
    LCD_WRITE_CMD(0x11);
    LCD_DELAY(120); //Delay 120ms
//--------------------------------ST7789S Frame rate setting----------------------------------//
    LCD_WRITE_CMD(0xb2);
    LCD_WRITE_DATA(0x0c);
    LCD_WRITE_DATA(0x0c);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x33);
    LCD_WRITE_DATA(0x33);
    LCD_WRITE_CMD(0xb7);
    LCD_WRITE_DATA(0x35);
    //---------------------------------ST7789S Power setting--------------------------------------//
    LCD_WRITE_CMD(0xbb);
    LCD_WRITE_DATA(0x2b);
    LCD_WRITE_CMD(0xc3);
    LCD_WRITE_DATA(0x17);
    LCD_WRITE_CMD(0xc4);
    LCD_WRITE_DATA(0x20);
    LCD_WRITE_CMD(0xc6);
    LCD_WRITE_DATA(0x0f);
    LCD_WRITE_CMD(0xd0);
    LCD_WRITE_DATA(0xa4);
    LCD_WRITE_DATA(0xa2);
    //--------------------------------ST7789S gamma setting---------------------------------------//
    LCD_WRITE_CMD(0xe0);
    LCD_WRITE_DATA(0xf0);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x0a);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x1b);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x44);
    LCD_WRITE_DATA(0x47);
    LCD_WRITE_DATA(0x28);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x16);
    LCD_WRITE_DATA(0x1b);
    LCD_WRITE_CMD(0xe1);
    LCD_WRITE_DATA(0xf0);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x0a);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x11);
    LCD_WRITE_DATA(0x1a);
    LCD_WRITE_DATA(0x3b);
    LCD_WRITE_DATA(0x34);
    LCD_WRITE_DATA(0x4e);
    LCD_WRITE_DATA(0x3a);
    LCD_WRITE_DATA(0x17);
    LCD_WRITE_DATA(0x16);
    LCD_WRITE_DATA(0x21);
    LCD_WRITE_DATA(0x22);

    LCD_WRITE_CMD(0x3a);
    LCD_WRITE_DATA(0x55);
    LCD_WRITE_CMD(0x29);
    
    
    LCD_WRITE_CMD(0x2C);//Memory Write
#endif    
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA((uint8)(start_col>>8));
    LCD_WRITE_DATA((uint8)start_col);
    LCD_WRITE_DATA((uint8)(end_col>>8));
    LCD_WRITE_DATA((uint8)end_col);
    
    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA((uint8)(start_row>>8));        
    LCD_WRITE_DATA((uint8)start_row);
    LCD_WRITE_DATA((uint8)(end_row>>8));        
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

boolean disp_st7789s_tft24_rot(disp_drv_ic_type *pdispic)
{

	uint8 tid3,tid4;

    // Read ID
    LCD_WRITE_CMD(0x00);  //5C
    tid3 = LCD_READ_DATA();
    tid4 = LCD_READ_DATA();

    //if((tid3 == 0x77) || (tid4 == 0x75))
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
