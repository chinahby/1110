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
#ifndef T_QSC1100
/* Use for the generic signal */
static uint32 sdcc_sigs =0;
#endif
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
#ifndef T_QSC1100
   sdcc_data_type *sdcc_pdata = (sdcc_data_type *)sdcc_pdata_cb;
   sdcc_sigs |= SDCC_TIMER_TIMEOUT_SIG_VAL;
   (void) rex_set_sigs(sdcc_pdata->sdcc_tcb, SDCC_RW_COMPLETE_SIG);
#endif
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
#ifndef T_QSC1100
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
#else
  return SDCC_NO_ERROR;
#endif
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
#ifndef T_QSC1100
  HWIO_OUTM(MCI_DMA_CONFIG,
            HWIO_FMSK(MCI_DMA_CONFIG, ENDIAN_CHANGE) |
            HWIO_FMSK(MCI_DMA_CONFIG, ENDIAN_CHANGE),
            0x03);
#endif    
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
#ifndef T_QSC1100
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
#endif //#ifndef T_QSC1100

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
#ifndef T_QSC1100
  uint32        count  = 0;
  uint32        i;
  uint32       *resp = NULL;
#endif
  SDCC_STATUS   rc   = SDCC_NO_ERROR;

  /*---------------------------------------------------------------------*/
  
  /* loading command responses */    
  if (sdcc_cmd->resp_type)
  {
#ifndef T_QSC1100
    count = ( SDCC_RESP_LONG == sdcc_cmd->resp_type ) ? 4 : 1;
    resp  = sdcc_cmd->resp;
    
    for (i = 0; i < count ; i++ )
    {
      *resp = HWIO_INI(MCI_RESPn, i);
      resp++;
    }
#endif
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
#ifndef T_QSC1100
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
#else
  SDCC_STATUS   rc;
  
  rc = sdcc_cmd->status;
  if(rc == SDCC_ERR_CMD_CRC_FAIL)
  {
    if(( SD_ACMD41_SD_APP_OP_COND == sdcc_cmd->cmd ) ||
       ( SD_CMD1_SEND_OP_COND     == sdcc_cmd->cmd ) ||
       ( SD_CMD5_IO_SEND_OP_COND  == sdcc_cmd->cmd ))
    {
        rc = SDCC_NO_ERROR;
    }
  }
#endif

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
#ifndef T_QSC1100
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
#else
  sdcc_pdata.errno = SDCC_NO_ERROR;
  return SDCC_NO_ERROR;
#endif
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
#ifndef T_QSC1100
#ifdef FEATURE_6275_COMPILE
    HWIO_OUT(MCI_DATA_TIMER, clk_hclk_freq_khz * timeout );
#else    
    HWIO_OUT(MCI_DATA_TIMER, clk_mclk_freq_mhz * timeout * 1000 );
#endif
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
#ifndef T_QSC1100
    sdcc_pdata.enable_dma = FALSE;
#endif
    /* set data length */
    data_length = units;
#ifndef T_QSC1100
    HWIO_OUT(MCI_DATA_LENGTH, data_length);
#endif
  }
  else
  {
#ifndef T_QSC1100
    sdcc_pdata.enable_dma = TRUE;
#endif
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
#ifndef T_QSC1100
    HWIO_OUT(MCI_DATA_LENGTH, data_length);

// Targets such as ULC have data cache-enabled MMU. The data 
// cache is shared and might have stale data.
#ifdef FEATURE_SDCC_DCACHE_ENABLED
    mmu_dcache_flush_all();
#endif
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
#ifndef T_QSC1100
     /************************************************************************
      *
      *       Setup the TCB for rex signal
      *
      ************************************************************************/
    
     sdcc_pdata.sdcc_tcb = rex_self ();

     if( NULL == sdcc_pdata.sdcc_tcb )
        ASSERT(0);
#endif
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
#ifndef T_QSC1100
    rc =  sdcc_cmd_send_verify();
#else
    rc = sdcc_cmd->status;
#endif
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
#ifndef T_QSC1100
  /***********************************************************************
   *
   *     wait for the device is done
   *
   ***********************************************************************/
  if(sdcc_cmd->prog_scan) 
  {
    rc = sdcc_process_interrupts(sdcc_cmd);
  }
#endif
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
#ifndef T_QSC1100
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
#else
    // Send Command
    boolean bSendNRC = TRUE;
    static byte cmd[20];
    register uint32 arg = sdcc_cmd->cmd_arg;
    
    cmd[0] = 0x40|(((byte)sdcc_cmd->cmd)&0x3F);
    cmd[1] = (byte)(arg>>24);
    cmd[2] = (byte)(arg>>16);
    cmd[3] = (byte)(arg>>8);
    cmd[4] = (byte)(arg);
    cmd[5] = CRC7(cmd, 5);
    
    sdcc_send_cmd_bytes(cmd, 6);
    dog_kick();
    // Get Respond
    sdcc_cmd->status = SDCC_NO_ERROR;
    if(sdcc_cmd->resp_type)
    {
        switch(sdcc_cmd->cmd){
        case SD_ACMD51_SEND_SCR:
        case SD_CMD17_READ_BLOCK:
        case SD_CMD18_READ_MULTIPLE_BLOCK:
            // 忽略RESPONSE
            gpio_tlmm_config(GPIO_SDCC_CMD_OUT);
            return;

        case SD_CMD24_WRITE_BLOCK:
        case SD_CMD25_WRITE_MULTIPLE_BLOCK:
            bSendNRC = FALSE;
            gpio_tlmm_config(GPIO_SDCC_CMD_IN);
            cmd[0] = sdcc_recv_cmd_byte_wait();
            break;
            
        default:
            gpio_tlmm_config(GPIO_SDCC_CMD_IN);
            cmd[0] = sdcc_recv_cmd_byte_wait();
            break;
        }
        if(sdcc_cmd->resp_type == SDCC_RESP_SHORT) // 6Bytes
        {
            sdcc_recv_cmd_bytes(cmd+1, 5);
            arg  = cmd[1]<<24;
            arg |= cmd[2]<<16;
            arg |= cmd[3]<<8;
            arg |= cmd[4];
            sdcc_cmd->resp[0] = arg;
            if(cmd[0]&0xC0) // 根据协议，回复的高两位应该为0
            {
                sdcc_cmd->status = SDCC_ERR_CMD_TIMEOUT;
            }
            else if(cmd[5] != CRC7(cmd, 5))
            {
                sdcc_cmd->status = SDCC_ERR_CMD_CRC_FAIL;
            }
            MSG_FATAL("SDCC_SEND_CMD %d %02x %08x",sdcc_cmd->status,cmd[0],arg);
        }
        else if(sdcc_cmd->resp_type == SDCC_RESP_LONG) // 18Bytes
        {
            sdcc_recv_cmd_bytes(cmd+1, 16);
            arg  = cmd[1]<<24;
            arg |= cmd[2]<<16;
            arg |= cmd[3]<<8;
            arg |= cmd[4];
            sdcc_cmd->resp[0] = arg;
            arg  = cmd[5]<<24;
            arg |= cmd[6]<<16;
            arg |= cmd[7]<<8;
            arg |= cmd[8];
            sdcc_cmd->resp[1] = arg;
            arg  = cmd[9]<<24;
            arg |= cmd[10]<<16;
            arg |= cmd[11]<<8;
            arg |= cmd[12];
            sdcc_cmd->resp[2] = arg;
            arg  = cmd[13]<<24;
            arg |= cmd[14]<<16;
            arg |= cmd[15]<<8;
            arg |= cmd[16];
            sdcc_cmd->resp[3] = arg;
            if(cmd[0]&0xC0)// 根据协议，回复的高两位应该为0
            {
                sdcc_cmd->status = SDCC_ERR_CMD_TIMEOUT;
            }
            else if(cmd[16] != CRC7(cmd+1, 15))
            {
                sdcc_cmd->status = SDCC_ERR_CMD_CRC_FAIL;
            }
        }
        gpio_tlmm_config(GPIO_SDCC_CMD_OUT);
    }
    
    dog_kick();
    if(bSendNRC)
    {
        sdcc_clock_out(8); // Nrc
    }
#endif
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
#ifndef T_QSC1100
  uint32               t      = 0;
#endif
  volatile uint32      status = 0;
  SDCC_STATUS          rc          = SDCC_ERR_PROG_DONE;
  SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;

  /*--------------------------------------------------------------------*/
#ifndef T_QSC1100
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
#else
  // TODO:
  rc = SDCC_NO_ERROR;
#endif
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
   
#ifdef T_QSC1100
   // output 74 clock
   sdcc_clock_out_slow(100);
#endif
   
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
#ifndef T_QSC1100
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
#else
   return SDCC_NO_ERROR;
#endif
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
#ifndef T_QSC1100
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
#endif
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
#ifndef T_QSC1100
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
#endif
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
#ifndef T_QSC1100
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
#endif
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
#ifndef T_QSC1100
   uint32 status;
   status = HWIO_IN(MCI_STATUS);
   if(status & HWIO_MCI_STATUS_SDIO_INTR_BMSK)
   {
      if(sdcc_pdata.io.isr)
         (void)sdcc_pdata.io.isr((void*)sdcc_pdata.io.isr_param);
      SDCC_CLR_STATUS(HWIO_MCI_CLEAR_SDIO_INTR_CLR_BMSK);
   }
#endif
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
#ifdef T_QSC1100
      sdcc_clock_out(100);
#else
      sdcc_udelay(100);
#endif
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
#ifndef T_QSC1100
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
#else
    SDCC_STATUS rc = SDCC_NO_ERROR;
    
    if(NULL == buff)
    {
       return(SDCC_ERR_READ_FIFO);
    }
    
    return rc;
#endif
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
#ifndef T_QSC1100
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
#else
    SDCC_STATUS rc = SDCC_NO_ERROR;
    if(!buff)
          return(SDCC_ERR_WRITE_FIFO);
    return rc;
#endif
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
#ifndef CUST_EDITION
  //else 无论多block还是单block都查询写入之后的状态
#endif
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
#ifndef T_QSC1100
   uint8            data_ctrl = 0;
   uint32           blk_in_bits = 0;
#endif
   SDCC_STATUS      status     = SDCC_NO_ERROR;
   SDCC_STATUS      status_complete = SDCC_NO_ERROR;
   uint16           length     = 0;
#ifndef T_QSC1100
   boolean          use_dma    = FALSE;
#endif
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

#ifndef T_QSC1100
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
#endif
   INTLOCK_SAV(isave);
#ifndef T_QSC1100
   /* This register needs to be done before sending the command */
   /* on reads and after on writes. */
   if (SDCC_IS_READ_TRANSFER(xfer_flags))
   {
      HWIO_OUT(MCI_DATA_CTL, data_ctrl);
   }
#endif
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
#ifndef T_QSC1100
   /* This register needs to be done before sending the command */
   /* on reads and after on writes. */
   if (SDCC_IS_WRITE_TRANSFER(xfer_flags))
   {
      HWIO_OUT(MCI_DATA_CTL, data_ctrl);
   }
#endif
   INTFREE_SAV(isave);

   /* Transfer the data to or from the device, either using DMA or */
   /* by reading or writing the FIFO registers directly */
   if (status == SDCC_NO_ERROR)
   {
#ifndef T_QSC1100
      if (use_dma)
      {
         status = sdcc_process_interrupts(xfer_cmd_ptr /*don't care*/);
      }
      else
#endif
      {
         if (SDCC_IS_READ_TRANSFER(xfer_flags))
         {
#ifndef T_QSC1100
            status = sdcc_read_fifo(buff, length);
#else
            status = sdcc_read_data(buff, length);
#endif
         }
         else
         {
#ifndef T_QSC1100
            status = sdcc_write_fifo(buff, length);
#else
            status = sdcc_write_data(buff, length);
#endif
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

#ifdef T_QSC1100
#include "crc.h"

#define SDCC_MIN_WAIT   1024
#define SDCC_MAX_WAIT   (1024*1024*8)

static const unsigned char crc7_table[256] = 
{
    0x00,0x09,0x12,0x1B,0x24,0x2D,0x36,0x3F,0x48,0x41,0x5A,0x53,0x6C,0x65,0x7E,0x77,
    0x99,0x90,0x8B,0x82,0xBD,0xB4,0xAF,0xA6,0xD1,0xD8,0xC3,0xCA,0xF5,0xFC,0xE7,0xEE,
    0x32,0x3B,0x20,0x29,0x16,0x1F,0x04,0x0D,0x7A,0x73,0x68,0x61,0x5E,0x57,0x4C,0x45,
    0xAB,0xA2,0xB9,0xB0,0x8F,0x86,0x9D,0x94,0xE3,0xEA,0xF1,0xF8,0xC7,0xCE,0xD5,0xDC,
    0x64,0x6D,0x76,0x7F,0x40,0x49,0x52,0x5B,0x2C,0x25,0x3E,0x37,0x08,0x01,0x1A,0x13,
    0xFD,0xF4,0xEF,0xE6,0xD9,0xD0,0xCB,0xC2,0xB5,0xBC,0xA7,0xAE,0x91,0x98,0x83,0x8A,
    0x56,0x5F,0x44,0x4D,0x72,0x7B,0x60,0x69,0x1E,0x17,0x0C,0x05,0x3A,0x33,0x28,0x21,
    0xCF,0xC6,0xDD,0xD4,0xEB,0xE2,0xF9,0xF0,0x87,0x8E,0x95,0x9C,0xA3,0xAA,0xB1,0xB8,
    0xC1,0xC8,0xD3,0xDA,0xE5,0xEC,0xF7,0xFE,0x89,0x80,0x9B,0x92,0xAD,0xA4,0xBF,0xB6,
    0x58,0x51,0x4A,0x43,0x7C,0x75,0x6E,0x67,0x10,0x19,0x02,0x0B,0x34,0x3D,0x26,0x2F,
    0xF3,0xFA,0xE1,0xE8,0xD7,0xDE,0xC5,0xCC,0xBB,0xB2,0xA9,0xA0,0x9F,0x96,0x8D,0x84,
    0x6A,0x63,0x78,0x71,0x4E,0x47,0x5C,0x55,0x22,0x2B,0x30,0x39,0x06,0x0F,0x14,0x1D,
    0xA5,0xAC,0xB7,0xBE,0x81,0x88,0x93,0x9A,0xED,0xE4,0xFF,0xF6,0xC9,0xC0,0xDB,0xD2,
    0x3C,0x35,0x2E,0x27,0x18,0x11,0x0A,0x03,0x74,0x7D,0x66,0x6F,0x50,0x59,0x42,0x4B,
    0x97,0x9E,0x85,0x8C,0xB3,0xBA,0xA1,0xA8,0xDF,0xD6,0xCD,0xC4,0xFB,0xF2,0xE9,0xE0,
    0x0E,0x07,0x1C,0x15,0x2A,0x23,0x38,0x31,0x46,0x4F,0x54,0x5D,0x62,0x6B,0x70,0x79
};

/******************************************************************************************************
* FunName : CRC7_T                                                                                                 
* Function : Get CRC7 value
* Input    : chr - point to the Arrarybuff; cnt - number needed to workout
* Output   : CRC value
******************************************************************************************************/
unsigned char CRC7( unsigned char * buf_ptr, int len )
{
    unsigned char crc = 0; 
    unsigned char idx;
    
    for (; len >= 1; len--, buf_ptr++) {
        idx = (crc<<1) ^ *buf_ptr;
        crc = (unsigned char)(crc7_table[idx]);
    }
    return((crc<<1)|1); 
}

/* Mask for CRC-16 4line polynomial:
**
**      x^16 + x^12 + x^5 + 1
**      x^64 + x^48 + x^20 + 1
**
** This is more commonly referred to as CCITT-16.
** Note:  the x^16 tap is left off, it's implicit.
*/
static const unsigned long long crc16_4_table[256] = 
{
    0x0000000000000000LL,0x0001000000100001LL,0x0002000000200002LL,0x0003000000300003LL,0x0004000000400004LL,0x0005000000500005LL,0x0006000000600006LL,0x0007000000700007LL,
    0x0008000000800008LL,0x0009000000900009LL,0x000A000000A0000ALL,0x000B000000B0000BLL,0x000C000000C0000CLL,0x000D000000D0000DLL,0x000E000000E0000ELL,0x000F000000F0000FLL,
    0x0010000001000010LL,0x0011000001100011LL,0x0012000001200012LL,0x0013000001300013LL,0x0014000001400014LL,0x0015000001500015LL,0x0016000001600016LL,0x0017000001700017LL,
    0x0018000001800018LL,0x0019000001900019LL,0x001A000001A0001ALL,0x001B000001B0001BLL,0x001C000001C0001CLL,0x001D000001D0001DLL,0x001E000001E0001ELL,0x001F000001F0001FLL,
    0x0020000002000020LL,0x0021000002100021LL,0x0022000002200022LL,0x0023000002300023LL,0x0024000002400024LL,0x0025000002500025LL,0x0026000002600026LL,0x0027000002700027LL,
    0x0028000002800028LL,0x0029000002900029LL,0x002A000002A0002ALL,0x002B000002B0002BLL,0x002C000002C0002CLL,0x002D000002D0002DLL,0x002E000002E0002ELL,0x002F000002F0002FLL,
    0x0030000003000030LL,0x0031000003100031LL,0x0032000003200032LL,0x0033000003300033LL,0x0034000003400034LL,0x0035000003500035LL,0x0036000003600036LL,0x0037000003700037LL,
    0x0038000003800038LL,0x0039000003900039LL,0x003A000003A0003ALL,0x003B000003B0003BLL,0x003C000003C0003CLL,0x003D000003D0003DLL,0x003E000003E0003ELL,0x003F000003F0003FLL,
    0x0040000004000040LL,0x0041000004100041LL,0x0042000004200042LL,0x0043000004300043LL,0x0044000004400044LL,0x0045000004500045LL,0x0046000004600046LL,0x0047000004700047LL,
    0x0048000004800048LL,0x0049000004900049LL,0x004A000004A0004ALL,0x004B000004B0004BLL,0x004C000004C0004CLL,0x004D000004D0004DLL,0x004E000004E0004ELL,0x004F000004F0004FLL,
    0x0050000005000050LL,0x0051000005100051LL,0x0052000005200052LL,0x0053000005300053LL,0x0054000005400054LL,0x0055000005500055LL,0x0056000005600056LL,0x0057000005700057LL,
    0x0058000005800058LL,0x0059000005900059LL,0x005A000005A0005ALL,0x005B000005B0005BLL,0x005C000005C0005CLL,0x005D000005D0005DLL,0x005E000005E0005ELL,0x005F000005F0005FLL,
    0x0060000006000060LL,0x0061000006100061LL,0x0062000006200062LL,0x0063000006300063LL,0x0064000006400064LL,0x0065000006500065LL,0x0066000006600066LL,0x0067000006700067LL,
    0x0068000006800068LL,0x0069000006900069LL,0x006A000006A0006ALL,0x006B000006B0006BLL,0x006C000006C0006CLL,0x006D000006D0006DLL,0x006E000006E0006ELL,0x006F000006F0006FLL,
    0x0070000007000070LL,0x0071000007100071LL,0x0072000007200072LL,0x0073000007300073LL,0x0074000007400074LL,0x0075000007500075LL,0x0076000007600076LL,0x0077000007700077LL,
    0x0078000007800078LL,0x0079000007900079LL,0x007A000007A0007ALL,0x007B000007B0007BLL,0x007C000007C0007CLL,0x007D000007D0007DLL,0x007E000007E0007ELL,0x007F000007F0007FLL,
    0x0080000008000080LL,0x0081000008100081LL,0x0082000008200082LL,0x0083000008300083LL,0x0084000008400084LL,0x0085000008500085LL,0x0086000008600086LL,0x0087000008700087LL,
    0x0088000008800088LL,0x0089000008900089LL,0x008A000008A0008ALL,0x008B000008B0008BLL,0x008C000008C0008CLL,0x008D000008D0008DLL,0x008E000008E0008ELL,0x008F000008F0008FLL,
    0x0090000009000090LL,0x0091000009100091LL,0x0092000009200092LL,0x0093000009300093LL,0x0094000009400094LL,0x0095000009500095LL,0x0096000009600096LL,0x0097000009700097LL,
    0x0098000009800098LL,0x0099000009900099LL,0x009A000009A0009ALL,0x009B000009B0009BLL,0x009C000009C0009CLL,0x009D000009D0009DLL,0x009E000009E0009ELL,0x009F000009F0009FLL,
    0x00A000000A0000A0LL,0x00A100000A1000A1LL,0x00A200000A2000A2LL,0x00A300000A3000A3LL,0x00A400000A4000A4LL,0x00A500000A5000A5LL,0x00A600000A6000A6LL,0x00A700000A7000A7LL,
    0x00A800000A8000A8LL,0x00A900000A9000A9LL,0x00AA00000AA000AALL,0x00AB00000AB000ABLL,0x00AC00000AC000ACLL,0x00AD00000AD000ADLL,0x00AE00000AE000AELL,0x00AF00000AF000AFLL,
    0x00B000000B0000B0LL,0x00B100000B1000B1LL,0x00B200000B2000B2LL,0x00B300000B3000B3LL,0x00B400000B4000B4LL,0x00B500000B5000B5LL,0x00B600000B6000B6LL,0x00B700000B7000B7LL,
    0x00B800000B8000B8LL,0x00B900000B9000B9LL,0x00BA00000BA000BALL,0x00BB00000BB000BBLL,0x00BC00000BC000BCLL,0x00BD00000BD000BDLL,0x00BE00000BE000BELL,0x00BF00000BF000BFLL,
    0x00C000000C0000C0LL,0x00C100000C1000C1LL,0x00C200000C2000C2LL,0x00C300000C3000C3LL,0x00C400000C4000C4LL,0x00C500000C5000C5LL,0x00C600000C6000C6LL,0x00C700000C7000C7LL,
    0x00C800000C8000C8LL,0x00C900000C9000C9LL,0x00CA00000CA000CALL,0x00CB00000CB000CBLL,0x00CC00000CC000CCLL,0x00CD00000CD000CDLL,0x00CE00000CE000CELL,0x00CF00000CF000CFLL,
    0x00D000000D0000D0LL,0x00D100000D1000D1LL,0x00D200000D2000D2LL,0x00D300000D3000D3LL,0x00D400000D4000D4LL,0x00D500000D5000D5LL,0x00D600000D6000D6LL,0x00D700000D7000D7LL,
    0x00D800000D8000D8LL,0x00D900000D9000D9LL,0x00DA00000DA000DALL,0x00DB00000DB000DBLL,0x00DC00000DC000DCLL,0x00DD00000DD000DDLL,0x00DE00000DE000DELL,0x00DF00000DF000DFLL,
    0x00E000000E0000E0LL,0x00E100000E1000E1LL,0x00E200000E2000E2LL,0x00E300000E3000E3LL,0x00E400000E4000E4LL,0x00E500000E5000E5LL,0x00E600000E6000E6LL,0x00E700000E7000E7LL,
    0x00E800000E8000E8LL,0x00E900000E9000E9LL,0x00EA00000EA000EALL,0x00EB00000EB000EBLL,0x00EC00000EC000ECLL,0x00ED00000ED000EDLL,0x00EE00000EE000EELL,0x00EF00000EF000EFLL,
    0x00F000000F0000F0LL,0x00F100000F1000F1LL,0x00F200000F2000F2LL,0x00F300000F3000F3LL,0x00F400000F4000F4LL,0x00F500000F5000F5LL,0x00F600000F6000F6LL,0x00F700000F7000F7LL,
    0x00F800000F8000F8LL,0x00F900000F9000F9LL,0x00FA00000FA000FALL,0x00FB00000FB000FBLL,0x00FC00000FC000FCLL,0x00FD00000FD000FDLL,0x00FE00000FE000FELL,0x00FF00000FF000FFLL
};

/******************************************************************************************************
* FunName : CRC16_4                                                                                                 
* Function : Get CRC16 4line value
* Input    : chr - point to the Arrarybuff; cnt - number needed to workout
* Output   : CRC value
******************************************************************************************************/
unsigned long long CRC16_4( unsigned char * buf_ptr, int len )
{
    unsigned long long crc = 0; 
    unsigned char *pCrc = (unsigned char *)&crc;

    for (; len >= 1; len--, buf_ptr++) {
        crc = (unsigned long long)(crc16_4_table[ pCrc[7] ^ *buf_ptr ] ^ (crc << 8));
    }
    return(crc); 
}

static INLINE void sdcc_clock_out(int cnt)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    while(cnt)
    {
        outpdw(pDest, clkl);
        cnt--;
        outpdw(pDest, clkh);
    }
}

static INLINE void sdcc_clock_out_slow(int cnt)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    
    while(cnt--)
    {
        outpdw(pDest, clkl);
        outpdw(pDest, clkl);
        outpdw(pDest, clkl);
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        outpdw(pDest, clkh);
        outpdw(pDest, clkh);
    }
}

static INLINE void sdcc_send_cmd_bytes(byte *pdata, int len)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkh = (GPIO_SDCC_CLK_MASK|(*pDest))&GPIO_SDCC_CMD_MASK_I;
    register byte data;
    register uint32 mask = GPIO_SDCC_CMD_MASK;
    
    while(len--)
    {
        data = *pdata;
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<22)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<22)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<23)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<23)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<24)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<24)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<25)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<25)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<26)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<26)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<27)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<27)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<28)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<28)&mask)|clkh);
        outpdw(pDest, (*pDest&GPIO_SDCC_CLK_MASK_I));
        outpdw(pDest, ((data<<29)&mask)|((*pDest)&GPIO_SDCC_CMD_MASK_I));
        outpdw(pDest, ((data<<29)&mask)|clkh);
        pdata++;
    }
    
    // Reset CMD to 1
    outpdw(pDest, clkh|GPIO_SDCC_CMD_MASK);
}

