/*===========================================================================
                I M P L E M E N T A T I O N   F I L E   F O R 
             U T I L I T Y   F U N C T I O N S   T H A T   A I D
            R F N V   - - >   R F C O N F I G   M I G R A T I O N

DESCRIPTION
  This file implements utility functions available for migrating
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


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_rfconfig_migration_util.c#2 $ $DateTime: 2008/11/19 11:46:56 $ $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/08   et      klocwork fixes
06/22/07   Vish    Made rfnv_rfconfig_migration_util_get_1x_[rise/fall/im_level]_for_lna()
                   APIs as static to this module.
                   Fixed handling of NULL pointers should RF_ASSERT() not be
                   compiled in to catch failing assertions in the production build.
06/20/07   Vish    Created initial version.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "rfnv_rfconfig_migration_util.h"
#include "rfcom.h"
#include "rfnv_rfconfig_migration_tracker.h"
#include "rfnv_cdma1x.h"
#include "rf_error.h"
#include "rfconfig_rx_lna.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/
// helper function for getting RISE val from RFNV digital item store
static int1 rfnv_rfconfig_migration_util_get_1x_rise_for_lna(
   const rfnv_digital_items_type                    *rfnv_digital_items_ptr,
   rf_path_enum_type                                 rx_lna_path,
   rfnv_rfconfig_migration_util_lna_gain_step_type   rx_lna_gain_step,
   rflib_power_mode_type                             rx_lna_linearity);
// helper function for getting FALL val from RFNV digital item store
static int1 rfnv_rfconfig_migration_util_get_1x_fall_for_lna(
   const rfnv_digital_items_type                    *rfnv_digital_items_ptr,
   rf_path_enum_type                                 rx_lna_path,
   rfnv_rfconfig_migration_util_lna_gain_step_type   rx_lna_gain_step,
   rflib_power_mode_type                             rx_lna_linearity);
// helper function for getting IM_LEVEL val from RFNV digital item store
static int1 rfnv_rfconfig_migration_util_get_1x_im_level_for_lna(
   const rfnv_digital_items_type                    *rfnv_digital_items_ptr,
   rf_path_enum_type                                 rx_lna_path,
   rfnv_rfconfig_migration_util_lna_gain_step_type   rx_lna_gain_step);


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/
// function for enabling tracking using an established RFNV->RFConfig conversion guide map
void rfnv_rfconfig_migration_util_enable_tracking_for_nv(
   nv_items_enum_type                       nv_item_id,
   rfnv_rfconfig_migration_util_map_type   *map_array,
   uint32                                   map_array_size)
{
   uint32 index = 0;

   RF_ASSERT(NULL != map_array);
   RF_ASSERT(map_array_size > 0);

   if (NULL != map_array)
   {
      for (index = 0; index < map_array_size; index++)
      {
         if (map_array[index].nv_item_id == nv_item_id)
         {
            // found a match in the map for the requested NV item
            rfnv_rfconfig_migration_tracker_enable(
               map_array[index].tracker_ptr,
               map_array[index].migration_item_id);
            break; // break after first match
         } //if
      } //for (index = 0; ...
   } //if (NULL != map_array) ...
}

// Function for converting RFNV items to RFConfigDBase; those conversions that
//  are unique only to certain chipsets will NOT be handled here. Those
//  will be relegated to chipset specific migration functions
void rfnv_rfconfig_migration_util_perform_conversion(
   const rfnv_digital_items_type               *rfnv_digital_items_ptr,
   boolean                                      use_1x_rfnv_items_to_init_evdo_sw_pts,
   rfconfig_rx_lna_type                        *prx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *prx_rfconfig_migration_tracker_ptr,
   rfconfig_rx_lna_type                        *drx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *drx_rfconfig_migration_tracker_ptr)
{
   // This function will handle migration of the following NV items:
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_RISE
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_FALL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_IM_LEVEL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_RISE
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_FALL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_IM_LEVEL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_RISE
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_FALL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_IM_LEVEL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_RISE
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_FALL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_IM_LEVEL
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1
   //   RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2

   typedef struct
   {
      rflib_power_mode_type                            rfconfig_rx_lna_linearity;
      rfnv_rfconfig_migration_util_lna_gain_step_type  rfconfig_rx_lna_gain_step;
      rfnv_rfconfig_migration_item_type                rfnv_rfconfig_migration_item_rise;
      rfnv_rfconfig_migration_item_type                rfnv_rfconfig_migration_item_fall;
      rfnv_rfconfig_migration_item_type                rfnv_rfconfig_migration_item_im_level;
   } fixed_switch_pt_migration_params_list_type;

   // PRx migration params list
   //   IMPORTANT to have at least one element
   fixed_switch_pt_migration_params_list_type  prx_fixed_switch_pt_migration_params_list[] = {
      // high linearity
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_IM_LEVEL
      },
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_IM_LEVEL
      },
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_IM_LEVEL
      },
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_IM_LEVEL
      },
#ifdef RF_HAS_POWER_MODE_LIB
      // LPM_1 linearity      
      {
         RFLIB_LOW_POWER_MODE_1,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_IM_LEVEL
      },
      {
         RFLIB_LOW_POWER_MODE_1,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_IM_LEVEL
      },
      {
         RFLIB_LOW_POWER_MODE_1,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_IM_LEVEL
      },
      {
         RFLIB_LOW_POWER_MODE_1,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_1,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_IM_LEVEL
      },
      // LPM_2 linearity      
      {
         RFLIB_LOW_POWER_MODE_2,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_IM_LEVEL
      },
      {
         RFLIB_LOW_POWER_MODE_2,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_IM_LEVEL
      },
      {
         RFLIB_LOW_POWER_MODE_2,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_IM_LEVEL
      },
      {
         RFLIB_LOW_POWER_MODE_2,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // rise
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2,  // fall
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_IM_LEVEL
      }
#endif /* RF_HAS_POWER_MODE_LIB */
   };
   // DRx migration params list
   //   IMPORTANT to have at least one element
   fixed_switch_pt_migration_params_list_type  drx_fixed_switch_pt_migration_params_list[] = {
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA1_IM_LEVEL
      },
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA2_IM_LEVEL
      },
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA3_IM_LEVEL
      },
      {
         RFLIB_NORMAL_POWER_MODE,
         RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_RISE,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_FALL,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA4_IM_LEVEL
      }
   };
   uint32  index = 0;

   RF_ASSERT(NULL != rfnv_digital_items_ptr);

   if (NULL != rfnv_digital_items_ptr)
   {
      // PRx NV items migration
      if ( (NULL != prx_rfconfig_rx_lna_ptr)
           && (NULL != prx_rfconfig_migration_tracker_ptr) )
      {
         for ( index = 0; index < ARR_SIZE(prx_fixed_switch_pt_migration_params_list); index ++ )
         {
            // In NV, each linearity has its own RISE and FALL values but IM_LEVEL is shared across
            //   all linearities. So only RISE and FALL items truly indicate the availability of a
            //   linearity mode
            if ( rfnv_rfconfig_migration_tracker_is_enabled(
                    prx_rfconfig_migration_tracker_ptr,
                    prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_rise)
                 || rfnv_rfconfig_migration_tracker_is_enabled(
                       prx_rfconfig_migration_tracker_ptr,
                       prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_fall) )
            {
               // 1X mode FIXED/OLTA switch point settings
               rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed_or_olta(
                  FALSE,  // do NOT force it to FIXED type in case it is OLTA previously in RFConfig
                  prx_rfconfig_rx_lna_ptr,
                  RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
                  prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity,
                  prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                  prx_rfconfig_migration_tracker_ptr,
                  prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_rise,
                  rfnv_rfconfig_migration_util_get_1x_rise_for_lna(
                     rfnv_digital_items_ptr,
                     RF_PATH_0,
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                  prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_fall,
                  rfnv_rfconfig_migration_util_get_1x_fall_for_lna(
                     rfnv_digital_items_ptr,
                     RF_PATH_0,
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                  prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_im_level,
                  rfnv_rfconfig_migration_util_get_1x_im_level_for_lna(
                     rfnv_digital_items_ptr,
                     RF_PATH_0,
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step) ); // for G(i)<->G(i+1)

               if ( use_1x_rfnv_items_to_init_evdo_sw_pts )
               {
                  // EVDO uses the same settings as 1X above
                  rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed_or_olta(
                     FALSE,  // do NOT force it to FIXED type in case it is OLTA previously in RFConfig
                     prx_rfconfig_rx_lna_ptr,
                     RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity,
                     prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                     prx_rfconfig_migration_tracker_ptr,
                     prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_rise,
                     rfnv_rfconfig_migration_util_get_1x_rise_for_lna(
                        rfnv_digital_items_ptr,
                        RF_PATH_0,
                        prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                        prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                     prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_fall,
                     rfnv_rfconfig_migration_util_get_1x_fall_for_lna(
                        rfnv_digital_items_ptr,
                        RF_PATH_0,
                        prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                        prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                     prx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_im_level,
                     rfnv_rfconfig_migration_util_get_1x_im_level_for_lna(
                        rfnv_digital_items_ptr,
                        RF_PATH_0,
                        prx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step) ); // for G(i)<->G(i+1)
               } //if ( use_1x_rfnv_items_to_init_evdo_sw_pts ) ...
            } //if (rfnv_rfconfig_migration_tracker_is_enabled(RISE) || rfnv_rfconfig_migration_tracker_is_enabled(FALL)) ...
         } //for ( index = 0; index < ARR_SIZE(prx_fixed_switch_pt_migration_params_list ...
      } //if ( (NULL != prx_rfconfig_rx_lna_ptr) ...

      // DRx NV items migration
      if ( (NULL != drx_rfconfig_rx_lna_ptr)
           && (NULL != drx_rfconfig_migration_tracker_ptr) )
      {
         for ( index = 0; index < ARR_SIZE(drx_fixed_switch_pt_migration_params_list); index ++ )
         {
            // In NV, each linearity has its own RISE and FALL values but IM_LEVEL is shared across
            //   all linearities. So only RISE and FALL items truly indicate the availability of a
            //   linearity mode
            if ( rfnv_rfconfig_migration_tracker_is_enabled(
                    drx_rfconfig_migration_tracker_ptr,
                    drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_rise)
                 || rfnv_rfconfig_migration_tracker_is_enabled(
                       drx_rfconfig_migration_tracker_ptr,
                       drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_fall) )
            {
               // 1X mode FIXED/OLTA switch point settings
               rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed_or_olta(
                  FALSE,  // do NOT force it to FIXED type in case it is OLTA previously in RFConfig
                  drx_rfconfig_rx_lna_ptr,
                  RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
                  drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity,
                  drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                  drx_rfconfig_migration_tracker_ptr,
                  drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_rise,
                  rfnv_rfconfig_migration_util_get_1x_rise_for_lna(
                     rfnv_digital_items_ptr,
                     RF_PATH_1,
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                  drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_fall,
                  rfnv_rfconfig_migration_util_get_1x_fall_for_lna(
                     rfnv_digital_items_ptr,
                     RF_PATH_1,
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                  drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_im_level,
                  rfnv_rfconfig_migration_util_get_1x_im_level_for_lna(
                     rfnv_digital_items_ptr,
                     RF_PATH_1,
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step) ); // for G(i)<->G(i+1)

               if ( use_1x_rfnv_items_to_init_evdo_sw_pts )
               {
                  // EVDO uses the same settings as 1X above
                  rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed_or_olta(
                     FALSE,  // do NOT force it to FIXED type in case it is OLTA previously in RFConfig
                     drx_rfconfig_rx_lna_ptr,
                     RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity,
                     drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                     drx_rfconfig_migration_tracker_ptr,
                     drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_rise,
                     rfnv_rfconfig_migration_util_get_1x_rise_for_lna(
                        rfnv_digital_items_ptr,
                        RF_PATH_1,
                        drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                        drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                     drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_fall,
                     rfnv_rfconfig_migration_util_get_1x_fall_for_lna(
                        rfnv_digital_items_ptr,
                        RF_PATH_1,
                        drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step, // for G(i)<->G(i+1)
                        drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_linearity),
                     drx_fixed_switch_pt_migration_params_list[index].rfnv_rfconfig_migration_item_im_level,
                     rfnv_rfconfig_migration_util_get_1x_im_level_for_lna(
                        rfnv_digital_items_ptr,
                        RF_PATH_1,
                        drx_fixed_switch_pt_migration_params_list[index].rfconfig_rx_lna_gain_step) ); // for G(i)<->G(i+1)
               } //if ( use_1x_rfnv_items_to_init_evdo_sw_pts ) ...
            } //if (rfnv_rfconfig_migration_tracker_is_enabled(RISE) || rfnv_rfconfig_migration_tracker_is_enabled(FALL)) ...
         } //for ( index = 0; index < ARR_SIZE(drx_fixed_switch_pt_migration_params_list ...
      } //if ( (NULL != drx_rfconfig_rx_lna_ptr) ...
   } //if (NULL != rfnv_digital_items_ptr) ...
}


// function for migrating a FIXED RFNV switch point to any type (FIXED/OLTA)
//   of RFConfigDBase switch point
void rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed_or_olta(
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
   int1                                         rfnv_im_level_val)
{
   // Case #1: For already existing OLTA switch point in RFConfig,
   //    if 'force_rfconfig_olta_to_fixed' is TRUE
   //       if whole set in {rise,fall,im} is defined in NV
   //          change RFConfig switch pt to FIXED based on {rise,fall,im}
   //       else if NULL set in {rise,fall} is defined in NV
   //          NOP
   //       else
   //          improperly defined subset of {rise,fall,im} is present in NV
   //          raise an error
   //       Result: RFConfig switch pt becomes FIXED if there's no error
   //    else (i.e. 'force_rfconfig_olta_to_fixed' is FALSE)
   //       if whole set in {rise,fall} is defined in NV
   //          set RFConfig's OLTA hysteresis to (rise-fall)
   //       else if NULL set in {rise,fall} is defined in NV
   //          NOP
   //       else
   //          improperly defined subset of {rise,fall} is present in NV
   //          raise an error
   //       Result: RFConfig switch pt remains as OLTA
   // 
   // Case #2: For already existing FIXED switch point in RFConfig,
   //    if whole/partial set in {rise,fall,im} is defined in NV
   //       set RFConfig's FIXED switch point based on available {rise,fall,im}
   //    else if NULL set in {rise,fall,im} is defined in NV
   //       NOP
   // 
   //  After this routine returns successfully, the end result will be a
   //  properly formed FIXED/OLTA RFConfigDBase switch point.

   const rfconfig_rx_lna_switch_pt_type  *read_rfconfig_rx_lna_switch_pt_ptr = NULL;
         rfconfig_rx_lna_switch_pt_type   migrating_rfconfig_rx_lna_switch_pt;
         boolean                          rfnv_rise_migration_reqd = FALSE;
         boolean                          rfnv_fall_migration_reqd = FALSE;
         boolean                          rfnv_im_level_migration_reqd = FALSE;
         boolean                          migration_successful = FALSE;

   if ( (NULL != rfconfig_rx_lna_config_ptr)
        && (NULL != rfnv_rfconfig_migration_tracker_ptr) )
   {
      rfnv_rise_migration_reqd = rfnv_rfconfig_migration_tracker_is_enabled(
                                    rfnv_rfconfig_migration_tracker_ptr,
                                    rfnv_rfconfig_migration_item_rise);
      rfnv_fall_migration_reqd = rfnv_rfconfig_migration_tracker_is_enabled(
                                    rfnv_rfconfig_migration_tracker_ptr,
                                    rfnv_rfconfig_migration_item_fall);
      rfnv_im_level_migration_reqd = rfnv_rfconfig_migration_tracker_is_enabled(
                                        rfnv_rfconfig_migration_tracker_ptr,
                                        rfnv_rfconfig_migration_item_im_level);
      if ( rfnv_rise_migration_reqd
           || rfnv_fall_migration_reqd
           || rfnv_im_level_migration_reqd )
      {
         // there is an intention to migrate, for sure
         read_rfconfig_rx_lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info(
                                                 rfconfig_rx_lna_config_ptr,
                                                 rfconfig_rx_lna_switch_pt_mode,
                                                 rfconfig_rx_lna_linearity_index,
                                                 rfconfig_rx_lna_gain_step_index);

         if (NULL != read_rfconfig_rx_lna_switch_pt_ptr)
         {
            migrating_rfconfig_rx_lna_switch_pt = *read_rfconfig_rx_lna_switch_pt_ptr;

            switch (rfconfig_rx_lna_switch_pt_get_config_mode(&migrating_rfconfig_rx_lna_switch_pt))
            {
               case RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED:
               {
                  // Whole/partial subset/NULL set migration allowed
                  if ( rfnv_rise_migration_reqd )
                  {
                     rfconfig_rx_lna_switch_pt_set_rise(
                        &migrating_rfconfig_rx_lna_switch_pt,
                        rfnv_rise_val);
                  }
                  if ( rfnv_fall_migration_reqd )
                  {
                     rfconfig_rx_lna_switch_pt_set_fall(
                        &migrating_rfconfig_rx_lna_switch_pt,
                        rfnv_fall_val);
                  }
                  if ( rfnv_im_level_migration_reqd )
                  {
                     rfconfig_rx_lna_switch_pt_set_im(
                        &migrating_rfconfig_rx_lna_switch_pt,
                        rfnv_im_level_val);
                  }

                  migration_successful = TRUE;
                  break;
               }
               case RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA:
               {
                  if ( force_rfconfig_olta_to_fixed )
                  {
                     // Force RFConfig OLTA to FIXED based on NV {rise,fall,im}
                     if ( rfnv_rise_migration_reqd
                          && rfnv_fall_migration_reqd
                          && rfnv_im_level_migration_reqd )
                     {
                        // Whole {rise,fall,im} set migrates
                        //  Update FIXED switch point based on {rise,fall,im}
                        rfconfig_rx_lna_switch_pt_set_fixed_config(
                           &migrating_rfconfig_rx_lna_switch_pt,
                           rfnv_rise_val,
                           rfnv_fall_val,
                           rfnv_im_level_val,
                           rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(
                              &migrating_rfconfig_rx_lna_switch_pt),
                           rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(
                              &migrating_rfconfig_rx_lna_switch_pt) );

                        migration_successful = TRUE;
                     }
                     else if ( (!rfnv_rise_migration_reqd)
                               && (!rfnv_fall_migration_reqd)
                               && (!rfnv_im_level_migration_reqd) )
                     {
                        // Null set {} migrates
                        //  NOP            
                        migration_successful = TRUE;
                     }
                     // else
                     //   error case!
                  }
                  else
                  {
                     // Update RFConfig OLTA using NV {rise,fall}
                     if ( rfnv_rise_migration_reqd
                          && rfnv_fall_migration_reqd )
                     {
                        // Whole {rise,fall} set migrates
                        //  Update hysteresis based on the difference between NV rise
                        //  and fall values for FIXED switch pt
                        rfconfig_rx_lna_switch_pt_set_hysteresis(
                           &migrating_rfconfig_rx_lna_switch_pt,
                           rfnv_rise_val - rfnv_fall_val);

                        migration_successful = TRUE;
                     }
                     else if ( (!rfnv_rise_migration_reqd)
                               && (!rfnv_fall_migration_reqd) )
                     {
                        // Null set {} migrates
                        //  NOP            
                        migration_successful = TRUE;
                     }
                     // else
                     //   error case!
                  }

                  break;
               }
               default:
               {
                  // error case!
                  break;
               }
            } //switch (rfconfig_rx_lna_switch_pt_get_config_mode(&migrating_rfconfig_rx_lna_switch_pt)) ...
         } //if (NULL != read_rfconfig_rx_lna_switch_pt_ptr) ...

         if ( migration_successful )
         {
            // record the successfully migrated switch point in RFConfigDBase
            rfconfig_rx_lna_set_switch_point_info(
               rfconfig_rx_lna_config_ptr,
               rfconfig_rx_lna_switch_pt_mode,
               rfconfig_rx_lna_linearity_index,
               rfconfig_rx_lna_gain_step_index,
               &migrating_rfconfig_rx_lna_switch_pt);
         } //if ( migration_successful ) ...
         else
         {
            RF_FATAL_ERROR("Migration of RFNV-->RFConfig FIXED LNA sw pt failed");
         } //else-if ( migration_successful ) ...
         
      } //if (rfnv_rise_migration_reqd || rfnv_fall_migration_reqd || rfnv_im_level_migration_reqd) ...
   } //if ((NULL != rfconfig_rx_lna_config_ptr) && (NULL != rfnv_rfconfig_migration_tracker_ptr)) ...
}


// function for migrating a fixed RFNV switch point ONLY IF the corresponding
//   RFConfigDBase switch point is already of FIXED type
void rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed(
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
   int1                                         rfnv_im_level_val)
{
   // Case #1: If already existing switch point in RFConfig = FIXED
   //    if whole/partial set in {rise,fall,im} is defined in NV
   //       set RFConfig's FIXED switch point based on available {rise,fall,im}
   //    else if NULL set in {rise,fall,im} is defined in NV
   //       NOP
   // 
   // Case #2: If already existing switch point in RFConfig = OLTA
   //    NOP
   // 
   //  After this routine returns successfully, the end result will be a
   //  properly formed FIXED RFConfigDBase switch point.

   const rfconfig_rx_lna_switch_pt_type  *read_rfconfig_rx_lna_switch_pt_ptr = NULL;
         rfconfig_rx_lna_switch_pt_type   migrating_rfconfig_rx_lna_switch_pt;
         boolean                          rfnv_rise_migration_reqd = FALSE;
         boolean                          rfnv_fall_migration_reqd = FALSE;
         boolean                          rfnv_im_level_migration_reqd = FALSE;

   if ( (NULL != rfconfig_rx_lna_config_ptr)
        && (NULL != rfnv_rfconfig_migration_tracker_ptr) )
   {
      rfnv_rise_migration_reqd = rfnv_rfconfig_migration_tracker_is_enabled(
                                    rfnv_rfconfig_migration_tracker_ptr,
                                    rfnv_rfconfig_migration_item_rise);
      rfnv_fall_migration_reqd = rfnv_rfconfig_migration_tracker_is_enabled(
                                    rfnv_rfconfig_migration_tracker_ptr,
                                    rfnv_rfconfig_migration_item_fall);
      rfnv_im_level_migration_reqd = rfnv_rfconfig_migration_tracker_is_enabled(
                                        rfnv_rfconfig_migration_tracker_ptr,
                                        rfnv_rfconfig_migration_item_im_level);
      if ( rfnv_rise_migration_reqd
           || rfnv_fall_migration_reqd
           || rfnv_im_level_migration_reqd )
      {
         // there is an intention to migrate, for sure
         read_rfconfig_rx_lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info(
                                                 rfconfig_rx_lna_config_ptr,
                                                 rfconfig_rx_lna_switch_pt_mode,
                                                 rfconfig_rx_lna_linearity_index,
                                                 rfconfig_rx_lna_gain_step_index);
         if (NULL != read_rfconfig_rx_lna_switch_pt_ptr)
         {
            if ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED
                    == rfconfig_rx_lna_switch_pt_get_config_mode(
                          read_rfconfig_rx_lna_switch_pt_ptr) )
            {
               migrating_rfconfig_rx_lna_switch_pt = *read_rfconfig_rx_lna_switch_pt_ptr;

               // Whole/partial subset/NULL set migration allowed
               if ( rfnv_rise_migration_reqd )
               {
                  rfconfig_rx_lna_switch_pt_set_rise(
                     &migrating_rfconfig_rx_lna_switch_pt,
                     rfnv_rise_val);
               }
               if ( rfnv_fall_migration_reqd )
               {
                  rfconfig_rx_lna_switch_pt_set_fall(
                     &migrating_rfconfig_rx_lna_switch_pt,
                     rfnv_fall_val);
               }
               if ( rfnv_im_level_migration_reqd )
               {
                  rfconfig_rx_lna_switch_pt_set_im(
                     &migrating_rfconfig_rx_lna_switch_pt,
                     rfnv_im_level_val);
               }

               // record the successfully migrated switch point in RFConfigDBase
               rfconfig_rx_lna_set_switch_point_info(
                  rfconfig_rx_lna_config_ptr,
                  rfconfig_rx_lna_switch_pt_mode,
                  rfconfig_rx_lna_linearity_index,
                  rfconfig_rx_lna_gain_step_index,
                  &migrating_rfconfig_rx_lna_switch_pt);
            } //if ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == ...
         } //if (NULL != read_rfconfig_rx_lna_switch_pt_ptr) ...
         else
         {
            RF_FATAL_ERROR("Migration of RFNV-->RFConfig FIXED LNA sw pt failed");
         } //else-if (NULL != read_rfconfig_rx_lna_switch_pt_ptr) ...
      } //if (rfnv_rise_migration_reqd || rfnv_fall_migration_reqd || rfnv_im_level_migration_reqd) ...
   } //if ((NULL != rfconfig_rx_lna_config_ptr) && (NULL != rfnv_rfconfig_migration_tracker_ptr)) ...
}


// helper function for getting RISE val from RFNV digital item store
static int1 rfnv_rfconfig_migration_util_get_1x_rise_for_lna(
   const rfnv_digital_items_type                    *rfnv_digital_items_ptr,
   rf_path_enum_type                                 rx_lna_path,
   rfnv_rfconfig_migration_util_lna_gain_step_type   rx_lna_gain_step,
   rflib_power_mode_type                             rx_lna_linearity)
{
   int1  rise_val = 0;

   RF_ASSERT(NULL != rfnv_digital_items_ptr);
   RF_ASSERT(rx_lna_path < RF_PATH_NUM);
   RF_ASSERT(rx_lna_gain_step < RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__INVALID);
#ifdef RF_HAS_POWER_MODE_LIB
   RF_ASSERT(rx_lna_linearity < RFLIB_MAX_POWER_MODE);
#else /* RF_HAS_POWER_MODE_LIB */
   RF_ASSERT(RFLIB_NORMAL_POWER_MODE == rx_lna_linearity);
#endif /* RF_HAS_POWER_MODE_LIB */

   switch (rx_lna_gain_step)
   {
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_1_rise[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_1_rise[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_2_rise[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_2_rise[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_3_rise[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_3_rise[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_4_rise[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         rise_val = rfnv_digital_items_ptr->rfnv_lna_range_4_rise[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      default:
      {
         // unreachable but added to satisfy's lint's/compiler's curiosity
         RF_FATAL_ERROR("Unexpected fatal error");
         break;
      }
   } //switch (rx_lna_gain_step)

   return rise_val;
}


// helper function for getting FALL val from RFNV digital item store
static int1 rfnv_rfconfig_migration_util_get_1x_fall_for_lna(
   const rfnv_digital_items_type                    *rfnv_digital_items_ptr,
   rf_path_enum_type                                 rx_lna_path,
   rfnv_rfconfig_migration_util_lna_gain_step_type   rx_lna_gain_step,
   rflib_power_mode_type                             rx_lna_linearity)
{
   int1  fall_val = 0;

   RF_ASSERT(NULL != rfnv_digital_items_ptr);
   RF_ASSERT(rx_lna_path < RF_PATH_NUM);
   RF_ASSERT(rx_lna_gain_step < RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__INVALID);
#ifdef RF_HAS_POWER_MODE_LIB
   RF_ASSERT(rx_lna_linearity < RFLIB_MAX_POWER_MODE);
#else /* RF_HAS_POWER_MODE_LIB */
   RF_ASSERT(RFLIB_NORMAL_POWER_MODE == rx_lna_linearity);
#endif /* RF_HAS_POWER_MODE_LIB */

   if (rx_lna_linearity >= RFLIB_MAX_POWER_MODE) 
   {
	   RF_FATAL_ERROR("rx_lna_linearity invalid");
	   rf_error_flag = TRUE;
	   return 0;
   }

   if (rx_lna_path >= RF_PATHS_TABLE_SIZ) 
   {
	   RF_FATAL_ERROR("rx_lna_path invalid");
	   rf_error_flag = TRUE;
	   return 0;
   }

   switch (rx_lna_gain_step)
   {
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_1_fall[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_1_fall[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_2_fall[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_2_fall[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_3_fall[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_3_fall[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4:
      {
#ifdef RF_HAS_POWER_MODE_LIB
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_4_fall[rx_lna_path][rx_lna_linearity];
#else /* RF_HAS_POWER_MODE_LIB */
         fall_val = rfnv_digital_items_ptr->rfnv_lna_range_4_fall[rx_lna_path];
#endif /* RF_HAS_POWER_MODE_LIB */
         break;
      }
      default:
      {
         // unreachable but added to satisfy's lint's/compiler's curiosity
         RF_FATAL_ERROR("Unexpected fatal error");
         break;
      }
   } //switch (rx_lna_gain_step)

   return fall_val;
}


// helper function for getting IM_LEVEL val from RFNV digital item store
static int1 rfnv_rfconfig_migration_util_get_1x_im_level_for_lna(
   const rfnv_digital_items_type                    *rfnv_digital_items_ptr,
   rf_path_enum_type                                 rx_lna_path,
   rfnv_rfconfig_migration_util_lna_gain_step_type   rx_lna_gain_step)
{
   int1  im_level_val = 0;

   RF_ASSERT(NULL != rfnv_digital_items_ptr);
   RF_ASSERT(rx_lna_path < RF_PATH_NUM);
   if (rx_lna_path >= RF_PATH_MAX) 
   {
	   RF_FATAL_ERROR("rx_lna_path >= RF_PATH_MAX");
	   rf_error_flag = TRUE;
	   return 0;
   }
   RF_ASSERT(rx_lna_gain_step < RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__INVALID);

   switch (rx_lna_gain_step)
   {
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G0_G1:
      {
         im_level_val = rfnv_digital_items_ptr->rfnv_im_level1[rx_lna_path];
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G1_G2:
      {
         im_level_val = rfnv_digital_items_ptr->rfnv_im_level2[rx_lna_path];
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G2_G3:
      {
         im_level_val = rfnv_digital_items_ptr->rfnv_im_level3[rx_lna_path];
         break;
      }
      case RFNV_RFCONFIG_MIGRATION_UTIL_LNA_GAIN_STEP__G3_G4:
      {
         im_level_val = rfnv_digital_items_ptr->rfnv_im_level4[rx_lna_path];
         break;
      }
      default:
      {
         // unreachable but added to satisfy's lint's/compiler's curiosity
         RF_FATAL_ERROR("Unexpected fatal error");
         break;
      }
   } //switch (rx_lna_gain_step)

   return im_level_val;
}


#endif /* RF_HAS_RFCONFIG_RX_LNA */


