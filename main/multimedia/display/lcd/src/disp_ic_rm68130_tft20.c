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
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x38 // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR 0x39 // Register to set col end
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x36 // Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR 0x37 // Register to set row end
#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x21 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x20 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write


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
        *ppscr = &MDP_DISP_SCR_TM7775R[0][0];
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

#if 0
    scr[7] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col>>8));
    scr[8] = SEND_LCD_DATA(DISP_LCD_18BPP(start_col));
    scr[9] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col>>8));
    scr[10] = SEND_LCD_DATA(DISP_LCD_18BPP(end_col));
    scr[12] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row>>8));
    scr[13] = SEND_LCD_DATA(DISP_LCD_18BPP(start_row));
    scr[14] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row>>8));
    scr[15] = SEND_LCD_DATA(DISP_LCD_18BPP(end_row));
#endif
}
#endif
									
static void disp_ic_init(void)
{
	//sanlong_RM68130_TM__start
	//请读0x00的寄存器，读两次第一次为高8位0x68,第二次为低8位0x13;可以合成0x6813
	LCD_WRITE_OUT(0x0028, 0x00CE); 
	LCD_WRITE_OUT(0x01, 0x011C); 
	LCD_WRITE_OUT(0x03, 0x1030);  //1018 
	LCD_WRITE_OUT(0x07, 0x0017);
	LCD_DELAY(50); 

	LCD_WRITE_OUT(0x11, 0x1000);//0808 
	LCD_WRITE_OUT(0x20, 0x0000);       
	LCD_WRITE_OUT(0x21, 0x0000); 
	LCD_DELAY(50);
	LCD_WRITE_OUT(0x30, 0x0000);//在主函数已调用祥见void set_windows_x_y()   
	LCD_WRITE_OUT(0x31, 0x00DB); //DB  
	LCD_WRITE_OUT(0x32, 0x0000);   
	LCD_WRITE_OUT(0x33, 0x0000);   
	LCD_WRITE_OUT(0x34, 0x00DB);  //DB 
	LCD_WRITE_OUT(0x35, 0x0000);
	   
	LCD_WRITE_OUT(0x36, 0x00AF);   
	LCD_WRITE_OUT(0x37, 0x0000);   
	LCD_WRITE_OUT(0x38, 0x00DB);  //DB 
	LCD_WRITE_OUT(0x39, 0x0000);     
	LCD_DELAY(50);
	LCD_WRITE_OUT(0x02, 0x0000); // Gamma Command
	LCD_DELAY(50);
//	LCD_WRITE_OUT(0x0d, 0x2100); // Gamma Command

	LCD_WRITE_OUT(0x61, 0x0103);
	LCD_WRITE_OUT(0xE8, 0x1000);
	LCD_WRITE_OUT(0xB0, 0x0812);
	LCD_WRITE_OUT(0x0B, 0x0000);
	LCD_WRITE_OUT(0x0D, 0x0000);
	LCD_WRITE_OUT(0xB1, 0x0404);
	LCD_WRITE_OUT(0x62, 0x0019);
	LCD_DELAY(50);
	LCD_WRITE_OUT(0xE8, 0x0100);

	LCD_WRITE_OUT(0x50, 0x0007);
	LCD_WRITE_OUT(0x51, 0x0708);
	LCD_WRITE_OUT(0x52, 0x0D0A);
	LCD_WRITE_OUT(0x53, 0x0404);
	LCD_WRITE_OUT(0x54, 0x0007);
	LCD_WRITE_OUT(0x55, 0x0706); 
	LCD_WRITE_OUT(0x56, 0x0D0A);
	LCD_WRITE_OUT(0x57, 0x0404);
	LCD_WRITE_OUT(0x58, 0x0000); 
	LCD_WRITE_OUT(0x59, 0x0000);

	LCD_WRITE_OUT(0x20, 0x0000);       
	LCD_WRITE_OUT(0x21, 0x0000);   
	LCD_WRITE_OUT(0x07, 0x1017);      
	LCD_DELAY(80);
	LCD_WRITE_CMD(0x22);
	//sanlong_RM68130_TM__end
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_OUT(0x36, end_col);
	LCD_WRITE_OUT(0x37, start_col);
	LCD_WRITE_OUT(0x38, end_row);
	LCD_WRITE_OUT(0x39, start_row);

	LCD_WRITE_OUT(0x20, start_col);
	LCD_WRITE_OUT(0x21, start_row);
	LCD_WRITE_CMD(0x22);

}

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {         	
       LCD_WRITE_OUT(0x0007,0x0000);
       LCD_DELAY(50); 	
      // LCD_WRITE_OUT(0x10,0x0003);
	  // LCD_DELAY(50);
    }
    else 
    { 
        //LCD_WRITE_OUT(0x0010,0x0000);
        //LCD_DELAY(50);
        LCD_WRITE_OUT(0x07,0x1017);
	    LCD_DELAY(50);
		
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


boolean disp_rm68130_tft20(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3;
	uint16 id = 0;
    
    // Read ID
    LCD_WRITE_CMD(0x00);
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    
    if(id1==0x68&&id2==0x13)
    {
        DISP_IC_INIT_TBL(pdispic);
		
        return TRUE;
    }
    return FALSE;
}