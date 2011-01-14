#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   128

#include "disp_ic_generic.h"

static void disp_ic_init(void)
{
    //--************ Start Initial Sequence **********--//
            
    LCD_WRITE_CMD(0x11); //Exit Sleep
    LCD_DELAY(120);
    
    LCD_WRITE_CMD(0x26); //Set Default Gamma
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_CMD(0xF2); //E0h & E1h Enable/Disable
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_CMD(0xB1);//Set Frame Rate  72HZ
    LCD_WRITE_DATA(0x0C);//DIVA = 0x0c  12   0B: 74HZ 0A:79HZ   09:85.4HZ    08:92.5HZ
    LCD_WRITE_DATA(0x14);//VPA  = 0x14  20  
    LCD_WRITE_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_WRITE_DATA(0x0C);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_CMD(0xEC); 
    LCD_WRITE_DATA(0x0C);
    LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_WRITE_DATA(0x44);
    LCD_WRITE_DATA(0x52);
    LCD_WRITE_CMD(0xC7); // VCOM offset ;reduce flicker
    LCD_WRITE_DATA(0xB9); // Debug B7-B10
    //************* Scan direction **********//
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
    LCD_WRITE_CMD(0x36); //Set Scanning Direction & BGR
    LCD_WRITE_DATA(0xC8);
    LCD_WRITE_CMD(0x3a); //Set Pixel
    LCD_WRITE_DATA(0x55);
    
    //************* Gamma Setting **********//
    LCD_WRITE_CMD(0xf2); //Enable Gamma bit
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x3F);//p1
    LCD_WRITE_DATA(0x1B);//p2
    LCD_WRITE_DATA(0x19);//p3
    LCD_WRITE_DATA(0x27);//p4
    LCD_WRITE_DATA(0x1F);//p5
    LCD_WRITE_DATA(0x0D);//p6
    LCD_WRITE_DATA(0x45);//p7
    LCD_WRITE_DATA(0xB8);//p8
    LCD_WRITE_DATA(0x30);//p9
    LCD_WRITE_DATA(0x17);//p10
    LCD_WRITE_DATA(0x0F);//p11
    LCD_WRITE_DATA(0x05);//p12
    LCD_WRITE_DATA(0x13);//p13
    LCD_WRITE_DATA(0x02);//p14
    LCD_WRITE_DATA(0x00);//p15
    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00);//p1
    LCD_WRITE_DATA(0x24);//p2
    LCD_WRITE_DATA(0x26);//p3
    LCD_WRITE_DATA(0x08);//p4
    LCD_WRITE_DATA(0x10);//p5
    LCD_WRITE_DATA(0x12);//p6
    LCD_WRITE_DATA(0x3A);//p7
    LCD_WRITE_DATA(0x74);//p8
    LCD_WRITE_DATA(0x4F);//p9
    LCD_WRITE_DATA(0x08);//p10
    LCD_WRITE_DATA(0x20);//p11
    LCD_WRITE_DATA(0x2A);//p12
    LCD_WRITE_DATA(0x3C);//p13
    LCD_WRITE_DATA(0x3D);//p14
    LCD_WRITE_DATA(0x3F);//p15
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
    
    if(id1 == 0x54 && id2 == 0x80 && id3 == 0x66)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
