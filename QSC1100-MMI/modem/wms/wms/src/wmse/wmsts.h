#ifndef WMSTS_H
#define WMSTS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    W I R E L E S S   M E S S A G I N G   S E R V I C E S
                   -- Translation Services

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the internal translation functions.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsts.h_v   1.2   26 Mar 2002 12:10:10   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsts.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information to include 2008
07/20/07   PMD     Moved TS functions to wms.h
06/27/07   PMD     Moved certain functions from wmstsi.h
06/20/07   PMD     Cleaned up file for internal functions - moved to wmstspriv
05/15/07   PMD     Added wms_ts_init() function
01/17/07   HQ      Updated copyright info to include 2007.
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
01/25/06   VK      Updated Copyright Info
10/31/05   VK      Feature define fix
10/08/05   VK      Updated Copyright info. 
08/05/05   Rex     Code cleanup
01/07/05   PMD     Added wms_ts_compute_user_data_header_length() function prototype.
                   Added wms_ts_compute_gw_user_data_length() function prototype.
07/29/04   PMD     Added Function Comments
07/15/04   PMD     Added wms_ts_decode_gw_cb_dcs() in header file.
02/03/04   HQ      Fixed compile errors when UTK is defined on 1x targets.
1/29/04    Rex     Added wms_ts_bcd_to_int and wms_ts_int_to_bcd for checking
                   in wms_ts_decode/encode_timestamp and wms_ts_decode/encode_relative_time.
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
11/04/03   AP      Deleted conversion prototypes.
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
02/28/03   HQ      Added support for CAT.
02/07/03   SUN     Added Support for Status Reports and Commands
07/31/02   HQ      Added wms_ts_decode_user_data_header(),
                   wms_ts_encode_user_data_header(), etc.
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/

#include "wms.h"
#include "wmspriv.h"

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
  wms_ts_compute_user_data_header_length

DESCRIPTION
  Computes the User Data Header Length

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length in bytes

SIDE EFFECTS
  None
=========================================================================*/
uint32 wms_ts_compute_user_data_header_length
(
  const uint8           num_headers,
  const wms_udh_s_type *headers
);

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

void wms_ts_init
(
  void
);

#endif /* WMSTS_H */


