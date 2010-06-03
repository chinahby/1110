#ifndef CUSTRELA_H
#define CUSTRELA_H
/*===========================================================================

            " M S M 6 5 0 0 -  S U R F "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SURF Target using MSM6500 ARM Processor.
  This is a Baseband build with IS2000 with Release A if Rel_A is not undefined.

  Copyright (c) 1999,2000,2001 by QUALCOMM Incorporated. All Rights Reserved.
  Copyright (c) 2002,2003,2004 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/build/rel/ulc/custrela.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/04   yll     Enabled FEATURE_APP_SDBTEST
05/07/04   ejv     Removed comment.
03/31/04   yll     Added FEATURE_IS2000_REL_A_CC_QPM.
11/04/03   yll     Added FEATURE_DYNAMIC_P_REV.
08/28/03   yll     Added FEATURE_IS2000_DYNAMIC_FEATURE.
05/29/03   lh      Added FEATURE_APP_SDBTEST.
04/15/03   jrp     Add FEATURE_DELAYED_TX_CLOCK_SHUTDOWN
03/17/03   ph      Add and Enable FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
11/27/02   lh      Added FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO and define it
                   based on T_MSM6100.
10/31/02   ph      Turned on FEATURE_SRCH_SCHEDULER independent of Rel A
10/31/02   ph      Enabled BCSMS on REL A
10/02/02   fc      Added FEATURE_IS2000_REL_A_CC_SMMC.
08/13/02   ph      added FEATURE_MCC_POWERUP_REG_COUNT
08/06/02   jrp     Added FEATURE_IS2000_REL_A_REACH_10MS
                   and FEATURE_IS2000_REL_A_REACH_5MS.
07/18/02   ph      added FEATURE_DBM
07/18/02   ph      added FEATURE_SEC_CALLS_SEARCHER_RANDOM
07/17/02   ph      Added FEATURE_DATA_IS2000_SDB
07/02/02   lh      Added FEATURE_DS_IS2000_RS2_16X_F_SCH and
                   FEATURE_DS_IS2000_RS2_16X_R_SCH, for RelA build temporarily.
06/28/02   lh      Added FEATURE_IS2000_REL_A_MUXPDU5,
                     FEATURE_IS2000_REL_A_MUXPDU5_32X_F_SCH and
                     FEATURE_IS2000_REL_A_MUXPDU5_32X_R_SCH.
06/19/02   ht      Added conditions to allow a Rel_0 build with Rel_A undefined.
04/19/02   ph      turned on FEATURE_SRCH_SCHEDULER.
04/10/02   yll     Added FEATURE_IS2000_REL_A_BCSMS
01/11/02   va      Defined FEATURE_IS2000_REL_A_SVD and FEATURE_CM_MULTIMODE
08/27/01   kk      Undefined FEATURE_TC_EXTENDED_MSG_LENGTH.
08/16/01   kk      Added FEATURE_UART_RATE1 to enable high data rates.
07/25/01   kk      Moved 2.0 features to main cust files
                   custcdea.h and custstea.h
07/19/01   kk      Added Puma 2.0 Flags. Rate Set 2 Flags left undefined.
07/02/01   ph      Updated to include REX+ features for callback timers
04/12/01   kk      Initial Rev.
===========================================================================*/

 /* Release A Feature Flag */
#define FEATURE_IS2000_REL_A

/* Added to allow use of REX callback timer mechanism */
#define FEATURE_REX_TIMER_EX

/* Turns on asynchronous procedure call support. Allows the user to queue
 * a function to be called in the context of a specific task. When the
 * task for which the APC is queued is scheduled for execution next,
 * the APC is called immediately.
*/
#define FEATURE_REX_APC

/* feature to store in NV count of registrations */
#define FEATURE_MCC_POWERUP_REG_COUNT

/* Added to enable high data rates on the UART */
#define FEATURE_UART_RATE1

/* Allow firmware to take care of the ULPN mask for QPCH.
*/
#undef FEATURE_HW_NO_QPCH_ULPN

/* Extended message length on traffic channel.
*/
#undef FEATURE_TC_EXTENDED_MSG_LENGTH

/* SRCH related features */
/*Undefine this feature to turn on Mobile Ip DMU feature*/
#undef FEATURE_SEC_CALLS_SEARCHER_RANDOM
/* MSM6500 does not use scheduler */
#undef FEATURE_SRCH_SCHEDULER

/* CM related features */

/* SDB */
#ifdef FEATURE_DBM
  #define FEATURE_DATA_IS2000_SDB
#endif

