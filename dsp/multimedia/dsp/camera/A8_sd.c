/************************************************************************************************************
 AIT810 SD API
			Copyright (C) 2004-2005 Alpha Imaging Technology Corp.

 Description :
   The purpose of AIT810 SD API is for Host CPU to control and communicate AIT810
	 SD card Interface

************************************************************************************************************/
#include "A8_common.h"
#include "AIT700_EBIbus.h"
#include "A8_sd.h"

u_int	glsdCardAddr = 0x00100000;
static u_int	glsdCardSize;
u_short gsIsSD;
u_short gsIsHC = 0;

u_char	gbHighSpeedMode;

u_char gbSdCardOCR[4];
u_char gbSdCardCID[16];

u_int	glsdDmaAddr = 0x18000 -0x800;
u_int	glsdDmaSize = 0x800;

#define	SD_SECTOR_SIZE	0x200
#ifdef MTK_PLATFORM 
#define	SD_READ_DELAY		(3)
#else
#define	SD_READ_DELAY		(20)
#endif
#define	SD_TIMEOUT_COUNT 	(50000)

#define SD_CLOCK_DIV            SD_CLOCK_DIV_2
u_char gbSD_CSD[17];

#ifdef	SUPPORT_PRAGMA	
#pragma	O2
#endif
u_char* A8L_GetSDCSDValue(void)
{
	return gbSD_CSD;
}
/*======================================================================
 function : A8L_EnableStorageClock									
 parameters:															
 return : 															
 description : 														
======================================================================*/
u_short	A8L_EnableStorageClock(u_short enable)
{
	if (enable)
	{
		SetA8RegB(0x6904, GetA8RegB(0x6904) & ~0x08);
		SetA8RegB(0x692C, (GetA8RegB(0x692C) & (~0x0C)) | 0x08);

		/* Enable the clock*/
		SetA8RegW(SD_CLOCK_CTL, SD_CLOCK_DIV);

		SetA8RegW(0x690A, 0x0080); //Soft ware SD reset
		sys_IF_ait_delay1us(SD_READ_DELAY); //need delay 20us, else it will be abnormal on sector r/w.
		SetA8RegW(0x690A, 0);
		
	}
	else
	{
	
		SetA8RegW(SD_CLOCK_CTL, 0);
		SetA8RegB(0x692C, GetA8RegB(0x692C) & (~0x0C));
		SetA8RegB(0x6904, GetA8RegB(0x6904) | 0x08);
		SetA8RegB(SD_RESPONSE_TYPE, 0);//SD2.0
	}

    return SD_NO_ERROR;
}
/*======================================================================
 function : A8L_InitSDIF											
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_short	A8L_InitSDIF(void)
{
    gsIsSD = 0;

	SetA8RegB(0x6960, (GetA8RegB(0x6960) | 0x70)); // 20080614, // 2T SRAM buffer memory Enable!
	
    SetA8RegB(0x692C, GetA8RegB(0x692C) & (~0x0C));
    SetA8RegB(0x692C, GetA8RegB(0x692C) | 0x08);
/*	
	//Antonio@20100225, change 0x04 to 0x74 for more driving current
    SetA8RegB(0x69D9, (GetA8RegB(0x69D9) & (~0x06)) | 0x74);	// Clk
    SetA8RegB(0x69DA, (GetA8RegB(0x69DA) & (~0x06)) | 0x74);	// Cmd
    SetA8RegB(0x69DB, (GetA8RegB(0x69DB) & (~0x06)) | 0x74);	// D0
    SetA8RegB(0x69DC, (GetA8RegB(0x69DC) & (~0x06)) | 0x74);	// D1
    SetA8RegB(0x69DD, (GetA8RegB(0x69DD) & (~0x06)) | 0x74);	// D2
    SetA8RegB(0x69DE, (GetA8RegB(0x69DE) & (~0x06)) | 0x74);	// D3
*/	
    SetA8RegB(0x69D9, (GetA8RegB(0x69D9) & (~0x06)) | 0x04);	// Clk
    SetA8RegB(0x69DA, (GetA8RegB(0x69DA) & (~0x06)) | 0x04);	// Cmd
    SetA8RegB(0x69DB, (GetA8RegB(0x69DB) & (~0x06)) | 0x04);	// D0
    SetA8RegB(0x69DC, (GetA8RegB(0x69DC) & (~0x06)) | 0x04);	// D1
    SetA8RegB(0x69DD, (GetA8RegB(0x69DD) & (~0x06)) | 0x04);	// D2
    SetA8RegB(0x69DE, (GetA8RegB(0x69DE) & (~0x06)) | 0x04);	// D3

    SetA8RegB(SD_TIMEOUT, 0xff);	/* maximum timeout*/

    /*	SetA8RegB(SD_CONTROL_I, PUSH_PULL_TYPE);*/

    return SD_NO_ERROR;
}
/*======================================================================
 function : A8L_ResetMMC											
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_short	A8L_ResetMMC(void)
{
    u_short	i;
    u_char	csizemult;
    u_int	c_size;
    u_char	block_size;

    A8L_EnableStorageClock(1);

    SetA8RegB(SD_CONTROL_I, GetA8RegB(SD_CONTROL_I) & 0x3f | MMC_MODE_EN | PUSH_PULL_TYPE);
    SetA8RegW(SD_CLOCK_CTL, SD_CLOCK_DIV_256);

    gbHighSpeedMode = 0;

    /* CMD	0*/
    A8L_SendSDCommand(GO_IDLE_STATE, 0);

   sys_IF_ait_delay1ms(2);

    i = 0;
    do {
        sys_IF_ait_delay1ms(2);
        /* CMD	1*/
        if((!A8L_SendSDCommand(SEND_OP_COND, SUPPORT_VOLTAGE))) {
            i++;
        }
        else {
            A8L_EnableStorageClock(0);
            return	SD_RESET_ERROR;
        }
    } while(!(GetA8RegW(0x643e) & 0x8000) && (i < 30));

    if (i == 30) {
        AIT_Message_P0("Get OCR failed\r\n");
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }

    AIT_Message_P1("Get OCR success. %d times\r\n", i);

    /* CMD	2*/
    if (A8L_SendSDCommand(ALL_SEND_CID, 0)) {
        A8L_EnableStorageClock(0);
        return	SD_RESET_ERROR;
    }

    glsdCardAddr += 0x10000;

    /* CMD	3*/
    if (A8L_SendSDCommand(SET_RELATIVE_ADDR, glsdCardAddr)) {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }

    sys_IF_ait_delay1ms(2);

    /* CMD	9*/
    if (!A8L_SendSDCommand(SEND_CSD, glsdCardAddr)) {
        block_size = GetA8RegW(0x643A) & 0x000f;
        c_size = (GetA8RegW(0x6438) & 0x03ff) << 2;
        c_size += (GetA8RegW(0x6436) & 0xC000) >> 14;
        csizemult = (GetA8RegB(0x6436) & 0x03) << 1;
        csizemult += (GetA8RegB(0x6435) & 0x80) >> 7;

        glsdCardSize = (((u_int)(c_size + 1)) * (1 << (csizemult + 2))) << (block_size - 9);
      for ( i = 0x00; i < 0x10; i++ )
      {               
         gbSD_CSD[i] = GetA8RegB( 0x6430 + i );
      }
    }
    else {
        A8L_EnableStorageClock(0);
        return	SD_RESET_ERROR;
    }

    AIT_Message_P1("glsdCardSize = 0x%x\r\n", glsdCardSize);

    SetA8RegW(SD_BLOCK_LEN, 0x200);
    SetA8RegW(SD_CLOCK_CTL, SD_CLOCK_DIV);//SD_CLOCK_DIV_3);

    gbHighSpeedMode = 1;
    /* Select SD card*/
    if (A8L_SendSDCommand(SELECT_CARD, glsdCardAddr))
        return SD_RESET_ERROR;

    if (A8L_SendSDCommand(SET_BLOCKLEN, 0x200))
        return SD_RESET_ERROR;

    AIT_Message_P0("Enter 1-bit SD Mode\r\n");

    gsIsSD = 0;

    /* DeSelect SD card*/
    /*	A8L_sd_DeselectCard();*/

    A8L_EnableStorageClock(0);

    return	SD_NO_ERROR;
}
/*======================================================================
 function : A8L_ResetSD												
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/

u_short	A8L_ResetSD(void)
{
    u_short	i;
    u_char	csizemult;
    u_short	c_size;
    u_char	block_size;
	u_char 	ret_byte_count=0;

    A8L_EnableStorageClock(1);
#if	defined(SD_1_WIRE)
    SetA8RegB(SD_CONTROL_I, GetA8RegB(SD_CONTROL_I) & 0x3f | SD_1_WIRE_MODE_EN |PUSH_PULL_TYPE);
#else
    SetA8RegB(SD_CONTROL_I, GetA8RegB(SD_CONTROL_I) & 0x3f | SD_4_WIRE_MODE_EN |PUSH_PULL_TYPE);
#endif
    SetA8RegW(SD_CLOCK_CTL, SD_CLOCK_DIV_256);

    gbHighSpeedMode = 0;

    /* CMD	0*/
    A8L_SendSDCommand(GO_IDLE_STATE, 0);
