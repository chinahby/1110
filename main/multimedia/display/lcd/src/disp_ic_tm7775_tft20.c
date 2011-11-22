#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"
#define LCD_WRITE_OUT(cmd, data)    LCD_WRITE_CMD((uint8)cmd); \
                                    LCD_WRITE_DATA16(data);

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA TM7775R driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_TM7775R[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_TM7775R[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_TM7775R[0][0];
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
    LCD_WRITE_CMD(0x01);// Software Reset
    LCD_DELAY(20);
    
    LCD_WRITE_CMD(0x11);//Sleep Out
    LCD_DELAY(120);
    
    //-------Display Control Setting-----------//
   LCD_WRITE_OUT(0x0001,0x011C);//0x001C
   
   LCD_WRITE_OUT(0x0002,0x0100);
   LCD_WRITE_OUT(0x0003,0x1030);
   LCD_WRITE_OUT(0x0008,0x0808);
   LCD_WRITE_OUT(0x000c,0x0000);
   LCD_WRITE_OUT(0x000F,0x0001);    
   LCD_WRITE_OUT(0x0020,0x0000);
   LCD_WRITE_OUT(0x0021,0x0000);
   //-----End Display Control setting------//

   
   //------------POWER CONTROL----------//
   //LCD_DELAY(10);
   LCD_WRITE_OUT(0x0010,0x0000);
   LCD_WRITE_OUT(0x0011,0x1000);
   LCD_DELAY(100);
   //------- End Power Control Registers Initial -----//
   
   //------Set GRAM area--------//
   LCD_WRITE_OUT(0x0030,0x0000);
   LCD_WRITE_OUT(0x0031,0x00db);
   LCD_WRITE_OUT(0x0032,0x0000);
   LCD_WRITE_OUT(0x0033,0x0000);
   LCD_WRITE_OUT(0x0034,0x00db);
   LCD_WRITE_OUT(0x0035,0x0000);
   LCD_WRITE_OUT(0x0036,0x00af);
   LCD_WRITE_OUT(0x0038,0x00DB);
   LCD_WRITE_OUT(0x0037,0x0000);
   LCD_WRITE_OUT(0x0039,0x0000);
   LCD_DELAY(10);

   LCD_WRITE_OUT(0x00ff,0x0003);
   //------------GAMMA CONTROL-----------//
   LCD_WRITE_OUT(0x0050,0x0103);
   LCD_WRITE_OUT(0x0051,0x0708);
   LCD_WRITE_OUT(0x0052,0x0007);
   LCD_WRITE_OUT(0x0053,0x0811);
   LCD_WRITE_OUT(0x0054,0x0803);
   LCD_WRITE_OUT(0x0055,0x0003);
   LCD_WRITE_OUT(0x0056,0x0101);
   LCD_WRITE_OUT(0x0057,0x0004);
   LCD_WRITE_OUT(0x0058,0x0811);
   LCD_WRITE_OUT(0x0059,0x0703);
   LCD_WRITE_OUT(0x00b0,0x0a01);
   
   LCD_WRITE_OUT(0x00ff,0x0000);
   LCD_WRITE_OUT(0x0007,0x1017);  
   LCD_DELAY(50);
   //---------Panel interface Control---------//
   LCD_WRITE_CMD(0x0022);

}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_OUT(0x0036, end_col);
	LCD_WRITE_OUT(0x0037, start_col);
	LCD_WRITE_OUT(0x0038, end_row);
	LCD_WRITE_OUT(0x0039, start_row);

	LCD_WRITE_OUT(0x0020, start_col);
	LCD_WRITE_OUT(0x0021, start_row);
	LCD_WRITE_CMD(0x0022);

}
#include "err.h"
static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
         	
       LCD_WRITE_OUT(0x0007,0x0000);
       LCD_DELAY(25); 	
       LCD_WRITE_OUT(0x0010,0x0003);
	   LCD_DELAY(100);
    }
    else 
    { 
        LCD_WRITE_OUT(0x0010,0x0000);
        LCD_DELAY(50);
        LCD_WRITE_OUT(0x0007,0x1017);
	    LCD_DELAY(200);
		
    }

}

static void disp_ic_rot(uint16 degree)
{
#if 0
    if(degree == 0)
    {
        LCD_WRITE_CMD(0x36); LCD_WRITE_DATA(0x48); // Set VCOMH/VCOML voltage   0x333A
    }
    else
    {
        LCD_WRITE_CMD(0x36); LCD_WRITE_DATA(0x28); // Set VCOMH/VCOML voltage   0x333A
    }
#endif
    return;
}



boolean disp_tm7775r_tft20(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
	uint16 id = 0;
    
    // Read ID
    LCD_WRITE_CMD(0x00);
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    
    if(id1==0x77&&id2==0x75)
    {
        
        DISP_IC_INIT_TBL(pdispic);
		
        return TRUE;
    }
    return FALSE;
}