static INLINE void sdcc_recv_cmd_bytes(byte *pdata, int len)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    register volatile byte *pIn = (volatile byte*)GPIO_SDCC_IN_ADDR;
    register byte data = 0;
    register byte mask;
    
    while(len--)
    {
        mask = 0x80;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data  = (inp(pIn)<<2)&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<1)&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)   )&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>1)&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>2)&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>3)&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>4)&mask;
        mask  >>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>5)&mask;
        *pdata++ = data;
    }
}

static INLINE byte sdcc_recv_cmd_byte_wait(void)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    register volatile byte *pIn = (volatile byte*)GPIO_SDCC_IN_ADDR;
    register byte data = 0;
    register byte mask;
    int i = SDCC_MIN_WAIT; //最大等待周期

    mask = 0x80;
    while(i--) // Ncr
    {
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data  = (inp(pIn)<<2)&mask;
        if(data == 0)
        {
            break;
        }
    }
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)<<1)&mask;
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)   )&mask;
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>1)&mask;
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>2)&mask;
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>3)&mask;
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>4)&mask;
    mask  >>= 1;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>5)&mask;
    return data;
}

static INLINE SDCC_STATUS sdcc_send_data_bytes(byte *pdata, int len)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    register uint32 clkm = clkh&GPIO_SDCC_DAT_0_MASK_I;
    register volatile byte *pIn = (volatile byte*)GPIO_SDCC_IN_ADDR;
    register byte data;
    register uint32 mask = 0x02000000;
    uint16 wCRC16 = crc_16_bytes(pdata, len);
    int i; //最大等待周期
    
    // Deley 2 Cycle -- Nwr
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    // START Bit
    outpdw(pDest, clkl);
    outpdw(pDest, clkm);
    dog_kick();
    // Data
    while(len--)
    {
        data = *pdata++;
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>6)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>5)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>4)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>3)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>2)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data<<23)&mask)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data<<24)&mask)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data<<25)&mask)|clkm);
    }
    dog_kick();
    // CRC
    len = 2;
    data = (byte)(wCRC16>>8);
    while(len--)
    {
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>6)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>5)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>4)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>3)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data>>2)<<24)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data<<23)&mask)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data<<24)&mask)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data<<25)&mask)|clkm);
        data = (byte)wCRC16;
    }
    
    // END Bit
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);

    // 2 delay Cycles, HOST control
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    // Check CRC status
    gpio_tlmm_config(GPIO_SDCC_DAT_0_IN);
    
    // START Bit
    i = SDCC_MIN_WAIT;
    while(i)
    {
        outpdw(pDest, clkl);
        i--;
        outpdw(pDest, clkh);
        if((inp(pIn) & 0x02) == 0)
        {
            break;
        }
    }
    
    // CRC status
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data  = (inp(pIn)<<1)&0x04;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)   )&0x02;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>1)&0x01;
    
    // END Bit
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    if(data != 0x2)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        return SDCC_ERR_DATA_CRC_FAIL;
    }
    
    // Busy Start bit
    i = SDCC_MIN_WAIT;
    while(i)
    {
        outpdw(pDest, clkl);
        i--;
        outpdw(pDest, clkh);
        if(!(inp(pIn) & 0x02))
        {
            break;
        }
    }
    
    if(i == 0)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        return SDCC_NO_ERROR;
    }
    dog_kick();
    // Wait until write finished
    i = SDCC_MAX_WAIT;
    while(i)
    {
        outpdw(pDest, clkl);
        i--;
        outpdw(pDest, clkh);
        if(inp(pIn)&0x02)
        {
            // Busy End bit
            break;
        }
    }
    dog_kick();
    if(i == 0)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        return SDCC_ERR_DATA_TIMEOUT;
    }
    
    gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
    return SDCC_NO_ERROR;
}

