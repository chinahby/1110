#ifndef RX_H
#define RX_H

/*===========================================================================

          M A I N   R E C E I V E   T A S K   D E F I N I T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the Main Receive task.

Copyright (c) 1992-2005 by QUALCOMM Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/rx.h_v   1.0.2.0   30 Nov 2001 17:47:34   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/rx.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/06   fc      Used next RXC reserved signal for RXC_PC_FS_TIMER_SIG.
05/17/04   vlc     Used next RXC reserved signal for RXC_PDCH_INT_SIG to support
                   F-PDCH.
02/10/04   az      Added RXC_ACTION_TIME_SIG for action time notification
01/13/02   fc      Fixed compiler warnings.
11/07/02   az      Mainlined FEATURE_IS95B
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added RXC_AHO_FADE_TIMER timer for T72M timer.
08/31/92   jjw     Added ACPRX Sound command done signal 
08/19/92   jjw     Added ACPRX timer signal to check RSSI during ACPRX
                   NULL_STATE so DB (and UI) will also know signal strength
07/08/92   jca     Deleted references to RXCDMA sigs (prefix is now RXC).
07/07/92   arh     Initial check-in for DMSS
04/16/92   jca     Creation

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*             ACP RECEIVE SUBTASK (ACPRX) SIGNAL DECLARATIONS             */
/*-------------------------------------------------------------------------*/

/* Timer Signals                                                           */

#define ACPRX_DOG_TIMER_SIG    0x0001     /* watchdog timer sig            */
#define ACPRX_FADE_TIMER_SIG   0x0002     /* fade timer signal             */
#define ACPRX_STATE_TIMER_SIG  0x0004     /* cmd processing state timer    */
#define ACPRX_SCC_TIMER_SIG    0x0008     /* SAT color code interval check */
#define ACPRX_RSSI_TIMER_SIG   0x0040     /* Check RSSI timer signal check */
#define ACPRX_SND_CMD_DONE_SIG 0x0080     /* Sound command complete signal */

/* Queue Signals                                                           */

#define ACPRX_RXWORD_SIG       0x0010     /* rx word in queue              */
#define ACPRX_CMD_Q_SIG        0x0020     /* command in queue              */
/* (NOTE! This sig def must differ from the RXC cmd queue sig def.)        */

/* Currently not used                                                      */

#define ACPRX_RSVD_0100_SIG    0x0100     /* reserved signal               */
#define ACPRX_RSVD_0200_SIG    0x0200     /* reserved signal               */
#define ACPRX_RSVD_0400_SIG    0x0400     /* reserved signal               */
#define ACPRX_RSVD_0800_SIG    0x0800     /* reserved signal               */

/*-------------------------------------------------------------------------*/
/*             CDMA RECEIVE SUBTASK (RXC) SIGNAL DECLARATIONS              */
/*-------------------------------------------------------------------------*/

/* Currently not used                                                      */
#define  RXC_RPT_TIMER_SIG  0x0001
  /* This signal is set when the rx_rpt_timer expires. */

#define  RXC_FADE_TIMER_SIG  0x0002
  /* This signal is set when the rx_fade_timer expires. */

#define  RXC_CMD_Q_SIG  0x0004
  /* This signal is set when an item is put on the rx_cmd_q. */
  /*  (NOTE! This sig def must differ from the ACPRX cmd queue sig def.)   */

#define  RXC_INT_SIG  0x0008
  /* This signal is used for signalling the Receive Task from the 
     Decoder Interrupt Handler */

#define RXC_ISR_CMD_SIG 0x0010
  /* This signal is specified when issuing a command to the rx interrupt
     handler */

#define RXC_AHO_FADE_TIMER_SIG 0x0020
  /* This signal is set as a fade timer when T72M timer expires */

       
#define RXC_ACTION_TIME_SIG  0x0040
  /* Action time notification for RXC       
  */
                
#define RXC_PDCH_INT_SIG  0x0080
  /* This signal is used for signalling the Receive Task from the 
     F-PDCH Decoder Interrupt Handler */

#define RXC_PC_FS_TIMER_SIG  0x0100
  /* This signal is set when there is no valid message on BCCH or FCCCH for
     4 seconds */
 
#define RXC_RSVD_0200_SIG  0x0200
#define RXC_RSVD_0400_SIG  0x0400

/*-------------------------------------------------------------------------*/
/*             MAIN RECEIVE TASK SIGNAL DECLARATIONS                       */
/*-------------------------------------------------------------------------*/

#define RX_RPT_TIMER_SIG  0x0001 /* Watchdog report interval timer         */

/* Subtask Command Queue Signals                                           */

#if (RXC_CMD_Q_SIG == ACPRX_CMD_Q_SIG)
#error CMD_Q_SIGs cannot be equal
#endif

#define RX_CDMA_CMD_Q_SIG   RXC_CMD_Q_SIG
/* Indicates new item on the RXC subtask command queue                     */
#define RX_ACP_CMD_Q_SIG    ACPRX_CMD_Q_SIG
/* Indicates new item on the ACPRX subtask command queue                   */

/* Currently not used                                                      */
#define RX_RSVD_0002_SIG  0x0002
#define RX_RSVD_0004_SIG  0x0004 
#define RX_RSVD_0008_SIG  0x0008 
#define RX_RSVD_0010_SIG  0x0010 
#define RX_RSVD_0040_SIG  0x0040
#define RX_RSVD_0080_SIG  0x0080              
#define RX_RSVD_0100_SIG  0x0100 
#define RX_RSVD_0200_SIG  0x0200 
#define RX_RSVD_0400_SIG  0x0400 

/* Signals 0x1000, 0x2000, 0x4000, and 0x8000 are system wide common and
   are defined in TASK.H                                                   */

/* <EJECT> */
/*===========================================================================

                     PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RX_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Main Receive task.
  It contains the main processing loop for the Main Receive task which
  controls the activation of the ACPRX and RXC subtasks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void rx_task (
  dword dummy
    /* Required for REX, ignore */
);

#endif

