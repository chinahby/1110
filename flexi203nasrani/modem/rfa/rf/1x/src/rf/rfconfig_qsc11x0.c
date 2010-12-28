/*===========================================================================
          R F C O N F I G   I M P L E M E N T A T I O N   F I L E
                     F O R   Q S C 6 0 X 5   C H I P

DESCRIPTION
  This file implements the interface available for accessing default configuration
  information for QSC11X0 chip. Currently this includes the following:
   - Rx LNA switch point info for both primary and secondary chains for all
     band classes

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfconfig_qsc11x0.c#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/07   ems     Ported from SC2X 3044
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfconfig_qsc11x0.h"
#include "rfnv_rfconfig_migration_util.h"
#include "rflib_share.h"
#include "err.h"
#include "rf_error.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/
#ifdef RF_HAS_BC0_NV_TABLE
static void rfconfig_qsc11x0_defaults_init_bc0(
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr);
#endif /* RF_HAS_BC0_NV_TABLE */

#if (defined(RF_HAS_BC1_NV_TABLE) || defined(RF_HAS_BC4_NV_TABLE)      \
     || defined(RF_HAS_BC6_NV_TABLE) || defined(RF_HAS_BC14_NV_TABLE)  \
     || defined(RF_HAS_BC15_NV_TABLE))
static void rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15(
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr);
#endif /* defined(RF_HAS_BC1_NV_TABLE) || defined(RF_HAS_BC4_NV_TABLE) ... */

#if (defined(RF_HAS_BC3_NV_TABLE) || defined(RF_HAS_BC10_NV_TABLE))
static void rfconfig_qsc11x0_defaults_init_bc3_bc10(
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr);
#endif /* defined(RF_HAS_BC3_NV_TABLE) || defined(RF_HAS_BC10_NV_TABLE) */


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================
FUNCTION   rfconfig_qsc11x0_init_default_rx_lna_info      EXTERNAL FUNCTION

DESCRIPTION
   This function returns the default configuration for both primary and
   secondary chain Rx LNA switch points for all linearities and operational
   modes (1X and EVDO) supported by QSC11X0 chip for a given bandclass.

DEPENDENCIES
   None.

RETURN VALUE
   Indicates whether this chip supports the specified band class.
      TRUE --> Config data returned through 'primary_rx_lna_info_ptr' and
               'secondary_rx_lna_info_ptr' is valid.
      FALSE --> Otherwise.

SIDE EFFECTS
   None.
===========================================================================*/
boolean rfconfig_qsc11x0_init_default_rx_lna_info(
   rf_card_band_type         band_class,
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr)
{
   boolean success = FALSE;
   static const struct
   {
      rf_card_band_type   bc;
      void (*init_fn) (rfconfig_rx_lna_type*, rfconfig_rx_lna_type*);
   } bc_to_init_fn_map[] = {
      #ifdef RF_HAS_BC0_NV_TABLE
      { RF_BC0_BAND, rfconfig_qsc11x0_defaults_init_bc0 },
      #endif /* RF_HAS_BC0_NV_TABLE */
      #ifdef RF_HAS_BC1_NV_TABLE
      { RF_BC1_BAND, rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15 },
      #endif /* RF_HAS_BC1_NV_TABLE */
      #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
      #endif /* RF_HAS_BC3_NV_TABLE */
      #ifdef RF_HAS_BC4_NV_TABLE
      { RF_BC4_BAND, rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15 },
      #endif /* RF_HAS_BC4_NV_TABLE */
      #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
      #endif /* RF_HAS_BC6_NV_TABLE */
      #ifdef RF_HAS_BC10_NV_TABLE
      { RF_BC10_BAND, rfconfig_qsc11x0_defaults_init_bc3_bc10 },
      #endif /* RF_HAS_BC10_NV_TABLE */
      #ifdef RF_HAS_BC14_NV_TABLE
      { RF_BC14_BAND, rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15 },
      #endif /* RF_HAS_BC14_NV_TABLE */
      #ifdef RF_HAS_BC15_NV_TABLE
      { RF_BC15_BAND, rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15 },
      #endif /* RF_HAS_BC15_NV_TABLE */
      { RF_MAX_BAND, NULL }  /* ensure at least one element is present */
   };
   uint32 index = 0;

   // check if the requested band_class is supported
   for (index = 0; index < ARR_SIZE(bc_to_init_fn_map); index ++)
   {
      if ( (bc_to_init_fn_map[index].bc == band_class)
           && (NULL != bc_to_init_fn_map[index].init_fn) )
      {
         // correct init_fn found; invoke it
         bc_to_init_fn_map[index].init_fn(
            primary_rx_lna_info_ptr,
            secondary_rx_lna_info_ptr);
         success = TRUE;
         break;
      }
   }

   if ( !success )
   {
      RF_FATAL_ERROR("Unknown QSC11X0 band class");
   }

   return success;
}


