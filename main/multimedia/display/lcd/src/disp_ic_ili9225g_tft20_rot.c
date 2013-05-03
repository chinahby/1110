#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   220

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ILI9225 driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x36 // Register to set col start
#define DISP_LCD_HORZ_RAM_ADDR_POS_2_ADDR 0x37 // Register to set col end
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x38 // Register to set row start
#define DISP_LCD_VERT_RAM_ADDR_POS_2_ADDR 0x39 // Register to set row end
#define DISP_LCD_RAM_ADDR_SET_1_ADDR      0x20 // Start address - col
#define DISP_LCD_RAM_ADDR_SET_2_ADDR      0x21 // Start address - row
#define DISP_LCD_CMD_RAMWR                0x22 // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ILI9225[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ILI9225[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
        *ppscr = &MDP_DISP_SCR_ILI9225[0][0];
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
    #if 0
    LCD_WRITE_CMD(0x00D0); LCD_WRITE_DATA16(0x0003);
    LCD_WRITE_CMD(0x00EB); LCD_WRITE_DATA16(0x0B00);
    LCD_WRITE_CMD(0x00EC); LCD_WRITE_DATA16(0x004F);// 0x0f”–colltalk
    LCD_WRITE_CMD(0x00c7); LCD_WRITE_DATA16(0x030f);
    LCD_DELAY(50);  // Delay 50 ms
    LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x011C); 
    LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0100); 

#if defined(Rotate90)
	LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1018); 
#elif defined (Rotate270)
	LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1028);   
#else
	LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1030); 
#endif
    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x0000); 
    LCD_WRITE_CMD(0x0008); LCD_WRITE_DATA16(0x0808);
    LCD_WRITE_CMD(0x000F); LCD_WRITE_DATA16(0x0a01);   

    LCD_DELAY(50);         
    LCD_WRITE_CMD(0x0010); LCD_WRITE_DATA16(0x0000); 
    LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16(0x1b41); //The register setting is suitable for VCI=2.8V
    LCD_DELAY(50);
    LCD_WRITE_CMD(0x0012); LCD_WRITE_DATA16(0x200e);//The register setting is suitable for VCI=2.8V 
    LCD_WRITE_CMD(0x0013); LCD_WRITE_DATA16(0x0054); //0x60 //The register setting is suitable for VCI=2.8V 
    LCD_WRITE_CMD(0x0014); LCD_WRITE_DATA16(0x606c); //0x646c //The register setting is suitable for VCI=2.8V 
    LCD_DELAY(50);
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

    LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0000);  
    LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0300); 
    LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0a00); 
    LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x0801); //0701
    LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x000a); 
    LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0003); 
    LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0000); 
    LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0108); //0107
    LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x0f00); //
    LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x000f); 

    LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000);       
    LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000);   
    LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017); 

  #elif defined(FEATURE_VERSION_K202)// XXZHEN
    LCD_WRITE_CMD(0x00d0); LCD_WRITE_DATA16(0x0003); // set SS and NL bit
	LCD_WRITE_CMD(0x00eb); LCD_WRITE_DATA16(0x0b00); // set 1 line inversion
	LCD_WRITE_CMD(0x00ec); LCD_WRITE_DATA16(0x000f); //1030 set GRAM write direction and BGR=1.
	LCD_WRITE_CMD(0x00c7); LCD_WRITE_DATA16(0x030f);
	//LCD_SET_REG(0x00c7, 0x030f); // set BP and FP

	LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x011C); // set SS and NL bit
	LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0100); // set 1 line inversion   0100
#if defined(Rotate90)
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1018); 
#elif defined (Rotate270)
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1028);   
#else
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1030); 
#endif
	LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0008); LCD_WRITE_DATA16(0x0808); // set BP and FP
	//LCD_WRITE_CMD(0x000C); LCD_WRITE_DATA16(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 
	LCD_WRITE_CMD(0x000F); LCD_WRITE_DATA16(0x0901); // Set frame rate  0B01
	//*************Power On sequence ****************
	LCD_DELAY(50); // Delay 50ms
	LCD_WRITE_CMD(0x0010); LCD_WRITE_DATA16(0x0000); // Set SAP,DSTB,STB
	LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16(0x1B41); // Set APON,PON,AON,VCI1EN,VC
	LCD_DELAY(50); // Delay 50ms
	LCD_WRITE_CMD(0x0012); LCD_WRITE_DATA16(0x200E); // Internal reference voltage= Vci;
	LCD_WRITE_CMD(0x0013); LCD_WRITE_DATA16(0x0052); //5D Set GVDD   //0x0064
	LCD_WRITE_CMD(0x0014); LCD_WRITE_DATA16(0x525f); //0x6561 // Set VCOMH/VCOML voltage   //4d5f  0x4B50  0x5150
	//Vcom=Vcom+0x100;
	//------------- Set GRAM area ------------------//
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
	// ----------- Adjust the Gamma Curve ----------//

	LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0509);
	LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0b07);
	LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x0400);
	LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x070b);
	LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0905);
	LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0004);
	LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x0e00);
	LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x000e);

	LCD_DELAY(50); // Delay 50ms
	LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017);//Display On
	LCD_DELAY(150); // Delay 50ms
	
    #else
    
    LCD_WRITE_CMD(0x00d0); LCD_WRITE_DATA16(0x0003); // set SS and NL bit
	LCD_WRITE_CMD(0x00eb); LCD_WRITE_DATA16(0x0b00); // set 1 line inversion
	LCD_WRITE_CMD(0x00ec); LCD_WRITE_DATA16(0x004f); //1030 set GRAM write direction and BGR=1.
	//LCD_SET_REG(0x00c7, 0x030f); // set BP and FP

	LCD_WRITE_CMD(0x0001); LCD_WRITE_DATA16(0x011C); // set SS and NL bit
	LCD_WRITE_CMD(0x0002); LCD_WRITE_DATA16(0x0100); // set 1 line inversion   0100
