/*===========================================================================

              S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the protocol stacks.


  Organization of the file:

    The file is divided into multiple sections. Each section contains
    validation routines for a particular entity.  The following entities
    are defined in this file:

        Service Indicators
        System Identifiers
        Network Lists
        Mobility Management Information

EXTERNALIZED FUNCTIONS
  All functions are externalized except for sys_plmn_id_digit_is_valid.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None


Copyright (c) 2002 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "sys.h"
#include "string.h"
#include "cmidbg.h" 

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/sys.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/09   ks      Validate spare_bits field in the mm_information to indicate 
                   no. of spare bits in end octet of n/w names(full/short)
08/07/07   ic      Lint clean up
11/15/06   pk      Added support for wild card MNC for GW systems
03/10/06   pk      Modified sys_sys_id_match to check for MCC and IMSI_11_12
                   for IS 95 systems
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/18/05   jqi     Replaced cmdbg.h with cmidbg.h file to reduce header file 
                   dependency.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/26/05   up      Updated sys_plmn_match to compare NA PLMNs of 2 digit MNCs 
                   when 3rd digit MNC is transmitted as '0'/'F'.
05/12/05   ic      Removed bcmcs_srv_status_is_srv()
04/04/05   ic      Removed unnecessary Lint comments 
03/22/05   up      Function sys_mcc_same_country is added to extend MCC
                   matching for NA MCC range 310-316.
03/09/05   kwa     Replaced code within sys_plmn_match with the code in
                   sys_hplmn_match.  Fixed a bug in sys_plmn_match so it
                   properly compares two PLMN IDs with 2 digit MNCs within
                   the North American country codes.
08/10/04   kwa     Added sys_hplmn_match.  Moved the functionality added to
                   sys_plmn_match in the previous update to sys_hplmn_match.
                   Removed the North American HPLMN comparison requirements
                   from sys_plmn_match.
06/24/04   kwa     Updated the function sys_plmn_match to support the PLMN
                   matching requirements for North American GSM networks as
                   defined in Annex A of 3GPP TS 23.122.
03/15/04   ws      Fixed bug with system id comparison.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
11/06/03   sj      Updated sys_srv_status_is_srv() for limited_regional srv.
10/24/03   ic      Changes to support common sys.c across targets
05/05/03   kwa     Updated the logic in sys_mm_information_is_valid for
                   determining if an sys_mm_information_s_type is valid to
                   account for optional elements within the structure.
                   Modified several functions to pass pointers rather than
                   structures to minimize stack utilization.
05/05/03   vt      Removed #if 0 stuff.
04/28/03   AT      Lint cleanup.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Modified sys_user_pref_plmn_list_info_is_valid() to only
                   validate plmn.
11/22/02   MKS     Added sys_network_name_coding_scheme_is_valid() to check
                   if a valid coding scheme is used to encode short name or
                   full name of the network.
04/01/02   PRK     Initial version
===========================================================================*/

/*==============================================================================

                     S E R V I C E   I N D I C A T O R S

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_oprt_mode_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_oprt_mode_e_type  value          Value that is to be checked
                                       for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_oprt_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_oprt_mode_is_valid
(
  sys_oprt_mode_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_OPRT_MODE_NONE &&
                    value < SYS_OPRT_MODE_MAX    );

  return valid;
} /* sys_oprt_mode_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_sys_mode_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_sys_mode_e_type  value          Value that is to be checked
                                      for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_sys_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_sys_mode_is_valid
(
  sys_sys_mode_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SYS_MODE_NONE &&
                    value < SYS_SYS_MODE_MAX    );

  return valid;
} /* sys_sys_mode_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_band_class_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_band_class_e_type  value        Value that is to be checked
                                      for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_band_class_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_band_class_is_valid
(
  sys_band_class_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_BAND_CLASS_NONE &&
                    value < SYS_BAND_CLASS_MAX    );

  return valid;
} /* sys_band_class_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_block_or_system_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_blksys_e_type  value            Value that is to be checked
                                      for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_blksys_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_block_or_system_is_valid
(
  sys_blksys_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_BLKSYS_NONE &&
                    value < SYS_BLKSYS_MAX    );

  return valid;
} /* sys_blksys_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_srv_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_srv_status_is_valid
(
  sys_srv_status_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SRV_STATUS_NONE &&
                    value < SYS_SRV_STATUS_MAX    );

  return valid;
} /* sys_srv_status_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_srv

DESCRIPTION

  Function that verifies that the input value is indicates any service.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates service( TRUE)
           or no service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_NO_SRV  ||
      value == SYS_SRV_STATUS_PWR_SAVE )
  {
    return FALSE;
  }


  return TRUE;

} /* sys_srv_status_is_srv */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_full_srv

