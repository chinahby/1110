/**********************************************************************
 * sdcc_sdio.c
 *
 * SDCC/SDIO driver.
 *
 * This file implements the SDCC driver support for SDIO card. 
 *
 * Copyright (c) 2004-2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_sdio.c#6 $
$DateTime: 2009/05/19 19:03:01 $ $Author: shermenc $

when       who     what, where, why
--------   -------------------------------------------------------------
2009-05-15 sc      Fixed issue taking up CPU usage when Hotplug polls SDCC
2008-11-17 vin     Added TCXO reset to sdcc_sdio_read/write
2008-08-25 vin     Call sdcc_close when no card is present
2008-07-24 vin     Minor cleanup
2008-06-19 vin     Added critical section locking
2008-06-19 vin     Removed SDCC Mux features
04/15/08   vin     Using sdcc_read_fifo and write in place of
                   sdcc_sdio_fifo_read and write.
12/14/07   vin     Added host state check to sdio_post_init
10/19/07   vin     Minor correction to pre and post_init
10/15/07   umr     Integrate pre and post init from 4.0 branch
08/02/07   vin     Added feature FEATURE_SDCC_WLAN_CONFIG_API
06/11/07   vin     Switch to upper slot "SDC1" on SC2x SURF for WLAN
04/11/07   sc      Ported in MMCPlus support
03/23/07   vin     Set 8mA drive strength on clk line for modified FFA
03/12/07   vin     Added FEATURE_WLAN_MODIFIED_FFA
02/16/07   sc      Fixed lint errors
01/16/07   dng     Added the sdcc_config_clk call to the sdio find card 
                   function.
11/28/06   dng     Added the loop to cmd5 for SDIO card detection.
11/28/06   dng     Added the return SDCC_NO_ERROR status in the function
                   sdcc_sdio_init.  If the current state is the HOST_STATE.
11/05/06   dng     Added the INTLOCK/INTFREE back to sdcc_sdio_read
                   sdcc_sdio_write and sdcc_sdio_stop_xfer functions.  
10/16/06   dng     Changed the sdcc_find_sdio_card function to 
                   fix the Atheros Card detection issue.
09/22/06   dng     Removed all INTLOCK & INTFREE macros.
09/19/06   dng     Added the FEATURE_SDCC_MUX_SINGLE_DRIVE_ON 
09/05/06   dng     Changed the function sdcc_process_dma to sdcc_process_interuupts
07/17/06   sc      Added SD2.0 feature
07/17/06   dng     First pass of code review clean up.
07/06/06   dng     Changed the default for the sdio_card to ATheros.
05/11/06   dng     Removed   fs_sfat_lock_card() and fs_sfat_unlock_card()
                   calls from the sdcc_sdio_init function.
05/10/06   dng     Added SDIO WLAN autodetection support.
05/04/06   dng     Added byte mode support for WLAN SDIO.
03/01/06   dng     Added Atheros wlan card support.
12/06/05   dng     Fixed compiler warnings.
11/10/05   hwu     Fixed lint error complains.
10/18/05   hwu     Added get tuple support. 
                   Changed to look for device by the manufacturer ID. 
08/28/05   hwu     General cleanups.
07/22/05   hwu     Locked the card access when in SDIO initialization.
06/01/05   hwu     Added deley in retries looking for card.
05/26/05   hwu     Merged changes from L4Linux.
05/09/05   hwu     - added DMA support
                   - added to enable host interrupt individually  
03/22/05   hwu     Added retries in looking for WLAN device.
02/15/05   hwu     Initial version.
=======================================================================*/
#include "msm.h"
#include "clk.h"        /* clk_busy_wait()  */
#include "hw.h"         /* MSM version info */
#include "sdcc_api.h"
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "assert.h"

#include "pm.h"

#include GPIO_H
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
#include CLKRGM_H
#endif

/* time in milli-seconds to ramp up Atheros WLAN card */
#define SDCC_SDIO_WAIT_ATHEROS_CARD_RAMP_UP_TIME_MS   300

extern void  mmu_invalidate_data_cache_lines(uint32* start_ptr, uint32 n_bytes);


#ifdef SDCC_DEBUG_ON
void         sdcc_sdio_read_cccr(void);
void         sdcc_sdio_read_fbr(uint8 devfn);
#endif

uint8        sdcc_sdio_find_device(uint16 dev_manfid);
void         sdcc_sdio_disable_fn(uint8 devfn);
SDCC_STATUS  sdcc_sdio_stop_xfer(uint32 direction);
SDCC_STATUS  sdcc_sdio_enable_fn(uint8 devfn);
SDCC_STATUS  sdcc_sdio_set_blksz(uint8 devfn, uint16 block_size);
SDCC_STATUS  sdcc_sdio_set_buswidth(SDCC_SDIO_INTERFACE_MODE  buswidth);
void         sdcc_sdio_get_tuple(uint32 tpl_addr, uint8 *pbuff);

/* SDIO Spec. V.1.1 P. 24 */
#define   SDIO_BUILD_CMD53_ARGU(dir, devfn, reg_addr, count)                 \
                           ((dir << 31)                      |          \
                            (devfn << 28)                    |          \
                            (sdcc_pdata.block_mode << 27)    |          \
                            (sdcc_pdata.io.op_code << 26)    |          \
                            (reg_addr << 9)                  |          \
                            count)


/* Default to the Atheros WLAN card type */
unsigned int sdio_card = SDIO_ATHEROS_MANFID;

#ifdef T_MSM6800
/*********************************************************
 * For now set the SDCC clk drive strength to 4ma
 * Later, make this a configurable item depending on which
 * FFA flavor we are working with
 *********************************************************/
uint16 sdcc_set_drv_strength=1;

#ifdef FEATURE_WLAN_MODIFIED_FFA
#error code not present
#else
uint16 gpio_clk_val=1;
#endif

