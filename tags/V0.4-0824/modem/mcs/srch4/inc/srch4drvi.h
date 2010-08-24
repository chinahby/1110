#ifndef SRCH4DRVI_H
#define SRCH4DRVI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R
                    I N T E R N A L   H E A D E R

GENERAL DESCRIPTION
   This file contains the internal definitions for the searcher4 drivers.

      Copyright (c) 2002 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/inc/srch4drvi.h#4 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
09/08/08   adw      Added SRCH4_USE_LPQ to featurize low priority queue.
12/14/07   sst      Correct featurization around Sched SR Flush
                    Added SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE
07/22/07   ljl      Support multi-carriers.
10/23/06   sst      Merge from QSC6055 branched files
                    Increase the lost dump timeouts for T_RUMI
02/04/05   bt       Add scheduled sample ram flush.
07/28/04   sst      Set the WINSIZ_MIN to 4 (smaller than any known used win)
06/21/04   sst      Corrected naming of srch_mdsp_check_cmd_ functions
05/04/04   sst/va   Merge from srch4 3Q dev branch (dev-changes)
05/04/04   sst      dev-Added define SRCH4_MDSP2_DEFINES_EXIST until
                     search_int.h is integrated into mdspext.h
04/26/04   sst      dev-Corrected Max win_siz and non_coh, 
                     corrected priority mask
04/08/04   sst      dev-Created SRCH4_HAS_HPQ1,SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
04/06/04   sst      dev-Updated some featurization
03/02/04   sst      dev-Use of MDSP defines for triggers
02/26/04   sst      dev-fixed CHECK_SEARCH_INT_FLAG
01/28/04   ejv      Mainline T_MSM6500.
12/15/03   sst      Replaced MDSP_CMD_NOT_DONE() call in loop to use new macro
                     MDSP_CHECK_CMD_NE() with better error checking
09/09/03   sst      Changed value of WINSIZ_MIN and removed WINSIZ_DEFAULT
07/01/03   sst      Moved SRCH4_MAX_NUM_TASKS, COH_TRUNC_*, and
                     SRCH2_SRCH4_OFFSET int srch4drv.h to preserve scope
                     of srch4drvi.h
04/14/03   sst      Added WINSIZ____ defines
03/28/03   bt       Changed SrchCmd macros so that it waits for the previous
                     command to be done before issuing another one and removed
                     the SRCH4_CLR_CMD macro.
01/07/03   sst      Added lost abort timer timeout value
12/05/02   sst      Code Clean up
11/07/02   sst      Added support for M6500
                    Code cleanup
11/05/02   sst      File re-format
11/05/02   sst      Merge changes
10/08/02   sst      Changed MAX_NUM_TASKS to SRCH$_MAX_NUM_TASKS for clarity
10/07/02   bt       SRCH4 interface changes (no -1 for winsize,
                     coherent/non-coherent integration)
10/01/02   sst      Added $Header line
09/22/02   sst      Removal of debug functions.
                    MSM6100 implementation and testing changes
08/02/02   gs       Initial merge for MSM6100.
07/01/02   sst      Added support for LPQ
06/07/02   bt       initial implementation

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"

#include "clk.h"
#include "mdspext.h"
#include "srch_hw_t.h"
#include "srch_mdsp.h"
#include "time.h"

#ifdef  SRCH_microHpq1HPQResults
  #define SRCH4_HAS_HPQ1
#else
  #undef  SRCH4_HAS_HPQ1
#endif

#ifdef  MASK_microSrchCmd__SR_FLUSH
  /* this is for the masks soon to be defined in mdspext.h */
  #define SRCH4_MDSP2_DEFINES_EXIST
#else
  #undef  SRCH4_MDSP2_DEFINES_EXIST
#endif

#ifdef  SRCH_microHpq0HPQResults
  #undef  SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE
  #undef  SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
#else
  #ifdef  SRCH_microHpqHPQResults
    #define SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE
    #undef  SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
  #else
    #undef  SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE
    #define SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
  #endif
#endif


/*===========================================================================
             LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/
/* Max number of tasks/sectors programmable for HPQ and LPQ searches */
/* Make sure SRCH4_MAX_NUM_TASKS in srch4trans.h is set correctly */
#define SRCH4_MAX_TASKS      SRCH_MAX_HPQ_TASKS
#define SRCH4_MAX_HPQ_TASKS  SRCH_MAX_HPQ_TASKS

