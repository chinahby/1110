#ifndef SRCH_UTIL_H
#define SRCH_UTIL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    U T I L I T Y    H E A D E R   F I L E



GENERAL DESCRIPTION
   This file is part of the breakup of the former file srchdrv.c. This file
   consists of all functions that don't fit into any of the other new
   searcher files (srch_hw.c, srch_8xhw.c, srch_16xhw.c, srch_Xxhw.c,
   srch_fing.c, srch_comb.c, srch_util.c, srch_PN.c).

  Copyright (c) 2001-2009
                by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srch_util.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/09/09   bb      Define the srch_util_gen_epoch() method
02/23/07   sst     Create position comparison macros
12/15/06   tjc     Modified traffic to use the sector module
09/20/06   pa      Export srch_util_rf_reset().
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
09/23/05   ejv     Delete srch_util_config_msm.
10/28/04   kwo     Exported srch_util_win_thresh_lecio()
09/03/04   kwo     Added srch_util_calc_lecio()
07/26/04   ejv     Remove SRCH_MSM_NO_VERSION.
07/23/04   ejv     Remove srch_cdma_search_active and srch_util_gen_epoch.
01/28/04   ejv     Mainline T_MSM6500.
01/16/04   ejv     Remove FEATURE_SRCH_SCHEDULER.
07/30/02   gs      Initial merge into perforce for cougar.
05/30/02   ejv     Mainlined T_MSM5000.
02/27/02   ejv     Modified date format in change history comments.
01/14/02   SST     Merged into tip to replace srchdrv.c
01/11/02   SST     Completed function name changes.
                        srch_config_msm -> srch_util_config_msm
                        srch_dmod_reset -> srch_util_dmod_reset
                        srch_get_dmod_id -> srch_util_get_dmod_id
                        srch_bypass_interface -> srch_util_bypass_interface
                        srch_demod_enable_bypass -> srch_util_dmod_ena_bypass
                        srch_demod_disable_bypass -> srch_util_dmod_dis_bypass
                        srch_generate_epoch -> srch_util_gen_epoch
                        srch_hw_err_recovery -> srch_util_hw_err_recovery
01/10/02   SST     finished move for srchdrv.c breakup
01/01/02   SST     Created Module - refer to srch_hw.c for earlier edit
                      history.

==========================================================================*/


/*-------------------------------------------------------------------------
  Macros
---------------------------------------------------------------------------*/

/* macro to check if a postion (pos) is within a give range defined by
   the min and max of the range (all values shall be in ChipX8) */
#define POS_IN_RANGE( pos, min, max )                                         \
  ( ( ( ( (pos) - (min) ) & PN_CIRCLE_MASK ) <=                               \
      ( ( (max) - (min) ) & PN_CIRCLE_MASK )    ) ? TRUE : FALSE )

/* macro to compare positions "a" and "b".
   note: a<b if "a" is between "b - half the pn circle" and "b"
         a>b if "a" is between "b" and "b + half the pn circle"
   (all values shall be in ChipX8) */
#define  HALF_PN_CIRCLE    (SHORT_CODE_HALF_LEN * (int4) CHIPX8_PER_CHIP)
#define POS_COMPARE_LT( a, b )                                                \
  ( POS_IN_RANGE( (a), (b) - HALF_PN_CIRCLE-1, (b) ) )
#define POS_COMPARE_GT( a, b )                                                \
  ( POS_IN_RANGE( (a), (b), (b) + HALF_PN_CIRCLE-1 ) )


/*-------------------------------------------------------------------------
  Function Prototypes
---------------------------------------------------------------------------*/

/*=============================================================================

FUNCTION       SRCH_UTIL_WIN_THRESH_LECIO
DESCRIPTION    This function looks up the energy to threshold peaks as a
               function of window size.

DEPENDENCIES   None

RETURN VALUE   Threshold in linear ecio

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_util_win_thresh_lecio
(
  uint32 win_size /* window size in cx8 */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_LECIO
DESCRIPTION    This function converts the peak energies to linear ecio and
               does the total linero ecio calc with filtering.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_util_calc_lecio
(
  srch_sect_result_type *results, /* sector to calc linear ecio */
  srch_sect_parm_type   *parms    /* sector to calc linear ecio */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_INX_FROM_POS
DESCRIPTION    This function takes sector structure and index and computes the
               peak inx value.

DEPENDENCIES   None

RETURN VALUE   Peak index from search results.

SIDE EFFECTS   None

=============================================================================*/
extern int32 srch_util_calc_inx_from_pos
(
  srch_sect_struct_type *sect,
  int32                  pos_index
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_SRCH_FILTER
DESCRIPTION    This function grows the IIR Filter for the search results.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_util_calc_srch_filter
(
  srch_sect_struct_type               *sector
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_SRCH_OFFSET
DESCRIPTION    This function determines the offset needed for neighbor searches
               as specified in OTA Neighbor List Messages and CFS Requests.

DEPENDENCIES   None

RETURN VALUE   Calculated search offset

SIDE EFFECTS   None

=============================================================================*/
extern int32 srch_util_calc_srch_offset
(
  srch_sect_struct_type *sector
);

/*===========================================================================

FUNCTION       SRCH_UTIL_PILOT_TRPT
DESCRIPTION    This function checks the pilots list in a trpt struct
               for a given pilot.  If the pilot is present, it returns its index
               in the list.  If the pilot is not present, it is added and
               NO_PILOT is returned.

               This is used to keep track of what has been reported to the base
               station.  For each sector, fake pilot value PILOT_SELF is put on
               this list to mark the sector itself as having been reported to
               the base.  Where this sector is being compared to other sectors
               for certain conditions, a report of this sector relative to
               another sector is marked by putting that other sector's pilot on
               this list.

DEPENDENCIES   None.

RETURN VALUE   The index in the trpt.pilots list if there is a match, or
               NO_PILOT if not.

SIDE EFFECTS   The pilot may be added to the trpt.pilots list.

===========================================================================*/
extern uint16 srch_util_pilot_trpt
(
  srch_trpt_struct_type  *trpt_ptr,
  int16  pilot
);

/*===========================================================================

FUNCTION SRCH_UTIL_RF_RESET

DESCRIPTION    This function does the RF initialization that used to be
               in the main reset function.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Various demod chip registers.

===========================================================================*/

extern void srch_util_rf_reset( void );

/*===========================================================================

FUNCTION       SRCH_UTIL_GEN_EPOCH

DESCRIPTION    This function generates an EPOCH, which establishes an
               arbitrary zero-offset time reference.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   PN generates, fingers, searcher, PDM's etc initialized.

===========================================================================*/
extern void srch_util_gen_epoch ( void );
#endif /* SRCH_UTIL_H */
