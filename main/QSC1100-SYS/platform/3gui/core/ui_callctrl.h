#ifndef _UI_CALLCTRL_H
#define _UI_CALLCTRL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
           U S E R   I N T E R F A C E   M A I N  M O D U L E 

GENERAL DESCRIPTION
  This file contains the call control related functionality from the 
  user interface.  This is used for both normal and ThinUI builds.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999 through 2007 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/ui_callctrl.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/07   ypmw    Remove ui_callctr remote api from misc group
05/24/06   cvs     Add call control for PS calls
04/12/06   cvs     Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "cm.h"

#ifdef FEATURE_UI_PBM
#include "pbmlib.h"
#endif

#ifdef FEATURE_PACKET_CALL_FDN_CHECK
#define UI_PACKET_CALL_FDN_STR   "*99#"
#endif

#define COREUTILS_NO_RESTRICTION 0x0000
#define COREUTILS_EFS            0x0001
#define COREUTILS_SPEED_DIAL     0x0002
#define COREUTILS_ADN_SLOT1      0x0004
#define COREUTILS_ADN_SLOT2      0x0008
#define COREUTILS_FDN_SLOT1      0x0010
#define COREUTILS_FDN_SLOT2      0x0020
#define COREUTILS_NO_EFS         0xFFFE
#define COREUTILS_NO_SPEED_DIAL  0xFFFD
#define COREUTILS_NO_ADN_SLOT1   0xFFFB
#define COREUTILS_NO_ADN_SLOT2   0xFFF7
#define COREUTILS_NO_FDN_SLOT1   0xFFEF
#define COREUTILS_NO_FDN_SLOT2   0xFFDF

/* <EJECT> */
/*===========================================================================
FUNCTION   get_call_type

DESCRIPTION
  Determine call type for a origination #

DEPENDENCIES
  None

RETURNS
  None, result will be put into call_type_ptr, otasp_code_ptr, and ecc_category

SIDE EFFECTS
  None
===========================================================================*/
void get_call_type(
  const cm_num_s_type     *orig_num_ptr,    // pointer to origination #
  cm_call_type_e_type     *call_type_ptr,   // associated call type, save result
  cm_activate_code_e_type *otasp_code_ptr,  // OTASP activation code(OTASP call)
  uint8                   *ecc_category     // emergency category (emergency call)
);

/* <EJECT> */
/*===========================================================================
FUNCTION UI_SET_RESTRICTIONS

DESCRIPTION
  This function sets the restriction_bitmask to the value from Core.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_set_restrictions(byte restriction_bitmask);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION ui_set_restrictions */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/*===========================================================================
FUNCTION UI_RESTRICT_DIALING_TO

DESCRIPTION
  This function checks the restriction_bitmask that was set with the ui_set
  restrictions function above and returns TRUE if the PBM file given has
  the bit in the mask set which corresponds to this restriction.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_restrict_dialing_to(pbm_device_type pbm_file);

/*===========================================================================
FUNCTION ui_nc_type_allowed_and_callcontrol_cb

DESCRIPTION
  The new NC function that checks all information about a number, including call
  control will have the following design.

  First, check if its an emergency call.
   If Emergency, return that the call is an emergency number, and CC not required.

  Second, the UI has non-required by specification user limits, specifically:
    Limit calls to numbers in the NV phone book(s)
    Limit calls to SIM1 ADN
    Limit calls to SIM2 ADN 
  If any of these limitations are set and the number does not match these numbers,
    Return the call is not allowed on either SIM.

  Third if the call is still allowed
   If either subscription uses SIM1:
     Do FDN check on SIM1
       If the call is not allowed on SIM1, mark the call as not allowed for modes whose subscription comes from SIM1.

   If either subscription uses SIM2
      Do FDN check on SIM2
         If the call is not allowed on SIM2, mark the call as not allowed for modes whose subscription comes from SIM2.

  If the call is not allowed now on either mode, 
    Return the call is not allowed on either SIM.

 Fourth Call the following do_cc function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ui_nc_type_allowed_and_callcontrol_cb (
  cm_call_id_type                       call_id, /* call id */
  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */
  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */
  cm_call_type_e_type                   call_type,
    /* call type */
  cm_num_s_type                         *orig_num_ptr,
    /* orginating number */
  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */
  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func
    /* function to be called when processing is complete */
#ifdef CM_API_GW_PS_CC
  ,
  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params
    /* Call control parameters for Packet data call
    */
#endif
#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
  ,
  const cm_cc_generic_params_s_type           *cc_generic_params
#endif
);

/*===========================================================================
FUNCTION ui_nc_call_control_preferences 

DESCRIPTION
 Type for call control preferences function to be registered by a client.  CM will
 call this function after call control is complete to mediate between which modes
 to use in the case where the call is allowed on both modes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cm_cc_mode_pref_e_type ui_nc_call_control_preferences (

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /* cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /* gw information for the call */ 

  cm_num_s_type                          *orig_num_ptr,
    /* Original number information before call control */

  cm_call_type_e_type                    call_type,
    /* The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */

);

#endif //_UI_CALLCTRL_H