#endif

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_find_sdio_card
*
* Description:
*    This function is to find out the sdio type of the device.
*
* Arguments:
*
* Returns:
*    Returns error code.
*    
******************************************************************************/
SDCC_CARD_TYPE
sdcc_find_sdio_card( void )
{
  sdcc_cmd_type  sdcc_cmd;
  uint32         i;
  SDCC_STATUS    rc        = SDCC_ERR_UNKNOWN;
  SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;

  uint32         voltage;
  uint32         mem_present = 0;

  /*-----------------------------------------------------------------------*/
  sdcc_config_clk(SDCC_IDENTIFICATION_MODE,
                  SDCC_CARD_UNKNOWN);
    
  if(sdio_card == SDIO_ATHEROS_MANFID)
  {
    sdcc_mdelay(SDCC_SDIO_WAIT_ATHEROS_CARD_RAMP_UP_TIME_MS);
  }


  if(sdio_card == SDIO_PHILLIP_MANFID)
  {
      /* enable the clock with the INVERT_IN set to 1*/
    HWIO_OUTM(MCI_CLK,
              HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
              MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE | MCI_CLK_INVERT_IN );

  }
  else
  {
    /* enable the clock */
    HWIO_OUTM(MCI_CLK,
              HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
             MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE);
  }
  

  /* power ramp up time: 1ms */
  sdcc_mdelay(1);

  for(i = 0; i < SDCC_SDIO_INIT_RETRIES; i++)
  {

    /* CMD5: look for IO card first */
    sdcc_cmd.cmd       = SD_CMD5_IO_SEND_OP_COND;
    sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
    sdcc_cmd.resp_type = SDCC_RESP_SHORT;
    sdcc_cmd.prog_scan = 0;
      
    rc = sdcc_command(&sdcc_cmd);

    if(SDCC_NO_ERROR == rc)
    {
      break;
    }
    sdcc_mdelay(50);

  }

  if(SDCC_NO_ERROR == rc)
  {
    if (sdcc_cmd.resp[0] & (SDCC_IO_NUM_FNS))
    {
      voltage     = ( sdcc_cmd.resp[0] ) & 0x00FFFFFF;
      mem_present = ( sdcc_cmd.resp[0] & (SDCC_IO_MP));

      for(i = 0; i < SDCC_SDIO_INIT_RETRIES; i++)
      {
        /* CMD5: negotiate IO card voltage */
        sdcc_cmd.cmd       = SD_CMD5_IO_SEND_OP_COND;
        sdcc_cmd.cmd_arg   = voltage;
        sdcc_cmd.resp_type = SDCC_RESP_SHORT;
        sdcc_cmd.prog_scan = 0;
        
        rc = sdcc_command(&sdcc_cmd);
        
          if( (SDCC_NO_ERROR == rc) && (sdcc_cmd.resp[0] & SDCC_DEVICE_RDY))
          {
              card_type = SDCC_CARD_SDIO;                    
              break;
          }

          if(sdio_card == SDIO_ATHEROS_MANFID)
          {
            sdcc_mdelay(50);
          }
          
      }
        
      DPRINTF(("%s Found SDIO card after %dth retry \n",
              (i == SDCC_SDIO_INIT_RETRIES) ? "Not": "", i));
        
      DPRINTF(("Memory %s present\n", (mem_present) ? "": "not"));
    }
  }
  return card_type;
}/* sdcc_find_sdio_card */

/******************************************************************************
* Name: sdio_pre_init
*
* Description:
*    This function checks to see if we can exit early if the drive is use.
*    If it  is not in use, initialize the SDCC controller.
*
* Arguments:
*    dev_manfid       [IN] : SDIO manufacturer identification
*
* Returns:
*    Returns card type in use (SDCC_CARD_UNKNOWN, in case it's not in use)
*
******************************************************************************/
SDCC_CARD_TYPE sdio_pre_init( uint16   dev_manfid )
{
   sdcc_init_crit_sect();
   sdcc_enter_crit_sect();
   /*----------------------------------------------------------------------*/
#if defined(FEATURE_SDCC_TURN_ON_PWR_SC2X) && !defined(FEATURE_EXT_USB_P1)
   // Use upper slot "SDC1" on SC2x SURF for WLAN
#ifdef WLAN_MUX_EN
   gpio_out(WLAN_MUX_EN, GPIO_HIGH_VALUE);
#endif
#endif
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
   sdcc_pdata.is_wlan = TRUE;
   if(sdcc_pdata.is_wlan_param_set == FALSE)
   {
      /*set wlan parameters to default*/
      sdcc_sdio_set(0,SDCC_SDIO_WLAN_CONFIG, NULL);
   }
#endif

   /* Do not re-init the SDIO card again if it's in the SDCC_HOST_TRAN state */
   if(SDCC_HOST_TRAN == sdcc_pdata.host_state)
   {     
      goto SDCC_EXIT_PRE_INIT;
   }

   /* Set the sdio card type*/
   sdio_card = dev_manfid;

   if(SDCC_CARD_UNKNOWN == sdcc_pdata.card_type)
   {
      /* initialize the SDCC controller */
      (void)sdcc_init();
   }
SDCC_EXIT_PRE_INIT:   
   sdcc_leave_crit_sect();
   return sdcc_pdata.card_type;
}

/******************************************************************************
* Name: sdio_post_init
*
* Description:
*    This function looks for the specified device function implemented
*    on the device.  If the device is found as mentioned in "card type", it 
*    then will configure the device and enable it. 
*
* Arguments:
*    dev_manfid       [IN] : SDIO manufacturer identification
*    cardtype         [IN] : card type from SDCC_CARD_TYPE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdio_post_init( uint16            dev_manfid,
                            SDCC_CARD_TYPE    card_type )
{
   SDCC_STATUS                rc = SDCC_NO_ERROR;
   SDIO_CARD_INTERFACE        card_intf;
   uint8                      devfn   = 0;
#if defined FEATURE_SDCC_WLAN_CONFIG_API && defined T_MSM6800
   uint8                      drv_strength = 0;
#endif

   sdcc_enter_crit_sect();
   sdcc_pdata.card_type = card_type;

   /* clean up and exit if no card */
   if(SDCC_CARD_UNKNOWN == sdcc_pdata.card_type)
   {
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_POST_INIT;
   }

   /* return immediately if not a SDIO card */
   if((SDCC_CARD_SDIO != sdcc_pdata.card_type ) &&
      (SDCC_CARD_COMB != sdcc_pdata.card_type))
   {
      DPRINTF(("Not an SDIO card.\n"));
      rc = SDCC_ERR_SDIO;
      goto SDCC_EXIT_POST_INIT;
   }


   /* If the SDCC host state already in transfer, just return */
   if (SDCC_HOST_TRAN == sdcc_pdata.host_state)
   {
      rc = SDCC_NO_ERROR;
      goto SDCC_EXIT_POST_INIT;
   }

   if(sdio_card == SDIO_PHILLIP_MANFID)
   {
      /* enable the clock with the INVERT_IN set to 1*/
      HWIO_OUTM(MCI_CLK,
                HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
                MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE | MCI_CLK_INVERT_IN );
   }
   else
   {
      /* enable the clock */
      HWIO_OUTM(MCI_CLK,
                HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
                MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE);
   }

   /* re-program the clock */
   sdcc_config_clk(SDCC_DATA_TRANSFER_MODE,
                   sdcc_pdata.card_type);

   HWIO_OUTM(MCI_CLK,
             HWIO_FMSK(MCI_CLK, FLOW_ENA),
             MCI_CLK_SET_FLOW);

#ifdef T_MSM6800
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
   switch(sdcc_pdata.sdcc_wlan_params.drive_strength)
   {
      case SDCC_DRV_2MA : drv_strength = 0;
         break;
      case SDCC_DRV_4MA : drv_strength = 1;
         break;
      case SDCC_DRV_6MA : drv_strength = 2;
         break;
      case SDCC_DRV_8MA : drv_strength = 3;
         break;
      case SDCC_DRV_10MA : drv_strength = 4;
         break;
      case SDCC_DRV_12MA : drv_strength = 5;
         break;
      case SDCC_DRV_14MA : drv_strength = 6;
         break;
      case SDCC_DRV_16MA : drv_strength = 7;
         break;
      default:
         break;
   }
   HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
   HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
             drv_strength << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
#else
   if(sdcc_set_drv_strength)
   {
      HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
      HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
      gpio_clk_val << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
   }
