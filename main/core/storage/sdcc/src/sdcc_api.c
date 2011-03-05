/**********************************************************************
 * sdcc_api.c
 *
 * SDCC(Secure Digital Card Controller) driver API.
 *
 * This file implements the SDCC driver APIs. 
 *
 * Copyright (c) 2004-2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_api.c#5 $
$DateTime: 2009/04/13 12:36:36 $ $Author: vink $

when       who     what, where, why
--------   -------------------------------------------------------------
2009-04-10 vin     Fixed sdcc_read_serial
2008-10-31 vin     Support for SDCC power collapse
2008-09-15 vin     Added power control to SDCC on ULC FFA
2008-06-19 vin     Added critical section locking
2008-06-19 vin     Removed SDCC Mux features
03/05/08   sc      Removed SDCC_PARAM_NOT_REF macro
01/21/08   sc      Configured MCI_DATA_CTL with block size and added
                   sdcc_set_tcxo_clk in sdcc_write
10/15/07   umr     Changed MPP4 from HIGH to a Tristate as per FET requirement.
09/26/07   dng     Added the MPP4 shutdown to sdcc_close function to fix
                   SC2X power consumption issue.
08/29/07   dng     Changed the MMC_VREG voltage level back to 3.05 to fix SDIO issues.
08/02/07   vin     Added feature FEATURE_SDCC_WLAN_CONFIG_API
07/25/07   dng     Added retry to both sdcc read/write functions.
                   Changed the voltage setting of the MMC_VREG from 3050
                   to 2850.
04/11/07   sc      Ported in MMCPlus support
03/16/07   dng     Added the new PMIC call to turn on SDCC for SC2X target.
03/13/07   vin     Changed sdcc_read & sdcc_write to handle n_sectors > 127
                   Now using FIFO instead of returning failure
03/12/07   vin     Added FEATURE_WLAN_MODIFIED_FFA
02/20/07   dng     Called the new PMIC function to turn on SDCC pwr for SC2X target.
02/15/07   sc      Replaced the banned API and fixed lint errors
11/28/06   dng     Added the temp buffer copy for the M6800 if the buffer
                   is not 32 bit aligned.
11/28/06   dng     Moved the CLK_REGIME defines to the sdcc_util.h
11/07/06   dng     Added prog_done ISR for sdcc read/write
11/07/06   dng     Added the checking for maximum no of blocks
                   for sdcc read/write.
10/19/06   dng     Changed the sdcc_init function to turn
                   on sdcc power.
09/19/06   dng     Made the FEATURE_SDCC_FIX_FOR_EXTR_10MA_ON_TCXO generic
                   so all targets can pick this feature.
09/08/06   dng     Modified the sdcc_get_serial function to remove
                   some obsolete code.
09/07/06   dng     Added SDCC/SDIO Mux feature.
09/05/06   dng     Changed the sdcc_polling_status to CMD ISR .
08/23/06   dng     Added the check in sdcc_close function to make sure it does
                   not turn off the sdcc controller twice.
07/17/06   sc      Added SD2.0 feature
07/17/06   dng     First pass of code review clean up.
06/07/06   dng     Added the card status check in the sdcc_read and sdcc_write
                   functions.  
05/25/06   dng     Changed the sequence in sdcc_read..Sent the command first
                   then turn on the DMA.  The new sequence fixed some of the
                   slow sd cards.
05/22/06   dng     Fixed Lint error for M6280 target.
05/09/06   dng     Added sdcc write retry to the sdcc_write function.
04/28/06   dng     Fixed the typo error on the sdcc_write function..
                   for M6280 target.
04/19/06   dng     Fixed the typo error on the sdcc_read function.
04/18/06   dng     In sdcc_read turn on the DMA first before sending the cmd.
03/02/06   dng     Removed all the time delay in the sdcc_write function.
                   This change only applies for the M6280 target.
01/26/06   dng     Added a temp buffer to use to work around the 4 bytes buffer  
                   alignment in the SDCC_WRITE function.  This work around
                   only applies for the MSM6280 target.
01/24/06   dng     Added more timeout when the sdcc_write is in multiblock mode.
                   This change only applies for the M6280 target only.
01/20/06   dng     For the m6280 target in the function sdcc_write
                   added 1ms delay after turn on the DMA.
01/17/06   dng     Changed the logic of when to turn on the DMA for
                   both sdcc_read and sdcc_write functions.
                   Turned the DMA on only if the cmds sent to the controller
                   are successful.
01/09/06   dng     Fixed the function sdcc_complete_data_xfer.
                   The new process for the function is as following:
                   If this is multi-block data transfer, send CMD12.
                   If this is single-block data transfer, 
                   wait for the card to be ready if this is a write. 
                   Do nothing if it is a read.
12/01/05   hwu     Close the controller if no device attached.
11/11/05   hwu     Ported for MSM6275.
11/10/05   hwu     Fixed the lint errors, not warning this time.
11/03/05   hwu     Changed to return the status code for internal functions 
                   and booleans for APIs.
08/28/05   hwu     General cleanups.
07/24/05   hwu     Added SDCC_GET_TYPE to pass up the card type.
06/07/05   hwu     Seperated out sdcc_complete_data_xfer().
                   Seperated out sdcc_process_dma().
05/07/05   hwu     Enabled DMA interrupt for DMA operation.
04/14/05   hwu     Changed to use sdcc_setup_data_xfer.
                   Moved block_mode and enable_dma to sdcc_data_type.
03/25/05   hwu     Featurized WLAN in sdcc_init().
02/15/05   hwu     added delay after switch on the power. PHG needs 5msec.
10/29/04   hwu     new release for MSM6550 v1.2 chipset. Turn on write DMA.
07/14/04   hwu     Added DMA support for dword aligned read.
                   Cleaned up some logic.
06/23/04   hwu     Changed to use TCXO for SD, TCXO/2 for MMC.(HW issue.)
                   Limited DMA usage to read only.(HW bug.)
                   Added support for single block only mode.
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
#ifndef FEATURE_DSP
#include "sdcc_util.h"
#include "sdcc_mmc_util.h"
#include "assert.h"
#include "sdcc_bsp.h"

extern unsigned int sdio_card;

#if defined(FEATURE_SDCC_FIX_FOR_EXTRA_10MA_ON_TCXO)                            
 #define PULL_DOWN ~GPIO_PULL_MASK | (GPIO_PULL_DOWN << GPIO_PULL_POSITION)
#endif
#else
#include "cam_module.h"
extern ait_sd_func AIT701_SD_Func;
static  boolean m_binit = TRUE;
#endif

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_open.
*
* Description:
*    This function returns failure if there is no memory device
*    attached. If there is a device, it then will initialize the device
*    and take it out of the identification state. It then sets up the
*    device for data transfer.

* Arguments:
*       driveno        Drive Number
*
* Returns:
*       TRUE  if sucessfully configured a SD/MMC device
*       FALSE if failed
*
******************************************************************************/
boolean
sdcc_open(int16 driveno)
{
#ifndef FEATURE_DSP
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   boolean      close_drive = FALSE;

   /*-------------------------------------------------------------------*/
   sdcc_enter_crit_sect();
   
   (void)sdcc_bsp_vdd_control(SDCC_BSP_VDD_ON);
   
   do
   {
      if(SDCC_CARD_SDIO == sdcc_pdata.card_type)
      { 
         rc = SDCC_ERR_SDIO;
         break;
      }

      /* need to check 4 types of card: SD, SDHC, MMC or MMCHC */
      /* and return if it has been initialized */
      if(( SDCC_CARD_SD   == sdcc_pdata.card_type ) ||
         ( SDCC_CARD_SDHC == sdcc_pdata.card_type ) ||
         ( SDCC_CARD_MMC  == sdcc_pdata.card_type ) ||
         ( SDCC_CARD_MMCHC == sdcc_pdata.card_type ))
      {
         rc = SDCC_NO_ERROR;
         break;
      }
#ifdef T_QSC1110
      /* enable the clock for the SD mode*/
      HWIO_OUTM(MCI_CLK,
                HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
                MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE);
#endif
      /* find the attached device */
      sdcc_pdata.card_type = sdcc_find_card();

      /* return and close controller if none attached */
      if( SDCC_CARD_UNKNOWN == sdcc_pdata.card_type )
      {
         TPRINTF(SDCC_MSG_LEVEL,("No device attached\n"));
         rc = SDCC_ERR_UNKNOWN;
         close_drive = TRUE;
         break;
      }

      /* return if SDIO device attached */
      if ( SDCC_CARD_SDIO == sdcc_pdata.card_type )
      {
         DPRINTF(("Found SDIO device.\n"));
         rc = SDCC_ERR_SDIO;
         break;
      }
  
      /* proceed if this is SD/MMC device */
      /* finish identification mode */
      rc = sdcc_init_memory_device();

      if(SDCC_NO_ERROR != rc)
      {
          close_drive = TRUE;
          break;
      }
 
      /* re-program the clock for data transfer */
      sdcc_config_clk(SDCC_DATA_TRANSFER_MODE, sdcc_pdata.card_type);
#ifdef T_QSC1110
      HWIO_OUTM(MCI_CLK,
                HWIO_FMSK(MCI_CLK, FLOW_ENA),
                MCI_CLK_SET_FLOW);
#endif
      /* get memory device info  */
      rc = sdcc_get_memory_info();
      if(SDCC_NO_ERROR != rc)
      {
          close_drive = TRUE;
          break;
      }

      /* config device for data transfer */
      rc = sdcc_config_memory_device();

      if(SDCC_NO_ERROR != rc)
      {
          close_drive = TRUE;
          break;
      }
#ifdef T_QSC1110
      /* Enable the DMA interrupt */
      sdcc_enable_int(MCI_INT_MASK_DMA_DONE |
                      MCI_INT_MASK_PROG_DONE); 
#endif
      /* Set the host state */
      sdcc_pdata.host_state = SDCC_HOST_TRAN;

      /* config MMC modes segment after enabling the DMA interrupt */
      if ( SDCC_CARD_MMC == sdcc_pdata.card_type ||
           SDCC_CARD_MMCHC == sdcc_pdata.card_type )
      {
         if(SDCC_NO_ERROR != sdcc_config_mmc_modes_segment())
         {
            close_drive = TRUE;
         }
      }
   }
   while (0);
   if(TRUE == close_drive)
   {
      sdcc_pdata.errno     = rc;
      sdcc_pdata.card_type = SDCC_CARD_UNKNOWN;
      (void) sdcc_close(driveno);
   }
   sdcc_leave_crit_sect();
   return(rc ? FALSE : TRUE);
#else
   boolean bRet = TRUE;
   
   return bRet;
#endif
}/* sdcc_open */