/*==========================================================================
FUNCTION   rfconfig_qsc11x0_migrate_from_rfnv             EXTERNAL FUNCTION

DESCRIPTION
   This function migrates data from initialized RFNV Rx LNA switch point
   items to RFConfigDBase for both primary and secondary Rx chains for the
   specified band class. The migration tracker objects indicate what
   particular RFNV items have actually been read from QCN file; only those
   reads items are considered for migration by this routine.

DEPENDENCIES
   A default RFConfigDBase must have been created earlier (using
   rfconfig_qsc11x0_init_default_rx_lna_info(), for instance) to accomodate
   migrating RFNV data items.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
void rfconfig_qsc11x0_migrate_from_rfnv(
   rf_card_band_type                            band_class,
   const rfnv_digital_items_type               *rfnv_digital_items_ptr,
   boolean                                      use_1x_rfnv_items_to_init_evdo_sw_pts,
   rfconfig_rx_lna_type                        *prx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *prx_rfconfig_migration_tracker_ptr,
   rfconfig_rx_lna_type                        *drx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *drx_rfconfig_migration_tracker_ptr)
{
   // Here is the opportunity to tweak the tracker(s)/RFNV item(s) for PRx and DRx for
   //  this particular chipset before they are handed over to default migration function
   //  below which would otherwise handle NV items in a chipset independent manner.
   //
   //  For QSC11X0, we have to ignore LPM_2 NV items for LNA RISE and FALL values even
   //   if they are erroneously set in QCN file.
   const rfnv_rfconfig_migration_tracker_type  *qsc11x0_prx_rfconfig_migration_tracker_ptr = prx_rfconfig_migration_tracker_ptr;
   rfnv_rfconfig_migration_tracker_type         qsc11x0_prx_rfconfig_migration_tracker;

   if (NULL != prx_rfconfig_migration_tracker_ptr)
   {
      qsc11x0_prx_rfconfig_migration_tracker = *prx_rfconfig_migration_tracker_ptr;
      rfnv_rfconfig_migration_tracker_disable(
         &qsc11x0_prx_rfconfig_migration_tracker,
         RFNV_RFCONFIG_MIGRATION_ITEM__LNA_SWITCH_POINTS_LPM_2);
      qsc11x0_prx_rfconfig_migration_tracker_ptr = &qsc11x0_prx_rfconfig_migration_tracker;
   }
   
   // first we'll perform common RFNV migration that's applicable to all chipsets
   rfnv_rfconfig_migration_util_perform_conversion(
      rfnv_digital_items_ptr,
      use_1x_rfnv_items_to_init_evdo_sw_pts,
      prx_rfconfig_rx_lna_ptr,
      qsc11x0_prx_rfconfig_migration_tracker_ptr,
      drx_rfconfig_rx_lna_ptr,
      drx_rfconfig_migration_tracker_ptr);

   // then perform any QSC11X0 specific migration
   RF_ASSERT(NULL != prx_rfconfig_rx_lna_ptr);

   #ifdef RF_HAS_BC0_NV_TABLE
   if ( (RF_BC0_BAND == band_class)
        && (NULL != qsc11x0_prx_rfconfig_migration_tracker_ptr) )
   {
      // for PRx only, migrate EVDO mode's G0<->G1 RFConfig switch point for high linearity
      //   i.e. apply NV_BC0_HDR_G0_IM_[RISE/FALL/LEVEL]_I to G0<->G1 iff
      //     - band_class = BC0,
      //     - current_mode = EVDO,
      //     - power_mode = HIGH (i.e. RFLIB_NORMAL_POWER_MODE), and
      //     - if it isn't already OLTA (i.e. it is FIXED)
      rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed(
         prx_rfconfig_rx_lna_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         0, // for G0<->G1
         qsc11x0_prx_rfconfig_migration_tracker_ptr,
         RFNV_RFCONFIG_MIGRATION_ITEM__HDR_G0_IM_RISE,
         rfnv_digital_items_ptr->rfnv_hdr_g0_im_rise,
         RFNV_RFCONFIG_MIGRATION_ITEM__HDR_G0_IM_FALL,
         rfnv_digital_items_ptr->rfnv_hdr_g0_im_fall,
         RFNV_RFCONFIG_MIGRATION_ITEM__HDR_G0_IM_LEVEL,
         rfnv_digital_items_ptr->rfnv_hdr_g0_im_level);

      // for PRx only, migrate EVDO mode's G1<->G2 RFConfig switch point for high linearity
      //   i.e. apply NV_BC0_HDR_IM_[RISE/FALL/LEVEL]_I to G1<->G2 iff
      //     - band_class = BC0,
      //     - current_mode = EVDO,
      //     - power_mode = HIGH (i.e. RFLIB_NORMAL_POWER_MODE), and
      //     - if it isn't already OLTA (i.e. it is FIXED)
      rfnv_rfconfig_migration_util_migrate_switch_point_rfnv_fixed_2_rfconfig_fixed(
         prx_rfconfig_rx_lna_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         1, // for G1<->G2
         qsc11x0_prx_rfconfig_migration_tracker_ptr,
         RFNV_RFCONFIG_MIGRATION_ITEM__HDR_IM_RISE,
         rfnv_digital_items_ptr->rfnv_hdr_im_rise,
         RFNV_RFCONFIG_MIGRATION_ITEM__HDR_IM_FALL,
         rfnv_digital_items_ptr->rfnv_hdr_im_fall,
         RFNV_RFCONFIG_MIGRATION_ITEM__HDR_IM_LEVEL,
         rfnv_digital_items_ptr->rfnv_hdr_im_level);
   } //if ( (RF_BC0_BAND == band_class) ...
   #endif /* RF_HAS_BC0_NV_TABLE */
}