DESCRIPTION

  Function that verifies that the input value is indicates full service.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates full service( TRUE)
           or other service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_full_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_SRV )
  {
    return TRUE;
  }


  return FALSE;

} /* sys_srv_status_is_full_srv */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_domain_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_srv_domain_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_service_domain_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_srv_domain_is_valid
(
  sys_srv_domain_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SRV_DOMAIN_NONE &&
                    value < SYS_SRV_DOMAIN_MAX    );

  return valid;
} /* sys_srv_domain_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_capability_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_srv_capability_e_type   value       Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_srv_capability_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_srv_capability_is_valid
(
  sys_srv_capability_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SRV_CAPABILITY_NONE &&
                    value < SYS_SRV_CAPABILITY_MAX    );

  return valid;
} /* sys_srv_capability_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_roam_status_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_roam_status_e_type value                   Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_roam_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_roam_status_is_valid
(
  sys_roam_status_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_ROAM_STATUS_NONE &&
                    value < SYS_ROAM_STATUS_MAX    );

  return valid;
} /* sys_roam_status_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_radio_access_tech_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_radio_access_tech_e_type value             Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_radio_access_tech_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_radio_access_tech_is_valid
(
  sys_radio_access_tech_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_RAT_NONE &&
                    value < SYS_RAT_MAX    );

  return valid;
} /* sys_radio_access_tech_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_sim_state_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_sim_state_e_type value                     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_sim_state_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_sim_state_is_valid
(
  sys_sim_state_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SIM_STATE_NONE &&
                    value < SYS_SIM_STATE_MAX    );

  return valid;
} /* sys_sim_state_is_valid */


/*==============================================================================

                     S Y S T E M   I D E N T I F I E R

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_digit_is_valid

DESCRIPTION

  Function that checks if the input digit is between 0 and 9 or equal to 0xF.

PARAMETERS

  byte       digit                     Digit to be checked for validity.

RETURN VALUE

  boolean  Flag indicating if the digit is valid (TRUE) or not (FALSE).

==============================================================================*/
static boolean sys_plmn_id_digit_is_valid
(
  byte digit
)
{
  return (boolean)( (digit <= 0x9) || (digit == 0xF) );
} /* sys_plmn_id_digit_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_get_digits

DESCRIPTION

  Function that returns the each of the digits in the MCC and MNC contained in
  a PLMN ID as integer values.

PARAMETERS

  sys_plmn_id_s_type                   plmn provided as input from which
            plmn                       to read the MCC and MNC.
  uint32*   mcc_digit_1_ptr            First digit of the MCC.
  uint32*   mcc_digit_2_ptr            Second digit of the MCC.
  uint32*   mcc_digit_3_ptr            Third digit of the MCC.
  uint32*   mnc_digit_1_ptr            First digit of the MNC.
  uint32*   mnc_digit_2_ptr            Second digit of the MNC.
  uint32*   mnc_digit_3_ptr            Third digit of the MNC.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_get_digits
(
  sys_plmn_id_s_type plmn_id,
  uint32*            mcc_digit_1_ptr,
  uint32*            mcc_digit_2_ptr,
  uint32*            mcc_digit_3_ptr,
  uint32*            mnc_digit_1_ptr,
  uint32*            mnc_digit_2_ptr,
  uint32*            mnc_digit_3_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mcc_digit_1_ptr  != NULL); 
  CM_ASSERT(mcc_digit_2_ptr  != NULL); 
  CM_ASSERT(mcc_digit_3_ptr  != NULL); 
  CM_ASSERT(mnc_digit_1_ptr  != NULL); 
  CM_ASSERT(mnc_digit_2_ptr  != NULL); 
  CM_ASSERT(mnc_digit_3_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *mcc_digit_1_ptr = plmn_id.identity[0] & 0x0F;
  *mcc_digit_2_ptr = plmn_id.identity[0] / 0x10;
  *mcc_digit_3_ptr = plmn_id.identity[1] & 0x0F;

  *mnc_digit_1_ptr = plmn_id.identity[2] & 0x0F;
  *mnc_digit_2_ptr = plmn_id.identity[2] / 0x10;
  *mnc_digit_3_ptr = plmn_id.identity[1] / 0x10;

}



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_get_mcc_mnc

DESCRIPTION

  Function that returns the MCC and MNC contained in a PLMN ID as integer
  values.

PARAMETERS

  sys_plmn_id_s_type                         plmn provided as input from which
                 plmn                        to read the MCC and MNC.
  boolean*       plmn_is_undefined_ptr       Flag indicating that the input plmn
                                             contains a defined MCC and MNC
                                             (FALSE) or was set to 0xFFFFFF
                                             (TRUE).
  boolean*       mnc_includes_PCS_digit_ptr  Flag indicating whether the MNC
                                             included the third PCS digit (TRUE)
                                             or not (FALSE)
  sys_mcc_type*  mcc_ptr                     MCC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.
  sys_mnc_ptr*   mnc_ptr                     MNC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_get_mcc_mnc
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
)
{

  sys_mcc_type        mcc_digit_1;
  sys_mcc_type        mcc_digit_2;
  sys_mcc_type        mcc_digit_3;

  sys_mnc_type        mnc_digit_1;
  sys_mnc_type        mnc_digit_2;
  sys_mnc_type        mnc_digit_3;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_is_undefined_ptr       != NULL); 
  CM_ASSERT(mnc_includes_pcs_digit_ptr  != NULL); 
  CM_ASSERT(mcc_ptr  != NULL); 
  CM_ASSERT(mnc_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (sys_plmn_id_is_undefined (plmn))
  {
    *plmn_is_undefined_ptr = TRUE;
    *mcc_ptr               = 0;
    *mnc_ptr               = 0;
  }
  else
  {
    *plmn_is_undefined_ptr = FALSE;

    mcc_digit_1 = plmn.identity[0] & 0x0F;
    mcc_digit_2 = plmn.identity[0] / 0x10;
    mcc_digit_3 = plmn.identity[1] & 0x0F;

    mnc_digit_1 = plmn.identity[2] & 0x0F;
    mnc_digit_2 = plmn.identity[2] / 0x10;
    mnc_digit_3 = plmn.identity[1] / 0x10;

    *mcc_ptr = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;

    if (mnc_digit_3 == 0xF)
    {
      *mnc_includes_pcs_digit_ptr = FALSE;
      *mnc_ptr = 10 * mnc_digit_1 + mnc_digit_2;
    }
    else
    {
      *mnc_includes_pcs_digit_ptr = TRUE;
      *mnc_ptr = 100 * mnc_digit_1 + 10 * mnc_digit_2 + mnc_digit_3;
    }
  }

} /* sys_plmn_get_mcc_mnc */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_set_mcc_mnc