/******************************************************************************
* Name: sdcc_read
*
* Description:
*    This function reads n_sectors blocks of data from the s_sector in
*    the device, storing them at the memory location given by buff.
*
* Arguments:
*       driveno    Drive Number
*       s_sector   start of the sector
*       buff       pointer to buffer for the incoming data
*       n_sector   number of sectors to be read
*
* Returns:
*       TRUE  if sucessful
*       FALSE if failure
*
* Note:
*    Multiblock transfer is used when more than 1 block data is
*    transfered. DMA  mode is used only when the buff address is
*    dword aligned.
*
******************************************************************************/
boolean
sdcc_read
(
int16      driveno,
uint32     s_sector,
byte      *buff,
uint16     n_sectors
)
{
#ifndef FEATURE_DSP
#ifdef T_QSC1110
   uint8            data_ctrl   = 0;
#endif
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS      rc          = SDCC_NO_ERROR;
   SDCC_STATUS      rc_complete = SDCC_NO_ERROR;
   uint32           length      = 0;
   boolean          multi_block = FALSE;
#ifdef T_QSC1110
   boolean          use_dma     = FALSE;
#endif
   boolean          ret_status  = FALSE;
   uint8            sdcc_read_retry_cnt =0;
#ifdef T_QSC1110
   uint32           blk_in_bits = 0;
#endif
   (void)driveno;
   if (sdcc_pdata.slot_state_changed)
      return FALSE;

   sdcc_enter_crit_sect();
   do
   {
      if(SDCC_HOST_TRAN != sdcc_pdata.host_state)
      {
         if( FALSE == sdcc_silent_reinit())
         {
            ret_status = FALSE;
            break;
         }
      }

      /*********************************************************************
      * Check the card status first to make sure that sdcc controller
      * has been turned on properly..
      ********************************************************************/
      if (sdcc_pdata.card_type ==  SDCC_CARD_UNKNOWN)
      {
         ret_status = FALSE;
         break;
      }

/* Sdcc read retry per San disk recommendation. */
SDCC_READ_RETRY:

      /* set up the data transfer */
      length = sdcc_setup_data_xfer(SDCC_DATA_READ, n_sectors);
      if (!length)
      {
         sdcc_pdata.errno = SDCC_ERR_READ_FIFO;
         ret_status = FALSE;
         break;
      }

#ifdef T_QSC1110
      /* turn on DMA only if the passed in address is dword aligned */
      if ((sdcc_pdata.enable_dma) && IS_ALIGNED(buff) && (n_sectors <= SDCC_MAX_NO_BLOCKS))
      {
         use_dma = TRUE;
         /* make sure to sync with the DMA memory */
         mmu_invalidate_data_cache_lines((uint32 *)buff, length); //lint !e826
         data_ctrl = MCI_DATA_DMA_ENABLED;
         HWIO_OUT(MCI_START_ADDR, (uint32)buff);
      }
      else
      {
         DPRINTF(("sdcc_read buffer not aligned use FIFO\n"));
      }

      blk_in_bits = sdcc_blk_in_bits(sdcc_pdata.mem.block_len);
      /* Refer to MCI_DATA_CTL register for the configuration in the target */
      /* specific Software Interface Manual */
      data_ctrl |= (blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT);
      data_ctrl |= MCI_DATA_DPSM_ENABLED | MCI_DATA_READ;        
#endif
      /* set the command index */
      if ( 1 == n_sectors )
         sdcc_cmd.cmd = SD_CMD17_READ_BLOCK;
      else
      {
         sdcc_cmd.cmd = SD_CMD18_READ_MULTIPLE_BLOCK;
         multi_block  = TRUE;
      }

      /* CMD17/CMD18 */
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      if ( SDCC_CARD_SDHC == sdcc_pdata.card_type ||
           SDCC_CARD_MMCHC == sdcc_pdata.card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg   = s_sector;
      }
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = s_sector * (sdcc_pdata.mem.block_len); 
      }

      /* send out CMD17/CMD18 */
      rc = sdcc_command(&sdcc_cmd);

      /* Wait for the sdcc read read */
      if (rc == SDCC_NO_ERROR)
      {
#ifdef T_QSC1110
         HWIO_OUT(MCI_DATA_CTL, data_ctrl);

         /* process read data */
         rc = ( TRUE == use_dma) ? sdcc_process_interrupts(&sdcc_cmd) :
                                   sdcc_read_fifo(buff, length);
#else
         rc = sdcc_read_data(buff, length);
#endif
      }
      else
      {
         DPRINTF(("sdcc_read sent cmd failed\n"));
      }

      /* Need to set the controller back to ready state */
      rc_complete = sdcc_complete_data_xfer(SDCC_DATA_READ, multi_block);

      if ((SDCC_NO_ERROR != rc) || (SDCC_NO_ERROR != rc_complete))
      {
         if (sdcc_read_retry_cnt < SDCC_RETRY_CNT)
         {
            sdcc_read_retry_cnt++;
            goto SDCC_READ_RETRY;
         }
         else
         {
            DPRINTF(("sdcc_read failed\n"));
            ret_status = FALSE;
         }    
      }
      else
      {
         ret_status = TRUE;
      }
      sdcc_pdata.errno = rc;
   }
   while (0);
   sdcc_leave_crit_sect();
   return(ret_status);
