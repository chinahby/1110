/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   P D   M A N A G E R

GENERAL DESCRIPTION
  This module contains functions used by the call manager in processing
  position determination messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmxpd.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/09   aj      Added support for hold cmd reason for hold cmd Q
12/15/08   ak      Allow PD session grant in LPM mode.
11/05/08   aj      check to grant PD session if cmregprx is not ACTIVE
10/01/08   sv      Corrected featurization to remove lint errors.
06/12/08   sv      Resolve Klocwork defects
04/16/08   vk      updated code to use pending flags
08/07/07   ic      Lint clean up
02/27/07   pk      Removing uapdms.h
02/12/07   pk      Honouring phone's band preference for HDR systems
02/09/07   pk      Added checks for CDMA subscriptions for GSM builds
01/23/07   pk      Controlling cmph_mo_sms_end under CDMA features 
07/07/06   pk      If a PD session is ended while changing the operating mode
                   to ONLINE, the operating mode change command is put on 
                   hold for atmost a second, or PD handshake is received
05/08/06   pk      Modified session grant checks for DMOD sessions and HDR
                   call originations
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
12/02/05   pk      If AT is in a CDMA call, grant PD session without checking
                   the last mode forced to SD
11/21/05   ic      Fixed "enumerated type mixed with another type" compiler 
                   warning in cmpd_call_start() 
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/25/05   pk      Fixed RVCT compiler warnings
10/18/05   pk      Modified the PD session grant proc as per the final design
09/02/05   pk      Added support for GPS session, added support for
                   CM_PD_SESSION_TYPE_WITH_GPS_NORMAL
                   CM_PD_SESSION_TYPE_WITH_GPS_DMOD
08/15/05   pk      Added support for priority Queues for origination 
                   parameters
07/15/05   ic      Lint fixes for CDMA + GW targets 
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/04/05   sj      Corrected base lat/long info in BS status response.
11/17/04   sj      Added base station info in the PD event.
08/20/04   sj      Added support for PRL Preference.
08/09/04   ic      Lint clean up for SNAR configuration
08/04/04   ic      CM Lint clean up - cmxpd.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmxpd_dummy() to get rid of ADS 1.2 compiler warning
07/15/04   sj      Changed pref_reason to ORIG_START_CS.
07/14/04   ws      Fixed compiler warnings with the addition of modifying info_structs
                   in event generations.
05/27/04   ic      In cmdbm_client_cmd_proc() call cmdbm_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
04/23/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.28 and MSMSHARED_CM.01.00.10.00.31
04/12/04   sj      Added support for setting DDTM on while doing GPS fix.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
11/03/03   ic      Merged in CR fixes over from other targets
10/23/03   ic      Corrected typos, replaced UNAVIALABLE with UNAVAILABLE
04/28/03   AT      Lint cleanup.
04/14/03   ws      Added BSID changes
02/28/03   vt      Added code to block/unblock HDR data call origination.
01/21/03   ws      Updated copyright information for 2003
11/15/02   vt      Fixed a compile error when comparing for valid client.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged in position determination changes
08/30/02   vt      Fixed a typo in an EMERGENCY enum
04/09/02   sh      Added one new argument in cmph_mo_sms_end
04/04/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/06/02   sh      Added hybrid 3.0 support
05/07/02   prk     Renamed to cmxpd.c.
01/23/02   ic      Fixes related to running with FEATURE_GPSONE_* enabled
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
12/05/01    ic     Fixed "Warning: C2207W: inventing 'extern int srch_rude_wakeup();'"
11/30/01    hxw    Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
10/23/01   VT/IC   Baseline for CCS
08/21/01    ic     Main-lined FEATURE_NEWCM
08/01/01    PJ     Made SD2.0 changes for handling sending of DBM msgs.
05/31/01    RC     Corrected compile time error when CM_DEBUG is defined.
01/29/01    HQ     Properly feature-ized NEWCM and FEATURE_GPSONE_DBM.
08/21/00    PJ     Made code review changes.
08/07/00    YLL    Declared the cmpd_mo_msg as static so that it can be
                   referred to get the numBytes and payload information.
06/22/00    PJ     Created.

===========================================================================*/

/*lint -esym(766,customer.h)*/
#include "customer.h"

#ifdef FEATURE_GPSONE
#error code not present
#else /* FEATURE_GPSONE */

/*===========================================================================
FUNCTION cmxpd_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmxpd_dummy( void )
/*lint -esym(714,cmxpd_dummy) */
/*lint -esym(765,cmxpd_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif /* FEATURE_GPSONE */
