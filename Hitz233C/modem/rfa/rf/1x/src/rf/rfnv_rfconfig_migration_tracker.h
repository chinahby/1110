#ifndef RFNV_RFCONFIG_MIGRATION_TRACKER_H
#define RFNV_RFCONFIG_MIGRATION_TRACKER_H

/*===========================================================================
                       H E A D E R   F I L E   F O R 
    R F N V   - - >   R F C O N F I G   M I G R A T I O N   T R A C K E R

DESCRIPTION
  This file lists the data type and interface available for manipulating
  tracker objects that indicate/control what particular RFNV items have been
  read from QCN file and those that must be considered for migration to
  RFConfig database.

  CAUTION:
  Do NOT access the data structure of the tracker object directly; rather
  use only the function interfaces provided to ensure the integrity of the data 
  contained within.

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_rfconfig_migration_tracker.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/07   Vish    Created initial version.
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "rficap.h"   // for RF_HAS_RFCONFIG_RX_LNA

#ifdef RF_HAS_RFCONFIG_RX_LNA

/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */
#define RFNV_RFCONFIG_MIGRATION_TRACKER_RESET_VAL_INITIALIZER (0)


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
typedef uint32 rfnv_rfconfig_migration_tracker_type;

typedef enum
{
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_RISE,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_FALL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_IM_LEVEL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_RISE,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_FALL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_IM_LEVEL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_RISE,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_FALL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_IM_LEVEL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_RISE,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_FALL,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_IM_LEVEL,

   RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,
   RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,

   RFNV_RFCONFIG_MIGRATION_ITEM__HDR_IM_RISE,
   RFNV_RFCONFIG_MIGRATION_ITEM__HDR_IM_FALL,
   RFNV_RFCONFIG_MIGRATION_ITEM__HDR_IM_LEVEL,
   RFNV_RFCONFIG_MIGRATION_ITEM__HDR_G0_IM_RISE,
   RFNV_RFCONFIG_MIGRATION_ITEM__HDR_G0_IM_FALL,
   RFNV_RFCONFIG_MIGRATION_ITEM__HDR_G0_IM_LEVEL,

   // IMPORTANT! No entries must be added beyond this 
   RFNV_RFCONFIG_MIGRATION_ITEM__NONE,   // is a special item that cannot be enabled
   RFNV_RFCONFIG_MIGRATION_ITEM__COUNT
} rfnv_rfconfig_migration_item_type;


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */
// mark all items as NOT requiring migration from NV to RFConfigDBase mgmt.
extern void rfnv_rfconfig_migration_tracker_reset(
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr);

// mark a particular item as requiring migration from NV to RFConfigDBase mgmt.
extern void rfnv_rfconfig_migration_tracker_enable(
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr,
   rfnv_rfconfig_migration_item_type      tracked_item);

// mark a particular item as NOT requiring migration from NV to RFConfigDBase mgmt.
extern void rfnv_rfconfig_migration_tracker_disable(
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr,
   rfnv_rfconfig_migration_item_type      tracked_item);

// find if a particular item requires migration from NV to RFConfigDBase mgmt.
extern boolean rfnv_rfconfig_migration_tracker_is_enabled(
   const rfnv_rfconfig_migration_tracker_type  *tracker_ptr,
   rfnv_rfconfig_migration_item_type            tracked_item);

#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFNV_RFCONFIG_MIGRATION_TRACKER_H */