#else 
   uint16 nRet;
   sdcc_enter_crit_sect();
   do
   {
      /******************************************************************
      * Check the card status first to make sure that sdcc controller
      * has been turned on properly..
      ********************************************************************/
      if (sdcc_pdata.card_type ==  SDCC_CARD_UNKNOWN)
      {
         nRet = SDCC_ERR_CARD_READY;
         break;
      }
      
      nRet = AIT701_SD_Func.ait_sd_read(s_sector,0,buff,n_sectors*sdcc_pdata.mem.block_len);
   }while(0);
   
   if(nRet == 0)
   {
      sdcc_pdata.errno = SDCC_NO_ERROR;
   }
   else
   {
      sdcc_pdata.errno = nRet;
   }
   sdcc_leave_crit_sect();
   return (nRet == 0)?TRUE:FALSE;
#endif 
}/* sdcc_read */

/******************************************************************************
* Name: sdcc_write.
*
* Description:
*    This function writes n_sectors blocks of data, obtained from the
*    memory location buff, to the s_sector on the device.
*
* Arguments:
*       driveno     Drive Number
*       s_sector    start of the sector
*       buff        pointer to buffer for the outgoing data
*       n_sector    number of sectors to be written
*
* Returns:
*       TRUE  if sucessful
*       FALSE if failure
*
* Note:
*    Multiblock transfer is used when more than 1 block data is
*    transfered. DMA  mode is used only when the buff address is
*    dword aligned.
******************************************************************************/
boolean
sdcc_write
(
   int16      driveno,
   uint32     s_sector,
   byte      *buff,
   uint16     n_sectors
)
{
#ifndef FEATURE_DSP
#ifdef T_QSC1110
   uint8            data_ctrl   = 0;
#endif
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS      rc          = SDCC_NO_ERROR;
   SDCC_STATUS      rc_complete = SDCC_NO_ERROR;
   uint32           length      = 0;
#ifdef T_QSC1110
   boolean          use_dma     = FALSE;
#endif
   boolean          multi_block = FALSE;
   boolean          ret_status  = FALSE;
   uint8            sdcc_write_retry_cnt = 0;
#ifdef T_QSC1110
   uint32           blk_in_bits = 0;
#endif
   (void)driveno;

   if (sdcc_pdata.slot_state_changed)
      return FALSE;

   sdcc_enter_crit_sect();
   do
   {
      if(SDCC_HOST_TRAN != sdcc_pdata.host_state)
      {
         if( FALSE == sdcc_silent_reinit())
         {
            ret_status = FALSE;
            break;
         }
      }

      /******************************************************************
      * Check the card status first to make sure that sdcc controller
      * has been turned on properly..
      ********************************************************************/
      if (sdcc_pdata.card_type ==  SDCC_CARD_UNKNOWN)
      {
         ret_status = FALSE;
         break;
      }

SDCC_WRITE_RETRY:

      /* setup the data transfer */
      length = sdcc_setup_data_xfer(SDCC_DATA_WRITE,
                                    n_sectors);
      if (!length)
      {
         sdcc_pdata.errno = SDCC_ERR_WRITE_FIFO;
         break;
      }

      /* set the command index */
      if ( n_sectors ==1 )
      {
         sdcc_cmd.cmd = SD_CMD24_WRITE_BLOCK;
      }
      else
      {
         sdcc_cmd.cmd = SD_CMD25_WRITE_MULTIPLE_BLOCK;
         multi_block  = TRUE;
      }

      /* CMD25/CMD24 */
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;

      if ( SDCC_CARD_SDHC == sdcc_pdata.card_type ||
           SDCC_CARD_MMCHC == sdcc_pdata.card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg   = s_sector;
      }
#ifdef CUST_EDITION
      else if(SDCC_CARD_SDIO == sdcc_pdata.card_type)
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = s_sector * (sdcc_pdata.io.fn_blksz);
      }
#endif
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = s_sector * (sdcc_pdata.mem.block_len);
      }

      /* send out CMD25/CMD24 */
      rc = sdcc_command(&sdcc_cmd);