#ifdef SRCH4_USE_LPQ
  #define SRCH4_MAX_LPQ_TASKS  SRCH_MAX_LPQ_TASKS
#endif /* SRCH4_USE_LPQ */

/* Status dump timeouts for lost searches in milliseconds */
/* The following formula is used to estimate lost dump times:
   (srch duration) = (setup times) +
                     ( (Win Size)/4 * (Coh_int + 18) * NonCoh_int * 1.5 ) /
                       (srch4 clock speed in KHz)
    (taken from HDR) */

/* window size to separate "Normal" and "Max" timeouts */
#define SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT     500

/* SRCH4 HPQ lost dump times (per search) */
/* searches with window size < SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT
   (standard allows max of 452 for normal searches)
    derived from window size = 500, coh = 512, noncoh = 1 */
#ifdef T_RUMI
#error code not present
#else
  #define SRCH4_HPQ_DUMP_TIMEOUT_NORMAL        12
#endif /* T_RUMI */
/* searches with window size > SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT
   (currently acq searches have window sizes >
                                        SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT)
    derived from window size = 8192, coh = 2048, noncoh = 1 */
#ifdef T_RUMI
#error code not present
#else
  #define SRCH4_HPQ_DUMP_TIMEOUT_MAX          166
#endif /* T_RUMI */

#ifdef SRCH4_USE_LPQ
/* SRCH4 LPQ lost dump times (per search) */
/* searches with window size < SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT
   derived from HPQ0_MAX + AFLT search time estimate (80ms)*/
#define SRCH4_LPQ_DUMP_TIMEOUT_NORMAL        246
/* searches with window size > SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT
   (currently acq searches have window sizes >
                                        SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT)
   derived from HPQ0_MAX * 2 */
#define SRCH4_LPQ_DUMP_TIMEOUT_MAX           340
#endif /* SRCH4_USE_LPQ */

/* define the abort timeout */
#define SRCH4_ABORT_TIMEOUT                  166

/* parameter limits */
#define WINSIZ_MIN              4  /* minimum valid window size (chips)      */
#define WINSIZ_MAX          32767  /* maximum valid window size (chips)      */

#define COH_LEN_MIN            48  /* minimum valid Coherent Length          */
#define COH_LEN_MAX          3584  /* maximum valid Coherent Length          */
#define COH_LEN_AFLT_MAX     2048  /* maximum valid AFLT Coherent Length     */

#define NON_COH_LEN_MIN         1  /* minimum valid Non-Coherent Length      */
#define NON_COH_LEN_MAX       127  /* maximum valid Non-Coherent Length      */
#define NON_COH_LEN_HDR_MAX     2  /* maximum valid HDR Non-Coherent Length  */

#define M_X_N_MAX            3948  /* maximum valid 
                                       Coh Len (M) * Non Coh Len (N)         */

#define BURST_LEN_MAX           2  /* maximum valid Burst Length (HDR only)  */


/* command trigger values */
#ifndef SRCH4_MDSP2_DEFINES_EXIST

#ifdef  SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
#define SRAM_FLUSH_TRIGGER 0x00000008
#else
#define SRAM_FLUSH_TRIGGER 0x00004000
#endif
#define ABORT_TRIGGER      0x00008000

#define HPQ0_TRIGGER       0x00000002
#define HPQ1_TRIGGER       0x00000004
#define LPQ_TRIGGER        0x00000001

#else  /* SRCH4_MDSP2_DEFINES_EXIST */

#define SRAM_FLUSH_TRIGGER MASK_microSrchCmd__SR_FLUSH    /* 0x00004000 */
#define ABORT_TRIGGER      MASK_microSrchCmd__ABORT       /* 0x00008000 */
#define HPQ0_TRIGGER       MASK_microSrchCmd__HPQ0        /* 0x00000002 */
#define HPQ1_TRIGGER       MASK_microSrchCmd__HPQ1        /* 0x00000004 */
#define LPQ_TRIGGER        MASK_microSrchCmd__LPQ         /* 0x00000001 */

#endif /* !SRCH4_MDSP2_DEFINES_EXIST */

#ifdef MASK_microSrchCmd__SCHEDULE_SR_FLUSH 
#define SCHED_SRAM_FLUSH_TRIGGER \
                           MASK_microSrchCmd__SCHEDULE_SR_FLUSH
                                                          /* 0x00001000 */