DESCRIPTION

  Function that fills a PLMN ID with the MCC and MNC provided as integer values.

PARAMETERS

  boolean           mnc_includes_pcs_digit  Flag indicating whether the MNC
                                            includes the third PCS digit (TRUE)
                                            or not (FALSE)
  sys_mcc_type      mcc                     MCC as an integer value (0 - 999).
  sys_mnc_type      mnc                     MCC as an integer value (0 - 99 or
                                            0 - 999 if the MCS digit is
                                            included).
  sys_plmn_id_type* plmn_ptr                Pointer to the plmn to populate
                                            input from which to read the MCC and
                                            MNC.

RETURN VALUE

  boolean  Flag indicating whether or not the plmn_ptr was successfully filled
           with values provided for the MCC and MNC.  If TRUE then *plmn_ptr
           was successfully filled.  If FALSE then the input parameters were
           determined to be out of range and *plmn_ptr was set to 0xFFFFFF.

==============================================================================*/
boolean sys_plmn_set_mcc_mnc
(
  boolean               mnc_includes_pcs_digit,
  sys_mcc_type          mcc,
  sys_mnc_type          mnc,
  sys_plmn_id_s_type*   plmn_ptr
)

{
  sys_mcc_type          mcc_digit_1;
  sys_mcc_type          mcc_digit_2;
  sys_mcc_type          mcc_digit_3;

  sys_mnc_type          mnc_digit_1;
  sys_mnc_type          mnc_digit_2;
  sys_mnc_type          mnc_digit_3;

  if ( (mcc > 999) ||
       (mnc_includes_pcs_digit && mnc > 999) ||
       (!mnc_includes_pcs_digit && mnc > 99) )
  {
    sys_plmn_undefine_plmn_id (plmn_ptr);
    return FALSE;
  }

  plmn_ptr->identity[0] = 0x00;
  plmn_ptr->identity[1] = 0x00;
  plmn_ptr->identity[2] = 0x00;

  mcc_digit_1 = mcc/100;
  mcc_digit_2 = (mcc - 100 * mcc_digit_1) / 10;
  mcc_digit_3 = mcc - 100 * mcc_digit_1 - 10 * mcc_digit_2; /*lint !e834 */

  if (mnc_includes_pcs_digit)
  {
    mnc_digit_1 = mnc/100;
    mnc_digit_2 = (mnc - 100 * mnc_digit_1) / 10;
    mnc_digit_3 = mnc - 100 * mnc_digit_1 - 10 * mnc_digit_2; /*lint !e834 */
  }
  else
  {
    mnc_digit_1 = mnc/10;
    mnc_digit_2 = mnc - 10 * mnc_digit_1;
    mnc_digit_3 = 0xF;
  }

  plmn_ptr->identity[0] = (byte)((mcc_digit_2 << 4) + mcc_digit_1);
  plmn_ptr->identity[1] = (byte)((mnc_digit_3 << 4) + mcc_digit_3);
  plmn_ptr->identity[2] = (byte)((mnc_digit_2 << 4) + mnc_digit_1);

  return TRUE;
} /* sys_plmn_set_mcc_mnc */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_is_valid

