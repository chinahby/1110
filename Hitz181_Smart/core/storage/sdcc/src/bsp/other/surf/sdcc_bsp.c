/**********************************************************************
* sdcc_bsp.c
*
* SDCC(Secure Digital Card Controller) driver BSP.
*
* This file implements the SDCC driver BSP for the board in use.
*
* Copyright (c) 2008
* Qualcomm Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header:
//source/qcom/qct/core/storage/sdcc/rel/04/src/bsp/surf/6240/sdcc_bsp.c#1 $
$DateTime: 2008/12/19 00:30:17 $ $Author: vijayj $

when         who     what, where, why
----------------------------------------------------------------------
2008-12-17   vj      Included sdcc_priv.h header file
2008-11-10   vin     Initial Version
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/

#include "sdcc_priv.h"
#include "sdcc_bsp.h"
#include "pm.h"
#include "customer.h"

/*==============================================================================
================================================================================
                           S D C C    B S P
==============================================================================*/

/*=============================================================================
 * FUNCTION      sdcc_bsp_vdd_control
 *
 * DESCRIPTION   This function turns ON/OFF the Vdd source to the card in
 *               use.
 * 
 * PARAMETERS    [IN] state : Desired power state: on or off
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_vdd_control (sdcc_bsp_vdd_ctl_type state)
{
   if(SDCC_BSP_VDD_ON == state)
   {
#if defined(FEATURE_WLAN) || defined(FEATURE_L4LINUX)
#error code not present
#endif

/* Turn on the MPP4 for the SC2X SDCC */
#if defined FEATURE_SDCC_TURN_ON_PWR_SC2X
      (void)pm_mpp_config_digital_output(PM_MPP_4,
                                         PM_MPP__DLOGIC__LVL_MSMP,
                                         PM_MPP__DLOGIC_OUT__CTRL_LOW);
#endif

/* Turn on the PMIC power supply */
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
#endif //FEATURE_WLAN_MODIFIED_FFA
#endif //FEATURE_SDCC_WLAN_CONFIG_API
   }
   else if (SDCC_BSP_VDD_OFF == state)
   {
      /* turn off the power supply to the card */
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
                          PM_VOTE_VREG_MMC_APP__MINI_SD );
#endif
#endif //FEATURE_SDCC_WLAN_CONFIG_API

/* Make MPP4 Tristate to turn off SD Vdd from FET */
#if defined FEATURE_SDCC_TURN_ON_PWR_SC2X
      (void) pm_mpp_config_i_sink( PM_MPP_4,
                                   PM_MPP__I_SINK__LEVEL_15mA,
                                   PM_MPP__I_SINK__SWITCH_DIS);
#endif
   }
   else
   {
      return SDCC_BSP_ERR_INVALID_PARAM;
   }
   return SDCC_BSP_NO_ERROR;
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_slot_interrupt_exists
 *
 * DESCRIPTION   This function returns TRUE if hardware supports detection
 *               of card removal / insertion.
 *
 * PARAMETERS    None
 *
 * RETURN VALUE  TRUE if supported, FALSE otherwise
 *
 *===========================================================================*/
boolean
sdcc_bsp_slot_interrupt_exists()
{
   return FALSE;
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_enable_slot_int
 *
 * DESCRIPTION   Setup the mechanism for detecting the card insertion/removal
 *               and associate an ISR to be called when a card is inserted
 *               or removed.
 *
 * PARAMETERS    [IN]: Pointer to the ISR function
 *
 * RETURN VALUE  None
 *
 *===========================================================================*/
void
sdcc_bsp_enable_slot_int(void *isr)
{
   (void) isr;
} /* sdcc_bsp_enable_slot_int */