#endif
#endif

   rex_sleep(2);

   /* select the card */
   rc = sdcc_config_rca();
   if(SDCC_NO_ERROR != rc)
   {
      DPRINTF(("Cannot get the RCA.\n"));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_POST_INIT ;
   }

   rc = sdcc_select_card(sdcc_pdata.rca[sdcc_pdata.curr_sd_drv], TRUE);

   /* look for the requested function on the card */
   devfn = sdcc_sdio_find_device(dev_manfid);
   if(!devfn)
   {
      DPRINTF(("Cannot find the device made by 0x%x.\n",dev_manfid));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_POST_INIT;
   }

   /* see PHG data sheet */
   sdcc_pdata.block_mode  = SDCC_SDIO_BLK_MODE;
   sdcc_pdata.io.op_code  = SDCC_SDIO_OP_FIXED;

   /* enable the found function */
   rc = sdcc_sdio_enable_fn(devfn);
   if(SDCC_NO_ERROR != rc)
   {
      DPRINTF(("Cannot enable device %d.\n", devfn));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_POST_INIT;
   }

   /* set the interface host supports */
   (void)sdcc_sdio_get(SD_GET_CARD_INTERFACE,
                       &card_intf,
                       sizeof(SDIO_CARD_INTERFACE));

   (void)sdcc_sdio_set_buswidth(card_intf.InterfaceMode);

   /* set the block size */
   (void)sdcc_sdio_set_blksz(SDCC_SDIO_FN1,
                             SDCC_SDIO_DEFAULT_BLKSZ);

   if(SDIO_ATHEROS_MANFID == dev_manfid)
   {
      /* Enable both DMA and Prog done only for Atheros */
      sdcc_enable_int(MCI_INT_MASK_DMA_DONE |
                      MCI_INT_MASK_PROG_DONE); 

   }
   else
   {
       /* Enable the DMA interrupt */
       sdcc_enable_int(MCI_INT_MASK_DMA_DONE);
   }
   sdcc_pdata.host_state = SDCC_HOST_TRAN;

SDCC_CLOSE_AND_EXIT_POST_INIT:

   if(SDCC_NO_ERROR != rc)
   {
      sdcc_pdata.errno = rc;
      (void)sdcc_close(0);
   }

SDCC_EXIT_POST_INIT:

   sdcc_leave_crit_sect();
   return rc;
}

/******************************************************************************
* Name: sdcc_sdio_init
*
* Description:
*    This function looks for the specified device function implemented
*    on the device. If it finds the device, it then will configure the
*    device and enable it. 
*
* Arguments:
*    dev_manfid: SDIO manufacturer Identification 
*
* Returns:
*    returns status code.
******************************************************************************/

SDCC_STATUS
sdcc_sdio_init(uint16 dev_manfid)
{
   SDCC_STATUS                rc = SDCC_NO_ERROR;
   SDIO_CARD_INTERFACE        card_intf;
   uint8                      devfn   = 0;
   uint8                      retries = 0;
   uint8                      drv_strength = 0;
   (void)drv_strength;
   /*----------------------------------------------------------------------*/
   sdcc_init_crit_sect();
   sdcc_enter_crit_sect();
#if defined(FEATURE_SDCC_TURN_ON_PWR_SC2X) && !defined(FEATURE_EXT_USB_P1)
   // Use upper slot "SDC1" on SC2x SURF for WLAN
#ifdef WLAN_MUX_EN
   gpio_out(WLAN_MUX_EN, GPIO_HIGH_VALUE);
#endif
#endif
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
   sdcc_pdata.is_wlan = TRUE;
   if(sdcc_pdata.is_wlan_param_set == FALSE)
   {
      /*set wlan parameters to default*/
      sdcc_sdio_set(0,SDCC_SDIO_WLAN_CONFIG, NULL);
   }
#endif
   /* Do not re-init the SDIO card again if it's in the SDCC_HOST_TRAN state */
   if(SDCC_HOST_TRAN == sdcc_pdata.host_state)
   {     
      goto SDCC_EXIT_SDIO_INIT;
   }
   
   /* Set the sdio card type*/
   sdio_card = dev_manfid;

#ifdef FEATURE_SDCC_WLAN_CONFIG_API
   rex_sleep(sdcc_pdata.sdcc_wlan_params.init_delay);
#else
#ifdef FEATURE_WLAN_MODIFIED_FFA
#error code not present
#endif //FEATURE_WLAN_MODIFIED_FFA
#endif //FEATURE_SDCC_WLAN_CONFIG_API

   if(SDCC_CARD_UNKNOWN == sdcc_pdata.card_type)
   {
      /* initialize the SDCC controller */
      (void)sdcc_init();

      while(retries++ < SDCC_SDIO_INIT_RETRIES)
      {
         /* reset the device by switching off the voltage */
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
         pm_vote_vreg_switch(PM_OFF_CMD,
                             sdcc_pdata.sdcc_wlan_params.vreg,
                             PM_VOTE_VREG_MMC_APP__MINI_SD);
#else

#ifdef FEATURE_WLAN_MODIFIED_FFA
#error code not present
#else
         pm_vote_vreg_switch(PM_OFF_CMD,
                             PM_VREG_MMC_ID,
                             PM_VOTE_VREG_MMC_APP__MINI_SD);
#endif
#endif            
         /* PHG says they need at least 5msec */
         rex_sleep(1500 * retries);
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
         pm_vote_vreg_switch(PM_ON_CMD,
                             sdcc_pdata.sdcc_wlan_params.vreg,
                             PM_VOTE_VREG_MMC_APP__MINI_SD);
#else
#ifdef FEATURE_WLAN_MODIFIED_FFA
#error code not present
#else
         pm_vote_vreg_switch(PM_ON_CMD,
                             PM_VREG_MMC_ID,
                             PM_VOTE_VREG_MMC_APP__MINI_SD);
#endif
#endif
         rex_sleep(1000);
            
         /* find the SDIO card */
         sdcc_pdata.card_type = sdcc_find_sdio_card();

         /* break if found any card */
         if(sdcc_pdata.card_type != SDCC_CARD_UNKNOWN)
            break;
      }
   }

   if(SDCC_CARD_SDIO == sdcc_pdata.card_type)
   {
      DPRINTF(("Found the SDIO card .\n"));
   }
   else
   { 
      DPRINTF(("SDIO card not found.\n"));
   }

   /* clean up and exit if no card */
   if(SDCC_CARD_UNKNOWN == sdcc_pdata.card_type)
   {
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_SDIO_INIT;
   }

   /* return immediately if not a SDIO card */
   if((SDCC_CARD_SDIO != sdcc_pdata.card_type ) &&
      (SDCC_CARD_COMB != sdcc_pdata.card_type))
   {
      DPRINTF(("Found memory card.\n"));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_EXIT_SDIO_INIT;
   }


   if(sdio_card == SDIO_PHILLIP_MANFID)
   {
      /* enable the clock with the INVERT_IN set to 1*/
      HWIO_OUTM(MCI_CLK,
                HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
                MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE | MCI_CLK_INVERT_IN );
   }
   else
   {
        /* enable the clock */
      HWIO_OUTM(MCI_CLK,
                HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
                MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE);
   }

   /* re-program the clock */
   sdcc_config_clk(SDCC_DATA_TRANSFER_MODE,
                   sdcc_pdata.card_type);
    
   HWIO_OUTM(MCI_CLK,
             HWIO_FMSK(MCI_CLK, FLOW_ENA),
             MCI_CLK_SET_FLOW);

#ifdef T_MSM6800
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
   switch(sdcc_pdata.sdcc_wlan_params.drive_strength)
   {
      case SDCC_DRV_2MA : drv_strength = 0;
           break;
      case SDCC_DRV_4MA : drv_strength = 1;
           break;
      case SDCC_DRV_6MA : drv_strength = 2;
           break;
      case SDCC_DRV_8MA : drv_strength = 3;
           break;
      case SDCC_DRV_10MA : drv_strength = 4;
           break;
      case SDCC_DRV_12MA : drv_strength = 5;
           break;
      case SDCC_DRV_14MA : drv_strength = 6;
           break;
      case SDCC_DRV_16MA : drv_strength = 7;
           break;
      default:
           break;
   }
   HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
   HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
   drv_strength << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
#else
   if(sdcc_set_drv_strength)
   {
      HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
      HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
      gpio_clk_val << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
   }
#endif
#endif

   rex_sleep(2);

   /* select the card */
   rc = sdcc_config_rca();
   if(SDCC_NO_ERROR != rc)
   {
      DPRINTF(("Cannot get the RCA.\n"));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_SDIO_INIT;
   }

   rc = sdcc_select_card(sdcc_pdata.rca[sdcc_pdata.curr_sd_drv], TRUE);

   /* look for the requested function on the card */
   devfn = sdcc_sdio_find_device(dev_manfid);
   if(!devfn)
   {
      DPRINTF(("Cannot find the device made by 0x%x.\n",dev_manfid));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_SDIO_INIT;
   }
    
   /* see PHG data sheet */
   sdcc_pdata.block_mode  = SDCC_SDIO_BLK_MODE;
   sdcc_pdata.io.op_code  = SDCC_SDIO_OP_FIXED;

   /* enable the found function */
   rc = sdcc_sdio_enable_fn(devfn);
   if(SDCC_NO_ERROR != rc)
   {
      DPRINTF(("Cannot enable device %d.\n", devfn));
      rc = SDCC_ERR_CARD_INIT;
      goto SDCC_CLOSE_AND_EXIT_SDIO_INIT;
   }

   /* set the interface host supports */
   (void)sdcc_sdio_get(SD_GET_CARD_INTERFACE,
                       &card_intf,
                       sizeof(SDIO_CARD_INTERFACE));
    
   (void)sdcc_sdio_set_buswidth(card_intf.InterfaceMode);
    
   /* set the block size */
   (void)sdcc_sdio_set_blksz(SDCC_SDIO_FN1,
                             SDCC_SDIO_DEFAULT_BLKSZ);


   if(SDIO_ATHEROS_MANFID == dev_manfid)
   {
      /* Enable both DMA and Prog done only for Atheros */
      sdcc_enable_int(MCI_INT_MASK_DMA_DONE |
                      MCI_INT_MASK_PROG_DONE); 
   }
   else
   {
      /* Enable the DMA interrupt */
      sdcc_enable_int(MCI_INT_MASK_DMA_DONE);
   }

   sdcc_pdata.host_state = SDCC_HOST_TRAN;

SDCC_CLOSE_AND_EXIT_SDIO_INIT:
   if(SDCC_NO_ERROR != rc)
   {
      sdcc_pdata.errno = rc;
      (void)sdcc_close(0);
   }
SDCC_EXIT_SDIO_INIT:
   sdcc_leave_crit_sect();
   return rc;
}/* sdcc_sdio_init */


