#ifndef SRCH_RX_DIV_H
#define SRCH_RX_DIV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H    R X   D I V  ---  H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains external header information for SRCH to use the RX DIV
  state machine.

EXTERNALIZED FUNCTIONS  (all SRCH regional)
 
  srch_rx_div_is_enabling - returns true if diversity is active or enabling.
  srch_rx_div_is_enabled - returns true if we're currently doing rx diversity.
  srch_rx_div_is_disabled - returns true if diversity is not in progress.
  srch_rx_div_allowed_for_bc - returns true if div is allowed for band class.
  srch_rx_div_set_ctrl  - sets RX diversity nv item
  srch_rx_div_is_capable - returns true if this phone supports rx diversity.
  srch_rx_div_get_algo - returns the diveristy combining algorithm type.
  srch_rx_div_is_auto_enabled - returns true if rx diversity autoswitching is
                                enabled.
  srch_rx_div_is_off_with_no_ffpc - returns where diversity is disabled with
                                    no ffpc. 
  srch_rx_div_is_bc_enable_mask_enabled - returns true if band class diversity
                                          switching is enabled.
  srch_rx_div_set_bc_enable_mask - sets the band class enable mask RX diversity
                                   nv item.
  srch_rx_div_is_bc_enable_mask_enabled - returns true if band class diversity
                                          switching is enabled.
  srch_rx_div_bc_enable_mask - returns the band class enable mask.
  srch_rx_div_request - requests that the SRCH_RX_DIV state machine
                        try and do diversity.
  srch_rx_div_stop - tells the SRCH_RX_DIV state machine to stop diversity.
  srch_rx_div_update_state - updates the rx diversity state depending on band
                             class and whether ffpc is enabled.
  srchrdiv_set_ftm_second_chain_test - sets the FTM test mode for using 2nd
                                       Rx only.
  srchrdiv_get_ftm_second_chain_test - returns true if the FTM 2nd chain test
                                       mode is set.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2005 - 2010
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_rx_div.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/10   pk      Added a func to return true if div is active or enabling.
11/13/08   adw     Added external function srch_rx_div_allowed_for_bc().
05/01/07   tjc     Add bit to rx diversity nv item to enable/disable rx
                   diversity when FFPC is not available.
                   Add bit to rx diversity nv item to enabled/disable rx
                   diversity on a BC basis.
04/30/07   sst     Update RX Diversity featurization
02/13/07   tjc     Changed function name to match what it is doing
                   Start/Stop now enable/disable autodiversity
11/10/06   tjc     Add bit to rx diversity nv item to enable/disable
                   autoswitching
08/29/05   ejv     Remove featurization from *ftm_second_chain_test.
05/31/05   rng     Merged from SHDR Sandbox.
01/05/05   gs      Implementation, first cut.
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_CDMA_RX_DIVERSITY

#include "srch_rx_t.h"
#include "stm.h"

/*-------------------------------------------------------------------------
      External Declarations
-------------------------------------------------------------------------*/
/* State Machine */
extern stm_state_machine_type DIV_SM;

/*-------------------------------------------------------------------------
      Data types
-------------------------------------------------------------------------*/

/* Diversity algorithm type */
typedef enum
{
  SRCH_RX_DIV_ALGO_MMSE,   /* MMSE combining algorithm */

  SRCH_RX_DIV_ALGO_SPW     /* Simple pilot weighting combining
                              algorithm (like MRC) */

} srch_rx_div_algo_enum_type;


/*-------------------------------------------------------------------------
      Prototypes
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION       SRCH_RX_DIV_IS_ENABLING

DESCRIPTION    This function returns true if we're currently enabling or
               actively doing rx diversity.

DEPENDENCIES   None

RETURN VALUE   TRUE if the div state machine is in the DIV_REQUESTED,
               DIV_INITING, or DIV_ENABLED state and stop_requested is
               not true,  FALSE otherwise.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_rx_div_is_enabling( void );

/*===========================================================================

FUNCTION       SRCH_RX_DIV_IS_ENABLED

DESCRIPTION    This function returns true if we're currently doing rx
               diversity.

DEPENDENCIES   None

RETURN VALUE   TRUE if the div state machine is in the DIV_ENABLED state,
               FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_rx_div_is_enabled( void );

/*===========================================================================

FUNCTION       SRCH_RX_DIV_IS_DISABLED

DESCRIPTION    This function checks to see if the RX diversity state machine
               is disabled.  Note that having the state machine enabled
               doesn't necessarily mean that RX diversity is in progress...
               we could be waiting for an RF lock or waiting for the hardware
               to warm up.

DEPENDENCIES   None

RETURN VALUE   TRUE if the RX DIVERSITY state machine is currently disabled.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_rx_div_is_disabled( void );

/*===========================================================================

FUNCTION SRCH_RX_DIV_ALLOWED_FOR_BC

DESCRIPTION   This function returns whether RX diversity is allowed depending
              on the band class that is passed in

DEPENDEDCIES  None.

RETURN VALUE  boolean - TRUE if diversity is allowed for the band class
                        FALSE otherwise

SIDE EFFECTS  None.

===========================================================================*/
extern boolean srch_rx_div_allowed_for_bc
(
  srch_rx_band_type  band
);

