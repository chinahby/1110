#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

#define LCD_SET_REG(cmd,v)   LCD_WRITE_CMD(cmd); \
									 LCD_WRITE_DATA(v);

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA HX8340 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR_HIGH  0x06  // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR_LOW  0x07 // Register to set col start

#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR_HIGH  0x08// Register to set col end
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR_LOW   0x09// Register to set col end

#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR_HIGH  0x02// Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR_LOW  0x03 // Register to set row start

#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR_HIGH 0x04 // Register to set row end
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR_LOW  0x05 // Register to set row end

#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x20 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x21 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_HX8340[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_HX8340[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
#endif
{
  NOP,               //0
  NOP,               //1
  SET_LCD_CNTL_ADDR, //2 set LCD command port address
  LCD_CMD_WH,    //3

  SET_LCD_DATA_ADDR, //4 set LCD data port address
  LCD_DATA_WH,   //5

  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR_HIGH)),//6
  NULL,             //7  
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR_LOW)),//6
  NULL,             //9
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR_HIGH)),//9
  NULL,             //11
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR_LOW)),//9
  NULL,             //13
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR_HIGH)),//12
  NULL,             //15  
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR_LOW)),//12
  NULL,             //17
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR_HIGH)),//15
  NULL,             //19
  SEND_LCD_CNTL(DISP_LCD_18BPP(DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR_LOW)),//15
  NULL,             //21

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
        *ppscr = &MDP_DISP_SCR_HX8340[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[11] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
    scr[15] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[17] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[19] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[21] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));

}
#endif

static void disp_ic_init(void)
{ 
	//hanstar  2.0 x339
	//Driving ability Setting
//Driving ability Setting
	LCD_SET_REG(0x60,0x90); //PTBA[15:8]
	LCD_SET_REG(0x61,0x06); //PTBA[7:0]
	LCD_SET_REG(0x62,0x00); //STBA[15:8]
	LCD_SET_REG(0x63,0xD9); //STBA[7:0]
	LCD_SET_REG(0x73,0x70); //OPON[7:0],SET OPON=70h, default 38h
	//Gamma 2.2 Setting
	LCD_SET_REG(0x40,0x61);
	LCD_SET_REG(0x41,0x45);
	LCD_SET_REG(0x42,0x04);
	LCD_SET_REG(0x43,0x0E);
	LCD_SET_REG(0x44,0xDC);
	LCD_SET_REG(0x45,0x0C);
	LCD_SET_REG(0x46,0x05);
	LCD_SET_REG(0x47,0xE6);
	LCD_SET_REG(0x48,0x07);
	LCD_SET_REG(0x50,0x77);
	LCD_SET_REG(0x51,0x01);
	LCD_SET_REG(0x52,0x77);
	LCD_SET_REG(0x53,0x03);
	LCD_SET_REG(0x54,0x84);
	LCD_SET_REG(0x55,0x0F);
	LCD_SET_REG(0x56,0x07);
	LCD_SET_REG(0x57,0xA8);
	//Power Voltage Setting
	LCD_SET_REG(0x1F,0x03); //VRH=4.65V
	LCD_SET_REG(0x20,0x02); //02//BT (VGH~15V,VGL~-7V,DDVDH~5V)
	LCD_SET_REG(0x24,0x49); //38//VMH(VCOM High voltage)
	LCD_SET_REG(0x25,0x2F); //2F//VML(VCOM Low voltage)
	//****VCOM offset**///
	LCD_SET_REG(0x23,0x2c); //3c//reload from OTP// For Flicker adjust
	//Power on Setting
	LCD_SET_REG(0x18,0x00); //44//I/P_RADJ,N/P_RADJ, Normal mode 80Hz
	LCD_SET_REG(0x1B,0x44); //FS1 Pumping Clock= 2 x line frequency
	LCD_SET_REG(0x21,0x01); //OSC_EN='1', start Osc
	LCD_SET_REG(0x01,0x00); //SLP='0', out sleep
	LCD_SET_REG(0x1C,0x05); //AP=011
	LCD_SET_REG(0x19,0x06); // VOMG=1,PON=1, DK=0,
	LCD_DELAY(10);
	//262k/65k color selection
	LCD_SET_REG(0x17,0x05); //default 0x06 262k color // 0x05 65k color

	LCD_SET_REG(0x16,0xa8);
	//Display ON Setting
	LCD_SET_REG(0x26,0x84); //PT=10,GON=0, DTE=0, D=0100
	LCD_DELAY(40);
	LCD_SET_REG(0x26,0xB8); //PT=10,GON=1, DTE=1, D=1000
	LCD_DELAY(40);
	LCD_SET_REG(0x26,0xBC); //PT=10,GON=1, DTE=1, D=1100
	//Set GRAM Area
	LCD_SET_REG(0x02,0x00);
	LCD_SET_REG(0x03,0x00); //Column Start
	LCD_SET_REG(0x04,0x00);
	LCD_SET_REG(0x05,0xAF); //Column End
	LCD_SET_REG(0x06,0x00);
	LCD_SET_REG(0x07,0x00); //Row Start
	LCD_SET_REG(0x08,0x00);
	LCD_SET_REG(0x09,0xDB); //Row End
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x06);
    LCD_WRITE_DATA((uint16)0);

    LCD_WRITE_CMD(0x07);
    LCD_WRITE_DATA((uint16)start_row);


    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
    LCD_WRITE_CMD(0x08); 
    LCD_WRITE_DATA((uint16)0);
    LCD_WRITE_CMD(0x09);
    LCD_WRITE_DATA((uint16)end_row);

    LCD_WRITE_CMD(0x02);
    LCD_WRITE_DATA((uint16)0);
    LCD_WRITE_CMD(0x03);
    LCD_WRITE_DATA((uint16)start_col);

	LCD_WRITE_CMD(0x04);
    LCD_WRITE_DATA((uint16)0);
    LCD_WRITE_CMD(0x05);
    LCD_WRITE_DATA((uint16)end_col);
	
    LCD_WRITE_CMD(0x22);

}

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
		LCD_SET_REG(0x26, 0xB8);
		LCD_DELAY(50); //delay 50ms
		LCD_SET_REG(0x19, 0x01);
		LCD_DELAY(50); //delay 50ms
		LCD_SET_REG(0x26, 0xA4);
		LCD_DELAY(50); //delay 50ms
		LCD_SET_REG(0x26, 0x84);
		LCD_DELAY(50); //delay 50ms
		LCD_SET_REG(0x1C, 0x00);
		LCD_SET_REG(0x01, 0x02);
		LCD_SET_REG(0x21, 0x00);

    }
    else
    {
		LCD_SET_REG(0x18, 0x00);
		LCD_SET_REG(0x21, 0x01);
		LCD_SET_REG(0x01, 0x00);
		LCD_SET_REG(0x1c, 0x03);
		LCD_SET_REG(0x19, 0x06);
		LCD_DELAY(5); //delay 50ms[		
		LCD_SET_REG(0x26, 0x84);
		LCD_DELAY(40); //delay 50ms		
		LCD_SET_REG(0x26, 0xb8);
		LCD_DELAY(40); //delay 50ms		
		LCD_SET_REG(0x26, 0xbc);


    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_HX8340b_tft20_rot(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2;
    
    // Read ID
    //LCD_WRITE_CMD(0x93);
  ///  id1 = LCD_READ_DATA();
  //  id2 = LCD_READ_DATA();
    
    //if(id1 == 0x49 && id2 == 0x49)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