DESCRIPTION

  Function that checks if each of the digits in the input plmn are between 0
  and 9 or equal to 0xF.

PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to validate.

RETURN VALUE

  boolean  Flag indicating if the plmn is valid (TRUE) or not (FALSE).

==============================================================================*/
boolean sys_plmn_id_is_valid
(
  sys_plmn_id_s_type  plmn
)
{
  return (boolean)( sys_plmn_id_digit_is_valid( (byte)(plmn.identity[0] & 0x0F) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[0] / 0x10) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[1] & 0x0F) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[2] & 0x0F) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[2] / 0x10) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[1] / 0x10) ) );
} /* sys_plmn_id_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_is_undefined

DESCRIPTION

  Function that checks if each of the digits in the input plmn is equal to
  0xF.  In other words, an undefined plmn contains 0xFFFFFF.

PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to check.

RETURN VALUE

  boolean  Flag indicating if the plmn is undefined (TRUE) or not (FALSE).

==============================================================================*/
boolean sys_plmn_id_is_undefined
(
  sys_plmn_id_s_type  plmn
)
{
  return (boolean)( (plmn.identity[0] == 0xFF) &&
                    (plmn.identity[1] == 0xFF) &&
                    (plmn.identity[2] == 0xFF));

} /* sys_plmn_id_is_undefined */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_undefine_plmn_id

DESCRIPTION

  Function that sets each of the digits in the plmn_id to 0xF.  The resulting
  undefined *plmn_ptr contains 0xFFFFFF.

PARAMETERS

  sys_plmn_id_s_type*  plmn_ptr           Pointer to the PLMN ID to undefine.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_undefine_plmn_id
(
  sys_plmn_id_s_type*  plmn_ptr
)
{
  plmn_ptr->identity[0] = 0xFF;
  plmn_ptr->identity[1] = 0xFF;
  plmn_ptr->identity[2] = 0xFF;
} /* sys_plmn_undefine_plmn_id */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_match

DESCRIPTION

  Function that compares two PLMN IDs and determines if they are equal.
  plmn_2 is the ID of the PLMN broadcast by the network and plmn_1 is the
  ID of the PLMN that it is being compared against (e.g., the HPLMN stored
  on the SIM).  If the third MNC digit in the plmn_2 is 0xF, then the plmn_2
  is assumed to contain a two digit MNC so the third MNC digit is not compared.
  This function differs from sys_plmn_match in that it implements a further
  requirement defined in 3GPP TS23.122 Annex A for comparing the HPLMN in
  North American PCS networks.

PARAMETERS

  sys_plmn_id_s_type  plmn_1                  The first PLMN ID.
  sys_plmn_id_s_type  plmn_2                  The second PLMN ID.

RETURN VALUE

  boolean  Flag indicating whether the two PLMN IDs are equal (TRUE) or
  not (FALSE).