/******************************************************************************
* Name: sdcc_sdio_find_device
*
* Description:
*    This function is to look for the SDIO device by the specified
*    manufacturer identification code. 
*
* Arguments:
*    dev_manfid  manufacturer identification.
*
* Returns:
*    Returns device function number if the manufacture id matches.
*    
******************************************************************************/
uint8
sdcc_sdio_find_device(uint16 dev_manfid)
{
    uint8       cccr[SDCC_SDIO_CCCR_MAX_BYTES];
    uint8       fbr[SDCC_SDIO_FBR_MAX_BYTES];
    uint32      i = 0;
    uint32      j = 0;
    uint8      *pdata = NULL;
    uint8       tuple[SDCC_SDIO_CISTPL_MAX_SZ];
    
    sdcc_pdata.io.devfn = 0;

    /* CMD52: SDIO read CCCR */
    memset(cccr, 0, SDCC_SDIO_CCCR_MAX_BYTES);
    
    pdata = cccr;
    for( i = 0; i < SDCC_SDIO_CCCR_MAX_BYTES; i++)
    {
        (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                              SDCC_DATA_READ,
                              i,
                              pdata++);
    }

    sdcc_pdata.io.cc       = cccr[8];
    sdcc_pdata.io.cccr_cis = (cccr[11] << 16) |
                             (cccr[10] << 8)  |
                              cccr[9];

    /* CMD52: SDIO read FBRs */
    for ( i = 1; i < (SDCC_SDIO_MAX_FNS + 1); i++)
    {
        memset(fbr, 0, SDCC_SDIO_FBR_MAX_BYTES);

        pdata = fbr;
        for (j = 0; j < SDCC_SDIO_FBR_MAX_BYTES; j++)
        {
            (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                                  SDCC_DATA_READ,
                                  i * SDCC_SDIO_FBR_LENGTH + j,
                                  pdata++);
        }

        sdcc_pdata.io.fn_cis = ((uint32)fbr[11] << 16)  |
                               ((uint32)fbr[10] << 8)   |
                               fbr[9];

        /* only look for the first implemented function for now */
        if(0 != sdcc_pdata.io.fn_cis)
        {
            sdcc_pdata.io.devfn    = i;
            sdcc_pdata.io.fn_code  = fbr[0] & 0x0F;
            
            sdcc_pdata.io.fn_blksz = ((uint16)fbr[17] << 8) |
                                       fbr[16];
            break;
        }
    }

    /* the device function only counts if manfid matches */
    sdcc_sdio_get_tuple(sdcc_pdata.io.cccr_cis,
                        tuple);

    if(dev_manfid != sdcc_pdata.io.manfid)
        sdcc_pdata.io.devfn = 0;
    
    return (sdcc_pdata.io.devfn);
}

/******************************************************************************
* Name: sdcc_sdio_disable_fn
*
* Description:
*    This function is to programm the CCCR register in FN0 to disable the
*    specified function. 
*
* Arguments:
*    devfn: device function number.
*
* Returns:
*    
******************************************************************************/
void
sdcc_sdio_disable_fn(uint8 devfn)
{
    uint8  devfn_bit = 1 << devfn;
    uint8  data = 0;
    
    /* read out the enable register */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_READ,
                          SDCC_SDIO_REG_ENABLE,
                          &data);
    
    data = data & (~devfn_bit);

    /* disable IO */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_WRITE,
                          SDCC_SDIO_REG_ENABLE,
                          &data);
    
    /* HW: that's it??? */
}

/******************************************************************************
* Name: sdcc_sdio_enable_fn
*
* Description:
*    This function is to programm the CCCR register in FN0 to enable the
*    specified function. 
*
* Arguments:
*    devfn: device function number.
*    flag:  SDCC_SDIO_ENABLE_FN or SDCC_SDIO_DISABLE_FN
*
* Returns:
*    Returns success or failure.
*    
******************************************************************************/
SDCC_STATUS
sdcc_sdio_enable_fn(uint8 devfn)
{
    uint8            devfn_bit = 1 << devfn;
    SDCC_STATUS      rc = SDCC_NO_ERROR;
    int              t  = 0;
    uint8            rdata = 0;


    if(sdio_card == SDIO_PHILLIP_MANFID)
    {
    /* see PHG data sheet */
    sdcc_pdata.block_mode  = SDCC_SDIO_BLK_MODE;
    sdcc_pdata.io.op_code  = SDCC_SDIO_OP_FIXED;
    }

    /* read out the enable register */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_READ,
                          SDCC_SDIO_REG_ENABLE,
                          &rdata);
    
    rdata = rdata | devfn_bit;
    
    /* enable IO */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_WRITE,
                          SDCC_SDIO_REG_ENABLE,
                          &rdata);
    
    /* check for IO ready */
    do
    {
        sdio_mdelay(1);    

        (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                              SDCC_DATA_READ,
                              SDCC_SDIO_REG_IO_READY,
                              &rdata);
        
        if( (rdata & devfn_bit) == devfn_bit)
        {
            sdcc_pdata.io.flag = SDCC_SDIO_DEV_ENABLED;
            break;
        }
    } while( t++ < SDCC_SDIO_RDY_MAX_RETRIES );
    
    if( t == (SDCC_SDIO_RDY_MAX_RETRIES + 1))
    {
        DPRINTF(("Cannot enable SDIO function %d\n", devfn));
        rc = SDCC_ERR_SDIO;
    }

    return rc;
}