#ifdef T_QSC1110
      /* not to turn on DMA in R1_V1 */
#ifdef T_MSM6550
      if (!(HW_MSM6550_HAS_R1_V1))
#endif
      {
         /* turn on DMA only if the passed in address is dword aligned and if n_sectors is 127 or lesser*/
         if ((sdcc_pdata.enable_dma) && IS_ALIGNED(buff) && (n_sectors <= SDCC_MAX_NO_BLOCKS))
         {
            use_dma = TRUE;
            data_ctrl = MCI_DATA_DMA_ENABLED;
            HWIO_OUT(MCI_START_ADDR, (uint32)buff);
         }
         else
         {
            DPRINTF(("sdcc_write buffer not aligned use FIFO\n"));
         }
      }

      blk_in_bits = sdcc_blk_in_bits(sdcc_pdata.mem.block_len);
      /* Refer to MCI_DATA_CTL register for the configuration in the target */
      /* specific Software Interface Manual */
      data_ctrl |= (blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT);
      /* Setup the DPSM */
      data_ctrl |= MCI_DATA_DPSM_ENABLED | MCI_DATA_WRITE;
#endif
      /* Turn on the DMA controller*/
      if (rc == SDCC_NO_ERROR)
      {
#ifdef T_QSC1110
         HWIO_OUT(MCI_DATA_CTL, data_ctrl);

         /* process write data */
         rc = ( TRUE == use_dma ) ? sdcc_process_interrupts(&sdcc_cmd) :
              sdcc_write_fifo(buff, length);
#else
         rc = sdcc_write_data(buff, length);
#endif
         /* Need to set the controller back to ready state */
         rc_complete = sdcc_complete_data_xfer(SDCC_DATA_WRITE, multi_block);
      }
      
      /* Check for the errors */
      if ((SDCC_NO_ERROR != rc) || (SDCC_NO_ERROR != rc_complete))
      {

         if (sdcc_write_retry_cnt < SDCC_RETRY_CNT)
         {
            sdcc_write_retry_cnt++;
            goto SDCC_WRITE_RETRY;
         }
         else
         {
            DPRINTF(("sdcc_write failed\n"));
            ret_status= FALSE;
         }

         /* pull TCXO clock low to save power */
         sdcc_set_tcxo_clk(FALSE);
      }
      else
      {
         ret_status= TRUE;
      }
      sdcc_pdata.errno = rc;
   }
   while (0);
   sdcc_leave_crit_sect();
   return(ret_status);
