#ifndef CMXPD_H
#define CMXPD_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   P D   M A N A G E R

GENERAL DESCRIPTION
  This module contains functions used by the call manager in processing
  position determination messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsms_init() must be call to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2000 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmxpd.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/07/06   pk      Added cmpd_process_oprt_mode_change
01/06/06   ic      Lint cleanup  
11/08/05   sk      Separated CM reports and commands.
10/18/05   pk      Added timer support for PD sessions
09/12/05   pk      Added suppport for PD session
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
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
02/28/03   vt      Added code to block/unblock HDR data call origination.
01/21/03   ws      Updated copyright information for 2003
04/02/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/06/2002 SH      Added hybrid 3.0 support
05/07/02   prk     Renamed to cmxpd.h.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01    VT/IC  Baseline for CCS
07/08/2001  PJ     Added cmpd_timer_proc prototype.
06/22/2000  PJ     Created.

===========================================================================*/

#include "cmi.h"
#include "cmcall.h"

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */
#endif /* CMXPD_H */