==============================================================================*/
boolean sys_plmn_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
)
{
  uint32 sim_mcc_digit_1  = plmn_1.identity[0] & 0x0F;
  uint32 sim_mcc_digit_2  = plmn_1.identity[0] / 0x10;
  uint32 sim_mcc_digit_3  = plmn_1.identity[1] & 0x0F;

  uint32 sim_mnc_digit_1  = plmn_1.identity[2] & 0x0F;
  uint32 sim_mnc_digit_2  = plmn_1.identity[2] / 0x10;
  uint32 sim_mnc_digit_3  = plmn_1.identity[1] / 0x10;

  uint32 bcch_mcc_digit_1 = plmn_2.identity[0] & 0x0F;
  uint32 bcch_mcc_digit_2 = plmn_2.identity[0] / 0x10;
  uint32 bcch_mcc_digit_3 = plmn_2.identity[1] & 0x0F;

  uint32 bcch_mnc_digit_1 = plmn_2.identity[2] & 0x0F;
  uint32 bcch_mnc_digit_2 = plmn_2.identity[2] / 0x10;
  uint32 bcch_mnc_digit_3 = plmn_2.identity[1] / 0x10;

  boolean value = FALSE;

  if ( (sim_mcc_digit_1 == bcch_mcc_digit_1) &&
       (sim_mcc_digit_2 == bcch_mcc_digit_2) &&
       (sim_mcc_digit_3 == bcch_mcc_digit_3) )
  {
    /* Check for wildcard MNC, if MNC if FFF, ignore it
    */
    if( sim_mnc_digit_1 == 0xF && sim_mnc_digit_2 == 0xF && sim_mnc_digit_3 == 0xF )
    {
      return TRUE;
    }
    if (bcch_mnc_digit_3 == 0xF)
    {
      if ( (bcch_mcc_digit_1 == 3) &&
           (bcch_mcc_digit_2 == 1) &&
           (bcch_mcc_digit_3 <= 6) )
      {
        if ((sim_mnc_digit_3 == 0) || (sim_mnc_digit_3 == 0xF))
        {
          value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                             (sim_mnc_digit_2 == bcch_mnc_digit_2) );
        }
      }
      else
      {
        value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                           (sim_mnc_digit_2 == bcch_mnc_digit_2) );
      }
    }
    else
    {
      if ( (bcch_mcc_digit_1 == 3) &&
           (bcch_mcc_digit_2 == 1) &&
           (bcch_mcc_digit_3 <= 6) &&
           (bcch_mnc_digit_3 == 0) &&
           (sim_mnc_digit_3 == 0xF) )
      {
        value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                           (sim_mnc_digit_2 == bcch_mnc_digit_2) );        
      }
      else
      {
        value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                           (sim_mnc_digit_2 == bcch_mnc_digit_2) &&
                           (sim_mnc_digit_3 == bcch_mnc_digit_3) );
      }
    }
  }

  return value;
} /* sys_hplmn_match */


/*==============================================================================

FUNCTION NAME

  sys_mcc_same_country

DESCRIPTION

  Function to check if the MCCs are of same country. MCCs from NA with
  different values (in range 310-316) are considered as MCCs of same country.

RETURN VALUE

  TRUE if MCCs are of same country, FALSE otherwise

==============================================================================*/

boolean sys_mcc_same_country
(
  uint32  mcc1,
  uint32  mcc2
)
{
  /*
  ** MCC in NA (i.e. in range 310-316) with different mcc values 
  ** are also considered as mcc of same country (NA).
  */
  return ((mcc1 == mcc2) ? TRUE : 
                          ((mcc1 >= 310) && (mcc1 <= 316) && (mcc2 >= 310) && (mcc2 <= 316)));
}

/*===========================================================================

FUNCTION sys_undefine_sys_id

DESCRIPTION
  This function undefines or initializes a system identifier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sys_undefine_sys_id(

  sys_sys_id_s_type     *sys_id_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sys_id_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) memset( sys_id_ptr, 0, sizeof( sys_sys_id_s_type ) );
  sys_id_ptr->id_type = SYS_SYS_ID_TYPE_UNDEFINED;

} /* sys_undefine_sys_id */



/*===========================================================================

FUNCTION sys_sys_id_match

DESCRIPTION
  This function compares two system identifiers.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two PLMN identifiers are equal
  FALSE = if the two PLMN identifiers are not equal

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_sys_id_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
)
{

  boolean   is_equal = FALSE;
  unsigned int       i = 0;

  if ( sys_id_1.id_type == sys_id_2.id_type )
  {
     switch (sys_id_1.id_type)
     {
       case SYS_SYS_ID_TYPE_UMTS      :
         is_equal = sys_plmn_match( sys_id_1.id.plmn, sys_id_2.id.plmn );
         break;

       case SYS_SYS_ID_TYPE_IS95      :
         is_equal = (boolean)( ( sys_id_1.id.is95.nid ==
                                 sys_id_2.id.is95.nid    ) &&
                               ( sys_id_1.id.is95.sid ==
                                 sys_id_2.id.is95.sid    ) &&
                               ( sys_id_1.id.is95.mcc ==
                                 sys_id_2.id.is95.mcc    ) &&
                               ( sys_id_1.id.is95.imsi_11_12 ==
                                 sys_id_2.id.is95.imsi_11_12 ) );
         break;

       case SYS_SYS_ID_TYPE_IS856     :
         is_equal = TRUE;
         for (i = 0; i < ARR_SIZE( sys_id_1.id.is856 ); i++)
         {
           if (sys_id_1.id.is856[i] != sys_id_2.id.is856[i])
           {
             is_equal = FALSE;
             break;
           }
         }
         break;


       case SYS_SYS_ID_TYPE_UNDEFINED :
         is_equal = TRUE;
         break;

       default                        :
         break;
     } /* switch id_type */
  } /* if id_type */

  return is_equal;

} /* cm_util_sys_id_match */

