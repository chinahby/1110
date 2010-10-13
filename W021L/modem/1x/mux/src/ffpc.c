/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  FAST FORWARD POWER CONTROL PROCESSING

GENERAL DESCRIPTION
  This module is responsible for low level processing of data received
  from the base station in CDMA mode of operation for Fast Forward
  Power Control.

Copyright (c) 1999 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/ffpc.c_v   1.41   07 Nov 2002 13:34:20   donb  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/ffpc.c#4 $ $DateTime: 2008/09/23 14:35:25 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/08   jtm     Added logic to ffpc_trans_cmd() to ignore channels with 
                   an RC of 1 or 2 and MCTRANS_FFPC_STOP actions when deciding 
                   if the RC is valid for ffpc.
08/11/08   jtm     Klockwork fixes. Fixed ffpc_trans_cmd() to properly handle 
                   RC 1 and 2 cases and notify searcher that FFPC is off.
07/16/08   jtm     Converted macro FFPC_CHAN_PARAMS into function ffpc_get_param_ptr()
                   to reduce ROM size for ULC effort. Also reduced debug string sizes.
10/11/06   tjc     Added ffpc_set_setpt_offset_dbq4()
10/05/06   bn      Added check for NULL pointer in the ffpc_log_frame_init function
09/19/06   vlc     Fixed lint errors.
09/11/06   vlc     Changed utility function ffpc_convert_dBq8_to_lin() to 
                   convert_dBq8_to_lin() as it will be used for RLGC as well.
09/08/05   ejv     Use new srch_mux interface calls.
07/25/05   rkc     Fixed lint errors.
07/18/05   bn      Added new interface function for MC to get FFPC_DEFAULT_FCH_INIT_SETPT_VAL
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
03/14/05   vlc     Fixed compiler warnings for MSM6700.
06/16/04   sr      Merged in changes from 6500 5004 build
06/08/04   ejv     Changed srch_set_fpc_mode to srch_notify_fpc_mode.
05/26/04   jrp/rdh Added ffpc_get_current_fch_setpoint().  Removed featurization
                   around ffpc_get_subchan_gain_db().  New parameter to 
                   srch_set_fpc_mode().
05/06/04   ll      Fixed setpoint adjustment problem after CFS.
04/28/04   jrp     Moved lint suppressions below includes.
04/20/04   jrp     Fixed lint warnings.
04/09/04   bn      Added new interface functions to get the SCH FFPC related info
12/02/03   jrp     Mainlined db-only maintained state from Jaguar orphans
                   branch #21.
12/01/03   ejv/bb  Updates for DCCH hw setpt.
12/01/03   vlc     Fix RVCT compiler warnings.
11/05/03   ejv     Added function ffpc_program_walsh_gain and corresponding
                   look-up table ffpc_iqacc_sch_scale_tab.
10/30/03   jrp     Fixed compiler warnings.
10/17/03   ejv/bb  Update ffpc_init_setpts.
10/07/03   ejv     Removed old implementation.
09/15/03   ejv/bb  Updates the lg2_to_lin table.
09/09/03   dna     Merged porting changes for MSM6500
08/21/03   ejv/bb  Added new FFPC implementation.
04/07/03   jrp     Lint changes.
11/15/02   jrp     Fixed logging defect with FPC Modes 4,5.
11/07/02   dlb     Increment idx for dcch frame record in FFPC Info log.
10/16/02   jrp     Changed ffpc_disable() to disable only in case of RC1 or RC2.
                   Other radio configurations allow FPC to be enabled so that
                   power control register values are updated by hardware that are
                   used in DTX detection.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
08/06/02   cr      Added MSM6100 changes.
08/14/02   jrp     Changed ffpc_update_setpt() to only write setpoint for primary
                   channel or SCH.
08/14/02   yll/vlc Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
08/01/02   jrp     Added featurization for FPC modes 4,5,6.  Changed FPC_MODE enums
                   to be more descriptive due to SVD.
07/11/02   bn      Added support new QIB algorithm and freeze the setpoint when not TX
06/18/02   jrp     Added support for FFPC Modes 4,5,6.
05/02/02   hrk     Fixed the case where FFPC HW was not unabled during a RC1->RC3
                   when in ECAM MS comes up on RC1 and during Service negotiation
                   RC is changed to RC3.
04/19/02   jrp     Added ffpc_get_setpt().
12/10/01   hrk     Renamed RXC_FULL_FRAME to RXC_GOOD_FRAME.
12/05/01   hrk     During HHO/CFS/GPS tuneaway period FFPC Setpts will not be
                   updated. After return/resume from HHO/CFS/GPS tuneaway checking
                   RC validity only if SCR is included.
11/27/01   hrk     MIN/MAX Setpt is set to CURRENT Setpt only for PCNM.
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
10/16/01   hrk     Fixed reporting of SCH FFPC logging data.
                   In an ESCAM if FPC_SCH_INIT_SETPT_OP=0 and if Init setpt
                   was > Max_setpt, the Init_setpt was not capped to Max_setpt.
10/02/01   bn      Fixed the FFPC logging data if FFPC_MODE = FPC_MODE_EIB
09/26/01   bgc     Added FEATURE_IS2000_SCH for ffpc_program_walsh_gain().
08/21/01   sr      Changed MSM_OUT to MSM_OUTH in ffpc_program_walsh_gain()
08/15/01   hrk     Corrected use of WALSH_GAIN register. Fixed compilation issues.
08/13/01   hrk     defined walsh gains for 32X.
06/15/01   day     Mainlined FEATURE_FFPC
06/07/01   bgc     Changed non-MSM5100 targets to use the new argument for
                   rxc_get_sch_info().
06/04/01   bgc     Added #ifdefs to turn off data services.
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
05/27/01   snn     Modified the rxc_get_sch_info to return SCH RC.
06/01/01   jq      Wrapped some code in FEATURE_IS95B_MAHHO.
05/18/01   hrk     Merged from rev 1.6.1.3. added #include <stdlib.h>
05/03/01   lcc     Changes for using new definition of rxc_returned_type.
04/23/01   sr      Changes to PLT to support new interface
04/20/01   hrk     SCH init setpt was not correctly initialized if MIN_SETPT in
                   ESCAM equals DEFAULT value for MIN_SETPT used by MS.
04/20/01   sr      Merged in from MSM5100 archive
04/17/01   vlc     Merged with MSM_MUX1X.01.00.00.
           hrk     Added support for FEATURE_IS2000_P2
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
01/09/01   tc      Program FPC_CTL2 to control the timing strobe to DSP.
01/05/01   tc      Set walsh gain based on radio configuration and walsh length.
02/20/01   hrk     FPC_SUBCHAN_GAIN affects only F-FCH, but it was also factored
                   into F-SCH setpoint value.
12/20/00   hrk     If FPC_SCH_INIT_SETPT_OP = 1, signed values for SCH_INIT_SETPT
                   were not correctly handled.
12/14/00   hrk     Enhancements in PLT for FFPC on F-FCH.
12/11/00   hrk     FFPC F-SCH logging indicated that SCH_INIT_SETPT started from
                   the default value instead of the value specified in ESCAM.
12/06/00   vlc     Changes to allow the inner loop to be enabled and the outer
                   loop to be disabled when performing certain forward link
                   power control tests.  (ks)
11/13/00   tc      Support MSM5100 demodulator forward power control.
11/22/00   hrk     Initializing default value of F-SCH RC to DEINT_RC_INVALID.
11/13/00   hrk     Optimized initialization and updates of setpoints.
                   Programming setpoint in HW is now done in ffpc_update_setpt
09/13/00   hrk     Default value for SCH_CURR_SETPT was not set. Default value
                   for SCH_TARGET_FER was used incorrectly.
09/11/00   hrk     Linted.
08/24/00   hrk     Removed redundant code to handle HO, SO, FPC_SUBCHAN_GAIN,
                   PWR CTRL cmds. FFPC changes during HO, SO .. is now handled
                   according to the new transaction mechanism.
07/31/00   lad     Fixed sub-record count in logging.
06/28/00   hrk     Initializing fpc_mode_incl to FALSE.
06/20/00   hrk     Removed unnecessary include files.
06/20/00   hrk     Updates to conform to changes in mctrans_ffpc_action_type.
                   Reporting delta between FCH and SCH setpts as a signed value
                   instead of an absolute value.
06/15/00   bgc     Changed PLT to support fch and sch setpoint.
06/12/00   lcc     Updates to conform to changes in mctrans_ffpc_action_type.
                   Also changes for new prototype for srch_set_fpc_mode.
06/09/00   hrk     Updating SRCH module with FPC_MODE information.
06/07/00   hrk     Added support for Outer Loop Report Message. Enclosed FFPC
                   functions put in Internal RAM with #ifndef MSM5000_IRAM_FWD
06/06/00   hrk     Added function ffpc_program_walsh_gain() for FFPC on F-SCH.
06/05/00   hrk     Accessing FWD RC info only if FWD RC info is included.
                   Support for logging F-FCH, F-SCH setpt delta.
                   Changed prototype for ffpc_update_setpt ().
06/02/00   lad     Added SCH support for FFPC logging.
05/30/00   hrk     Added support for new MC-RXC transaction mechanism.
04/28/00   hrk     Added support for FPC_MODE 011 (EIB 50Hz)
                   Ensuring that MIN_SETPT <= CURR_SETPT <= MAX_SETPT and
                   when CURR_SETPT_DB is decremented after receiving a good
                   frame it does not go below 0 dB.
04/25/00   hrk     Added FFPC support for SCH.
04/20/00   lad     Added subrecord count and len_per_frame to FFPC log.
04/17/00   hrk     Reformatted MSG display.
04/12/00   hrk     Moved sending of FFPC log packet into rxc_service_ho_cmd()
                   in case of a Hard HO.
04/11/00   lad     Added FFPC logging.
04/11/00   hrk     If BS changed FPC_SUBCHAN_GAIN and/or FWD_RC but not Outer
                   Loop params, the linear setpt values corresponding to
                   MIN_SETPT and MAX_SETPT were not updated.
                   Added function ffpc_rc_changed() to optimize setpt
                   re-computation only for specific RC changes.
                   Added logging support.
04/07/00   hrk     Added prototype for ffpc_disable();
04/05/00   hrk     Disabling Outer Loop on exiting TC state.
04/04/00   hrk     Fixed problem with Outer Loop being disabled when BS sends
                   msg with no Outer Loop params included.
                   Setpoints updates are done only if Outer Loop is enabled.
                   Initializing FCH FFPC params with default values to cover
                   the case if BS does not send FFPC params.
                   Added dignostic msgs for some error conditions.
03/27/00   hrk     Added diagnostic msg for FFPC setpts, Subchan gain and
                   Target FER settings. Also removed test code.
02/19/00   hrk     Added FEATURE_FFPC_EXTENDED_SETPT_RANGE
02/12/00   hrk     ffpc_init () was not called from ffpc_so_cmd.
01/19/00   hrk     Added erasure information to FFPC logging. Moved
                   definition of ffpc_params, ffpc_params_type from ffpc.h
                   into ffpc.c. Also moved definition of struct plt_ffpc_log_data
                   from plt_ffpc_log.h into ffpc.c.
12/21/99   hrk     Incorporated PLT support.
12/02/99   hrk     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <memory.h>
#include "comdef.h"
#include "qw.h"
#include "err.h"
#include "cai.h"
#include "log.h"
#include "ts.h"
#include "rxc.h"
#include "mccrx.h"
#include "txc.h"
#include "deint.h"
#include "ffpc.h"
#include "mctrans.h"
#include "msm_drv.h"
#include "muxmsm.h"
#include "srch_mux.h"

/*lint -e704 Right Shift of signed is ignored for entire file.
 * This is used extensively in this file as efficient divide by power of 2.
 * However, this is not portable.  Some compilers may treat as sign fill, and
 * others may treat as zero fill.
 * For example of right shift of signed value, see convert_dBq8_to_lin().
 */

/*lint -e703 Left Shift of signed is ignored similarly, but doesn't have the
 * portability concerns.
 */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* lookup table for linearly interpolated lg2 to linear conversions.
 * This interpolates only in the range (1,2], the rest must be done with
 * shifts. All values are in precision of Q14.
 */

struct 
{
  int16 leftpt;    /* left point of line segment */
  int16 slope;     /* slope of the line segment, for interpolation */
} lg2_to_lin [] =
  { /* round(2^(0:.25:.75)*2^14)     diff(round(2^(0:.25:1.00)*2^14))*/
    16384,                           3100,
    19484,                           3686,
    23170,                           4384,
    27554,                           5214
  };


/* scaled values for FPC_FCH_INIT_SETPT, unit : 0.125 dB */

const byte fpc_setpt_db_lin_0_132 [133] =
{
  0,   0,   0,   1,   2,   2,   3,   4,   4,   5,
  6,   7,   7,   8,   9,   10,  11,  11,  12,  13,
  14,  15,  16,  17,  18,  18,  19,  20,  21,  22,
  23,  24,  25,  26,  27,  28,  30,  31,  32,  33,
  34,  35,  36,  38,  39,  40,  41,  43,  44,  45,
  46,  48,  49,  50,  52,  53,  55,  56,  58,  59,
  61,  62,  64,  65,  67,  69,  70,  72,  74,  75,
  77,  79,  81,  83,  85,  86,  88,  90,  92,  94,
  96,  98,  100, 103, 105, 107, 109, 111, 114, 116,
  118, 121, 123, 126, 128, 131, 133, 136, 138, 141,
  144, 147, 149, 152, 155, 158, 161, 164, 167, 170,
  173, 176, 180, 183, 186, 189, 193, 196, 200, 203,
  207, 211, 214, 218, 222, 226, 230, 234, 238, 242,
  246, 250, 255
};

const byte fpc_setpt_db_lin_133_255 [123] =
{
  7,   8,   9,   10,  10,  11,  12,  12,  13,  14,
  15,  16,  16,  17,  18,  19,  20,  21,  21,  22,
  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,
  33,  34,  35,  36,  37,  39,  40,  41,  42,  43,
  44,  46,  47,  48,  49,  51,  52,  53,  55,  56,
  57,  59,  60,  62,  63,  65,  66,  68,  69,  71,
  73,  74,  76,  78,  79,  81,  83,  84,  86,  88,
  90,  92,  94,  96,  98,  100, 102, 104, 106, 108,
  110, 112, 114, 117, 119, 121, 124, 126, 128, 131,
  133, 136, 138, 141, 144, 146, 149, 152, 154, 157,
  160, 163, 166, 169, 172, 175, 178, 181, 185, 188,
  191, 194, 198, 201, 205, 208, 212, 216, 219, 223,
  227, 231, 235
};