static INLINE SDCC_STATUS sdcc_recv_data_bytes(byte *buff, int len)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    register volatile byte *pIn = (volatile byte*)GPIO_SDCC_IN_ADDR;
    register byte data;
    register byte mask;
    register byte *pdata = buff;
    int i;
    uint16 wCRC16 = 0;
    

    // Wait 2 Clock cycles
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    dog_kick();
    // START Bit
    i = SDCC_MIN_WAIT*4;
    while(i--)
    {
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        if((inp(pIn) & 0x02) == 0)
        {
            break;
        }
    }
    dog_kick();
    if(i == 0)
    {
        return SDCC_ERR_DATA_TIMEOUT;
    }
    
    // Data
    i = len;
    while(i--)
    {
        mask = 0x80;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data  = (inp(pIn)<<6)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<5)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<4)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<3)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<2)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<1)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)   )&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>1)&mask;
        *pdata++ = data;
    }
    dog_kick();
    // CRC16
    i = 2;
    while(i--)
    {
        mask = 0x80;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data  = (inp(pIn)<<6)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<5)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<4)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<3)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<2)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)<<1)&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)   )&mask;
        mask>>= 1;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>1)&mask;
        wCRC16 = (wCRC16<<8)|data;
    }
    
    // END Bit, Dont care
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    dog_kick();
    // CRC verify
    if(wCRC16 != crc_16_bytes(buff, len))
    {
        return SDCC_ERR_DATA_CRC_FAIL;
    }
    return SDCC_NO_ERROR;
}

