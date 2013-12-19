#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   128

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
   
   LCD_WRITE_CMD(0x3a);
   LCD_WRITE_DATA(0x05);
   
   LCD_WRITE_CMD(0x36);
   LCD_WRITE_DATA(0xc8); //0x08     180
   
   LCD_WRITE_CMD(0xfe);
   LCD_WRITE_CMD(0xef);

   LCD_WRITE_CMD(0xb4);
   LCD_WRITE_DATA(0x00);

   LCD_WRITE_CMD(0xff);
   LCD_WRITE_DATA(0x16);

   LCD_WRITE_CMD(0xfd); 	
   LCD_WRITE_DATA(0x20);  //0x19 Ë®²¨ÎÆ
   
   LCD_WRITE_CMD(0xa4); 	
   LCD_WRITE_DATA(0x96);
   
   LCD_WRITE_CMD(0xe7); 	
   LCD_WRITE_DATA(0x94);
   LCD_WRITE_DATA(0x88);   
   
   LCD_WRITE_CMD(0xed); 	
   LCD_WRITE_DATA(0x11);
   
   LCD_WRITE_CMD(0xe4); 	
   LCD_WRITE_DATA(0xc5);
   
   LCD_WRITE_CMD(0xe2);
   LCD_WRITE_DATA(0x80);//80
   
   LCD_WRITE_CMD(0xa3); 	
   LCD_WRITE_DATA(0x12);   
   
   LCD_WRITE_CMD(0xe3);
   LCD_WRITE_DATA(0x07);   //07
   
   LCD_WRITE_CMD(0xe5); 	
   LCD_WRITE_DATA(0x10);   
   
   //*****GAMMA******
   LCD_WRITE_CMD(0xF0);
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_CMD(0xF1);
   LCD_WRITE_DATA(0x55);
   LCD_WRITE_CMD(0xF2);
   LCD_WRITE_DATA(0x05);
   LCD_WRITE_CMD(0xF3);
   LCD_WRITE_DATA(0x53);
   LCD_WRITE_CMD(0xF4);
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_CMD(0xF5);
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_CMD(0xF7);
   LCD_WRITE_DATA(0x27);
   LCD_WRITE_CMD(0xF8);
   LCD_WRITE_DATA(0x22);
   LCD_WRITE_CMD(0xF9);
   LCD_WRITE_DATA(0x77);
   LCD_WRITE_CMD(0xFA);
   LCD_WRITE_DATA(0x35);
   LCD_WRITE_CMD(0xFB);
   LCD_WRITE_DATA(0x00);
   LCD_WRITE_CMD(0xFC);
   LCD_WRITE_DATA(0x00);
   //*****************

   
   LCD_WRITE_CMD(0x11);
   LCD_DELAY(200);   
   LCD_WRITE_CMD(0x29);
   LCD_WRITE_CMD(0x2C);//Memory Write
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
		disp_ic_init();
    }
}

static void disp_ic_rot(uint16 degree)
{
#if 0
    if(degree == 0)
    {
        LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
        LCD_WRITE_DATA(0x3C);
        LCD_WRITE_DATA(0x40);
    }
    else
    {
        LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
        LCD_WRITE_DATA(0x2C);
        LCD_WRITE_DATA(0x40);
    }
#endif
    return;
}

boolean disp_gc9102_tft177(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2,id3,id4;
    
    // Read ID 04
    /*LCD_WRITE_CMD(0xDA);
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDB);
    id2 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDC);
    id3 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
	*/
	LCD_WRITE_CMD(0x04);
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
	id3 = LCD_READ_DATA();
	id4 = LCD_READ_DATA();
    
    //if(id2==0x02)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
