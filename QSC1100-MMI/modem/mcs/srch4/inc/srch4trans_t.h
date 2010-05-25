#ifndef SRCH4TRANS_T_H
#define SRCH4TRANS_T_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   T R A N S L A T I O N S
                      T Y P E D E F   H E A D E R F I L E

GENERAL DESCRIPTION
   This file contains the exported structs and defines for searcher 4's
   translation layer.

      Copyright (c) 2003 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/inc/srch4trans_t.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/08   adw     Added SRCH4_AFLT_ENABLED & SRCH4_HDR_ENABLED.
06/20/08   mca     Merge from //depot
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
04/24/08   ljl     Changed the task number to 28 for MSM7800
01/19/07   sst     Create srch4_tech_type to replace defines
01/16/06   awj     Lint cleanup
10/06/05   bt      Added SRCH4_MIN_WINDOW_SIZE
07/05/05   sst     Update comment for SRCH4_MAX_WINDOW_SIZE
05/17/05   sst     Defined SRCH4_MAX_WINDOW_SIZE
07/13/04   sst     Changed SRCH4_MAX_TYPE to SRCH4_MAX_TECH in srch4_srch_type
                   Added SRCH4_NO_TECH = SRCH4_MAX_TECH in srch4_srch_type
                   Changed SRCH4_NO_TECH to SRCH4_NO_TECH_MASK
06/25/04   jcm     Mainlined FEATURE_HDR, SRCH4_HDR_ENABLED, and
                   SRCH4_AFLT_ENABLED
05/04/04   sst     Merge from srch4 3Q dev branch
05/03/04   sst     Moved COH_TRUNC_* defines from srch4drv_t.h to be exported
04/15/04   sst     More code cleanup
04/12/04   sst     Moved srch4_tech_params_struct_type to srch4trans.c
                   Removed debug include file
04/08/04   sst     Code cleanup
03/25/04   sst     Added SRCH4_RET_APP_NOT_VALID and SRCH4_PRIO_MIN
03/11/04   sst     Added search priority enum
03/04/04   sst     First cut of MDSP mechanism rework
02/20/04   sst     Intermediate Checkin
02/12/04   sst     Intermediate Checkin
12/05/03   sst     initial implementation, broken out from srch4drv.h

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"

#include "comdef.h"
#include "srch_hw_t.h"


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/

/* External "re-definitions" for internal defines */

/* define the maximum allowable 1x srch4 window size in chips */
#define SRCH4_MAX_WINDOW_SIZE  32767

/* dfine the minimum 1X srch4 window size in chips */
#define SRCH4_MIN_WINDOW_SIZE  4

/* Max number of tasks/sectors programmable for HPQ and LPQ searches */
/* Make sure SRCH4_MAX_NUM_TASKS is set correctly */
#ifdef T_MDM7800
#define SRCH4_MAX_NUM_TASKS      28
#else
#define SRCH4_MAX_NUM_TASKS      16
#endif /* T_MDM7800 */

/* for AFLT support */
#define SRCH4_MAX_AFLT_TASKS     40

/* coherent truncation value */
/*  (use determined by MSM6500 MDSP 1x/Searcher Interface, dated 9/2002)    */
#define COH_TRUNC_1        0x0 /* truncate by 1 bit                         */
#define COH_TRUNC_2        0x1 /* truncate by 2 bit(eqv to gain=16 in srch2)*/
#define COH_TRUNC_3        0x2 /* truncate by 3 bit(eqv to gain=8 in srch2) */
#define COH_TRUNC_4        0x3 /* truncate by 4 bit(eqv to gain=4 in srch2) */
#define COH_TRUNC_5        0x4 /* truncate by 5 bit(eqv to gain=2 in srch2) */
#define COH_TRUNC_6        0x5 /* truncate by 6 bit                         */
#define COH_TRUNC_7        0x6 /* truncate by 7 bit                         */
#define COH_TRUNC_8        0x7 /* truncate by 8 bit                         */

/* estimated search time constants (experimentally found) in us */
#define SRCH4_SW_QUEUE_OVHD_US  187     /* SW overhead for a queue (any siz) */
#define SRCH4_SW_TASK_OVHD_US    30     /* SW overhead for a task            */


#define SRCH4_TD_PROXIMITY        6     /* maximum proximity between common
                                           and diversity search Indexes    */

/* searcher 4 error return values */
enum {
  SRCH4_RET_FULL             =  -1,     /* queue is full                 */
  SRCH4_RET_ACTIVE           =  -2,     /* queue is active               */
  SRCH4_RET_UNCLEAN          =  -3,     /* last search wasn't cleaned up */
  SRCH4_RET_INVALID_QUEUE    =  -4,     /* no queue has been assigned    */
  SRCH4_RET_INVALID_PARAM    =  -5,     /* bad parameter was passed
                                           (null ptr)                    */
  SRCH4_RET_TECH_MISMATCH    =  -6,     /* technology mismatch           */
  SRCH4_RET_NO_QUEUES        =  -7,     /* no queues were avail for srch */
  SRCH4_RET_APP_NOT_VALID    =  -8,     /* MDSP app was not valid        */
  SRCH4_RET_FATAL            =  -9,     /* fatal errors                  */
  SRCH4_RET_OK               = -10,     /* no errors to report           */
};

/* retain old type for gps/aflt */
typedef int32 srch4_err_ret_values_type;

