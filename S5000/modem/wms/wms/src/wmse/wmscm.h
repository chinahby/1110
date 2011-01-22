#ifndef WMSCM_H
#define WMSCM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Interface with CM

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the WMS interfaces used by Call Manager.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmscm.h_v   1.7   17 Jun 2002 15:47:36   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscm.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information to include 2008
05/21/07   PMD     Added wms_cm_sms_send_cdma_msg() prototype under feature
03/06/07   HQ      Put wms_cm_sms_send_cdma_msg2() under FEATURE_CDSMS.
03/01/07   HQ      Cleaned up SS info for GW.
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
01/25/06   VK      Updated Copyright Info
10/08/05   VK      Updated copyright info
01/10/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
11/04/03   AP      Put wms_cm_get_current_acq_mode under feature flag
                   FEATURE_SMS_TRANSPARENT_MO.
10/29/03   AP      Added utility for checking the current acquisition mode.
10/09/02   AP      Added cm_num_s_type to wms_cm_s_type to originate SMS calls
                   using MULTIMODE Architecture.
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
06/15/02   HQ      Feature-ized sys.h.
06/03/02   SUN     New ss_info struct to store event changes.
05/16/02   HQ      Include sys.h.
05/07/02   HQ      Added more support under FEATURE_CDSMS.
03/12/02   HQ      Added support of Multimode WMS
                   - added FEATURE_CDSMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmsi.h"
#include "cm.h"

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


/*===========================================================================
============================ DATA TYPES =====================================
===========================================================================*/

typedef struct
{
  cm_client_id_type             cm_client;

#ifdef FEATURE_CDSMS
#ifdef FEATURE_MULTIMODE_ARCH
  cm_mm_call_info_s_type           call_info;
  cm_num_s_type                    called_number;
#else /* FEATURE_MULTIMODE_ARCH */
  cm_call_info_s_type           call_info;
#endif /* FEATURE_MULTIMODE_ARCH */
#endif /* FEATURE_CDSMS */

} wms_cm_s_type;



/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_cm_s_ptr

DESCRIPTION
  Return the pointer to the CM data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal CM data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cm_s_type* wms_cm_s_ptr
(
  void
);


void wms_cm_init
(
  void
);

void wms_cm_process_event
(
  wms_cmd_type    *cmd_ptr
);

#ifdef FEATURE_CDSMS

void wms_cm_call_orig
(
  wms_dc_so_e_type   so
);

void wms_cm_call_end
(
  void
);

void wms_cm_call_answer
(
  void
);


void wms_cm_awi_ack
(
  uint8                   seq_no,
  boolean                 bf_bit,
  wms_error_class_e_type  error_class,
  wms_cdma_tl_status_e_type    tl_status
);

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_CDSMS_BROADCAST
void wms_cm_bc_enable
(
  boolean  bc_enable
);
#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_CDSMS
#ifdef FEATURE_WMS_NO_CM_MSG2
/*===========================================================================
FUNCTION wms_cm_sms_send_cdma_msg

DESCRIPTION
  CM Send MO SMS Message Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_sms_send_cdma_msg
(
  uint8     * cdma_mc_msg
);

#else /* FEATURE_WMS_NO_CM_MSG2 */

/*===========================================================================
FUNCTION wms_cm_sms_send_cdma_msg2

DESCRIPTION
  CM Send MO SMS Message Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_sms_send_cdma_msg2
(
  const wms_address_s_type    * address_ptr,
  const wms_OTA_message_type  * OTA_ptr,
  wms_bearer_e_type           bearer
);

#endif /* FEATURE_WMS_NO_CM_MSG2 */

#endif /* FEATURE_CDSMS */


#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSCM_H */