#ifdef RF_HAS_BC0_NV_TABLE
/*==========================================================================
FUNCTION   rfconfig_qsc11x0_defaults_init_bc0                LOCAL FUNCTION

DESCRIPTION
   This function returns the default configuration for both primary and
   secondary chain Rx LNA switch points for all linearities and operational
   modes (1X and EVDO) supported on band class 0.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
static void rfconfig_qsc11x0_defaults_init_bc0(
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr)
{
   rfconfig_rx_lna_switch_pt_type sw_pt[4];

   if (NULL != primary_rx_lna_info_ptr)
   {
      // first reset our return val arg to start off on a clean slate
      rfconfig_rx_lna_reset(primary_rx_lna_info_ptr);

      // PRx high linearity (1X/BC0)
      //   G0/G1: rise(-88.2dBm), fall(-90.6dBm), im(-88.2dBm), bpht(6.8ms), non-bpht(0ms)
      //   G1/G2: rise(-80.6dBm), fall(-81.8dBm), im(-80.6dBm), bpht(6.8ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -61, -67, -61, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -42, -45, -42, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 67, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 60, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
      // PRx low linearity (1X/BC0)
      //   G0/G1: rise(-88.2dBm), fall(-89.4dBm), im(-88.2dBm), bpht(6.8ms), non-bpht(0ms)      
      //   G1/G2: rise(-80.6dBm), fall(-81.8dBm), im(-80.6dBm), bpht(6.8ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -61, -64, -61, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -42, -45, -42, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 67, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 60, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_LOW_POWER_MODE_1,
         4, // num_switch_pts,
         &sw_pt[0]);

      // PRx high linearity (EVDO/BC0)
      //   G0/G1: rise(-88.2dBm), fall(-92.6dBm), im(-88.2dBm), bpht(1.0ms), non-bpht(0ms)
      //   G1/G2: rise(-80.6dBm), fall(-81.8dBm), im(-80.6dBm), bpht(6.8ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -61, -72, -61, 39, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -42, -45, -42, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 67, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 60, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
      // PRx low linearity (EVDO/BC0)      
      //   G0/G1: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms)
      //   G1/G2: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[0], 10, 72, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[1], 10, 71, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 67, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 60, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_LOW_POWER_MODE_1,
         4, // num_switch_pts,
         &sw_pt[0]);
   } //if (NULL != primary_rx_lna_info_ptr)


   if (NULL != secondary_rx_lna_info_ptr)
   {
      // first reset our return val arg to start off on a clean slate
      rfconfig_rx_lna_reset(secondary_rx_lna_info_ptr);

      // DRx high linearity (1X/BC0)
      //   G0/G1: rise(-85.0dBm), fall(-86.2dBm), im(-85.0dBm), bpht(6.8ms), non-bpht(0ms)
      //   G1/G2: rise(-71.8dBm), fall(-73.0dBm), im(-71.8dBm), bpht(6.8ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -53, -56, -53, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -20, -23, -20, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 74, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 69, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         secondary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);

      // DRx high linearity (EVDO/BC0)
      //   G0/G1: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms)
      //   G1/G2: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[0], 10, 64, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[1], 10, 54, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 74, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 69, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         secondary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
   } //if (NULL != secondary_rx_lna_info_ptr)
}
#endif /* RF_HAS_BC0_NV_TABLE */


