/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        RF Linearizer Services

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS

  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rflin_mm_cdma1x.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/07/07   adk     Fixed compiler warning in rf_calc_new_tx_lin().
03/28/07   adk     rf_calc_new_tx_lin() is not needed if using FW TX AGC.
12/14/05   sar     Eliminated more lint errors resulting from new config file.
07/04/05   dyc     Lint Cleanup.
03/19/04   et      added interpolation (frequency comp) support
09/03/03   dbc     Use global RF variable rf_chain_status[] to access RF state
                   related variables
08/28/03   aks     Change variables to signed, which are used for TX linearizer
                   calculations.  Fixes issue from large positive values.
01/13/03   aks     Changes for GSM PLT.
08/29/02   dbc     Cleanup
03/18/01   dbc     Removed rx linearization
07/02/01   aks     Created from re-organization of rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
===================================================================================*/

#include "target.h"
#include "customer.h"

#ifndef FEATURE_GSM_PLT  

#include "comdef.h"
#include "db.h"
#include "msm.h"
#include "msm_drv.h"
#include "rfi.h"
#include "rf.h"



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                    SCALE FACTORS FOR FIXED POINT MATH

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* Scale factor for the CDMA transmit linearizer master curve slope
*/
#define RF_CDMA_TX_LIN_SLP_SCALE       32

/* RF_CDMA_TX_LIN_SLP_SCALE expressed as a shift.  scale == (1 << value)
*/
#define RF_CDMA_TX_LIN_SLP_SCALE_SHIFT 5

/* RF_CDMA_TX_LIN_SLP_SCALE expressed as a mask, for modulus operations.
** This can only be done if scale is a power of 2.  x % scale == x & mask
*/
#define RF_CDMA_TX_LIN_SLP_SCALE_MASK   (RF_CDMA_TX_LIN_SLP_SCALE-1)


/* -----------------------------------------------------------------------
** Linearizer constants
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------

                          SHARED CALIBRATION RELATED DATA

-------------------------------------------------------------------------- */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   CDMA mode Tx AGC linearization (MSM2P data is derived from these tables)
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int2  rf_cdma_tx_lin_vs_temp_now[RF_NUM_TX_LINEARIZERS];   
                                                 /* comp val for current temp */

int2  rf_cdma_tx_slp_vs_temp_now[RF_NUM_TX_LINEARIZERS];



/* -----------------------------------------------------------------------

                          CALIBRATION RELATED DATA

-------------------------------------------------------------------------- */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Tx Linearizer offset and slope arrays.  
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/* Tx linearizer offsets and slopes */
byte rf_tx_lin_off[ RF_NUM_TX_LINEARIZERS ][ RF_TX_LIN_HW_SEGMENTS ];  
byte rf_tx_lin_slp[ RF_NUM_TX_LINEARIZERS ][ RF_TX_LIN_HW_SEGMENTS ];  



/*===========================================================================

FUNCTION RF_TX_LIN_MASTER_CURVE_CALC                        INTERNAL FUNCTION

DESCRIPTION
  Calculates one point on the Tx linearizer master curve based on Tx
  linearizer curve value.

DEPENDENCIES
  rf_cdma_tx_lin_master_off[] and rf_cdma_tx_lin_master_slp[]
  must have been loaded from NV storage.

RETURN VALUE
  Returns a byte value representing one Tx linearizer offset point.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL word rf_tx_lin_master_curve_calc
(
  int2 curve_x,       /* curve x value, curve_x must be >=0 */
  int2 *table_ptr     /*lint -e818 table_ptr could be declared as const */
)
{
  word segment;       /* segment (index) for Tx lin offset/slope tables */
  int2 master_x;      /* temp value for master_curve[x] calculations    */
  int2 slope;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ------------------------------
  ** calculate which segment to use
  ** ------------------------------ */
  segment = (word)((uint2)curve_x >> RF_CDMA_TX_LIN_SLP_SCALE_SHIFT );

  /* ------------------------------------------------------------------
  ** MASTER_CURVE[x] = MASTER_OFFSET[segment]
  **                 + ((MASTER_SLOPE[segment] * (x MOD 32)) + 16) / 32
  ** ------------------------------------------------------------------ */
  /* slope is calculated from adjacent offsets */
  slope = table_ptr[segment+1]-table_ptr[segment];

  master_x = curve_x & RF_CDMA_TX_LIN_SLP_SCALE_MASK;
  master_x *= slope;
  master_x += RF_CDMA_TX_LIN_SLP_SCALE/2; /* add 0.5xSCALER for round up */
  master_x = (uint2)master_x >> RF_CDMA_TX_LIN_SLP_SCALE_SHIFT;
  master_x += table_ptr[segment];
  return( (word)master_x );

} /* rf_tx_lin_master_curve_calc */


#ifndef RF_HAS_MDSP_TX_AGC