/********************************************************/
/********************************************************/
/********************************************************/
/*       IS2000 RELEASE A                               */
/********************************************************/
/********************************************************/
/********************************************************/
#ifdef FEATURE_IS2000_REL_A

  /* sanity check */
  #ifndef FEATURE_ZZ2
  #error FEATURE_IS2000_REL_A needs FEATURE_ZZ2 to be turned on
  #endif

  /* to enable 32X Data Rate */
  #define FEATURE_32X_SCH_DATA_RATE

  /* Enable MUXPDU5 */
  #define FEATURE_IS2000_REL_A_MUXPDU5

  /* Enable 32X FSCH */
  #define FEATURE_IS2000_REL_A_MUXPDU5_32X_F_SCH

  #ifdef T_MSM6100
    /* Enable 32X RSCH */
    #define FEATURE_IS2000_REL_A_MUXPDU5_32X_R_SCH

    /* Enable 32X Turbo coding */
    #define FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO

  #else
#error code not present
  #endif

  /* The following two features are not strictly relA only, but
  ** they are only enabled on relA builds for testing purose,
  ** and are put here to avoid affection Rel0 builds.
  */
  /* To enable RS2 16X data rate on F-SCH */
  #define FEATURE_DS_IS2000_RS2_16X_F_SCH

  /* To enable RS2 16X data rate on R-SCH */
  #define FEATURE_DS_IS2000_RS2_16X_R_SCH

  /* to enable Transmit Diversity Feature */
  #define FEATURE_IS2000_REL_A_TD

  /* Added Release A Logging Feature */
  #define FEATURE_IS2000_RELEASE_A_LOGGING

  /* Enable simulataneous Voice and Data feature by default */
  #define FEATURE_IS2000_REL_A_SVD

  /* Support sending Base Station Status Req/process Rsp for GPS */
  #define FEATURE_IS2000_REL_A_BSID

  /* Enable Release A Common Channel support */
  #define FEATURE_IS2000_REL_A_CC

  /* Enable Sync Channel workaround */
  #define FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND

  /* Enable simulataneous monitoring multiple Release A Common Channels */
  #define FEATURE_IS2000_REL_A_CC_SMMC

  /* Enable the Quick Page Match on Release A Common Channels */
  #define FEATURE_IS2000_REL_A_CC_QPM
  
  /* Allow selection of 10ms frame length on R-EACH
  */
  #define FEATURE_IS2000_REL_A_REACH_10MS
  /* Allow selection of 5ms frame length on R-EACH.
  */
  #define FEATURE_IS2000_REL_A_REACH_5MS

  /* To enable Broadcast SMS on the Rel A Common Channel.
  ** FEATURE_IS2000_REL_A_CC and FEATURE_BROADCAST_SMS have
  ** to be defined in order to define this feature.
  ** See customer.h for the checkings.
  */
#ifndef FEATURE_STANDALONE_MODEM
  #define FEATURE_IS2000_REL_A_CC_BCSMS
#endif

  /* Process the unscheduled Broadcast SMS message. */
  #define FEATURE_IS2000_REL_A_CC_BCSMS_PROC_UNSCHED

  #ifdef FEATURE_PROPRIETARY_INVESTIGATION_1
    #define FEATURE_SO_8026
    #define FEATURE_DELAYED_TX_CLOCK_SHUTDOWN
    #define FEATURE_MODIFIED_SCI
  #endif

  /* SDB Test app */
  #define FEATURE_APP_SDBTEST

  /* Enable the Dynamic Feature support. */
  #define FEATURE_IS2000_DYNAMIC_FEATURE

  /* During powerup, if DF verification fails, enable this
  ** feature will set DF to the default value and continue
  ** running online. Disable this feature the mobile will
  ** go offline waiting for service programming.
  */
  #define FEATURE_IS2000_DYNAMIC_FEATURE_ERR_USE_DEFAULT

  /* Enable the Dynamic P_REV */
  #define FEATURE_DYNAMIC_P_REV

  /* Warning!!! Don't undef this feature.
  ** Default the Dynamic P_REV to 6.
  **/
  #define FEATURE_DYNAMIC_P_REV_DEFAULT_REL_0

  #ifdef T_MSM6500
    #if (!defined(FEATURE_DYNAMIC_P_REV) || !defined(FEATURE_DYNAMIC_P_REV_DEFAULT_REL_0))
    #error Are you sure you want a P_REV 7 build??
    #endif
  #endif

#endif /* FEATURE_IS2000_REL_A */

#endif /* CUSTRELA_H */