#else
   uint16 nRet;
   sdcc_enter_crit_sect();
   do
   {
      /******************************************************************
      * Check the card status first to make sure that sdcc controller
      * has been turned on properly..
      ********************************************************************/
      if (sdcc_pdata.card_type ==  SDCC_CARD_UNKNOWN)
      {
         nRet = SDCC_ERR_CARD_READY;
         break;
      }
      
      nRet = AIT701_SD_Func.ait_sd_write(s_sector,0,buff,n_sectors*sdcc_pdata.mem.block_len);
   }while(0);
   
   if(nRet == 0)
   {
      sdcc_pdata.errno = SDCC_NO_ERROR;
   }
   else
   {
      sdcc_pdata.errno = nRet;
   }
   sdcc_leave_crit_sect();
   return (nRet == 0)?TRUE:FALSE;
#endif
}/* sdcc_write */


/******************************************************************************
* Name: sdcc_ioctl
*
* Description:
*    This function is to get various information about the memory device.
*
* Arguments:
*       driveno       Drive Number
*       what          the field to query
*       data          the returned data
*
* Returns:
*       YES if sucessful
*       NO if failure
*
******************************************************************************/
uint32
sdcc_ioctl
(
int16    driveno,
uint8    what
)
{
#ifndef FEATURE_DSP
   uint32 value = (uint32)SDCC_NO_ERROR;
   (void)driveno;

   if (sdcc_pdata.slot_state_changed)
      return SDCC_CARD_UNKNOWN;

   sdcc_enter_crit_sect();

   switch (what)
   {
      case SDCC_CARD_STATE:
      {
         SDCC_CARD_STATUS  card_status;

         card_status = sdcc_send_status();

         if (SDCC_CARD_IGNORE == card_status)
         {
            sdcc_pdata.card_type = SDCC_CARD_UNKNOWN;
            value = (uint32)SDCC_ERR_UNKNOWN;
         }
         break;
      }

      case SDCC_GET_TYPE:
         value = (uint32)sdcc_pdata.card_type;
         break;

      case SDCC_GET_ERROR:
         value = (uint32)sdcc_pdata.errno;
         break;

      case SDCC_CARD_SIZE:
         value = sdcc_pdata.mem.card_size;
         break;

      case SDCC_BLOCK_LEN:
         value = sdcc_pdata.mem.block_len;
         break;

      default:
         DPRINTF(("Invalid request:(0x%x)?", what));
         value = (uint32)SDCC_ERR_UNKNOWN;
         break;
   }
   sdcc_leave_crit_sect();
   return value;
#else
   uint32 value = (uint32)SDCC_NO_ERROR;
   (void)driveno;

   if (sdcc_pdata.slot_state_changed)
      return SDCC_CARD_UNKNOWN;

   sdcc_enter_crit_sect();

   switch (what)
   {
      case SDCC_CARD_STATE:
         value = SDCC_NO_ERROR;
         break;
         
      case SDCC_GET_TYPE:
         value = (uint32)sdcc_pdata.card_type;
         break;
        
      case SDCC_GET_ERROR:
         value = (uint32)sdcc_pdata.errno;
         break;

      case SDCC_CARD_SIZE:
         value = sdcc_pdata.mem.card_size;
         break;

      case SDCC_BLOCK_LEN:
         value = sdcc_pdata.mem.block_len;;
         break;

      default:
         DPRINTF(("Invalid request:(0x%x)?", what));
         value = (uint32)SDCC_ERR_UNKNOWN;
         break;
   }
   sdcc_leave_crit_sect();
   return value;
#endif
}/* sdcc_ioctl */

