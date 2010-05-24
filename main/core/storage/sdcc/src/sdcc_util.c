/**********************************************************************
 * sdcc_util.c
 *
 * SDCC(Secure Digital Card Controller) driver utility functions.
 *
 * This file implements the utility functions for SDCC driver. 
 *
 * Copyright (c) 2004-2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_util.c#12 $
$DateTime: 2009/04/22 22:40:04 $ $Author: spuliven $

when       who     what, where, why
---------- --------------------------------------------------------------
2009-04-22 sp      Fix the PSN reading from CID
2009-01-05 rcc     Removed unnecessary inclusion of sdcc_debug.c
2008-12-03 vj      Fixed sdcc_read_fifo to increment buffer in unaligned reads
                   Modifed to use single signal FS_OP_COMPLETE_SIG
2008-11-17 vin     Moved TCXO reset to sdcc_sdio_read/write
2008-11-07 vin     Added timeout to sdcc_read_fifo
2008-10-31 vin     Support for SDCC power collapse
2008-10-07 vin     Always search for SDIO card first in sdcc_find_card
2008-08-25 vin     Use sdio_isr for SDIO interrupt.
2008-07-24 vin     Flush MMU dcache on targets that support it.
2008-06-19 vin     Added critical section locking
2008-06-19 vin     Removed SDCC Mux features
2008-06-11 sc      Ported card detection changes
03/28/08   sc      Re-wrote sdcc_write_fifo
03/28/08   sc      Removed 9.6MHz config that was left out in CC#22777
03/05/08   sc      Removed SDCC_PARAM_NOT_REF macro
02/29/08   sc      Fixed sdcc_read_fifo to handle data transfer less than 64
                   bytes and sdcc_set_tcxo_clk in sdcc_setup_data_xfer
02/28/08   vin     Register with clk_regim to keep HCLK constant
10/19/07   vin     Added FEATURE_SDCC_9P6MHZ_CLK
08/02/07   vin     Added feature FEATURE_SDCC_WLAN_CONFIG_API
07/20/07   dng     Added the FEATURE_SDCC_TURN_ON_POWER_SC2X to the sdcc_config_clk module.
06/13/07   dng     Reduce the SDIO clock for Phillip card to 9.6MHZ on M6800 target.
06/11/07   dng     Added the HCLOCK vote against sleep function for SC2X target.
05/21/07   dng     Moved the setup TCB to sdcc_process_interrupts function.
05/18/07   sc      Fixed the SDHC card size calculation
04/11/07   sc      Ported in MMCPlus support and fixed clock freq configuration
                   for CR 116981
03/28/07   vin     Reverted to the former sdcc_fifo_read and write. Added 
                   safe exit points under some conditions
03/15/07   vin     Added feature FEATURE_SDCC_CLK_CONFIG
03/05/07   vin     Fixed the sdcc_read_fifo and sdcc_write_fifo functions.
02/15/07   sc      Fixed the PNM, PSN fields and lint errors
01/10/07   dng     Added new include file to fix compile error for M6280 target.
11/28/06   dng     Replaced the FEATURE_6800_COMPILE with  T_MSM6800
11/05/06   dng     Added the PROG_DONE interrupts support.
11/05/06   dng     Added the new M6550 clock regime function calls to fix
                   the 400KHZ MMC detection.
10/16/06   dng     Added the sdio card detection into the sdcc_find_card function.
10/16/06   dng     Added assert.h header file to fix M6800 compilation issue.
10/04/06   dng     Fixed the compiler error for M6275 target.  
10/04/06   dng     Added the clk regimes call to turn on TCXO during sleep
                   for M6800 target.
10/04/06   dng     Added new vote agains sleep scheme for M6550 & M6800 targets.
09/22/06   dng     Removed all INTLOCK and INFREE macros.
09/07/06   dng     Added SDCC/SDIO Mux feature.
09/05/06   dng     Changed the sdcc_polling_status to CMD ISR .
08/23/06   dng     Changed the vote against sleep from Macro to function calls.
07/17/06   sc      Added SD2.0 feature
07/17/06   dng     First pass of code review clean up.
07/06/06   dng     Turned the INVERT_IN bit on for SDIO PHILLIP card.
06/30/06   dng     Increased the delay in sdcc_find_card function from
                   50 to 75 to detect slow PNY sd card.
06/20/06   dng     Changed the dprintf Macro to Tprintf in poll_status &
                   sdcc_command functions
06/07/06   dng     Reset the card_type and host_state in the sdcc_close 
                   function.   
05/25/06   dng     Removed the 20 microsecond delay in the sdcc_poll_status
05/22/06   dng     Added 20 microsecond delay in the sdcc_poll_status 
                   for M6800 and M6280 targets. This is a temporary solution.
05/18/06   dng     Changed the AHB high for M6275 target.
05/10/06   dng     Added SDIO WLAN autodetection support.
05/04/06   dng     Added byte mode support for WLAN SDIO.
04/25/06   dng     Reduce the SD clock to 9.6 for M6800.  Will need to 
                   change the clock back to 19.2 after sleep added the
                   vote against sleep support for the sdcc driver.
04/17/06   dng     Aded #ifdef FEATURE_SDCC_FIX_FOR_EXTRA_10MA_ON_TCXO
04/04/06   dng     Changed the sequence of clear the timeout signal
                   to avoid the signal conflict, in the module 
                   sdcc_dma_processing.
03/31/06   dng     Added more time delay in sdcc_find_card function to
                   detect some really slow sd cards.
03/28/06   dng     Fixed T_MSMS6800 typo error.
03/14/06   dng     Changed the WLAN Clock on M6800 to 9.6 MHZ.
03/01/06   dng     Added Atheros WLAN card support.
02/28/06   dng     Moved the vote against sleep on M6800 and M6550 targets
                   to process dma only.  This will help the battery life.
02/21/06   dng     Added Vote against sleep when sdcc_init got called..
                   And turns it off in sdcc_close module.
01/27/06   dng     Turned sleep off when wait for the DMA data to be completed.
01/18/06   dng     Fixed the WLAN clock to 9.6MHZ configure for 6275 target.
01/16/06   dng     Changed the WLAN clock to 9.6MHZ for 6275 target.
11/22/05   hwu     Changed to process dma completion and ignore timeout
                   if received both signals. This helps on NOR builds where
                   other higher priority tasks take too long and time us out 
                   after we receive dma completion.
11/11/05   hwu     Ported for 6275.
11/10/05   hwu     Fixed lint error complains.
11/03/05   hwu     Changed to return status code for internal functions.
10/07/05   sch     Changed delay between retries from 10 msec to 15 msec
08/28/05   hwu     General cleanup. Seperate get response from polling status.
08/08/05   hwu     Changed to define the timer when seting up data xfer. 
07/12/05   hwu     Added support for FFA.
06/07/05   hwu     Changed to use a timer for DMA timed out.
06/01/05   hwu     Added sdcc_disable_int().
05/26/05   hwu     Merged the changes for L4Linux.
05/09/05   hwu     -Added sdcc_enable_int(), sdcc_isr()
                   -merged changes for dual clock regime.  
04/14/05   hwu     Changed to use sdcc_setup_data_xfer.
                   Moved block_mode and enable_dma to sdcc_data_type.
02/15/05   hwu     change sdcc_find_card to look for SDIO card first. 
                   (only if FEATURE_WLAN is defined)
10/29/04   hwu     new release for MSM6550 v1.2 chipset.
07/14/04   hwu     Changed the way sdcc_process_data.
06/27/04   hwu     Changed to time the DMA status polling.
                   Fixed the problem where the data buff address may not be 
                   word aligned. 
06/23/04   hwu     Changed to use 19.2MHz for SD, 9.6MHz for MMC.(HW issue.)
                   Limited DMA usage to read only (HW bug.)
                   Changed the way to write to fifo.
                   Added sdcc_check_card_ready().
06/15/04   hwu     Initial version.
===========================================================================*/

 
#include "target.h"

#ifdef FEATURE_SDCC_VOTE_AGAINST_SLEEP
#include "sleep.h"
#endif

#if defined (T_MSM6275) || defined (T_MSM6280)
#error code not present
#else
#include "tlmm.h"
#endif

#include "pm.h"
#include CLKRGM_H

#include "sdcc_api.h"
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_sd_util.h"
#include "assert.h"
#include "armmmu.h"
#include "sdcc_bsp.h"
#include "timetick.h"

/* Maximum wait before timing out for data availability
 in FIFO during read operation. */
#define SDCC_FIFO_READ_TIMEOUT_MS            500

/* Current MCLK frequency (in MHz)*/
#ifndef FEATURE_6275_COMPILE
extern dword           clk_mclk_freq_mhz;
#else
extern dword           clk_hclk_freq_khz;
#endif
                                                                                
/* sdcc_pdata is a global structure variable. */
/* It is initialized by sdcc_init() and used by others. */
sdcc_data_type sdcc_pdata;



#ifdef FEATURE_SDCC_VOTE_AGAINST_SLEEP
static sleep_okts_handle sdcc_sleep_okts_handle;
static boolean sleep_handle_register=FALSE;
#endif

/* Use for the generic signal */
static uint32 sdcc_sigs =0;

extern unsigned int sdio_card;

static boolean sdcc_crit_sec_initialized;
static rex_crit_sect_type sdcc_crit_sect;


/*lint -save -e641 Suppress 'Converting enum to int' warning */
/*****************************************************************************/
/*                             PRIVATE FUNCTIONS                             */
/*****************************************************************************/
static void sdcc_get_hc_memory_info(uint32 block_len,
                                    uint32 c_size);
static void sdcc_get_std_memory_info(uint32 block_len, 
                                     uint8 c_size_mult,
                                     uint32 c_size);
static void sdcc_decode_std_csd(const uint32 *data, sdcc_csd_type *csd);
static void sdcc_decode_hc_csd(const uint32 *data, sdcc_csd_type *csd);

/******************************************************************************
* Name: sdcc_timer_cb
*
* Description:
*    Callback function called when sdcc_timer times out. This function
*    sets the timeout flag
*
* Arguments:
*    sdcc_pdata_cb         [IN] : sdcc data pointer
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_timer_cb(unsigned long sdcc_pdata_cb)
{
   sdcc_data_type *sdcc_pdata = (sdcc_data_type *)sdcc_pdata_cb;
   sdcc_sigs |= SDCC_TIMER_TIMEOUT_SIG_VAL;
   (void) rex_set_sigs(sdcc_pdata->sdcc_tcb, SDCC_RW_COMPLETE_SIG);
} /* sdcc_timer_cb */

