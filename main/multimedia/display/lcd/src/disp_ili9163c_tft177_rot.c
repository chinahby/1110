#include "disp_drv.h"

static void disp_ic_init(void)
{
#ifdef FEATURE_OLD_ROTLCD
    LCD_WRITE_CMD(0x11); //Exit Sleep
    LCD_DELAY(120);

    LCD_WRITE_CMD(0x26); //Set Default Gamma
    LCD_WRITE_DATA(0x04);

    LCD_WRITE_CMD(0xB1);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x10);

    LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_WRITE_DATA(0x02);

    LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_WRITE_DATA(0x4C);//0x4C
    LCD_WRITE_DATA(0x5E);

    LCD_WRITE_CMD(0xC7); 
    LCD_WRITE_DATA(0x00);

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
	LCD_WRITE_DATA(0x60);  //..0x60 //..0x40 //..0x20 //..0x00 //..0x80 //0xa0 //..0xc0 //0xe0

    LCD_WRITE_CMD(0xB7); //Set Source Output Direction
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xf2); //Enable Gamma bit
    LCD_WRITE_DATA(0x01);

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x3F);//p1
    LCD_WRITE_DATA(0x22);//p2
    LCD_WRITE_DATA(0x20);//p3
    LCD_WRITE_DATA(0x24);//p4
    LCD_WRITE_DATA(0x20);//p5
    LCD_WRITE_DATA(0x0C);//p6
    LCD_WRITE_DATA(0x4E);//p7
    LCD_WRITE_DATA(0xB7);//p8
    LCD_WRITE_DATA(0x3C);//p9
    LCD_WRITE_DATA(0x19);//p10
    LCD_WRITE_DATA(0x22);//p11
    LCD_WRITE_DATA(0x1E);//p12
    LCD_WRITE_DATA(0x02);//p13
    LCD_WRITE_DATA(0x01);//p14
    LCD_WRITE_DATA(0x00);//p15

    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00);//p1
    LCD_WRITE_DATA(0x1B);//p2
    LCD_WRITE_DATA(0x1F);//p3
    LCD_WRITE_DATA(0x0B);//p4
    LCD_WRITE_DATA(0x0F);//p5
    LCD_WRITE_DATA(0x13);//p6
    LCD_WRITE_DATA(0x31);//p7
    LCD_WRITE_DATA(0x84);//p8
    LCD_WRITE_DATA(0x43);//p9
    LCD_WRITE_DATA(0x06);//p10
    LCD_WRITE_DATA(0x1D);//p11
    LCD_WRITE_DATA(0x21);//p12
    LCD_WRITE_DATA(0x3D);//p13
    LCD_WRITE_DATA(0x3E);//p14
    LCD_WRITE_DATA(0x3F);//p15
    
    LCD_WRITE_CMD(0x29); // Display On
#else
    //************* Start Initial Sequence **********//
    LCD_WRITE_CMD(0x11); //Exit Sleep
    LCD_DELAY(120);
    
    LCD_WRITE_CMD(0xEC);
    LCD_WRITE_DATA(0x0C);
    
    LCD_WRITE_CMD(0x26); //Set Default Gamma
    LCD_WRITE_DATA(0x04);
    
    LCD_WRITE_CMD(0xB1);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x12);
    
    LCD_WRITE_CMD(0xB4);
    LCD_WRITE_DATA(0x06);
    
    LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x00);
    
    LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_WRITE_DATA(0x02);
    
    LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_WRITE_DATA(0x2C);
    LCD_WRITE_DATA(0x40);
    
    LCD_WRITE_CMD(0xC7);
    LCD_WRITE_DATA(0xC3);
    
    LCD_WRITE_CMD(0x3A); //Set Color Format
    LCD_WRITE_DATA(0x55);
    
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
    LCD_WRITE_DATA(0x68); // 0x60 For Old LCD 0x68 For New LCD
    
    LCD_WRITE_CMD(0xF2); //Enable Gamma bit
    LCD_WRITE_DATA(0x01);
    
    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x3F);//p1
    LCD_WRITE_DATA(0x18);//p2
    LCD_WRITE_DATA(0x18);//p3
    LCD_WRITE_DATA(0x26);//p4
    LCD_WRITE_DATA(0x20);//p5
    LCD_WRITE_DATA(0x0D);//p6  
    LCD_WRITE_DATA(0x46);//p7
    LCD_WRITE_DATA(0xF3);//p8
    LCD_WRITE_DATA(0x32);//p9
    LCD_WRITE_DATA(0x09);//p10
    LCD_WRITE_DATA(0x02);//p11
    LCD_WRITE_DATA(0x02);//p12
    LCD_WRITE_DATA(0x00);//p13
    LCD_WRITE_DATA(0x00);//p14
    LCD_WRITE_DATA(0x00);//p15
    
    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00);//p1
    LCD_WRITE_DATA(0x27);//p2
    LCD_WRITE_DATA(0x27);//p3
    LCD_WRITE_DATA(0x09);//p4
    LCD_WRITE_DATA(0x0F);//p5
    LCD_WRITE_DATA(0x12);//p6  
    LCD_WRITE_DATA(0x39);//p7
    LCD_WRITE_DATA(0xC0);//p8
    LCD_WRITE_DATA(0x4D);//p9
    LCD_WRITE_DATA(0x16);//p10
    LCD_WRITE_DATA(0x1D);//p11
    LCD_WRITE_DATA(0x2D);//p12
    LCD_WRITE_DATA(0x3F);//p13
    LCD_WRITE_DATA(0x3F);//p14
    LCD_WRITE_DATA(0x3F);//p15
    
    LCD_WRITE_CMD(0x29); // Display On
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
        LCD_WRITE_CMD(0x10); //Sleep in
        LCD_DELAY(120);
    }
    else
    {
        LCD_WRITE_CMD(0x11); //Exit Sleep
        LCD_DELAY(120);
    }
}

boolean disp_ili9163c_tft177_rot(disp_drv_ic_type *pdispic)
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
        pdispic->disp_ic_init       = disp_ic_init;
        pdispic->disp_ic_setwindow  = disp_ic_setwindow;
        pdispic->disp_ic_bitblt     = disp_ic_bitblt;
        pdispic->disp_ic_set        = disp_ic_set;
        pdispic->disp_ic_sleep      = disp_ic_sleep;
        return TRUE;
    }
    return FALSE;
}
