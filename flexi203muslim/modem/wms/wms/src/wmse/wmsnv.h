#ifndef WMSNV_H
#define WMSNV_H


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 W I R E L E S S   M E S S A G I N G   S E R V I C E S
                 -- NV Operations

GENERAL DESCRIPTION
  This module provides the utility functions for the use by WMS for accessing
  NV items needed for SMS processing.

EXTERNALIZED FUNCTIONS
  wms_nv_read_wait
    Read an item from NV, handle non-active items

  wms_nv_write_wait
    Write an item to NV

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsnv.h_v   1.5   10 Jul 2002 20:18:20   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsnv.h#3 $ $DateTime: 2010/02/23 11:51:47 $ $Author: rajang $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/08   PMD     Added support for BC-SMS and SMSCAP EFs in RUIM.
01/31/08   PMD     Updated Copyright Information to include 2008
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP.
01/17/07   HQ      Updated copyright info to include 2007.
06/26/06   PMD     Removed wms_nv_delete_cdma_params()
01/25/06   VK      Updated Copyright Info
11/18/05   VK      Cleaned up Lint warnings.
10/20/05   VK      Updated Copyright info
08/29/05   PMD     Added Read GCF Flag Functionality.
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
07/10/02   HQ      Store RUIM format to NV for CDMA SMS parameters.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
02/13/02   SUN     Added nv replace and delete commands
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
10/30/01   HQ      Initial version for FEATURE_GWSMS.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>

#include "wmsi.h"
#include "err.h"
#include "nv.h"



/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define      MAX_SMS_FILE_NAME_SIZE      20

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_WRITE_WAIT

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_write_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr    /* pointer to data for item */
);


/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_READ_WAIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_read_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr          /* pointer to space for item */
);

#ifdef FEATURE_CDSMS_RUIM
/*===========================================================================
FUNCTION WMS_NV_INIT_NVRUIM

DESCRIPTION
  Get certain NV items from the EFS or RUIM depending on NVRUIM's settings.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_init_nvruim
(
  void
);
#endif /* FEATURE_CDSMS_RUIM */

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_INIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
void wms_nv_init
(
  void
);

#ifdef FEATURE_CDSMS
boolean wms_nv_read_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
);

boolean wms_nv_write_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
);

boolean wms_nv_replace_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
);

boolean wms_nv_delete_cdma_sms
(
  wms_message_index_type   index,
  const uint8                    * data
);

boolean wms_nv_read_cdma_params
(
  wms_message_index_type    index,
  uint8                     * data
);

boolean wms_nv_write_cdma_params
(
  wms_message_index_type    index,
  const uint8                     * data
);

void wms_nv_read_cdma_channel_setting(void);

void wms_nv_read_cdma_service_option(void);

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */



boolean wms_nv_write_routing
(
  const wms_routes_s_type  *ptr
);

boolean wms_nv_read_routing
(
  wms_routes_s_type  *ptr
);

/*===========================================================================
FUNCTION wms_nv_read_bearer_prefs

DESCRIPTION
  Read the Bearer Preferences

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
);

/*===========================================================================
FUNCTION wms_nv_write_bearer_prefs

DESCRIPTION
  Write the Bearer Preferences to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Params
===========================================================================*/
boolean wms_nv_write_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
);

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSNV_H */