/*===========================================================================

           M D S P    A P P    R E L A T E D    S T R U C T S

===========================================================================*/
/* bit masks for technologies */
typedef enum
{
  SRCH4_NO_TECH_MASK    = 0x0,
  SRCH4_1X_MASK         = 0x1,
  SRCH4_HDR_MASK        = 0x2,
  SRCH4_AFLT_MASK       = 0x4,
  SRCH4_GPS_MASK        = 0x0,   /* this makes this non GPS dependent */
} srch4_tech_type;

#define SRCH4_MDSP_APP_CB_POOL_SIZE    4

/*===========================================================================

           T E C H N O L O G Y   M A P P I N G    S T R U C T S

===========================================================================*/
/* searcher 4 tech type */
/* the following enum can also be used as a mask, by the way it's designed
   if everything,   then one gets 1x=0, HDRs=1, HDRa=3, AFLT=4, NUM=5 
   if !HDR,         then one gets 1x=0, AFLT=1                , NUM=2
   if !AFLT,        then one gets 1x=0, HDRs=1, HDRa=3        , NUM=4
   if !HDR & !AFLT, then one gets 1x=0,                       , NUM=1


   */
typedef enum
{
  SRCH4_1X              = 0x00,          /* CDMA 1X search             */
  #ifdef SRCH4_HDR_ENABLED
  SRCH4_HDR_SHOULDERS,                   /* HDR search interpolation
                                            using shoulders            */
  SRCH4_RESERVED,                        /* not used, but keeps the mask
                                            correct */
  SRCH4_HDR_ACCUM,                       /* HDR search with actual
                                            accumulation energies      */
  #endif /* SRCH4_HDR_ENABLED */
  #ifdef SRCH4_AFLT_ENABLED
  SRCH4_AFLT,                            /* AFLT search (used to diff
                                            between 1x and AFLT srch,
                                            MDSP want's 0, but all but
                                            2 LSB bits are masked off) */
  #endif /* SRCH4_AFLT_ENABLED */
  SRCH4_NUM_TECH,                        /* number of technologies     */
  SRCH4_NO_TECH         = 0x08,          /* mask for no tech           */
   /* Masks to differentiate between normal 1x and offtl searching */
  SRCH4_1X_NORM_MASK  = 0x10,           
  SRCH4_1X_OFFTL_MASK = 0x20,
} srch4_srch_type;

/*===========================================================================

           S E A R C H    P R I O R I T Y    E N U M

===========================================================================*/
/* searcher 4 priority */
/* this list was derived from a MSM6500 memo on search scenarios */
/* the "real" priority of each scenario has been shifted left and or'd
   with it's technology type to differentiate priorities between
   technologies */
typedef enum
{
  SRCH4_PRIO_MIN                  = 0,
  #ifdef SRCH4_AFLT_ENABLED
  SRCH4_PRIO_AFLT                 = ( ( ( 1) << 3 ) | SRCH4_AFLT ),
  #endif /* SRCH4_AFLT_ENABLED */
  #ifdef SRCH4_HDR_ENABLED
  SRCH4_PRIO_HDR_ACQ              = ( ( ( 2) << 3 ) | SRCH4_HDR_SHOULDERS ),
  #endif /* SRCH4_HDR_ENABLED */
  SRCH4_PRIO_1X_ACQ               = ( ( ( 3) << 3 ) | SRCH4_1X ),
  #ifdef SRCH4_HDR_ENABLED
  SRCH4_PRIO_HDR_TRAFFIC          = ( ( ( 4) << 3 ) | SRCH4_HDR_SHOULDERS ),
  SRCH4_PRIO_HDR_NON_SLOTTED      = ( ( ( 4) << 3 ) | SRCH4_HDR_SHOULDERS ),
  #endif /* SRCH4_HDR_ENABLED */
  SRCH4_PRIO_1X_TRAFFIC           = ( ( ( 4) << 3 ) | SRCH4_1X ),
  SRCH4_PRIO_1X_NON_SLOTTED       = ( ( ( 4) << 3 ) | SRCH4_1X ),
  #ifdef SRCH4_HDR_ENABLED
  SRCH4_PRIO_HDR_TRAFFIC_CFS      = ( ( ( 5) << 3 ) | SRCH4_HDR_SHOULDERS ),
  SRCH4_PRIO_HDR_OFREQ            = ( ( ( 5) << 3 ) | SRCH4_HDR_SHOULDERS ),
  #endif /* SRCH4_HDR_ENABLED */
  SRCH4_PRIO_1X_TRAFFIC_CFS       = ( ( ( 5) << 3 ) | SRCH4_1X ),
  SRCH4_PRIO_1X_OFREQ             = ( ( ( 5) << 3 ) | SRCH4_1X ),
  #ifdef SRCH4_HDR_ENABLED
  SRCH4_PRIO_HDR_REACQ_BROADCAST  = ( ( ( 6) << 3 ) | SRCH4_HDR_SHOULDERS ),
  #endif /* SRCH4_HDR_ENABLED */
  SRCH4_PRIO_1X_REACQ_BROADCAST   = ( ( ( 7) << 3 ) | SRCH4_1X ),
  #ifdef SRCH4_HDR_ENABLED
  SRCH4_PRIO_HDR_REACQ_PAGING     = ( ( ( 8) << 3 ) | SRCH4_HDR_SHOULDERS ),
  #endif /* SRCH4_HDR_ENABLED */
  SRCH4_PRIO_1X_REACQ_PAGING      = ( ( ( 9) << 3 ) | SRCH4_1X ),
  SRCH4_PRIO_MAX                  = 10       /* note: this is the max value */
} srch4_priority_type;

#endif /* SRCH4TRANS_T_H */
