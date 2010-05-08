#ifndef SRCH_HW_H
#define SRCH_HW_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    H A R D W A R E 
            
            H E A D E R F I L E 
            
GENERAL DESCRIPTION
   This file is part of the breakup of the former file srchdrv.c. This file
   consists of the functions that are involved with programming and retrieving
   searches.

  Copyright (c) 2001 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_hw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
05/31/06   awj     Further sect and srch4 updates
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
09/08/05   ejv     Add prototypes for functions in srch_hw.c.
05/20/05   ejv     Added srch_range_slew and constants from srch_pn.h.
05/12/05   ejv     Remove unused externs.
12/15/04   kwo     Rework of DH interface
09/08/04   jcm     Rename searcher1 functions srch1
09/08/04   jcm     Moved srch_hw types to srch_hw_t.h
09/03/04   kwo     Added define for dynamic lecio threshold
09/03/04   ejv     Add cpcch_info.
09/01/04   jcm     Moved integration parameter conversion macros to srch_hw.h
08/27/04   ejv     Mainline more features.
08/24/04   ejv     Removed srch_win_struct_type.
08/13/04   va      Changed featurization of SRCH4_AFLT_ENABLED to allow
                   compilation with FEATURE_GPSONE off.
07/22/04   kwo     Added Linear ecio fields to the sector structure
07/19/04   ejv     Mainline FEATURE_IS95B_MAHHO, _ACCESS_HO, and PILOT_RPT.
07/14/04   jcm     Changed last time sector searched OTA to ms resolution
05/28/04   jcm     Sector module integration
04/21/04   sfm     Changes for traffic channel manager
03/09/04   ejv     Removed FEATURE_MSM5105_PPM_SRCH_ENGINE.
02/24/04   ejv     Still more FEATURE_SEARCH1.
02/20/04   ejv     Added more FEATURE_SEARCH1 checks.
02/11/04   ejv     Mainline FEATURE_IS95B.
02/11/04   ejv     Changed FEATURE_IS2000C_DV to FEATURE_IS2000_REL_C_DV.
01/21/04   sfm     Added dv_pilot_info to srch_sect_struct_type
01/21/04   kwo     Cleaned up srch_prio_status_type enum
01/19/04   ejv     Remove unused TD-related drivers that used both 8x
                   and 16x searchers, since 16x does not exist.
01/16/04   ejv     Remove FEATURE_SEARCH2 and FEATURE_SRCH_SCHEDULER.
                   Changed SRCH2_SW_Q_LEN to SRCH_SW_Q_LEN.
01/12/04   ejv     Added FEATURE_SEARCH1.
08/11/03   va      Added PPM Ptr to srch_sect_cmd, and moved PPM related
                   definitions from srchi.h
07/28/03   ejv     Mainline FEATURE_ZZ2.
07/24/03   ejv     Add walsh_len field to sch_info in srch_sect_struct_type.
07/14/03   ejv     Remove FEATURE_SPECIAL_MDR.
06/26/03   ejv     Remove FEATURE_DYNAMIC_THROTTLE.
06/09/03   ejv     Remove nop function srch_set_agc_cdma().
03/25/03   ejv     Added function srch_stop_1x_searches().
02/18/03   va      SRCH4 AFLT support.
02/11/03   kwo     Mainlined structure srch_dcch_info
01/28/03   sst     Encapsulated srch_win_center to srch_hw.c/h
                   Added srch_(set/get/slew)_win_center()
01/07/03   sst      Removed freq_incl from sect struct, All pilots now have
                     freq and band class info.
12/30/02   ejv     Removed FEATURE_MULTI_SRCH_SCHEDULER.
12/05/02   sst     Added search_path_struct_type to contain common and
                   diversity path values in sector struct
11/25/02   gs      Change win_siz to int4.  Needs to be int to account for
                   FULL_CIRCLE define in srchi.h, and needs to cover at least
                   1/4 circle in chipx8 for searcher 4.
11/25/02   ejv     Support for TD on NCC.
10/21/02   rm      Changes win_siz and maxinx to an unsigned ineteger in 
                   sector structue.
10/04/02   sst     Added support for 4 & 6 searcher peaks
                   Added definitions for SRCH_MAX_PEAKS, SRCH_MIN_PEAKS,
                   SRCH_NUM_PEAKS
10/02/02   gs      Merge with MSM_SRCH.08.06.42
07/30/02   gs      Initial merge into perforce for cougar.
07/16/02   ejv     Added FEATURE_SEC_CALLS_SEARCHER_RANDOM.
06/28/02   bt      Changed max_pos from a single value to an array.
05/21/02   ejv     Mainlined FEATURE_IS2000.
04/22/02   ejv     Added extern from srch.h.
04/08/02   br      Removed unused srch_sect_cmd_struct_type fields. Cleanup.
03/21/02   br      Exchanged FEATURE_GPSONE with
                   FEATURE_MSM5105_PPM_SRCH_ENGINE.
02/13/02   abh     Added srch_current_8x_req and srch_current_16x_req
                   extern declaration to fix compile problems.
02/27/02   ejv     Featurization around "bcch_supported" is changed.
02/27/02   ejv     Modified date format in change history comments.
02/22/02   ejv     FEATURE_IS2000_REL_A_NCC -> FEATURE_IS2000_REL_A_CC.
02/21/02   ejv     Added FEATURE_IS2000_REL_A_NCC.
02/14/02   ejv     Added missing FEATURE_DH prototypes.  Added missing
                     parameters in functions srch_req_done_mgmt and
                     srch_set_status_dump.
