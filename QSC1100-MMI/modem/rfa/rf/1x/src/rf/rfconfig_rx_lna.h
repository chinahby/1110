#ifndef RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_H
#define RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_H

/*===========================================================================
        R F C O N F I G   D E F I N I T I O N   H E A D E R   F I L E
                  F O R   R x   L N A   S T R U C T U R E

DESCRIPTION
  This file lists the interface available for manipulating the configuration
  items for an LNA on a single receive chain.

  CAUTION:
  Do NOT access the individual LNA config struct fields directly; 
  rather use only the function interfaces provided to ensure the integrity
  of the data contained within.

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfconfig_rx_lna.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/07   Vish    Added rfconfig_rx_lna_[set/get]_switch_point_info().
03/14/07   Vish    Created initial version.
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "rficap.h"   // for RF_HAS_RFCONFIG_RX_LNA
#include "rfconfig_rx_lna_switch_pt.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */
#define RFCONFIG_RX_LNA_MAX_GAIN_STEPS  (4)
#define RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES (3)

// Use this to static initialize any Rx LNA config structure (rfconfig_rx_lna_type) to reset state at load time;
//  similar effect as using rfconfig_rx_lna_reset() at run-time
#define RFCONFIG_RX_LNA_LINEARITY_INFO_RESET_VAL              \
   {                                                          \
      4,                                                      \
      {                                                       \
         RFCONFIG_RX_LNA_SWITCH_PT_RESET_VAL_INITIALIZER,     \
         RFCONFIG_RX_LNA_SWITCH_PT_RESET_VAL_INITIALIZER,     \
         RFCONFIG_RX_LNA_SWITCH_PT_RESET_VAL_INITIALIZER,     \
         RFCONFIG_RX_LNA_SWITCH_PT_RESET_VAL_INITIALIZER      \
      }                                                       \
   }
#define RFCONFIG_RX_LNA_MODE_INFO_RESET_VAL                   \
   {                                                          \
      FALSE,                                                  \
      {                                                       \
         RFCONFIG_RX_LNA_LINEARITY_INFO_RESET_VAL,            \
         RFCONFIG_RX_LNA_LINEARITY_INFO_RESET_VAL,            \
         RFCONFIG_RX_LNA_LINEARITY_INFO_RESET_VAL             \
      }                                                       \
   }
#define RFCONFIG_RX_LNA_RESET_VAL                             \
   {                                                          \
      {                                                       \
         RFCONFIG_RX_LNA_MODE_INFO_RESET_VAL,                 \
         RFCONFIG_RX_LNA_MODE_INFO_RESET_VAL                  \
      }                                                       \
   }


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
// always add new modes to the end of this enum just before
//  RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES entry
typedef enum
{
   RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X,
   RFCONFIG_RX_LNA_OPERATIONAL_MODE__EVDO,
   RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES,
   RFCONFIG_RX_LNA_OPERATIONAL_MODE__NONE = RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES
} rfconfig_rx_lna_operational_mode_type;


typedef struct
{
   uint32                          num_gain_steps;
   rfconfig_rx_lna_switch_pt_type  switch_pt[RFCONFIG_RX_LNA_MAX_GAIN_STEPS];
} rfconfig_rx_lna_linearity_info_type;

typedef struct
{
   boolean                              mode_configured;
   rfconfig_rx_lna_linearity_info_type  linearity_info[RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES];
} rfconfig_rx_lna_mode_info_type;

typedef struct
{
   rfconfig_rx_lna_mode_info_type       mode_info[RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES];
} rfconfig_rx_lna_type;


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