struct
{
  byte  fpc_subchan_gain_db;   /* (units=dBq2, same as cdma2000 signalling) */
  boolean setpt_thresh_init;
  uint8 sch_thresh;
  int16 pri_sch_init_delta;
  fpc_mode_type fpc_mode;
  ffpc_chan_type fpc_pri_chan;
} ffpc_info;

#ifdef FEATURE_IS2000_SCH
/* IQACC_SCALE table for FFPC on SCH.
   The table values can be found in an internal QC proprietary document titled
   "MSM6500 MDSP 1x Demodulator/Searcher Interface" section 2.7, table 2-5. */
LOCAL word const ffpc_iqacc_sch_scale_tab[ 3 ][ 6 ] =
{
/*  rate =     1x    2x    4x    8x   16x   32x */
           { 8192, 5793, 4096, 2896, 2048,    0}, /* RC 3  */
           { 8192, 5793, 4096, 2896, 2048, 1448}, /* RC 4 */
           { 6689, 4730, 3344, 2365, 1672,    0}  /* RC 5 */
};
#endif /* FEATURE_IS2000_SCH */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

ffpc_chan_info_type  ffpc_fch_params;
ffpc_chan_info_type  ffpc_sch_params;

#ifdef FEATURE_IS2000_P2
ffpc_chan_info_type  ffpc_dcch_params;
#endif /*  FEATURE_IS2000_P2 */

/*
Accumulation buffer for FFPC logging.
*/
 /* Accumulated buffer */
log_ffwd_pctrl_type           ffpc_log_buf = {0};
/* Ptr to sub-record  */
log_ffwd_pctrl_subrecord_type *ffpc_log_sr_ptr = NULL;

#define DEFAULT_STEPSIZE_DBQ20 0x80000  /* 0.5 dB in Q20 */
#define SETPT_ADJUST_RC5_DBQ4       28 
  /* = 1.75 ~= 1.7609dB = 10log10(1.5) in Q4. Setpoint adjustment
   * required for RC due to 1.5 info bits (at 14.4kbps) during
   * the PCB duration (since PCB is used to estimate Eb).
   */
#define SETPT_ADJUST_PI_ON_TWO_DBQ4 31
  /* = 1.9375 ~= 1.9612dB = 10log10(pi/2) in Q4. Setpoint adjustment
   * for Jaguar.
   */

#define LIN_OUTPUT_BITS             16
  /* number of bits of output */

#define HW_SETPT_Q                  10
  /* Q-precision of output */

#define SHIFT_Q20_2_Q8              13
  /* setpt dB, >>13 is: div-by-2 for sqrt & convert Q20->Q8 */

#define DCCH_HW_SETPT_Q              8
  /* Q-precision of output for DCCH */

#define DCCH_SHIFT_Q20_2_Q8         12
  /* set pt dB, converted Q20->Q8 */

#define FFPC_LOG_OFFSET \
  (ffpc_log_buf.hdr.len - (word) FPOS(log_ffwd_pctrl_type, var_len_buf))

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

/* send FFPC log packet every 50 frames */
#define FFPC_FPC_MODE_EIB_LOGGING_INTERVAL  50

static int fpc_mode3_frame_counter = 1;

extern void rxc_mc_rpt (mccrx_rpt_type *);
log_ffwd_pctrl_frame_type *
 ffpc_log_frame_init (byte, word, boolean, boolean, boolean);

#ifdef FEATURE_LOG_PC_INFO_2
/*---------------------------------------------------------------------------
                   FAST FORWARD POWER CONTROL LOG TYPE
---------------------------------------------------------------------------*/
/* 1 channel rec:  (80bytes/4bytes) *20ms/frame --> 0.40 sec per log
** 2 channel recs: (80bytes/6bytes) *20ms/frame --> 0.26 sec per log
** 3 channel recs: (80bytes/8bytes) *20ms/frame --> 0.20 sec per log
** 4 channel recs: (80bytes/10bytes)*20ms/frame --> 0.16 sec per log
*/
/* 40 2-byte frame records plus 2-bytes per subrecord */
#define FFPC_INFO_MAX_BUF_SIZE (80 + 2)

/* Fast Forward Power Control frame record.  This structure defines each
** frame entry in the sub-record.  All fields are aligned on 2-byte boundaries
*/
typedef struct
{
  uint16 dec_history; /* Decision History: 16 1-bit decisions per frame */

  /* List of ffpc setpoint indicators that are active in the channel set mask.
  ** Note: There will be an entry for each member that is associated with an
  ** active channel in the active channel mask.  Not all members will exist
  ** in each frame entry.  Entries appear in order of active LSB to MSB in
  ** the channel set mask (FCH first).
  */
  union
  {
    /* If FCH is present */
    struct
    {
      uint8 power;      /* F-FCH setpoint in0.125 bB units        */
      uint8 rate : 4;   /* Use log_f_fch_rate_enum_type           */
      uint8 type : 4;   /* Use log_fch_type_enum_type             */
    } f_fch;

    /* If SCH0 is present */
    struct
    {
      uint8 power;      /* F-SCH0 setpoint in 0.125 bB units      */
      uint8 rate;       /* Use log_sch_rate_enum_type             */
    } f_sch0;

    /* If SCH1 is present */
    struct
    {
      uint8 power;      /* F-SCH1 setpoint in 0.125 bB units      */
      uint8 rate;       /* Use log_sch_rate_enum_type             */
    } f_sch1;

    /* If DCCH is present */
    struct
    {
      uint8 power;      /* F-DCCH setpoint in 0.125 bB units      */
      uint8 mux;        /* Use DCCH MUX type                      */
    } f_dcch;

    /* If SCH0 or SCH1 is present */
    struct
    {
      int8  delta;      /* F-SCH delta setpoint in 0.125 dB units */
      int8  reserved;
    } f_sch;

    /* If SCCH channels are present */
    struct
    {
      uint8 f_scch1_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 f_scch2_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 f_scch3_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 f_scch4_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 f_scch5_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 f_scch6_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 f_scch7_rate : 2;  /* Use log_sch_rate_enum_type      */
      uint8 reserved     : 2;
    } f_scch;

  } setpts[5]; /* Up to five entries based on channel set mask.       */
               /* (SCCH channels will not co-exist with SCH and DCCH) */

} ffpc_info_log_frame_type;

/* Fast Forward Power Control sub-record.  Used to log multiple frames with
** the same active channel set.  If the active channel set changes, a new
** sub-record is started.  All fields are aligned on 2-byte boundaries.
*/
typedef struct
{
  log_ded_fwd_chan_set_type ch_set_mask; /* Set of active channels */

  uint8 frame_cnt;   /* # of frames represented in this sub-record */

  /* Buffer to hold variable length array of */
  /* log_ffpc_info_frame_type records.       */
  uint8 frame_buf[FFPC_INFO_MAX_BUF_SIZE -
                  2 /* # of bytes in this structure before this field */ ];

} ffpc_info_log_subrecord_type;

/* Fast Forward Power Control Log Record
*/
typedef struct
{
  log_hdr_type hdr;          /* All logs must have a header */

  log_fwd_ch_rc_type  fwd_ch_rc;   /* Fwd Ch Radio Config   */
  uint16              so;          /* Service Option        */

  uint8 frame_offset;     /* Frame offset in 1.25 ms units. */
  uint8 fpc_mode;         /* FPC Mode                       */
  uint8 fpc_pri_chan;     /* Sub-channel Indicator          */
  uint8 fpc_subchan_gain; /* Sub-channel gain 0.25dB units  */

  /* Buffer to hold variable length array of */
  /* ffpc_info_subrecord_type records.       */
  uint8 data_buf[FFPC_INFO_MAX_BUF_SIZE];

} ffpc_info_log_type;

/* Accumulated buffer */
LOCAL ffpc_info_log_type            ffpc_info_log_buf;
/* Ptr to sub-record  */
LOCAL ffpc_info_log_subrecord_type *ffpc_info_log_sr_ptr = NULL;
/* Current length of packet */
LOCAL int                           ffpc_info_log_length = 0;

/* Threshold to determine when the buf is full and the log should be sent.*/
#define FFPC_INFO_LOG_BUF_THRESHOLD                  \
  (sizeof(ffpc_info_log_type) -                      \
    (FPOS(ffpc_info_log_subrecord_type, frame_buf) + \
     sizeof(ffpc_info_log_frame_type)))

/* Index into data_buf of start of next frame */
#define FFPC_INFO_LOG_OFFSET(cur_buf_len) \
  (cur_buf_len - (word) FPOS(ffpc_info_log_type, data_buf))

ffpc_info_log_frame_type *
 ffpc_info_log_init (byte,
                     word,
                     boolean,
                     cai_radio_config_type,
                     boolean,
                     cai_radio_config_type,
                     boolean,
                     cai_radio_config_type);

#endif /* FEATURE_LOG_PC_INFO_2 */

byte ffpc_do_not_update_sch_setpt_counter = 0;
#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
extern boolean rxc_rifhho_cfs_gps_in_progress;
extern byte rxc_cfs_tuneback_count;
#endif
extern boolean rxc_tx_in_traffic_on;

/*========================================================================

FUNCTION FFPC_GET_PARAM_PTR

DESCRIPTION
  Returns a pointer to the correct ffpc params data based on the 
  provided channel.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to ffpc params structure

SIDE EFFECTS
  None

=========================================================================*/
ffpc_chan_info_type* ffpc_get_param_ptr 
(
  ffpc_chan_type channel
)
{
  ffpc_chan_info_type  *ffpc_params_ptr = NULL;

#ifdef FEATURE_IS2000_P2
  if (channel == FFPC_FCH) 
  {             
    ffpc_params_ptr = &ffpc_fch_params;     
  } 
  else if (channel == FFPC_DCCH) 
  {     
    ffpc_params_ptr = &ffpc_dcch_params;    
  } 
  else 
  {                               
    ffpc_params_ptr = &ffpc_sch_params;     
  }
#else
  if (channel == FFPC_FCH) 
  {             
    ffpc_params_ptr = &ffpc_fch_params;     
  }
  else 
  {                               
    ffpc_params_ptr = &ffpc_sch_params;     
  }
#endif /* FEATURE_IS2000_P2 */

  return ffpc_params_ptr;
}


/*========================================================================

FUNCTION FFPC_SCH_FPC_ENABLED

DESCRIPTION
  Returns true if SCH FPC is enabled.

DEPENDENCIES
  None.

RETURN VALUE
  true if SCH FPC enabled

SIDE EFFECTS
  None

=========================================================================*/
boolean ffpc_sch_fpc_enabled(void)
{
   return (ffpc_sch_params.olpc_enabled);
}

#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )

/*========================================================================

FUNCTION FFPC_GET_CURRENT_FCH_SETPOINT

DESCRIPTION
  This function retrieves the current FCH FFPC setpoint.

DEPENDENCIES
  None.

RETURN VALUE
  Signed Int32 setpoint in dB.

SIDE EFFECTS
  None

=========================================================================*/
int32 ffpc_get_current_fch_setpoint( void )
{

  return( ffpc_fch_params.curr_setpt_db );

} /* ffpc_get_current_fch_setpoint */

/*========================================================================

FUNCTION FFPC_CALCULATE_PCBIT_DATA

DESCRIPTION
  This function calculates the pcbit data that should be punctured on
  the reverse pilot channel for forward power control.

DEPENDENCIES
  None.

RETURN VALUE
  The word of bits, one for each pcg.

SIDE EFFECTS
  None

=========================================================================*/
word ffpc_calculate_pcbit_data
(
  boolean fch_eib
#ifdef FEATURE_IS2000_REL_A
  ,boolean sch_eib
  ,boolean fch_qib
#endif /* FEATURE_IS2000_REL_A */
)
{
  word pcbit_data = 0;
  switch (ffpc_info.fpc_mode)
  {
    /* No pcbit data for following modes:
     *
     * FPC_MODE_FCH_800Hz:
     * FPC_MODE_FCH_400Hz_SCH_400Hz:
     * FPC_MODE_FCH_200Hz_SCH_600Hz:
     *
     */

    case FPC_MODE_PRI_EIB:
      pcbit_data = fch_eib * 0xffff; /*lint !e734 Loss of precision 24 bits to 16 bits */
      break;

#ifdef FEATURE_IS2000_REL_A
    case FPC_MODE_PRI_QIB:
      pcbit_data = fch_qib * 0xffff; /*lint !e734 Loss of precision 24 bits to 16 bits */
      break;

    case FPC_MODE_PRI_QIB_SEC_EIB:
      pcbit_data |= fch_qib * 0x5555; /*lint !e734 Loss of precision 23 bits to 16 bits */
      pcbit_data |= sch_eib * 0xaaaa; /*lint !e734 Loss of precision 24 bits to 16 bits */
      break;

    case FPC_MODE_PRI_400Hz_SEC_EIB:
      pcbit_data = sch_eib * 0xaaaa; /*lint !e734 Loss of precision 24 bits to 16 bits */
      break;
#endif /* FEATURE_IS2000_REL_A */
    default:
      /* Nothing to do for other modes */
      break;
  }
  return pcbit_data;
}

/* <EJECT> */

/*=========================================================================
FUNCTION FFPC_TCI_CMD

DESCRIPTION
  This function is the interface function to be called from RXC module
  in Traffic Channel Initialization state. It will setup the FFPC
  parameters for FFPC SW/HW setpoints programming.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/

void
ffpc_tci_cmd
(
 rxc_cmd_msg_type  *cmd_ptr
)
{
   boolean rc_invalid_for_ffpc = FALSE;
   mctrans_fpc_olpc_param_type *fch_olpc_ptr, sch_olc_params;
   mctrans_fpc_olpc_param_type fch_olpc_params;
#ifdef FEATURE_IS2000_P2
   mctrans_fpc_olpc_param_type *dcch_olpc_ptr, dcch_olpc_params;
#endif /* FEATURE_IS2000_P2 */

   /* All information is invalid */
   ffpc_fch_params.incl_status = 0;
   ffpc_sch_params.incl_status = 0;
#ifdef FEATURE_IS2000_P2
   ffpc_dcch_params.incl_status = 0;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_PLT
#error code not present
#else

   /* intialize FPC_PRI_CHAN, FPC_MODE, FPC_SUBCHAN_GAIN with default values */
   ffpc_info.fpc_pri_chan         = FFPC_FCH;
   ffpc_info.fpc_mode             = FPC_MODE_PRI_800Hz;
   ffpc_info.fpc_subchan_gain_db  = FFPC_DEFAULT_SUBCHAN_GAIN;

#ifdef FEATURE_IS2000_P2
   ffpc_dcch_params.incl_status |= FFPC_SUBCHAN_GAIN_INCL;
