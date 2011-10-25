#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   240

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ILI9342 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ILI9342[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ILI9342[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_ILI9342[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[12] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[15] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
}
#endif

static void disp_ic_init(void)
{
    //--************ Start Initial Sequence **********--/
    //LCD_WRITE_CMD(0x01);
    LCD_DELAY(150);

	LCD_WRITE_CMD(0xEC); //Set Default Gamma
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x8F);

	LCD_WRITE_CMD(0x01); //set SS and SM bit
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x02); //set 1 line inversion
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x03); // set GRAM write direction and BGR=1.
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x30);

	LCD_WRITE_CMD(0x04); //Resize register
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x08); // set the back porch and front porch
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x02);

	LCD_WRITE_CMD(0x09); // set non-display area refresh cycle ISC[3:0]
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x0A); // FMARK function
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x0C); // RGB interface setting
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x0D); // Frame marker Position
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x0F); // RGB interface polarity
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	//*************Power On sequence ****************//
	LCD_WRITE_CMD(0x10); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x11); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x07);

	LCD_WRITE_CMD(0x12); // VREG1OUT voltage
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x13); // VDV[4:0] for VCOM amplitude
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x07); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_DELAY(200); // Dis-charge capacitor power voltage

	LCD_WRITE_CMD(0x10); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_WRITE_DATA(0x16);
	LCD_WRITE_DATA(0x90);

	LCD_WRITE_CMD(0x11); // Set DC1[2:0], DC0[2:0], VC[2:0]
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x27);
	LCD_DELAY(50);

	LCD_WRITE_CMD(0x12); // External reference voltage= Vci;
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x8A);
	LCD_DELAY(50);

	LCD_WRITE_CMD(0x13); // VDV[4:0] for VCOM amplitude
	LCD_WRITE_DATA(0x1A);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x29); // VCM[5:0] for VCOMH
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x0E);
	LCD_WRITE_CMD(0x2B); // Set Frame Rate
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x0B);
	LCD_DELAY(50);
	
	LCD_WRITE_CMD(0x20); // GRAM horizontal Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x21); // GRAM Vertical Address
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	// ----------- Adjust the Gamma Curve ----------//
	LCD_WRITE_CMD(0x30); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x31); 
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_CMD(0x32); 
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_CMD(0x35); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x36); 
	LCD_WRITE_DATA(0x0C);
	LCD_WRITE_DATA(0x09);
	LCD_WRITE_CMD(0x37); 
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_CMD(0x38); 
	LCD_WRITE_DATA(0x03);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_CMD(0x39); 
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_CMD(0x3C); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x3D); 
	LCD_WRITE_DATA(0x0A);
	LCD_WRITE_DATA(0x0A);
	//------------------ Set GRAM area ---------------//
	LCD_WRITE_CMD(0x50); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);// Horizontal GRAM Start Address
	LCD_WRITE_CMD(0x51); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0xEF);// Horizontal GRAM End Address
	LCD_WRITE_CMD(0x52); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);// Vertical GRAM Start Address
	LCD_WRITE_CMD(0x53); 
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x3F);// Vertical GRAM Start Address
	LCD_WRITE_CMD(0x60); 
	LCD_WRITE_DATA(0xA7);
	LCD_WRITE_DATA(0x00);// Gate Scan Line
	LCD_WRITE_CMD(0x61); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);// NDL,VLE, REV
	LCD_WRITE_CMD(0x6A); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);// set scrolling line
	//-------------- Partial Display Control ---------//
	LCD_WRITE_CMD(0x80); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x81); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x82); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x83); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x84); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x85); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	//-------------- Panel Control -------------------//
	LCD_WRITE_CMD(0x90); 
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_CMD(0x92); 
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_CMD(0x07); 
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x33);// 262K color and display ON
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x20);
    //LCD_WRITE_DATA(0x0);
	LCD_WRITE_DATA16((uint16)start_col);
    //LCD_WRITE_DATA(0x0);
	LCD_WRITE_DATA16((uint16)end_col);
    
	/* Set LCD hardware to set start address */
	/* Transfer command to display hardware */
	LCD_WRITE_CMD(0x21);
    //LCD_WRITE_DATA(0x0);        
	LCD_WRITE_DATA16((uint16)start_row);
    //LCD_WRITE_DATA(0x0);        
	LCD_WRITE_DATA16((uint16)end_row);
	
	LCD_WRITE_CMD(0x22);
}


static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
    	//enter Sleep
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0131); // Set D1=0, D0=1
		LCD_DELAY(10);
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0130); // Set D1=0, D0=0
		LCD_DELAY(10);
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0000); // display OFF
		//************* Power OFF sequence **************//
		LCD_WRITE_CMD(0x10);
		LCD_WRITE_DATA16(0x0080); // SAP, BT[3:0], APE, AP, DSTB, SLP
		LCD_WRITE_CMD(0x11);
		LCD_WRITE_DATA16(0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
		LCD_WRITE_CMD(0x12);
		LCD_WRITE_DATA16(0x0000); // VREG1OUT voltage
		LCD_WRITE_CMD(0x13);
		LCD_WRITE_DATA16(0x0000); // VDV[4:0] for VCOM amplitude
		LCD_DELAY(200); // Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x10);
		LCD_WRITE_DATA16(0x0082); // SAP, BT[3:0], APE, AP, DSTB, SLP
    }
    else
    {
    	//Exit Sleep
        //*************Power On sequence ******************//
        LCD_WRITE_CMD(0x10); // SAP, BT[3:0], AP, DSTB, SLP
		LCD_WRITE_DATA16(0x0080);
		LCD_WRITE_CMD(0x11); // // DC1[2:0], DC0[2:0], VC[2:0]
		LCD_WRITE_DATA16(0x0000);
		LCD_WRITE_CMD(0x12); // VREG1OUT voltage
		LCD_WRITE_DATA16(0x0000);
		LCD_WRITE_CMD(0x13); //// VDV[4:0] for VCOM amplitude
		LCD_WRITE_DATA16(0x0000);
		LCD_WRITE_CMD(0x07); //// VDV[4:0] for VCOM amplitude
		LCD_WRITE_DATA16(0x0001);
		LCD_DELAY(200);// Dis-charge capacitor power voltage
		LCD_WRITE_CMD(0x10);
		LCD_WRITE_DATA16(0x1490); // SAP, BT[3:0], AP, DSTB, SLP, STB
		LCD_WRITE_CMD(0x11);
		LCD_WRITE_DATA16(0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
		LCD_DELAY(50); // Delay 50ms
		LCD_WRITE_CMD(0x12);
		LCD_WRITE_DATA16(0x008F); // External reference voltage =Vci;
		LCD_DELAY(50); // Delay 50ms
		LCD_WRITE_CMD(0x13);
		LCD_WRITE_DATA16(0x1800); // VDV[4:0] for VCOM amplitude
		LCD_WRITE_CMD(0x29);
		LCD_WRITE_DATA16(0x0008); // VCM[5:0] for VCOMH
		LCD_DELAY(50); // Delay 50ms
		LCD_WRITE_CMD(0x07);
		LCD_WRITE_DATA16(0x0133); // 262K color and display ON
		
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_ili9325d_tft26_rot(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
    
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}

