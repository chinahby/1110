#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   240//320

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ILI9301 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write
 uint16 i ,j;
#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ILI9301[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ILI9301[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
#endif
{
  NOP,               //0
  NOP,               //1
  SET_LCD_CNTL_ADDR, //2 set LCD command port address
  LCD_CMD_WH,    //3

  SET_LCD_DATA_ADDR, //4 set LCD data port address
  LCD_DATA_WH,   //5

  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR)),//6
  NULL,             //7
  NULL,             //8
  NULL,             //10
  NULL,             //11
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR)),//12
  NULL,             //13
  NULL,             //14
  NULL,             //16
  NULL,             //17
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_RAMWR)),//24

  RETURN            //25
};

static uint8 disp_ic_mdp_getformat(void)
{
    return LCD_16BPP_88_II; //8 16BPP 0 //18BPP
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_ILI9301[0][0];
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

//static unsigned VCOM_Offset=0x5a6c;
static void disp_ic_init(void)
{
    #if 0
    LCD_WRITE_CMD(0xb0);
	LCD_WRITE_DATA16(0x40);
	LCD_WRITE_CMD(0xb4);
	LCD_WRITE_DATA16(0x00);
	LCD_WRITE_CMD(0xb5);
	LCD_WRITE_DATA16(0x02);
	LCD_WRITE_DATA16(0x02);
	LCD_WRITE_DATA16(0x0a);
	LCD_WRITE_DATA16(0x14);
    LCD_WRITE_CMD(0xb6);
	LCD_WRITE_DATA16(0x0a);
	LCD_WRITE_DATA16(0xa2);
    #endif
	LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA16(0x48);
	LCD_WRITE_CMD(0x3a);
	LCD_WRITE_DATA16(0x05);

    LCD_WRITE_CMD(0xfe);
	LCD_WRITE_CMD(0xef);  //analog voltage adjust enable
    LCD_WRITE_CMD(0xe2);  
	LCD_WRITE_DATA16(0x80);	
	LCD_WRITE_CMD(0xa3);  //set Frame rate
	LCD_WRITE_DATA16(0x06);
	LCD_WRITE_CMD(0xa4); //set VRH[3:0]
	LCD_WRITE_DATA16(0x0a);
	LCD_WRITE_CMD(0xff); //set VDV[4:0]	
	LCD_WRITE_DATA16(0x0b);
	LCD_WRITE_CMD(0xfd);
	LCD_WRITE_DATA16(0x12); // set VCM[5:0]0x17
	

    LCD_WRITE_CMD(0xf5);  //set Gamma 
    LCD_WRITE_DATA16(0x00);
    LCD_WRITE_CMD(0xf4);
    LCD_WRITE_DATA16(0x06);
    LCD_WRITE_CMD(0xfb);
    LCD_WRITE_DATA16(0x00);
    LCD_WRITE_CMD(0xfc);
    LCD_WRITE_DATA16(0x0c);
    LCD_WRITE_CMD(0xf3);
    LCD_WRITE_DATA16(0x50);
    LCD_WRITE_CMD(0xfa);
    LCD_WRITE_DATA16(0x05);

	LCD_WRITE_CMD(0x11);
	LCD_DELAY(120);
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
        LCD_WRITE_CMD(0x28); // Display off
        LCD_DELAY(20);
        LCD_WRITE_CMD(0x10); // Enter Sleep mode
        //LCD_WRITE_CMD(0x07);
        //LCD_WRITE_DATA16(0x0000);
        //LCD_DELAY(50);

       // LCD_WRITE_CMD(0x10);
       // LCD_WRITE_DATA16(0x0001); // Enter Standby mode
    }
    else
    {

        disp_ic_init();
        
       // LCD_WRITE_CMD(0x11); // Sleep out
       // LCD_DELAY(120);
        //LCD_WRITE_CMD(0x29); // Display on
        //LCD_WRITE_CMD(0x10);
        //LCD_WRITE_DATA16(0x0000);
       // LCD_DELAY(50);

       // LCD_WRITE_CMD(0x07);
       // LCD_WRITE_DATA16(0x1017); //Exit Sleep
       // LCD_DELAY(50);

    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_GC9301_tft20_rot(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2;
    
    // Read ID
    LCD_WRITE_CMD(0x00);
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    
    //if(id1 == 0x92 && id2 == 0x25)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