/*===========================================================================

FUNCTION        SRCH_RX_DIV_SET_CTRL

DESCRIPTION     This function sets RX diversity nv item

                Diversity CTRL NV Item details:
                -----------------------------------
                B0     = RX Diversity Enable/Disable
                B3:B1  = RX Diversity Combining Algorithm
                         000 : MMSE
                         001 : Simple Pilot Weights
                B4     = RX Diversity Autoswitching Enable/Disable
                B5     = RX Diversity Disabled/Enabled with no FFPC
                B6     = BC Diversity switching Enable/Disable
                B15:B7 = Reserved

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void srch_rx_div_set_ctrl
(
  uint16 rx_div_nv /* NV item which controls the Rx diversity */
);

/*===========================================================================

FUNCTION        SRCH_RX_DIV_IS_CAPABLE

DESCRIPTION     This function returns the rx diversity capability of the
                terminal

DEPENDENCIES    None

RETURN VALUE    TRUE    : Rx Diversity capable
                FALSE   : Rx Diversity incapabale

SIDE EFFECTS    None

===========================================================================*/
extern boolean srch_rx_div_is_capable( void );

/*===========================================================================

FUNCTION        SRCH_RX_DIV_GET_ALGO

DESCRIPTION     This function returns the diversity algorithm

DEPENDENCIES    None

RETURN VALUE    Algorithm type

SIDE EFFECTS    None.

===========================================================================*/
extern srch_rx_div_algo_enum_type srch_rx_div_get_algo( void );

#ifdef FEATURE_SRCH_RXDIV_AUTOSWITCH
/*===========================================================================

FUNCTION        SRCH_RX_DIV_IS_AUTO_ENABLED

DESCRIPTION     This function returns the whether rx diversity autoswitching
                is enabled on the terminal

DEPENDENCIES    None

RETURN VALUE    TRUE    : Rx Diversity Autoswitching is available
                FALSE   : Rx Diversity Autoswitching is not available

SIDE EFFECTS    None

===========================================================================*/
extern boolean srch_rx_div_is_auto_enabled( void );
#endif /* FEATURE_SRCH_RXDIV_AUTOSWITCH */

/*===========================================================================

FUNCTION        SRCH_RX_DIV_IS_OFF_WITH_NO_FFPC

DESCRIPTION     This function returns whether rx diversity is enabled with just
                FPC.

DEPENDENCIES    None

RETURN VALUE    TRUE    : Rx Diversity is disabled with no FFPC
                FALSE   : Rx Diversity is enabled with no FFPC

SIDE EFFECTS    None

===========================================================================*/
extern boolean srch_rx_div_is_off_with_no_ffpc( void );

#ifdef FEATURE_SRCH_RXDIV_BC_SWITCHING
/*===========================================================================

FUNCTION        SRCH_RX_DIV_IS_BC_ENABLE_MASK_ENABLED

DESCRIPTION     This function returns whether the band class enable mask is
                enabled

DEPENDENCIES    None

RETURN VALUE    TRUE    : BC Diversity Switching Enable Mask enabled
                FALSE   : BC Diversity Switching Enable Mask disabled

SIDE EFFECTS    None

===========================================================================*/
boolean srch_rx_div_is_bc_enable_mask_enabled( void );
#endif /* FEATURE_SRCH_RXDIV_BC_SWITCHING */

#ifdef FEATURE_SRCH_RXDIV_BC_SWITCHING
/*===========================================================================

FUNCTION        SRCH_RX_DIV_SET_BC_ENABLE_MASK

DESCRIPTION     This function sets RX diversity band class enabled mask nv item.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void srch_rx_div_set_bc_enable_mask
(
  uint32 bc_en_mask_nv /* NV item which controls the band class enable mask */
);
#endif /* FEATURE_SRCH_RXDIV_BC_SWITCHING */

#ifdef FEATURE_SRCH_RXDIV_BC_SWITCHING
/*===========================================================================

FUNCTION        SRCH_RX_DIV_BC_ENABLE_MASK

DESCRIPTION     This function returns the band class enable mask

DEPENDENCIES    None

RETURN VALUE    uint32

SIDE EFFECTS    None

===========================================================================*/
extern uint32 srch_rx_div_bc_enable_mask( void );
#endif /* FEATURE_SRCH_RXDIV_BC_SWITCHING */

/*===========================================================================

FUNCTION SRCH_RX_DIV_REQUEST

DESCRIPTION   Requests diversity and enables the autoswitching algorithm if
              requested.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
extern void srch_rx_div_request
(
  boolean auto_div   /* Whether autodiversity is requested */
);

/*===========================================================================

FUNCTION       SRCH_RX_DIV_STOP

DESCRIPTION    Stop diversity and disables the autoswitching algorithm.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_div_stop( void );

/*===========================================================================

FUNCTION SRCH_RX_DIV_UPDATE_STATE

DESCRIPTION   This function updates the RX diversity state depending on the
              band class and whether ffpc is enabled.

DEPENDEDCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
extern void srch_rx_div_update_state
(
  srch_rx_band_type  band,
  boolean            fpc_enabled
);
#endif /* FEATURE_CDMA_RX_DIVERSITY */

/* the following functions are defined outside of the RxDiv featurization to
   remove dependency requirements */

/*===========================================================================

FUNCTION       SRCHRDIV_SET_FTM_SECOND_CHAIN_TEST

DESCRIPTION    Sets the FTM test mode for using 2nd Rx only.

DEPENDENCIES

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srchrdiv_set_ftm_second_chain_test( boolean test_mode );

/*===========================================================================

FUNCTION       SRCHRDIV_GET_FTM_SECOND_CHAIN_TEST

DESCRIPTION    Retrieves the FTM test mode for using 2nd Rx only.

DEPENDENCIES

RETURN VALUE   true if the FTM second chain test mode is set.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srchrdiv_get_ftm_second_chain_test( void );

#endif /* SRCH_RX_DIV_H */