static INLINE SDCC_STATUS sdcc_send_widedata_bytes(byte *pdata, int len)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    register uint32 clkm = clkh&GPIO_SDCC_DAT_MASK_I;
    register volatile byte *pIn = (volatile byte*)GPIO_SDCC_IN_ADDR;
    register byte data;
    register byte maskh = 0xF0,maskl=0x0F;
    uint64 wCRC64 = CRC16_4(pdata, len);
    int i;

    dog_kick();
    // Deley 2 Cycle -- Nwr
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    // START Bit
    outpdw(pDest, clkl);
    outpdw(pDest, clkm);
    
    while(len--)
    {
        data = *pdata++;
        outpdw(pDest, clkl);
        outpdw(pDest, ((data&maskh)<<21)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data&maskl)<<25)|clkm);
    }

    dog_kick();
    // CRC
    len = 8;
    pdata = (byte *)&wCRC64;
    pdata += 7;
    while(len--)
    {
        data = *pdata--;
        outpdw(pDest, clkl);
        outpdw(pDest, ((data&maskh)<<21)|clkm);
        outpdw(pDest, clkl);
        outpdw(pDest, ((data&maskl)<<25)|clkm);
    }
    
    // END Bit
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    // 2 delay Cycles, HOST Control
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    // Check CRC status
    gpio_tlmm_config(GPIO_SDCC_DAT_0_IN);
    
    // START Bit
    i = SDCC_MIN_WAIT;
    while(i)
    {
        outpdw(pDest, clkl);
        i--;
        outpdw(pDest, clkh);
        if((inp(pIn) & 0x02) == 0)
        {
            break;
        }
    }
    
    // CRC status
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data  = (inp(pIn)<<1)&0x04;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)   )&0x02;
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    data |= (inp(pIn)>>1)&0x01;

    // END Bit
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    if(data != 0x2)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        return SDCC_ERR_DATA_CRC_FAIL;
    }
    
    // Busy Start bit
    i = SDCC_MIN_WAIT;
    while(i)
    {
        outpdw(pDest, clkl);
        i--;
        outpdw(pDest, clkh);
        if(!(inp(pIn) & 0x02))
        {
            break;
        }
    }
    
    if(i == 0)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        return SDCC_NO_ERROR;
    }
    dog_kick();
    // Wait until write finished
    i = SDCC_MAX_WAIT;
    while(i)
    {
        outpdw(pDest, clkl);
        i--;
        outpdw(pDest, clkh);
        if(inp(pIn)&0x02)
        {
            // Busy End bit
            break;
        }
    }
    dog_kick();
    if(i == 0)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        return SDCC_ERR_DATA_TIMEOUT;
    }
    
    gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
    return SDCC_NO_ERROR;
}