/******************************************************************************
* Name: sdcc_read_serial
*
* Description:
*    This function is to read some drive information for SFAT..
*
* Arguments:
*       driveno       Drive Number
*       iDrvPtr       Pointer to the drive geometry descriptor that is
*                     to be filled 
*
* Returns:
*       TRUE   if sucessful
*       FALSE  if failure
*
******************************************************************************/
boolean
sdcc_read_serial
(
   uint16             driveno,
   DRV_GEOMETRY_DESC *iDrvPtr
)
{
   if (sdcc_pdata.slot_state_changed)
      return FALSE;

   (void)driveno;
   sdcc_enter_crit_sect();

   iDrvPtr->dfltCyl          = 0;     
   iDrvPtr->dfltHd           = 0; 
   iDrvPtr->dfltSct          = 0;

   iDrvPtr->totalLBA         = sdcc_pdata.mem.card_size;
   iDrvPtr->dfltBytesPerSect = sdcc_pdata.mem.block_len;

   iDrvPtr->serialNum[0]     = sdcc_pdata.mem.psn;

   memcpy(iDrvPtr->modelNum, sdcc_pdata.mem.pnm, sizeof(sdcc_pdata.mem.pnm));
   sdcc_leave_crit_sect();
   return TRUE;
}/* sdcc_read_serial */