/******************************************************************************
* Name:sdcc_set_tcxo_clk
*
* Description: When set_status is TRUE
*         1. Setup the TCB for rex signal
*         2. Vote to keep clock high during the transfer
*         3. Vote not to sleep during the transfer
*         When set_status is FALSE, vote for sleep and clk low.
* Arguments:
*         boolean set_status
* Returns:
*    None
*
******************************************************************************/
void sdcc_set_tcxo_clk(boolean set_status)
{

   static boolean sdcc_set_tcxo_on = FALSE;

   if((TRUE == set_status) && (FALSE == sdcc_set_tcxo_on)) 
   {
      sdcc_set_tcxo_on = TRUE;

      /* Keep bus speed high during the transfer */
#if defined (T_MSM6275) || defined (T_MSM6280) 
#error code not present
#endif 

#if defined (T_MSM6550) || defined (T_MSM6800) 
      clk_regime_set_sdio_mode (TRUE);
#endif
    
#ifdef FEATURE_SDCC_VOTE_AGAINST_SLEEP
      sleep_negate_okts(sdcc_sleep_okts_handle);
      /* Set the clock to high for M6550 & M6800 targets */
      clk_regime_set_sdio_mode (TRUE);
#endif
   }
   else if(set_status == FALSE)
   {
      sdcc_set_tcxo_on = FALSE;
      /* reset the clock to low for M6275 & M6280 targets */
#if defined (T_MSM6275) || defined (T_MSM6280) 
#error code not present
#endif 
   
#ifdef FEATURE_SDCC_VOTE_AGAINST_SLEEP
      clk_regime_set_sdio_mode (FALSE);
      sleep_assert_okts(sdcc_sleep_okts_handle);
#endif
   }
}/* sdcc_set_tcxo_clk */

/******************************************************************************
* Name: sdcc_blk_in_bits
*
* Description:
*    This function is to calculate the block size in terms of bits.
*
* Returns:
*    Returns the bits.
*
* Note:
******************************************************************************/
uint32
sdcc_blk_in_bits(uint32 size)
{
  uint32 blk_in_bits = 0;
  uint32 block_size  = size;

  /*-----------------------------------------------------------------------*/

  while(block_size)
  {
    blk_in_bits++;
    block_size >>= 1;
  }

  if ( size > 1 )
  {
     --blk_in_bits;
  }

  return(blk_in_bits);

}/* sdcc_blk_in_bits */

/******************************************************************************
* Name: sdcc_cmd_send_verify
*
* Description:
*    This function is to make sure the command is sent out and then
*    clear the corresponding bit in the status.
*
* Returns:
*    Returns error code.
*
* Note:
*    For slow clock(400KHz), this is needed.
******************************************************************************/
SDCC_STATUS
sdcc_cmd_send_verify( void )
{
  uint32 t      = 0;
  uint32 status = 0;

  /*----------------------------------------------------------------------*/
    
  while (t++ < 800)
  {
    status = HWIO_IN(MCI_STATUS);

    if(status & (HWIO_FMSK(MCI_STATUS, CMD_SENT)))
    {
      break;
    }

    if(sdio_card == SDIO_ATHEROS_MANFID)
    {
      sdcc_udelay(20);
    }
    else
    {
      sdcc_udelay(10);
    }

  }/* while */

  if(t < 800)
  {
    SDCC_CLR_STATUS(HWIO_MCI_CLEAR_CMD_SENT_CLR_BMSK);
    
    while (t++ < 800)
    {
      /* make sure cmd_sent is cleared */
      status = HWIO_IN(MCI_STATUS);    
      if( !(status & HWIO_FMSK(MCI_STATUS, CMD_SENT)))
          return SDCC_NO_ERROR;
          
      if(sdio_card == SDIO_ATHEROS_MANFID)
      {
        sdcc_udelay(20);
      }
      else
      {
        sdcc_udelay(10);
      }


    }/* while */

  }
    
  return SDCC_ERR_CMD_SENT;

}/* sdcc_cmd_send_verify */

/******************************************************************************
* Name: sdcc_config_dma
*
* Description:
*    This function is to configure SDCC controller for DMA transfer.
*
* Note:
*    Use the defaults for now.
******************************************************************************/
void
sdcc_config_dma( void )
{
  HWIO_OUTM(MCI_DMA_CONFIG,
            HWIO_FMSK(MCI_DMA_CONFIG, ENDIAN_CHANGE) |
            HWIO_FMSK(MCI_DMA_CONFIG, ENDIAN_CHANGE),
            0x03);
    
    /* Don't start the DMA engine here
       HWIO_OUTM(MCI_DATA_CTL,
                 HWIO_FMSK(MCI_DATA_CTL, DMA_ENABLE),
                 MCI_DATA_DMA_ENABLED); */    
}


/******************************************************************************
* Name: sdcc_config_clk
*
* Description:
*    This function is to configure the SDCC controller clock depending
*    on current mode.
*
******************************************************************************/
void
sdcc_config_clk
(
  SDCC_CARD_MODE   mode,
  SDCC_CARD_TYPE   card_type
)
{
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
  uint32 clk=0;
#endif
  /*-----------------------------------------------------------------------*/

    /* turn on the HCLK and MCLK */
  if( SDCC_INIT_MODE == mode )
  {
    CLK_REGIME_ENABLE(CLK_RGM_SDCC_HCLK_M);
    CLK_REGIME_ENABLE(CLK_RGM_SDCC_MCLK_M);
  }

#if defined (FEATURE_SDCC_M6550_CLK_CONFIG) || defined (FEATURE_SDCC_CLK_CONFIG)

  /* lower the clock to < 400KHz for card identification */
  if( SDCC_IDENTIFICATION_MODE == mode )
  {
    clk_regime_set_sdcc_clk_frequency(CLKRGM_SDCC_400KHZ);
  }
    
  if( SDCC_DATA_TRANSFER_MODE == mode )
  {

    /* For SDIO card, try different clock on different target */ 
    if( SDCC_CARD_SDIO == card_type )
    {
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
     switch(sdcc_pdata.sdcc_wlan_params.clk)
     {
     case SDCC_CLK_400KHZ : clk = CLKRGM_SDCC_400KHZ;
         break;
     case SDCC_CLK_4P8MHZ : clk = CLKRGM_SDCC_4P8MHZ;
         break;
     case SDCC_CLK_9P6MHZ : clk = CLKRGM_SDCC_9P6MHZ;
         break;
     case SDCC_CLK_19P2MHZ : clk = CLKRGM_SDCC_19P2MHZ;
         break;
     default:
         break;
     }
     clk_regime_set_sdcc_clk_frequency(clk);
#else
#if defined (T_MSM6800) || defined (FEATURE_SDCC_TURN_ON_PWR_SC2X)
     if(sdio_card == SDIO_ATHEROS_MANFID)
     {
         clk_regime_set_sdcc_clk_frequency(CLKRGM_SDCC_19P2MHZ);		  
	 }
	 else
	 {
		 clk_regime_set_sdcc_clk_frequency(CLKRGM_SDCC_9P6MHZ);
	 }
#else
     clk_regime_set_sdcc_clk_frequency(CLKRGM_SDCC_19P2MHZ);
#endif //T_M6800 or SC2x
#endif //FEATURE_SDCC_WLAN_CONFIG_API
    }
    else
    {
       /* Set SD/MMC card to TCXO 19.2 MHZ clock */
       clk_regime_set_sdcc_clk_frequency(CLKRGM_SDCC_19P2MHZ);
    }
  }

#else
    
  /* lower the clock to < 400KHz for card identification */
  if( SDCC_IDENTIFICATION_MODE == mode )
  {


    CLK_REGIME_DISABLE(CLK_RGM_SDCC_MCLK_M);

    /* 400KHz */
    HWIO_OUT(SDCC_MCLK_MD, 0x0001FFCF);
    HWIO_OUT(SDCC_MCLK_NS, 0xFFD00100);

    /* 144KHz */
    //HWIO_OUT(SDCC_MCLK_MD, 0x0003FF9B);
    //HWIO_OUT(SDCC_MCLK_NS, 0xFF9E0130);

    CLK_REGIME_ENABLE(CLK_RGM_SDCC_MCLK_M);

  }
    
  if( SDCC_DATA_TRANSFER_MODE == mode )
  {


    CLK_REGIME_DISABLE(CLK_RGM_SDCC_MCLK_M);
        
    /* For SDIO card, try different clock on different target */ 
    if( SDCC_CARD_SDIO == card_type )
    {
      HWIO_OUT(SDCC_MCLK_MD, 0x0);
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
      switch(sdcc_pdata.sdcc_wlan_params.clk)
      {
      case SDCC_CLK_9P6MHZ : clk = 0x10;
          break;
      case SDCC_CLK_19P2MHZ : clk = 0x00;
          break;
      default: clk = 0x00;
          break;
      }
     HWIO_OUT(SDCC_MCLK_NS, clk);
#else
#if defined (T_MSM6275) || defined (T_MSM6800)

      if(sdio_card == SDIO_ATHEROS_MANFID)
      {
        HWIO_OUT(SDCC_MCLK_NS, 0x00);
      }
      else
      {
        /* Set TCXO to 9.6 MHZ for M6275 & M6280 targets PHILLIP card only*/
        HWIO_OUT(SDCC_MCLK_NS, 0x10);
      }
#else
      HWIO_OUT(SDCC_MCLK_NS, 0x00);
      
#endif
#endif//FEATURE_SDCC_WLAN_CONFIG_API
    }
    else
    {
      /* Set SD/MMC card to TCXO 19.2 MHZ clock */
      HWIO_OUT(SDCC_MCLK_MD, 0x0);
      HWIO_OUT(SDCC_MCLK_NS, 0x0);
    }

    CLK_REGIME_ENABLE(CLK_RGM_SDCC_MCLK_M);
  }

#endif

#ifdef  FEATURE_SDCC_VOTE_AGAINST_SLEEP
  /* Get the sleep handle for to vote against sleep */
  if(FALSE == sleep_handle_register)
  { 
    sdcc_sleep_okts_handle = sleep_register("SDCC_SDIO", TRUE);
    sleep_handle_register = TRUE;

#if defined FEATURE_SDCC_TURN_ON_PWR_SC2X
	sleep_set_hclk_restriction (sdcc_sleep_okts_handle, CLKRGM_HCLK_NO_SWITCH);
#endif 


  }
#endif

}/* sdcc_config_clk */