static INLINE SDCC_STATUS sdcc_recv_widedata_bytes(byte *buff, int len)
{
    register volatile uint32 *pDest = (volatile uint32*)GPIO_SDCC_OUT_ADDR;
    register uint32 clkl = (*pDest)&GPIO_SDCC_CLK_MASK_I, clkh = (*pDest)|GPIO_SDCC_CLK_MASK;
    register volatile byte *pIn = (volatile byte*)GPIO_SDCC_IN_ADDR;
    register byte data;
    register byte maskh = 0xF0,maskl=0x0F;
    register byte *pdata = buff;
    int i;
    uint64 wCRC64 = 0;

    dog_kick();
    // START Bit
    i = SDCC_MIN_WAIT*4;
    while(i--)
    {
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        if((inp(pIn) & 0x1E) == 0)
        {
            break;
        }
    }
    dog_kick();
    if(i == 0)
    {
        return SDCC_ERR_DATA_TIMEOUT;
    }
    
    // Data
    i = len;
    while(i--)
    {
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data  = (inp(pIn)<<3)&maskh;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>1)&maskl;
        *pdata++ = data;
    }
    dog_kick();
    i = 8;
    while(i--)
    {
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data  = (inp(pIn)<<3)&maskh;
        outpdw(pDest, clkl);
        outpdw(pDest, clkh);
        data |= (inp(pIn)>>1)&maskl;
        wCRC64 = (wCRC64<<8)|data;
    }

    // END Bit, Dont care
    outpdw(pDest, clkl);
    outpdw(pDest, clkh);
    
    // CRC verify
    if(wCRC64 != CRC16_4(buff, len))
    {
        return SDCC_ERR_DATA_CRC_FAIL;
    }
    return SDCC_NO_ERROR;
}

