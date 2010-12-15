#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

static void disp_ic_init(void)
{
    LCD_WRITE_CMD(0x01); LCD_WRITE_DATA16(0x011C); // set SS and NL bit
    LCD_WRITE_CMD(0x02); LCD_WRITE_DATA16(0x0100); // set 1 line inversion
    LCD_WRITE_CMD(0x03); LCD_WRITE_DATA16(0x1030); // set GRAM write direction and BGR=1.
    LCD_WRITE_CMD(0x08); LCD_WRITE_DATA16(0x0808); // set BP and FP
    LCD_WRITE_CMD(0x0C); LCD_WRITE_DATA16(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111 for RGB16Bit
    LCD_WRITE_CMD(0x0F); LCD_WRITE_DATA16(0x0E01); // Set frame rate
    LCD_WRITE_CMD(0x20); LCD_WRITE_DATA16(0x0000); // Set GRAM Address
    LCD_WRITE_CMD(0x21); LCD_WRITE_DATA16(0x0000); // Set GRAM Address
    //-*************Power On sequence ****************-//
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x10); LCD_WRITE_DATA16(0x0A00); // Set SAP,DSTB,STB
    LCD_WRITE_CMD(0x11); LCD_WRITE_DATA16(0x1038); // Set APON,PON,AON,VCI1EN,VC
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x12); LCD_WRITE_DATA16(0x1121); // Internal reference voltage= Vci;
    LCD_WRITE_CMD(0x13); LCD_WRITE_DATA16(0x006E); // Set GVDD
    LCD_WRITE_CMD(0x14); LCD_WRITE_DATA16(0x5560); // Set VCOMH/VCOML voltage   0x3336
    //------------------------ Set GRAM area --------------------------------//
    LCD_WRITE_CMD(0x30); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x31); LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x32); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x33); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x34); LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x35); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x36); LCD_WRITE_DATA16(0x00AF);
    LCD_WRITE_CMD(0x37); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x38); LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x39); LCD_WRITE_DATA16(0x0000);
    // ----------- Adjust the Gamma Curve ----------//
    LCD_WRITE_CMD(0x50); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x51); LCD_WRITE_DATA16(0x0705);
    LCD_WRITE_CMD(0x52); LCD_WRITE_DATA16(0x0E0A);
    LCD_WRITE_CMD(0x53); LCD_WRITE_DATA16(0x0300);
    LCD_WRITE_CMD(0x54); LCD_WRITE_DATA16(0x0A0E);
    LCD_WRITE_CMD(0x55); LCD_WRITE_DATA16(0x0507);
    LCD_WRITE_CMD(0x56); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x57); LCD_WRITE_DATA16(0x0003);
    LCD_WRITE_CMD(0x58); LCD_WRITE_DATA16(0x090A);
    LCD_WRITE_CMD(0x59); LCD_WRITE_DATA16(0x0A09);
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x07); LCD_WRITE_DATA16(0x1017);
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x38);
    LCD_WRITE_DATA16((uint16)end_row);
    LCD_WRITE_CMD(0x39);
    LCD_WRITE_DATA16((uint16)start_row);

    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
    LCD_WRITE_CMD(0x36); 
    LCD_WRITE_DATA16((uint16)end_col);
    LCD_WRITE_CMD(0x37);
    LCD_WRITE_DATA16((uint16)start_col);

    LCD_WRITE_CMD(0x20);
    LCD_WRITE_DATA16((uint16)start_col);
    LCD_WRITE_CMD(0x21);
    LCD_WRITE_DATA16((uint16)start_row);
    
    LCD_WRITE_CMD(0x22);
}

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x07);
        LCD_WRITE_DATA16(0x0000);
        LCD_DELAY(50);

        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0A01); // // Enter Standby mode
        LCD_DELAY(10);
    }
    else
    {
        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0xA000);
        LCD_DELAY(50);

        LCD_WRITE_CMD(0x07);
        LCD_WRITE_DATA16(0x1017); //Exit Sleep   
        LCD_DELAY(10);
    }
}

static void disp_ic_rot(uint16 degree)
{
    if(degree == 0)
    {
        LCD_WRITE_CMD(0x14); LCD_WRITE_DATA16(0x5560); // Set VCOMH/VCOML voltage   0x333A
    }
    else
    {
        LCD_WRITE_CMD(0x14); LCD_WRITE_DATA16(0x333A); // Set VCOMH/VCOML voltage   0x3336
    }
    return;
}

boolean disp_ili9225b_tft20(disp_drv_ic_type *pdispic)
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