/******************************************************************************
* Name: sdcc_sdio_read
*
* Description:
*    This function is to read in data from the SDIO device via CMD53. It
*    reads count element of data from the device register reg_addr on
*    the device function devfn, storing them at the memory location
*    given by buff.
*
* Arguments:
*       devfn      device function number.
*       reg_addr   register address
*       units      number of blocks to be read
*       buff       pointer to buffer for the incoming data 
*
* Returns:
*       returns status code.
*
******************************************************************************/
SDCC_STATUS
sdcc_sdio_read
(
   uint8   devfn,
   uint32  reg_addr,
   uint16  units,
   uint8  *buff
)
{
   uint8            data_ctrl    = 0;
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS      rc           = SDCC_NO_ERROR;
   byte             *dw_buff     = NULL;
   uint32           length       = 0;
   uint32           blk_in_bits  = 0;
   uint32           count        = 0;
   uint32           isave        = 0;
    
   /*---------------------------------------------------------------------*/
   sdcc_enter_crit_sect();
   do
   {
      DPRINTF((">>sdio_read %d units from 0x%x in %d mode \n",
             units, reg_addr, sdcc_pdata.block_mode));
    
      length = sdcc_setup_data_xfer(SDCC_DATA_READ, units);
      if(!length)
      {
         rc = SDCC_ERR_SDIO_READ;
         break;
      }
        
#ifdef T_MSM6800
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
      HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
                HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
                sdcc_pdata.sdcc_wlan_params.drive_strength << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
#else
      if(sdcc_set_drv_strength)
      {
         HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
                   HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
                   gpio_clk_val << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
      }
#endif
#endif
      /* assuming all data buffer is dword aligned for now */
      if(IS_ALIGNED(buff))
         dw_buff = (byte *)buff;
      else
         ASSERT(0);

      /* set up the DPSM flags */
      data_ctrl = MCI_DATA_DPSM_ENABLED | MCI_DATA_READ;

      /* program DMA mode if needed */
      if (TRUE == sdcc_pdata.enable_dma)
      {
         /* make sure to sync with the DMA memory */
         mmu_invalidate_data_cache_lines((uint32 *)buff, length);
         data_ctrl |= MCI_DATA_DMA_ENABLED;
      }

      /* set block size */
      if(SDCC_SDIO_BYTE_MODE == sdcc_pdata.block_mode)
      {
         blk_in_bits  = sdcc_blk_in_bits(length);
         data_ctrl   |= blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT;
         count        = length;
      }

      if(SDCC_SDIO_BLK_MODE == sdcc_pdata.block_mode)
      {
         blk_in_bits  = sdcc_blk_in_bits(sdcc_pdata.io.fn_blksz);
         data_ctrl   |= blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT;
         count = units;
      }

      /* CMD53: READ command */
      sdcc_cmd.cmd       = SD_CMD53_IO_RW_EXTENDED;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.cmd_arg   = SDIO_BUILD_CMD53_ARGU(SDCC_DATA_READ,
                                                 devfn,
                                                 reg_addr,
                                                 count);

      INTLOCK_SAV(isave);

      if (TRUE == sdcc_pdata.enable_dma)
      {
         if(sdio_card == SDIO_ATHEROS_MANFID)
         {
            rc = sdcc_command(&sdcc_cmd);
            HWIO_OUT(MCI_START_ADDR, (uint32)buff);
            HWIO_OUT(MCI_DATA_CTL, data_ctrl);
         }
         else
         {
            HWIO_OUT(MCI_START_ADDR, (uint32)buff);
            HWIO_OUTM(MCI_DATA_CTL,
                      HWIO_FMSK(MCI_DATA_CTL, ENABLE)    |
                      HWIO_FMSK(MCI_DATA_CTL, DIRECTION) |
                      HWIO_FMSK(MCI_DATA_CTL, DMA_ENABLE),
                      data_ctrl);

            rc = sdcc_command(&sdcc_cmd);
         }
      }
      else
      {
         HWIO_OUT(MCI_START_ADDR, (uint32)buff);
         HWIO_OUT(MCI_DATA_CTL, data_ctrl);
         rc = sdcc_command(&sdcc_cmd);
      }

      INTFREE_SAV(isave);

      if(SDCC_NO_ERROR != rc)
      {
         DPRINTF(("CMD53(read) failed\n"));
      }

      /* process read data */
      rc = ( TRUE == sdcc_pdata.enable_dma) ? sdcc_process_interrupts(&sdcc_cmd) :
                                              sdcc_read_fifo(dw_buff,length);

      if(SDCC_NO_ERROR == rc)
      {
         rc = sdcc_sdio_stop_xfer(SDCC_DATA_READ);
      }

      if(SDCC_NO_ERROR != rc)
         DPRINTF(("sdcc_sdio_read failed, mode = %d\n", sdcc_pdata.block_mode));

      /* turn off tcxo, allow sleep */
      sdcc_set_tcxo_clk(FALSE);   

      DPRINTF(("<<sdio_read exit(0x%x).\n",*dw_buff));
   }
   while(0);
   sdcc_leave_crit_sect();
   return rc;
}

/******************************************************************************
* Name: sdcc_sdio_write
*
* Description:
*    This function is to write data to the SDIO device via CMD53. It
*    writes count element of data, obtained from the memory location
*    buff, to the device register reg_addr on the device function devfn.
*
* Arguments:
*    devfn     device function number
*    reg_addr  register address
*    units     number of blocks
*    buff      pointer of where the outgoing data is
*
* Returns:
*    returns status code.
******************************************************************************/
SDCC_STATUS
sdcc_sdio_write
(
   uint8   devfn,
   uint32  reg_addr,
   uint16  units,
   uint8  *buff
)
{
   SDCC_STATUS      rc          = SDCC_NO_ERROR;
   uint8            data_ctrl   = 0;
   sdcc_cmd_type    sdcc_cmd;
   uint32           length      = 0;
   byte             *dw_buff    = NULL;
   uint32           blk_in_bits = 0;
   uint32           isave       = 0;

   sdcc_enter_crit_sect();
   do
   {
      DPRINTF((">>sdio_write %d units to 0x%x in %d mode\n",
               units, reg_addr, sdcc_pdata.block_mode));

#ifdef T_MSM6800
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
      HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
                HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
                sdcc_pdata.sdcc_wlan_params.drive_strength << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
#else
      if(sdcc_set_drv_strength)
      {
         HWIO_OUTM(GPIO_PAD_HDRIVE_MSEL_3, 
               HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31), 
               gpio_clk_val << HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_31));
      }