SDCC_STATUS sdcc_write_data(byte *buff, uint16 length)
{
    SDCC_STATUS rc = SDCC_NO_ERROR;
    uint32 blksize = sdcc_getblksize(length);
    
    if(NULL == buff)
    {
        return(SDCC_ERR_WRITE_FIFO);
    }

    if(length < blksize)
    {
        blksize = length;
    }
    
    if(sdcc_pdata.wide_bus)
    {
        while(length)
        {
            rc = sdcc_send_widedata_bytes(buff, blksize);
            length -= blksize;
            buff += blksize;
            if(rc != SDCC_NO_ERROR)
            {
                break;
            }
        }
    }
    else
    {
        while(length)
        {
            rc = sdcc_send_data_bytes(buff, blksize);
            length -= blksize;
            buff += blksize;
            if(rc != SDCC_NO_ERROR)
            {
                break;
            }
        }
    }
    
    sdcc_clock_out(8);
    return rc;
}

SDCC_STATUS sdcc_read_data(byte *buff, uint16 length)
{
    SDCC_STATUS rc = SDCC_NO_ERROR;
    uint32 blksize = sdcc_getblksize(length);
    
    if(!buff)
    {
        return(SDCC_ERR_READ_FIFO);
    }

    if(length < blksize)
    {
        blksize = length;
    }
    
    gpio_tlmm_config(GPIO_SDCC_CMD_IN);
    if(sdcc_pdata.wide_bus)
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_IN);
        gpio_tlmm_config(GPIO_SDCC_DATOUT_1_IN);
        gpio_tlmm_config(GPIO_SDCC_DATOUT_2_IN);
        gpio_tlmm_config(GPIO_SDCC_DATOUT_3_IN);
        while(length)
        {
            rc = sdcc_recv_widedata_bytes(buff, blksize);
            length -= blksize;
            buff += blksize;
            if(rc != SDCC_NO_ERROR)
            {
                break;
            }
        }
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
        gpio_tlmm_config(GPIO_SDCC_DATOUT_1_OUT);
        gpio_tlmm_config(GPIO_SDCC_DATOUT_2_OUT);
        gpio_tlmm_config(GPIO_SDCC_DATOUT_3_OUT);
    }
    else
    {
        gpio_tlmm_config(GPIO_SDCC_DAT_0_IN);
        while(length)
        {
            rc = sdcc_recv_data_bytes(buff, blksize);
            length -= blksize;
            buff += blksize;
            if(rc != SDCC_NO_ERROR)
            {
                break;
            }
        }
        gpio_tlmm_config(GPIO_SDCC_DAT_0_OUT);
    }
    gpio_tlmm_config(GPIO_SDCC_CMD_OUT);
    sdcc_clock_out(8);
    return rc;
}

uint16 sdcc_getblksize(uint16 units)
{
  uint16    blksz       = 0;
  
    /* Note:
       1. data size in byte_mode should be the power of 2
       2. DMA doesn't work for small data size
       3. units: number of bytes in byte mode
                 number of blocks in block mode
    */
  if(SDCC_SDIO_BYTE_MODE == sdcc_pdata.block_mode ||
     SDCC_MEM_BYTE_MODE == sdcc_pdata.block_mode)
  {
    /* set data length */
    blksz = units;
  }
  else
  {
    if(SDCC_CARD_SDIO == sdcc_pdata.card_type)
    {
        blksz = sdcc_pdata.io.fn_blksz;
    }
    else
    {
        blksz = sdcc_pdata.mem.block_len;
    }
  }
  
  return blksz;

}/* sdcc_getblksize */ 

#endif