#endif /* FEATURE_IS2000_P2 */

   if (cmd_ptr->tci.fpc.included)
   {
      if (cmd_ptr->tci.fpc.fpc_pri_chan == CAI_FPC_PRI_CHAN_FCH)
      {
         ffpc_info.fpc_pri_chan = FFPC_FCH;
      }
#ifdef FEATURE_IS2000_P2
      else
      {
         ffpc_info.fpc_pri_chan = FFPC_DCCH;
      }
#endif /* FEATURE_IS2000_P2 */

      /* store FPC_SUBCHAN_GAIN */
      ffpc_info.fpc_subchan_gain_db = cmd_ptr->tci.fpc.fpc_subchan_gain;

      /* initialize FPC_MODE */
      ffpc_info.fpc_mode = (fpc_mode_type) cmd_ptr->tci.fpc.fpc_mode;
   }

   /* initialze FPC_SUBCHAN_GAIN */
   ffpc_init_subchan_gain (ffpc_info.fpc_subchan_gain_db);

   /* Initialze FCH FFPC parameters with default values */
   ffpc_fch_params.init_setpt_db  = FFPC_DEFAULT_FCH_INIT_SETPT_VAL;
   ffpc_fch_params.curr_setpt_db  = ffpc_fch_params.init_setpt_db << 17;
   ffpc_fch_params.incl_status    |= FFPC_INIT_SETPT_INCL;

   fch_olpc_params.min_setpt.set_to_current = FALSE;
   fch_olpc_params.min_setpt.value          = FFPC_DEFAULT_FCH_MIN_SETPT_VAL;
   fch_olpc_params.max_setpt.set_to_current = FALSE;
   fch_olpc_params.max_setpt.value          = FFPC_DEFAULT_FCH_MAX_SETPT_VAL;
   fch_olpc_params.target_fer               = FFPC_DEFAULT_FCH_TARGET_FER;
   fch_olpc_ptr = &fch_olpc_params;

   ffpc_fch_params.incl_status    |= FFPC_OLPC_INCL;
   ffpc_fch_params.olpc_enabled   = FALSE;

#ifdef FEATURE_IS2000_P2
   /* Initialze DCCH FFPC parameters with default values */
   ffpc_dcch_params.init_setpt_db = FFPC_DEFAULT_DCCH_INIT_SETPT_VAL;
   ffpc_dcch_params.curr_setpt_db = ffpc_fch_params.init_setpt_db << 17;
   ffpc_dcch_params.incl_status   |= FFPC_INIT_SETPT_INCL;

   dcch_olpc_params.min_setpt.set_to_current = FALSE;
   dcch_olpc_params.min_setpt.value          = FFPC_DEFAULT_DCCH_MIN_SETPT_VAL;
   dcch_olpc_params.max_setpt.set_to_current = FALSE;
   dcch_olpc_params.max_setpt.value          = FFPC_DEFAULT_DCCH_MAX_SETPT_VAL;
   dcch_olpc_params.target_fer               = FFPC_DEFAULT_DCCH_TARGET_FER;
   dcch_olpc_ptr = &dcch_olpc_params;

   ffpc_dcch_params.incl_status   |= FFPC_OLPC_INCL;
   ffpc_dcch_params.olpc_enabled  = FALSE;
#endif /* FEATURE_IS2000_P2 */

   if (cmd_ptr->tci.fpc.included)
   {
      if (cmd_ptr->tci.fpc.fch_olpc.included)
      {
         ffpc_fch_params.init_setpt_db = cmd_ptr->tci.fpc.fch_olpc.init_setpt;
         ffpc_fch_params.curr_setpt_db = ffpc_fch_params.init_setpt_db << 17;
         fch_olpc_ptr                  = &cmd_ptr->tci.fpc.fch_olpc.params;
      }

#ifdef FEATURE_IS2000_P2
      if (cmd_ptr->tci.fpc.dcch_olpc.included)
      {
         ffpc_dcch_params.init_setpt_db = cmd_ptr->tci.fpc.dcch_olpc.init_setpt;
         ffpc_dcch_params.curr_setpt_db = ffpc_dcch_params.init_setpt_db << 17;
         dcch_olpc_ptr                  = &cmd_ptr->tci.fpc.dcch_olpc.params;
      }
#endif /* FEATURE_IS2000_P2 */
   }

   ffpc_init_outer_loop_params (fch_olpc_ptr, FFPC_FCH);

#ifdef FEATURE_IS2000_P2
   ffpc_init_outer_loop_params (dcch_olpc_ptr, FFPC_DCCH);
#endif /* FEATURE_IS2000_P2 */

   if ((cmd_ptr->tci.fpc.included) && (cmd_ptr->tci.fpc.fch_olpc.included))
   {
      MSG_HIGH ("FCH: INIT %d Subchan Gain %d",
                 ffpc_fch_params.init_setpt_db, ffpc_info.fpc_subchan_gain_db, 0);
      MSG_HIGH ("FCH: MIN %d  MAX %d  Target FER %d",
                 ffpc_fch_params.min_setpt_db, ffpc_fch_params.max_setpt_db,
                 ffpc_fch_params.target_fer);
   }

#ifdef FEATURE_IS2000_P2
   if ((cmd_ptr->tci.fpc.included) && (cmd_ptr->tci.fpc.dcch_olpc.included))
   {
      MSG_HIGH ("DCCH: INIT %d Subchan Gain %d",
                 ffpc_dcch_params.init_setpt_db, ffpc_info.fpc_subchan_gain_db, 0);
      MSG_HIGH ("DCCH: MIN %d  MAX %d  Target FER %d",
                 ffpc_dcch_params.min_setpt_db, ffpc_dcch_params.max_setpt_db,
                 ffpc_dcch_params.target_fer);
   }
#endif /* FEATURE_IS2000_P2 */

   /* Initialize F-FCH RC to Default value */
   ffpc_fch_params.fwd_rc  = DEINT_RC_INVALID;

#ifdef FEATURE_IS2000_P2
   if (cmd_ptr->tci.for_fch.included)
#endif /* FEATURE_IS2000_P2 */
   {
      ffpc_init_rc ((deint_rc_type) cmd_ptr->tci.for_fch.rc, FFPC_FCH);

      /* determine if RC is valid for FFPC operation */
      if (ffpc_fch_params.fwd_rc <= DEINT_RC2)
      {
         rc_invalid_for_ffpc = TRUE;
      }
   }

#ifdef FEATURE_IS2000_P2
   /* Initialize F-DCCH RC to Default value */
   ffpc_dcch_params.fwd_rc = DEINT_RC_INVALID;

   if (cmd_ptr->tci.for_dcch.included)
   {
      ffpc_init_rc ((deint_rc_type) cmd_ptr->tci.for_dcch.rc, FFPC_DCCH);

      /* determine if RC is valid for FFPC operation */
      if (ffpc_dcch_params.fwd_rc <= DEINT_RC2)
      {
         rc_invalid_for_ffpc = TRUE;
      }
   }
#endif /* FEATURE_IS2000_P2 */

   /* initialize F-SCH FFPC with default parameters */
   ffpc_sch_params.init_setpt_db = FFPC_DEFAULT_SCH_INIT_SETPT_VAL;
   ffpc_sch_params.curr_setpt_db = ffpc_sch_params.init_setpt_db << 17;

   sch_olc_params.min_setpt.set_to_current = FALSE;
   sch_olc_params.min_setpt.value          = FFPC_DEFAULT_SCH_MIN_SETPT_VAL;
   sch_olc_params.max_setpt.set_to_current = FALSE;
   sch_olc_params.max_setpt.value          = FFPC_DEFAULT_SCH_MAX_SETPT_VAL;
   sch_olc_params.target_fer               = FFPC_DEFAULT_SCH_TARGET_FER;
   ffpc_init_outer_loop_params (&sch_olc_params, FFPC_SCH);

   ffpc_sch_params.fwd_rc        = DEINT_RC_INVALID;
   ffpc_info.setpt_thresh_init   = FALSE;

   /* Default FPC_MODE = 0. This is set through F-FCH FFPC info */
   ffpc_sch_params.incl_status   = (FFPC_SUBCHAN_GAIN_INCL |
                                    FFPC_INIT_SETPT_INCL   |
                                    FFPC_OLPC_INCL         |
                                    FFPC_MODE_INCL);
#endif /* FEATURE_PLT */

   /* Validity of RCs on forward channels for FFPC established,
   ** update SCRH, TXC with FPC_MODE based on RC validity and FPC_MODE.
   */
   ffpc_update_fpc_mode (rc_invalid_for_ffpc, ffpc_info.fpc_mode);

   /* if RC invalid for FFPC or using only EIB and/or QIB, disable FFPC and return */
   if (rc_invalid_for_ffpc || (ffpc_info.fpc_mode == FPC_MODE_PRI_EIB)
#ifdef FEATURE_IS2000_REL_A
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB)
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
   )
   {
     /* disable FFPC */
     ffpc_disable ();

     return;
   }

   /* initialize FFPC HW */
   ffpc_program_hw ();

   /* Initialize Setpoints and enable Outer Loop in SW */
#ifdef FEATURE_PLT
#error code not present
#else
   if (cmd_ptr->tci.fpc.included && cmd_ptr->tci.fpc.fch_olpc.included)
   {
      ffpc_init_setpts (FFPC_FCH);
      ffpc_fch_params.olpc_enabled = TRUE;
   }

#ifdef FEATURE_IS2000_P2
   if (cmd_ptr->tci.fpc.included && cmd_ptr->tci.fpc.dcch_olpc.included)
   {
      ffpc_init_setpts (FFPC_DCCH);
      ffpc_dcch_params.olpc_enabled = TRUE;
   }
#endif /* FEATURE_IS2000_P2 */
#endif /* FEATURE_PLT */
}

/*=========================================================================
FUNCTION FFPC_UPDATE_FPC_MODE

DESCRIPTION
  This function will update SRCH, TXC modules with the FPC_MODE based on
  FPC_MODE and whether or not the RCs for the forward traffic channels are
  valid for FFPC operation (RC > 2). It will also initialize the FPC_MODE
  in HW.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_update_fpc_mode
(
 boolean rc_invalid_for_ffpc,
 fpc_mode_type fpc_mode
)
{
   if (rc_invalid_for_ffpc ||
     (fpc_mode == FPC_MODE_PRI_EIB)
#ifdef FEATURE_IS2000_REL_A
     || (fpc_mode == FPC_MODE_PRI_QIB)
     || (fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB )
#endif /* FEATURE_IS2000_REL_A */
     )
   {
      /* Update FPC_MODE in SRCH module. */
      srch_mux_notify_fpc_mode (FALSE,(word)fpc_mode);

      /* Update FPC_MODE in TXC module. */
      if (rc_invalid_for_ffpc)
      {
         /* set FPC_MODE in TXC to FPC_MODE_FFPC_DISABLED regardless of
         ** the actual FPC_MODE value
         */
         txc_set_fpc_mode (FPC_MODE_FFPC_DISABLED);
      }
      else
      {
         /* RC valid, update TXC with the actual FPC_MODE value */
         txc_set_fpc_mode (ffpc_info.fpc_mode);
      }
   }
   else
   {
      /* program FPC_MODE in HW */
      ffpc_program_fpc_mode ((fpc_mode_type) ffpc_info.fpc_mode);

      /* update FPC_MODE info in TXC, SRCH */
      txc_set_fpc_mode ((fpc_mode_type) ffpc_info.fpc_mode);
      srch_mux_notify_fpc_mode (TRUE,(word)fpc_mode);
   }
}

/*=========================================================================
FUNCTION FFPC_INIT_SUBCHAN_GAIN

DESCRIPTION
  This function will validate and store the FPC_SUBCHAN_GAIN value

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_init_subchan_gain
(
 byte fpc_subchan_gain
)
{
   /* valid value for FPC_SUBCHAN_GAIN - 0:31, 5 bits */
   if (fpc_subchan_gain > 31)
   {
      MSG_ERROR("FPC_SUBCHAN_GAIN val out of rng", 0, 0, 0);
   }
   else
   {
      ffpc_info.fpc_subchan_gain_db  = fpc_subchan_gain;
   }
   ffpc_fch_params.incl_status  |= FFPC_SUBCHAN_GAIN_INCL;
   ffpc_sch_params.incl_status  |= FFPC_SUBCHAN_GAIN_INCL;

#ifdef FEATURE_IS2000_P2
   ffpc_dcch_params.incl_status |= FFPC_SUBCHAN_GAIN_INCL;
#endif /* FEATURE_IS2000_P2 */
}

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_RETURNED_FAILURE_CMD

DESCRIPTION
  This function is the interface function to be called from RXC module
  in case of Hard Handoff Failure (RXC_RETURNED_F).
  It will setup the FFPC parameters for FFPC SW/HW setpoints programming.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_returned_failure_cmd
(
 rxc_cmd_msg_type  *cmd_ptr
)
{
   boolean rc_invalid_for_ffpc = FALSE;

   /* process FPC_SUBCHAN_GAIN */
   if (cmd_ptr->ret.fpc_subchan_gain.included)
   {
      ffpc_init_subchan_gain (cmd_ptr->ret.fpc_subchan_gain.value);
   }

   /* process SCR */
   if (cmd_ptr->ret.scr.included)
   {
      ffpc_process_scr (&cmd_ptr->ret.scr.data);

      /* determine if F-FCH RC is valid for FFPC operation */
      if (cmd_ptr->ret.scr.data.for_fch.included &&
          (ffpc_fch_params.fwd_rc <= DEINT_RC2))
      {
         rc_invalid_for_ffpc = TRUE;
      }

#ifdef FEATURE_IS2000_P2
      /* determine if F-DCCH RC is valid for FFPC operation */
      if (cmd_ptr->ret.scr.data.for_dcch.included &&
          (ffpc_dcch_params.fwd_rc <= DEINT_RC2))
      {
         rc_invalid_for_ffpc = TRUE;
      }
#endif /* FEATURE_IS2000_P2 */
   }

   /* Validity of RCs on forward channels for FFPC established,
   ** update SCRH, TXC with FPC_MODE based on RC validity and FPC_MODE.
   */
   ffpc_update_fpc_mode (rc_invalid_for_ffpc, ffpc_info.fpc_mode);

   /* if RC invalid for FFPC or using only EIB and/or QIB, disable FFPC and return */
   if ((rc_invalid_for_ffpc) || (ffpc_info.fpc_mode == FPC_MODE_PRI_EIB)
#ifdef FEATURE_IS2000_REL_A
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB)
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
   )
   {
      ffpc_disable ();
      return;
   }

   if (cmd_ptr->ret.fpc_subchan_gain.included ||
      (cmd_ptr->ret.scr.included &&
       cmd_ptr->ret.scr.data.fpc.included &&
       cmd_ptr->ret.scr.data.fpc.fch_olpc.included) ||
      (cmd_ptr->ret.scr.included && cmd_ptr->ret.scr.data.for_fch.included))
   {
      /* re-init F-FCH setpoints */
      ffpc_init_setpts (FFPC_FCH);
   }

