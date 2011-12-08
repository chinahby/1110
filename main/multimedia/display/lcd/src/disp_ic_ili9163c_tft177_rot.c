#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   160

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA IlI9163C driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_IlI9163C[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_IlI9163C[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_IlI9163C[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP((start_col)>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP((end_col)>>8));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[12] = SEND_LCD_DATA(DISP_LCD_18BPP((start_row)>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP((end_row)>>8));
    scr[15] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
}
#endif

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
#ifdef	FEATURE_VERSION_C500BE
    LCD_WRITE_DATA(0x04);  //08 
    LCD_WRITE_DATA(0x0b);  //12
#else
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x12);
#endif     
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

#ifdef FEATURE_VERSION_W025    
    LCD_WRITE_CMD(0x36); //Set Scanning Direction
    LCD_WRITE_DATA(0xa0); // 0x60 For Old LCD 0x68 For New LCD
#else
	LCD_WRITE_CMD(0x36); //Set Scanning Direction
    LCD_WRITE_DATA(0x68); // 0x60 For Old LCD 0x68 For New LCD
#endif
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
    return;
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
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
