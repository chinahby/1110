#ifndef SRCH4DRV_T_H
#define SRCH4DRV_T_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       S E A R C H E R   4   D R I V E R    T Y P E D E F    H E A D E R

GENERAL DESCRIPTION
   This file contains the exported structs and defines for searcher 4's
   driver layer.

      Copyright (c) 2003 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/inc/srch4drv_t.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/08   adw     Added SRCH4_USE_LPQ and SRCH4_TECH_STRING macro.
06/17/08   gs      8650 turn-on changes
04/24/08   sst     Moved srch4 log definations here from srchlog.h
07/11/07   ljl     Supported multi-carriers.
10/23/06   jyw/awj Added structure members for uninterpolated search results.
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
01/16/06   awj     Lint cleanup
08/10/05   ddh     Added SRCH4_MIN_Q = 0 in srch4_queue_type
07/13/04   sst     Added SRCH4_NO_QUEUE = SRCH4_MAX_Q in srch4_queue_type
05/28/04   jcm     Sector module integration
05/04/04   sst     Merge from srch4 3Q dev branch
05/04/04   sst     Removed include search_int.h
05/03/04   sst     Moved COH_TRUNC_* defines to srch4trans_t.h to be exported
05/03/04   sst     Peer Review Comments
04/26/04   sst     Changed SRCH4DRV_RET_FREE to be queue specific
04/15/04   sst     More code cleanup
04/12/04   sst     Removed debug include file
04/08/04   sst     Code cleanup
                   Mainlined FEATURE_SRCH4_MDSP2
                   Created SRCH4_HAS_HPQ1, SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
                   Created SRCH4_DEBUG, SRCH4_DEBUG_MSG()
04/06/04   sst     Updated some featurization
03/24/04   sst     Added extern for srch4_prio_str
03/02/04   sst     Use of MDSP version of num of peaks
02/26/04   sst     File formating
01/06/03   sst     Added field age_search to srch4_task_struct_type
12/05/03   sst     initial implementation, broken out from srch4drv.h

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "log.h"

#include "srch4drvi.h"
#include "srch4trans_t.h"


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Debugging defines and Macros
-------------------------------------------------------------------------*/
#undef SRCH4_DEBUG
#define SRCH4_DEBUG_MSG( str, a, b, c )  
/* MSG_FATAL( str, a, b, c ) */


/*===========================================================================

                  SRCH4 logging structs and definitions
  
  note: Reserved fields are to make packet to word aligned
===========================================================================*/

#define SRCH4_HDR_LOG_VERSION  1

typedef struct {
  uint8                 Reserved;
  uint8                 PeakPos_MSB;
  uint16                PeakPos_LSB;
  uint16                PeakEng;
} srch4_res_log_type;

typedef struct {
  uint16                Pilot_Antenna_BurstLen_QOF_WalshMSB;
  uint16                WalshLSB_NonCohLen;
  uint16                CohTrunc_CohLen;
  uint16                PNPos;
  uint16                WinSize;
  #ifdef T_MDM7800
  uint16                carrier;
  #endif /* T_MDM7800 */
  srch4_res_log_type    results[ SRCH_MAX_PEAKS ];
} srch4_task_log_type;

typedef struct {
  log_hdr_type          hdr;
  #ifdef T_MDM7800
  uint8                 version;
  uint8                 reserved;
  #endif /* T_MDM7800 */
  uint8                 Tech_QPCH_NumPeaks;
  uint8                 Queue_Priority;
  uint8                 NumTasks;
  int8                  FreqOffset;
  uint8                 rx0_agc;
  uint8                 rx1_agc;
  srch4_task_log_type   tasks[ SRCH4_MAX_NUM_TASKS ];
} LOG_SRCH4_C_type;

/* size of the header block (minus task) */
#define SRCH4_LOG_HEADER_SIZ     ( FPOS( LOG_SRCH4_C_type, tasks ) )

/* size of the parameters (minus the results) */
#define SRCH4_LOG_PARAM_SIZ      ( FPOS( srch4_task_log_type, results ) )