#endif /* MASK_microSrchCmd__SCHEDULE_SR_FLUSH */

/* Values, offsets, shifts, and masks relating to the MDSP*/
#ifndef SRCH4_MDSP2_DEFINES_EXIST
#define NUM_SORTED_PEAKS   6
#endif
#define TASK_SIZE          5
#define NUM_TASK_MASK      0x1F
#define SRCH_TYPE_MASK     0x0003
#define SRCH_PRIORITY_MASK 0x7FFF
#define FREQ_OFFSET_MASK   0xFFFF

#define WINSIZ             0
#define PNPOS              1
#define PHASE_M            2
#define PILOT_N            3
#define DIV_CT_QOF_WALSH   4

#define PHASE_SHIFT        12
#define PILOT_SHIFT        7
#define BURST_SHIFT        2
#define DIVERSITY_SHIFT    14
#define COH_TRUNC_SHIFT    11
#define QOF_SHIFT          9
#ifdef T_MDM7800
#define CARRIER_SHIFT      12
#endif /* T_MDM7800 */

/* Register physically allows 16 bits, but keep the mask at 32767 */
#define WINSIZ_MASK        0x7FFF
#define PNPOS_MASK         0x7FFF
#define PHASE_MASK        (0xE    << PHASE_SHIFT)

#ifdef T_MDM7800
#define COHERENT_MASK      0xFFF
#define CARRIER_MASK      (0xF    << CARRIER_SHIFT)
#else
#define COHERENT_MASK      0x1FFF
#endif /* T_MDM7800 */

#define PILOT_MASK        (0x1FF  << PILOT_SHIFT)
#define BURST_MASK        (0x3    << BURST_SHIFT)
#define NON_COH_MASK       0x003F
#define NON_COH_BURST_MASK 0x0003
#define DIVERSITY_MASK    (0x3    << DIVERSITY_SHIFT)
#define COH_TRUNC_MASK    (0x7    << COH_TRUNC_SHIFT)
#define QOF_MASK          (0x3    << QOF_SHIFT)
#define WALSH_MASK         0x01FF
#define ROT_MASK           0xFFFF

/* Result values,offsets and masks */
#define PEAK_SIZE          4
#define RESULT_SIZE        (PEAK_SIZE * NUM_SORTED_PEAKS)

#define PEAK_POS           0
#define PEAK_ENG           1
#define PEAK_LS_ENG        2
#define PEAK_RS_ENG        3


/* define dummy addresses to allow for invalid addresses in memory map */
extern  int32              SRCH_dummy_addr;


/*****************************************************************************
                     Macros for searcher - DSP commands
*****************************************************************************/

/* macro to notify DSP to execute command */
#define SRCH4_SIGNAL_DSP( )                                                    \
          MDSP_SIGNAL( )

#ifndef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
/* macro to read to determine if the dump was caused by the completion
     of a search */
#define SRCH4_CHECK_SEARCH_INT_FLAG( flag )                                    \
          if( mdsp_read( SRCH_srchIntFlag ) )                                  \
          {                                                                    \
            mdsp_write( SRCH_srchIntFlag, 0 );                                 \
            flag = TRUE;                                                       \
          }                                                                    \
          else                                                                 \
          {                                                                    \
            flag = FALSE;                                                      \
          }
#else /* SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */
/* macro to read to determine if the dump was caused by the completion
     of a search - NOT SUPPORTED */
#define SRCH4_CHECK_SEARCH_INT_FLAG( flag )                                    \
          flag = TRUE
#endif /* SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */

/* macro to flush the searcher 4 sample RAM */
#define SRCH4_FLUSH_SAMPLE_RAM( antenna )                                      \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          /* antenna select here */                                            \
          mdsp_write( SRCH_cdmaAntSelect, ( antenna & 0x1 ) );                 \
          mdsp_write( SRCH_microSrchCmd, SRAM_FLUSH_TRIGGER )

#ifdef SCHED_SRAM_FLUSH_TRIGGER 
/* macro to do a scheduled searcher 4 sample RAM flush */
#define SRCH4_SCHED_FLUSH_SAMPLE_RAM( antenna, position )                      \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          /* antenna select here */                                            \
          mdsp_write( SRCH_cdmaAntSelect, ( antenna & 0x1 ) );                 \
          mdsp_write( SRCH_microSrchData, ( position ) );                      \
          mdsp_write( SRCH_microSrchCmd, SCHED_SRAM_FLUSH_TRIGGER )
