#include "disp_drv.h"

static void disp_ic_init(void)
{
    //--------------------------------End ST7715R Reset Sequence --------------------------------------//
    LCD_WRITE_CMD(0x11); //Sleep out
    LCD_DELAY (120); //Delay 120ms
    //------------------------------------ST7715R Frame rate-----------------------------------------//
    LCD_WRITE_CMD(0xB1); //Frame rate 80Hz
    LCD_WRITE_DATA(0x02);  //0x02
    LCD_WRITE_DATA(0x35);
    LCD_WRITE_DATA(0x36);
    LCD_WRITE_CMD(0xB2); //Frame rate 80Hz
    LCD_WRITE_DATA(0x02);   //0x02
    LCD_WRITE_DATA(0x35);
    LCD_WRITE_DATA(0x36);
    LCD_WRITE_CMD(0xB3); //Frame rate 80Hz
    LCD_WRITE_DATA(0x02);   //0x02
    LCD_WRITE_DATA(0x35);
    LCD_WRITE_DATA(0x36);
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x35);
    LCD_WRITE_DATA(0x36);
    //------------------------------------End ST7715R Frame rate-----------------------------------------//
    LCD_WRITE_CMD(0xB4); //Column inversion
    LCD_WRITE_DATA(0x07);
    //------------------------------------ST7715R Power Sequence-----------------------------------------//
    LCD_WRITE_CMD(0xC0);
    LCD_WRITE_DATA(0xA2);
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x84);
    LCD_WRITE_CMD(0xC1);
    LCD_WRITE_DATA(0xC5);
    LCD_WRITE_CMD(0xC2);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_CMD(0xC3);
    LCD_WRITE_DATA(0x8A);
    LCD_WRITE_DATA(0x2A);
    LCD_WRITE_CMD(0xC4);
    LCD_WRITE_DATA(0x8A);
    LCD_WRITE_DATA(0xEE);
    //---------------------------------End ST7715R Power Sequence-------------------------------------//
    LCD_WRITE_CMD(0xC5); //VCOM    
    LCD_WRITE_DATA(0x07);   //0x03
    LCD_WRITE_CMD(0x36); //MX, MY, RGB mode
    LCD_WRITE_DATA(0xC8);
    //------------------------------------ST7715R Gamma Sequence-----------------------------------------//
    LCD_WRITE_CMD(0xe0);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x1c);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x18);
    LCD_WRITE_DATA(0x33);
    LCD_WRITE_DATA(0x2c);
    LCD_WRITE_DATA(0x25);
    LCD_WRITE_DATA(0x28);
    LCD_WRITE_DATA(0x28);
    LCD_WRITE_DATA(0x27);
    LCD_WRITE_DATA(0x2f);
    LCD_WRITE_DATA(0x3c);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x10);

    LCD_WRITE_CMD(0xe1);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x1c);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x18);
    LCD_WRITE_DATA(0x2d);
    LCD_WRITE_DATA(0x28);
    LCD_WRITE_DATA(0x23);
    LCD_WRITE_DATA(0x28);
    LCD_WRITE_DATA(0x28);
    LCD_WRITE_DATA(0x26);
    LCD_WRITE_DATA(0x2f);
    LCD_WRITE_DATA(0x3b);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x10);

    //LCD_WRITE_CMD(0x2a);
    //LCD_WRITE_DATA(0x00);
    //LCD_WRITE_DATA(0x02);
    //LCD_WRITE_DATA(0x00);
    //LCD_WRITE_DATA(0x81);
      
    //LCD_WRITE_CMD(0x2B);
    //LCD_WRITE_DATA(0x00);
    //LCD_WRITE_DATA(0x03);
    //LCD_WRITE_DATA(0x00);
    //LCD_WRITE_DATA(0x82);

    //------------------------------------End ST7715R Gamma Sequence-----------------------------------------//
    LCD_WRITE_CMD(0xF0); //Enable test command
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_CMD(0xF6); //Disable ram power save mode
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_CMD(0x3A); //65k mode
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_CMD(0x29); //Display on
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

static void disp_ic_bitblt(void *src_ptr, dword copy_count)
{
    register uint16 *pdata = src_ptr;
    register uint16  data;
    dword   mod_count;
    uint16 *pend;
    
    mod_count   = copy_count&0x07;
    copy_count -= mod_count;
    pend  = pdata + copy_count;
    
    while(pdata<pend)
    {
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
    }
    
    while(mod_count--)
    {
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
    }
}

static void disp_ic_set(uint32 src, dword copy_count)
{
    dword   mod_count;
    
    mod_count   = copy_count&0x07;
    copy_count  = copy_count>>3;
    
    while(copy_count--)
    {
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
    }
    
    while(mod_count--)
    {
        LCD_WRITE_DATA16(src);
    }
}

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x28); 
        LCD_DELAY(1);
        LCD_WRITE_CMD(0x10); //Sleep in
        LCD_DELAY(120);
    }
    else
    {
        LCD_WRITE_CMD(0x11); //Exit Sleep
        LCD_DELAY(120);
        LCD_WRITE_CMD(0x29); 
        LCD_DELAY(1);
    }
}

boolean disp_st7735r_tft144(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
    
    LCD_WRITE_CMD(0x04);
    LCD_READ_DATA();
    id1=LCD_READ_DATA();
    id2=LCD_READ_DATA();
    id3=LCD_READ_DATA();
    
    if(id1 == 0x1 && id2 == 0x1 && id3 == 0x1)
    {
        pdispic->disp_ic_init       = disp_ic_init;
        pdispic->disp_ic_setwindow  = disp_ic_setwindow;
        pdispic->disp_ic_bitblt     = disp_ic_bitblt;
        pdispic->disp_ic_set        = disp_ic_set;
        pdispic->disp_ic_sleep      = disp_ic_sleep;
        return TRUE;
    }
    return FALSE;
}