/******************************************************************************
* Name: sdcc_config_rca
*
* Description:
*    This function is to set/get the RCA from the card. 
*
******************************************************************************/
SDCC_STATUS
sdcc_config_rca( void )
{
  sdcc_cmd_type    sdcc_cmd;
  SDCC_STATUS      rc = SDCC_ERR_UNKNOWN;

  /*---------------------------------------------------------------------*/

  sdcc_cmd.cmd       = SD_CMD3_SET_RELATIVE_ADDR;
  sdcc_cmd.resp_type = SDCC_RESP_SHORT;
  sdcc_cmd.prog_scan = 0;
  
  /* CMD3: assign/get relative card address */
  if( SDCC_CARD_MMC == sdcc_pdata.card_type ||
      SDCC_CARD_MMCHC == sdcc_pdata.card_type )
  {
    /* assign RCA to the card */
    sdcc_pdata.rca[sdcc_pdata.curr_sd_drv] = 2; 
    sdcc_cmd.cmd_arg   = (sdcc_pdata.rca[sdcc_pdata.curr_sd_drv]) << 16;
      
    rc = sdcc_command(&sdcc_cmd);
  }
  else
  {
    sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
    rc = sdcc_command(&sdcc_cmd);

    /* get RCA from the card */
    sdcc_pdata.rca[sdcc_pdata.curr_sd_drv] = sdcc_cmd.resp[0] >> 16;
  }

  return rc;

}/* sdcc_config_rca */


/******************************************************************************
* Name: sdcc_get_resp
*
* Description:
*    This function reads out  the command response. 
*
* Arguments:
*       sdcc_cmd   pointer to command data structure
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_get_resp(sdcc_cmd_type *sdcc_cmd)
{
  uint32        count  = 0;
  uint32        i;
  uint32       *resp = NULL;
  SDCC_STATUS   rc   = SDCC_NO_ERROR;

  /*---------------------------------------------------------------------*/
  
  /* loading command responses */    
  if (sdcc_cmd->resp_type)
  {
    count = ( SDCC_RESP_LONG == sdcc_cmd->resp_type ) ? 4 : 1;
    resp  = sdcc_cmd->resp;
    
    for (i = 0; i < count ; i++ )
    {
      *resp = HWIO_INI(MCI_RESPn, i);
      resp++;
    }

    /* check the R5 response flags */ 
    if (( SD_CMD52_IO_RW_DIRECT   == sdcc_cmd->cmd ) ||
        ( SD_CMD53_IO_RW_EXTENDED == sdcc_cmd->cmd ))
    {
      if(sdcc_cmd->resp[0] & 0xCF00)
      {
        rc = SDCC_ERR_SDIO_R5_RESP;
        DPRINTF(("R5 flag(0x%x) error.\n", sdcc_cmd->resp[0]));
      }
    }
  }

  sdcc_pdata.errno = rc;
  
  return rc;

}/* sdcc_get_resp */


/******************************************************************************
* Name: sdcc_poll_status
*
* Description:
*    This function is to poll the SDCC status.
*
* Arguments:
*       sdcc_cmd   pointer to command data structure
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_poll_status(sdcc_cmd_type *sdcc_cmd)
{
  uint32        i      = 0;
  uint32        status = 0;
  SDCC_STATUS   rc     = SDCC_ERR_UNKNOWN;
   
    
  
  /* polling for status */
  while (i++ < SDCC_STATUS_POLL_MAX+200 ) 
  {
    status = HWIO_IN(MCI_STATUS);
    
    /* response comes back */
    if(status & (HWIO_FMSK(MCI_STATUS, CMD_RESPONSE_END)))
    {
        SDCC_CLR_STATUS(HWIO_MCI_CLEAR_CMD_RESP_END_CLT_BMSK);
        rc = SDCC_NO_ERROR;

        break;
    }

    /* timedout on response */
    if (status & (HWIO_FMSK(MCI_STATUS, CMD_TIMEOUT)))
    {
        SDCC_CLR_STATUS(HWIO_MCI_CLEAR_CMD_TIMOUT_CLR_BMSK);
        rc = SDCC_ERR_CMD_TIMEOUT;

        break;
    }
    
    /* check CRC error */
    if (status & (HWIO_FMSK(MCI_STATUS, CMD_CRC_FAIL)))
    {
        /* the following cmd response doesn't have CRC. */     
        if(( SD_ACMD41_SD_APP_OP_COND == sdcc_cmd->cmd ) ||
           ( SD_CMD1_SEND_OP_COND     == sdcc_cmd->cmd ) ||
           ( SD_CMD5_IO_SEND_OP_COND  == sdcc_cmd->cmd ))
        {
            rc = SDCC_NO_ERROR;
        }
        else
        {
            rc = SDCC_ERR_CMD_CRC_FAIL;
        }
        
        SDCC_CLR_STATUS(HWIO_MCI_CLEAR_CMD_CRC_FAIL_CLR_BMSK);
        break;
    }

  }

  sdcc_cmd->status = status;

       

  if( rc != SDCC_NO_ERROR)
  {
    TPRINTF(SDCC_MSG_LEVEL,("polling status error"));
  }

  sdcc_pdata.errno = rc;

  return rc;

}/* sdcc_poll_status */


/****************************************************************************
* Name: sdcc_poll_dma
*
* Description:
*  This function is to process DMA data transfer by polling.
*
* Arguments:
*
* Returns:
*  Returns error code.
*
* Note:
*  Not used.
****************************************************************************/
SDCC_STATUS
sdcc_poll_dma( void )
{
  SDCC_STATUS      rc = SDCC_ERR_UNKNOWN;
  volatile uint32  status;
  int              t = 0;

  /*----------------------------------------------------------------------*/
  
  status = HWIO_IN(MCI_STATUS);

  while( t++ < SDCC_DMA_DONE_MAX)
  {
    if(status & ( HWIO_FMSK(MCI_STATUS, DMA_DONE) ))
    {
        rc = (status & SDCC_DMA_ERROR) ? SDCC_ERR_DMA : SDCC_NO_ERROR;
        break;
    }
    
    sdcc_udelay(5);
    status = HWIO_IN(MCI_STATUS);
  }
      
  if( t == (SDCC_DMA_DONE_MAX + 1))
  {
    rc = SDCC_ERR_DMA;
    DPRINTF(("polling DMA done timed out\n"));
  }
  
  /* need sometime to let DMA data settled */
  sdcc_mdelay(1);

  sdcc_pdata.errno = rc;
  
  return rc;

}/* sdcc_poll_dma */

/******************************************************************************
* Name: sdcc_setup_data_xfer
*
* Description:
*    This function sets up the controller prior to data transfer. 
*
* Arguments:
*       direction  data read or data write
*       n_sector   number of sectors to be read
*
* Returns:
*       YES if sucessful
*       NO if failure
*
******************************************************************************/
uint32
sdcc_setup_data_xfer
(
  uint32    direction,
  uint16    units
)
{
  uint32    data_length = 0; 
  uint32    timeout     = 0;
  uint16    blksz       = 0;

  /*------------------------------------------------------------------------*/
  
  /* set data timeout */
  timeout = (SDCC_DATA_READ == direction) ? MCI_READ_TIMEOUT :
                                            MCI_WRITE_TIMEOUT;
#ifdef FEATURE_6275_COMPILE
    HWIO_OUT(MCI_DATA_TIMER, clk_hclk_freq_khz * timeout );
#else    
    HWIO_OUT(MCI_DATA_TIMER, clk_mclk_freq_mhz * timeout * 1000 );
#endif

    /* Note:
       1. data size in byte_mode should be the power of 2
       2. DMA doesn't work for small data size
       3. units: number of bytes in byte mode
                 number of blocks in block mode
    */
  if(SDCC_SDIO_BYTE_MODE == sdcc_pdata.block_mode ||
     SDCC_MEM_BYTE_MODE == sdcc_pdata.block_mode)
  {
    sdcc_pdata.enable_dma = FALSE;
        
    /* set data length */
    data_length = units;
    HWIO_OUT(MCI_DATA_LENGTH, data_length);
  }
  else
  {
    sdcc_pdata.enable_dma = TRUE;
    
    if(SDCC_CARD_SDIO == sdcc_pdata.card_type)
    {
        blksz = sdcc_pdata.io.fn_blksz;
    }
    else
    {
        blksz = sdcc_pdata.mem.block_len;
        sdcc_pdata.mem.multi_block = (units > 1) ? TRUE : FALSE;
    }
    
    /* set data length */
    data_length = units * blksz;
    HWIO_OUT(MCI_DATA_LENGTH, data_length);

// Targets such as ULC have data cache-enabled MMU. The data 
// cache is shared and might have stale data.
#ifdef FEATURE_SDCC_DCACHE_ENABLED
    mmu_dcache_flush_all();
#endif
  }
  if ( data_length )
  {
#ifdef FEATURE_SDCC_CLKREGIM_VOTE_HIGH_CLK
  /* H Clk must be kept constant during DMA transfer */
     clk_regime_register_for_cpu_resource ( CLKRGM_CPU_CLIENT_SDCC );
#endif
     /* only set TCXO to high if data_length is non-zero */
     sdcc_set_tcxo_clk(TRUE);
     
     /************************************************************************
      *
      *       Setup the TCB for rex signal
      *
      ************************************************************************/
    
     sdcc_pdata.sdcc_tcb = rex_self ();

     if( NULL == sdcc_pdata.sdcc_tcb )
        ASSERT(0);
  }
  return data_length;

}/* sdcc_setup_data_xfer */ 

/****************************************************************************
* Name: sdcc_command
*
* Desscription:
*  This function is to send the command and poll the status. 
*
* Arguments:
*  sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*  Returns error code.
*
****************************************************************************/
SDCC_STATUS
sdcc_command(sdcc_cmd_type   *sdcc_cmd)
{
  SDCC_STATUS   rc = SDCC_NO_ERROR;
  

  /*---------------------------------------------------------------------*/

  /************************************************************************
   *
   *     Return error if the host_state is in idle mode.
   *     Which mean the power of the sdcc has not turned on Yet.
   *
   ***********************************************************************/

  if(sdcc_pdata.host_state ==  SDCC_HOST_IDLE)
  {
    rc = SDCC_ERR_CMD_TIMEOUT;
    return rc;
  }

 

  /************************************************************************
   *
   *      Send the command
   *
   ***********************************************************************/

  sdcc_send_cmd(sdcc_cmd);


  /* CMD0 takes 74 clock clcles, need to verify that we have
       finished sending command */
  if( SD_CMD0_GO_IDLE_STATE == sdcc_cmd->cmd )
  {
    rc =  sdcc_cmd_send_verify();
    return rc;
  }


  /************************************************************************
   *
   *     Wait for the command done ISR or poll the status
   *
   ***********************************************************************/

  if ( sdcc_cmd->resp_type )
  {
    rc = sdcc_poll_status(sdcc_cmd);

  /************************************************************************
   *
   *      Get the command response
   *
   ***********************************************************************/
    if( SDCC_NO_ERROR == rc )
    {
      rc = sdcc_get_resp(sdcc_cmd);
    }
  }

  /***********************************************************************
   *
   *     wait for the device is done
   *
   ***********************************************************************/
  if(sdcc_cmd->prog_scan) 
  {
    rc = sdcc_process_interrupts(sdcc_cmd);
  }

  return rc;

}/* sdcc_command */