01/14/02   SST     Merged into tip to replace srchdrv.c
01/11/02   SST     Completed function name changes.
                        srch_schedule_new_search_req -> srch_req_schedule_new
                        srch_prog_new_search_req -> srch_req_prog_new
                        srch_clear_scheduled_srch_req -> 
                                                    srch_req_clear_scheduled
                        srch_err_clear_requests -> srch_req_err_clear
                        srch_done_srch_req_management -> srch_req_done_mgmt
                        srch_ppm_free -> srch_req_ppm_free
                        srch_get_aborted_ppm_srch_result -> 
                                            srch_req_get_aborted_ppm_result
                        srch_stop_current_ppm_srch -> 
                                                  srch_req_stop_cur_ppm_srch
01/10/02   SST     finished move for srchdrv.c breakup
01/01/02   SST     Created Module - refer to srch_hw.c for earlier edit
                      history.

==========================================================================*/
#include "clk.h"
#include "comdef.h"
#include "queue.h"
#include "srch.h"
#include "srch_genlog.h"
#include "srch_hw_t.h"

#ifdef FEATURE_IS2000_REL_A
#include "cai.h"
#include "caii.h"
#endif

#ifdef FEATURE_SEC
#ifndef FEATURE_SEC_CALLS_SEARCHER_RANDOM
#include "secutil.h"
#endif /* FEATURE_SEC_CALLS_SEARCHER_RANDOM */
#endif /* FEATURE_SEC */


/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#define  SHORT_CODE_LEN         32768L
#define  SHORT_CODE_HALF_LEN    16384L
#define  SHORT_CODE_QUARTER_LEN  8192L
  /* The length in chips of the short code PN sequence (and
     half and quarter lengths) */

#define  PN_CIRCLE_MASK    0x3FFFF
  /* Mask for normalizing chipx8 values */

/*-------------------------------------------------------------------------
      Prototypes 
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION SRCH_RANGE_SLEW

DESCRIPTION    This function makes sure a particular slew amount is
      within the allowable + - 32768-chip range.

DEPENDENCIES   None.

RETURN VALUE   The range-justified slew.

SIDE EFFECTS   None.

===========================================================================*/
extern int4 srch_range_slew
(
  int32 slew
    /* The slew to keep within good range */
);



/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X                                                                  X
  X     Temporary placement of security and rf functions...          X
  X     hopefully to be moved soon to their own files/subsystems     X
  X                                                                  X
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/       

#ifdef FEATURE_SEC
/*===========================================================================

FUNCTION SRCH_SEC_FILL_RND

DESCRIPTION     This function collects random data and merges it into the
      random data buffer storage area.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

void srch_sec_fill_rnd( void );

#endif /* FEATURE_SEC */

#ifdef FEATURE_DH
/*===========================================================================

FUNCTION SRCH_START_RAND_BUF_FILL

DESCRIPTION    This function sets the appropriate flag to start filling the
      buffer with random bits, necessary for DH task.  It also grabs the
      start time of the process.

DEPENDENCIES   None.

RETURN VALUE   Good status.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_cmd_status_type srch_start_rand_buf_fill ( void );

/*===========================================================================

FUNCTION SRCH_PEAK_FILL_RAND_BUF 

DESCRIPTION     This function takes randomsearch peak data and loads it into a
      buffer for use by DH

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern void srch_peak_fill_rand_buf
(
  srch_sect_result_type *results,   /* pointer to a sector to take data from */
  uint32                 num_peaks  /* number of peaks to use */
);

#endif /* FEATURE_DH */

/*===========================================================================

FUNCTION SRCH_GET_TX_ADJ

DESCRIPTION    This function returns the current value of the tx gain
      adjust register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern uint8  srch_get_tx_adj ( void );

/*===========================================================================

FUNCTION SRCH_SET_TX_ADJ

DESCRIPTION    This function allows external tasks (namely the Transmit
  Task) to have access to the tx gain adjust register of the demod chip.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Changes the contents of the tx gain adjust register of the
               demod chip.

===========================================================================*/
extern void  srch_set_tx_adj
(
  uint8  tx_adj
    /* The 8-bit value to be written to the tx gain adjust register.
       Note that this is a signed value. For maximum power, use the
       value -128 (0x80); for minimum power, use +127 (0x7F). */
);


/*===========================================================================

FUNCTION SRCH_SET_TX_RATE_ADJ

DESCRIPTION    This function allows external tasks (namely the Transmit
  Task) to have access to the tx gain adjust register for the data rate.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Changes the contents of the tx rate adjust register of the
               demod chip.

===========================================================================*/
extern void  srch_set_tx_rate_adj
(
  int16  tx_adj
    /* The 9-bit value to be written to the tx rate adjust register. */
);

#ifndef FEATURE_SRCH_SINGLE_ANTENNA
/*===========================================================================

FUNCTION SRCH_GET_RX1_AGC

DESCRIPTION    This function returns the current value of the rx1 agc register.
      For targets using diversity receiver, this is simply a passthru for RF
      CAGC function calls.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern uint8  srch_get_rx1_agc ( void );
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

#endif /* SRCH_HW_H */