/******************************************************************************
* Name: sdcc_init
*
* Description:
*    This function initializes the SDCC controller. It first turns on
*    the clock, configures the GPIOs and then turns on the power supply
*    to the device. It also initializes the related software data
*    structures.
*
* Returns:
*    Returns TRUE 
******************************************************************************/
boolean
sdcc_init(void)
{
   /* Its OK to re-initialize the sdcc critical section */
#ifdef CUST_EDITION
   boolean bRet = FALSE;
#endif
   sdcc_init_crit_sect();
   sdcc_enter_crit_sect();
#ifndef FEATURE_DSP
   do
   {
      /* The controller already turned on */
      if ((sdcc_pdata.host_state  == SDCC_HOST_IDENT) ||
          (sdcc_pdata.host_state  == SDCC_HOST_TRAN))
         break;

      sdcc_pdata.curr_sd_drv = 0;
      /* Turn on the SDCC clock */
      sdcc_config_clk(SDCC_INIT_MODE, SDCC_CARD_UNKNOWN);
#ifdef T_QSC1110
      /* Re-configure the GPIO pins for SDCC */
#ifdef FEATURE_6275_COMPILE
      gpio_config(SDCC_CLK);
      gpio_config(SDCC_CMD);
      gpio_config(SDCC_DAT_0);
      gpio_config(SDCC_DATOUT_1);
      gpio_config(SDCC_DATOUT_2);
      gpio_config(SDCC_DATOUT_3);
#else
      gpio_tlmm_config(SDCC_CLK);
      gpio_tlmm_config(SDCC_CMD);
      gpio_tlmm_config(SDCC_DAT_0);
      gpio_tlmm_config(SDCC_DATOUT_1);
      gpio_tlmm_config(SDCC_DATOUT_2);
      gpio_tlmm_config(SDCC_DATOUT_3);
#endif

      /* initialize the controller */
      HWIO_OUT(MCI_CMD, 0);
      HWIO_OUT(MCI_DATA_CTL, 0);
      HWIO_OUT(MCI_CLEAR, MCI_STATUS_STATIC_MASK);
      HWIO_OUT(MCI_CLEAR, HWIO_MCI_CLEAR_SDIO_INTR_CLR_BMSK);
      HWIO_OUTI(MCI_INT_MASKn, 0, 0);
      HWIO_OUTI(MCI_INT_MASKn, 1, 0);
#endif
      /* Initializes the sdcc_pdata structure */
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
      if (sdcc_pdata.is_wlan)
      {
         sdcc_pdata.card_type = 0;
         sdcc_pdata.host_state = 0;
         sdcc_pdata.rca[0] = 0;
         sdcc_pdata.rca[1] = 0;
         sdcc_pdata.errno = 0;
#ifdef T_QSC1110
         sdcc_pdata.enable_dma = 0;
#else
         sdcc_pdata.wide_bus = 0;
#endif
         sdcc_pdata.block_mode = 0;
#ifdef T_QSC1110
         sdcc_pdata.sdcc_tcb = 0;
#endif
         sdcc_pdata.status = 0;
#ifdef T_QSC1110
         memset(&(sdcc_pdata.sdcc_dma_timer),0,sizeof(rex_timer_type));
#endif
         memset(&(sdcc_pdata.mem),0,sizeof(sdcc_mem_type));
         memset(&(sdcc_pdata.io),0,sizeof(sdcc_io_type));
         sdcc_pdata.curr_sd_drv = 0;
         /* is_wlan is initialized in sdcc_sdio_init */
         /* sdcc_wlan_params is initialized in sdcc_sdio_set*/
      }
      else
      {
         memset(&sdcc_pdata, 0, sizeof(sdcc_data_type));
         //if hotplug comes in first, vreg needs to be initialized
         sdcc_pdata.sdcc_wlan_params.vreg = PM_VREG_MMC_ID;
      }
#else
      memset(&sdcc_pdata, 0, sizeof(sdcc_data_type));
#endif

      /* configure the controller */
      sdcc_pdata.host_state  = SDCC_HOST_IDENT;
#ifdef T_QSC1110
      if (sdio_card == SDIO_ATHEROS_MANFID)
      {
         sdcc_pdata.enable_dma  = 0;
      }
      else
      {
         sdcc_pdata.enable_dma  = 1;
      }
#endif
      
#ifdef T_QSC1110
      /* set the power mode to 'power on' */
      HWIO_OUTM(MCI_POWER,
                HWIO_FMSK(MCI_POWER, CONTROL),
                MCI_POWER_CTRL_ON);
#endif
      if(sdcc_bsp_slot_interrupt_exists())
      {
         sdcc_bsp_enable_slot_int((void *)sdcc_slot_isr);
      }
      sdcc_pdata.slot_state_changed = FALSE;
   }
   while (0);
#else
{
   if(m_binit)
   {
	   memset(&sdcc_pdata, 0, sizeof(sdcc_data_type));
	   //sdcc_enter_crit_sect();
	   bRet = (AIT701_SD_Func.ait_sd_init() == NULL)?FALSE:TRUE;
	   if(bRet)
	   {
	   		MSG_FATAL("bRet=%d",bRet,0,0);
	       sdcc_pdata.card_type = SDCC_CARD_SD;
	       sdcc_pdata.mem.card_size = AIT701_SD_Func.ait_sd_get_size();
	       sdcc_pdata.mem.block_len = 512;
		   MSG_FATAL("AIT701_SD_Func.ait_sd_get_size()=%d",AIT701_SD_Func.ait_sd_get_size(),0,0);
		   MSG_FATAL("sdcc_pdata.card_type=%d",sdcc_pdata.card_type,0,0);
		   MSG_FATAL("sdcc_pdata.mem.card_size=%d",sdcc_pdata.mem.card_size,0,0);
	   }
	   else
	   {
	   	   m_binit = FALSE;
	       memset(&sdcc_pdata, 0, sizeof(sdcc_data_type));
	   }
   }
    
   }
   //sdcc_leave_crit_sect();
#endif
   sdcc_leave_crit_sect();
#ifdef CUST_EDITION
   return (bRet);
#else
   return(TRUE);
#endif
}/* sdcc_init */

