
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                                  D S  S H I M

GENERAL DESCRIPTION
  This files contains functions which are referenced by non-DS files, but
  which have changed for the new architecture.  These functions provide a
  shim to the new DS architecture.
  
EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds_shim.c_v   1.4   13 Nov 2002 14:37:54   rchar  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds_shim.c#1 $ $DateTime: 2007/11/06 21:08:25 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
06/14/06   gr/sk      Merged changes to support no SCH
05/28/03   aku        Added support for retrieving the IP address when both  
                      707 and UMTS features are defined.
11/13/02   rc         Wrapped code to retrieve UMTS IP Addr around 
                      FEATURE_GSM_GPRS.
10/03/02   rc         Added code to retrieve UMTS IP Addr.
09/18/02   aku        Removed global Ip_addr.
08/20/02   ak         Initial verion.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_scrm.h"
#endif /* FEATURE_DATA_IS707 */

#include "ps_ppp_defs.h"
#include "netuser.h"
#include "ps_iface.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgr.h"
#endif /* FEATURE_DATA_IS707 */
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */


/*===========================================================================
                         EXTERNAL FUNCTIONS
===========================================================================*/
byte ds_scrm_max_allowable_sch_rate(void)
{
byte rate = 0;

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_IS2000_R_SCH
  rate = ds707_scrm_max_allowable_sch_rate();
#endif
#endif /* FEATURE_DATA_IS707 */

  return rate;
}

uint32 dsiface_get_ip_addr
(
  ppp_dev_enum_type iface   /* interface stack type (PPP Um or PPP Rm) */
)
{
  ps_iface_type *ps_iface_ptr=NULL;
#ifdef FEATURE_DATA_IS707
  ps_iface_type *ps_iface_ptr_1x;
#endif /* FEATURE_DATA_IS707 */
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#ifdef FEATURE_DATA_IS707
  ps_iface_ptr_1x = ds707_pkt_get_iface_ptr(0);
  if(ps_iface_state(ps_iface_ptr_1x) != IFACE_DOWN)
  {
    ps_iface_ptr = ps_iface_ptr_1x;
  }
#endif /* FEATURE_DATA_IS707 */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

  return PS_IFACE_IP_V4_ADDR(ps_iface_ptr);

} /* dsiface_get_ip_addr() */