#endif
#endif

      length = sdcc_setup_data_xfer(SDCC_DATA_WRITE, units);
      if(!length)
      {
         rc = SDCC_ERR_SDIO_WRITE;
         break;
      }

      if(IS_ALIGNED(buff))
         dw_buff = (byte *)buff;
      else
         ASSERT(0);

      /* set up DPSM flags */
      data_ctrl = MCI_DATA_DPSM_ENABLED | MCI_DATA_WRITE;
      if (TRUE == sdcc_pdata.enable_dma)
      {
         data_ctrl |= MCI_DATA_DMA_ENABLED;
      }
    
      /* CMD53: write command */
      sdcc_cmd.cmd       = SD_CMD53_IO_RW_EXTENDED;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
    
      /* set block size */
      if(SDCC_SDIO_BYTE_MODE == sdcc_pdata.block_mode)  
      {
         blk_in_bits = sdcc_blk_in_bits(length);
         sdcc_cmd.cmd_arg   = SDIO_BUILD_CMD53_ARGU(SDCC_DATA_WRITE,
                                                    devfn,
                                                    reg_addr,
                                                    length);
      }

      if(SDCC_SDIO_BLK_MODE == sdcc_pdata.block_mode)
      {
         blk_in_bits = sdcc_blk_in_bits(sdcc_pdata.io.fn_blksz);
         sdcc_cmd.cmd_arg = SDIO_BUILD_CMD53_ARGU(SDCC_DATA_WRITE,
                                                  devfn,
                                                  reg_addr,
                                                  units);
      }
      data_ctrl  |= blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT;

      INTLOCK_SAV(isave);

      rc = sdcc_command(&sdcc_cmd);

      /* program DMA mode if needed */
      if (TRUE == sdcc_pdata.enable_dma)
      {
         HWIO_OUT(MCI_START_ADDR, (uint32)buff);
         if(sdio_card == SDIO_ATHEROS_MANFID)
         {
            HWIO_OUT(MCI_DATA_CTL, data_ctrl);
         }
         else
         {
            /* start the DPSM.
            Note: Needs to be after sending the command */
            HWIO_OUTM(MCI_DATA_CTL,
                       HWIO_FMSK(MCI_DATA_CTL, ENABLE)    |
                       HWIO_FMSK(MCI_DATA_CTL, DIRECTION) |
                       HWIO_FMSK(MCI_DATA_CTL, DMA_ENABLE),
                       data_ctrl);
         }
      }
      else
      {
         HWIO_OUT(MCI_DATA_CTL, data_ctrl);
      }

      INTFREE_SAV(isave);

      if(SDCC_NO_ERROR != rc)
      {
         DPRINTF(("CMD53(write) failed\n"));
      }

      rc = (TRUE == sdcc_pdata.enable_dma) ? sdcc_process_interrupts(&sdcc_cmd):
                                          sdcc_write_fifo(dw_buff, length);
      
      if(SDCC_NO_ERROR == rc)
      {
         (void)sdcc_sdio_stop_xfer(SDCC_DATA_WRITE);
      }

      if(SDCC_NO_ERROR != rc)
         DPRINTF(("sdcc_sdio_write failed, mode = %d\n", sdcc_pdata.block_mode));

      /* turn off tcxo, allow sleep */
      sdcc_set_tcxo_clk(FALSE);        

      DPRINTF(("<<sdio_write exit(0x%x).\n", *dw_buff));
   }
   while(0);
   sdcc_leave_crit_sect();
   return rc;
} 

/******************************************************************************
* Name: sdcc_sdio_connect_intr
*
* Description:
*    This function enables the controller interrupt, writes to the
*    device to enable the interrupt for devfn and register an interrupt
*    handler for the device.
*
* Arguments:
*    devfn    : device function number.
*    isr      : pointer to the ISR function from OEM
*    isr_param: params for the ISR
*
* Returns:
*    
******************************************************************************/
void
sdcc_sdio_connect_intr
(
    uint8   devfn,
    void   *isr,
    void   *isr_param
)
{
   uint8   devfn_bit = 1 << devfn;
   uint8   reg_value = 0;
   sdcc_enter_crit_sect();

   sdcc_enable_int(MCI_INT_MASK_SDIO_INTR);
 
   /* set intr handler */
   sdcc_pdata.io.isr = (SDCC_SDIO_ISR_FUNC)isr;
   sdcc_pdata.io.isr_param = isr_param;

   /* CMD52: enable interrupt on device */  
   reg_value = devfn_bit | SDCC_SDIO_IENM;
   (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                         SDCC_DATA_WRITE,
                         SDCC_SDIO_REG_INT_ENABLE,
                         &reg_value);
   sdcc_leave_crit_sect();
}

/******************************************************************************
* Name: sdcc_sdio_disconnect_intr
*
* Description:
*    This function disconnects the SDIO interrupt. This should be called
*    at clean up time if the interrupt connection is called earlier.
*
* Arguments:
*    devfn: device function number.
*
* Returns:
*    
******************************************************************************/
void
sdcc_sdio_disconnect_intr(uint8 devfn)
{
   uint8       devfn_bit = 1 << devfn;
   uint8       reg_value = 0;
     
   sdcc_enter_crit_sect();
   /* disable SDIO interrupt */
   sdcc_disable_int(MCI_INT_MASK_SDIO_INTR);
    
   /* CMD52: disenable function interrupt */
   (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                         SDCC_DATA_READ,
                         SDCC_SDIO_REG_INT_ENABLE,
                         &reg_value);

   reg_value &= (~(devfn_bit | SDCC_SDIO_IENM));
   (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                         SDCC_DATA_WRITE,
                         SDCC_SDIO_REG_INT_ENABLE,
                         &reg_value);   
    
   /* set intr handler */
   sdcc_pdata.io.isr        = NULL;
   sdcc_pdata.io.isr_param  = NULL;
   sdcc_leave_crit_sect();
}

/******************************************************************************
* Name: sdcc_sdio_set_buswidth
*
* Description:
*    This function is to program the cccr to set the SD/MMC bus width.
*
* Arguments:
*    buswidth:  SDCC_SDIO_INTERFACE_1Bit or SDCCC_SDIO_INTERFACE_4Bit
*
* Returns:
*    returns status code.
******************************************************************************/
SDCC_STATUS
sdcc_sdio_set_buswidth(SDCC_SDIO_INTERFACE_MODE  buswidth)
{
    uint8       data;
    uint8       cc = sdcc_pdata.io.cc;
    SDCC_STATUS rc = SDCC_ERR_SDIO;
        
    if ( SDCC_INTERFACE_SDIO_4BIT == buswidth )
    {
        /* return if 4bit mode not supported */
        if(((cc & SDCC_SDIO_CC_LSC)  == SDCC_SDIO_CC_LSC) &&
           ((cc & SDCC_SDIO_CC_4BLS) != SDCC_SDIO_CC_4BLS))
            return rc;
    }

    /* FN0: read bus interface control */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_READ,
                          SDCC_SDIO_REG_BUS_CONTROL,
                          &data);
    data = data & 0x7C;

    /* Host Controller: set the MCI_CLK register 
       CCCR: set the 4bit bus mode and disable the card detection */
    if ( SDCC_INTERFACE_SDIO_4BIT == buswidth )
    {
        HWIO_OUTM(MCI_CLK,
                  HWIO_FMSK(MCI_CLK, WIDEBUS),
                  MCI_CLK_SET_WIDE);
        
        data |= (SDCC_SDIO_CD_DISABLE  | SDCC_SDIO_BUS_WIDTH_4BIT);
    }
    else
    {
        HWIO_OUTM(MCI_CLK,
                  HWIO_FMSK(MCI_CLK, WIDEBUS),
                  0);
        
        data |= (SDCC_SDIO_CD_DISABLE  | SDCC_SDIO_BUS_WIDTH_1BIT);
    }
    
    /* FN0: set bus width  */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_WRITE,
                          SDCC_SDIO_REG_BUS_CONTROL,
                          &data);
    
    return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_sdio_set_blksz