#endif /* SCHED_SRAM_FLUSH_TRIGGER */


/*****************************************************************************
                     Macros for starting, aborting, and detecting searches
*****************************************************************************/

/* macro to start a search */
#ifdef SRCH4_HAS_HPQ1
  #ifdef SRCH4_USE_LPQ
  #define SRCH4_START( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, HPQ0_TRIGGER );                   \
              break;                                                           \
            }                                                                  \
            case SRCH4_LPQ:                                                    \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, LPQ_TRIGGER );                    \
              break;                                                           \
            }                                                                  \
            case SRCH4_HPQ1:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, HPQ1_TRIGGER );                   \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in start macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #else /* !SRCH4_USE_LPQ */
  #define SRCH4_START( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, HPQ0_TRIGGER );                   \
              break;                                                           \
            }                                                                  \
            case SRCH4_HPQ1:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, HPQ1_TRIGGER );                   \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in start macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #endif /* SRCH4_USE_LPQ */
#else  /* !SRCH4_HAS_HPQ1 */
  #ifdef SRCH4_USE_LPQ
  #define SRCH4_START( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, HPQ0_TRIGGER );                   \
              break;                                                           \
            }                                                                  \
            case SRCH4_LPQ:                                                    \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, LPQ_TRIGGER );                    \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in start macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }          
  #else /* !SRCH4_USE_LPQ */
  #define SRCH4_START( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, HPQ0_TRIGGER );                   \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in start macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #endif /* SRCH4_USE_LPQ */
#endif /* SRCH4_HAS_HPQ1 */

/* macro to determine which searcher queue caused the dump */
#define SRCH4_CHECK_SEARCH_DONE( queue, flag )                                 \
          if( mdsp_read( srch4_mem_map[ queue ][ SRCH4I_SEARCH_DONE_ADDR ] ) ) \
          {                                                                    \
            mdsp_write( srch4_mem_map[ queue ][ SRCH4I_SEARCH_DONE_ADDR ], 0 );\
            flag = TRUE;                                                       \
          }                                                                    \
          else                                                                 \
          {                                                                    \
            flag = FALSE;                                                      \
          }

/* macro to issue an abort */
#ifdef SRCH4_HAS_HPQ1
  #ifdef SRCH4_USE_LPQ
  #define SRCH4_ABORT( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | HPQ0_TRIGGER );   \
              break;                                                           \
            }                                                                  \
            case SRCH4_LPQ:                                                    \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | LPQ_TRIGGER );    \
              break;                                                           \
            }                                                                  \
            case SRCH4_HPQ1:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | HPQ1_TRIGGER );   \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in abort macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #else /* !SRCH4_USE_LPQ */
  #define SRCH4_ABORT( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | HPQ0_TRIGGER );   \
              break;                                                           \
            }                                                                  \
            case SRCH4_HPQ1:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | HPQ1_TRIGGER );   \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in abort macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #endif /* SRCH4_USE_LPQ */
#else  /* !SRCH4_HAS_HPQ1 */
  #ifdef SRCH4_USE_LPQ
  #define SRCH4_ABORT( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | HPQ0_TRIGGER );   \
              break;                                                           \
            }                                                                  \
            case SRCH4_LPQ:                                                    \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | LPQ_TRIGGER );    \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in abort macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #else /* !SRCH4_USE_LPQ */
  #define SRCH4_ABORT( queue )                                                 \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          switch( queue )                                                      \
          {                                                                    \
            case SRCH4_HPQ0:                                                   \
            {                                                                  \
              mdsp_write( SRCH_microSrchCmd, ABORT_TRIGGER | HPQ0_TRIGGER );   \
              break;                                                           \
            }                                                                  \
            default:                                                           \
            {                                                                  \
              ERR( "Invalid Queue (%d) in abort macro", queue, 0, 0 );         \
              break;                                                           \
            }                                                                  \
          }
  #endif /* SRCH4_USE_LPQ */
#endif /* SRCH4_HAS_HPQ1 */

