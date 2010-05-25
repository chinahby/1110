#ifndef RFCONFIG_QSC11X0_DEFAULTS_H
#define RFCONFIG_QSC11X0_DEFAULTS_H

/*===========================================================================
                   R F C O N F I G   H E A D E R   F I L E 
                       F O R   Q S C 1 1 X 0   C H I P

DESCRIPTION
  This file lists the interface available for accessing default configuration
  information for QSC11X0 chip. Currently this includes the following:
   - Rx LNA switch point info for both primary and secondary chains for all
     band classes

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfconfig_qsc11x0.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/07   ems     Ported from SC2X 3044
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "rfconfig_rx_lna.h"
#include "rfifreq.h"
#include "rficap.h"   // for RF_HAS_RFCONFIG_RX_LNA
#include "rfnv_cdma1x.h"
#include "rfnv_rfconfig_migration_tracker.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

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
extern boolean rfconfig_qsc11x0_init_default_rx_lna_info(
   rf_card_band_type         band_class,
   rfconfig_rx_lna_type     *primary_rx_lna_info_ptr,
   rfconfig_rx_lna_type     *secondary_rx_lna_info_ptr);


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
extern void rfconfig_qsc11x0_migrate_from_rfnv(
   rf_card_band_type                            band_class,
   const rfnv_digital_items_type               *rfnv_digital_items_ptr,
   boolean                                      use_1x_rfnv_items_to_init_evdo_sw_pts,
   rfconfig_rx_lna_type                        *prx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *prx_rfconfig_migration_tracker_ptr,
   rfconfig_rx_lna_type                        *drx_rfconfig_rx_lna_ptr,
   const rfnv_rfconfig_migration_tracker_type  *drx_rfconfig_migration_tracker_ptr);

#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFCONFIG_QSC11X0_DEFAULTS_H */



