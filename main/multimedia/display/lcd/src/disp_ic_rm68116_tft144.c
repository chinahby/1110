#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   128

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA rm68116 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_rm68116[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_rm68116[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_rm68116[0][0];
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
	#if 1
	LCD_WRITE_CMD(0x01);
	LCD_DELAY(200);
	LCD_WRITE_CMD(0x11);


	LCD_WRITE_CMD(0xC0);
	LCD_WRITE_DATA(0xD3);  
	LCD_WRITE_DATA(0x13); 
				
	LCD_WRITE_CMD(0xc1); 
	LCD_WRITE_DATA(0x09); 

	LCD_WRITE_CMD(0xc5); 
	LCD_WRITE_DATA(0x02); 

	LCD_WRITE_CMD(0xc7); 
	LCD_WRITE_DATA(0x10); 


	LCD_WRITE_CMD(0xB4);
	LCD_WRITE_DATA(0x03);  

	LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA(0xC8); 

	LCD_WRITE_CMD(0x3A);
	LCD_WRITE_DATA(0x05); 
			 
	LCD_WRITE_CMD(0xF8);
	LCD_WRITE_DATA(0x01);
									  
	LCD_WRITE_CMD(0xE0);//Gamma
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x29);
	LCD_WRITE_DATA(0x27);
	LCD_WRITE_DATA(0x1F);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x0E);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x0c);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x07);

	LCD_WRITE_CMD(0xE1);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x29);
	LCD_WRITE_DATA(0x27);
	LCD_WRITE_DATA(0x1F);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x0E);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x0c);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x07);
				 
	LCD_WRITE_CMD(0xFE);
	LCD_WRITE_DATA(0x09);
	LCD_WRITE_DATA(0xB0);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x48);
				 
	LCD_WRITE_CMD(0xB1);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x04);
				 
	LCD_WRITE_CMD(0xFD);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0xDF);
	LCD_WRITE_DATA(0x60);
	LCD_WRITE_DATA(0xD0);
				 
	LCD_WRITE_CMD(0xF4);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x0C);
				 
	LCD_WRITE_CMD(0xC2);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x80);  
				 
				 
	LCD_WRITE_CMD(0xF8);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0x29);
#else
	LCD_WRITE_CMD(0x01);
	LCD_DELAY(100);
	LCD_WRITE_CMD(0x11);
	LCD_DELAY(120);
	
	LCD_WRITE_CMD(0xC0);
	LCD_WRITE_DATA(0xD6);
	LCD_WRITE_DATA(0x16);
	
	LCD_WRITE_CMD(0xc5);
	LCD_WRITE_DATA(0x0B);
	
	LCD_WRITE_CMD(0xc7);
	LCD_WRITE_DATA(0x10);
	
	LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA(0xC8);
	
	LCD_WRITE_CMD(0x3A);
	LCD_WRITE_DATA(0x05);
	
	
	LCD_WRITE_CMD(0xc1);
	LCD_WRITE_DATA(0x09);
	LCD_WRITE_CMD(0xb4);
	LCD_WRITE_DATA(0x03);
	
	LCD_WRITE_CMD(0xf8);
	LCD_WRITE_DATA(0x01);
	
	LCD_WRITE_CMD(0xe0);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x29);
	LCD_WRITE_DATA(0x27);
	LCD_WRITE_DATA(0x1f);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x0e);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x0c);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x07);
	
	LCD_WRITE_CMD(0xe1);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x29);
	LCD_WRITE_DATA(0x27);
	LCD_WRITE_DATA(0x1f);
	LCD_WRITE_DATA(0x07);
	LCD_WRITE_DATA(0x0e);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x0c);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x07);
	
	LCD_WRITE_CMD(0xfe);
	LCD_WRITE_DATA(0x09);
	LCD_WRITE_DATA(0xb0);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0x48);
	
	LCD_WRITE_CMD(0xb1);
	LCD_WRITE_DATA(0x0f);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x04);
	
	LCD_WRITE_CMD(0xfd);
	LCD_WRITE_DATA(0x10);
	LCD_WRITE_DATA(0xdf);
	LCD_WRITE_DATA(0x60);
	LCD_WRITE_DATA(0xd0);
	
	LCD_WRITE_CMD(0xb1);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x04);
	
	LCD_WRITE_CMD(0xf4);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x0c);
	
	LCD_WRITE_CMD(0xc2);
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x84);
	
	LCD_WRITE_CMD(0xf8);
	LCD_WRITE_DATA(0x00);
	
	LCD_WRITE_CMD(0x29);
	
	LCD_WRITE_CMD(0x2C);
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
		LCD_WRITE_CMD(0x28);
		LCD_DELAY(120);
		LCD_WRITE_CMD(0x10); // Sleep in
		LCD_DELAY(120);
    }
    else
    {
		LCD_WRITE_CMD(0x11); //Exit Sleep
		LCD_DELAY(120);
	    LCD_WRITE_CMD(0x29);
		LCD_DELAY(120);
		LCD_WRITE_CMD(0x2c);
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_rm68116_tft144(disp_drv_ic_type *pdispic)
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
    
    if(id1 == 0x5C)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
