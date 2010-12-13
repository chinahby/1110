#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   160

#include "disp_ic_generic.h"

static void disp_ic_init(void)
{
#ifdef FEATURE_OLD_LCD
    //--************ Start Initial Sequence **********--//
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
    LCD_WRITE_DATA(0xC8);  //..0x60 //..0x40 //..0x20 //..0x00 //..0x80 //0xa0 //..0xc0 //0xe0

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
    LCD_WRITE_CMD(0x11); //Sleep out
    LCD_DELAY(120); //Delay 120ms
    //------------------------------------ST7735R Frame Rate-----------------------------------------//
    LCD_WRITE_CMD(0xB1);
    LCD_WRITE_DATA(0x01);  //01
    LCD_WRITE_DATA(0x2C);
    LCD_WRITE_DATA(0x2D);
    LCD_WRITE_CMD(0xB2);
    LCD_WRITE_DATA(0x01);  //01
    LCD_WRITE_DATA(0x2C);
    LCD_WRITE_DATA(0x2D);
    LCD_WRITE_CMD(0xB3);
    LCD_WRITE_DATA(0x01);   //01
    LCD_WRITE_DATA(0x2C);
    LCD_WRITE_DATA(0x2D);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x2C);
    LCD_WRITE_DATA(0x2D);
    //------------------------------------End ST7735R Frame Rate-----------------------------------------//
    LCD_WRITE_CMD(0xB4); //Column inversion
    LCD_WRITE_DATA(0x03);  //0x07



    LCD_WRITE_CMD(0xB6);   //
    LCD_WRITE_DATA(0xB4); //
    LCD_WRITE_DATA(0xF0); //

    //------------------------------------ST7735R Power Sequence-----------------------------------------//
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
    //---------------------------------End ST7735R Power Sequence-------------------------------------//


    LCD_WRITE_CMD(0xC5); //VCOM
    LCD_WRITE_DATA(0x0A);   //0x0E

    LCD_WRITE_CMD(0x36); //MX, MY, RGB mode
    LCD_WRITE_DATA(0xC8);
    //------------------------------------ST7735R Gamma Sequence-----------------------------------------//
    LCD_WRITE_CMD(0xe0);
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x1c);
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x37);
    LCD_WRITE_DATA(0x32);
    LCD_WRITE_DATA(0x29);
    LCD_WRITE_DATA(0x2d);
    LCD_WRITE_DATA(0x29);
    LCD_WRITE_DATA(0x25);
    LCD_WRITE_DATA(0x2b);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x10);

    LCD_WRITE_CMD(0xe1);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x1d);
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x2e);
    LCD_WRITE_DATA(0x2c);
    LCD_WRITE_DATA(0x29);
    LCD_WRITE_DATA(0x2d);
    LCD_WRITE_DATA(0x2e);
    LCD_WRITE_DATA(0x2e);
    LCD_WRITE_DATA(0x37);
    LCD_WRITE_DATA(0x3f);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x10);
    //------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

    LCD_WRITE_CMD(0x2a);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x7f);

    LCD_WRITE_CMD(0x2b);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x9f);

    LCD_WRITE_CMD(0xF0); //Enable test command
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_CMD(0xF6); //Disable ram power save mode
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0x3A); //65k mode
    LCD_WRITE_DATA(0x05);

    LCD_WRITE_CMD(0x29); //Display on
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
        LCD_DELAY(120);
    }
    else
    {
        LCD_WRITE_CMD(0x11); //Exit Sleep
        LCD_DELAY(120);
    }
}

static void disp_ic_rot(uint16 degree)
{
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
    
    //if(id1 == 0x54 && id2 == 0x80 && id3 == 0x66)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