#ifdef FEATURE_IS2000_P2
   if (cmd_ptr->ret.fpc_subchan_gain.included ||
      (cmd_ptr->ret.scr.included &&
       cmd_ptr->ret.scr.data.fpc.included &&
       cmd_ptr->ret.scr.data.fpc.dcch_olpc.included) ||
      (cmd_ptr->ret.scr.included && cmd_ptr->ret.scr.data.for_dcch.included))
   {
      /* re-init F-DCCH setpoints */
      ffpc_init_setpts (FFPC_DCCH);
   }
#endif /* FEATURE_IS2000_P2 */
}
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_PROCESS_SCR

DESCRIPTION
  This function will setup FFPC parameters from a Service Configuration
  Record.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_process_scr
(
  rxc_scr_info_type  *scr
)
{
   if (scr->fpc.included)
   {
      /* process FPC_PRI_CHAN */
      if (scr->fpc.fpc_pri_chan == CAI_FPC_PRI_CHAN_FCH)
      {
         ffpc_info.fpc_pri_chan = FFPC_FCH;
      }
#ifdef FEATURE_IS2000_P2
      else
      {
         ffpc_info.fpc_pri_chan = FFPC_DCCH;
      }
#endif /* FEATURE_IS2000_P2 */

      /* store new FPC_MODE */
      ffpc_info.fpc_mode = (fpc_mode_type) scr->fpc.fpc_mode;

      /* update F-FCH FFPC OLPC parameters */
      if (scr->fpc.fch_olpc.included)
      {
         ffpc_init_outer_loop_params (&scr->fpc.fch_olpc.params, FFPC_FCH);
      }

#ifdef FEATURE_IS2000_P2
      /* update F-DCCH FFPC OLPC parameters */
      if (scr->fpc.dcch_olpc.included)
      {
         ffpc_init_outer_loop_params (&scr->fpc.dcch_olpc.params, FFPC_DCCH);
      }
#endif /* FEATURE_IS2000_P2 */
   }

   /* process new F-FCH RC */
   if (scr->for_fch.included)
   {
      ffpc_init_rc ( (deint_rc_type) scr->for_fch.rc, FFPC_FCH);
   }

#ifdef FEATURE_IS2000_P2
   /* process new F-DCCH RC */
   if (scr->for_dcch.included)
   {
      ffpc_init_rc ( (deint_rc_type) scr->for_dcch.rc, FFPC_DCCH);
   }
#endif /* FEATURE_IS2000_P2 */
}

/*=========================================================================
FUNCTION FFPC_INIT_RC

DESCRIPTION
  This function will initialize the new RC for the channel specified.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_init_rc
(
  deint_rc_type fwd_rc,
  ffpc_chan_type channel
)
{
   ffpc_chan_info_type  *ffpc_params;

   ffpc_params = ffpc_get_param_ptr(channel);

   /* determine if new RC and old RC are different
   ** to optimize FFPC setpt computation
   */
   if (ffpc_rc_changed (ffpc_params->fwd_rc, fwd_rc))
   {
      /* RC has changed */
      ffpc_params->incl_status |= FFPC_FWD_RC_INCL;
   }

   /* store new RC */
   ffpc_params->fwd_rc = fwd_rc;
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_PROGRAM_HW

DESCRIPTION
  This function will program registers in HW for FFPC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_program_hw (void)
{
   /* forward link PC bit filter gain */
   DEMOD_SET_PC_FILT_GAIN( 0x4000 );

   /* forward link Nt/Io filter gain */
   DEMOD_SET_NT_IO_FILT_GAIN( 0x0800 );

   /* Program FPC_CTL register, enable FPC */

   /* Due to a bug in previous code, the incorrect values were being
   ** written to FPC_POSITION1 and FPC_POSITION2, as follows:
   ** register         intended value  actual value
   ** ---------------- --------------- ------------
   ** FPC_POSITION1     9              4
   ** FPC_POSITION2     9              0
   ** For now, just keep writing the same values.
   */
   MUXMSM_FPC_SCH_POS( 0x00 );
   MUXMSM_FPC_POSITION1( 0x04 );
   MUXMSM_FPC_POSITION2( 0x00 );
   MUXMSM_FPC_ENABLE();
   MUXMSM_FPC_SCH_POS_DSP( 0x08 );

}

/*=========================================================================
FUNCTION
  convert_dBq8_to_lin()

DESCRIPTION
  Converts dB to linear.

DEPENDENCE
  None.

RETURN VALUE
  10^(input/10) in Q factor given by input

SIDE EFFECTS
  None.
=========================================================================*/
int32
convert_dBq8_to_lin
(
 int32 x_dB_q8,          /* db value in Q8 (i.e. resolution of 1/256 dB) */
 int32 out_Q,            /* desired Q-factor of the output */
 int32 out_b             /* number of signed bits of output */
)
{
   int32 x_lg2_q16;
   int32 index, interp, shift;
   int32 x_lin;

   x_lg2_q16 = x_dB_q8 * 85;
     /* convert to log2() (q16) with conversion factor in Q8:
      * factor = 85 = round(1/(10*log10(2))*2^8) ~= 0.33219*2^8
      */

   /* setup computation: x_lg2_q16[31:16]   define number of shifts (i.e. power of 2)
    *                    x_lg2_q16[15:14]   define lookup table index
    *                    x_lg2_q16[13:0]    define interpolation fraction (Q14)
    */
   index  = (x_lg2_q16 >> 14) & 0x3;
   interp = x_lg2_q16 & 0x3fff;
   shift  = out_Q - 14 + (x_lg2_q16 >> 16);

   /* compute */
   x_lin = lg2_to_lin[index].leftpt + ((interp * lg2_to_lin[index].slope)>>14);
   
   if (shift < -14)
   {
      x_lin >>= 14;
      MSG_ERROR("UNDRFLW",0,0,0);
   }
   else if (shift > out_b - 16)
   {
      /* don't let it overflow 32 signed bits */
      x_lin <<= (out_b - 16);
      MSG_ERROR("OVRFLW",0,0,0);
   }
   else if (shift < 0)
   {
      x_lin >>= -shift; //lint !e504 Unusual shift operation (because of negation)
   }
   else
   {
      x_lin <<= shift;
   }
   
   return x_lin;
}

/*=========================================================================
FUNCTION
  ffpc_get_hw_setpt()

DESCRIPTION
  This takes the adjusted setpoint in dB and converts to the set point
  in linear units as required by the DSP.

DEPENDENCE
  None.

RETURN VALUE
  HW set point value

SIDE EFFECTS
  None.
=========================================================================*/
word
ffpc_get_hw_setpt
(
 int32 setpt_dB_q20               /* set point in units of dB Q20 */
)
{
   /* note: calls convert_dBq8_to_lin() with input scaled >>1
    * (in Q8) so that output is magnitude (instead of mag^2).
    */
   return (word) (convert_dBq8_to_lin(setpt_dB_q20 >> SHIFT_Q20_2_Q8,
                                           HW_SETPT_Q, LIN_OUTPUT_BITS));
}

/*========================================================================

FUNCTION FFPC_SET_SETPT_OFFSET_DBQ4

DESCRIPTION
  This function programs the FFPC setpoint offset applied to the FFPC 
  inner loop.  This is for short term modifications of the inner loop.
  The effect of an asserted offset is removed by re-setting the offset to
  0.
  
  The function accepts the offset in units of dB Q4, i.e. 0.0625dB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void
ffpc_set_setpt_offset_dbq4
(
  int16 offset_dbq4
)
{
  word hw_setpt;
  ffpc_chan_info_type  *ffpc_params;

  if (ffpc_info.fpc_pri_chan == FFPC_FCH
      #ifdef FEATURE_IS2000_P2
      || (ffpc_info.fpc_pri_chan == FFPC_DCCH)
      #endif /* FEATURE_IS2000_P2 */
     )
  {
  
    ffpc_params = ffpc_get_param_ptr(ffpc_info.fpc_pri_chan);

    /* store new offset */
    ffpc_params->setpt_offset_dbq4 = offset_dbq4;
    
    /* update adjusted set point */
    ffpc_params->setpt_adjusted_dbq20 = 
      ffpc_params->curr_setpt_db +
      ((ffpc_params->setpt_adjust_dbq4 +
        ffpc_params->setpt_offset_dbq4) << 16);
    
    /* write the set point to HW */
    hw_setpt = ffpc_get_hw_setpt(ffpc_params->setpt_adjusted_dbq20);
    MSG_MED("SP dB: 0x%08x. SPA dB: 0x%08x. SPD lin: 0x%08x.",
            ffpc_params->curr_setpt_db,
            ffpc_params->setpt_adjusted_dbq20,
            hw_setpt);

    ffpc_program_setpt(hw_setpt, ffpc_info.fpc_pri_chan);
  }
  else
  {
    MSG_MED("SP offset not allwd, FPC Pri Ch (%d)", 
            ffpc_info.fpc_pri_chan, 0, 0);
  }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_PROGRAM_SETPT

DESCRIPTION
  This function will program the FCH, SCH SETPT register in HW for FFPC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_program_setpt
