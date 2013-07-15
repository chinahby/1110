#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   240

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ILI9342 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x50 // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR 0x51 // Register to set col end
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x52 // Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR 0x53 // Register to set row end
#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x20 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x21 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write
#define DISP_LCD_CMD_START                0x00

boolean is_rm68090;
uint8 id1;
uint8 id2;
uint8 id3;
uint8 id4;
uint8 id5;
uint8 id6;

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_RM68090[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_RM68090[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
#endif
{
      NOP,               //0
  	  NOP,               //1
	  SET_LCD_CNTL_ADDR, //2 set LCD command port address
	  LCD_CMD_WH,    //3

	  SET_LCD_DATA_ADDR, //4 set LCD data port address
	  LCD_DATA_WH,   //5
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR)),//6
	  NULL,             //7
	  NULL,             //8
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR)),//9
	  NULL,             //10
	  NULL,             //11
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR)),//12
	  NULL,             //13
	  NULL,             //14
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR)),//15
	  NULL,             //16
	  NULL,             //17
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_RAM_ADDR_SET_1_ADDR)),//18
	  NULL,             //19
	  NULL,             //20
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_RAM_ADDR_SET_2_ADDR)),//21
	  NULL,             //22
	  NULL,             //23
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_START)),
	  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_CMD_RAMWR)),//24

	  RETURN            //19
};

static uint8 disp_ic_mdp_getformat(void)
{
    return LCD_16BPP_88_II; //8 16BPP 0 //18BPP
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_RM68090[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
	#if 0
	scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[11] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[16] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[17] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
    scr[19] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[20] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[22] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[23] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
	#else
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[12] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[16] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[17] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[20] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[21] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
    scr[24] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[25] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[28] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[29] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
	#endif
}
#endif

static void disp_ic_init(void)
{
    //--************ Start Initial Sequence **********--/
    //LCD_WRITE_CMD(0x01);
    LCD_DELAY(150);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x01);
	LCD_WRITE_DATA16(0x0100); // set SS and NL bit
	
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x02);
	LCD_WRITE_DATA16(0x0700);

	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x03);
	LCD_WRITE_DATA16(0x1030);
	
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x04);
	LCD_WRITE_DATA16(0x0000);
	
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x08);
	LCD_WRITE_DATA16(0x0202);

	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x09);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x0A);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x0C);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x0D);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x0F);
	LCD_WRITE_DATA16(0x0000);
	LCD_DELAY(10);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x10);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x11);
	LCD_WRITE_DATA16(0x0001);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x12);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x13);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x07);
	LCD_WRITE_DATA16(0x0001);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x07);
	LCD_WRITE_DATA16(0x0020);
	LCD_DELAY(200);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x10);
	LCD_WRITE_DATA16(0x1290);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x11);
	LCD_WRITE_DATA16(0x0221);
	LCD_DELAY(50);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x12);
	LCD_WRITE_DATA16(0x0082);
	LCD_DELAY(10);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x13);
	LCD_WRITE_DATA16(0x1800);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x29);
	LCD_WRITE_DATA16(0x001c);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x2B);
	LCD_WRITE_DATA16(0x000D);
	LCD_DELAY(50);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x20);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x21);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x30);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x31);
	LCD_WRITE_DATA16(0x0606);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x32);
	LCD_WRITE_DATA16(0x0506);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x35);
	LCD_WRITE_DATA16(0x0007);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA16(0x1000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x37);
	LCD_WRITE_DATA16(0x0102);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x38);
	LCD_WRITE_DATA16(0x0101);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x39);
	LCD_WRITE_DATA16(0x0707);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x3C);
	LCD_WRITE_DATA16(0x0700);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x3D);
	LCD_WRITE_DATA16(0x0010);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x50);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x51);
	LCD_WRITE_DATA16(0x00EF);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x52);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x53);
	LCD_WRITE_DATA16(0x013F);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x60);
	LCD_WRITE_DATA16(0xA700);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x61);
	LCD_WRITE_DATA16(0x0001);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x6A);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x80);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x81);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x82);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x83);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x84);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x85);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x90);
	LCD_WRITE_DATA16(0x0010);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x92);
	LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x07);
	LCD_WRITE_DATA16(0x0133);
	LCD_DELAY(50);
	
	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x22);	//Write GRAM Start
	
	
	//LCD_WRITE_CMD(0x08);
	//id3 = LCD_READ_DATA();
	//id4 = LCD_READ_DATA();

	//LCD_WRITE_CMD(0x03);
	//id5 = LCD_READ_DATA();
	//id6 = LCD_READ_DATA();

}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{

	LCD_WRITE_CMD(0x00);
    LCD_WRITE_CMD(0x50);
    //LCD_WRITE_DATA16((uint16)start_col);
    LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
    LCD_WRITE_CMD(0x51);
    //LCD_WRITE_DATA16((uint16)end_col);
    LCD_WRITE_DATA16(0x00F0);
    
	LCD_WRITE_CMD(0x00);
    LCD_WRITE_CMD(0x52); 
    //LCD_WRITE_DATA16((uint16)start_row);
    LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
    LCD_WRITE_CMD(0x53);
    //LCD_WRITE_DATA16((uint16)end_row);
    LCD_WRITE_DATA16(0x0140);

	LCD_WRITE_CMD(0x00);
    LCD_WRITE_CMD(0x20);
    //LCD_WRITE_DATA16((uint16)start_col);
    LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x00);
    LCD_WRITE_CMD(0x21);
    //LCD_WRITE_DATA16((uint16)start_row);
    LCD_WRITE_DATA16(0x0000);

	LCD_WRITE_CMD(0x00);
	LCD_WRITE_CMD(0x22);
	
}


