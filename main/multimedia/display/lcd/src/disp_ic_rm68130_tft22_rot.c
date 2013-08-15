#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"


//For lcd QVGA RM68130 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x38//0x36 // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR 0x39//0x37 // Register to set col end
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x36//0x38 // Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR 0x37//0x39 // Register to set row end
#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x21//0x20 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x20//0x21 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_RM68130[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_RM68130[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_RM68130[0][0];
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

//static unsigned VCOM_Offset=0x5a6c;
static void disp_ic_init(void)
{
  
#if 1
/* display option1,
LCD_WRITE_CMD(0x0028); LCD_WRITE_DATA16(0x00CE);	
//LCD_DELAY(150);  
LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x001c);//0x011c	

LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1038);//
LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x0017);
LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16(0x1000);
LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000);
LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000);
LCD_DELAY(50);	
LCD_WRITE_CMD(0x0036); LCD_WRITE_DATA16(0x00AF);
LCD_WRITE_CMD(0x0037); LCD_WRITE_DATA16(0x0000);
LCD_WRITE_CMD(0x0038); LCD_WRITE_DATA16(0x00DB);
LCD_WRITE_CMD(0x0039); LCD_WRITE_DATA16(0x0000);
LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0000);//
LCD_WRITE_CMD(0x00B0); LCD_WRITE_DATA16(0x1612);

LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0100);

	//------------GAMMA CONTROL-----------//	  
LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0000);	
LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0709);	
LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0d0b);  
LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x0106); 
LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x0000);	
LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0709);	
LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0d0b);
LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0106); 
LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x0000); 
LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x0000);   
LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0102);
LCD_WRITE_CMD(0x00FB); LCD_WRITE_DATA16(0x002A);
LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0101);
LCD_WRITE_CMD(0x00F1); LCD_WRITE_DATA16(0x0040);
LCD_DELAY(50);	
//LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017);
//LCD_DELAY(50);	

*/
//display option2;
    LCD_WRITE_CMD(0x0028); LCD_WRITE_DATA16(0x00CE);	
    LCD_DELAY(150);  
    LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x001c);//0x011c	
    LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0000);//DOT OR COLUMN INVERSION	
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1038);//
    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x0017);
    LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16(0x1000);
    LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000);
    LCD_DELAY(50);  
    LCD_WRITE_CMD(0x0036); LCD_WRITE_DATA16(0x00AF);
    LCD_WRITE_CMD(0x0037); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0038); LCD_WRITE_DATA16(0x00DB);
    LCD_WRITE_CMD(0x0039); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0100);
    LCD_WRITE_CMD(0x00B0); LCD_WRITE_DATA16(0x1812);
    LCD_WRITE_CMD(0x000B); LCD_WRITE_DATA16(0x0003);//FRAME RATE
    LCD_WRITE_CMD(0x00B1); LCD_WRITE_DATA16(0x0400);	
		//------------GAMMA CONTROL-----------//	  
    LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0003);    
    LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0802);    
    LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0A0A);  
    LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x0400); 
    LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x0003);    
    LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0802);    
    LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0A0A);
    LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0400); 
    LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x0000); 
    LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x0000);   
    LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0102);
    LCD_WRITE_CMD(0x00FB); LCD_WRITE_DATA16(0x002A);
    LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0101);
    LCD_WRITE_CMD(0x00F1); LCD_WRITE_DATA16(0x0040);
    LCD_DELAY(50);  
    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017);
    LCD_DELAY(50);  
	LCD_DELAY(150); // Delay 50ms
#else

	LCD_WRITE_CMD(0x0028); LCD_WRITE_DATA16(0x00CE);	
    LCD_DELAY(150);  
    LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16( 0x011c);//0x011c	
    LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0000);//DOT OR COLUMN INVERSION	
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16( 0x0230);//
    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16( 0x0017);
    LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16( 0x1000);
    LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000);
    LCD_DELAY(50);  
    LCD_WRITE_CMD(0x0036); LCD_WRITE_DATA16( 0x00AF);
    LCD_WRITE_CMD(0x0037); LCD_WRITE_DATA16(0x0000);
    LCD_WRITE_CMD(0x0038); LCD_WRITE_DATA16( 0x00DB);
    LCD_WRITE_CMD(0x0039); LCD_WRITE_DATA16( 0x0000);
    LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16( 0x0100);
    LCD_WRITE_CMD(0x00B0); LCD_WRITE_DATA16( 0x1812);
    LCD_WRITE_CMD(0x000B); LCD_WRITE_DATA16( 0x0003);//FRAME RATE
    LCD_WRITE_CMD(0x00B1); LCD_WRITE_DATA16( 0x0400);	
		//------------GAMMA CONTROL-----------//	  
    LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0003);    
    LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0802);    
    LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0A0A);  
    LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x0400); 
    LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x0003);    
    LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0802);    
    LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0A0A);
    LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0400); 
    LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x0000); 
    LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x0000);   
    LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0102);
    LCD_WRITE_CMD(0x00FB); LCD_WRITE_DATA16(0x002A);
    LCD_WRITE_CMD(0x00E8); LCD_WRITE_DATA16(0x0101);
    LCD_WRITE_CMD(0x00F1); LCD_WRITE_DATA16(0x0040);
    LCD_DELAY(50);  
    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017);
    LCD_DELAY(50);            
	LCD_DELAY(150); // Delay 50ms
#endif
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
/*
LCD_WRITE_CMD(0x36); 
LCD_WRITE_DATA16((uint16)end_col);
LCD_WRITE_CMD(0x37);
LCD_WRITE_DATA16((uint16)start_col);

LCD_WRITE_CMD(0x38);
LCD_WRITE_DATA16((uint16)end_row);
LCD_WRITE_CMD(0x39);
LCD_WRITE_DATA16((uint16)start_row);

/* Set LCD hardware to set start address */
/* Transfer command to display hardware */

/*
LCD_WRITE_CMD(0x20);
LCD_WRITE_DATA16((uint16)start_col);
LCD_WRITE_CMD(0x21);
LCD_WRITE_DATA16((uint16)start_row);

*/
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
//uint32 kk=0x4b5E; 

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
//        LCD_WRITE_CMD(0x07);
//        LCD_WRITE_DATA16(0x0000);
//        LCD_DELAY(50);
        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0002); // Enter Standby mode
    }
    else
    {
        disp_ic_init();
        //LCD_WRITE_CMD(0x10);
        //LCD_WRITE_DATA16(0x0000);
        LCD_DELAY(50);
 //       LCD_WRITE_CMD(0x07);
//        LCD_WRITE_DATA16(0x1017); //Exit Sleep
 //       LCD_DELAY(50);
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_rm68130_tft20_rot(disp_drv_ic_type *pdispic)
{
    uint8 tid1,tid2;
    
    // Read ID
    LCD_WRITE_CMD(0x00);
    tid1 = LCD_READ_DATA();
    tid2 = LCD_READ_DATA();
    
    if(tid1 == 0x68 && tid2 == 0x13)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
