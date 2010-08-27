#ifndef WMSCL_H
#define WMSCL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Client Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the client group data types and functions.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmscl.h_v   1.2   26 Mar 2002 12:10:02   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscl.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
01/31/08   PMD     Updated Copyright Information to include 2008
03/30/07   HQ      Added support for Power Collapse/Power Management
                   for Dual CPU chipsets.
01/17/07   HQ      Updated copyright info to include 2007.
01/25/06   VK      Updated Copyright Info
10/18/05   VK      Updated copyright info
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
02/14/03   AP      Added support for client parsing.
11/13/02   AP      Deleted support for message info cache.
10/09/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE.
03/09/02   HQ      Added DC, BC and DBG groups for Multimode WMS
                   - added FEATURE_CDSMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)



/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"



/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/
typedef struct
{
  boolean                   initialized;
  boolean                   activated;

  wms_cl_parse_msg_cb_type  parse_msg_cb;

  wms_cfg_event_cb_type     cfg_event_cb;
  wms_msg_event_cb_type     msg_event_cb;
  wms_dc_event_cb_type      dc_event_cb;
#ifdef CUST_EDITION
  wms_bc_event_cb_type      bc_event_cb;
#endif
  wms_bc_mm_event_cb_type   bc_mm_event_cb;
  wms_dbg_event_cb_type     dbg_event_cb;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */
} wms_client_s_type;



/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION wms_client_ptr

DESCRIPTION
  Return the pointer to client data of a client.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to client data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_client_s_type* wms_client_ptr
(
  wms_client_id_type client_id
);


/*=========================================================================
FUNCTION
  wms_cl_init

DESCRIPTION
  Initialize all client data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Internal data initialized.

=========================================================================*/
void wms_cl_init
(
  void
);

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSCL_H */