*
* Description:
*    This function is to set the specified block size if block mode is
*    supported. 
*
* Arguments:
*    devfn:     device function number.
*    blk_size:  block size to be set for that device function
*
* Returns:
*    returns status code.
******************************************************************************/
SDCC_STATUS
sdcc_sdio_set_blksz
(
    uint8     devfn,
    uint16    blksz
)
{
  uint8       data = 0 ;
  uint8       cc = sdcc_pdata.io.cc;
  SDCC_STATUS rc = SDCC_ERR_SDIO;
  uint16      bsize = 0;

  uint32      blk_in_bits = 0;
    
  /* return if block mode not supported */
  if((cc & SDCC_SDIO_CC_SMB) != SDCC_SDIO_CC_SMB)
    return rc;

  if(sdio_card == SDIO_PHILLIP_MANFID)
  {
    if(SDCC_SDIO_BLK_MODE == sdcc_pdata.block_mode)  
    {
      /* program the controller */
      blk_in_bits = sdcc_blk_in_bits(blksz);
      HWIO_OUTM( MCI_DATA_CTL,
                 HWIO_FMSK(MCI_DATA_CTL, BLOCKSIZE),
                 blk_in_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT );
    } 
  }
   
 

  /* program the device    */
  data = (uint8)blksz;
  (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                      SDCC_DATA_WRITE,
                      devfn * SDCC_SDIO_FBR_LENGTH + 0x10,
                      &data);

  data = (uint8)(blksz >> 8);
  (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                        SDCC_DATA_WRITE,
                        devfn * SDCC_SDIO_FBR_LENGTH + 0x11,
                        &data);


  (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                        SDCC_DATA_READ,
                        devfn * SDCC_SDIO_FBR_LENGTH + 0x11,
                        &data);
    
  bsize = data;
  (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                        SDCC_DATA_READ,
                        devfn * SDCC_SDIO_FBR_LENGTH + 0x10,
                        &data);

  bsize = (bsize << 8) | data;

  /* update ourself */
  if(SDCC_SDIO_FN0 == devfn) 
    sdcc_pdata.io.cccr_blksz = bsize;
  else
    sdcc_pdata.io.fn_blksz   = bsize;

  DPRINTF(("fn%d block size = 0x%x\n", devfn, bsize));

  return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_sdio_reset_device
*
* Description:
*   This function is to reset the device.
*
* Arguments:
* Returns:
******************************************************************************/
void
sdcc_sdio_reset_device(void)
{
    uint8 data = 0x08;
    
    /* write bit 3 of register 6 to soft reset the card. */
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_WRITE,
                          SDCC_SDIO_REG_IO_ABORT,
                          &data);
    
    sdio_mdelay(1);
}


/******************************************************************************
* Name: sdcc_sdio_abort
*
* Description:
*   This function is to abort the specified device function.
*
* Arguments:
* Returns:
******************************************************************************/
void
sdcc_sdio_abort(uint8 devfn)
{
    uint8  data = devfn;
    
    (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                          SDCC_DATA_WRITE,
                          SDCC_SDIO_REG_IO_ABORT,
                          &data);
    
    sdcc_udelay(250);
}


/******************************************************************************
* Name: sdcc_sdio_stop_xfer
*
* Description:
*    This function is to signal the controller to either stop or
*    abort. This is required in multi-block transfer as the controller
*    doesn't know which block is the last block 
*
* Arguments:
* Returns:
*
* Note:
*    In the SDCC controller, the interrupt period (in the host) will
*    start two cycles after the end of the command response of whichever
*    command that is sent after the abort bit is set. Based on the
*    above, the procedure is to set abort bit and then send some
*    command. Two clock cycles after this, the interrupt will start. 
******************************************************************************/
SDCC_STATUS
sdcc_sdio_stop_xfer(uint32 direction)
{
    
    SDCC_STATUS          rc       = SDCC_NO_ERROR;
    sdcc_cmd_type        sdcc_cmd;
    uint32               isave    = 0;

    /*----------------------------------------------------------------------*/

     /* tell the controller to stop */
    HWIO_OUTM(MCI_ABORT,
              HWIO_FMSK(MCI_ABORT, MCIABORT),
              0x1);

     
    /* code the dummy command */
    sdcc_cmd.cmd       = SD_CMD52_IO_RW_DIRECT;
    sdcc_cmd.cmd_arg   = (SDCC_DATA_READ << 31) | (SDCC_SDIO_FN0 << 28);     
    sdcc_cmd.resp_type = SDCC_RESP_SHORT;
    sdcc_cmd.resp[0]   = 0;

    /* Set the prog_scan to on for Atheros FIFO write */
    if((SDCC_DATA_WRITE == direction) && (FALSE == sdcc_pdata.enable_dma))
    {
      sdcc_cmd.prog_scan = 1;
    }
    else
    {
      sdcc_cmd.prog_scan = 0;
    }

    
    if(sdcc_pdata.enable_dma)
    {
      INTLOCK_SAV(isave);
    }

    rc = sdcc_command(&sdcc_cmd);


    if(sdcc_pdata.enable_dma)
    {
      INTFREE_SAV(isave);
    }

    if(rc)
    {
        DPRINTF(("SDCC sdio abort error, status = 0x%x.\n", rc));
    }

    return rc;

}

/******************************************************************************
* Name: sdcc_sdio_cmd52
*
* Description:
*    This function allow to read or write, depends on the specified
*    direction, of one byte data to the SDIO device function devfn. If
*    is commonly used to program the device register map.
*
* Arguments:
*    devfn     device function number
*    dir       direction, MMC_SDIO_READ or MMC_SDIO_WRITE
*    reg_addr  register address
*    wdata     data to be written 
*
* Returns:
*    returns data on successful read, 0xFFFF else.
******************************************************************************/
SDCC_STATUS
sdcc_sdio_cmd52
(
uint32   devfn,
uint32   dir,
uint32   reg_addr,
uint8   *pdata
)
{
   sdcc_cmd_type  sdcc_cmd;
   uint8          data = 0;
   SDCC_STATUS    rc = SDCC_NO_ERROR;

   sdcc_enter_crit_sect();
   if ( SDCC_DATA_WRITE == dir )
      data = *pdata;

   sdcc_cmd.cmd = SD_CMD52_IO_RW_DIRECT;
   sdcc_cmd.cmd_arg = (dir << 31) | (devfn << 28) |
                      (reg_addr << 9) | data;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.resp[0]   = 0;

   rc = sdcc_command(&sdcc_cmd);
   if (SDCC_NO_ERROR == rc)
   {
      if ( SDCC_DATA_READ == dir )
         *pdata = (uint8)(sdcc_cmd.resp[0]);
   }
   sdcc_leave_crit_sect();
   return rc;
}


