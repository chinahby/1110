#ifndef WMSSIM_H
#define WMSSIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S   M E S S A G I N G   S E R V I C E S
           -- SIM Interface Module

DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the SIM Interface functions.

Copyright (c) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 
2008 by QUALCOMM, Incorporated.  All Rights Reserved.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmssim.h_v   1.2   26 Mar 2002 12:10:08   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmssim.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
10/08/05   VK      Updated Copyright info. 
06/24/04   PMD     Added prototypes for cbmi_init, cbmir_init and cbmid_init.
02/14/04   PMD     Fixed Prototypes.
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
02/10/03   SUN     Added function prototypes for Status Reports
10/12/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
03/12/02   HQ      Added support of Multimode WMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
10/30/01   HQ      Initial version for WCDMA & GSM SMS.

===========================================================================*/

#include "comdef.h"
#include "wmsi.h"

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION WMS_SIM_GSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the WMS task.
===========================================================================*/
void wms_sim_gsdi_card_status_cb (gsdi_sim_events_T   gsdi_event);

/*===========================================================================
FUNCTION WMS_SIM_GSDI_CARD_STATUS_CB_PROC

DESCRIPTION
  Card Status Processing Function in WMS.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_sim_gsdi_card_status_cb_proc (wms_cmd_type   * cmd_ptr);

/*===========================================================================
FUNCTION WMS_SIM_GSDI_INIT

DESCRIPTION
  Initialize GSDI and register a callback to get SIM card status.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the WMS task.
===========================================================================*/
void wms_sim_gsdi_init(void);

#endif /* FEATURE_MMGSDI */
#endif /* WMSSIM_H */