/* size of the results */ 
#define SRCH4_LOG_RESULTS_SIZ    ( sizeof( srch4_res_log_type ) )

#define SRCH4_LOG_SIZ( tasks, peaks )                                    \
          ( SRCH4_LOG_HEADER_SIZ + (tasks) *                             \
              ( SRCH4_LOG_PARAM_SIZ + (peaks) * SRCH4_LOG_RESULTS_SIZ ) )

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/

/* number of peaks for searcher 4 */
#define SRCH4_NUM_PEAKS      NUM_SORTED_PEAKS

/* define for no dump occured queue */
#define SRCH4_NO_DUMP        -1

/* maximum sector gain index */
#define MAX_SECTOR_GAIN_VAL  4

/*-------------------------------------------------------------------------
      Typedefs - ENUMs
-------------------------------------------------------------------------*/

/* searcher 4 error return values */
enum {
  SRCH4DRV_RET_FULL           =  0,       /* queue is full              */
  SRCH4DRV_RET_ACTIVE         = -1,       /* queue is active            */
  SRCH4DRV_RET_UNCLEAN        = -2,       /* last srch wasn't cleaned up*/
  SRCH4DRV_RET_INVALID_PARAM  = -3,       /* bad parameter was passed
                                             (i.e. null ptr)            */
  SRCH4DRV_RET_INVALID_QUEUE  = -4,       /* invalid/no queue specified */
  SRCH4DRV_RET_TECH_MISMATCH  = -5,       /* technology mismatch        */
  SRCH4DRV_RET_FATAL          = -6,       /* fatal errors               */
  SRCH4DRV_RET_FREE           = -SRCH4_MAX_TASKS,
                                          /* queue is inactive and empty*/
  SRCH4DRV_RET_HPQ0_FREE      = -SRCH_MAX_HPQ_TASKS,
                                          /* HPQ0 is inactive and empty */
  #ifdef SRCH4_USE_LPQ
  SRCH4DRV_RET_LPQ_FREE       = -SRCH_MAX_LPQ_TASKS,
                                          /* LPQ is inactive and empty */
  #endif /* SRCH4_USE_LPQ */
  #ifdef SRCH4_HAS_HPQ1
  SRCH4DRV_RET_HPQ1_FREE      = -SRCH_MAX_HPQ_TASKS,
                                          /* HPQ1 is inactive and empty */
  #endif /* SRCH4_HAS_HPQ1 */
  SRCH4DRV_RET_OK             = -(SRCH4_MAX_TASKS +1),
                                          /* no errors to report        */
};

/* searcher 4 queue types */
/* make sure this matches srch4_mem_map defined in srch4drv.c */
typedef enum
{
  SRCH4_MIN_Q = 0,                       /* none    */
  SRCH4_HPQ0 = SRCH4_MIN_Q,              /* HPQ 0   */
  #ifdef SRCH4_USE_LPQ
  SRCH4_LPQ,                             /* LPQ     */
  #endif /* SRCH4_USE_LPQ */
  #ifdef SRCH4_HAS_HPQ1
  SRCH4_HPQ1,                            /* HPQ 1   */
  #endif /* SRCH4_HAS_HPQ1 */
  SRCH4_MAX_Q,                           /* none    */
  SRCH4_NO_QUEUE = SRCH4_MAX_Q,
} srch4_queue_type;

/* searcher 4 antenna select */
typedef enum
{
  SRCH4_ANT_0         = 0,               /* antenna 0 mode    */
  SRCH4_ANT_1         = 1,               /* antenna 1 mode    */
  SRCH4_ANT_DUAL      = 2,               /* dual antenna mode */
  SRCH4_ANT_MAX,
} srch4_ant_select_type;

/* Memory Address structures are defined in srch4drv.c
    these types and structures are used to simplify the macros used to
    read and write into the shared memory between the DSP and Searcher
 */