/******************************************************************************
* Name: sdcc_sdio_set
*
* Description:
*    This function is provided to set various SDIO device
*    features. Currently supported features are: enable/disable
*    function, set block mode/block size and set bus width.
*
* Arguments:
*    devfn     device function number
*    type      features to be set
*    pdata     data associated with the feature 
*
* Returns:
*    returns status code.
******************************************************************************/
SDCC_STATUS
sdcc_sdio_set
(
uint8                   devfn,
SDIO_SET_FEATURE_TYPE   type,
void                   *pdata
)
{
   SDCC_STATUS status = SDCC_NO_ERROR;
   sdcc_enter_crit_sect();
   switch (type)
   {
   case SD_IO_FUNCTION_ENABLE:
      status = sdcc_sdio_enable_fn(devfn);
      break;

   case SD_IO_FUNCTION_DISABLE:        
      sdcc_sdio_disable_fn(devfn);
      break;

   case SD_SET_BLOCK_MODE:
      /* 0: byte mode, 1: block mode */
      sdcc_pdata.block_mode = *(uint32 *)pdata;
      break;

   case SD_IO_FUNCTION_SET_BLOCK_SIZE:
      {
         uint16 blksz = *(uint16 *)pdata;
         status = sdcc_sdio_set_blksz(devfn, blksz);
         break;
      }

   case SD_SET_OP_CODE:
      /* 0: fixed address, 1: incrementing address */
      sdcc_pdata.io.op_code = *(uint32 *)pdata;
      break;

   case SD_SET_DATA_TRANSFER_CLOCKS:
      break;

   case SD_SET_CARD_INTERFACE:
      {
         SDIO_CARD_INTERFACE   *card_intf = (SDIO_CARD_INTERFACE *)pdata;
         status = sdcc_sdio_set_buswidth(card_intf->InterfaceMode);
         break;
      }
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
   case SDCC_SDIO_WLAN_CONFIG:
      sdcc_pdata.is_wlan_param_set = TRUE;
      if (pdata != NULL)
      {
         sdcc_pdata.sdcc_wlan_params.clk = ((sdcc_wlan_param_t *)pdata)->clk;
         sdcc_pdata.sdcc_wlan_params.drive_strength = ((sdcc_wlan_param_t *)pdata)->drive_strength;
         sdcc_pdata.sdcc_wlan_params.init_delay = ((sdcc_wlan_param_t *)pdata)->init_delay;
         sdcc_pdata.sdcc_wlan_params.device = ((sdcc_wlan_param_t *)pdata)->device;
         sdcc_pdata.sdcc_wlan_params.vreg = ((sdcc_wlan_param_t *)pdata)->vreg;
      }
      else
      {
         /*set defaults*/
         sdcc_pdata.sdcc_wlan_params.clk = SDCC_CLK_9P6MHZ;
         sdcc_pdata.sdcc_wlan_params.drive_strength = SDCC_DRV_2MA;
         sdcc_pdata.sdcc_wlan_params.init_delay = 0;
         sdcc_pdata.sdcc_wlan_params.device = SDCC_SDIO_UNKNOWN;
         sdcc_pdata.sdcc_wlan_params.vreg = PM_VREG_MMC_ID;
      }
      break;
#endif
   default:
      break;
   }
   sdcc_leave_crit_sect();
   return status;
}

/******************************************************************************
* Name: sdcc_sdio_get
*
* Description:
*    This function provides the queried info.
*
* Arguments:
*    type      type of the info
*    pdata     data for the queried info
*    size      the size of the info
*    
* Returns:
*    returns status code.
******************************************************************************/
SDCC_STATUS
sdcc_sdio_get
(
SDIO_GET_INFO_TYPE     type,
void                  *pdata,
uint32                 size
)
{
   SDCC_STATUS status = SDCC_NO_ERROR;
   sdcc_enter_crit_sect();
   switch (type)
   {
   case SD_GET_RCA:
      memcpy(pdata, &(sdcc_pdata.rca[sdcc_pdata.curr_sd_drv]), size);
      break;

   case SD_GET_CARD_INTERFACE:
      {
         SDIO_CARD_INTERFACE   card_intf;
         card_intf.InterfaceMode = SDCC_INTERFACE_SDIO_4BIT; 
         card_intf.ClockRate     = 19200; /*in KHz */
         memcpy(pdata, &card_intf, size);
         break;
      }
   default:
      status = SDCC_ERR_SDIO; 
      break;
   }
   sdcc_leave_crit_sect();
   return status;
}

/******************************************************************************
* Name: sdcc_sdio_get_tuple
*
* Description:
*    This function reads out the tuple chain for the specified cis address.
*
* Arguments:
*    tpl_address   the address of the tuple chain
*    pbuff         the address of the buff to hold tuple data
*
* Returns:
*
******************************************************************************/
void sdcc_sdio_get_tuple(uint32 tpl_addr, uint8 *pbuff)
{
    uint8         tpl_code = 0, tpl_link = 0;
    uint32        i  = 0;
    SDCC_STATUS   rc = SDCC_NO_ERROR;
    uint8        *tpl_pdata;
    uint16        temp = 0;

    if(!pbuff)
        return;
    
    while(1)
    {
        rc = sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                             SDCC_DATA_READ,
                             tpl_addr,
                             &tpl_code);

        if( (SDCC_NO_ERROR != rc) || (tpl_code == 0xFF))
            break;
       
        tpl_addr++;
        rc = sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                             SDCC_DATA_READ,
                             tpl_addr,
                             &tpl_link);

        if( (SDCC_NO_ERROR != rc) || (tpl_link == 0xFF))
            break;

        tpl_pdata = pbuff;         
        for( i = 0; i < tpl_link; i++)
        {
            tpl_addr++;
            (void)sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                                  SDCC_DATA_READ,
                                  tpl_addr,
                                  pbuff);
            pbuff++;
        }
        tpl_addr++;

        /* currently, the only thing that we are interested in is
           manufacture code */
        if (SDCC_SDIO_CISTPL_MANFID == tpl_code)
        { 
            temp = *tpl_pdata++;
            temp = ((*tpl_pdata) << 8) | temp;

            sdcc_pdata.io.manfid = temp;
        }
    }

    return;
}

#ifdef SDCC_DEBUG_ON

/*************************************************/
/*    Debug Utility                              */
/*************************************************/
void
sdcc_sdio_read_cccr(void)
{
    uint32         i = 0;
    uint8          cccr[SDCC_SDIO_CCCR_MAX_BYTES];

    memset(cccr, 0, SDCC_SDIO_CCCR_MAX_BYTES);
    
    /* CMD52: SDIO read CCCR */
    for( i = 0; i < SDCC_SDIO_CCCR_MAX_BYTES; i++)
    {
        sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                        SDCC_DATA_READ,
                        i,
                        &(cccr[i]));
        
        if(cccr[i])
            DPRINTF(("cccr[%d] = 0x%x\n",i, cccr[i]));
    }    
}

void
sdcc_sdio_read_fbr(uint8 devfn)
{
    uint32         i = 0;
    uint8          fbr[SDCC_SDIO_FBR_MAX_BYTES];

    memset(fbr, 0, SDCC_SDIO_FBR_MAX_BYTES);
    
    /* CMD52: SDIO read FBR */
    for( i = 0; i < SDCC_SDIO_FBR_MAX_BYTES; i++)
    {
        sdcc_sdio_cmd52(SDCC_SDIO_FN0,
                        SDCC_DATA_READ,
                        devfn * SDCC_SDIO_FBR_LENGTH + i,
                        &(fbr[i]));
        
        if(fbr[i])
            DPRINTF(("fbr[%d] = 0x%x\n",i, fbr[i]));
    }    
}

#endif
/*lint -restore */
