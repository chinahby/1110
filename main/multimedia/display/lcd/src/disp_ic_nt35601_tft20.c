#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

static void disp_ic_init(void)
{
    LCD_WRITE_CMD(0x01);// Software Reset
    LCD_DELAY(20);
    
    LCD_WRITE_CMD(0x11);//Sleep Out
    LCD_DELAY(120);
    
    LCD_WRITE_CMD(0xDE);
    LCD_WRITE_DATA(0xAA);
    LCD_WRITE_DATA(0x55);
    LCD_WRITE_DATA(0x15);
    
    LCD_WRITE_CMD(0xC2);
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x04);
    
    LCD_WRITE_CMD(0xC0); 
    LCD_WRITE_DATA(0x15);
    LCD_WRITE_DATA(0x02);
    
    LCD_WRITE_CMD(0xC1); 
    LCD_WRITE_DATA(0x07);
    
    LCD_WRITE_CMD(0xC3); 
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x07);
    
    LCD_WRITE_CMD(0xC4); 
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x04);
    
    LCD_WRITE_CMD(0xC5);//Set Vcom  
    LCD_WRITE_DATA(0x21);
    LCD_WRITE_DATA(0x31);
    
    LCD_WRITE_CMD(0xC7);//Set VCOM-OFFSET   
    LCD_WRITE_DATA(0xAF);//
    
    LCD_WRITE_CMD(0xb1); 
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x0A);
    
    LCD_WRITE_CMD(0xF9); 
    LCD_WRITE_DATA(0x08);
    
    LCD_WRITE_CMD(0x3A); 
    LCD_WRITE_DATA(0x55);
    
    LCD_WRITE_CMD(0x36);//Memory data  access control
    LCD_WRITE_DATA(0x48);//MY MX MV ML RGB MH 0 0     
    
    LCD_WRITE_CMD(0xE0);//E0H Set
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x57);
    LCD_WRITE_DATA(0x71);
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0xE1);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0xBE);
    
    LCD_WRITE_CMD(0xE1);//E1H Set
    LCD_WRITE_DATA(0x32);
    LCD_WRITE_DATA(0x11);
    LCD_WRITE_DATA(0x77);
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0xE7);
    LCD_WRITE_DATA(0x0D);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x2F);
    
    LCD_WRITE_CMD(0x29);//display on
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
