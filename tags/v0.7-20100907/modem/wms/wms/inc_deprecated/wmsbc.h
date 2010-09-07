#ifndef WMSBC_H
#define WMSBC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Broadcast Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the data types for the BC group.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsbc.h_v   1.1   17 Jun 2002 15:46:52   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/inc_deprecated/wmsbc.h#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/08   PMD     Removed Tabs from code
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
01/31/08   PMD     Updated Copyright Information to include 2008
07/11/07   PY      removed wms_bc_s_ptr dependency.
01/17/07   HQ      Updated copyright info to include 2007.
01/25/06   VK      Updated Copyright Info
11/18/05   VK      Cleaned up Lint warnings. 
10/08/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/08/05   VK      Lint Errors & Updated copyright info
07/17/05   PMD     Added wms_bc_gw_cb_pre_init function.
02/01/05   PMD     Added support for wms_bc_mm_set/get_reading_preferences API
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
11/19/02   fc      Renamed FEATURE_IS2000_REL_A_BCSMS to
                   FEATURE_IS2000_REL_A_CC_BCSMS.
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
06/15/02   HQ      Added wms_bc_enable_failure_proc().
03/09/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
===========================================================================*/


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#include "caii.h"
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */


/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_bc_enable_check

DESCRIPTION
  Return the TRUE or FALSE based on whether broadcast is enabled/disabled.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal BC data.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_bc_enable_check
(
  void
);

/*===========================================================================
FUNCTION wms_bc_process_cmd

DESCRIPTION
  Dispatch the BC command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_bc_process_cmd
(
  wms_cmd_type  *cmd_ptr
);



/*===========================================================================
FUNCTION wms_bc_init

DESCRIPTION
  Do BC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_init
(
  void
);

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
FUNCTION wms_bc_mm_process_cmd

DESCRIPTION
  Dispatch the BC MM command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_bc_mm_process_cmd
(
  wms_cmd_type  *cmd_ptr
);
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_CDSMS_BROADCAST
/* <EJECT>^L */
/*=========================================================================
FUNCTION
  wms_bc_addr_match

DESCRIPTION
  This function checks the BC_ADDR of a received broadcast page or
  broadcast message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - the mobile should process the received message
  FALSE - the mobile should discard the received message

SIDE EFFECTS
  None

=========================================================================*/
extern boolean wms_bc_addr_match
(
  const uint8 *bc_addr_ptr
);


void wms_bc_online_proc
(
  void
);

void wms_bc_enable_failure_proc
(
  wms_cmd_type  *cmd_ptr
);

#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*=========================================================================
FUNCTION
  wms_bc_partial_addr_match

DESCRIPTION
  This function checks the partial broadcast address of received Universal
  Page message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_bc_partial_addr_match
(
  caii_univ_page_interleaved_addr_type *addr_ptr
);
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */


#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION   
  wms_bc_scpt_data

DESCRIPTION
  Send an internal command to WMS task to process the Broadcast SCPT data

DEPENDENCIES
  None

RETURNS
  wms_status_e_type

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_bc_scpt_data( const wms_tl_message_type * tl_ptr );
#endif /* FEATURE_CDSMS_BROADCAST */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSBC_H */