/******************************************************************************
* Name: sdcc_close
*
* Description:
*    This function de-select the device, turn off the power supply to
*    the device and switch off the controller clock.
*
* Arguments:
*       driveno  Drive Number
*
* Returns:
*       TRUE  if sucessful
*       FALSE if failure
*
******************************************************************************/
boolean
sdcc_close(int16 driveno)
{
#ifndef FEATURE_DSP
   (void)driveno;
   sdcc_enter_crit_sect();
   do
   {
      /* Do not turn off the sdcc controller if controller was not turned on first*/
      if (sdcc_pdata.host_state ==  SDCC_HOST_IDLE)
         break;

      if (sdcc_pdata.card_type != SDCC_CARD_UNKNOWN)
      {
         /* CMD7: de-select the card */
         (void) sdcc_select_card(0, FALSE);
      }
#ifdef T_QSC1110
      /* turn off the controller CLK */
      HWIO_OUTM(MCI_POWER,
                HWIO_FMSK(MCI_POWER, CONTROL),
                MCI_POWER_CTRL_OFF);

      /* disable all the interrupts */
      sdcc_disable_int(MCI_INT_MASK_DMA_DONE  |
                       SDCC_CMD_INTERRUPTS    |
                       MCI_INT_MASK_PROG_DONE |
                       MCI_INT_MASK_SDIO_INTR );
#endif
#if defined(FEATURE_SDCC_FIX_FOR_EXTRA_10MA_ON_TCXO)  

      //a tonyo production: VREG_AUX1 will be shut off, thus configure
      //these GPIOs to inputs with no pull in order to avoid current
      //sourcing via the TFLASH pullup resistors

      /*lint -save -e64 Suppress 'Type mismatch' warning */    
      gpio_config(GPIO_INP_31  & PULL_DOWN);
      gpio_config(GPIO_INP_30  & PULL_DOWN);
      gpio_config(GPIO_INP_32  & PULL_DOWN);
      gpio_config(GPIO_INP_99  & PULL_DOWN);
      gpio_config(GPIO_INP_100 & PULL_DOWN);
      gpio_config(GPIO_INP_101 & PULL_DOWN);
      /*lint -restore */
#endif    

#if defined(FEATURE_SDCC_FIX_FOR_EXTRA_10MA_TCXO_ON)
      gpio_disable_vreg_aux1_gpios();
#endif

      sdcc_bsp_vdd_control(SDCC_BSP_VDD_OFF);

#if defined(FEATURE_SDCC_FIX_FOR_EXTRA_10MA_ON_TCXO)  

      /* disble the clock and others*/
      HWIO_OUTM(MCI_CLK,                               //a tonyo production
                HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE)|
                HWIO_FMSK(MCI_CLK, FLOW_ENA)|HWIO_FMSK(MCI_CLK, WIDEBUS),
                MCI_POWER_CTRL_OFF); 
#endif    
#ifdef T_QSC1110
      /* disable the clock regime */
      CLK_REGIME_DISABLE(CLK_RGM_SDCC_MCLK_M);
      CLK_REGIME_DISABLE(CLK_RGM_SDCC_HCLK_M);
#endif
      /* Reset the card type & host state */
      sdcc_pdata.card_type  =  SDCC_CARD_UNKNOWN;
      sdcc_pdata.host_state =  SDCC_HOST_IDLE;
   }
   while (0);
   sdcc_leave_crit_sect();
   return TRUE;
#else
   return TRUE;
#endif
}/* sdcc_close */

/*=============================================================================
 * FUNCTION      sdcc_activity_timeout_event
 *
 * DESCRIPTION   This function tells SDCC that the SD slot has not been accessed 
 *               for a certain period of time. If we have the ability to 
 *               detect card removal / insertion, we shutdown the slot, otherwise
 *               do nothing.
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    driveno [IN]: The driveno parameter is not used, and is present 
 *               for consistency.
 *
 * RETURN VALUE  SDCC_NO_ERROR if successful, error code otherwise
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS sdcc_activity_timeout_event( int16 driveno )
{
#ifndef FEATURE_DSP
   if(FALSE == sdcc_bsp_slot_interrupt_exists())
      return SDCC_ERR_UNKNOWN;

   DPRINTF(("SDCC Activity timeout. Closing the slot.\n")); 
#endif
   (void)sdcc_close(driveno);
   return SDCC_NO_ERROR;
}
/*lint -restore */