/*==========================================================================
FUNCTION   rfconfig_rx_lna_reset                          EXTERNAL FUNCTION

DESCRIPTION
   This function clears out the database for a *particular* Rx LNA. Here 
   "particular" refers to an LNA on a single RF chain only and it holds
   info about LNA switch points for all linearities (meaningful to that
   LNA) across various operational modes (like 1X or EVDO). Specifically
   it doesn't hold info for that LNA across multiple band classes, so
   multiple database have to be created for the same LNA h/w on a per band
   class basis.

DEPENDENCIES
   Use this function to start the database in a guaranteed cleared out
   state before operating on it using other APIs in this module:
    - rfconfig_rx_lna_set_switch_points()
    - rfconfig_rx_lna_get_linearity_info()
    - rfconfig_rx_lna_get_switch_point_info_from_linearity()

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
extern void rfconfig_rx_lna_reset(
   rfconfig_rx_lna_type *lna_config_ptr);


/*==========================================================================
FUNCTION   rfconfig_rx_lna_set_switch_points             EXTERNAL FUNCTION

DESCRIPTION
   This function programs the Rx LNA database with info about one or more
   Rx LNA switch points for a given linearity (normal or one of low power
   modes) while the LNA operates in a particualr operational mode (like 1X
   or EVDO).

DEPENDENCIES
   rfconfig_rx_lna_reset() should have been called prior to the *first* instance
   of the invocation of this API for a *particular* Rx LNA. That way, operations
   are performed for the first time on a properly guaranteed empty database.
   Note: Do not call rfconfig_rx_lna_reset() API before invoking this API unless
         you want LNA database to be cleared out completely before this API
         takes effect. This will be the case when this API is used multiple
         times for incrementally building the database for a particular Rx LNA.
         
RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
extern void rfconfig_rx_lna_set_switch_points(
   rfconfig_rx_lna_type                  *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type  mode,
   uint32                                 linearity_index,
   uint32                                 num_switch_pts,
   const rfconfig_rx_lna_switch_pt_type  *switch_pt_array);


/*==========================================================================
FUNCTION   rfconfig_rx_lna_get_linearity_info             EXTERNAL FUNCTION

DESCRIPTION
   This function returns info about LNA switch points that have been
   programmed into this database for the LNA on the specified RF chain 
   (primary or secondary), operational mode (like 1X or EVDO), and the 
   linearity (i.e normal or one of the low power modes).

DEPENDENCIES
   - At least rfconfig_rx_lna_reset() should have been called prior to this
     invocation to initialize the Rx LNA database to a clean void state that
     would cause this API to return a NULL value correctly (to indicate that
     the database doesn't have any programmed LNA switch points yet).
   - Typically though for this API to return a valid NON-NULL value, the
     database should have been properly initialized with switch point data
     using rfconfig_rx_lna_set_switch_points() calls.

RETURN VALUE
   - Returns info about available LNA switch points for a given mode (like 1X
     or EVDO) and the specified linearity.
   Type: const rfconfig_rx_lna_linearity_info_type*
   - Possible values:
     - non-NULL  --->  At least one LNA switch point data is available
     - NULL      --->  LNA switch point data isn't available

SIDE EFFECTS
   None.
===========================================================================*/
extern const rfconfig_rx_lna_linearity_info_type* rfconfig_rx_lna_get_linearity_info(
   const rfconfig_rx_lna_type                   *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode,
   uint32                                        linearity_index);


/*==========================================================================
FUNCTION   rfconfig_rx_lna_get_switch_point_info_from_linearity  EXTERNAL FUNCTION

DESCRIPTION
   This function returns info about a single Rx LNA switch point belonging to
   the specified linearity (info about which would have been retrieved earlier
   using rfconfig_rx_lna_get_linearity_info API) and LNA gain step.

DEPENDENCIES
   - At least rfconfig_rx_lna_reset() should have been called prior to this
     invocation to initialize the Rx LNA database to a clean void state that
     would cause this API to return a NULL value correctly (to indicate that
     the database doesn't have any programmed LNA switch points yet).
   - Typically though for this API to return a valid NON-NULL value, the
     database should have been properly initialized with switch point data
     using rfconfig_rx_lna_set_switch_points() calls.
   - Prior to invoking this API for getting LNA switch point data, info
     about the desired linearity must be retrieved first through
     rfconfig_rx_lna_get_linearity_info() API.

RETURN VALUE
   - Returns info about a single Rx LNA switch point.
   Type: const rfconfig_rx_lna_switch_pt_type*
   - Possible values:
     - non-NULL  --->  Valid LNA switch point data is available
     - NULL      --->  LNA switch point data isn't available

SIDE EFFECTS
   None.
===========================================================================*/
extern const rfconfig_rx_lna_switch_pt_type* rfconfig_rx_lna_get_switch_point_info_from_linearity(
   const rfconfig_rx_lna_linearity_info_type     *linearity_info_ptr,
   uint32                                         gain_step_index);


extern void rfconfig_rx_lna_set_switch_point_info(
   rfconfig_rx_lna_type                  *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type  mode,
   uint32                                 linearity_index,
   uint32                                 gain_step_index,
   const rfconfig_rx_lna_switch_pt_type  *switch_pt_ptr);


extern const rfconfig_rx_lna_switch_pt_type* rfconfig_rx_lna_get_switch_point_info(
   const rfconfig_rx_lna_type            *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type  mode,
   uint32                                 linearity_index,
   uint32                                 gain_step_index);

#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_H */


