#ifndef TCXOMGR_XO_TRIM_H
#define TCXOMGR_XO_TRIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                T C X O    M A N A G E R    X O    T R I M
                     TCXO Manager XO Trim Header File

GENERAL DESCRIPTION

  This module .

EXTERNALIZED FUNCTIONS



REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS



Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/inc/tcxomgr_xo_trim.h#2 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/16/07   sst     Move XO Trim NV functions to tcxomgr_data.c
07/26/07   sst     Added function tcxomgr_xo_trim_switch_by_one
07/09/07   sst     Added function update_temp_table_after_xo_trim_switch
03/07/07   sst     Initial Version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"

#include "tcxomgr.h"


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

#define TCXOMGR_XO_TRIM_INVALID ( (uint8)(~0) )


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_RES

DESCRIPTION
  This function returns the XO Trim/Capacitor/Coarse Rotator calculated
  resolution in ppm between two xo trim settings

DEPENDENCIES
  None

RETURN VALUE
  XO Trim/Capacitor/Coarse Rotator calculated resolution

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_ppm_type tcxomgr_xo_trim_get_res
(
  /* xo_trim value to start calculation from (i.e. current) */
  uint8 start_val,
  /* xo_trim value to end on (i.e. destination) */
  uint8 end_val
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_TO_SWITCH

DESCRIPTION
  This function returns the XO Trim/Capacitor/Coarse Rotator value to correct
  for a given error

DEPENDENCIES
  None

RETURN VALUE
  XO trim value to program

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_get_to_switch
(
  /* xo_trim value to start calculation from (i.e. current) */
  uint8 start_val,
  /* error to transfer to xo_trim */
  tcxomgr_ppm_type ppm_err,
  /* error that will be left over if a transfer takes place */
  tcxomgr_ppm_type *residual_err
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_TABLE_VAL

DESCRIPTION
  This function returns XO Trim/Capacitor/Coarse Rotator value that
  corresponds to the current Freq/Temp Table

DEPENDENCIES
  None

RETURN VALUE
  The XO Trim/Capacitor/Coarse Rotator value that corresponds to the
  current Freq/Temp Table

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_get_table_val( void );

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_VAL

DESCRIPTION
  This function returns the current XO Trim/Capacitor/Coarse Rotator value
  as stored in the tcxomgr

DEPENDENCIES
  None

RETURN VALUE
  the current XO Trim/Capacitor/Coarse Rotator value as stored in the tcxomgr

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_get_val( uint8 *hw_val );

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_SWITCHED

DESCRIPTION
  This function gets a flag indicating that the XO Trim/Capacitor/Coarse
  Rotator has been modified

DEPENDENCIES
  None

RETURN VALUE
  a flag indicateing that the XO Trim/Capacitor/Coarse Rotator has been
  modified

SIDE EFFECTS
  None

============================================================================*/

extern boolean tcxomgr_xo_trim_get_switched( void );

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SET_VAL

DESCRIPTION
  This function sets the new value at which the XO Trim/Capacitor/Coarse
  Rotator has been changed to.

DEPENDENCIES
  None

RETURN VALUE
  The new xo_trim value
  TCXOMGR_XO_TRIM_INVALID - ndicates that xo_trim switching is not allowed

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_set_val
(
  uint8 new_val
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SWITCH

DESCRIPTION
  Something

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

============================================================================*/

extern boolean tcxomgr_xo_trim_switch
(
  tcxomgr_ppm_type      err_to_switch,           /* error in ppm to switch  */
  tcxomgr_ppm_type      threshold,               /* max error to txfr in ppm*/
  tcxomgr_ppm_type     *err_txfred,              /* ppm of error txfr'ed    */
  tcxomgr_ppm_type     *residual_err             /* residual error in ppm   */
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SWITCH_BY_ONE

DESCRIPTION
  This function determines if the XO Trim need to be switched to account
  for the err passed in. If the XO Trim needs to be switched, this
  function will do so.

DEPENDENCIES
  None

RETURN VALUE
  PPM error change pulled in by the XO Trim switch

SIDE EFFECTS
  XO Trim could be switched

============================================================================*/

extern tcxomgr_ppm_type tcxomgr_xo_trim_switch_by_one
(
  int8                  to_switch                /* +/- xo_trim to switch   */
);

/*============================================================================

FUNCTION TCXOMGR_UPDATE_TEMP_TABLE_AFTER_XO_TRIM_SWITCH

DESCRIPTION
  This function will re-calculate the Freq/Temp table, based upon the number
  of Caps switched and a calculated xo_trim resolution
  Currently it just clears the table to be the default value.

DEPENDENCIES
  This function should only be called if (tcxomgr.xo_trim.switched == TRUE)
  and a new value is about to be stored into the Freq/Temp Table

RETURN VALUE
  None

SIDE EFFECTS
  F/T (vco) table is cleared

============================================================================*/

extern void tcxomgr_update_temp_table_after_xo_trim_switch
(
  uint8 temp,
  tcxomgr_ppm_type old_val,
  tcxomgr_ppm_type new_val
);

#endif /* !TCXOMGR_XO_TRIM_H */
