#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"
#define ZGD_TFT_020_T087
#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA NT35601 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_NT35601[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_NT35601[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_NT35601[0][0];
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

static void disp_ic_init(void)
{
    LCD_WRITE_CMD(0x01);// Software Reset
    LCD_DELAY(20);
    
    LCD_WRITE_CMD(0x11);//Sleep Out
    LCD_DELAY(120);
    
#ifdef ZGD_TFT_020_T087
  //zgd tft-020-t087
    LCD_WRITE_CMD(0xC0);
    LCD_WRITE_DATA (0x03);
    LCD_WRITE_DATA (0x00);

    LCD_WRITE_CMD(0xC2);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x00);

    LCD_WRITE_CMD(0xC3);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x07);

    LCD_WRITE_CMD(0xC4);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x04);

    LCD_WRITE_CMD(0xC5);
    LCD_WRITE_DATA (0x40);
    LCD_WRITE_DATA (0x4A);

    LCD_WRITE_CMD(0xC7);
    LCD_WRITE_DATA (0xC8);//0xCB 0xD4  0xC8  0xCA

    LCD_WRITE_CMD(0xB1);   //Frame Rate Control (In normal mode/ Full colors)
    LCD_WRITE_DATA (0x1A);//0x12   0x16
    LCD_WRITE_DATA (0x0A);//0x04   0x0A

    LCD_WRITE_CMD(0x3A);
    LCD_WRITE_DATA (0x55);

    LCD_WRITE_CMD(0x36);
    LCD_WRITE_DATA (0x48);//0xA8  0x28 0xE8 

    LCD_WRITE_CMD(0xC9);
    LCD_WRITE_DATA (0x08);

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA (0x60);
    LCD_WRITE_DATA (0x76);
    LCD_WRITE_DATA (0x25);
    LCD_WRITE_DATA (0x07);
    LCD_WRITE_DATA (0xA0);
    LCD_WRITE_DATA (0x0B);
    LCD_WRITE_DATA (0x0F);
    LCD_WRITE_DATA (0xA0);
    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA (0x30);
    LCD_WRITE_DATA (0x30);
    LCD_WRITE_DATA (0x62);
    LCD_WRITE_DATA (0x0F);
    LCD_WRITE_DATA (0x62);
    LCD_WRITE_DATA (0x0E);
    LCD_WRITE_DATA (0x08);
    LCD_WRITE_DATA (0x00);

    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0xAF);
    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0x00);
    LCD_WRITE_DATA (0xDB);

    LCD_WRITE_CMD(0x29);
    LCD_WRITE_CMD(0x2c);
#else
   LCD_WRITE_CMD(0xDE);
   LCD_WRITE_DATA(0xAA);
   LCD_WRITE_DATA(0x55);
   LCD_WRITE_DATA(0x15);
   
   LCD_WRITE_CMD(0xC1); 
   LCD_WRITE_DATA(0x03);

   LCD_WRITE_CMD(0xC0); 
   LCD_WRITE_DATA(0x03);
   LCD_WRITE_DATA(0x00);

   LCD_WRITE_CMD(0xC2);
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_DATA(0x04);

   LCD_WRITE_CMD(0xC3); 
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_DATA(0x07);
   
   LCD_WRITE_CMD(0xC4); 
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_DATA(0x04);
   
   LCD_WRITE_CMD(0xC5);//Set Vcom  
   LCD_WRITE_DATA(0x36);
   LCD_WRITE_DATA(0x1C);
   
   LCD_WRITE_CMD(0xC7);//Set VCOM-OFFSET   
   LCD_WRITE_DATA(0xD4);//
   
   LCD_WRITE_CMD(0xb1); 
   LCD_WRITE_DATA(0x12);
   LCD_WRITE_DATA(0x0D);
   
   LCD_WRITE_CMD(0x3A); 
   LCD_WRITE_DATA(0x55);
   
   LCD_WRITE_CMD(0x36);//Memory data  access control
   LCD_WRITE_DATA(0x48);//MY MX MV ML RGB MH 0 0     
   
   LCD_WRITE_CMD(0xE0);//E0H Set
   LCD_WRITE_DATA(0x30);
   LCD_WRITE_DATA(0x53);
   LCD_WRITE_DATA(0x50);
   LCD_WRITE_DATA(0x05);
   LCD_WRITE_DATA(0x6B);
   LCD_WRITE_DATA(0x03);
   LCD_WRITE_DATA(0x04);
   LCD_WRITE_DATA(0x95);
   
   LCD_WRITE_CMD(0xE1);//E1H Set
   LCD_WRITE_DATA(0x71);
   LCD_WRITE_DATA(0x42);
   LCD_WRITE_DATA(0x64);
   LCD_WRITE_DATA(0x04);
   LCD_WRITE_DATA(0x73);
   LCD_WRITE_DATA(0x0E);
   LCD_WRITE_DATA(0x03);
   LCD_WRITE_DATA(0x1F);
   
   LCD_WRITE_CMD(0x29);//display on
#endif
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA16((uint16)start_col);
    LCD_WRITE_DATA16((uint16)end_col);
    
    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA16((uint16)start_row);
    LCD_WRITE_DATA16((uint16)end_row);
    
    LCD_WRITE_CMD(0x2C);
}

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x28);
        LCD_DELAY(50);
        LCD_WRITE_CMD(0x10);
    }
    else
    {
        LCD_WRITE_CMD(0x11);
        LCD_DELAY(120);
        LCD_WRITE_CMD(0x29);
    }
}

static void disp_ic_rot(uint16 degree)
{
#if 0
    if(degree == 0)
    {
        LCD_WRITE_CMD(0x36); LCD_WRITE_DATA(0x48); // Set VCOMH/VCOML voltage   0x333A
    }
    else
    {
        LCD_WRITE_CMD(0x36); LCD_WRITE_DATA(0x28); // Set VCOMH/VCOML voltage   0x333A
    }
#endif
    return;
}

boolean disp_nt35601_tft20(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
    
    // Read ID
    LCD_WRITE_CMD(0xD3);
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
    
    if(id1 == 0x01 && id2 == 0x15)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