static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
    	//enter Sleep
    	LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0131); // Set D1=0, D0=1
		LCD_DELAY(10);
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0130); // Set D1=0, D0=0
		LCD_DELAY(10);
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0000); // display OFF
		//************* Power OFF sequence **************//
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x10);
		LCD_WRITE_DATA16(0x0080); // SAP, BT[3:0]); APE, AP, DSTB, SLP
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x11);
		LCD_WRITE_DATA16(0x0000); // DC1[2:0]); DC0[2:0]); VC[2:0]
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x12);
		LCD_WRITE_DATA16(0x0000); // VREG1OUT voltage
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x13);
		LCD_WRITE_DATA16(0x0000); // VDV[4:0] for VCOM amplitude
		LCD_DELAY(50); // Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x10);
		LCD_WRITE_DATA16(0x0082); // SAP, BT[3:0]); APE, AP, DSTB, SLP
    }
    else
    {
    	//Exit Sleep
        //*************Power On sequence ******************//
		/*
		LCD_WRITE_CMD(0x00);
              LCD_WRITE_CMD(0x10); // SAP, BT[3:0]); AP, DSTB, SLP
		LCD_WRITE_DATA16(0x0080);
		LCD_DELAY(50); // Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x10); // // DC1[2:0]); DC0[2:0]); VC[2:0]
		LCD_WRITE_DATA16(0x1290);
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x11); // // DC1[2:0]); DC0[2:0]); VC[2:0]
		LCD_WRITE_DATA16(0x0221);
		LCD_DELAY(80); // Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x12); // VREG1OUT voltage
		LCD_WRITE_DATA16(0x0082);
		LCD_DELAY(10); // Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x13); //// VDV[4:0] for VCOM amplitude
		LCD_WRITE_DATA16(0x1800);
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x29); //// VDV[4:0] for VCOM amplitude
		LCD_WRITE_DATA16(0x0018);
		LCD_DELAY(50); // Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x00);
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0133); // 262K color and display ON
		*/
		
		disp_ic_init();
		
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_rm68090_tft24_rot(disp_drv_ic_type *pdispic)
{
    // Read ID
    LCD_WRITE_CMD(0x00);
	id1 = 0;
	id2 = 0;
	id3 = 0;
	id4 = 0;
	id5 = 0;
	id6 = 0;
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    if(id1 == 0x68 && id2 == 0x09)
    {
    	is_rm68090 = TRUE;
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}

