#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   160

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ST7775R driver

#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x38//0x36 // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR 0x39//0x37 // Register to set col end
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x36//0x38 // Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR 0x37//0x39 // Register to set row end
#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x21//0x20 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x20//0x21 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write


#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ST7775R[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ST7775R[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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

    
    RETURN            //17
};

static uint8 disp_ic_mdp_getformat(void)
{
    return LCD_16BPP_88_II;
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_ST7775R[0][0];
    }
    return MDP_LCD_SCR_SIZE;
}

static void disp_ic_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[11] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
    scr[16] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[17] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    
    scr[19] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[20] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[22] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[23] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));

}
#endif

static void disp_ic_init(void)
{
	LCD_DELAY (1); //Delay 1ms 
	LCD_DELAY (1); //Delay 1ms 
	LCD_DELAY (10); 
	//----------------------------------End ST7775R Reset Sequence ------------------------------------// 
	//------------------------------------Display Control Setting----------------------------------------------// 
   #ifdef FEATURE_VERSION_K252_JT
	LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x031C); 
   #else
    LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x001c); 
   #endif
	LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1038); 
	LCD_WRITE_CMD(0x0008); LCD_WRITE_DATA16(0x0808); 
	LCD_WRITE_CMD(0x000C); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x000F); LCD_WRITE_DATA16(0x0001); 
	LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000); 
	//-----------------------------------End Display Control setting-----------------------------------------// 
	//--------------------------------Power Control Registers Initial --------------------------------------// 
	LCD_WRITE_CMD(0x0010); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16(0x1000); 
	//---------------------------------End Power Control Registers Initial -------------------------------// 
	LCD_DELAY (100); 
	//----------------------------------Display Windows 220 X 176----------------------------------------// 
	LCD_WRITE_CMD(0x0030); LCD_WRITE_DATA16(0x0000); 



	LCD_WRITE_CMD(0x0031); LCD_WRITE_DATA16(0x00DB); 
	LCD_WRITE_CMD(0x0032); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0033); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0034); LCD_WRITE_DATA16(0x00DB); 
	LCD_WRITE_CMD(0x0035); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0036); LCD_WRITE_DATA16(0x00AF); 
	LCD_WRITE_CMD(0x0037); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0038); LCD_WRITE_DATA16(0x00DB); 
	LCD_WRITE_CMD(0x0039); LCD_WRITE_DATA16(0x0000); 
	//----------------------------------End Display Windows 176 X 220----------------------------------// 
	LCD_DELAY(10); 
	LCD_WRITE_CMD(0x00ff); LCD_WRITE_DATA16(0x0003); 
	//-------------------------------------Gamma Cluster Setting-------------------------------------------// 
	LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0103); 
	LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0b08); 
	LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0108); 
	LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x1222); 
	LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x0502); 
	LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0103); 
	LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0604); 
	LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0107); 

	LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x1222); 
	LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x0502); 
	//---------------------------------------End Gamma Setting---------------------------------------------// 
	LCD_WRITE_CMD(0x00B0); LCD_WRITE_DATA16( 0x1901); 
	//---------------------------------------End Vcom Setting---------------------------------------------// 
	LCD_WRITE_CMD(0x00ff); LCD_WRITE_DATA16(0x0000); 
	LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017); 
	LCD_DELAY (200); //Delay 200ms 
    

}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
	LCD_WRITE_CMD(0x36); 
    LCD_WRITE_DATA16((uint16)end_row);
    LCD_WRITE_CMD(0x37);
    LCD_WRITE_DATA16((uint16)start_row);
	
    LCD_WRITE_CMD(0x38);
    LCD_WRITE_DATA16((uint16)end_col);
    LCD_WRITE_CMD(0x39);
    LCD_WRITE_DATA16((uint16)start_col);

    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
   

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
       // LCD_WRITE_CMD(0x10); //Sleep in
        LCD_WRITE_CMD(0x00ff); LCD_WRITE_DATA16(0x0000); 
	    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x0000); 
		LCD_DELAY(50);
	    LCD_WRITE_CMD(0x0010); LCD_WRITE_DATA16(0x0003); 
		LCD_DELAY(200);
    }
    else
    {
        //LCD_WRITE_CMD(0x11); //Exit Sleep
        //LCD_DELAY(120);
        LCD_DELAY(200);
		LCD_WRITE_CMD(0x00ff); LCD_WRITE_DATA16(0x0000); 
	    LCD_WRITE_CMD(0x0010); LCD_WRITE_DATA16(0x0000); 
		LCD_DELAY(50);
	    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017); 
		LCD_DELAY(200);
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_st7775r_tft20_rot(disp_drv_ic_type *pdispic)
{

	uint8 tid3,tid4;

    // Read ID
    LCD_WRITE_CMD(0x00);  //5C
    tid3 = LCD_READ_DATA();
    tid4 = LCD_READ_DATA();

    if((tid3 == 0x77) || (tid4 == 0x75))
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