/*===========================================================================

FUNCTION sys_lac_match

DESCRIPTION
  This function compares two LAC system identifiers of a UMTS SYS ID.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two LAC are equal
  FALSE = if the two LAC are not equal

  Note: TRUE is returned if both system identifiers are not UMTS type.

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_lac_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
)
{
  if( sys_id_1.id_type == sys_id_1.id_type )
  {
    if( sys_id_1.id_type == SYS_SYS_ID_TYPE_UMTS)
    {
      if( sys_id_1.id.plmn_lac.lac != sys_id_2.id.plmn_lac.lac)
      {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*==============================================================================

                            N E T W O R K   L I S T S

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_signal_quality_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_signal_quality_e_type value                Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_signal_quality_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_signal_quality_is_valid
(
  sys_signal_quality_e_type value
)
{
  boolean valid = FALSE;

  valid = (boolean)(value > SYS_SIGNAL_QUALITY_NONE &&
                    value < SYS_SIGNAL_QUALITY_MAX    );

  return valid;
} /* sys_signal_quality_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_category_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_detailed_plmn_list_category_e_type value   Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_category_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_detailed_plmn_list_category_is_valid
(
  sys_detailed_plmn_list_category_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_DETAILED_PLMN_LIST_CATEGORY_NONE &&
                    value < SYS_DETAILED_PLMN_LIST_CATEGORY_MAX    );

  return valid;
} /* sys_detailed_plmn_list_category_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_found_plmn_rat

DESCRIPTION

  Function that verifies that the given PLMN and RAT is in the PLMN list.

PARAMETERS

  sys_detailed_plmn_list_s_type  plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched
  sys_radio_access_tech_e_type   rat              The RAT being searched

RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
boolean sys_detailed_plmn_list_found_plmn_rat
(
  const sys_detailed_plmn_list_s_type*      plmn_list_ptr,
  sys_plmn_id_s_type                        plmn,
  sys_radio_access_tech_e_type              rat
)
{
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_list_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i = 0; i < plmn_list_ptr->length; i++)
  {
    if ( (sys_plmn_match(plmn_list_ptr->info[i].plmn, plmn) ) &&
         (plmn_list_ptr->info[i].rat == rat) )
    {
      return TRUE;
    }
  }

  return FALSE;

} /* sys_detailed_plmn_list_found_plmn_rat */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_list_found_plmn_rat

DESCRIPTION

  Function that verifies that the given PLMN is in the PLMN list.

PARAMETERS

  sys_plmn_list_s_type           plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched

RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
boolean sys_plmn_list_found_plmn
(
  const sys_plmn_list_s_type*               plmn_list_ptr,
  sys_plmn_id_s_type                        plmn
)
{
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_list_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i = 0; i < plmn_list_ptr->length; i++)
  {
    if (sys_plmn_match(plmn_list_ptr->plmn[i], plmn))
    {
      return TRUE;
    }
  }

  return FALSE;

} /* sys_plmn_list_found_plmn */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_info_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_detailed_plmn_list_info_s_type value       Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_PLMN_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_detailed_plmn_list_info_is_valid
(
  sys_detailed_plmn_list_info_s_type value
)
{
  boolean valid;

  valid = (boolean)( sys_plmn_id_is_valid                     ( value.plmn ) &&
                     sys_radio_access_tech_is_valid           ( value.rat ) &&
                     sys_srv_capability_is_valid              ( value.plmn_service_capability ) &&
                     sys_signal_quality_is_valid              ( value.signal_quality ) &&
                     sys_detailed_plmn_list_category_is_valid ( value.list_category ) );

  return valid;
}



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_detailed_plmn_list_s_type value            Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_detailed_plmn_list_is_valid
(
  const sys_detailed_plmn_list_s_type* value_ptr
)
{
 
  uint32   i;
  boolean  valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->length <= SYS_PLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_detailed_plmn_list_info_is_valid (value_ptr->info[i]))
      {
        valid = FALSE;
        break;
      }
    } /* for */
  }
  else
  {
    valid = FALSE;
  }

  return valid;

} /* sys_detailed_plmn_list_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_PLMN_list_s_type value                     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_plmn_list_is_valid
(
  const sys_plmn_list_s_type* value_ptr
)
{
  uint32   i;
  boolean  valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->length <= SYS_PLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_plmn_id_is_valid (value_ptr->plmn[i]))
      {
        valid = FALSE;
        break;
      }
    } /* for */
  }
  else
  {
    valid = FALSE;
  }

  return valid;

} /* sys_plmn_list_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_user_pref_plmn_list_info_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_user_pref_plmn_list_info_s_type  value     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_user_pref_plmn_list_info_is_valid
(
  sys_user_pref_plmn_list_info_s_type  value
)
{
  boolean valid;

  valid = (boolean)( sys_plmn_id_is_valid( value.plmn ) );

  return valid;
} /* sys_user_pref_plmn_list_info_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_user_pref_plmn_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_user_pref_plmn_list_s_type  value          Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_user_pref_plmn_list_is_valid
(
  const sys_user_pref_plmn_list_s_type*  value_ptr
)
{
  uint32   i;
  boolean  valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->length <= SYS_USER_PPLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_user_pref_plmn_list_info_is_valid (value_ptr->info[i]))
      {
        valid = FALSE;
        break;
      }
    } /* for */
  }
  else
  {
    valid = FALSE;
  }

  return valid;

} /* sys_user_pref_plmn_list_is_valid */