#if defined(Rotate90)
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1018); 
#elif defined (Rotate270)
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1028);   
#else
    LCD_WRITE_CMD(0x0003); LCD_WRITE_DATA16(0x1030); 
#endif

	LCD_WRITE_CMD(0x0008); LCD_WRITE_DATA16(0x0808); // set BP and FP
	LCD_WRITE_CMD(0x000C); LCD_WRITE_DATA16(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 
	LCD_WRITE_CMD(0x000F); LCD_WRITE_DATA16(0x0D01); // Set frame rate  0B01
	//*************Power On sequence ****************
	LCD_DELAY(50); // Delay 50ms
	LCD_WRITE_CMD(0x0010); LCD_WRITE_DATA16(0x0000); // Set SAP,DSTB,STB
	LCD_WRITE_CMD(0x0011); LCD_WRITE_DATA16(0x1B41); // Set APON,PON,AON,VCI1EN,VC
	LCD_DELAY(50); // Delay 50ms
	LCD_WRITE_CMD(0x0012); LCD_WRITE_DATA16(0x200E); // Internal reference voltage= Vci;
	LCD_WRITE_CMD(0x0013); LCD_WRITE_DATA16(0x003E); //5D Set GVDD   //0x0064
	LCD_WRITE_CMD(0x0014); LCD_WRITE_DATA16(0x4B64); //0x6561 // Set VCOMH/VCOML voltage   //4d5f  0x4B50  0x5150
	//Vcom=Vcom+0x100;
	//------------- Set GRAM area ------------------//
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
	// ----------- Adjust the Gamma Curve ----------//

	LCD_WRITE_CMD(0x0050); LCD_WRITE_DATA16(0x0003);
	LCD_WRITE_CMD(0x0051); LCD_WRITE_DATA16(0x0900);
	LCD_WRITE_CMD(0x0052); LCD_WRITE_DATA16(0x0d05);
	LCD_WRITE_CMD(0x0053); LCD_WRITE_DATA16(0x0900);
	LCD_WRITE_CMD(0x0054); LCD_WRITE_DATA16(0x0407);
	LCD_WRITE_CMD(0x0055); LCD_WRITE_DATA16(0x0502);
	LCD_WRITE_CMD(0x0056); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0057); LCD_WRITE_DATA16(0x0005);
	LCD_WRITE_CMD(0x0058); LCD_WRITE_DATA16(0x1700);
	LCD_WRITE_CMD(0x0059); LCD_WRITE_DATA16(0x001F);

	LCD_DELAY(50); // Delay 50ms
	LCD_WRITE_CMD(0x0020); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0021); LCD_WRITE_DATA16(0x0000);
	LCD_WRITE_CMD(0x0007); LCD_WRITE_DATA16(0x1017);//Display On
	LCD_DELAY(150); // Delay 50ms
	
	

    #endif
}

static void disp_ic_setwindow(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col)
{
    LCD_WRITE_CMD(0x38);
    LCD_WRITE_DATA16((uint16)end_row);
    LCD_WRITE_CMD(0x39);
    LCD_WRITE_DATA16((uint16)start_row);

    /* Set LCD hardware to set start address */
    /* Transfer command to display hardware */
    LCD_WRITE_CMD(0x36); 
    LCD_WRITE_DATA16((uint16)end_col);
    LCD_WRITE_CMD(0x37);
    LCD_WRITE_DATA16((uint16)start_col);

    LCD_WRITE_CMD(0x20);
    LCD_WRITE_DATA16((uint16)start_col);
    LCD_WRITE_CMD(0x21);
    LCD_WRITE_DATA16((uint16)start_row);
    
    LCD_WRITE_CMD(0x22);
}
//uint32 kk=0x4b5E; 

static void disp_ic_sleep(boolean bin)
{
    if(bin)
    {
        LCD_WRITE_CMD(0x07);
        LCD_WRITE_DATA16(0x0000);
        LCD_DELAY(50);

        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0001); // Enter Standby mode
    }
    else
    {
        LCD_WRITE_CMD(0x10);
        LCD_WRITE_DATA16(0x0000);
        LCD_DELAY(50);
        /*
            LCD_WRITE_CMD(0x0012); LCD_WRITE_DATA16(0x200E); // Internal reference voltage= Vci;
            LCD_WRITE_CMD(0x0013); LCD_WRITE_DATA16(0x003E); //5D Set GVDD   //0x0064
            LCD_WRITE_CMD(0x0014); LCD_WRITE_DATA16(kk); //0x6561 // Set VCOMH/VCOML voltage   //4d5f  0x4B50  0x5150
*/
        LCD_WRITE_CMD(0x07);
        LCD_WRITE_DATA16(0x1017); //Exit Sleep
        LCD_DELAY(50);
  // kk=kk+0x0001;
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_ili9225g_tft20_rot(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2;
    
    // Read ID
    LCD_WRITE_CMD(0x00);
    id1 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    
    //if(id1 == 0x92 && id2 == 0x25)
    {
        DISP_IC_INIT_TBL(pdispic);
        return TRUE;
    }
    return FALSE;
}