/* index into memory address array */
typedef enum
{
  SRCH4I_ABORT_ADDR = 0,
  SRCH4I_SEARCH_DONE_ADDR,
  SRCH4I_NUM_TASKS_ADDR,
  SRCH4I_SRCH_TYPE_ADDR,
  SRCH4I_PRIORITY_ADDR,
  SRCH4I_FREQ_OFFSET_ADDR,
  SRCH4I_SRCH_PARAMS_ADDR,
  SRCH4I_NUM_RESULTS_ADDR,
  SRCH4I_RESULTS_ADDR,
  SRCH4I_MAX_ADDR
} srch4drvi_mem_map_type;

#if defined (T_MDM7800) || defined (T_MSM8650B)
/* Carrier select */
typedef enum
{
   SRCH4_CARRIER_0 = 0,
   SRCH4_CARRIER_1 = 1,
   SRCH4_CARRIER_2 = 2
} srch4_carrier_type;
#endif /* T_MDM7800 || T_MSM8650B */

/*-------------------------------------------------------------------------
      Typedefs - Task structure types
-------------------------------------------------------------------------*/

/* searcher 4 task structure type
    (roughly ordered as it is to program the MDSP) */
struct  srch4_task_struct
{
  uint16    window_size;               /* srch sweep win size (chips)
                                          (MSM6100 max 8192
                                           MSM6500 max 32768)         */
  uint16    pn_pos;                    /* PN position (chips)
                                          (0-32767)                   */
  uint8     phase;                     /* phase (Cx8)
                                          (MSM6500 not used )         */
  uint16    coherent_len;              /* coherent length
                                          (typical 64-2048,
                                           MSM6500 min 48, max 2048)  */
  uint16    pilot_inx;                 /* pilot offset
                                          (0-511)                     */
  uint8     burst_len;                 /* HDR burst length
                                          (1-2)                       */
  uint8     non_coherent_len;          /* non-coherent length
                                          (0-128
                                           MSM6500-HDR 1-2)           */
  srch4_ant_select_type  diversity;    /* receive diversity select
                                          (see srch4_ant_select)      */
#if defined (T_MDM7800) || defined (T_MSM8650B)
  srch4_carrier_type     carrier;      /* carrier */
#endif /* T_MDM7800 || T_MSM8650B */

  uint8     coherent_trunc;            /* coherent truncation
                                          (see coh_tr[])              */
  uint8     qof;                       /* quasi-orthogonal function
                                          (0-3)                       */
  uint16    walsh;                     /* walsh index
                                          (0-511)                     */
  uint32    window_pos;                /* window position (Cx8) (end)
                                          (0-32767)                   */
  uint32    peak_pos[SRCH4_NUM_PEAKS]; /* peak position (Cx16)        */
  uint16    peak_eng[SRCH4_NUM_PEAKS]; /* peak energy                 */

  uint32    peak_posx2_raw[SRCH4_NUM_PEAKS]; /* raw peak position(Cx2)*/
  uint16    peak_eng_raw[SRCH4_NUM_PEAKS];   /* raw peak energy       */

  uint32    pilot_set_mask;            /* pilot set membership        */

  void    (*process_results)(
              struct srch4_task_struct *task);
                                       /* ptr to res processing func  */
  void     *parm_ptr;                  /* ptr to parm struct          */
  void     *result_ptr;                /* ptr to result struct        */
};
typedef struct srch4_task_struct srch4_task_struct_type;

/*-------------------------------------------------------------------------
      Declarations
-------------------------------------------------------------------------*/

/* searcher 4 memory map */
extern volatile uint16   *srch4_mem_map[ SRCH4_MAX_Q ][ SRCH4I_MAX_ADDR ];

/* strings for F3 messages */
extern const char srch4_tech_str[ (SRCH4_NUM_TECH + 1) ][ 6 ];
#define SRCH4_TECH_STRING( tech )                                 \
          srch4_tech_str[ ( ( (tech & 0xF) >= SRCH4_NUM_TECH ) ?  \
                             SRCH4_NUM_TECH : (tech & 0xF) ) ]
extern const char srch4_queue_str[ (SRCH4_MAX_Q + 1) ][ 5 ];
extern const char srch4_prio_str[ SRCH4_PRIO_MAX ][ 10 ];

#endif /* SRCH4DRV_T_H */