/*------------------------------------------------------------------------------
   Mobility Management Information Functions
------------------------------------------------------------------------------*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_name_coding_scheme_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

   sys_network_name_coding_scheme_type_e_type value
                             Value that is to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_name_coding_scheme_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_name_coding_scheme_is_valid
(
  sys_network_name_coding_scheme_type_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_NW_NAME_CODING_SCHEME_NONE &&
                    value < SYS_NW_NAME_CODING_SCHEME_MAX);

  return valid;
} /* sys_network_name_coding_scheme_is_valid */


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_short_name_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_network_short_name_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_short_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_short_name_is_valid
(
  const sys_network_short_name_s_type* value_ptr
)
{

  boolean valid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e685 */
  valid = (boolean)((value_ptr->length <= SYS_MAX_SHORT_NAME_CHARS) &&
                     sys_network_name_coding_scheme_is_valid(value_ptr->coding_scheme)&&
                    (value_ptr->spare_bits <= SYS_MAX_NW_NAME_SPARE_BITS));
  /*lint -restore */

  return valid;

} /* sys_network_short_name_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_full_name_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_network_full_name_s_type* value_ptr  Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_full_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_full_name_is_valid
(
  const sys_network_full_name_s_type* value_ptr
)
{
  boolean valid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e685 */
  valid = (boolean)((value_ptr->length <= SYS_MAX_FULL_NAME_CHARS) &&
                     sys_network_name_coding_scheme_is_valid(value_ptr->coding_scheme)&&
                    (value_ptr->spare_bits <= SYS_MAX_NW_NAME_SPARE_BITS));
  /*lint -restore */ 

  return valid;

}/* sys_network_full_name_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_time_zone_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_time_zone_type value                       Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_time_zone_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_time_zone_is_valid
(
  sys_time_zone_type value
)
{
  boolean valid;

  valid = (boolean)(value >= -96 && value <= 96);

  return valid;
} /* sys_time_zone_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_time_and_time_zone_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_time_and_time_zone_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_time_and_time_zone_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_time_and_time_zone_is_valid
(
  const sys_time_and_time_zone_s_type* value_ptr
)
{

  boolean valid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  valid = (boolean)( ( value_ptr->year   <= 99) &&
                     ( value_ptr->month  >= 1 && value_ptr->month <= 12) &&
                     ( value_ptr->day    >= 1 && value_ptr->day   <= 31) &&
                     ( value_ptr->hour   <= 23) &&
                     ( value_ptr->minute <= 59) &&
                     ( value_ptr->second <= 59) &&
                     sys_time_zone_is_valid ( value_ptr->time_zone ) );
  return valid;

} /* sys_time_and_time_zone_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_lsa_id_type_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_LSA_identity_type_e_type value             Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_lsa_id_type_is_valid
(
  sys_lsa_identity_type_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_LSA_ID_TYPE_NONE &&
                    value < SYS_LSA_ID_TYPE_MAX);

  return valid;
} /* sys_lsa_id_type_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_lsa_id_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_LSA_identity_s_type value                  Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_lsa_id_is_valid
(
  sys_lsa_identity_s_type value
)
{
  boolean valid;

  valid = sys_lsa_id_type_is_valid ( value.type );

  return valid;
} /* sys_lsa_id_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_day_sav_adj_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_daylight_saving_adj_e_type value           Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_daylight_saving_adj_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_day_sav_adj_is_valid
(
  sys_daylight_saving_adj_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_DAY_SAV_ADJ_NONE &&
                    value < SYS_DAY_SAV_ADJ_MAX);

  return valid;
} /* sys_day_sav_adj_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mm_information_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_mm_information_s_type* value_ptr     Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_mm_information_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_mm_information_is_valid
(
  const sys_mm_information_s_type* value_ptr
)
{

  boolean valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->plmn_avail)
  {
    valid = (boolean)( valid && sys_plmn_id_is_valid( value_ptr->plmn ) );
  }

  if (value_ptr->full_name_avail)
  {
    valid = (boolean)( valid && sys_network_full_name_is_valid( &(value_ptr->full_name) ) );
  }

  if (value_ptr->short_name_avail)
  {
    valid = (boolean)( valid && sys_network_short_name_is_valid ( &(value_ptr->short_name) ) );
  }

  if (value_ptr->univ_time_and_time_zone_avail)
  {
    valid = (boolean)( valid && sys_time_and_time_zone_is_valid ( &(value_ptr->univ_time_and_time_zone) ) );
  }

  if (value_ptr->time_zone_avail)
  {
    valid = (boolean)( valid && sys_time_zone_is_valid          ( value_ptr->time_zone ) );
  }

  if (value_ptr->lsa_identity_avail)
  {
    valid = (boolean)( valid && sys_lsa_id_is_valid             ( value_ptr->lsa_identity ) );
  }

  if (value_ptr->daylight_saving_adj_avail)
  {
    valid = (boolean)( valid && sys_day_sav_adj_is_valid        ( value_ptr->daylight_saving_adj ) );
  }

  return valid;

} /* sys_mm_information_is_valid */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_undefine_ssid

