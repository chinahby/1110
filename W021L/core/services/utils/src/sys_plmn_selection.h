#ifndef _SYS_PLMN_SELECTION_H_
#define _SYS_PLMN_SELECTION_H_
/*==============================================================================

       S Y S T E M   P L M N   S E L E C T I O N   H E A D E R   F I L E

DESCRIPTION

  This header file contains the types associated with the process of selecting
  a Public Land Mobile Network (PLMN) for service.


         COPYRIGHT INFORMATION

Copyright (c) 2002 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
         EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_plmn_selection.h_v   1.0   10 May 2002 15:26:34   kabraham  $
$Header: //depot/asic/msmshared/umts/services/utils/sys_plmn_selection.h#3 $ $DateTime: 2003/02/28 11:43:32 $ $Author: kabraham $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/07/02   kwa     Initial version.
10/28/02   kwa     Added SYS_STOP_MODE_REASON_DEEP_SLEEP.
02/28/03   kwa     Added ps_data_suspend to sys_plmn_service_state_s_type.
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h"
#include "sys.h"

/*==============================================================================

                             DATA DECLARATIONS

==============================================================================*/

/*
** Definition of how a PLMN is to be selected.
*/
typedef enum sys_network_selection_mode_e
{
  SYS_NETWORK_SELECTION_MODE_NONE = -1,          /* FOR INTERNAL USE ONLY! */
  SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
  SYS_NETWORK_SELECTION_MODE_MANUAL,
  SYS_NETWORK_SELECTION_MODE_LIMITED,
  SYS_NETWORK_SELECTION_MODE_MAX                 /* FOR INTERNAL USE ONLY! */
} sys_network_selection_mode_e_type;


/*
** The following enumerated type defines the mode preferred
** by the user.
*/
typedef enum sys_mode_pref_e
{
  SYS_MODE_PREF_NONE = -1,                       /* FOR INTERNAL USE ONLY! */
  SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY,
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY,
  SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED,
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED,
  SYS_MODE_PREF_MAX                              /* FOR INTERNAL USE ONLY! */
} sys_mode_pref_e_type;


/*
** The following enumerated type defines the reasons for
** stopping the current mode.
*/
typedef enum sys_stop_mode_reason_e
{
  SYS_STOP_MODE_REASON_NONE=-1,                  /* FOR INTERNAL USE ONLY! */
  SYS_STOP_MODE_REASON_MODE_CHANGE,
  SYS_STOP_MODE_REASON_POWER_OFF,
  SYS_STOP_MODE_REASON_OFFLINE,
  SYS_STOP_MODE_REASON_DEEP_SLEEP,
  SYS_STOP_MODE_REASON_MAX                       /* FOR INTERNAL USE ONLY! */
} sys_stop_mode_reason_e_type;


/*
** The following enumerated type defines the reasons for
** deregistering from a PLMN.
*/
typedef enum sys_nreg_reason_e
{
  SYS_NREG_REASON_NONE=-1,                       /* FOR INTERNAL USE ONLY! */
  SYS_NREG_REASON_SIM_REMOVED,
  SYS_NREG_REASON_POWER_DOWN,
  SYS_NREG_REASON_MAX                            /* FOR INTERNAL USE ONLY! */
} sys_nreg_reason_e_type;


/*==============================================================================

              Types for the service state on the selected network

==============================================================================*/

/*
** Define a structure that contains the collection of information that
** fully defines the service state of the mobile on a PLMN.
**
** If the PLMN_service_capability is
** SYS_SVC_CAPABILITY_NOT_AVAILABLE, then the plmn field
** is undefined.
*/
typedef struct sys_plmn_service_state_s
{
  sys_sim_state_e_type                      sim_state;
  sys_plmn_id_s_type                        plmn;
  sys_srv_capability_e_type                 plmn_service_capability;
  boolean                                   plmn_forbidden;
  sys_roam_status_e_type                    roaming_ind;
  sys_srv_status_e_type                     service_status;
  sys_srv_domain_e_type                     reg_service_domain;
  sys_radio_access_tech_e_type              active_rat;
  boolean                                   ps_data_suspend;
} sys_plmn_service_state_s_type;


/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_selection_mode_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_network_selection_mode_e_type value        Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_selection_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_network_selection_mode_is_valid
(
  sys_network_selection_mode_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mode_pref_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_mode_pref_e_type value                     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_mode_pref_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_mode_pref_is_valid
(
  sys_mode_pref_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_stop_mode_reason_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_stop_mode_reason_e_type value              Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_stop_mode_reason_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_stop_mode_reason_is_valid
(
  sys_stop_mode_reason_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_nreg_reason_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_nreg_reason_e_type value                   Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_nreg_reason_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_nreg_reason_is_valid
(
  sys_nreg_reason_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_service_state_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_plmn_service_state_s_type value            Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_service_state_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_plmn_service_state_is_valid
(
  sys_plmn_service_state_s_type value
);



#endif /* #ifndef _SYS_PLMN_SELECTION_H_ */