#if 1	//added by ming @2009117    
    A8L_SendSDCommand(SEND_IF_COND, (0x1<<8)|0xAA);    
#endif
    sys_IF_ait_delay1ms(2);

    i = 0;
    do {
        sys_IF_ait_delay1ms(2);
        /* AMCD41*/
        if(!A8L_SendSDCommand(APP_CMD, 0)) {
#if 1	//modified by ming @20091117     
            if(!A8L_SendSDCommand(SD_APP_OP_COND, SUPPORT_VOLTAGE | (1<<30))) {
#else
            if(!A8L_SendSDCommand(SD_APP_OP_COND, SUPPORT_VOLTAGE)) {
#endif
                i++;
            }
            else {
                A8L_EnableStorageClock(0);
                return	SD_RESET_ERROR;
            }
        }
        else {
            A8L_EnableStorageClock(0);
            return	SD_RESET_ERROR;
        }
    } while(!(GetA8RegB(SD_PARAMETER_BUF_XVI_B) & 0x80) && (i < 500));

    if (i >= 500) {		//Gason_070530: update delay times for KingMax card
        AIT_Message_P0("Get OCR failed\r\n");
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }
	for (ret_byte_count = 0; ret_byte_count < 4; ret_byte_count++)
	{
		gbSdCardOCR[ret_byte_count] = GetA8RegB(0x643C + ret_byte_count);
	}

    AIT_Message_P1("Get OCR success. %d times\r\n", i);

	if(GetA8RegB(SD_PARAMETER_BUF_XVI_B) & 0x40) {
		gsIsHC = 1;
		
	}else{
		gsIsHC = 0;
	}
    AIT_Message_P1("gsIsHC = %d \r\n", gsIsHC);	
    /* CMD	2*/
    if (A8L_SendSDCommand(ALL_SEND_CID, 0)) {
        A8L_EnableStorageClock(0);
        return	SD_RESET_ERROR;
    }

    /* CMD	3*/
    if(!A8L_SendSDCommand(SEND_RELATIVE_ADDR, 0)) {
        glsdCardAddr = ((u_int)GetA8RegW(SD_PARAMETER_BUF_XV_W) << 16);
    }
    else {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }

    /* CMD	9*/
    if (!A8L_SendSDCommand(SEND_CSD, glsdCardAddr)) {
    	if(gsIsHC){
    		c_size = (GetA8RegW(0x6436) | (GetA8RegW(0x6438) & 0x3f));
    		glsdCardSize = (c_size + 1) * 1024;
    	}else{
        block_size = GetA8RegW(0x643A) & 0x000f;
        c_size = (GetA8RegW(0x6438) & 0x03ff) << 2;
        c_size += (GetA8RegW(0x6436) & 0xC000) >> 14;
        csizemult = (GetA8RegB(0x6436) & 0x03) << 1;
        csizemult += (GetA8RegB(0x6435) & 0x80) >> 7;

        glsdCardSize = (((u_int)(c_size + 1)) * (1 << (csizemult + 2))) << (block_size - 9);
    	}

	AIT_Message_P0("CSD OK!!\r\n");
	
	for (ret_byte_count = 0; ret_byte_count < 16; ret_byte_count++)
      {               
		gbSD_CSD[ret_byte_count] = GetA8RegB(0x6430 + ret_byte_count);
      }
	gbSD_CSD[ret_byte_count] = gsIsHC;

//		A8L_GetTransferSpeed(gbSD_CSD[0x0c]);

    } else {
        A8L_EnableStorageClock(0);
        return	SD_RESET_ERROR;
    }


    AIT_Message_P1("glsdCardSize = 0x%x\r\n", glsdCardSize);

    SetA8RegW(SD_BLOCK_LEN, 0x200);
    /* KingMax's Max clock 25Mhz*/
#if 0	//test by ming @20090410	
    SetA8RegW(SD_CLOCK_CTL, SD_CLOCK_DIV_4);	//25MHz is T-card limitation
#else
    SetA8RegW(SD_CLOCK_CTL, SD_CLOCK_DIV_2);//SD_CLOCK_DIV_3);	//25MHz is T-card limitation
#endif
    gbHighSpeedMode = 1;

    /* Select SD card*/
    if (A8L_SendSDCommand(SELECT_CARD, glsdCardAddr)) {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }

    /*	AIT_SUNNY_20051010 dont clear SD internal resistor for card in detect */
#if 1	//added by ming @20091117
    /* ACMD42	*/
    if (A8L_SendSDCommand(APP_CMD, glsdCardAddr)) {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }

    if (A8L_SendSDCommand(SET_CLR_CARD_DETECT, (glsdCardAddr|0))) {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }
#endif
#if	defined(SD_1_WIRE)
    AIT_Message_P0("Enter 1-bit SD Mode\r\n");
#else
    if (A8L_SendSDCommand(APP_CMD, glsdCardAddr)) {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }

    if (A8L_SendSDCommand(SET_BUS_WIDTH, 0x2)) {
        A8L_EnableStorageClock(0);
        return SD_RESET_ERROR;
    }
    AIT_Message_P0("Enter 4-bit SD Mode\r\n");
#endif	
    /* DeSelect SD card*/
    /*	A8L_sd_DeselectCard();*/

    gsIsSD = 1;

    A8L_EnableStorageClock(0);
    /*DAT3 is high befor starting access t-flash*/
#if 0
    /* Blocked by han chang hyun , 2005.10.01	*/
    /*	SetA8RegB(0x69DE, 4 );*/
    /*	SetA8RegB(0x6601, GetA8RegB(0x6601) & (~(1<<1)) );*/
#endif	
    return	SD_NO_ERROR;
}
/*======================================================================
 function : A8L_GetSDSize											
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_int A8L_GetSDSize(void)
{
    return	glsdCardSize;
}
/*======================================================================
 function : A8L_GetSDCardAddr										
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_int	A8L_GetSDCardAddr(void)
{
    return	glsdCardAddr;
}
/*======================================================================
 function : A8L_SendSDCommand										
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_short	A8L_SendSDCommand(u_char command, u_int argument)
{
    u_char	tmp;
    u_short	i;

    if(command == SEND_IF_COND)//SD2.0
    {
        SetA8RegB(SD_RESPONSE_TYPE, command | OTHER_RESP);
    }
    
    SetA8RegW(SD_COMMAND_ARG_L, (u_short)(argument));
    SetA8RegW(SD_COMMAND_ARG_H, (u_short)(argument >> 16));

    SetA8RegB(SD_HOST_INT_STAT, 0xff);

    SetA8RegB(SD_CONTROL_II, command | SD_TRANSFER_START);

    if (command == GO_IDLE_STATE) {
        while(GetA8RegB(SD_CONTROL_II) & SD_TRANSFER_START);
    }
    else {
        while(1) {
            tmp = GetA8RegB(SD_HOST_INT_STAT);
            if (tmp & COMMAND_RESPONSE_DONE)
                break;
            if (tmp & COMMAND_RESPONSE_TIMEOUT) {
                AIT_Message_P2("Command %d Time Out. INT_STATUS = 0x%X\r\n", command, GetA8RegB(SD_HOST_INT_STAT));
                SetA8RegB(SD_RESPONSE_TYPE, GetA8RegB(SD_RESPONSE_TYPE) | 0x40);
                sys_IF_ait_delay1us(100);
                SetA8RegB(SD_RESPONSE_TYPE, GetA8RegB(SD_RESPONSE_TYPE) & ~(0x40));
                return SD_COMMAND_FAILED;
            }
        }
    }

    if (command == STOP_TRANSMISSION || command == SELECT_CARD) {
        while(!(GetA8RegB(SD_BUS_STATUS) & DATA0_STATUS_HIGH));
        if (GetA8RegB(SD_HOST_INT_STAT) & (CRC7_ERROR | BUS_STATUS_ERROR))  {
            AIT_Message_P2("Command %d Error = 0x%X\r\n", command, GetA8RegB(SD_HOST_INT_STAT));
            SetA8RegB(SD_RESPONSE_TYPE, 0);
            return	SD_COMMAND_FAILED;
        }
    }
    else {
        if ((command != ALL_SEND_CID) && (command != SET_RELATIVE_ADDR)) {
            if (GetA8RegB(SD_HOST_INT_STAT) &
                    (CRC7_ERROR | BUS_STATUS_ERROR | COMMAND_RESPONSE_TIMEOUT)) {
                AIT_Message_P2("Command %d Error = 0x%X\r\n", command, GetA8RegB(SD_HOST_INT_STAT));
	            SetA8RegB(SD_RESPONSE_TYPE, 0);                
                return	SD_COMMAND_FAILED;
            }
        }
    }

    if (!gbHighSpeedMode)
        for (i = 0; i < 0x5000; i++);
    SetA8RegB(SD_RESPONSE_TYPE, 0); 
    return	SD_NO_ERROR;
}
/*======================================================================
 function : A8L_sd_DeselectCard											
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_short	A8L_sd_DeselectCard()
{
    SetA8RegB(SD_CONTROL_II, DESELECT_CARD);
    SetA8RegW(SD_COMMAND_ARG_H, 0);
    SetA8RegW(SD_COMMAND_ARG_L, 0);

    SetA8RegB(SD_CPU_INT_STAT, 0xff);

    SetA8RegB(0x6414, 0x40);

    SetA8RegB(SD_CONTROL_II, GetA8RegB(SD_CONTROL_II) | SD_TRANSFER_START);

    while(GetA8RegB(SD_CONTROL_II) & 0x80);

    SetA8RegB(0x6414, 0x00);

    return	1;
}
/*======================================================================
 function : A8L_ReadSDSector										
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_short	A8L_ReadSDSector(u_int dmastartaddr, u_int startsect, u_short sectcount)
{
    u_int	time_out;
	u_int i;
    /*	AIT_Message_P2("READ : startsect : sectcount = 0x%X, 0x%X\r\n", startsect, sectcount);*/
	if(!sectcount)
		return SD_PARAM_ERROR;
		
    A8L_EnableStorageClock(1);

    SetA8RegW(0x6222, GetA8RegW(0x6222) & ~(0x0800));

    /* Select SD card*/
    /*	if (A8L_SendSDCommand(SELECT_CARD, glsdCardAddr))*/
    /*		return SD_COMMAND_FAILED;*/

    SetA8RegW(SD_DMA_START_ADDR_L, (u_short)(dmastartaddr));
    SetA8RegW(SD_DMA_START_ADDR_H, (u_short)(dmastartaddr >> 16));

	if(gsIsHC)//SD2.0
    {
        SetA8RegW(SD_COMMAND_ARG_L, (u_short) startsect);
        SetA8RegW(SD_COMMAND_ARG_H, (u_short) (startsect >> 16));
    } else {
        SetA8RegW(SD_COMMAND_ARG_L, (u_short) (startsect << 9));
        SetA8RegW(SD_COMMAND_ARG_H, (u_short) ((startsect << 9) >> 16));
    }

    if (sectcount == 1) {
        SetA8RegB(SD_CONTROL_I,
                      GetA8RegB(SD_CONTROL_I) & 0xf0 | SINGLE_BLOCK_MODE);
        SetA8RegB(SD_CONTROL_II, READ_SINGLE_BLOCK);
    }
    else {
        SetA8RegB(SD_CONTROL_I,
                      GetA8RegB(SD_CONTROL_I) & 0xf0 | MULTIPLE_BLOCK_MODE);
        SetA8RegB(SD_CONTROL_II, READ_MULTIPLE_BLOCK);
    }

    SetA8RegW(SD_BLOCK_NUM, sectcount);

    SetA8RegB(SD_HOST_INT_STAT, 0xff);

    SetA8RegB(SD_CONTROL_II, GetA8RegB(SD_CONTROL_II) | SD_TRANSFER_START);

    time_out = 0;
    do {
        time_out++;
    }while((GetA8RegB(SD_CONTROL_II) & SD_TRANSFER_START) && (time_out < SD_TIMEOUT_COUNT));
    if (time_out >= SD_TIMEOUT_COUNT) {
        /* AIT_SUNNY_20051010 */
 //       A8L_InfoSDCardOut();
        AIT_Message_P0("**** SD Card READ: Wait for SD_TRANSFER_START time out\r\n");
        return	SD_TIME_OUT;
    }

    time_out = 0;
    do {
        time_out++;
    }while(!(GetA8RegB(SD_HOST_INT_STAT) & DATA_BLOCK_TRANSFER_DONE) && (time_out < SD_TIMEOUT_COUNT));
    if (time_out >= SD_TIMEOUT_COUNT) {
        /* AIT_SUNNY_20051010 */
//        A8L_InfoSDCardOut();
        AIT_Message_P0("**** SD Card READ: Wait for DATA_BLOCK_TRANSFER_DONE time out\r\n");
        return	SD_TIME_OUT;
    }

    if (sectcount != 1) {
        if(A8L_SendSDCommand(STOP_TRANSMISSION, 0)) {
            A8L_EnableStorageClock(0);
            return SD_COMMAND_FAILED;
        }
    }
    else {
        for (i = 0; i < 16; i++);
    }

    /* DeSelect SD card*/
    /*	A8L_sd_DeselectCard();*/

    A8L_EnableStorageClock(0);

    return SD_NO_ERROR;
}
/*======================================================================
 function : A8L_WriteSDSector										
 parameters:	none													
 description : Fill 3gp Tail (28 bytes return)						
======================================================================*/
u_short	A8L_WriteSDSector(u_int dmastartaddr, u_int startsect, u_short sectcount)
{
    u_int	time_out;
    u_int i;

    /*	AIT_Message_P2("WRITE : startsect : sectcount = 0x%X, 0x%X\r\n", startsect, sectcount);*/

    A8L_EnableStorageClock(1);

    SetA8RegW(0x6222, GetA8RegW(0x6222) & ~(0x0800));

    /* Select SD card*/
    /*	if (A8L_SendSDCommand(SELECT_CARD, glsdCardAddr))*/
    /*		return SD_COMMAND_FAILED;*/

    if (gsIsSD  && sectcount != 1) {
        /* ACMD	23*/
        if (A8L_SendSDCommand(APP_CMD, glsdCardAddr)) {
            A8L_EnableStorageClock(0);
            return SD_COMMAND_FAILED;
        }

        if (A8L_SendSDCommand(SET_WR_BLK_ERASE_COUNT, (u_int)sectcount)) {
            A8L_EnableStorageClock(0);
            return SD_COMMAND_FAILED;
        }
    }

    SetA8RegW(SD_DMA_START_ADDR_L, (u_short)(dmastartaddr));
    SetA8RegW(SD_DMA_START_ADDR_H, (u_short)(dmastartaddr >> 16));

	if(gsIsHC)//SD2.0
    {
        SetA8RegW(SD_COMMAND_ARG_L, (u_short) startsect);
        SetA8RegW(SD_COMMAND_ARG_H, (u_short) (startsect >> 16));
    } else {
        SetA8RegW(SD_COMMAND_ARG_L, (u_short) (startsect << 9));
        SetA8RegW(SD_COMMAND_ARG_H, (u_short) ((startsect << 9) >> 16));
    }

    if (sectcount == 1) {
        SetA8RegB(SD_CONTROL_I,
                      GetA8RegB(SD_CONTROL_I) & 0xf0 | SINGLE_BLOCK_MODE);
        SetA8RegB(SD_CONTROL_II, WRITE_BLOCK);
    }
    else {
        SetA8RegB(SD_CONTROL_I,
                      GetA8RegB(SD_CONTROL_I) & 0xf0 | MULTIPLE_BLOCK_MODE);
        SetA8RegB(SD_CONTROL_II, WRITE_MULTIPLE_BLOCK);
    }

    SetA8RegW(SD_BLOCK_NUM, sectcount);

    SetA8RegB(SD_HOST_INT_STAT, 0xff);

    SetA8RegB(SD_CONTROL_II, GetA8RegB(SD_CONTROL_II) | SD_TRANSFER_START);

    time_out = 0;
    do {
        time_out++;
    }while((GetA8RegB(SD_CONTROL_II) & SD_TRANSFER_START) && (time_out < SD_TIMEOUT_COUNT));
    if (time_out >= SD_TIMEOUT_COUNT) {
        /* AIT_SUNNY_20051010  */
//        A8L_InfoSDCardOut();
        AIT_Message_P0("**** SD Card Write: Wait for SD_TRANSFER_START time out\r\n");
        return	SD_TIME_OUT;
    }

    time_out = 0;
    do {
        time_out++;
    }while(!(GetA8RegB(SD_HOST_INT_STAT) & DATA_BLOCK_TRANSFER_DONE) && (time_out < (SD_TIMEOUT_COUNT*2)));
    if (time_out >= (SD_TIMEOUT_COUNT*2)) {
        /* AIT_SUNNY_20051010  */
//        A8L_InfoSDCardOut();
        AIT_Message_P0("**** SD Card Write: Wait for DATA_BLOCK_TRANSFER_DONE time out\r\n");
        return	SD_TIME_OUT;
    }

    if (sectcount == 1) {
        for (i = 0; i < 16; i++);
        while(!(GetA8RegB(SD_BUS_STATUS) & DATA0_STATUS_HIGH));
    }
    else {
        if (sectcount != 1) {
            if(A8L_SendSDCommand(STOP_TRANSMISSION, 0)) {
                A8L_EnableStorageClock(0);
                return SD_COMMAND_FAILED;
            }
        }
    }

    /* DeSelect SD card*/
    /*	A8L_sd_DeselectCard();*/

    A8L_EnableStorageClock(0);

    return SD_NO_ERROR;
}
/*======================================================================
 function : A8L_SetStorageDmaAddr									
 parameters:	none													
 description : 														
======================================================================*/
u_short	A8L_SetStorageDmaAddr(u_int dmastartaddr, u_int dmasize)
{
    glsdDmaAddr = dmastartaddr;
    glsdDmaSize	= dmasize;

    return SD_NO_ERROR;
}
/*======================================================================
 function : A8L_ReadSD												
 parameters:	none													
 description : 														
======================================================================*/
u_short	A8L_ReadSD(u_int startsect, u_int offset, u_char* buf, u_int read_size)
{
    u_char	*tmp;
    u_int	tmp_read_size;
    u_short	fRet;
    u_int	sect_count;

    tmp = buf;

    if (offset) {
        tmp_read_size = SD_SECTOR_SIZE - offset;
        if (read_size > tmp_read_size)
            tmp_read_size = SD_SECTOR_SIZE - offset;
        else
            tmp_read_size = read_size;

        fRet = A8L_ReadSDSector(glsdDmaAddr, startsect, 1);
        if (fRet)
            return fRet;

        CopyMemByteA8ToHost(tmp, glsdDmaAddr + offset, tmp_read_size);

        read_size -= tmp_read_size;
        startsect += 1;
        tmp += tmp_read_size;
    }

    while (read_size) {
        if (read_size >= glsdDmaSize) {
            tmp_read_size = glsdDmaSize;
            sect_count = tmp_read_size/SD_SECTOR_SIZE;

            fRet = A8L_ReadSDSector(glsdDmaAddr, startsect, sect_count);
            if (fRet)
                return fRet;

            CopyMemByteA8ToHost(tmp, glsdDmaAddr, tmp_read_size);
        }
        else {
            tmp_read_size = read_size;

            if (tmp_read_size & ~(SD_SECTOR_SIZE))
                sect_count = tmp_read_size/SD_SECTOR_SIZE + 1;
            else
                sect_count = tmp_read_size/SD_SECTOR_SIZE;

            fRet = A8L_ReadSDSector(glsdDmaAddr, startsect, sect_count);
            if (fRet)
                return fRet;

            CopyMemByteA8ToHost(tmp, glsdDmaAddr, tmp_read_size);
        }
        read_size -= tmp_read_size;
        startsect += sect_count;
        tmp += tmp_read_size;
    }

    return SD_NO_ERROR;
}
/*======================================================================
 function : A8L_WriteSD												
 parameters:	none													
 description : 														
======================================================================*/
u_short	A8L_WriteSD(u_int startsect, u_int offset, u_char* buf, u_int write_size)
{
    u_char	*tmp;
    u_int	tmp_write_size;
    u_short	fRet;
    u_int	sect_count;
    u_int	write_left;

    tmp = buf;
    write_left = write_size;

    if (offset) {
        tmp_write_size = SD_SECTOR_SIZE - offset;
        if (write_left > tmp_write_size)
            tmp_write_size = SD_SECTOR_SIZE - offset;
        else
            tmp_write_size = write_left;

        fRet = A8L_ReadSDSector(glsdDmaAddr, startsect, 1);
        if (fRet)
            return	fRet;

        CopyMemByteHostToA8(glsdDmaAddr + offset, tmp, tmp_write_size);
        fRet = A8L_WriteSDSector(glsdDmaAddr, startsect, 1);
        if (fRet)
            return	fRet;

        tmp += tmp_write_size;
        write_left -= tmp_write_size;
        startsect += 1;
    }

    while (write_left >= SD_SECTOR_SIZE) {
        if (write_left >= glsdDmaSize) {
            sect_count = glsdDmaSize / SD_SECTOR_SIZE;
            tmp_write_size = glsdDmaSize;
        }
        else {
            sect_count = write_left / SD_SECTOR_SIZE;
            tmp_write_size = sect_count * SD_SECTOR_SIZE;
        }
        CopyMemByteHostToA8(glsdDmaAddr, tmp, tmp_write_size);
        fRet = A8L_WriteSDSector(glsdDmaAddr, startsect, sect_count);
        if (fRet)
            return	fRet;

        tmp += tmp_write_size;
        write_left -= tmp_write_size;
        startsect += sect_count;
    }

    if (write_left) {
        fRet = A8L_ReadSDSector(glsdDmaAddr, startsect, 1);
        if (fRet)
            return	fRet;

        CopyMemByteHostToA8(glsdDmaAddr, tmp, write_left);
        fRet = A8L_WriteSDSector(glsdDmaAddr, startsect, 1);
        if (fRet)
            return	fRet;
    }

    return SD_NO_ERROR;
}