/****************************************************************************
* Name: sdcc_send_cmd
*
* Desscription:
*  This function is to send out the command. 
*
* Arguments:
*  sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*     YES if sucessful
*     NO if failure
*
****************************************************************************/
void sdcc_send_cmd(sdcc_cmd_type *sdcc_cmd)
{
  uint32   command  = 0;
 
  
  /*--------------------------------------------------------------------*/
  
  /* clear any status flags 
     Note: For slower clock, need to wait to make sure the flags
           are cleared. */
  while( HWIO_IN(MCI_STATUS) & (MCI_STATUS_STATIC_MASK))
  {
    SDCC_CLR_STATUS(MCI_STATUS_STATIC_MASK);
  }
      

  /* set the command */
  command = ((uint32)sdcc_cmd->cmd) | MCI_CMD_ENABLED;
  
  if(sdcc_cmd->resp_type)
  {
    command |= MCI_CMD_RESPONSE;
    
    if( SDCC_RESP_LONG == sdcc_cmd->resp_type )
    {
      command |= MCI_CMD_LONG_RESP;
    }
  }

  if(sdcc_cmd->prog_scan)
  {
    command |= MCI_CMD_PROG_ENABLED;
  }

 
  HWIO_OUT(MCI_ARGUMENT, sdcc_cmd->cmd_arg);

  HWIO_OUT(MCI_CMD, command);


}/* sdcc_send_cmd */


/******************************************************************************
* Name: sdcc_send_status
*
* Description:
*    This function is to let the device(currently selected device) send
*    the device status. 
*
* Arguments:
*
* Returns:
*    Returns the current device status.
*
******************************************************************************/
SDCC_CARD_STATUS sdcc_send_status( void )
{

  sdcc_cmd_type        sdcc_cmd;
  SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;
  SDCC_STATUS          rc          = SDCC_ERR_UNKNOWN;
  uint32               temp        =0;

  /*---------------------------------------------------------------------*/
  
  sdcc_cmd.cmd       = SD_CMD13_SEND_STATUS;
  sdcc_cmd.resp_type = SDCC_RESP_SHORT;
  sdcc_cmd.prog_scan = 0;
  sdcc_cmd.cmd_arg   = ((uint32)sdcc_pdata.rca[sdcc_pdata.curr_sd_drv]) << 16;
  
  rc = sdcc_command(&sdcc_cmd);
 
  if(!rc)    
  {
    temp =  sdcc_cmd.resp[0] >> SDCC_CARD_STATUS_SHFT;
    card_status = (SDCC_CARD_STATUS) (temp & SDCC_CARD_STATUS_BMSK);
  }

  return card_status;

}/* sdcc_send_status */

/****************************************************************************
* Name: sdcc_wait_prog_done
*
* Description:
*  This function is to poll the status to make sure the device is not
*  busy. CMD12(stop) and CMD38(erase) need to call this to make sure
*  the device programming has finished.
*
* Returns:
*  Returns error code.
*
****************************************************************************/
SDCC_STATUS sdcc_wait_prog_done( void )
{
  uint32               t      = 0;
  volatile uint32      status = 0;
  SDCC_STATUS          rc          = SDCC_ERR_PROG_DONE;
  SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;

  /*--------------------------------------------------------------------*/
  
  while (t++ < SDCC_PROG_DONE_MAX  )
  {
    status = HWIO_IN(MCI_STATUS);
    
    if (status & (HWIO_FMSK(MCI_STATUS, PROG_DONE)))
    {
        SDCC_CLR_STATUS(HWIO_MCI_CLEAR_PROG_DONE_CLR_BMSK);
        rc = SDCC_NO_ERROR;
        

        break;
    }

    sdcc_udelay(25);
  }
    
  /* CMD13: if no prog_done indication, give it one more
     chance by polling the card directly */
  if(SDCC_NO_ERROR != rc)
  {
    DPRINTF(("timeout waiting for prog done."));
    
    card_status = sdcc_send_status();
    DPRINTF(("card status = 0x%x.\n", card_status));
    
    if(( SDCC_CARD_TRAN == card_status ) || ( card_status == SDCC_CARD_RCV))
    {
      rc = SDCC_NO_ERROR; 
    } 
        
  }

  sdcc_pdata.status = status;
          
  return rc;

}/* sdcc_wait_prog_done */

/******************************************************************************
* Name: sdcc_find_card
*
* Description:
*    This function is to find out the type of the device.
*
* Arguments:
*
* Returns:
*    Returns error code.
*    
******************************************************************************/
SDCC_CARD_TYPE
sdcc_find_card( void )
{
   sdcc_cmd_type  sdcc_cmd;
   uint32         i;
   SDCC_STATUS    rc        = SDCC_ERR_UNKNOWN;
   SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;
   uint32         cmd41_arg = SDCC_NEGOTIATE_OCR;

   /* Try to detect SDIO device first */
   card_type = sdcc_find_sdio_card();

   /* Detected SDIO card */
   if ( card_type != SDCC_CARD_UNKNOWN )
   {
      /* If SDIO card is detected then return the card type, but
      *  first set the card type to UNKNOWN so the SDIO can initialize the
      *  card properly 
      */
      return card_type;
   }

   /* power ramp up time: 1ms */
   sdcc_mdelay(1);

   /* lower the clock to < 400KHz for card identification */
   sdcc_config_clk(SDCC_IDENTIFICATION_MODE, SDCC_CARD_UNKNOWN);

   /* CMD0: reset card first */
   sdcc_cmd.cmd       = SD_CMD0_GO_IDLE_STATE;
   sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_NONE;
   sdcc_cmd.prog_scan = 0;

   (void)sdcc_command(&sdcc_cmd);

   /* card may not be in sync, make sure CMD8 response is received */
   for ( i = 0; i < SDCC_CMD8_RETRIES; i++ )
   {
      /* CMD8: verify SDHC card interface operating condition */
      sdcc_cmd.cmd       = SD_CMD8_SEND_IF_COND;
      sdcc_cmd.cmd_arg   = SDCC_HC_VOLT_SUPPLIED;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      rc = sdcc_command(&sdcc_cmd);

      if ( SDCC_NO_ERROR == rc )
      {
         /* If the card can operate on the supplied voltage, the response */
         /* echos back the supply voltage and the check pattern. */
         if ( sdcc_cmd.resp[0] != SDCC_HC_VOLT_SUPPLIED )
         {
            /* non-compatible voltage range or check pattern is incorrect */
            /* unusable card */
            return card_type;
         }

         /* Receiving of CMD8 expands the ACMD41 function.  Setting the */
         /* Host Capacity Support bit to 1 in ACMD41 argument indicates */
         /* the host supports High Capacity SD memory card. */
         cmd41_arg |= SDCC_HC_HCS;
         break;
      }
      sdcc_mdelay(1);
   }
    
   for ( i = 0; i < SDCC_SD_DETECT_MAX_RETRIES; i++ )
   {
      /* CMD55: the resp will have APP_CMD set, indicating expect
         ACMD from now on */
      sdcc_cmd.cmd       = SD_CMD55_APP_CMD;
      sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;

      rc = sdcc_command(&sdcc_cmd);

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for SD card detection if there is issue in */
         /* sending the command to the card */
         break;
      }

      /* ACMD41: sent voltage to be used */
      sdcc_cmd.cmd       = SD_ACMD41_SD_APP_OP_COND;
      sdcc_cmd.cmd_arg   = cmd41_arg;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      
      rc = sdcc_command(&sdcc_cmd);

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for SD card detection if there is issue in */
         /* sending the command to the card */
         break;
      }
      else if ( sdcc_cmd.resp[0] & SDCC_DEVICE_RDY )
      {
         if ( SDCC_CARD_SDIO == card_type )
         {
            card_type = SDCC_CARD_COMB;
         }
         else if ( sdcc_cmd.resp[0] & SDCC_HC_CCS )
         {
            /* The response of ACMD41 includes Card Capacity Status field */
            /* information. If CCS bit is set, the card is a High Capacity */
            /* SD memory card. */
            card_type = SDCC_CARD_SDHC;
            DPRINTF(("Found SDHC card after %dth retry\n", i));
         }
         else
         {
            card_type = SDCC_CARD_SD;
            DPRINTF(("Found SD card after %dth retry\n", i));
         }
      
         return card_type;
      }

      /* Refer to section 3.6 of SD Specifications Part A2 SD Host */
      /* Controller Standard Specification Version 2.00 for the */
      /* SDCC_SD_DETECT_BUSY_RETRY_INTERVAL */
      sdcc_mdelay(SDCC_SD_DETECT_BUSY_RETRY_INTERVAL);
   }

   /* CMD0: reset card first */
   sdcc_cmd.cmd       = SD_CMD0_GO_IDLE_STATE;
   sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_NONE;
   sdcc_cmd.prog_scan = 0;

   (void)sdcc_command(&sdcc_cmd);

   /* continue on to look for MMC card */
   for ( i = 0; i < SDCC_MMC_DETECT_MAX_RETRIES; i++ )
   {
      /* CMD1: sent voltage to be used */
      sdcc_cmd.cmd       = SD_CMD1_SEND_OP_COND;
      sdcc_cmd.cmd_arg   = SDCC_MMC_HCS_NEGOTIATE_OCR;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;

      rc = sdcc_command(&sdcc_cmd);

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for MMC card detection if there is issue in */
         /* sending the command to the card */
         break;
      }
      else if ( sdcc_cmd.resp[0] & SDCC_DEVICE_RDY )
      {
         /* access mode validation: */
         /* byte access mode represents <= 2GB card size */
         /* sector access mode represents > 2GB card size, however */
         /* this needs to be reconfirmed by reading SEC_COUNT */
         /* in EXT_CSD */
         /* >2GB: [30:29] = 1,0 */
         if ( ((sdcc_cmd.resp[0] >> 30) & 1) && 
               !((sdcc_cmd.resp[0] >> 29) & 1) )
         {
            card_type = SDCC_CARD_MMCHC;
            DPRINTF(("Found MMCHC card after %dth retry\n", i));
         }
         else
         {
            card_type = SDCC_CARD_MMC;
            DPRINTF(("Found MMC card after %dth retry\n", i));
         }
         return card_type;
      }

      sdcc_mdelay(SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL);
   }

   return card_type;
} /* sdcc_find_card */