/* macro to abort all searches */
#define SRCH4_ABORT_ALL( )                                                     \
          MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );                      \
          mdsp_write( SRCH_microSrchCmd,                                       \
                      ABORT_TRIGGER | HPQ0_TRIGGER |                           \
                      HPQ1_TRIGGER  | LPQ_TRIGGER )

/* macro to determine which searcher queue has reported an abort */
#define SRCH4_CHECK_ABORT( queue, flag )                                       \
          if( mdsp_read( srch4_mem_map[ queue ][ SRCH4I_ABORT_ADDR ] ) )       \
          {                                                                    \
            mdsp_write( srch4_mem_map[ queue ][ SRCH4I_ABORT_ADDR ], 0 );      \
            flag = TRUE;                                                       \
          }                                                                    \
          else                                                                 \
          {                                                                    \
            flag = FALSE;                                                      \
          }



/*****************************************************************************
                     Macros to set searcher parameters
*****************************************************************************/

/* -------------------- Queue Parameters ---------------------------------- */

/* macro to set number of tasks in each queue */
#define SRCH4_NUM_TASKS( queue, tasks )                                        \
          mdsp_write( srch4_mem_map[ queue ][ SRCH4I_NUM_TASKS_ADDR ],         \
                      ( tasks & NUM_TASK_MASK ) )

/* macro to bypass rotators */
#ifdef SRCH4_USE_LPQ
  #define SRCH4_ROT_BYPASS( queue )                                            \
          if( queue != SRCH4_LPQ )                                             \
          {                                                                    \
            mdsp_write( srch4_mem_map[ queue ][ SRCH4I_ROT_BYPASS_ADDR ],      \
                         ( (0xFFFF) & ROT_MASK ) )                             \
          }
#else /* !SRCH4_USE_LPQ */
  #define SRCH4_ROT_BYPASS( queue )                                            \
          {                                                                    \
            mdsp_write( srch4_mem_map[ queue ][ SRCH4I_ROT_BYPASS_ADDR ],      \
                         ( (0xFFFF) & ROT_MASK ) )                             \
          }
#endif /* SRCH4_USE_LPQ */

/* macro to set search types */
#define SRCH4_SRCH_TYPE( queue, srch_type )                                    \
          mdsp_write( srch4_mem_map[ queue ][ SRCH4I_SRCH_TYPE_ADDR ],         \
                      ( srch_type & SRCH_TYPE_MASK ) )

/* macro to set search priority */
#define SRCH4_SRCH_PRIORITY( queue, priority )                                 \
          mdsp_write( srch4_mem_map[ queue ][ SRCH4I_PRIORITY_ADDR ],          \
                      ( priority & SRCH_PRIORITY_MASK ) )

/* macro to set frequency offsets */
#define SRCH4_SET_FREQ_OFFSET( queue, freq_offset )                            \
          mdsp_write( srch4_mem_map[ queue ][ SRCH4I_FREQ_OFFSET_ADDR ],       \
                      ( freq_offset & FREQ_OFFSET_MASK ) )

/* macro to set HDR's Q8 scale factor */
#define SRCH4_SET_HDR_SCALE_Q8( q8_scale )                                     \
          mdsp_write( SRCH_hdrSearchScaleQ8,                                   \
                      ( q8_scale & 0xffff ) )

/* macro to select the antenna as the 1x sample server RTC source */
#define SRCH4_SET_CDMA_ANT( antenna )                                          \
          mdsp_write( SRCH_cdmaAntSelect,                                      \
                      ( antenna & 0x1 ) )

/* -------------------- Task Parameters ----------------------------------- */

/* macro to set the window sizes */
#define SRCH4_WIN_SIZE( queue, task, winsiz )                                  \
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + WINSIZ ),                  \
                             ( winsiz & WINSIZ_MASK ) )

/* macro to set the pn position */
#define SRCH4_PN_POSITION( queue, task, pnpos )                                \
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + PNPOS ),                   \
                             ( pnpos & PNPOS_MASK ) )

#ifdef T_MDM7800
/* macro to set carrier and coherent integration length (M)*/
#define SRCH4_CARRIER_M( queue, task, carrier, coherent )                          \
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + PHASE_M ),                 \
                             ( ( (carrier << CARRIER_SHIFT) & CARRIER_MASK ) |       \
                               (coherent & COHERENT_MASK) ) )