#if (defined(RF_HAS_BC1_NV_TABLE) || defined(RF_HAS_BC4_NV_TABLE)      \
     || defined(RF_HAS_BC6_NV_TABLE) || defined(RF_HAS_BC14_NV_TABLE)  \
     || defined(RF_HAS_BC15_NV_TABLE))
/*==========================================================================
FUNCTION   rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15  LOCAL FUNCTION

DESCRIPTION
   This function returns the default configuration for both primary and
   secondary chain Rx LNA switch points for all linearities and operational
   modes (1X and EVDO) supported on band classes 1, 4, 6, 14, and 15.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
static void rfconfig_qsc11x0_defaults_init_bc1_bc4_bc6_bc14_bc15(
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr)
{
   rfconfig_rx_lna_switch_pt_type sw_pt[4];

   if (NULL != primary_rx_lna_info_ptr)
   {
      // first reset our return val arg to start off on a clean slate
      rfconfig_rx_lna_reset(primary_rx_lna_info_ptr);

      // PRx/1X/BC1 high/low linearity share the same configuration
      //   G0/G1: rise(-115.0dBm), fall(-115.0dBm), im(-115.0dBm), bpht(6.8ms), non-bpht(0ms)
      //   G1/G2: rise(-77.8dBm), fall(-81.8dBm), im(-77.8dBm), bpht(6.8ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -128, -128, -128, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -35, -45, -35, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 50, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 50, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_LOW_POWER_MODE_1,
         4, // num_switch_pts,
         &sw_pt[0]);

      // PRx/EVDO/BC1 high/low linearity share the same configuration
      //   G0/G1: rise(-115.0dBm), fall(-115.0dBm), im(-115.0dBm), bpht(6.8ms), non-bpht(0ms)
      //   G1/G2: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms) 
      //   G2/G3: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms) 
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms) 
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -128, -128, -128, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[1], 10, 40, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 50, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 50, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_LOW_POWER_MODE_1,
         4, // num_switch_pts,
         &sw_pt[0]);
   } //if (NULL != primary_rx_lna_info_ptr)

   if (NULL != secondary_rx_lna_info_ptr)
   {
      // first reset our return val arg to start off on a clean slate
      rfconfig_rx_lna_reset(secondary_rx_lna_info_ptr);

      // DRx high linearity (1X/BC1)
      //   G0/G1: rise(-82.2dBm), fall(-86.2dBm), im(-82.2dBm), bpht(6.8ms), non-bpht(0ms)  
      //   G1/G2: rise(-69.0dBm), fall(-70.2dBm), im(-69.0dBm), bpht(6.8ms), non-bpht(0ms)  
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms) 
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -46, -56, -46, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -13, -16, -13, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 69, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 64, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         secondary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);

      // DRx/EVDO/BC1 high linearity
      //   G0/G1: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms) 
      //   G1/G2: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms) 
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[0], 10, 66, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[1], 10, 54, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 69, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 64, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         secondary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
   } //if (NULL != secondary_rx_lna_info_ptr)
}
#endif /* defined(RF_HAS_BC1_NV_TABLE) || defined(RF_HAS_BC4_NV_TABLE) ... */


