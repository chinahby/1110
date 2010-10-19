#include "disp_drv.h"

static void disp_ic_init(void)
{
    //--************ Start Initial Sequence **********--/
    LCD_WRITE_CMD(0x01);
    LCD_DELAY(150);
    
	LCD_WRITE_CMD(0xB9);
	LCD_WRITE_DATA(0xFF);
	LCD_WRITE_DATA(0x93);
	LCD_WRITE_DATA(0x42);

	LCD_WRITE_CMD(0x21);

	LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA(0xC8);  //08

	LCD_WRITE_CMD(0x3A);
	LCD_WRITE_DATA(0x05);



	LCD_WRITE_CMD(0xC0);
	LCD_WRITE_DATA(0x1D);  //0x25
	LCD_WRITE_DATA(0x0A);  //0x0A

	LCD_WRITE_CMD(0xC1);
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0xC5);
	LCD_WRITE_DATA(0x2F);//0x2F
	LCD_WRITE_DATA(0x27);//0x27

	LCD_WRITE_CMD(0xC7);
	LCD_WRITE_DATA(0xA4); //0xD3

	LCD_WRITE_CMD(0xB8);
	LCD_WRITE_DATA(0x0B);


	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x0F);
	LCD_WRITE_DATA(0x24);
	LCD_WRITE_DATA(0x21);
	LCD_WRITE_DATA(0x0C);
	LCD_WRITE_DATA(0x0F);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x50);
	LCD_WRITE_DATA(0x75);
	LCD_WRITE_DATA(0x3F);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x12);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x11);
	LCD_WRITE_DATA(0x0B);
	LCD_WRITE_DATA(0x08);

	LCD_WRITE_CMD(0xE1);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x1d);
	LCD_WRITE_DATA(0x20);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x0E);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x31);
	LCD_WRITE_DATA(0x24);
	LCD_WRITE_DATA(0x42);
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x0B);
	LCD_WRITE_DATA(0x09);
	LCD_WRITE_DATA(0x30);
	LCD_WRITE_DATA(0x36);
	LCD_WRITE_DATA(0x0F);

	LCD_WRITE_CMD(0xF2);
	LCD_WRITE_DATA(0x00);





	LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(10);
	LCD_WRITE_CMD(0x11); //Exit Sleep
	LCD_DELAY(80);
	LCD_WRITE_CMD(0x29); //Display On



	LCD_WRITE_CMD(0x2A);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x3F);


	LCD_WRITE_CMD(0x2B);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0xEF);

	LCD_WRITE_CMD(0x2C);   //write data
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
        LCD_WRITE_CMD(0x28); //Sleep in
        LCD_WRITE_CMD(0x10);
        LCD_DELAY(120);
    }
    else
    {
    	LCD_WRITE_CMD(0x11); // Standby out
		LCD_DELAY(10);
		LCD_WRITE_CMD(0x11); //Exit Sleep
		LCD_DELAY(80);
		LCD_WRITE_CMD(0x29); // Display on
        LCD_DELAY(10);
    }
}

boolean disp_ili9342_tft23_rot(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
    
    // Read ID
    LCD_WRITE_CMD(0xD3);
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
    
    if(id2 == 0x93 && id3 == 0x42)
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