/******************************************************************************
* Name: sdcc_process_interrupts
*
* Description:
*    This function is to process all the Interrupts from the sdcc controller.
*
* Arguments:
      sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_process_interrupts( sdcc_cmd_type *sdcc_cmd)
{
   rex_sigs_type      sigs;
   SDCC_STATUS        rc = SDCC_NO_ERROR;
   rex_timer_type    *sdcc_timer_ptr;
    
   (void)sdcc_cmd;
    
   sdcc_pdata.status   = 0;
   sdcc_timer_ptr      = &sdcc_pdata.sdcc_dma_timer;

   /*-----------------------------------------------------------------------*/
    
   sdcc_set_tcxo_clk(TRUE);

   /************************************************************************
    *
    *       Setup the TCB for rex signal
    *
    ************************************************************************/
    
   sdcc_pdata.sdcc_tcb = rex_self ();

   if( NULL == sdcc_pdata.sdcc_tcb )
      ASSERT(0);
    
   /* define a timer for DMA data transfer
    * Note: Ideally, we should create timer once, set/clr it as
    *       needed. However, we cannot do this now because the
    *       calling task changes in our case.
    */

   rex_def_timer_ex(sdcc_timer_ptr,
                    (rex_timer_cb_type)sdcc_timer_cb,
                    (unsigned long)&sdcc_pdata);

   /* 10000 msec */
   (void)rex_set_timer(sdcc_timer_ptr, 10000);

   /**************************************************************************
    *
    *            Wait for the incoming Rex signal
    *
    **************************************************************************/

   for (;;)
   {    
      sigs = rex_wait(  SDCC_RW_COMPLETE_SIG );

      if ( 0 != sdcc_sigs)
      {
         break;
      }
      else
      {
         /* Spurious Signal for SDCC */
         continue;
      }
   }
    

   /**************************************************************************
    *
    *            Clear The Rex signal
    *
    **************************************************************************/
    
   /* clear the timer regardless timed out */
   (void)rex_clr_timer(sdcc_timer_ptr);

   /* Clear the timeout signal */
   (void)rex_clr_sigs(sdcc_pdata.sdcc_tcb,
                       SDCC_RW_COMPLETE_SIG);

   /**************************************************************************
    *
    *    Check for the individual signal that got set from the sdcc_isr
    *
    **************************************************************************/

   /* process the dma completion and ignore the timeout if received both */
   if( (sdcc_sigs & SDCC_DMA_DONE_SIG_VAL) != 0 )
   {
      sdcc_sigs &= ~(SDCC_DMA_DONE_SIG_VAL |
                     SDCC_TIMER_TIMEOUT_SIG_VAL);

      if(sdcc_pdata.status & SDCC_DMA_ERROR)
      {
         DPRINTF(("DMA xfer error, status = 0x%x\n",
                  sdcc_pdata.status));
         rc = SDCC_ERR_DMA;
      }
   }

   /* Process the prog done signal */
   else if( (sdcc_sigs & SDCC_PROG_DONE_SIG_VAL) != 0 )
   {
      sdcc_sigs &= ~(SDCC_PROG_DONE_SIG_VAL |
                     SDCC_TIMER_TIMEOUT_SIG_VAL);
   }    
   else if( (sdcc_sigs & SDCC_TIMER_TIMEOUT_SIG_VAL) != 0 )
   {
      sdcc_sigs &= ~SDCC_TIMER_TIMEOUT_SIG_VAL;
      rc =  SDCC_ERR_DATA_TIMEOUT;
      DPRINTF(("DMA xfer timed out\n"));
   }
   sdcc_pdata.errno = rc;
   return rc;
}

/******************************************************************************
* Name: sdcc_enable_int
*
* Description:
*    This function is to enable the SDCC host interrupts.
*
* Arguments:
*    int_mask   the interrupts to be turned on
* Retuns:
*
* Notes:
*    None.
******************************************************************************/
void
sdcc_enable_int( uint32 int_mask )
{
  uint32  reg_value = 0;

  /*
  Since the SDIO interrupt is unpredictable (i.e., we dont know when to expect one), 
  it could happen when we are servicing another SDCC interrupt. We therefore need
  a separate interrupt source for SDIO to ensure we dont miss it.
  */
  if(MCI_INT_MASK_SDIO_INTR == int_mask)
  {
     reg_value = HWIO_MCI_INT_MASKn_INI(1);

     /* connect isr if we haven't yet */
     if(!reg_value)
     {
        tramp_set_isr(TRAMP_SDCC_1_ISR, sdio_isr);
     }
     reg_value |= int_mask;
     HWIO_MCI_INT_MASKn_OUTI(1, reg_value);
  }
  else
  {
     reg_value = HWIO_MCI_INT_MASKn_INI(0);
 
     /* connect isr if we haven't yet */
     if(!reg_value)
     {
        tramp_set_isr(TRAMP_SDCC_0_ISR, sdcc_isr);
     }
     reg_value |= int_mask;
     HWIO_MCI_INT_MASKn_OUTI(0, reg_value);
  }
}/* sdcc_enable_int */

/******************************************************************************
* Name: sdcc_disable_int
*
* Description:
*    This function is to disable the SDCC host interrupts.
*
* Arguments:
*   int_mask    the interrupts to be masked off
* Retuns:
*
* Notes:
*    None.
******************************************************************************/
void
sdcc_disable_int( uint32 int_mask )
{
   uint32  reg_value = 0;

   if(MCI_INT_MASK_SDIO_INTR == int_mask)
   {
      reg_value  = HWIO_MCI_INT_MASKn_INI(1);

      reg_value &= (~int_mask);

     /* disable all interrupts */
      HWIO_MCI_INT_MASKn_OUTI(1, reg_value);

      /* remove the isr handle */
      if( reg_value == 0 )
      {
         tramp_set_isr(TRAMP_SDCC_1_ISR, NULL);
      }
   }
   else
   {
      reg_value  = HWIO_MCI_INT_MASKn_INI(0);

      reg_value &= (~int_mask);
  
      /* disable all interrupts */
      HWIO_MCI_INT_MASKn_OUTI(0, reg_value);
  
      /* remove the isr handle */
      if( reg_value == 0 )
      {
         tramp_set_isr(TRAMP_SDCC_0_ISR, NULL);
      }
   }
}/* sdcc_disable_int */

/******************************************************************************
* Name: sdcc_isr
*
* Description:
*    This function is to process sdcc interrupts..
*
* Arguments:
* Retuns:
* Notes:
    This function is in interrupt context. 
******************************************************************************/
void
sdcc_isr(void)
{
  uint32 status;
     
  /*---------------------------------------------------------------------------*/
    
  status = HWIO_IN(MCI_STATUS);

  /* Check for the DMA done interrupt */
  if(status & HWIO_MCI_STATUS_DMA_DONE_BMSK)
  {
      sdcc_pdata.status = status;
      
      SDCC_CLR_STATUS(HWIO_MCI_CLEAR_DMA_DONE_CLR_BMSK);

      sdcc_sigs |= SDCC_DMA_DONE_SIG_VAL;

      /* suppress lint */
      ASSERT(sdcc_sigs);

      (void)rex_set_sigs(sdcc_pdata.sdcc_tcb,
                         SDCC_RW_COMPLETE_SIG);
  }


  /* Check for the prog done interrupt */
  if(status & HWIO_MCI_STATUS_PROG_DONE_BMSK)
  {
    sdcc_pdata.status = status;
    SDCC_CLR_STATUS(HWIO_MCI_CLEAR_PROG_DONE_CLR_BMSK);

    sdcc_sigs |=  SDCC_PROG_DONE_SIG_VAL;

    (void)rex_set_sigs(sdcc_pdata.sdcc_tcb,
                       SDCC_RW_COMPLETE_SIG);
  }

}/* sdcc_isr */

/******************************************************************************
* Name: sdio_isr
*
* Description:
*    This function processes the SDIO interrupt
*
* Arguments:
* Retuns:
* Notes:
    This function is in interrupt context. 
******************************************************************************/

void sdio_isr()
{
   uint32 status;
   status = HWIO_IN(MCI_STATUS);
   if(status & HWIO_MCI_STATUS_SDIO_INTR_BMSK)
   {
      if(sdcc_pdata.io.isr)
         (void)sdcc_pdata.io.isr((void*)sdcc_pdata.io.isr_param);
      SDCC_CLR_STATUS(HWIO_MCI_CLEAR_SDIO_INTR_CLR_BMSK);
   }
}


/******************************************************************************
* Name: sdcc_wait_ard_ready
*
* Description:
*    This function is to check the card busy status by calling "send
*    status" command.
*
* Arguments:
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_card_ready(void)
{
  uint32               t           = 0;
  SDCC_STATUS          rc          = SDCC_ERR_CARD_READY;
  SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;

  /*-----------------------------------------------------------*/
  
  while (t++ < SDCC_PROG_DONE_MAX )
  {    
      card_status = sdcc_send_status();
      if(( SDCC_CARD_TRAN == card_status ) || (card_status == SDCC_CARD_RCV))
      {
          rc = SDCC_NO_ERROR;
          break;
      }
      
      sdcc_udelay(100);
  }

  return rc;

}/* sdcc_wait_card_ready */

/******************************************************************************
* Name: sdcc_select_card
*
* Description:
*    This function is to select/deselect the specified card.
*
* Arguments:
*    select  select if TRUE, deselect if FALSE
*    rca     card RCA address.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_select_card
(
  uint16    rca,
  uint32    select
)
{
  sdcc_cmd_type    sdcc_cmd;
  SDCC_STATUS      rc = SDCC_ERR_UNKNOWN;

  /*----------------------------------------------------*/
    
  /* CMD7: select the card */
  sdcc_cmd.cmd_arg = (select) ? (rca << 16) : 0;
  
  sdcc_cmd.cmd       = SD_CMD7_SELECT_CARD;
  sdcc_cmd.resp_type = SDCC_RESP_SHORT;
  sdcc_cmd.prog_scan = 0;
  
  rc = sdcc_command(&sdcc_cmd);

  return rc;

}/* sdcc_select_card */