#if (defined(RF_HAS_BC3_NV_TABLE) || defined(RF_HAS_BC10_NV_TABLE))
/*==========================================================================
FUNCTION   rfconfig_qsc11x0_defaults_init_bc3_bc10           LOCAL FUNCTION

DESCRIPTION
   This function returns the default configuration for both primary and
   secondary chain Rx LNA switch points for all linearities and operational
   modes (1X and EVDO) supported on band classes 3, and 10.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
static void rfconfig_qsc11x0_defaults_init_bc3_bc10(
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr)
{
   rfconfig_rx_lna_switch_pt_type sw_pt[4];

   if (NULL != primary_rx_lna_info_ptr)
   {
      // first reset our return val arg to start off on a clean slate
      rfconfig_rx_lna_reset(primary_rx_lna_info_ptr);

      // PRx high linearity (1X/BC3)
      //   G0/G1: rise(-88.2dBm), fall(-90.6dBm), im(-88.2dBm), bpht(6.8ms), non-bpht(0ms) 
      //   G1/G2: rise(-80.6dBm), fall(-81.8dBm), im(-80.6dBm), bpht(6.8ms), non-bpht(0ms) 
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -61, -67, -61, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -42, -45, -42, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 10, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
      // PRx low linearity (1X/BC3)
      //   G0/G1: rise(-88.2dBm), fall(-89.4dBm), im(-88.2dBm), bpht(6.8ms), non-bpht(0ms) 
      //   G1/G2: rise(-80.6dBm), fall(-81.8dBm), im(-80.6dBm), bpht(6.8ms), non-bpht(0ms) 
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms)
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms)
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -61, -64, -61, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -42, -45, -42, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 10, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_LOW_POWER_MODE_1,
         4, // num_switch_pts,
         &sw_pt[0]);

      // PRx/EVDO/BC3 high/low linearity share the same configuration
      //   G0/G1: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms) 
      //   G1/G2: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms) 
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[0], 10, 20, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[1], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 10, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
      rfconfig_rx_lna_set_switch_points(
         primary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_LOW_POWER_MODE_1,
         4, // num_switch_pts,
         &sw_pt[0]);

   } //if (NULL != primary_rx_lna_info_ptr)
   
   if (NULL != secondary_rx_lna_info_ptr)
   {
      // first reset our return val arg to start off on a clean slate
      rfconfig_rx_lna_reset(secondary_rx_lna_info_ptr);

      // DRx high linearity (1X/BC3)
      //   G0/G1: rise(-85.0dBm), fall(-86.2dBm), im(-85.0dBm), bpht(6.8ms), non-bpht(0ms)
      //   G1/G2: rise(-71.8dBm), fall(-73.0dBm), im(-71.8dBm), bpht(6.8ms), non-bpht(0ms)
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms) 
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[0], -53, -56, -53, 262, 0);
      rfconfig_rx_lna_switch_pt_set_fixed_config(&sw_pt[1], -20, -23, -20, 262, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 10, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         secondary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);

      // DRx high linearity (EVDO/BC3)
      //   G0/G1: hysteresis=4dB, backoff=20dB?, bpht(1ms), non-bpht(0ms)
      //   G1/G2: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G2/G3: hysteresis=4dB, backoff=18dB?, bpht(1ms), non-bpht(0ms) 
      //   G3/G4: hysteresis=4dB, backoff=16dB?, bpht(1ms), non-bpht(0ms) 
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[0], 10, 20, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[1], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[2], 10, 15, 39, 0);
      rfconfig_rx_lna_switch_pt_set_olta_config(&sw_pt[3], 10, 10, 39, 0);
      rfconfig_rx_lna_set_switch_points(
         secondary_rx_lna_info_ptr,
         RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
         RFLIB_NORMAL_POWER_MODE,
         4, // num_switch_pts,
         &sw_pt[0]);
   } //if (NULL != secondary_rx_lna_info_ptr)
}
#endif /* defined(RF_HAS_BC3_NV_TABLE) || defined(RF_HAS_BC10_NV_TABLE) */


#endif /* RF_HAS_RFCONFIG_RX_LNA */


