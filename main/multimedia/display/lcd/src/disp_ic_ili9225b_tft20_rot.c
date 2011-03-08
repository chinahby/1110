#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ILI9225 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x36 // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR 0x37 // Register to set col end
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x38 // Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR 0x39 // Register to set row end
#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x20 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x21 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ILI9225[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ILI9225[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
#endif
{
  NOP,               //0
  NOP,               //1
  SET_LCD_CNTL_ADDR, //2 set LCD command port address
  LCD_CMD_WH,    //3

  SET_LCD_DATA_ADDR, //4 set LCD data port address
  LCD_DATA_WH,   //5

  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR)),//6
  NULL,             //7
  NULL,             //8
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR)),//9
  NULL,             //10
  NULL,             //11
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR)),//12
  NULL,             //13
  NULL,             //14
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR)),//15
  NULL,             //16
  NULL,             //17
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_RAM_ADDR_SET_1_ADDR)),//18
  NULL,             //19
  NULL,             //20
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_RAM_ADDR_SET_2_ADDR)),//21
  NULL,             //22
  NULL,             //23
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_RAMWR)),//24

  RETURN            //25
};

static uint8 disp_ic_mdp_getformat(void)
{
    return LCD_16BPP_88_II; //8 16BPP 0 //18BPP
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_ILI9225[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[11] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[16] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[17] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    
    scr[19] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[20] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[22] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[23] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
}
#endif

static void disp_ic_init(void)
{
    LCD_WRITE_CMD(0x28); LCD_WRITE_DATA16(0x00CE);
    LCD_DELAY(150); // Delay 150ms
    LCD_WRITE_CMD(0x01); LCD_WRITE_DATA16(0x031C); // set SS and NL bit
    LCD_WRITE_CMD(0x02); LCD_WRITE_DATA16(0x0100); // set 1 line inversion
    LCD_WRITE_CMD(0x03); LCD_WRITE_DATA16(0x1038); // set GRAM write direction and BGR=1.
    LCD_WRITE_CMD(0x08); LCD_WRITE_DATA16(0x0808); // set BP and FP
    LCD_WRITE_CMD(0x0C); LCD_WRITE_DATA16(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111 for RGB16Bit
    LCD_WRITE_CMD(0x0F); LCD_WRITE_DATA16(0x0B01); // Set frame rate
    LCD_WRITE_CMD(0x20); LCD_WRITE_DATA16(0x0000); // Set GRAM Address
    LCD_WRITE_CMD(0x21); LCD_WRITE_DATA16(0x0000); // Set GRAM Address
    //-*************Power On sequence ****************-//
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x10); LCD_WRITE_DATA16(0x0800); // Set SAP,DSTB,STB
    LCD_WRITE_CMD(0x11); LCD_WRITE_DATA16(0x1038); // Set APON,PON,AON,VCI1EN,VC
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x12); LCD_WRITE_DATA16(0x1121); // Internal reference voltage= Vci;
    LCD_WRITE_CMD(0x13); LCD_WRITE_DATA16(0x0066); // Set GVDD
    LCD_WRITE_CMD(0x14); LCD_WRITE_DATA16(0x333A); // Set VCOMH/VCOML voltage   0x3336
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
    LCD_WRITE_CMD(0x51); LCD_WRITE_DATA16(0x0102);
    LCD_WRITE_CMD(0x52); LCD_WRITE_DATA16(0x0901);
    LCD_WRITE_CMD(0x53); LCD_WRITE_DATA16(0x0700);
    LCD_WRITE_CMD(0x54); LCD_WRITE_DATA16(0x0109);
    LCD_WRITE_CMD(0x55); LCD_WRITE_DATA16(0x0201);
    LCD_WRITE_CMD(0x56); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x57); LCD_WRITE_DATA16(0x0007);
    LCD_WRITE_CMD(0x58); LCD_WRITE_DATA16(0x1F00);
    LCD_WRITE_CMD(0x59); LCD_WRITE_DATA16(0x001F);
    LCD_DELAY(50); // Delay 50ms
    LCD_WRITE_CMD(0x07); LCD_WRITE_DATA16(0x1017);
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x36);
    LCD_WRITE_DATA16((uint16)end_row);
    LCD_WRITE_CMD(0x37);
    LCD_WRITE_DATA16((uint16)start_row);

    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
    LCD_WRITE_CMD(0x38); 
    LCD_WRITE_DATA16((uint16)end_col);
    LCD_WRITE_CMD(0x39);
    LCD_WRITE_DATA16((uint16)start_col);

    LCD_WRITE_CMD(0x20);
    LCD_WRITE_DATA16((uint16)start_row);
    LCD_WRITE_CMD(0x21);
    LCD_WRITE_DATA16((uint16)start_col);
    
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
        LCD_WRITE_DATA16(0x0A01); // Enter Standby mode
    }
    else
    {
        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0A00);
        LCD_DELAY(50);

        LCD_WRITE_CMD(0x07);
        LCD_WRITE_DATA16(0x1017); //Exit Sleep
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_ili9225b_tft20_rot(disp_drv_ic_type *pdispic)
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