/******************************************************************************
* Name: sdcc_init_memory_device
*
* Description:
*    This function sends CMD2 to get the device ID and product related
*    information. It then configures the device Relative Card Address and
*    completes the card identification process.
*
* Returns:
*    Returns error code.
*
* Note:
*   Card is in Identification mode. 
*****************************************************************************/
SDCC_STATUS sdcc_init_memory_device(void)
{
  sdcc_cmd_type  sdcc_cmd;
  SDCC_STATUS    rc = SDCC_ERR_UNKNOWN;
  int            i;

  /*-------------------------------------------------------*/
  
  /* CMD2: request card ID */
  sdcc_cmd.cmd       = SD_CMD2_ALL_SEND_CID;
  sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
  sdcc_cmd.resp_type = SDCC_RESP_LONG;
  sdcc_cmd.prog_scan = 0;
  
  rc = sdcc_command(&sdcc_cmd);
  if(SDCC_NO_ERROR != rc)
      return rc;

  /* For SD: CID[103:64] --> PNM */
  /* For MMC: CID[103:56] --> PNM */
  sdcc_pdata.mem.pnm[0] = (uint8)(sdcc_cmd.resp[0]);
      
  for(i = 4; i > 0; i--)
  {
      sdcc_pdata.mem.pnm[i] = (sdcc_cmd.resp[1] & 0xFF);
      (sdcc_cmd.resp[1])  >>= 8;
  }

  if (SDCC_CARD_MMC == sdcc_pdata.card_type ||
      SDCC_CARD_MMCHC == sdcc_pdata.card_type)
  {
     sdcc_pdata.mem.pnm[5] = (sdcc_cmd.resp[2] >> 24);
     sdcc_pdata.mem.pnm[6] = '\0';
  }
  else
  {
    sdcc_pdata.mem.pnm[5] = '\0';
  }
  
  /* For SD: CID[55:24] --> PSN */
  /* For MMC: CID[47:16] --> PSN */
  if (SDCC_CARD_MMC == sdcc_pdata.card_type ||
      SDCC_CARD_MMCHC == sdcc_pdata.card_type)
  {
    sdcc_pdata.mem.psn = ((sdcc_cmd.resp[2] & 0x0000FFFF) << 16) |
                         (sdcc_cmd.resp[3] >> 16);
  }
  else
  {
    sdcc_pdata.mem.psn = ((sdcc_cmd.resp[2] & 0x00FFFFFF) << 8) |
                         (sdcc_cmd.resp[3] >> 24);
  }
  
  /* CMD3: configure card RCA */
  rc = sdcc_config_rca();
  
  return (rc);

}/* sdcc_init_memory_device */


/******************************************************************************
* Name: sdcc_get_memory_info.
*
* Description:
*    This function is to get the specific information related to the
*    memory device via CMD9 and/or CMD10 before it goes into data
*    transfer mode. 
*
* Arguments:
*
* Returns:
*    Returns error code.
*
* Note:
*    Card is in stand-by mode.
******************************************************************************/
SDCC_STATUS sdcc_get_memory_info(void)
{

  SDCC_STATUS      rc = SDCC_NO_ERROR;
  sdcc_cmd_type    sdcc_cmd;
  sdcc_csd_type    csd;
  SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;
  
  /* CMD13: make sure in STBY state */
  card_status = sdcc_send_status();
  if ( SDCC_CARD_STBY != card_status )
  {
      DPRINTF(("In invalid data TX state 0x%x\n",card_status));
      rc = SDCC_ERR_INVALID_TX_STATE;

      return rc;
  }
  
  /* CMD9: get card specific data */
  sdcc_cmd.cmd       = SD_CMD9_SEND_CSD; 
  sdcc_cmd.resp_type = SDCC_RESP_LONG;
  sdcc_cmd.prog_scan = 0;
  sdcc_cmd.cmd_arg   = (sdcc_pdata.rca[sdcc_pdata.curr_sd_drv]) << 16;

  rc = sdcc_command(&sdcc_cmd);

  if(SDCC_NO_ERROR == rc)
  {
    sdcc_decode_csd(sdcc_cmd.resp, &csd);

    switch (sdcc_pdata.card_type)
    {
    case SDCC_CARD_SD:
    case SDCC_CARD_SDHC:
       {
          if (csd.csd_structure == 1)
          {
             /* csd_structure = 1 means CSD Version 2.0 for */
             /* High Capacity SD memory card */
             sdcc_get_hc_memory_info(csd.read_bl_len, csd.c_size);
          }
          else
          {
             /* csd_structure = 0 means CSD Version 1.0 for */
             /* Standard Capacity SD memory card */
             sdcc_get_std_memory_info(csd.read_bl_len, csd.c_size_mult,
                                      csd.c_size);
          }
       }
       break;

    case SDCC_CARD_MMCHC:
       {
          /* defer to sdcc_config_mmc_modes_segment as EXT_CSD register */
          /* is read there.  SEC_COUNT must be used to calculate the high */
          /* density card size. */
       }
       break;

    case SDCC_CARD_MMC:
    default:
       {
          /* get the standard memory info anyway */
          sdcc_get_std_memory_info(csd.read_bl_len, csd.c_size_mult,
                                   csd.c_size);
       }
       break;
    }
  }

  return rc;

}/*  sdcc_get_memory_info */

/******************************************************************************
* Name: sdcc_config_memory_device.
*
* Description:
*    This function is to programm the card into data transfer mode. It
*    selects the card, sets up the bus width and block data transfer mode. 
*
* Arguments:
*
* Returns:
*    Returns error code.
*
* Note:
*    Card is in data transfer mode.
******************************************************************************/
SDCC_STATUS sdcc_config_memory_device(void)
{
  SDCC_STATUS      rc = SDCC_NO_ERROR;
  SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;
  sdcc_scr_type    scr;

  /*-------------------------------------------------------*/

  /* CMD7: select the card */
  rc = sdcc_select_card(sdcc_pdata.rca[sdcc_pdata.curr_sd_drv], TRUE);
  if(SDCC_NO_ERROR != rc)
      return rc;

  /* CMD16: set the data block length */
  sdcc_pdata.block_mode = SDCC_MEM_BLK_MODE;             
  rc = sdcc_set_blksz(SDCC_DEFAULT_BLK_LENGTH_SIZE);

  /* default device clk freq to regular speed */
  sdcc_pdata.mem.clk_freq = 0;

  /* program the bus width for MMC */
  if ( SDCC_CARD_MMC == sdcc_pdata.card_type ||
       SDCC_CARD_MMCHC == sdcc_pdata.card_type )
  {
     /* default MMC bus width to 1-bit mode */
     (void) sdcc_config_mmc_bus_width(0 /*don't care*/, SDCC_MMC_BUSWIDTH_1BIT);
  }
  else if ( SDCC_CARD_SD == sdcc_pdata.card_type ||
            SDCC_CARD_SDHC == sdcc_pdata.card_type )
  {
     /* default SD bus width to 1-bit mode */
     rc = sdcc_set_sd_bus_width(SDCC_SD_BUS_WIDTH_1BIT);
     if ( SDCC_NO_ERROR != rc )
     {
        DPRINTF(("Error %d in setting SD bus width to 1 bit\n", rc));
        return rc;
     }

     memset( &scr, 0, sizeof(scr) );

     /* read the SD configuration register (SCR) -- ACMD51 */
     rc = sdcc_get_scr( &scr );
     if ( SDCC_NO_ERROR != rc )
     {
        return rc;
     }

     if ( SDCC_SD_WIDE_BUS_WIDTH == scr.sd_bus_widths )
     {
        rc = sdcc_set_sd_bus_width(SDCC_SD_BUS_WIDTH_4BIT);
        if ( SDCC_NO_ERROR != rc )
        {
           return rc;
        }
     }
  }

  /* CMD13: confirm now in TRAN state */
  card_status = sdcc_send_status();
  if( SDCC_CARD_TRAN != card_status )
  {
      rc = SDCC_ERR_INVALID_TX_STATE;
  }
  
  return rc;

}/* sdcc_config_memory_device */

/******************************************************************************
* Name: sdcc_set_blksz
*
* Description:
*    This function is to set the specified block size if block mode is
*    supported.  The default block length is fixed to 512Bytes.  If
*    block length is set larger than 512Bytes, the card sets the 
*    BLOCK_LEN_ERROR bit.
*
* Arguments:
*       block_length   block length in bytes
*
* Returns:
*    returns status code.
*
******************************************************************************/
SDCC_STATUS sdcc_set_blksz ( uint32 block_length )
{

  SDCC_STATUS      rc          = SDCC_ERR_SET_BLKSZ;
  sdcc_cmd_type    sdcc_cmd;

  /*--------------------------------------------------------*/

  /* CMD16: set the data block length */
  sdcc_cmd.cmd       = SD_CMD16_SET_BLOCKLEN;
  sdcc_cmd.cmd_arg   = block_length;
  sdcc_cmd.resp_type = SDCC_RESP_SHORT;
  sdcc_cmd.prog_scan = 0;

  rc = sdcc_command(&sdcc_cmd);
  if(SDCC_NO_ERROR == rc)
  {
    if(!(sdcc_cmd.resp[0] & SDCC_CARD_STATUS_BLKLEN_ERR))
    {
      rc = SDCC_NO_ERROR;
    }
    else
    {
      rc = SDCC_ERR_SET_BLKSZ;
    }
    if (SDCC_NO_ERROR == rc)
    {
      sdcc_pdata.mem.block_len = block_length;
    }
  }
  
  return rc;

}/* sdcc_set_blksz */

 
/******************************************************************************
* Name: sdcc_decode_csd
*
* Description:
*    This function is to parse out the CSD data.
*
* Arguments:
*       data  pointer to data read in from response registers.
*       csd   pointer to the stucture to save the csd data to.
*
******************************************************************************/
void sdcc_decode_csd
(
  const uint32   *data,
  sdcc_csd_type  *csd
)
{
  uint32   value;

  /*----------------------------------------------------------------------*/

  if(!csd)
      return;
          
  /* In resp[0] */
  value = *data;
  csd->csd_structure      = (uint8)(value >> 30);

  switch (sdcc_pdata.card_type)
  {
  case SDCC_CARD_SD:
  case SDCC_CARD_SDHC:
     {
        if (csd->csd_structure == 1)
        {
           /* CSD Version 2.0: CSD_STRUCTURE = 1 */
           sdcc_decode_hc_csd(data, csd);
        }
        else
        {
           /* CSD Version 1.0: CSD_STRUCTURE = 0 */
           sdcc_decode_std_csd(data, csd);
        }
     }
     break;

  case SDCC_CARD_MMC:
  case SDCC_CARD_MMCHC:
  default:
     {
        sdcc_decode_std_csd(data, csd);
        /* System Specification version is for MMC only. */
        sdcc_pdata.mem.spec_vers = csd->spec_vers;
     }
     break;
  }

  return;

}/* sdcc_decode_csd */

/******************************************************************************
* Name: sdcc_read_fifo
*
* Description:
*    This function is to process "read" in non-dma mode. It reads data
*    out of the FIFO.
*
* Arguments:
*    buff        pointer of the data buffer
*    length      data length
*
* Returns:
*    Returns error code.
*
******************************************************************************/