(
  word setpt,
  ffpc_chan_type channel
)
{
   if (channel == FFPC_FCH)
   {
      DEMOD_SET_EB_NO_TARGET_FCH (setpt);

      /* cache hw setpt for logging */
      ffpc_fch_params.hw_eb_no_setpt = setpt;
   }
   else if (channel == FFPC_SCH)
   {
      DEMOD_SET_EB_NO_TARGET_SCH (setpt);
      /* cache hw setpt for logging */
      ffpc_sch_params.hw_eb_no_setpt = setpt;
   }
#ifdef FEATURE_IS2000_P2
   else
   {
      DEMOD_SET_EB_NO_TARGET_FCH (setpt);

      /* cache hw setpt for logging */
      ffpc_dcch_params.hw_eb_no_setpt = setpt;
   }
#endif /* FEATURE_IS2000_P2 */
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_PROGRAM_FPC_MODE

DESCRIPTION
  This function will program the FPC_SEL register in HW for FFPC.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_program_fpc_mode
(
 fpc_mode_type fpc_mode
)
{
   /* program channel source selection */
   if (fpc_mode == FPC_MODE_PRI_400Hz_SEC_400Hz)
   {
     MUXMSM_FPC_SEL( 0xaaaa );
   }
   else if (fpc_mode == FPC_MODE_PRI_200Hz_SEC_600Hz)
   {
     MUXMSM_FPC_SEL( 0xdddd );
   }
   else
   {
      /*
       * There is no inner loop power control for the following modes:
       *   FPC_MODE_PRI_EIB
       *   FPC_MODE_PRI_QIB
       *   FPC_MODE_PRI_QIB_SEC_EIB
       * For these, it is safe to set this register to 0x0.
       *
       * The following modes have inner loop power control only on primary:
       *   FPC_MODE_FCH_800Hz=0x0
       *   FPC_MODE_FCH_400Hz_SCH_EIB
       * For these, it is required to set this register to 0x0 so that all
       * PCG's are punctured corresponding to inner loop commands from the
       * primary channel.
       */
      MUXMSM_FPC_SEL( 0x0000 );
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_DISABLE

DESCRIPTION
  This function will disable FFPC in HW and the Outer Loop in SW.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_disable (void)
{
   /* disable FFPC in HW only in case of RC1 or RC2.  If disabled for other
    * radio configurations, disabling fpc will prevent proper DTX detection.
    */
   if ( (ffpc_fch_params.fwd_rc == DEINT_RC1)
      || (ffpc_fch_params.fwd_rc == DEINT_RC2) )
   {
     MUXMSM_FPC_DISABLE();
   }

   /* shut off outer loop in software */
   ffpc_fch_params.olpc_enabled  = FALSE;
   ffpc_sch_params.olpc_enabled  = FALSE;

#ifdef FEATURE_IS2000_P2
   ffpc_dcch_params.olpc_enabled = FALSE;
#endif /* FEATURE_IS2000_P2 */
}

/*=========================================================================
FUNCTION FFPC_DISABLE_CHAN_OLPC

DESCRIPTION
  This function will disable Outer Loop for the channel specified.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_disable_chan_olpc
(
  ffpc_chan_type  channel
)
{
   ffpc_chan_info_type  *ffpc_params;

   ffpc_params = ffpc_get_param_ptr(channel);

   /* shut off FFPC outer loop */
   ffpc_params->olpc_enabled = FALSE;
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_UPDATE_SETPT

DESCRIPTION
 This function will update the FFPC set points values based on whether
 the frame was succesfully decoded or was an erasure and program the updated
 value in HW.

DEPENDENCIES
 Assumes the parameters for outer loop FPC have been initialized in
 struct ffpc_params : This is done by calling the function ffpc_init().

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/

void
ffpc_update_setpt
(
 rxc_frame_type  frame_status,
 ffpc_chan_type  channel
)
{
   ffpc_chan_info_type *ffpc_params;
   word                 hw_setpt;

   ffpc_params = ffpc_get_param_ptr(channel);

   /* do not update SETPT for period of tune away due to HHO/CFS/GPS
   ** or the transmitter is turning off.
   */

   
   if ( !rxc_tx_in_traffic_on
        #if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
        || rxc_rifhho_cfs_gps_in_progress || (rxc_cfs_tuneback_count > 0)
        #endif
      )
   {
     return;
   }


   if ((channel == FFPC_SCH) && (ffpc_do_not_update_sch_setpt_counter > 0))
   {
      ffpc_do_not_update_sch_setpt_counter--;
      return;
   }

   if (ffpc_params->olpc_enabled)
   {
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

      if (frame_status == RXC_DTX_FRAME)
      {
         /* do nothing */
         return;
      }
      else if (frame_status == RXC_ERASURE_FRAME)
      {
         /* update the set point based on the current linear set point. */

         /* must maintain the dB in the new way not the old way
            (didn't replicate dB set pt) */
         ffpc_params->curr_setpt_db += ffpc_params->setpt_adj_up_dbq20;
         if (ffpc_params->curr_setpt_db > ((int32)ffpc_params->max_setpt_db << 17))
         {
            ffpc_params->curr_setpt_db = (int32)ffpc_params->max_setpt_db << 17;
         }
      }
      else if (frame_status == RXC_GOOD_FRAME)
      {
         /* update the set point based on the current linear set point. */
         ffpc_params->curr_setpt_db -= ffpc_params->setpt_adj_dn_dbq20;
         if (ffpc_params->curr_setpt_db < ((int32)ffpc_params->min_setpt_db << 17))
         {
            ffpc_params->curr_setpt_db = (int32)ffpc_params->min_setpt_db << 17;
         }
      }

      /* update adjusted set point */
      ffpc_params->setpt_adjusted_dbq20 = 
        ffpc_params->curr_setpt_db +   
        ( (ffpc_params->setpt_adjust_dbq4 + ffpc_params->setpt_offset_dbq4)
          << 16 );

      /* Only write setpoint for primary channel or SCH. */
      if ( (channel == ffpc_info.fpc_pri_chan)
          || (channel == FFPC_SCH) )
      {
         hw_setpt = ffpc_get_hw_setpt (ffpc_params->setpt_adjusted_dbq20);
         MSG_LOW( "SP dB: 0x%08x. SPA dB: 0x%08x. SPD lin: 0x%08x.",
                   ffpc_params->curr_setpt_db,
                   ffpc_params->setpt_adjusted_dbq20,
                   hw_setpt );

         ffpc_program_setpt (hw_setpt, channel);
      }
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_SCALE_FER

DESCRIPTION
  This function converts the TARGET_FER value into a linear scale. The
  conversion is from the signalling format (IS-2000.5-A-2 table 3.7.3.3.2.25-2)
  to FER in units of 0.001 (thousandths).

DEPENDENCIE
  None.

RETURN VALUE
  FER*1000

SIDE EFFECTS
  None.
=========================================================================*/
word
ffpc_scale_fer
(
 byte  fer_bin
)
{
   word   fer_scaled = 0;

   if (fer_bin == 0)
   {
      fer_scaled = 2;
   }
   else if ( (fer_bin > 0 ) && (fer_bin <= 20) )
   {
      fer_scaled = fer_bin * 5;
   }
   else if ( (fer_bin > 20) && (fer_bin <= 25) )
   {
      fer_scaled = (fer_bin - 10) * 10;
   }
   else if ( (fer_bin > 25) && (fer_bin <= 30) )
   {
      fer_scaled = ((fer_bin - 26) * 30) + 180;
   }
   else
   {
      MSG_ERROR("Bad val for TARGET_FER %d spec", fer_bin, 0, 0);
   }

   return fer_scaled;
}

/* <EJECT> */
/*===========================================================================

FUNCTION FFPC_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated FFPC frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A FFPC log may be sent.

===========================================================================*/
void
ffpc_send_log(void)
{
  if (ffpc_log_buf.hdr.len != 0)
  {
    /* Will send log if submitted. */
    (void) log_submit((log_type *) &ffpc_log_buf);

    /* Flag to init start of log */
    ffpc_log_buf.hdr.len = 0;
  }

} /* ffpc_send_log */


/* <EJECT> */
/*===========================================================================

FUNCTION FFPC_LOG_INIT

DESCRIPTION
  This function initializes the FFPC log subrecord, as well as the
  pointer to it and the frame entry, in the FFPC log buffer.  If necessary,
  the log is sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A FFPC log may be sent.

===========================================================================*/
log_ffwd_pctrl_frame_type *
ffpc_log_frame_init
(
  byte  frame_offset, /* Frame offset in 1.25 ms units */
  word  so,           /* Service Option                */
  boolean log_fch,
  boolean log_dcch,
  boolean log_sch
)
{
  log_ffwd_pctrl_frame_type *frm_ptr = NULL;  /* Ptr to frame entry */
  log_ded_fwd_chan_set_type chan_set_mask;

  /* Is logging enabled? */
  if (log_status(LOG_FFWD_PCTRL_C)
#ifdef FEATURE_LOG_PC_INFO_2_EXCLUSIVE
      /* The FFWD_PCTRL log message is a subset of the FFPC_INFO log */
      /* message, so only log it if FFPC_INFO is not being logged.   */
      && !log_status(LOG_FFPC_INFO_C)
#endif /* FEATURE_LOG_PC_INFO_2_EXCLUSIVE */
     )
  {
    /* If we reached the logging threshold for FFPC, send it. */
    if (ffpc_log_buf.hdr.len >= LOG_FFPC_BUF_THRESHOLD) //lint !e413 FPOS
    {
      ffpc_send_log();
    }

    if (ffpc_log_buf.hdr.len == 0) /* Need to re-init */
    {
      ffpc_log_buf.hdr.len = (word) FPOS(log_ffwd_pctrl_type, var_len_buf); //lint !e413 FPOS

      ffpc_log_buf.hdr.code = LOG_FFWD_PCTRL_C;

      /* First frame, log time stamp. */
      ts_get((unsigned long *) ffpc_log_buf.hdr.ts);

      /*------------------------------
        Fill in beginning of record
      ------------------------------*/
      /* Frame offset specified by base station */
      ffpc_log_buf.frame_offset = frame_offset;

      /* Service Option */
      ffpc_log_buf.so = so;

      /* Operating mode */
      ffpc_log_buf.fpc_mode = (byte) ffpc_info.fpc_mode;

      /* FFPC Primary Channel */
      ffpc_log_buf.fpc_pri_chan = ffpc_info.fpc_pri_chan;

      /* Sub-channel gain in 0.25 dB units*/
      ffpc_log_buf.fpc_subchan_gain = ffpc_info.fpc_subchan_gain_db;

      /* We no longer have a valid sub-record. */
      ffpc_log_buf.sr_cnt = 0;
      ffpc_log_sr_ptr     = NULL;
    }
    else if (ffpc_log_buf.fpc_mode != FPC_MODE_PRI_EIB
#ifdef FEATURE_IS2000_REL_A
     && (ffpc_info.fpc_mode != FPC_MODE_PRI_QIB)
     && (ffpc_info.fpc_mode != FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
    )
    {
      /* If a new subrecord is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      chan_set_mask.mask = 0;

      /* Check if FCH is active */
      /* NOTE: FCH is always active until DCCH */
      chan_set_mask.chan_set.f_fch = (log_fch) ? TRUE : FALSE;

#ifdef FEATURE_IS2000_P2
      /* Check if DCCH is active */
      chan_set_mask.chan_set.f_dcch = (log_dcch) ? TRUE : FALSE;
#endif /* FEATURE_IS2000_P2 */

      /* Check if SCH is active. */
      chan_set_mask.chan_set.f_sch0 = (log_sch) ? TRUE : FALSE;

      if ( (ffpc_log_sr_ptr != NULL) &&
           (chan_set_mask.mask != ffpc_log_sr_ptr->ch_set_mask.mask) )
      {
        /* We no longer have a valid sub-record. */
        ffpc_log_sr_ptr = NULL;
      }
    }

    /* for FPC_MODE using only EIB and or QIB, the inner loop decision history data
    ** (FPC_HISTORY register) is invalid and the outer loop SETPT is static,
    ** there is no need to create the new sub-records. Just send the header
    ** every FFPC_FPC_MODE_EIB_LOGGING_INTERVAL frames
    */
    if (ffpc_log_buf.fpc_mode == FPC_MODE_PRI_EIB
#ifdef FEATURE_IS2000_REL_A
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB)
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
    )
    {
      if ((fpc_mode3_frame_counter % FFPC_FPC_MODE_EIB_LOGGING_INTERVAL) == 0)
      {
        ffpc_send_log();
        fpc_mode3_frame_counter = 0;
      }
      fpc_mode3_frame_counter++;

      /* since there is no need to creating sub-record, the return pointer
      ** should be to NULL so that ffpc_log_frame will stop its execution
      ** after calling this function
      */
      return (NULL);
    }

    /* Need to init a new subrecord? */
    if (ffpc_log_sr_ptr == NULL)
    {
      /* Init sub-record pointer by finding the offset into the variable length buf.
      ** FFPC_LOG_OFFSET is calculated from the length of the log packet.
      */
      ffpc_log_sr_ptr = (log_ffwd_pctrl_subrecord_type *)
                          &ffpc_log_buf.var_len_buf.buf_ref[FFPC_LOG_OFFSET]; //lint !e413 FPOS

      /* Increment sub-record count. */
      ffpc_log_buf.sr_cnt++;

      /* Init all bits in mask, and len_per_frame */
      ffpc_log_sr_ptr->ch_set_mask.mask = 0;
      ffpc_log_sr_ptr->len_per_frame = (byte) FPOS(log_ffwd_pctrl_frame_type, setpts);

      if (log_fch)
      {
        ffpc_log_sr_ptr->ch_set_mask.chan_set.f_fch = TRUE;

        /* Increase length per frame value. */
        ffpc_log_sr_ptr->len_per_frame +=
          FSIZ(log_ffwd_pctrl_frame_type, setpts[0]);
      }

#ifdef FEATURE_IS2000_P2
      if (log_dcch)
      {
        ffpc_log_sr_ptr->ch_set_mask.chan_set.f_dcch = TRUE;

        /* Increase length per frame value. */
        ffpc_log_sr_ptr->len_per_frame +=
          FSIZ(log_ffwd_pctrl_frame_type, setpts[0]);
      }
#endif /* FEATURE_IS2000_P2 */

      if (log_sch)
      {
        ffpc_log_sr_ptr->ch_set_mask.chan_set.f_sch0 = TRUE;

        /* Increase length per frame value.
        ** Note: if SCH is active, two entries are
        ** required to include delta setpoint value
        */
        ffpc_log_sr_ptr->len_per_frame +=
          2 * FSIZ(log_ffwd_pctrl_frame_type, setpts[0]);
      }

      /* Init frame count */
      ffpc_log_sr_ptr->frame_cnt = 0;

      /* Update packet length */
      ffpc_log_buf.hdr.len += (word) FPOS(log_ffwd_pctrl_subrecord_type, var_len_buf); //lint !e413 FPOS

    } /* If new sub-record */

    /* Increment frame count */
    ffpc_log_sr_ptr->frame_cnt++;

    /* Init frame pointer by finding the offset into the variable length buf.
    ** FFPC_LOG_OFFSET is calculated from the length of the log packet.
    */
    frm_ptr = (log_ffwd_pctrl_frame_type *)
                 &ffpc_log_buf.var_len_buf.buf_ref[FFPC_LOG_OFFSET]; //lint !e413 FPOS
  }
  else
  {
    ffpc_log_buf.hdr.len = 0; /* Flag that this record has stopped */
  }

  return frm_ptr;

} /* ffpc_log_frame_init */


/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_LOG_FRAME

DESCRIPTION
  This function populates FFPC frame logs.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_log_frame
(
  byte  frame_offset, /* Frame offset in 1.25 ms units. */
  word  so,           /* Service Option                 */
  word  dec_history,  /* FFPC Decision History          */
  boolean log_fch,
  boolean log_dcch,
  boolean log_sch
)
{
  unsigned int idx; /* Setpoint array index */
  log_ffwd_pctrl_frame_type *frm_ptr;

  frm_ptr = ffpc_log_frame_init(frame_offset, so, log_fch, log_dcch, log_sch);

  if (frm_ptr != NULL)
  {
    /* Record history */
    frm_ptr->dec_history = dec_history;

    /* Start recording per channel data */
    idx = 0; /* Initialize setpoint array index */

    if (ffpc_log_sr_ptr->ch_set_mask.chan_set.f_fch == TRUE)
    {
      frm_ptr->setpts[idx++].f_fch = (byte) (ffpc_fch_params.curr_setpt_db >> 17);
    }

    if (ffpc_log_sr_ptr->ch_set_mask.chan_set.f_sch0 == TRUE)
    {
      frm_ptr->setpts[idx++].f_sch0 =
                 (byte) (ffpc_sch_params.curr_setpt_db >> 17);
    }

#ifdef FEATURE_IS2000_P2
    if (ffpc_log_sr_ptr->ch_set_mask.chan_set.f_dcch == TRUE)
    {
      frm_ptr->setpts[idx++].f_dcch =
                 (byte) (ffpc_dcch_params.curr_setpt_db >> 17);
    }
#endif /* FEATURE_IS2000_P2 */

    if (ffpc_log_sr_ptr->ch_set_mask.chan_set.f_sch0 == TRUE)
    {
      int8 delta = 0;

      if (ffpc_info.fpc_pri_chan == FFPC_FCH)
      {
         delta = (int8) ((ffpc_fch_params.curr_setpt_db >> 17) -
                         (ffpc_sch_params.curr_setpt_db >> 17));
      }
#ifdef FEATURE_IS2000_P2
      else
      {
         delta = (int8) ((ffpc_dcch_params.curr_setpt_db >> 17) -
                         (ffpc_sch_params.curr_setpt_db >> 17));
      }
#endif /* FEATURE_IS2000_P2 */

      frm_ptr->setpts[idx++].f_sch_delta = delta;
    }

    /* TODO: Add other channels here! */

    /* end per channel data */

    /* Update packet length */
    ffpc_log_buf.hdr.len += ffpc_log_sr_ptr->len_per_frame;

  } /* If logging enabled */
} /* ffpc_log_frame */

#ifdef FEATURE_LOG_PC_INFO_2

/* <EJECT> */
/*===========================================================================

FUNCTION FFPC_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated FFPC Info log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A FFPC log may be sent.

===========================================================================*/
void
ffpc_info_send_log(void)
{
  if (ffpc_info_log_length != 0)
  {
    /* Will send log if submitted. */
    log_set_length (&ffpc_info_log_buf, ffpc_info_log_length);
    (void) log_submit((log_type *) &ffpc_info_log_buf);

    /* Flag to init start of log */
    ffpc_info_log_length = 0;
  }

} /* ffpc_info_send_log */

/* <EJECT> */
/*===========================================================================

FUNCTION FFPC_INFO_LOG_INIT

DESCRIPTION
  This function initializes the FFPC log subrecord, as well as the
  pointer to it and the frame entry, in the FFPC log buffer.  If necessary,
  the log is sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A FFPC log may be sent.

===========================================================================*/
ffpc_info_log_frame_type *
ffpc_info_log_init
(
  byte                  frame_offset, /* Frame offset in 1.25 ms units */
  word                  so,           /* Service Option                */
  boolean               log_fch,
  cai_radio_config_type fch_rc,       /* FCH Radio Configuration */
  boolean               log_dcch,
  cai_radio_config_type dcch_rc,      /* DCCH Radio Configuration */
  boolean               log_sch,
  cai_radio_config_type sch_rc        /* SCH Radio Configuration */
)
{
  ffpc_info_log_frame_type *frm_ptr = NULL;  /* Ptr to frame entry */
  log_ded_fwd_chan_set_type chan_set_mask;

  /* Is logging enabled? */
  if (log_status(LOG_FFPC_INFO_C))
  {
    /* If we reached the logging threshold for FFPC, send it. */
    if (ffpc_info_log_length >= FFPC_INFO_LOG_BUF_THRESHOLD)
    {
      ffpc_info_send_log();
    }

    if (ffpc_info_log_length == 0) /* Need to re-init */
    {
      ffpc_info_log_length = (word) FPOS(ffpc_info_log_type, data_buf);

      /* Set header info (except length which is set later */
      log_set_code      (&ffpc_info_log_buf, LOG_FFPC_INFO_C);
      log_set_timestamp (&ffpc_info_log_buf);

      /*------------------------------
        Fill in beginning of record
      ------------------------------*/
      /* Channel Radio Configurations */
      ffpc_info_log_buf.fwd_ch_rc.f_fch_rc  = log_fch  ? fch_rc  : 0;
      ffpc_info_log_buf.fwd_ch_rc.f_dcch_rc = log_dcch ? dcch_rc : 0;
      ffpc_info_log_buf.fwd_ch_rc.f_sch0_rc = sch_rc;
      ffpc_info_log_buf.fwd_ch_rc.f_sch1_rc = 0; /* not yet supported */

      /* Service Option */
      ffpc_info_log_buf.so = so;

      /* Frame offset specified by base station */
      ffpc_info_log_buf.frame_offset = frame_offset;

      /* Operating mode */
      ffpc_info_log_buf.fpc_mode = (byte) ffpc_info.fpc_mode;

      /* FFPC Primary Channel */
      ffpc_info_log_buf.fpc_pri_chan = ffpc_info.fpc_pri_chan;

      /* Sub-channel gain in 0.25 dB units*/
      ffpc_info_log_buf.fpc_subchan_gain = ffpc_info.fpc_subchan_gain_db;

      /* We no longer have a valid sub-record. */
      ffpc_info_log_sr_ptr = NULL;
    }
    else if (ffpc_info_log_buf.fpc_mode != FPC_MODE_PRI_EIB
#ifdef FEATURE_IS2000_REL_A
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB)
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
    )
    {
      /* If a new subrecord is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      chan_set_mask.mask = 0;

      /* Check if FCH is active */
      /* NOTE: FCH is always active until DCCH */
      chan_set_mask.chan_set.f_fch = (log_fch) ? TRUE : FALSE;

#ifdef FEATURE_IS2000_P2
      /* Check if DCCH is active */
      chan_set_mask.chan_set.f_dcch = (log_dcch) ? TRUE : FALSE;
#endif /* FEATURE_IS2000_P2 */

      /* Check if SCH is active. */
      chan_set_mask.chan_set.f_sch0 = (log_sch) ? TRUE : FALSE;
      if (ffpc_info_log_sr_ptr != NULL)
      {
        if (chan_set_mask.mask != ffpc_info_log_sr_ptr->ch_set_mask.mask)
        {
          /* We no longer have a valid sub-record. */
          ffpc_info_log_sr_ptr = NULL;
        }
      }
    }

    /* for FPC_MODE using only EIB and or QIB, the inner loop decision history data
    ** (FPC_HISTORY register) is invalid and the outer loop SETPT is static,
    ** there is no need to create the new sub-records. Just send the header
    ** every FFPC_FPC_MODE_EIB_LOGGING_INTERVAL frames
    */
    if (ffpc_info_log_buf.fpc_mode == FPC_MODE_PRI_EIB
#ifdef FEATURE_IS2000_REL_A
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB)
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
    )
    {
      if ((fpc_mode3_frame_counter % FFPC_FPC_MODE_EIB_LOGGING_INTERVAL) == 0)
      {
        ffpc_info_send_log();
        fpc_mode3_frame_counter = 0;
      }
      fpc_mode3_frame_counter++;

      /* since there is no need to creating sub-record, the return pointer
      ** should be to NULL so that ffpc_log_frame will stop its execution
      ** after calling this function
      */
      return (NULL);
    }

    /* Need to init a new subrecord? */
    if (ffpc_info_log_sr_ptr == NULL)
    {
      /* Init sub-record pointer by finding the offset into the variable length buf.
      ** FFPC_LOG_OFFSET is calculated from the length of the log packet.
      */
      ffpc_info_log_sr_ptr = (ffpc_info_log_subrecord_type *)
                    &ffpc_info_log_buf.data_buf
                            [FFPC_INFO_LOG_OFFSET(ffpc_info_log_length)];

      /* Init all bits in mask, and len_per_frame */
      ffpc_info_log_sr_ptr->ch_set_mask.mask = 0;

      if (log_fch)
      {
        ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_fch = TRUE;
      }

#ifdef FEATURE_IS2000_P2
      if (log_dcch)
      {
        ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_dcch = TRUE;
      }
#endif /* FEATURE_IS2000_P2 */

      if (log_sch)
      {
        ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_sch0 = TRUE;
      }

      /* Init frame count */
      ffpc_info_log_sr_ptr->frame_cnt = 0;

      /* Update packet length */
      ffpc_info_log_length += (word) FPOS(ffpc_info_log_subrecord_type,
                                               frame_buf);

    } /* If new sub-record */

    /* Increment frame count */
    ffpc_info_log_sr_ptr->frame_cnt++;

    /* Init frame pointer by finding the offset into the variable length buf.
    ** FFPC_INFO_LOG_OFFSET is calculated from the length of the log packet.
    */
    frm_ptr = (ffpc_info_log_frame_type *)
                 &ffpc_info_log_buf.data_buf
                            [FFPC_INFO_LOG_OFFSET(ffpc_info_log_length)];
  }
  else
  {
    ffpc_info_log_length = 0; /* Flag that this record has stopped */
  }

  return frm_ptr;

} /* ffpc_info_log_init */

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_INFO_LOG_FRAME

DESCRIPTION
  This function populates FFPC frame logs.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_info_log_frame
(
  byte                   frame_offset, /* Frame offset in 1.25 ms units. */
  word                   so,           /* Service Option                 */
  word                   dec_history,  /* FFPC Decision History          */
  boolean                log_fch,
  cai_radio_config_type  fch_rc,       /* FCH Radio Configuration */
  boolean                log_dcch,
  cai_radio_config_type  dcch_rc,      /* DCCH Radio Configuration */
  boolean                log_sch,
  cai_radio_config_type  sch_rc,       /* SCH Radio Configuration */
  int16                  frame_rate,   /* FCH  Frame rate */
  log_fch_type_enum_type fch_type,     /* FCH  Frame type */
  log_sch_rate_enum_type sch0_rate     /* SCH0 Frame type */
  #ifdef FEATURE_IS2000_P2
  , byte mux                           /* mux type for DCCH logging */
  #endif /* FEATURE_IS2000_P2 */
)
{
  unsigned int idx; /* Setpoint array index */
  ffpc_info_log_frame_type *frm_ptr;

  frm_ptr = ffpc_info_log_init(frame_offset,
                               so,
                               log_fch,
                               fch_rc,
                               log_dcch,
                               dcch_rc,
                               log_sch,
                               sch_rc);

  if (frm_ptr != NULL)
  {
    /* Record history */
    frm_ptr->dec_history = dec_history;

    /* Start recording per channel data */
    idx = 0; /* Initialize setpoint array index */

    if (ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_fch == TRUE)
    {
      frm_ptr->setpts[idx].f_fch.power = (byte)(ffpc_fch_params.curr_setpt_db >> 17);
      frm_ptr->setpts[idx].f_fch.rate  = frame_rate;
      frm_ptr->setpts[idx].f_fch.type  = fch_type;
      idx++;
    }

    if (ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_sch0 == TRUE)
    {
      frm_ptr->setpts[idx].f_sch0.power = (byte)(ffpc_sch_params.curr_setpt_db >> 17);
      frm_ptr->setpts[idx].f_sch0.rate  = sch0_rate;
      idx++;
    }

#ifdef FEATURE_IS2000_P2
    if (ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_dcch == TRUE)
    {
      frm_ptr->setpts[idx].f_dcch.power = (byte)(ffpc_dcch_params.curr_setpt_db >> 17);
      frm_ptr->setpts[idx].f_dcch.mux   = mux;
      idx++;
    }
#endif /* FEATURE_IS2000_P2 */

    if (ffpc_info_log_sr_ptr->ch_set_mask.chan_set.f_sch0 == TRUE)
    {
      int8 delta = 0;

      if (ffpc_info.fpc_pri_chan == FFPC_FCH)
      {
         delta = (int8) ((ffpc_fch_params.curr_setpt_db >> 17) -
                         (ffpc_sch_params.curr_setpt_db >> 17));
      }
#ifdef FEATURE_IS2000_P2
      else
      {
         delta = (int8) ((ffpc_dcch_params.curr_setpt_db >> 17) -
                         (ffpc_sch_params.curr_setpt_db >> 17));
      }
#endif /* FEATURE_IS2000_P2 */

      frm_ptr->setpts[idx++].f_sch.delta = delta;
    }

    /* TODO: Add other channels here! */

    /* end per channel data */

    /* Update packet length */
    ffpc_info_log_length += FPOS(ffpc_info_log_frame_type, setpts) +
                            idx * FSIZ(ffpc_info_log_frame_type, setpts[0]);

  } /* If logging enabled */
} /* ffpc_info_log_frame */
#endif /* FEATURE_LOG_PC_INFO_2 */


#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

/* <EJECT> */
#ifdef FEATURE_IS2000_SCH
/*=========================================================================
FUNCTION FFPC_PROGRAM_WALSH_GAIN

DESCRIPTION
  This function will program the WALSH_GAIN register based on the
  F-SCH Radio Configuration and Rate.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_program_walsh_gain (void)
{
  dec_sch_rate_type sch_rate;
  boolean           double_ptr;
  dec_rc_type       sch_rc;

  if (rxc_get_sch_info (&sch_rc, &sch_rate, &double_ptr))
  {
     word  walsh_gain = 0;

     switch (ffpc_sch_params.fwd_rc)
     {
        case DEINT_RC3:
        {
           switch (sch_rate)
           {
              case DEC_SCH_RATE_1X:
              {
                 walsh_gain = FFPC_RC3_1X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_2X:
              {
                 walsh_gain = FFPC_RC3_2X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_4X:
              {
                 walsh_gain = FFPC_RC3_4X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_8X:
              {
                 walsh_gain = FFPC_RC3_8X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_16X:
              {
                 walsh_gain = FFPC_RC3_16X_SCH_WALSH_GAIN;
                 break;
              }
              default:
              {
                 ERR("Bad SCH rate", 0, 0, 0);
                 break;
              }
           }
           break;
        }
        case DEINT_RC4:
        {
           switch (sch_rate)
           {
              case DEC_SCH_RATE_1X:
              {
                 walsh_gain = FFPC_RC4_1X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_2X:
              {
                 walsh_gain = FFPC_RC4_2X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_4X:
              {
                 walsh_gain = FFPC_RC4_4X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_8X:
              {
                 walsh_gain = FFPC_RC4_8X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_16X:
              {
                 walsh_gain = FFPC_RC4_16X_SCH_WALSH_GAIN;
                 break;
              }
#ifdef FEATURE_32X_SCH_DATA_RATE
              case DEC_SCH_RATE_32X:
              {
                 walsh_gain = FFPC_RC4_32X_SCH_WALSH_GAIN;
                 break;
              }
#endif // FEATURE_32X_SCH_DATA_RATE
              default:
              {
                 ERR("Bad SCH rate", 0, 0, 0);
                 break;
              }
           }
           break;
        }
        case DEINT_RC5:
        {
           switch (sch_rate)
           {
              case DEC_SCH_RATE_1X:
              {
                 walsh_gain = FFPC_RC5_1X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_2X:
              {
                 walsh_gain = FFPC_RC5_2X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_4X:
              {
                 walsh_gain = FFPC_RC5_4X_SCH_WALSH_GAIN;
                 break;
              }
              case DEC_SCH_RATE_8X:
              {
                  walsh_gain = FFPC_RC5_8X_SCH_WALSH_GAIN;
                  break;
              }
              case DEC_SCH_RATE_16X:
              {
                 walsh_gain = FFPC_RC5_16X_SCH_WALSH_GAIN;
                 break;
              }
              default:
              {
                 ERR("Bad SCH rate", 0, 0, 0);
                 break;
              }
           }
           break;
        }
        default:
        {
           ERR("Bad RC for SCH", 0, 0, 0);
        }
     }
     MUXMSM_FPC_WALSH_GAIN( walsh_gain );
  }
}

/*=========================================================================
FUNCTION FFPC_PROGRAM_IQACC_SCALE

DESCRIPTION
  This function will program the IQACC_SCALE MDSP-register based on the
  F-SCH Radio Configuration and Rate.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_program_iqacc_scale (void)
{
  dec_sch_rate_type sch_rate;
  boolean           double_ptr;
  dec_rc_type       sch_rc;

  if (rxc_get_sch_info (&sch_rc, &sch_rate, &double_ptr))
  {
     word  iq_acc_scale = 0;

     sch_rc = (dec_rc_type) ffpc_sch_params.fwd_rc;

     /*lint -e{56} Bad type conflict between rxc_get_sch_info() and DEINT_SCH_RC_INDEX */
     iq_acc_scale = ffpc_iqacc_sch_scale_tab[ DEINT_SCH_RC_INDEX(sch_rc) ][ sch_rate ];

     if ( sch_rc > DEC_RC5 )
     {
       ERR("Bad RC for SCH", 0, 0, 0);
     }

#ifdef FEATURE_32X_SCH_DATA_RATE
     if ( sch_rate > DEC_SCH_RATE_32X )
     {
       ERR("Bad SCH rate", 0, 0, 0);
     }

     if ( sch_rate == DEC_SCH_RATE_32X )
     {
       if ( (sch_rc == DEC_RC3) || (sch_rc == DEC_RC5) )
       {
         ERR("32x not supp for RC%d", sch_rc, 0, 0);
       }
     }
#else
     if ( sch_rate > DEC_SCH_RATE_16X )
     {
       ERR("Bad SCH rate", 0, 0, 0);
     }
#endif // FEATURE_32X_SCH_DATA_RATE

     MUXMSM_FPC_IQACC_SCH_SCALE( iq_acc_scale );
  }
}
#endif /* FEATURE_IS2000_SCH */

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_TRANS_CMD

DESCRIPTION
  This function handles MCTRANS_FFPC_ACTION command fro MC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_trans_cmd
(
  mctrans_action_packet_type *cmd_ptr
)
{
   mctrans_ffpc_action_type  *ffpc_ptr;
   boolean rc_invalid_for_ffpc = FALSE;

   if (cmd_ptr->action_type != MCTRANS_FFPC_ACTION)
   {
      MSG_ERROR("Bad Act Type for FFPC in MCTRANS", 0, 0, 0);
      return;
   }

   ffpc_ptr = &cmd_ptr->action.ffpc;

   /* Make sure that Secondary channel is set to F-SCH0 */
   if (ffpc_ptr->sch.fpc_sec_chan.included && ffpc_ptr->sch.fpc_sec_chan.value != 0)
   {
      MSG_ERROR("FPC_SEC_CHAN not set to F-SCH0", 0, 0, 0);

      /* disable SCH Outer Loop */
      ffpc_disable_chan_olpc (FFPC_SCH);

      return;
   }

   if ((ffpc_ptr->fch_ffpc_op.mode  == MCTRANS_FFPC_STOP) ||
#ifdef FEATURE_IS2000_P2
       (ffpc_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_STOP) ||
#endif /* FEATURE_IS2000_P2 */
       (ffpc_ptr->sch_ffpc_op.mode  == MCTRANS_FFPC_STOP))
   {
      /* do not disable OLPC, must wait for another frame to disable */
   }

   /* update FPC_PRI_CHAN */
   if (ffpc_ptr->fpc_pri_chan.included)
   {
      if (ffpc_ptr->fpc_pri_chan.value == CAI_FPC_PRI_CHAN_FCH)
      {
         ffpc_info.fpc_pri_chan = FFPC_FCH;
      }
#ifdef FEATURE_IS2000_P2
      else
      {
         ffpc_info.fpc_pri_chan = FFPC_DCCH;
      }
#endif /* FEATURE_IS2000_P2 */
   }

   /* process FPC_SUBCHAN_GAIN value */
   if (ffpc_ptr->fpc_subchan_gain.included)
   {
      ffpc_init_subchan_gain (ffpc_ptr->fpc_subchan_gain.value);
   }

   /* process FPC_MODE */
   if (ffpc_ptr->fpc_mode.included)
   {
      ffpc_info.fpc_mode = (fpc_mode_type) ffpc_ptr->fpc_mode.value;
   }

   /* Initialize F-FCH, F-SCH FFPC params received. */
   ffpc_fch_init_params  (ffpc_ptr);
   ffpc_sch_init_params  (ffpc_ptr);

#ifdef FEATURE_IS2000_P2
   /* Initialize F-DCCH FFPC params received. */
   ffpc_dcch_init_params (ffpc_ptr);
#endif /* FEATURE_IS2000_P2 */

   /* Check if there is anything that will disallow FFPC */
   if ((((deint_rc_type)ffpc_ptr->fch_ffpc_op.rc == DEINT_RC1) ||
       ((deint_rc_type)ffpc_ptr->fch_ffpc_op.rc == DEINT_RC2)) &&
       (ffpc_ptr->fch_ffpc_op.mode != MCTRANS_FFPC_STOP))
   {
     /* RC1 & RC2 always disallow FFPC 
        Except when this is a FFPC_STOP action */
     rc_invalid_for_ffpc = TRUE;
   }
   else if (((deint_rc_type)ffpc_ptr->fch_ffpc_op.rc == DEINT_RC_INVALID) &&
            ((ffpc_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_START) ||
             ((ffpc_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
              (ffpc_fch_params.olpc_enabled == TRUE))))
   {
     /* If RC is ambiguous/unspecified, either one of the following 
        will disallow FFPC:
        1) If this is an FFPC_START action
        2) If this is an FFPC_CONTINUE action, and olpc is already enabled */
     rc_invalid_for_ffpc = TRUE;
   }

#ifdef FEATURE_IS2000_P2
  /* Check if there is anything that will disallow FFPC */
   if ((((deint_rc_type)ffpc_ptr->dcch_ffpc_op.rc == DEINT_RC1) ||
       ((deint_rc_type)ffpc_ptr->dcch_ffpc_op.rc == DEINT_RC2)) &&
       (ffpc_ptr->dcch_ffpc_op.mode != MCTRANS_FFPC_STOP))
   {
     /* RC1 & RC2 always disallow FFPC 
        Except when this is a FFPC_STOP action */
     rc_invalid_for_ffpc = TRUE;
   }
   else if (((deint_rc_type)ffpc_ptr->dcch_ffpc_op.rc == DEINT_RC_INVALID) &&
            ((ffpc_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_START) ||
             ((ffpc_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
              (ffpc_dcch_params.olpc_enabled == TRUE))))
   {
     /* If RC is ambiguous/unspecified, either one of the following 
        will disallow FFPC:
        1) If this is an FFPC_START action
        2) If this is an FFPC_CONTINUE action, and olpc is already enabled */
     rc_invalid_for_ffpc = TRUE;
   }
#endif /* FEATURE_IS2000_P2 */

  /* Check if there is anything that will disallow FFPC */
   if ((((deint_rc_type)ffpc_ptr->sch_ffpc_op.rc == DEINT_RC1) ||
       ((deint_rc_type)ffpc_ptr->sch_ffpc_op.rc == DEINT_RC2)) &&
       (ffpc_ptr->sch_ffpc_op.mode != MCTRANS_FFPC_STOP))
   {
     /* RC1 & RC2 always disallow FFPC 
        Except when this is a FFPC_STOP action */
     rc_invalid_for_ffpc = TRUE;
   }
   else if (((deint_rc_type)ffpc_ptr->sch_ffpc_op.rc == DEINT_RC_INVALID) &&
            ((ffpc_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_START) ||
             ((ffpc_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
              (ffpc_sch_params.olpc_enabled == TRUE))))
   {
     /* If RC is ambiguous/unspecified, either one of the following 
        will disallow FFPC:
        1) If this is an FFPC_START action
        2) If this is an FFPC_CONTINUE action, and olpc is already enabled */
     rc_invalid_for_ffpc = TRUE;
   }

   /* Validity of RCs on forward channels for FFPC established,
   ** update SCRH, TXC with FPC_MODE based on RC validity and FPC_MODE.
   */
   ffpc_update_fpc_mode (rc_invalid_for_ffpc, ffpc_info.fpc_mode);

   /* if RC invalid for FFPC, or EIB and or QIB only, disable FFPC and return */
   if (rc_invalid_for_ffpc || (ffpc_info.fpc_mode == FPC_MODE_PRI_EIB)
#ifdef FEATURE_IS2000_REL_A
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB)
     || (ffpc_info.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
   )
   {
      ffpc_disable ();
      return;
   }

   /*
   ** All parameters have been initialized. RC, Primary channel and
   ** FPC_MODE have been validated. Proceed to compute SETPTs based
   ** on the new parameters.
   */
   /* init setpts if new assignment or if the channel is already up */
   if ((ffpc_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_START) ||
       ((ffpc_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        (ffpc_fch_params.olpc_enabled == TRUE)) ||
       ((ffpc_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        ((deint_rc_type) ffpc_ptr->fch_ffpc_op.rc > DEINT_RC2)))
   {
      ffpc_init_setpts (FFPC_FCH);

      if (!ffpc_fch_params.olpc_enabled)
      {
         /* enable Outer Loop */
         ffpc_fch_params.olpc_enabled = TRUE;

         /*
         ** MS may be transitioning from RC1/RC2 to RC3/RC4 or
         ** FPC_MODE may have changed from FPC_MODE_PRI_EIB to FPC_MODE=0/1/2
         ** in which case FFPC was not enabled in HW earlier. Enable FFPC now.
         */
         ffpc_program_hw ();
      }
   }

#ifdef FEATURE_IS2000_P2
   /* init setpts if new assignment or if the channel is already up */
   if ((ffpc_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_START) ||
       ((ffpc_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        (ffpc_dcch_params.olpc_enabled == TRUE)))
   {
      ffpc_init_setpts (FFPC_DCCH);

      /* enable Outer Loop : does'nt hurt if already enabled. */
      ffpc_dcch_params.olpc_enabled = TRUE;
   }
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_SCH
   if (ffpc_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_START)
   {
      /* Program WALSH_GAIN values in HW for F-SCH */
      ffpc_program_walsh_gain ();

      /* Program the IQACC_SCALE values in the MDSP for F-SCH */
      ffpc_program_iqacc_scale();
   }
#endif /* FEATURE_IS2000_SCH */

   /* init setpts if new assignment or if the channel is already up */
   if ((ffpc_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_START) ||
       ((ffpc_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        (ffpc_sch_params.olpc_enabled == TRUE)))
   {
      ffpc_init_setpts (FFPC_SCH);

      /* enable Outer Loop : does'nt hurt if already enabled. */
      ffpc_sch_params.olpc_enabled = TRUE;
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_FCH_INIT_PARAMS

DESCRIPTION
  This function handles F-FCH action from the FFPC transaction from MC

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_fch_init_params
(
 mctrans_ffpc_action_type *cmd_ptr
)
{
   if ((cmd_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_START) ||
       ((cmd_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        (ffpc_fch_params.olpc_enabled == TRUE)) ||
       ((cmd_ptr->fch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        ((deint_rc_type) cmd_ptr->fch_ffpc_op.rc > DEINT_RC2)))
   {
      ffpc_init_rc ((deint_rc_type) cmd_ptr->fch_ffpc_op.rc, FFPC_FCH);
   }

   /* process F-FCH Outer Loop FFPC parameters */
   if (cmd_ptr->fch_olpc.included)
   {
      ffpc_init_outer_loop_params (&cmd_ptr->fch_olpc.param, FFPC_FCH);
   }
}

#ifdef FEATURE_IS2000_P2
/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_DCCH_INIT_PARAMS

DESCRIPTION
  This function handles F-DCCH action from the FFPC transaction from MC

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_dcch_init_params
(
 mctrans_ffpc_action_type *cmd_ptr
)
{
   if ((cmd_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_START) ||
       ((cmd_ptr->dcch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        (ffpc_dcch_params.olpc_enabled == TRUE)))
   {
      ffpc_init_rc ((deint_rc_type) cmd_ptr->dcch_ffpc_op.rc, FFPC_DCCH);
   }

   /* process F-DCCH Outer Loop FFPC parameters */
   if (cmd_ptr->dcch_olpc.included)
   {
      ffpc_init_outer_loop_params (&cmd_ptr->dcch_olpc.param, FFPC_DCCH);
   }
}
#endif /* FEATURE_IS2000_P2 */

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_SCH_INIT_PARAMS

DESCRIPTION
  This function will initialize FFPC configuration parameters for F-SCH

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_sch_init_params
(
  mctrans_ffpc_action_type *cmd_ptr
)
{
   ffpc_chan_info_type  *ffpc_params = &ffpc_sch_params;

   if ((cmd_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_START) ||
       ((cmd_ptr->sch_ffpc_op.mode == MCTRANS_FFPC_CONTINUE) &&
        (ffpc_sch_params.olpc_enabled == TRUE)))
   {
      ffpc_init_rc ((deint_rc_type) cmd_ptr->sch_ffpc_op.rc, FFPC_SCH);
   }

   /* Process F-SCH Outer Loop FFPC parameters */
   if (cmd_ptr->sch.included)
   {
      if (cmd_ptr->sch.sch_olpc[0].included)
      {
         byte  min_setpt = cmd_ptr->sch.sch_olpc[0].param.min_setpt.value;
         byte  max_setpt = cmd_ptr->sch.sch_olpc[0].param.max_setpt.value;

         if ( cmd_ptr->sch.sch_olpc[0].init_setpt.included )
         {
            ffpc_chan_info_type  *pri_chan_params;

            pri_chan_params = ffpc_get_param_ptr(ffpc_info.fpc_pri_chan);

            ffpc_params->incl_status |= FFPC_INIT_SETPT_INCL;

            /*
            ** New Init Setpt received.
            ** This will be effective at the next frame boundary. No need
            ** to update the SCH_CURR_SETPT in this frame as from next
            ** frame onwards the new init setpt will be used.
            ** Also, no need to update the SCH_CURR_SETPT in next frame as
            ** the SCH decoder ISR in the next frame corresponds to the last
            ** SCH frame with old setpt.
            ** Hence SCH_CURR_SETPT is not updated for 2 consecutive frames.
            */
            if (ffpc_params->olpc_enabled)
            {
               ffpc_do_not_update_sch_setpt_counter = 2;
            }

            /*
            ** store INIT_SETPT and INIT delta between FPC_FCH_CURR_SETPT,
            ** FPC_SCH_CURR_SETPT for threshold crossing determination
            */
            if (cmd_ptr->sch.fpc_sch_init_setpt_op)
            {
               /* FPC_SCH_INIT_SETPT is signed 2's compliment offset value
               */
               int16 init_setpt_db;
               int8  init_setpt_offset =
                          (int8) cmd_ptr->sch.sch_olpc[0].init_setpt.value;

               /*
               ** SCH Inner Loop SETPT is relative to CURR_SETPT of FPC_PRI_CHAN
               */
               init_setpt_db =
                      (int16) ((word) (pri_chan_params->curr_setpt_db >> 17) +
                               (int16) init_setpt_offset);

               /* make sure MIN_SETPT <= CURR_SETPT <= MAX_SETPT.
               */
               if (init_setpt_db < min_setpt)
               {
                  ffpc_params->init_setpt_db = min_setpt;
               }
               else if (init_setpt_db > max_setpt)
               {
                  ffpc_params->init_setpt_db = max_setpt;
               }
               else
               {
                  ffpc_params->init_setpt_db = (byte) init_setpt_db;
               }

               ffpc_info.pri_sch_init_delta = -(init_setpt_offset);
            }
            else
            {
               byte init_setpt = cmd_ptr->sch.sch_olpc[0].init_setpt.value;

               /* make sure MIN_SETPT <= CURR_SETPT <= MAX_SETPT.
               */
               if (init_setpt < min_setpt)
               {
                  ffpc_params->init_setpt_db = min_setpt;
               }
               else if (init_setpt > max_setpt)
               {
                  ffpc_params->init_setpt_db = max_setpt;
               }
               else
               {
                  ffpc_params->init_setpt_db = init_setpt;
               }

               ffpc_info.pri_sch_init_delta =
                    (pri_chan_params->curr_setpt_db >> 17) -
                    ffpc_params->init_setpt_db;
            }

            /* store scaled FPC_SCH_CURR_SETPT */
            ffpc_params->curr_setpt_db = ffpc_params->init_setpt_db << 17;
         }

         /* Initialize F-SCH Outer Loop params */
         ffpc_init_outer_loop_params (&cmd_ptr->sch.sch_olpc[0].param, FFPC_SCH);
      }

      if (cmd_ptr->sch.fpc_thresh_sch.included)
      {
         /* SETPT threshold value included to trigger OLRM message */
         ffpc_info.setpt_thresh_init = TRUE;
         ffpc_info.sch_thresh = cmd_ptr->sch.fpc_thresh_sch.value;
      }
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_INIT_OUTER_LOOP_PARAMS

DESCRIPTION
  This function will initialize Outer Loop FFPC parameters.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_init_outer_loop_params
(
 mctrans_fpc_olpc_param_type *olpc_ptr,
 ffpc_chan_type channel
)
{
   ffpc_chan_info_type  *ffpc_params;
   byte min_setpt, max_setpt, target_fer;

   ffpc_params = ffpc_get_param_ptr(channel);

   min_setpt  = olpc_ptr->min_setpt.value;
   max_setpt  = olpc_ptr->max_setpt.value;
   target_fer = olpc_ptr->target_fer;

   /* set OLPC_INCL to TRUE */
   ffpc_params->incl_status |= FFPC_OLPC_INCL;

   /*  cache target FER  */
   ffpc_params->target_fer = target_fer;

   /* scale target FER */
   ffpc_params->fer_scaled = ffpc_scale_fer (target_fer);

   ffpc_params->setpt_adj_up_dbq20 = DEFAULT_STEPSIZE_DBQ20;
   ffpc_params->setpt_adj_dn_dbq20 = (ffpc_params->fer_scaled *
                                      DEFAULT_STEPSIZE_DBQ20) /
                                     (1000 - ffpc_params->fer_scaled);

   /* clear any asserted offset to the set point */
   ffpc_params->setpt_offset_dbq4  = 0;

   /*
   ** MIN_SETPT <= MAX_SETPT has already been verified in L3,
   ** make sure MIN_SETPT <= CURR_SETPT <= MAX_SETPT.
   **
   ** If MIN_SETPT and/or MAX_SETPT ="11111111" (set_to_current == TRUE)
   ** set MIN_SETPT/MAX_SETPT to CURR_SETPT.
   */

   /* initialize MIN_SETPT */
   if (olpc_ptr->min_setpt.set_to_current)
   {
      /* handle the special value for MIN_SETPT */
      ffpc_params->min_setpt_db = (byte) (ffpc_params->curr_setpt_db >> 17);
   }
   else if (ffpc_params->min_setpt_db != min_setpt)
   {
      ffpc_params->min_setpt_db = min_setpt;

      /* make sure CURR_SETPT >= MIN_SETPT */
      if ((ffpc_params->curr_setpt_db >> 17) < min_setpt)
      {
         ffpc_params->curr_setpt_db = min_setpt << 17;
      }
   }

   /* initialize MAX_SETPT */
   if (olpc_ptr->max_setpt.set_to_current)
   {
      /* handle the special value for MAX_SETPT */
      ffpc_params->max_setpt_db = (byte) (ffpc_params->curr_setpt_db >> 17);
   }
   else if (ffpc_params->max_setpt_db != max_setpt)
   {
      ffpc_params->max_setpt_db = max_setpt;

      /* make sure CURR_SETPT <= MAX_SETPT */
      if ((ffpc_params->curr_setpt_db >> 17) > max_setpt)
      {
         ffpc_params->curr_setpt_db = max_setpt << 17;
      }
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_INIT_SETPTS

DESCRIPTION
  This function will initialize FFPC SETPTs for F-FCH and F-SCH

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_init_setpts
(
 ffpc_chan_type channel
)
{
   ffpc_chan_info_type  *ffpc_params;

   ffpc_params = ffpc_get_param_ptr(channel);

   if ((ffpc_params->incl_status & FFPC_SUBCHAN_GAIN_M) ||
       (ffpc_params->incl_status & FFPC_INIT_SETPT_M)   ||
       (ffpc_params->incl_status & FFPC_FWD_RC_M)       ||
       (ffpc_params->incl_status & FFPC_OLPC_M))
   {
      dword  curr_setpt_db = 0;
      word   hw_setpt = 0;

      /*
      ** Already made sure that MIN_SETPT <= CURR_SETPT <= MAX_SETPT
      ** Go ahead and compute new CURR_SETPT.
      */
      curr_setpt_db = ffpc_params->curr_setpt_db >> 17;
      ffpc_params->curr_setpt_db = (int32) (curr_setpt_db << 17);

      /* modify adjustment to conform to MSM6500 set point interface */
      ffpc_params->setpt_adjust_dbq4 = SETPT_ADJUST_PI_ON_TWO_DBQ4;

      /* clear any asserted offset to the set point */
      ffpc_params->setpt_offset_dbq4  = 0;

      if ((channel == FFPC_FCH) || (channel == FFPC_DCCH))
      {
         /* apply adjustments for using PC subchannel to estimate Eb */
         /* adjust for subchannel gain, converting q2 to q4 */
         ffpc_params->setpt_adjust_dbq4 += ffpc_info.fpc_subchan_gain_db << 2;
         if (ffpc_params->fwd_rc == DEINT_RC5)
         {
            /* multiply by 3/2 to adjust for the 1.5 FCH info bits per PC
               subchannel bit duration for RC5 */
            ffpc_params->setpt_adjust_dbq4 += SETPT_ADJUST_RC5_DBQ4;
         }
      }

      /* update adjusted set point */
      ffpc_params->setpt_adjusted_dbq20 = 
        ffpc_params->curr_setpt_db +   
        ( (ffpc_params->setpt_adjust_dbq4 + ffpc_params->setpt_offset_dbq4)
          << 16 );

      /* compute the HW value for the adjusted set point */
      hw_setpt = ffpc_get_hw_setpt (ffpc_params->setpt_adjusted_dbq20);

      /* program the scaled setpt value in HW */
      ffpc_program_setpt (hw_setpt, channel);
      /*
      ** reset incl_status to 0. BS may schedule another SCH assignment
      ** with new FFPC parameters by an ESCAM or send new SCH FFPC
      ** parameters during the current assignment by a PCM. incl_status
      ** will then indicate only the new parameters received.
      ** This may help optimize re-computation of all setpts.
      */
      ffpc_params->incl_status = 0x00;
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_CHECK_SETPT_THRESH

DESCRIPTION
  This function will compute the current delta between FCH and SCH current
  setpoints and check if it exceeds the threshold specified by the BS.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_check_setpt_thresh (void)
{
   ffpc_chan_info_type *pri_chan_params;

   pri_chan_params = ffpc_get_param_ptr(ffpc_info.fpc_pri_chan);

   if (ffpc_sch_params.olpc_enabled && pri_chan_params->olpc_enabled &&
       ((ffpc_info.fpc_mode == FPC_MODE_PRI_800Hz) ||
        (ffpc_info.fpc_mode == FPC_MODE_PRI_EIB)) &&
         ffpc_info.setpt_thresh_init)
   {
      int16 curr_setpt_delta;
      uint8 abs_delta;

      curr_setpt_delta = (int16) ((pri_chan_params->curr_setpt_db >> 17) -
                                  (ffpc_sch_params.curr_setpt_db >> 17));

      abs_delta = (uint8) ABS(curr_setpt_delta - ffpc_info.pri_sch_init_delta);

      if (ffpc_info.sch_thresh <= abs_delta)
      {
         /* send OLRM */
         ffpc_send_olrm ();

         /* update init FCH-SCH delta */
         ffpc_info.pri_sch_init_delta = curr_setpt_delta;
      }
   }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_SEND_OLRM

DESCRIPTION
  This function will send an Outer Loop Report Message to MC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
ffpc_send_olrm (void)
{
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report to be sent to the Main Control task */

  if ((rpt_ptr = (mccrx_rpt_type *)q_get( &mcc_rx_free_q )) == NULL)
  {
    ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
  }
  else
  {
    mccrx_ol_type ol;
    memset(&ol, 0, sizeof(mccrx_ol_type));

    MSG_HIGH ("Send OL Rpt Msg", 0, 0, 0);

    /* Generate Outer Loop Report for MC */
    ol.hdr.rpt   = RXC_OL_R;

    ol.fch_incl  = FALSE;
    if (ffpc_fch_params.olpc_enabled)
    {
       ol.fch_incl = TRUE;
       ol.fpc_fch_curr_setpt = (byte) (ffpc_fch_params.curr_setpt_db >> 17);
         /* convert Q20 to Q3 */
    }

#ifdef FEATURE_IS2000_P2
    ol.dcch_incl = FALSE;
    if (ffpc_dcch_params.olpc_enabled)
    {
       ol.dcch_incl = TRUE;
       ol.fpc_dcch_curr_setpt = (byte) (ffpc_dcch_params.curr_setpt_db >> 17);
         /* convert Q20 to Q3 */
    }
#endif /* FEATURE_IS2000_P2 */

    ol.num_sup   = 0;
    if (ffpc_sch_params.olpc_enabled)
    {
       ol.num_sup = 1;
       ol.sch[0].sch_id = 0;
       ol.sch[0].fpc_sch_curr_setpt = (byte) (ffpc_sch_params.curr_setpt_db >> 17);
         /* convert Q20 to Q3 */
    }

    rpt_ptr->ol = ol;
    rxc_mc_rpt (rpt_ptr);
  }
}

/* <EJECT> */
/*=========================================================================
FUNCTION FFPC_RC_CHANGED

DESCRIPTION
  This function will monitor the changes in RCs on the F-FCH and F-SCH
  according to the following logic:
   if old RC was {RC3 or RC4} and new RC is {RC3 or RC4} return FALSE,
   if old RC was RC5 and new is RC5 return FALSE,
   if old RC was {RC1 or RC2} and new RC is {RC1 or RC2} return FALSE,
   else return TRUE.
  FFPC setpt computation differs for {RC3,RC4} and {RC5}. For optimization,
  setpts need not be re-computed if RC changes from RC3 to RC4 or vice-
  versa. Or if old RC and new RC are identical.

DEPENDENCIE
  None.

RETURN VALUE: indicates if RC changed or not.

SIDE EFFECTS
  None.
=========================================================================*/
boolean
ffpc_rc_changed
(
  deint_rc_type  old_rc,
  deint_rc_type  new_rc
)
{
   if (((old_rc == DEINT_RC3) || (old_rc == DEINT_RC4)) &&
       ((new_rc == DEINT_RC3) || (new_rc == DEINT_RC4)))
   {
      return FALSE;
   }
   else if ((old_rc == DEINT_RC5) && (new_rc == DEINT_RC5))
   {
      return FALSE;
   }
   else if (((old_rc == DEINT_RC1) ||
             (old_rc == DEINT_RC2) ||
             (old_rc == DEINT_RC_INVALID)) &&
            ((new_rc == DEINT_RC1) || (new_rc == DEINT_RC2)))
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

#ifdef FEATURE_ENHANCE_DCCH_DTX_DETECTION
/*=========================================================================
FUNCTION FFPC_GET_SUBCHAN_GAIN_LIN

DESCRIPTION
  Returns the current subchannel linear gain (magnitude gain [not mag^w2])
  as calculated by 45*convert_dBq8_to_lin().

DEPENDENCIES
  None.

RETURN VALUE:
  Linear subchannel gain as a byte

SIDE EFFECTS
  None.
=========================================================================*/
byte
ffpc_get_subchan_gain_lin
(
  void
)
{
  /* !!!! the scaling of this (unity=45) could change */
  /* note: fpc_subchan_gain_db << (6-1), converts Q2 to Q8 and then /2 to
   * get magnitude instead of mag^2
   */
  return (byte) ((45*convert_dBq8_to_lin(ffpc_info.fpc_subchan_gain_db << 5, 16, 24))>>16);
}

#endif /* End of FEATURE_ENHANCE_DCCH_DTX_DETECTION  */

/*=========================================================================
FUNCTION FFPC_GET_SUBCHAN_GAIN_DB

DESCRIPTION
  Returns the current subchannel gain in dB as stored in
  ffpc_info.fpc_subchan_gain_db.

DEPENDENCIES
  None.

RETURN VALUE:
  Subchannel gain in db as a byte

SIDE EFFECTS
  None.
=========================================================================*/
byte
ffpc_get_subchan_gain_db
(
  void
)
{
  return ffpc_info.fpc_subchan_gain_db;
}

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
/*=========================================================================
FUNCTION FFPC_GET_DCCH_MIN_SETPT_DB

DESCRIPTION
  Returns the current DCCH subchannel minimum set point as stored in
  ffpc_dcch_params.min_setpt_db.

DEPENDENCIES
  None.

RETURN VALUE:
  Minimum set point as a byte

SIDE EFFECTS
  None.
=========================================================================*/
byte
ffpc_get_dcch_min_setpt_db
(
  void
)
{
  return ffpc_dcch_params.min_setpt_db;
}

/*=========================================================================
FUNCTION FFPC_GET_DCCH_HW_SETPT

DESCRIPTION
  Returns the current DCCH subchannel set point in units of linear
  magnitude-square Q8.

DEPENDENCIES
  None.

RETURN VALUE:
  HW set point

SIDE EFFECTS
  None.
=========================================================================*/
word
ffpc_get_dcch_hw_setpt
(
  void
)
{
  return (word) (convert_dBq8_to_lin(ffpc_dcch_params.setpt_adjusted_dbq20 >> DCCH_SHIFT_Q20_2_Q8,
                                          DCCH_HW_SETPT_Q, LIN_OUTPUT_BITS));
}

#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  */

/*=========================================================================
FUNCTION FFPC_GET_SCH_SETPT_LINQ8

DESCRIPTION
  Returns the current DCCH subchannel set point in units of linear magnitude-square
  Q8.

DEPENDENCIES
  None.

RETURN VALUE:
  linear set point

SIDE EFFECTS
  None.
=========================================================================*/
int16
ffpc_get_sch_setpt_linQ8
(
  void
)
{
  return (int16) (  convert_dBq8_to_lin(ffpc_sch_params.curr_setpt_db >> 12, /* set pt dB, converted Q20->Q8 */
                                            8,                  /* Q-precision of output */
                                            16                  /* number of bits of output */
                                            )
                );
}


#ifdef FEATURE_IS2000_REL_A

/*=========================================================================
FUNCTION FFPC_GET_SETPT

DESCRIPTION
  Returns the current dB setpoints for fch, dcch, and sch.

DEPENDENCIES
  None.

RETURN VALUE:
  The current setpoints.

SIDE EFFECTS
  None.
=========================================================================*/
void ffpc_get_setpt
(
  boolean *fch_incl,
  byte *fpc_fch_curr_setpt,
  boolean *dcch_incl,
  byte *fpc_dcch_curr_setpt,
  byte *num_sup,
  byte *sch_id,
  byte *fpc_sch_curr_setpt
)
{
  *fch_incl  = FALSE;
  if (ffpc_fch_params.olpc_enabled)
  {
     *fch_incl = TRUE;
     *fpc_fch_curr_setpt = (byte) (ffpc_fch_params.curr_setpt_db >> 17);
       /* convert q20 to q3 */
  }

  *dcch_incl = FALSE;
#ifdef FEATURE_IS2000_P2
  if (ffpc_dcch_params.olpc_enabled)
  {
     *dcch_incl = TRUE;
     *fpc_dcch_curr_setpt = (byte) (ffpc_dcch_params.curr_setpt_db >> 17);
       /* convert q20 to q3 */
  }
#endif /* FEATURE_IS2000_P2 */

  *num_sup   = 0;
  if (ffpc_sch_params.olpc_enabled)
  {
     *num_sup = 1;
     *sch_id = 0;
     *fpc_sch_curr_setpt = (byte) (ffpc_sch_params.curr_setpt_db >> 17);
  }
}
#endif //FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION FFPC_GET_DEFAULT_FCH_INIT_SETPT_VAL

DESCRIPTION
 

DEPENDENCIES

RETURN VALUE
 Return FFPC_DEFAULT_FCH_INIT_SETPT_VAL.

SIDE EFFECTS
  None.
===========================================================================*/
int8 ffpc_get_default_fch_init_setpt_val(void)
{
  return (FFPC_DEFAULT_FCH_INIT_SETPT_VAL);
}