DESCRIPTION

  Function that sets the length of SSID to 0.
PARAMETERS

  sys_ssid_s_type*  ssid_ptr           Pointer to the SSID to undefine.

RETURN VALUE

  void

==============================================================================*/
void sys_undefine_wlan_ssid
(
  sys_wlan_ssid_s_type*  ssid_ptr
)
{
  if(ssid_ptr != NULL)
  {
    ssid_ptr->len = 0;
  }

} /* sys_undefine_ssid */

/*===========================================================================

FUNCTION sys_is_wlan_ssid_match

DESCRIPTION
  This function compares two WLAN SSIDs.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two system identifiers are equal
  FALSE = if the two system identifiers are not equal

  Note: TRUE is returned if both system identifiers are undefined.

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_is_wlan_ssid_match(

  const sys_wlan_ssid_s_type     *ssid_1,
  const sys_wlan_ssid_s_type     *ssid_2
)
{

  unsigned int       i = 0;

  if( (ssid_1 == NULL) || (ssid_2 == NULL))
  {
    return FALSE;
  }

  if(ssid_1->len != ssid_2->len)
  {
    return FALSE;
  }


  for (i = 0; i < ARR_SIZE( ssid_1->ssid ) && i < ssid_1->len; i++)
  {
    if (ssid_1->ssid[i] != ssid_2->ssid[i])
    {
       return FALSE;
    }
  }

  return TRUE;

} /* sys_is_wlan_ssid_match */

/*===========================================================================

FUNCTION sys_is_wlan_ssid_undefined

DESCRIPTION
  This function checks if WLAN ssid is undefined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_is_wlan_ssid_undefined(

  const sys_wlan_ssid_s_type     *sys_id_ptr
)
{
  if(sys_id_ptr == NULL)
  {
    return TRUE;
  }

  if(sys_id_ptr->len == 0)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION sys_is_wlan_bssid_undefined

DESCRIPTION
  This function checks if WLAN bssid is undefined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_is_wlan_bssid_undefined(

  const sys_wlan_bssid_type    *bss_id_ptr
)
{
  if(bss_id_ptr == NULL)
  {
    return TRUE;
  }

  if(*bss_id_ptr == 0)
  {
    return TRUE;
  }

  return FALSE;

}


/*===========================================================================

FUNCTION sys_undefine_wlan_bssid

DESCRIPTION
  This function undefines or initializes a WLAN BSSID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sys_undefine_wlan_bssid(

  sys_wlan_bssid_type     *bss_id_ptr
)
{
  if(bss_id_ptr == NULL)
  {
    return;
  }

  *bss_id_ptr = 0;

}

/*===========================================================================

FUNCTION sys_is_wlan_bssid_match

DESCRIPTION
  This function compares two WLAN BSSIDs.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two system identifiers are equal
  FALSE = if the two system identifiers are not equal


SIDE EFFECTS
  None

===========================================================================*/
boolean sys_is_wlan_bssid_match(

  const sys_wlan_bssid_type     *bssid_1_ptr,
  const sys_wlan_bssid_type     *bssid_2_ptr
)
{
  if(bssid_1_ptr == NULL ||
     bssid_2_ptr == NULL)
  {
    return FALSE;
  }

  if(*bssid_1_ptr == *bssid_2_ptr)
  {
    return TRUE;
  }

  return FALSE;
}