SDCC_STATUS sdcc_read_fifo
(
   byte      *buff,
   uint16     length
)
{
   volatile uint32 status = 0;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 data_size = (uint32)length;
   uint32 fifo_word = 0;
   uint32 tmp_data_size = data_size;
   boolean reset_inactivity_timer = TRUE;
   timetick_type start_time = 0;
   timetick_type elapsed_time_ms = 0;
   boolean buff_aligned = FALSE;

   if(NULL == buff)
   {
      return(SDCC_ERR_READ_FIFO);
   }
  
   /* is buff 32bit aligned? */
   if (!(((uint32) buff) & 0x03))
   {
      buff_aligned = TRUE;
   }

   while (data_size > 0)
   {
      status = HWIO_IN(MCI_STATUS);
      if (status & SDCC_READ_ERROR)
      {
         DPRINTF(("read error, SDCC status = 0x%x.\n", status));
         rc = SDCC_ERR_READ_FIFO;
         break;
      }

      if (data_size && IS_RXDATA_AVLBL(status))
      {
         fifo_word = HWIO_IN(MCI_FIFO);
         if ( data_size >= 4)
         {
            /* is buff 32bit aligned? */
            if (buff_aligned)
            {
               *(uint32 *)buff = fifo_word;
            }
            else
            {
               /* if buff addr is not aligned, read in data byte by byte*/
               buff[0] = (byte)(fifo_word & 0xFF);
               buff[1] = (byte)((fifo_word & 0xFF00) >> 8);
               buff[2] = (byte)((fifo_word & 0xFF0000) >> 16);
               buff[3] = (byte)((fifo_word & 0xFF000000) >> 24);
            }
            buff += 4;
            data_size -= 4;
         }
         else
         {
            uint32 i = 0;

            for (i = 0; i < data_size; i++)
            {
               buff[i] = (byte)((fifo_word >> (i * 8)) & 0xFF);
            }
            data_size = 0;
         }
      }
      /* If the previous read word attempt was successful data_size would 
      have decremented */
      if (tmp_data_size == data_size)
      {
         if (reset_inactivity_timer)
         {
            start_time = timetick_get();
            reset_inactivity_timer = FALSE;
         }
         else
         {
            elapsed_time_ms = timetick_get_elapsed (start_time, T_MSEC);
            if (elapsed_time_ms > SDCC_FIFO_READ_TIMEOUT_MS)
            {
               DPRINTF(("read timeout error.\n"));
               rc = SDCC_ERR_READ_FIFO;
               break;
            }
         }
      }
      else
      {
         reset_inactivity_timer = TRUE;
         /* update tmp_data_size with the decremented data_size value */
         tmp_data_size = data_size;
      }
   } /*while*/

   /* if end of the block, check for error */
   if ( status & (HWIO_FMSK(MCI_STATUS, DATAEND)) )
   {
      /* clear DATAEND */
      SDCC_CLR_STATUS(HWIO_MCI_CLEAR_DATA_END_CLR_BMSK);
      if( status & SDCC_READ_ERROR)
      {
         DPRINTF(("DATAEND error: status error = 0x%x", status));
         rc = SDCC_ERR_READ_FIFO;
      }
   }

   /* if end of the block, check for error */
   if( status & (HWIO_FMSK(MCI_STATUS, DATA_BLK_END)))
   {
      /* clear DATA_BLK_END */
      SDCC_CLR_STATUS(HWIO_MCI_CLEAR_DATA_BLK_END_CLR_BMSK);
         
      if( status & SDCC_READ_ERROR)
      {
         DPRINTF(("BLK_END error: status error = 0x%x", status));
         rc = SDCC_ERR_READ_FIFO;
      }
   }
  
   /* clear the static status bits */
   SDCC_CLR_STATUS(MCI_STATUS_STATIC_MASK);
   return rc;
}/* sdcc_read_fifo */


/******************************************************************************
* Name: sdcc_write_fifo
*
* Description:
*    This function is to process "write" in non-dma mode. It writes data
*    directly to FIFO. 
*
* Arguments:
*    buff        pointer of the data buffer
*    length      data length
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_write_fifo
(
  byte     *buff,
  uint16    length
)
{
  volatile uint32  status;
  SDCC_STATUS      rc        = SDCC_NO_ERROR;
  uint32           i         = 0;
  uint32           fifo_word = 0;
  int32            dsize     = (int32) length;

  /*------------------------------------------------------------------------*/

  if(!buff)
      return(SDCC_ERR_WRITE_FIFO);

  status = HWIO_IN(MCI_STATUS);

  while ( dsize > 0 )
  {
     if ( status & SDCC_WRITE_ERROR )
     {
        DPRINTF(("write error, SDCC status = 0x%x", status));
        rc = SDCC_ERR_WRITE_FIFO;
        break;
     }

     /* fill up the fifo if not full */
     if ( dsize && !IS_TXDATA_FULL(status) )
     {
        /* pack data byte by byte */
        /* buffer address may not be word aligned */
        fifo_word = 0;
        for ( i = 0; i < 4; i++ )
        {
           fifo_word |= (*buff) << (i * 8);
           buff++;
        }
        HWIO_OUT( MCI_FIFO, fifo_word );
        dsize -= 4;
     }

     status = HWIO_IN(MCI_STATUS);
  }

  /* clear the static status bits */
  SDCC_CLR_STATUS(MCI_STATUS_STATIC_MASK);

  return rc;

}/* sdcc_write_fifo */

/******************************************************************************
* Name: sdcc_complete_data_xfer
*
* Description:
*    This function is to finish up data transfer after the data has been
*    successfully transferred. If this is a multi-block data transfer,
*    we need to send CMD12 to stop the data transfer. 
*
* Arguments:
*       direction        read or write
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_complete_data_xfer(uint32   direction,
                        boolean  multi_block)
{
  SDCC_STATUS      rc = SDCC_NO_ERROR;
  sdcc_cmd_type    sdcc_cmd;

  /*-------------------------------------------------------------*/

  if(multi_block)
  {
    sdcc_cmd.cmd       = SD_CMD12_STOP_TRANSMISSION;
    sdcc_cmd.resp_type = SDCC_RESP_SHORT;
    sdcc_cmd.prog_scan = (SDCC_DATA_WRITE == direction ) ?
                           MCI_CMD_PROG_ENABLED : 0; 
    sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;

    rc = sdcc_command(&sdcc_cmd);


  }

  /* Send the status cmd to check for the card write status */
  else 
  {
    if(SDCC_DATA_WRITE == direction)
    {
       rc =  sdcc_wait_card_ready();
    }
         

  }

  /* Reset the TCXO clock */
  sdcc_set_tcxo_clk(FALSE);
#ifdef FEATURE_SDCC_CLKREGIM_VOTE_HIGH_CLK
  /* Inform clk regime we are finished with DMA transfer */
  clk_regime_deregister_for_cpu_resource ( CLKRGM_CPU_CLIENT_SDCC );
#endif
  return rc;

}/* sdcc_complete_data_xfer */

/******************************************************************************
 * Name: sdcc_send_app_command
 *
 * Description:
 *    This function is to send an App-command to the memory device.
 *
 * Arguments:
 *    cmd_ptr          [IN] : pointer to a structure describing the
 *                            App-Command to send.  The command should
 *                            be one of the SD_ACMDXXXX enums.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_send_app_command
(
   sdcc_cmd_type *cmd_ptr
)
{
   sdcc_cmd_type sdcc_cmd;
   SDCC_STATUS   status;

   /*------------------------------------------------------------------*/

   /*
   First, we must send 'CMD55_APP_CMD' to tell the memory device to expect
   an app-command in the next command we send.  Set up the command
   structure and send the command.  The argument to this command is
   the card address (RCA) in the upper 16 bits.
   */
   sdcc_cmd.cmd       = SD_CMD55_APP_CMD;
   sdcc_cmd.cmd_arg   = (sdcc_pdata.rca[sdcc_pdata.curr_sd_drv] << 16);
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;

   status = sdcc_command(&sdcc_cmd);
   status = sdcc_command(cmd_ptr);

   return status;
} /* sdcc_send_app_command */

