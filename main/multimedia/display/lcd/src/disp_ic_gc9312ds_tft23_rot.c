#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   320

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA GC9312DS driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_GC9312DS[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_GC9312DS[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_GC9312DS[0][0];
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
    ///////////////CMO2.4//////////
    LCD_DELAY(120);
    #ifdef FEATURE_VERSION_K212_VHOPE
    LCD_WRITE_CMD(0x36);
	LCD_WRITE_DATA(0x48);	
	
    LCD_WRITE_CMD(0x3a);
	LCD_WRITE_DATA(0x05);	
	
	LCD_WRITE_CMD(0xfe);
	LCD_WRITE_CMD(0xef);

	LCD_WRITE_CMD(0xE2);
	LCD_WRITE_DATA(0x80);

	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x63);

	LCD_WRITE_CMD(0xE4);
	LCD_WRITE_DATA(0x10);

	LCD_WRITE_CMD(0xEB);
	LCD_WRITE_DATA(0x04);

	LCD_WRITE_CMD(0xE8);
	LCD_WRITE_DATA(0x09);


	LCD_WRITE_CMD(0xa3); //frame
	LCD_WRITE_DATA(0x0c);	   //0a

	LCD_WRITE_CMD(0xa4); 
	LCD_WRITE_DATA(0x96);	   
	
	LCD_WRITE_CMD(0xff);  //vdv[4:0]  
	LCD_WRITE_DATA(0x15);	   //15
		
	LCD_WRITE_CMD(0xfd);  //vcm[5:0]  flicker
	LCD_WRITE_DATA(0x00);	 

	//================gamma test=============//

	LCD_WRITE_CMD(0xf0);   //kp1[]  kp0
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xf1);	  //kp3 kp2
	LCD_WRITE_DATA(0x57); //52

	LCD_WRITE_CMD(0xf2);	 //kp5 kp4
	LCD_WRITE_DATA(0x04);  //07  05  14

	LCD_WRITE_CMD(0xf3);	  //rp1 rp0	 ok
	LCD_WRITE_DATA(0x52);    //52

	LCD_WRITE_CMD(0xf4);
	LCD_WRITE_DATA(0x00); 

	LCD_WRITE_CMD(0xf5);
	LCD_WRITE_DATA(0x00); 

	LCD_WRITE_CMD(0xf7);		 //kn1 kn0
	LCD_WRITE_DATA(0x37); //07  27 36 

	LCD_WRITE_CMD(0xf8);		//kn3 kn2
	LCD_WRITE_DATA(0x02); 

	LCD_WRITE_CMD(0xf9);		//kn5 kn4
	LCD_WRITE_DATA(0x77); 

	LCD_WRITE_CMD(0xfa);		//rn1 rn0  ok
	LCD_WRITE_DATA(0x25);   //25

	LCD_WRITE_CMD(0xfb);
	LCD_WRITE_DATA(0x00); 

	LCD_WRITE_CMD(0xfc);
	LCD_WRITE_DATA(0x00);
    #else
    LCD_WRITE_CMD(0xfe);
    LCD_WRITE_CMD(0xef);

    LCD_WRITE_CMD(0x3a);
    LCD_WRITE_DATA(0x05);

    LCD_WRITE_CMD(0x36);
    LCD_WRITE_DATA(0x48);

    LCD_WRITE_CMD(0xea);
    LCD_WRITE_DATA(0x1f);
    	
    LCD_WRITE_CMD(0xa3); //frame
    LCD_WRITE_DATA(0x08);	

    LCD_WRITE_CMD(0xb4); 
    LCD_WRITE_DATA(0x00);	  


    LCD_WRITE_CMD(0xfd);  //vcm[5:0]  flicker
    LCD_WRITE_DATA(0x17);	 

    		
    LCD_WRITE_CMD(0xff);  //vdv[4:0]  
    LCD_WRITE_DATA(0x19);	

    //================gamma test=============//
    LCD_WRITE_CMD(0xf0);   //kp1[]  kp0
    LCD_WRITE_DATA(0x20);

    LCD_WRITE_CMD(0xf1);	  //kp3 kp2
    LCD_WRITE_DATA(0x57); 

    LCD_WRITE_CMD(0xf2);	 //kp5 kp4
    LCD_WRITE_DATA(0x03);

    LCD_WRITE_CMD(0xf3);	  //rp1 rp0	 ok
    LCD_WRITE_DATA(0x04); 

    LCD_WRITE_CMD(0xf4);
    LCD_WRITE_DATA(0x00); 

    LCD_WRITE_CMD(0xf5);
    LCD_WRITE_DATA(0x00); 

    LCD_WRITE_CMD(0xf7);		 //kn1 kn0
    LCD_WRITE_DATA(0x47); 

    LCD_WRITE_CMD(0xf8);		//kn3 kn2
    LCD_WRITE_DATA(0x02); 

    LCD_WRITE_CMD(0xf9);		//kn5 kn4
    LCD_WRITE_DATA(0x75); 

    LCD_WRITE_CMD(0xfa);		//rn1 rn0  ok
    LCD_WRITE_DATA(0x37);

    LCD_WRITE_CMD(0xfb);
    LCD_WRITE_DATA(0x00); 

    LCD_WRITE_CMD(0xfc);
    LCD_WRITE_DATA(0x00);
    #endif
    LCD_WRITE_CMD(0x11);
    LCD_DELAY(120);
    LCD_WRITE_CMD(0x29);
    LCD_WRITE_CMD(0x2C);
    LCD_DELAY(50);
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x2A);
    //LCD_WRITE_DATA(0x0);
	LCD_WRITE_DATA16((uint16)start_col);
    //LCD_WRITE_DATA(0x0);
	LCD_WRITE_DATA16((uint16)end_col);
    
	/* Set LCD hardware to set start address */
	/* Transfer command to display hardware */
	LCD_WRITE_CMD(0x2B);
    //LCD_WRITE_DATA(0x0);        
	LCD_WRITE_DATA16((uint16)start_row);
    //LCD_WRITE_DATA(0x0);        
	LCD_WRITE_DATA16((uint16)end_row);
	
	LCD_WRITE_CMD(0x2C);
}


static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x28); //Sleep in
        LCD_WRITE_CMD(0x10);
        LCD_DELAY(120);
    }
    else
    {

    	LCD_WRITE_CMD(0x11); // Standby out
		LCD_DELAY(10);
		LCD_WRITE_CMD(0x11); //Exit Sleep
		LCD_DELAY(80);
		LCD_WRITE_CMD(0x29); // Display on
        LCD_DELAY(10);
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_gc9312ds_tft23_rot(disp_drv_ic_type *pdispic)
{ 
    uint8 id1,id2,id3;
    
    // Read ID
    LCD_WRITE_CMD(0xfe);
	LCD_WRITE_CMD(0xef);

	LCD_WRITE_CMD(0xE2);
	LCD_WRITE_DATA(0x80);
    LCD_WRITE_CMD(0x00);
    id1 = LCD_READ_DATA();    
    id2 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();    
    
    if(id3 == 0x93)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}

