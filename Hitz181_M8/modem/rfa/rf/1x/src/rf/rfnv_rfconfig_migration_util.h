#ifndef RFNV_RFCONFIG_MIGRATION_UTIL_H
#define RFNV_RFCONFIG_MIGRATION_UTIL_H

/*===========================================================================
                      H E A D E R   F I L E   F O R
            U T I L I T Y   F U N C T I O N S   T H A T   A I D
           R F N V   - - >   R F C O N F I G   M I G R A T I O N

DESCRIPTION
  This file lists data type and utility functions available for migrating
  certain RFNV items (as listed in RFNV --> RFConfig tracker
  header file "rfnv_rfconfig_migration_tracker.h") to RFConfig database.

  Note that this module deals with migration of only those RFNV items that
  are applicable across all Rx chips (i.e. any Rx chip specific NV items will
  NOT be handled here but in chip specific RFConfig module [e.g. rfconfig_rfr6500.*]
  or in RF card file [e.g. rfc_dzrf6500.*]).

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_rfconfig_migration_util.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/07   Vish    Made rfnv_rfconfig_migration_util_get_1x_[rise/fall/im_level]_for_lna()
                   APIs as static to this module.
06/20/07   Vish    Created initial version.
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "rficap.h"     // for RF_HAS_RFCONFIG_RX_LNA
#include "rfcom.h"
#include "rfnv_cdma1x.h"
#include "rfconfig_rx_lna.h"
#include "nv_items.h"
#include "rflib_share.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
typedef struct
{
   /* NV item identifier */
   nv_items_enum_type                     nv_item_id;

   /* RFNV->RFConfig migration tracker item identifier */
   rfnv_rfconfig_migration_item_type      migration_item_id;

   /* Pointer to the tracker to be manipulated */
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr;
} rfnv_rfconfig_migration_util_map_type;

typedef enum
{
   // IMPORTANT: the first gain step G0<-> G1 must be 0 for compatibility with RFConfigDBase;
   //    don't change this
   RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1 = 0,
   RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2,
   RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3,
   RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4,
   
   // IMPORTANT! No entries must be added beyond this 
   RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__MAX_NUM,
   RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__INVALID = RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__MAX_NUM
} rfnv_rfconfig_migration_util_lna_gain_step_type;


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */
// function for enabling tracking using an established RFNV->RFConfig conversion guide map
extern void rfnv_rfconfig_migration_util_enable_tracking_for_nv(
   nv_items_enum_type                       nv_item_id,
   rfnv_rfconfig_migration_util_map_type   *map_array,
   uint32                                   map_array_size);

// Function for converting RFNV items to RFConfigDBase; those conversions that
//  are unique only to certain chipsets will NOT be handled here. Those
//  will be relegated to chipset specific migration functions
extern void rfnv_rfconfig_migration_util_perform_conversion(
   const rfnv_digital_items_type               *rfnv_digital_items_ptr,
   boolean                                      use_1x_rfnv_items_to_init_evdo_sw_pts,
   rfconfig_rx_lna_type                        *prx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *prx_rfconfig_migration_tracker_ptr,
   rfconfig_rx_lna_type                        *drx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *drx_rfconfig_migration_tracker_ptr);

// function for migrating a FIXED RFNV switch point to any type (FIXED/OLTA)
//   of RFConfigDBase switch point
extern void rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed_or_olta(
   boolean                                      force_rfconfig_olta_to_fixed,
   rfconfig_rx_lna_type                        *rfconfig_rx_lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type        rfconfig_rx_lna_switch_pt_mode,
   uint32                                       rfconfig_rx_lna_linearity_index,
   uint32                                       rfconfig_rx_lna_gain_step_index,
   const rfnv_rfconfig_migration_tracker_type  *rfnv_rfconfig_migration_tracker_ptr,
   rfnv_rfconfig_migration_item_type            rfnv_rfconfig_migration_item_rise,
   int1                                         rfnv_rise_val,
   rfnv_rfconfig_migration_item_type            rfnv_rfconfig_migration_item_fall,
   int1                                         rfnv_fall_val,
   rfnv_rfconfig_migration_item_type            rfnv_rfconfig_migration_item_im_level,
   int1                                         rfnv_im_level_val);

// function for migrating a fixed RFNV switch point ONLY IF the corresponding
//   RFConfigDBase switch point is already of FIXED type
extern void rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed(
   rfconfig_rx_lna_type                        *rfconfig_rx_lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type        rfconfig_rx_lna_switch_pt_mode,
   uint32                                       rfconfig_rx_lna_linearity_index,
   uint32                                       rfconfig_rx_lna_gain_step_index,
   const rfnv_rfconfig_migration_tracker_type  *rfnv_rfconfig_migration_tracker_ptr,
   rfnv_rfconfig_migration_item_type            rfnv_rfconfig_migration_item_rise,
   int1                                         rfnv_rise_val,
   rfnv_rfconfig_migration_item_type            rfnv_rfconfig_migration_item_fall,
   int1                                         rfnv_fall_val,
   rfnv_rfconfig_migration_item_type            rfnv_rfconfig_migration_item_im_level,
   int1                                         rfnv_im_level_val);

#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFNV_RFCONFIG_MIGRATION_UTIL_H */