u_short A8L_EraseDevice(void)
{
	u_short error_code = SD_NO_ERROR;
	u_int loop_cnt;

	//  BUGBUG: need check the corresponding function!!!.
	A8L_EnableStorageClock(1);

	error_code = A8L_SendSDCommand(SET_BLOCKLEN, 0x00000200);
	if (error_code != SD_NO_ERROR)
	{
		A8L_EnableStorageClock(0);
		return SD_COMMAND_FAILED;
	}			
	error_code = A8L_SendSDCommand(ERASE_WR_BLK_START, 0x00000000);
	if (error_code != SD_NO_ERROR)
	{
		A8L_EnableStorageClock(0);
		return SD_COMMAND_FAILED;
	}		
	error_code = A8L_SendSDCommand(ERASE_WR_BLK_END, A8L_GetSDSize());
	if (error_code != SD_NO_ERROR)
	{
		A8L_EnableStorageClock(0);
		return SD_COMMAND_FAILED;
	}		
	error_code = A8L_SendSDCommand(ERASE, 0x00000000);
	//		if(error_code != SD_NO_ERROR)
	//		{
	//  		EnableStorageClock(0);
	//			return SD_COMMAND_FAILED;
	//		}		

	//for(loop_cnt=0; loop_cnt<0x800000; loop_cnt++);
	for (loop_cnt = 0; loop_cnt < 0x50000; loop_cnt++);

	//  BUGBUG: need check the corresponding function!!!.
	A8L_EnableStorageClock(0);

	return SD_NO_ERROR;
}