/******************************************************************************
 * Name: sdcc_do_transfer
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the memory device.  The flags value contains a flag for whether we are
 *    sending a read or write transfer.
 *
 * Arguments:
 *    xfer_cmd_ptr         [IN] : command to send to the device to perform 
 *                                the read or write
 *    xfer_flags           [IN] : SDCC_TRANSFER_FLAG_IS_APP_COMMAND - Set this
 *                                bit to send the command as an app-command
 *                                SDCC_TRANSFER_FLAG_IS_READ_TRANSFER - Set
 *                                this bit for read transfers, clear it for
 *                                write transfers
 *    buff             [IN/OUT] : buffer of data to read or write
 *    xfer_size            [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_do_transfer
(
   sdcc_cmd_type     *xfer_cmd_ptr,
   uint32             xfer_flags,
   byte              *buff,
   uint16             xfer_size
)
{
   uint8            data_ctrl = 0;
   uint32           blk_in_bits = 0;
   SDCC_STATUS      status     = SDCC_NO_ERROR;
   SDCC_STATUS      status_complete = SDCC_NO_ERROR;
   uint16           length     = 0;
   boolean          use_dma    = FALSE;
   uint32           isave      = 0;
   boolean          send_stop_command;

   /*------------------------------------------------------------------*/

   /* check if xfer_flags is valid */
   if ( SDCC_IS_INVALID_TRANSFER_FLAG(xfer_flags) )
   {
      DPRINTF(("Invalid xfer_flags 0x%x\n", xfer_flags));
      sdcc_pdata.errno = SDCC_ERR_UNKNOWN;
      return SDCC_ERR_UNKNOWN;
   }

   /* setup the data transfer */
   /* This function sets a timeout and the data length for the transfer. */
   length = (uint16)sdcc_setup_data_xfer(SDCC_DATA_DIRECTION(xfer_flags), 
                                         xfer_size);

   if (!length)
   {
      sdcc_pdata.errno = SDCC_ERR_UNKNOWN;
      return SDCC_ERR_UNKNOWN;
   }

   if ( SDCC_MEM_BLK_MODE == sdcc_pdata.block_mode &&
        xfer_size > SDCC_MAX_NO_BLOCKS )
   {
      DPRINTF(("xfer_size %d exceeds max. number of blocks\n", xfer_size));
   }
   /* turn on DMA only if the passed in address is dword aligned and we */
   /* are supposed to enable DMA.  enable_dma is set in sdcc_setup_data_xfer */
   else if ( (TRUE == sdcc_pdata.enable_dma) && IS_ALIGNED(buff) )
   {
      use_dma = TRUE;

      /* make sure to sync with the DMA memory */
      mmu_invalidate_data_cache_lines((uint32*)buff, length); //lint !e826
      data_ctrl |= MCI_DATA_DMA_ENABLED;
      HWIO_OUT(MCI_START_ADDR, (uint32)buff);
   }
   else
   {
      DPRINTF(("buffer address 0x%x not aligned\n", buff));
   }

   if ( SDCC_MEM_BYTE_MODE == sdcc_pdata.block_mode )
   {
      blk_in_bits = sdcc_blk_in_bits(length);
   }
   else
   {
      blk_in_bits = sdcc_blk_in_bits(sdcc_pdata.mem.block_len);
   }
   data_ctrl |= (blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT);
   data_ctrl |= MCI_DATA_DPSM_ENABLED;
   data_ctrl |= SDCC_MCI_DIRECTION(xfer_flags);

   INTLOCK_SAV(isave);

   /* This register needs to be done before sending the command */
   /* on reads and after on writes. */
   if (SDCC_IS_READ_TRANSFER(xfer_flags))
   {
      HWIO_OUT(MCI_DATA_CTL, data_ctrl);
   }

   /* Send the command to the memory device.  Note that app. commands use a */
   /* different function since they actually send two commands. */
   if ((xfer_flags & SDCC_APP_COMMAND_FLAG) != 0)
   {
      status = sdcc_send_app_command(xfer_cmd_ptr);
   }
   else
   {
      status = sdcc_command(xfer_cmd_ptr);
   }

   /* This register needs to be done before sending the command */
   /* on reads and after on writes. */
   if (SDCC_IS_WRITE_TRANSFER(xfer_flags))
   {
      HWIO_OUT(MCI_DATA_CTL, data_ctrl);
   }

   INTFREE_SAV(isave);

   /* Transfer the data to or from the device, either using DMA or */
   /* by reading or writing the FIFO registers directly */
   if (status == SDCC_NO_ERROR)
   {
      if (use_dma)
      {
         status = sdcc_process_interrupts(xfer_cmd_ptr /*don't care*/);
      }
      else
      {
         if (SDCC_IS_READ_TRANSFER(xfer_flags))
         {
            status = sdcc_read_fifo(buff, length);
         }
         else
         {
            status = sdcc_write_fifo(buff, length);
         }
      }
   }

   /* Complete the data transfer by sending a stop command if required, */
   /* and clearing the status register */
   send_stop_command =
      (((xfer_flags & SDCC_APP_COMMAND_FLAG) == 0) &&
         ((xfer_cmd_ptr->cmd == SD_CMD18_READ_MULTIPLE_BLOCK) ||
         (xfer_cmd_ptr->cmd == SD_CMD25_WRITE_MULTIPLE_BLOCK)));

   status_complete = sdcc_complete_data_xfer(SDCC_DATA_DIRECTION(xfer_flags),
                                             send_stop_command);

   sdcc_pdata.errno = status;
   if ( SDCC_NO_ERROR != status || SDCC_NO_ERROR != status_complete )
   {
      status = (SDCC_IS_READ_TRANSFER(xfer_flags)) ? 
               SDCC_ERR_READ_FIFO : SDCC_ERR_WRITE_FIFO;
   }

   return status;
} /* sdcc_do_transfer */

/******************************************************************************
* Name: sdcc_get_hc_memory_info
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the High Capacity SD memory card.
*
* Arguments:
*       block_len        data block length
*       c_size           device size
*
* Returns:
*    None
*
******************************************************************************/
static void 
sdcc_get_hc_memory_info
(
  uint32    block_len,
  uint32    c_size
)
{
  sdcc_pdata.mem.block_len = 1UL << block_len; /* in Bytes */

  /* The user data area capacity is calculated from the c_size as follows: */
  /*    memory capacity = (c_size+1)*512K byte    */
  /* where 512 is the block len of the High Capacity SD memory card and */
  /*       1K byte = 1024 bytes */
  /* Calculate the card size in the absolute block length of muliple */
  /* 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE) */
  sdcc_pdata.mem.card_size = (c_size + 1)*((sdcc_pdata.mem.block_len*1024)/SDCC_DEFAULT_BLK_LENGTH_SIZE);
} /* sdcc_get_hc_memory_info */

/******************************************************************************
* Name: sdcc_get_std_memory_info
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the Standard Capacity SD memory card.
*
* Arguments:
*       block_len        data block length
*       c_size_mult      device size multiplier
*       c_size           device size
*
* Returns:
*    None
*
******************************************************************************/
static void 
sdcc_get_std_memory_info
(
  uint32    block_len, 
  uint8     c_size_mult, 
  uint32    c_size
)
{
  uint32    card_size;

  card_size     = 1 << (c_size_mult + 2);
  card_size    *= (c_size + 1); 

  sdcc_pdata.mem.block_len    = 1UL << block_len; /* in Bytes */

  /* Since the block length can be in diffrent size
  * 512, 1024, 2048 etc..Calculate the card size in the absolute 
  * block length of multiple 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE)
  */
  sdcc_pdata.mem.card_size    = card_size*(sdcc_pdata.mem.block_len/SDCC_DEFAULT_BLK_LENGTH_SIZE);
} /* sdcc_get_std_memory_info */

/******************************************************************************
* Name: sdcc_decode_std_csd
*
* Description:
*    This function is to read the passed in data of the Standard Capacity
*    SD memory card (CSD version 1.0) and assign the proper bit values
*    to the fields in csd structure.
*
* Arguments:
*       data             pointer to data read in from response registers.
*       csd              pointer to the stucture to save the csd data to.
*
* Returns:
*    None
*
******************************************************************************/
static void 
sdcc_decode_std_csd
(
  const uint32   *data, 
  sdcc_csd_type  *csd
)
{
  uint32    value;
  uint32    tmp;

  if (NULL == csd)
  {
    return;
  }

  /* In resp[0] */
  value                   = *data;
  /* For MMC only: spec_vers: system specification version --> csd[125:122] */
  csd->spec_vers          = (uint8)((value & 0x3C000000) >> 26);
  /* taac: data read access-time-1 --> csd[119:112] */
  csd->taac               = (uint8)((value & 0x00FF0000) >> 16);
  /* nsac: data read access-time-2 in CLK cycles --> csd[111:104] */
  csd->nsac               = (uint8)((value & 0x0000FF00) >> 8);
  /* tran_speed: max. data transfer rate --> csd[103:96] */
  csd->tran_speed         = (uint8)(value & 0x000000FF);

  /* read_bl_len: max. read data block length --> csd[83:80] */
  value                   = *(++data);
  csd->read_bl_len        = (uint8)((value & 0x000F0000) >> 16);

  /* c_size: device size --> csd[73:62] */
  tmp         = value & 0x000003FF;
  value       = *(++data);
  csd->c_size = (uint32)((tmp << 2) | ((value & 0xC0000000) >> 30));

  /* c_size_mult: device size multiplier --> csd[49:47] */
  csd->c_size_mult        = (uint16)((value & 0x00038000) >> 15);
} /* sdcc_decode_std_csd */

/******************************************************************************
* Name: sdcc_decode_hc_csd
*
* Description:
*    This function is to read the passed in data of the High Capacity
*    SD memory card (CSD version 2.0) and assign the proper bit values
*    to the fields in csd structure.

*
* Arguments:
*       data             pointer to data read in from response registers.
*       csd              pointer to the stucture to save the csd data to.
*
* Returns:
*    None
*
******************************************************************************/
static void 
sdcc_decode_hc_csd
(
  const uint32   *data,
  sdcc_csd_type  *csd
)
{
  uint32    value;
  uint32    tmp;

  if (NULL == csd)
  {
    return;
  }

  /* In resp[0] */
  value                   = *data;
  /* taac: data read access-time is fixed to 0Eh (means 1ms) --> csd[119:112]*/
  csd->taac               = SDCC_CSD_TAAC;
  /* nsac: data read access-time in CLK cycles is fixed to 00h --> csd[111:104] */
  csd->nsac               = SDCC_CSD_NSAC;
  /* tran_speed: max. data transfer rate --> csd[103:96] */
  csd->tran_speed         = (uint8)(value & 0x000000FF);

  /* read_bl_len: max. read data block length is fixed to */
  /* 9h (means 512 Bytes) --> csd[83:80] */
  value                   = *(++data);
  csd->read_bl_len        = SDCC_CSD_READ_BL_LEN;

  /* c_size: device size --> csd[69:48] */
  /* This field is expanded to 22 bits and can indicate up to 2TBytes. */
  tmp         = value & 0x0000003F;
  value       = *(++data);
  csd->c_size = (uint32)((tmp << 16) | ((value & 0xFFFF0000) >> 16));
} /* sdcc_decode_hc_csd */


/******************************************************************************
* Name: sdcc_enter_crit_sect
*
* Description:
*    Enable exclusive access to SD controller
*
* Arguments:
*    None
* Returns:
*    None
*
******************************************************************************/
void sdcc_enter_crit_sect(void)
{
    rex_enter_crit_sect(&sdcc_crit_sect);
} /* sdcc_enter_crit_sect */

/******************************************************************************
* Name: sdcc_leave_crit_sect
*
* Description:
*   Disable exclusive access to the SD controller
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_leave_crit_sect(void)
{
    rex_leave_crit_sect(&sdcc_crit_sect);
} /* sdcc_leave_crit_sect */

/******************************************************************************
* Name: sdcc_init_crit_sect
*
* Description:
*    Create and initialize a critical section for exclusive access to 
*    the SD controller. If critical section already exists, do nothing
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_init_crit_sect(void)
{
   if(FALSE == sdcc_crit_sec_initialized)
   {
     rex_init_crit_sect(&sdcc_crit_sect);
     sdcc_crit_sec_initialized = TRUE;
   }
} /* sdcc_init_crit_sect */


/******************************************************************************
* Name: sdcc_slot_isr
*
* Description: This ISR is called when either a card is inserted or removed
*    
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_slot_isr(void)
{
   sdcc_pdata.slot_state_changed = TRUE;
}

/******************************************************************************
* Name: sdcc_silent_reinit
*
* Description: 
*    Initializes the card.
*    This function is called if the driver determined that the slot was 
*    shutdown by sdcc_activity_timeout_event() and the card was not physically
*    removed after that. 
*
* Arguments:
*    None
*
* Returns:
*    TRUE if successful, FALSE otherwise
*
******************************************************************************/
boolean sdcc_silent_reinit()
{
   const int16 driveno = 0;
   //already in SDCC critical section

   // if card is removed or reinserted, fail
   if(sdcc_pdata.slot_state_changed)
   {
      return FALSE;
   }
   DPRINTF(("SDCC : Begin silent_reinit, driveno %d\n", driveno));
   if(FALSE == sdcc_init())
      return FALSE;
   if(FALSE == sdcc_open(driveno))
      return FALSE;
   DPRINTF(("SDCC : End silent_reinit, driveno %d\n", driveno));
   return TRUE;
}
/*lint -restore */