/*===========================================================================

FUNCTION RF_CALC_NEW_TX_LIN                                 INTERNAL FUNCTION

DESCRIPTION
  Calculate new Tx linearizer with frequency and temperature compensation and
  then update rf_tx_lin_off[] and rf_tx_lin_slp[].

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_tx_lin_off[] is updated.
  rf_tx_lin_slp[] is updated.

===========================================================================*/
void rf_calc_new_tx_lin(void)
{
  int2 segment;
  int2 temp_int2;
  int2 trim_val;
  int2 slp_trim;
  int2 offset;
  int2 next_offset;
  int  lin_tbl;

  byte *offp;
  byte *slpp;

  rf_chain_state_type *rf_chain_ptr = 
    &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  for (lin_tbl=0; lin_tbl<RF_NUM_TX_LINEARIZERS; lin_tbl++) {

    offp = rf_tx_lin_off[lin_tbl];
    slpp = rf_tx_lin_slp[lin_tbl];

    /* ---------------------------------------------------------------
    ** If "new" PA driver, also frequency compensate the Tx linearizer
    ** --------------------------------------------------------------- */

    trim_val =
#ifdef RF_HAS_LINEAR_INTERPOLATION
      rf_freq_comp.i_lin_freq_comp[lin_tbl];
#else
      (int2)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_comp_vs_freq[lin_tbl][rf_chain_ptr->rf_freq_index];

#endif
    /* ----------------------------------------------------------------------
    ** Calculate segments of the master curve and use it to calculate
    ** the Tx linearizer offset & slope.  Load the resulting Tx linearizer
    ** offset and slope values into the MSM2P.
    **
    ** Calculate the offset for the 0th segment, and then loop over the
    ** segments calculating the I+1 segment's value.  Each linearizer segment
    ** is an offset and a slope.  The slope is the difference between the
    ** current segments offset and the successive segment's offset.
    ** ------------------------------------------------------------------- */

    /* Offset the trim value by the size of 1 segment
    */
    trim_val += RF_TX_LIN_SEGMENT_SIZ;

    /* Initialize the linearizer value to the frequency compensation
    ** and limit the linearizer to the maximum displacement
    */
    temp_int2 = RF_SATURATE( trim_val,
                             RF_TX_MASTER_X_MIN,
                             RF_TX_MASTER_X_MAX );

    /* Calculate the offset using the master curve
    */
    offset = (int2)rf_tx_lin_master_curve_calc( temp_int2,
               rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_master_off[ lin_tbl ] );

    /* Adjust the offset for temperature
    */
    offset += rf_cdma_tx_lin_vs_temp_now[lin_tbl];

    /* Limit the range of the offset
    */
    offset = RF_SATURATE( offset, RF_TX_LIN_OFF_MIN, RF_TX_LIN_OFF_MAX );

    /* Calculate the initial slope trim based on temperature.
    ** Use RF_LIN_HW_SEGMENTS/2 to round instead of truncate,
    ** this gains 1/6 of a dB in accuracy.
    */

    slp_trim = (RF_TX_LIN_HW_SEGMENTS / 2);

    for (segment=0; segment < RF_TX_LIN_HW_SEGMENTS; ) {

      /* Store the 8 MSBs of the 9-bit offset for this segment
      */
      *offp++ = WORD_LO( (uint2)offset >> 1 );

      /* Offset the trim value by the size of 1 segment
      */
      trim_val += RF_TX_LIN_SEGMENT_SIZ;

      /* Initialize the linearizer value to the temperature compensation,
      ** and limit the linearizer to the maximum displacement.
      */
      temp_int2 = RF_SATURATE( trim_val,
                               RF_TX_MASTER_X_MIN,
                               RF_TX_MASTER_X_MAX );

      /* Calculate the displacement using the master curve
      */
      next_offset = (int2)rf_tx_lin_master_curve_calc( temp_int2,
                      rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_master_off[ lin_tbl ] );

      /* Move to the next segment
      */
      segment++;

      /* Step slop trim forward by 1 segment
      */
      slp_trim += rf_cdma_tx_slp_vs_temp_now[lin_tbl];

      /* Adjust the offset for temperature and slope vs temperature.
      */
      next_offset += rf_cdma_tx_lin_vs_temp_now[lin_tbl]
                     + slp_trim / RF_TX_LIN_HW_SEGMENTS;

      /* Prevent negative slope and limit the offset range
      */
      next_offset = RF_SATURATE( next_offset, offset, RF_TX_LIN_OFF_MAX );

      /* Store the LSB of the offset and the 7 bit slope for this
      ** segment, offset(n+1) - offset(n).
      */
      *slpp++ = ((offset & 1)? 0x80 : 0x00) | WORD_LO( next_offset - offset );

      /* The next offset now becomes the current offset
      */
      offset = next_offset;
    }
  }   /* for (lin_tbl=0;.....) */
} /* rf_calc_new_tx_lin */


#endif /* RF_HAS_MDSP_TX_AGC */

#endif /* !FEATURE_GSM_PLT */