#else
/* macro to set pn phase (Cx8) and coherent integration length (M)*/
#define SRCH4_PHASE_M( queue, task, phase, coherent )                          \
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + PHASE_M ),                 \
                             ( ( (phase << PHASE_SHIFT) & PHASE_MASK ) |       \
                               (coherent & COHERENT_MASK) ) )
#endif /* T_MDM7800 */

/* macro to set pilot index (PILOT) and non-coherent int. (N) */
#define SRCH4_PILOT_N( queue, task, pilot, non_coherent )                      \
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + PILOT_N ),                 \
                             ( ( (pilot << PILOT_SHIFT) & PILOT_MASK ) |       \
                               (non_coherent & NON_COH_MASK) ) )

/* macro to set pilot index (PILOT), burst length and
    non-coherent int. (N) */
/*  (not used in MSM6100) */
#define SRCH4_PILOT_BURST_N( queue, task, pilot, burst_len, non_coherent )     \
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + PILOT_N ),                 \
                             ( ( (pilot << PILOT_SHIFT) &                      \
                                 PILOT_MASK ) |                                \
                               ( (burst_len << BURST_SHIFT) &                  \
                                 BURST_MASK ) |                                \
                               (non_coherent & NON_COH_BURST_MASK) ) )

/* macro to set coherent Truncation (CT), QOF and walsh index */
#define SRCH4_DIV_CT_QOF_WALSH( queue, task, diversity, coh_trunc, qof, walsh )\
          mdsp_write_offset( srch4_mem_map[ queue ][ SRCH4I_SRCH_PARAMS_ADDR ],\
                             ( (task * TASK_SIZE) + DIV_CT_QOF_WALSH ),        \
                             ( ( (diversity << DIVERSITY_SHIFT ) &             \
                                 DIVERSITY_MASK ) |                            \
                               ( (coh_trunc << COH_TRUNC_SHIFT ) &             \
                                 COH_TRUNC_MASK ) |                            \
                               ( (qof << QOF_SHIFT) & QOF_MASK ) |             \
                               (walsh & WALSH_MASK) ) )


/*****************************************************************************
                     Macros to read searcher results
*****************************************************************************/

/* -------------------- Queue Results ------------------------------------- */

/* macro to get number of tasks that were completed */
#define SRCH4_NUM_RESULTS( queue )                                             \
          mdsp_read( srch4_mem_map[ queue ][ SRCH4I_NUM_RESULTS_ADDR ] )

/* macro to clear out the number of results */
#define SRCH4_CLR_NUM_RESULTS( queue )                                         \
          mdsp_write( srch4_mem_map[ queue ][ SRCH4I_NUM_RESULTS_ADDR ], 0 );

/* -------------------- Task Results -------------------------------------- */

/* macro to get the HPQ peak position in Cx2 */
#define SRCH4_PEAK_POS( queue, result, peak )                                  \
          mdsp_read_offset( srch4_mem_map[ queue ][ SRCH4I_RESULTS_ADDR ],     \
                            ( (result * RESULT_SIZE) +                         \
                              (peak * PEAK_SIZE) +                             \
                              PEAK_POS ) )

/* macro to get the HPQ peak energy */
#define SRCH4_PEAK_ENG( queue, result, peak )                                  \
          mdsp_read_offset( srch4_mem_map[ queue ][ SRCH4I_RESULTS_ADDR ],     \
                            ( (result * RESULT_SIZE) +                         \
                              (peak * PEAK_SIZE) +                             \
                              PEAK_ENG ) )

/* macro to get HPQ left shoulder energy */
#define SRCH4_LS_ENG( queue, result, peak )                                    \
          mdsp_read_offset( srch4_mem_map[ queue ][ SRCH4I_RESULTS_ADDR ],     \
                            ( (result * RESULT_SIZE) +                         \
                              (peak * PEAK_SIZE) +                             \
                              PEAK_LS_ENG ) )

/* macro to get HPQ right shoulder energy */
#define SRCH4_RS_ENG( queue, result, peak )                                    \
          mdsp_read_offset( srch4_mem_map[ queue ][ SRCH4I_RESULTS_ADDR ],     \
                            ( (result * RESULT_SIZE) +                         \
                              (peak * PEAK_SIZE) +                             \
                              PEAK_RS_ENG ) )

#endif /* SRCH4DRVI_H */

