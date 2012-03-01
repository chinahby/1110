#include "disp_drv.h"

#define DISP_IC_WIDTH_MAX   160

#include "disp_ic_generic.h"

#ifdef FEATURE_MDP
#include "mdp_drv.h"
#include "mdp_hw.h"

//For lcd QVGA ST7735R driver
#define DISP_LCD_18BPP(x)                 ((uint16)(x))
#define DISP_LCD_HORZ_RAM_ADDR_POS_1_ADDR 0x2A // Register to set col start
#define DISP_LCD_VERT_RAM_ADDR_POS_1_ADDR 0x2B // Register to set row start
#define DISP_LCD_CMD_RAMWR                0x2C // RAM Data Write

#ifdef __GNUC__
static uint32 MDP_DISP_SCR_ST7735R[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN]
__attribute__ ((aligned (16))) =
#else
__align(16) static uint32 MDP_DISP_SCR_ST7735R[MDP_LCD_SCR_SIZE][MDP_LCD_SCR_LEN] =
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
    return LCD_16BPP_88_II;
}

static uint16 disp_ic_mdp_getscr(uint32 **ppscr)
{
    if(ppscr)    
    {
        *ppscr = &MDP_DISP_SCR_ST7735R[0][0];
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
    /************以下代码为68110初始化************/
//厂商调试的版本“

	LCD_WRITE_CMD(0x01);       // Software Reset
	LCD_WRITE_CMD(0x11);       // Sleep Out
	LCD_DELAY(120); 	            // Must Delay At Least 100 ms
	
	LCD_WRITE_CMD(0xC0);       // Power Control 1
	LCD_WRITE_DATA(0xF0);      // GVDD Setting//0xF1//调节亮度，值越大越亮，颜色就越淡，
	LCD_WRITE_DATA(0x10);      // GVCL Setting//0x11//以上面同步
	
	LCD_WRITE_CMD(0xC1);	  //set VGH&VGL driver voltage
	LCD_WRITE_DATA(0x09);//ff

	LCD_WRITE_CMD(0xC2);
	LCD_WRITE_DATA(0x05);

	LCD_WRITE_CMD(0xB4);
	LCD_WRITE_DATA(0x02);
	
	LCD_WRITE_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_WRITE_DATA(0x0B); //31
	
	LCD_WRITE_CMD(0xF8);
	LCD_WRITE_DATA(0x01);
	
	LCD_WRITE_CMD(0x3A);		//设置色位;
	LCD_WRITE_DATA(0x55);
	//************* Start Gamma Setting **********//

	LCD_WRITE_CMD(0xE0);       // Gamma Command
	LCD_WRITE_DATA(0x00);      
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x20);
	LCD_WRITE_DATA(0x1E);
	LCD_WRITE_DATA(0x1B);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x0C);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x0D);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_CMD(0xE1);       // Gamma Command
	LCD_WRITE_DATA(0x00);      
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x26);
	LCD_WRITE_DATA(0x1E);
	LCD_WRITE_DATA(0x1B);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x0C);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x06);
	LCD_WRITE_DATA(0x08);
	LCD_WRITE_DATA(0x0D);
	LCD_WRITE_DATA(0x05);
	LCD_WRITE_DATA(0x08);
	
	LCD_WRITE_CMD(0x36);       // Memory Data Access Control
	LCD_WRITE_DATA(0xC8);      // RGB Color Filter Setting

	LCD_WRITE_CMD(0xB1);		//set gamma ver
	LCD_WRITE_DATA(0x01);
	LCD_WRITE_DATA(0x04);
	LCD_WRITE_DATA(0x18);

	LCD_WRITE_CMD(0xF4);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x08);

	LCD_WRITE_CMD(0xD9);
	LCD_WRITE_DATA(0x00); 

  	LCD_WRITE_CMD(0xC7);
	LCD_WRITE_DATA(0x19);//调节fincker
	
	LCD_WRITE_CMD(0x2A);
  	LCD_WRITE_DATA(0x00);//
	LCD_WRITE_DATA(0x00);//
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x7F);
	
	LCD_WRITE_CMD(0x2B);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x9F);

 	LCD_WRITE_CMD(0x29);       // Display Ondelay(20);
	LCD_DELAY(20);
	LCD_WRITE_CMD(0x2C);
	LCD_DELAY(20); 
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
        LCD_WRITE_CMD(0x28);//Sleep in        
	    LCD_DELAY(20);
	    LCD_WRITE_CMD(0x10);
	    LCD_DELAY(120);
    }
    else
    {
        LCD_WRITE_CMD(0x11); //Exit Sleep
        LCD_DELAY(120);

        LCD_WRITE_CMD(0x11);
     	LCD_DELAY(120);
     	//if(RM68110==read_ID)//判断开机时读出的ID是否为RM68110的ID，如果是的话就执行这里面的语句//D3寄存器的ID为0x00 0x68 0x11
     	{//以防VCOM电压在唤醒睡眠时会跑掉
     		LCD_WRITE_CMD(0xF8);
     		LCD_WRITE_DATA(0x01);
     		LCD_DELAY(20);
     		LCD_WRITE_CMD(0xF4);
     		LCD_WRITE_DATA(0x00);
     		LCD_WRITE_DATA(0x08);
     
     		LCD_WRITE_CMD(0xD9);
     		LCD_WRITE_DATA(0x00); 
     
       		LCD_WRITE_CMD(0xC7);
     		LCD_WRITE_DATA(0x19);
     	}
        LCD_WRITE_CMD(0x29);
        LCD_DELAY(20);
    }
}

static void disp_ic_rot(uint16 degree)
{
    return;
}

boolean disp_rm68110_tft177(disp_drv_ic_type *pdispic)
{
    uint8 id1,id2;
	uint8 id3;
    //extern uint8 ID1,ID2,ID3;
    
    // Read ID
    LCD_WRITE_CMD(0xDA);
    id1 = LCD_READ_DATA();
    id1 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xDB);
    id2 = LCD_READ_DATA();
    id2 = LCD_READ_DATA();
    LCD_WRITE_CMD(0xD3);
    id3 = LCD_READ_DATA();
    id3 = LCD_READ_DATA();
    // 0x00 0x68 0x11
    //ID1=id1,ID2=id2,ID3=id3;
    if(id3 == 0x68)
    {
        DISP_IC_INIT_TBL(pdispic);        
        return TRUE;
    }
    return FALSE;
}