u_short A8L_CheckSDDevice(void)
{
	A8L_InitSDIF();

	if (SD_NO_ERROR == A8L_ResetSD())
	{
		AIT_Message_P0("A8L_ResetSD OK\r\n");
		return SD_NO_ERROR;
	}
	else
	{
		AIT_Message_P0("A8L_ResetSD fail\r\n");
		return SD_RESET_ERROR;
	}
}
/*======================================================================
 function : A8L_CheckSDCardIn										
 parameters:	none													
 description : 														
======================================================================*/
#if 0
u_short	A8L_CheckSDCardIn(void)
{
    if (gbSDCardDetectionPolarity) {
        if (gbSDCardDetectionPinNum >= 16) {
            if (GetA8RegW(GPIO_DATA_2) & (1 << (gbSDCardDetectionPinNum - 16)))
                return	A8_CARD_EXIST;
            else
                return	A8_CARD_NOT_EXIST;
        }
        else {
            if (GetA8RegW(GPIO_DATA_0) & (1 << gbSDCardDetectionPinNum))
                return	A8_CARD_EXIST;
            else
                return	A8_CARD_NOT_EXIST;
        }
    }
    else {
        if (gbSDCardDetectionPinNum >= 16) {
            if (GetA8RegW(GPIO_DATA_2) & (1 << (gbSDCardDetectionPinNum - 16)))
                return	A8_CARD_NOT_EXIST;
            else
                return	A8_CARD_EXIST;
        }
        else {
            if (GetA8RegW(GPIO_DATA_0) & (1 << gbSDCardDetectionPinNum))
                return	A8_CARD_NOT_EXIST;
            else
                return	A8_CARD_EXIST;
        }
    }
}
/*======================================================================
 function : A8L_CheckSDCardWP										
 parameters:	none													
 description : 														
======================================================================*/
u_short	A8L_CheckSDCardWP(void)
{
    if (gbSDCardDetectionPolarity) {
        if (gbSDCardWProtectPinNum >= 16) {
            if (GetA8RegW(GPIO_DATA_2) & (1 << (gbSDCardWProtectPinNum - 16)))
                return	A8_CARD_WP;
            else
                return	A8_CARD_NOT_WP;
        }
        else {
            if (GetA8RegW(GPIO_DATA_0) & (1 << gbSDCardWProtectPinNum))
                return	A8_CARD_WP;
            else
                return	A8_CARD_NOT_WP;
        }
    }
    else {
        if (gbSDCardWProtectPinNum >= 16) {
            if (GetA8RegW(GPIO_DATA_2) & (1 << (gbSDCardWProtectPinNum - 16)))
                return	A8_CARD_NOT_WP;
            else
                return	A8_CARD_WP;
        }
        else {
            if (GetA8RegW(GPIO_DATA_0) & (1 << gbSDCardWProtectPinNum))
                return	A8_CARD_NOT_WP;
            else
                return	A8_CARD_WP;
        }
    }
}/* AIT_SUNNY_20051010 Enable INT from BB  */
/*==============================================================================
 function : A8L_InfoSDCardOut
 parameters:
 description :
==============================================================================*/
u_short A8L_InfoSDCardOut(void)
{
    SetA8RegW(A8_DSC_STATUS_RG, GetA8RegW(A8_DSC_STATUS_RG) | 0x2000);
    A8L_SetIntLevel();
    A8L_EnableStorageClock(0);
    return  SD_NO_ERROR;
}
#endif
