#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

static void disp_ic_init(void)
{
    //************* Start Initial Sequence **********//
    LCD_WRITE_CMD(0x0001);LCD_WRITE_DATA16(0x011C); // set SS and NL bit
    LCD_WRITE_CMD(0x0002);LCD_WRITE_DATA16(0x0100); // set 1 line inversion
    LCD_WRITE_CMD(0x0003);LCD_WRITE_DATA16(0x1030); // set GRAM write direction and BGR=1.
    LCD_WRITE_CMD(0x0008);LCD_WRITE_DATA16(0x0808); // set BP and FP
    LCD_WRITE_CMD(0x000C);LCD_WRITE_DATA16(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
    LCD_WRITE_CMD(0x000F);LCD_WRITE_DATA16(0x0C01); // Set frame rate		 //0801
    LCD_WRITE_CMD(0x0020);LCD_WRITE_DATA16(0x0000); // Set GRAM Address
    LCD_WRITE_CMD(0x0021);LCD_WRITE_DATA16(0x0000); // Set GRAM Address
    //*************Power On sequence ****************//
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x0010);LCD_WRITE_DATA16(0x0A00); // Set SAP,DSTB,STB
    LCD_WRITE_CMD(0x0011);LCD_WRITE_DATA16(0x1038); // Set APON,PON,AON,VCI1EN,VC
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x0012);LCD_WRITE_DATA16(0x1121); // Internal reference voltage= Vci;
    LCD_WRITE_CMD(0x0013);LCD_WRITE_DATA16(0x006E); // Set GVDD
    LCD_WRITE_CMD(0x0014);LCD_WRITE_DATA16(0x6163); // Set VCOMH/VCOML voltage  //6561  6563
    
    //------------- Set GRAM area ------------------//
    LCD_WRITE_CMD(0x0030);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0031);LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x0032);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0033);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0034);LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x0035);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0036);LCD_WRITE_DATA16(0x00AF);
    LCD_WRITE_CMD(0x0037);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0038);LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x0039);LCD_WRITE_DATA16(0x0000);
    // ----------- Adjust the Gamma Curve ----------//
    LCD_WRITE_CMD(0x0050);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0051);LCD_WRITE_DATA16(0x0705);
    LCD_WRITE_CMD(0x0052);LCD_WRITE_DATA16(0x0E0A);
    LCD_WRITE_CMD(0x0053);LCD_WRITE_DATA16(0x0300);
    LCD_WRITE_CMD(0x0054);LCD_WRITE_DATA16(0x0A0E);
    LCD_WRITE_CMD(0x0055);LCD_WRITE_DATA16(0x0507);
    LCD_WRITE_CMD(0x0056);LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0057);LCD_WRITE_DATA16(0x0003);
    LCD_WRITE_CMD(0x0058);LCD_WRITE_DATA16(0x090A);
    LCD_WRITE_CMD(0x0059);LCD_WRITE_DATA16(0x0A09);
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x0007);LCD_WRITE_DATA16(0x1017);
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
        LCD_WRITE_CMD(0x0011); // Set APON,PON,AON,VCI1EN,VC   
        LCD_WRITE_DATA16(0x0007);
        LCD_DELAY(50);
        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0A01); // // Enter Standby mode
    }
    else
    {
        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0A00);
        LCD_WRITE_CMD(0x11);
        LCD_WRITE_DATA16(0x1038);
        LCD_DELAY(50);
        LCD_WRITE_CMD(0x07);
        LCD_WRITE_DATA16(0x1017); //Exit Sleep   
    }
}

static void disp_ic_rot(uint16 degree)
{
#if 0
    if(degree == 0)
    {
        LCD_WRITE_CMD(0x14); LCD_WRITE_DATA16(0x5560); // Set VCOMH/VCOML voltage   0x333A
    }
    else
    {
        LCD_WRITE_CMD(0x14); LCD_WRITE_DATA16(0x333A); // Set VCOMH/VCOML voltage   0x3336
    }
#endif
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
