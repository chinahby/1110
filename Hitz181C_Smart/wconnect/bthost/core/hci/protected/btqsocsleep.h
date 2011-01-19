#ifndef _BTQSOCSLEEP_H
#define _BTQSOCSLEEP_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_QSOC
#ifdef FEATURE_BT_QSOC_SLEEP

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       B L U E T O O T H    Q S O C    S L E E P    H E A D E R    F I L E

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/protected/btqsocsleep.h#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $
*   #6         15 Aug 2008           SA
*   For Platform that use low speed uart (460 Kbps) increased the busy wait time to  
*   enter UART break condition correctly.
*
*   #5         28 Feb 2008           SSK
*   Optimise Inband SM to enter TX BREAK only after waiting for TX path to go idle.
* 
*   #4         10 Jan 2008           SSK
*   Added new MACROES to Inband Sleep SM, Changed RX BREAK\TX BREAK intervals
*
*   #3         29 Nov 2007           SSK
*   Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
* 
*   #2         26 Apr  2007          BH
*   Added bt_qsoc_sleep_wake_soc, so that the client can attempt to 
*   fully wake the SoC. 
*
*   #1         06 Apr  2007          BH
*   Created new file for BT QSoC Sleep support.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

/*=========================================================================*/
/*                               CONSTANTS                                 */
/*=========================================================================*/
/* None. */

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/

/* BT MSM sleep states */
typedef enum
{
    BT_QSOC_SLEEP_MSM_STATE_UNDEF = 0, /* 0 */
    BT_QSOC_SLEEP_MSM_STATE_INIT,      /* 1 */
    BT_QSOC_SLEEP_MSM_STATE_ASLEEP,    /* 2 */
    BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT,/* 3 */
    BT_QSOC_SLEEP_MSM_STATE_AWAKE,     /* 4 */
    BT_QSOC_SLEEP_MSM_STATE_MAX_STATE  /* 5 */ 
} bt_qsoc_sleep_msm_state_type;

/* BT SOC sleep states */
typedef enum
{
    BT_QSOC_SLEEP_SOC_STATE_UNDEF = 0, /* 0 */
    BT_QSOC_SLEEP_SOC_STATE_INIT,      /* 1 */
    BT_QSOC_SLEEP_SOC_STATE_ASLEEP,    /* 2 */
    BT_QSOC_SLEEP_SOC_STATE_AWAKE,     /* 3 */
    BT_QSOC_SLEEP_SOC_STATE_MAX_STATE  /* 4 */
} bt_qsoc_sleep_soc_state_type;


#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
/* BT Inband Sleep MSM sleep states */
typedef enum
{
   /* Undefined INBAND TX SM state */
    BT_QSOC_ISLEEP_MSM_TX_STATE_UNDEF = 0, /* 0 */
    /* INBAND TX SM has finished TXing data to SOC*/
    BT_QSOC_ISLEEP_MSM_TX_DONE,            /* 1 */
    /* INBAND TX SM has started TX BREAK to enable it to go to Inband TX sleep*/
    BT_QSOC_ISLEEP_MSM_TX_START_BREAK,     /* 2 */
    /* INBAND TX SM has just finished  TX BREAK CONDITION*/
    BT_QSOC_ISLEEP_MSM_TX_DONE_BREAK,      /* 3 */
    /* INBAND TX SM has entered TX BREAK CONDTION and voted to Inband TX sleep*/
    BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP,     /* 4 */
   /* INBAND TX SM has sent a wakeup mesg to SOC as it needs to TX data */
    BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK,/* 5 */
   /* INBAND TX SM is not yet asleep and is TXing data to SOC */
    BT_QSOC_ISLEEP_MSM_TX_ACTIVE_TRANSMIT  /* 6 */
} bt_qsoc_sleep_inband_msm_tx_state_type;


typedef enum
{
   /* Undefined INBAND RX SM state */
    BT_QSOC_ISLEEP_MSM_RX_STATE_UNDEF = 0, /* 0 */
   /* INBAND RX SM is not yet asleep and is recieving data or waiting for data */
    BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT,   /* 1 */
   /* INBAND RX SM has detected a RX BREAK CONDTION */
    BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP,     /* 2 */
    /* The RX path has been flushed of stale date after recieving RX BREAK*/
    BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED,    /* 3 */
   /* Inband RX SM has recvd a wakeup from SOC*/
    BT_QSOC_ISLEEP_MSM_RX_UNBRK_RCVD,      /* 4 */
   /* Inband RX SM is processing RX data */
    BT_QSOC_ISLEEP_MSM_RX_PROCESSING_PKT   /* 5 */
} bt_qsoc_sleep_inband_msm_rx_state_type; 
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

/*=========================================================================*/
/*                           DATA DECLARATIONS                             */
/*=========================================================================*/
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
extern bt_qsoc_sleep_inband_msm_tx_state_type  bt_qsoc_sleep_inband_tx_state;
extern bt_qsoc_sleep_inband_msm_rx_state_type  bt_qsoc_sleep_inband_rx_state;

extern rex_timer_type   bt_qsoc_inband_rx_brk_timer;

extern rex_timer_type   bt_qsoc_retry_timer;

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
/*=========================================================================*/
/*                                MACROS                                   */
/*=========================================================================*/

/* Signals used for sleep timeout and signaling the BT task from an ISR
   This signal is used to wait befor going to Inband TX sleep(TX BRK) for TX path  to flush*/
#define BT_QSOC_SLEEP_SIG                  BT_SOC_SIG3 /* 0x00000200 */

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP

#define BT_QSOC_INBAND_BREAK_RX_SIG        BT_SOC_SIG4 /* 0x00040000 */

/* BT_QSOC_HCI_RESET_FLUSH_SIG(BT_SOC_SIG2) is reused once SOC Init is complete*/
#define BT_QSOC_INBAND_RX_FLUSH_SIG        BT_SOC_SIG2 /* 0x00000100 */

/* BT_QSOC_RETRY_SIG(BT_SOC_SIG1) is reused once SOC Init is complete*/
#define BT_QSOC_INBAND_TX_ACK_REQD_SIG     BT_SOC_SIG1 /* 0x00000004 */

#else
#define BT_QSOC_SLEEP_T_SIG                BT_SOC_SIG4 /* 0x00040000 */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP

/* 10ms wait for TX path to be empty befor we enter TX BREAK condition */
#define  BT_QSOC_INBAND_TX_FLUSH_TIMEOUT      0x0A 

 /* 10ms wait for RX path buffers to be empty before voting for RX INBAND SLEEP*/
#define  BT_QSOC_INBAND_RX_FLUSH_TIMEOUT      0x0A       

#ifdef FEATURE_BT_SOC_BITRATE_460800
/* At 460 kbps. 10 bits need 2.174 usecs* 10 = 21.74 micro seconds busy wait
   30 micro secs  busy wait for 10 bit Tx  Break at 460 kbps */
#define  BT_QSOC_SLEEP_TX_BRK_INTVL           0x1E
#else
/* At 3.2 mpbs. 10 bits need 0.3125 usecs* 10 = 3.125 micro seconds busy wait
   4 micro secs  busy wait for 10 bit Tx  Break at 3.2 mbps */
#define  BT_QSOC_SLEEP_TX_BRK_INTVL           0x4
#endif /* FEATURE_BT_SOC_BITRATE_460800 */

/* 4 milli secs inactivity wait for detecting RX Break condition at 3.2 mbps */
#define  BT_QSOC_SLEEP_RX_BRK_INTVL           0x4

/* 48 milli secs for working around flow control */
#define  BT_QSOC_INBAND_CTS_DELAY             0x30 

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

/*  The following constants are for the bt_qsoc_sleep_decision_table
    structure.

    LSB                                         MSB
   _________________________________________________
   |  0 |  1 |  2 |    3    |  4  |  5  |6|7|...|15|
   |____|____|____|_________|_____|_____|_|_|___|__|
   | HW | TX | RX |   BT    | CMD | ACL |          |
   |    |    |    |   Q     |  Q  |  Q  | RESERVED |<-bt_qsoc_sleep_wake_conditions
   ------------------------------------------------
   | 1  | 1  | 1  | C OR A  | CMD | ACL | RESERVED |
   |    |    |    |  flow   |flow |FLOW |          |<-bt_qsoc_sleep_wake_control              
   -------------------------------------------------
*/

/* Wake Conditions Bits                                     */                  
/* -------------------------------------------------------- */
#define BT_QSOC_SLEEP_HOST_WAKE_BIT          0x0001
#define BT_QSOC_SLEEP_SIO_TX_BIT             0x0002
#define BT_QSOC_SLEEP_SIO_RX_BIT             0x0004

#define BT_QSOC_SLEEP_BT_BIT                 0x0008
#define BT_QSOC_SLEEP_CMD_BIT                0x0010
#define BT_QSOC_SLEEP_ACL_BIT                0x0020

#define BT_QSOC_SLEEP_TIMER                  0x0000

#define BT_QSOC_SLEEP_BITS          (  BT_QSOC_SLEEP_HOST_WAKE_BIT | \
                                    BT_QSOC_SLEEP_SIO_TX_BIT | \
                                    BT_QSOC_SLEEP_SIO_RX_BIT | \
                                    BT_QSOC_SLEEP_BT_BIT | \
                                    BT_QSOC_SLEEP_CMD_BIT | \
                                    BT_QSOC_SLEEP_ACL_BIT )

#define BT_QSOC_SLEEP_CONTROL_BIT   0x8000  /* condition or control */
#define BT_QSOC_SLEEP_ENABLE_BIT    0x4000  /* Set bit or clear bit */


/* Sleep events - Bitfields instead of enums for speed */
#define BT_QSOC_SLEEP_EVENT_HOST_WAKE_DISABLED  (BT_QSOC_SLEEP_HOST_WAKE_BIT)
                                                /* 0x0001 */

#define BT_QSOC_SLEEP_EVENT_HOST_WAKE_ENABLED    (BT_QSOC_SLEEP_HOST_WAKE_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0x4001 */

#define BT_QSOC_SLEEP_EVENT_SIO_TX_IDLE          (BT_QSOC_SLEEP_SIO_TX_BIT)
                                                /* 0x0002 */

#define BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY          (BT_QSOC_SLEEP_SIO_TX_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0x4002 */

#define BT_QSOC_SLEEP_EVENT_SIO_RX_IDLE          (BT_QSOC_SLEEP_SIO_RX_BIT)
                                                /* 0x0004 */

#define BT_QSOC_SLEEP_EVENT_SIO_RX_BUSY          (BT_QSOC_SLEEP_SIO_RX_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0x4004 */

#define BT_QSOC_SLEEP_EVENT_BT_Q_EMPTY           (BT_QSOC_SLEEP_BT_BIT)
                                                /* 0x0008 */

#define BT_QSOC_SLEEP_EVENT_BT_Q_NONEMPTY        (BT_QSOC_SLEEP_BT_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0x4008 */

#define BT_QSOC_SLEEP_EVENT_CMD_Q_EMPTY          (BT_QSOC_SLEEP_CMD_BIT)
                                                /* 0x0010 */

#define BT_QSOC_SLEEP_EVENT_CMD_Q_NONEMPTY       (BT_QSOC_SLEEP_CMD_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0x4010 */

#define BT_QSOC_SLEEP_EVENT_ACL_Q_EMPTY          (BT_QSOC_SLEEP_ACL_BIT)
                                                /* 0x0020 */

#define BT_QSOC_SLEEP_EVENT_ACL_Q_NONEMPTY       (BT_QSOC_SLEEP_ACL_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0x4020 */

#define BT_QSOC_SLEEP_EVENT_HCI_CMD_FLOWED_OFF   (BT_QSOC_SLEEP_CMD_BIT \
                                                | BT_QSOC_SLEEP_CONTROL_BIT)
                                                /* 0x8010 */

#define BT_QSOC_SLEEP_EVENT_HCI_CMD_FLOWED_ON    (BT_QSOC_SLEEP_CMD_BIT \
                                                | BT_QSOC_SLEEP_CONTROL_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0xC010 */

#define BT_QSOC_SLEEP_EVENT_HCI_ACL_FLOWED_OFF   (BT_QSOC_SLEEP_ACL_BIT \
                                                | BT_QSOC_SLEEP_CONTROL_BIT)
                                                /* 0x8020 */

#define BT_QSOC_SLEEP_EVENT_HCI_ACL_FLOWED_ON    (BT_QSOC_SLEEP_ACL_BIT \
                                                | BT_QSOC_SLEEP_CONTROL_BIT \
                                                | BT_QSOC_SLEEP_ENABLE_BIT)
                                                /* 0xC020 */

/* 
    The sleep timer neither sets or clears any bits and is not tracked.
    This timer is only an event which signals the sleep state machine
    that the RX path is now empty.
*/
#define BT_QSOC_SLEEP_EVENT_SLEEP_TIMER          (BT_QSOC_SLEEP_TIMER)
                                                /* 0x0000  */


#ifdef FEATURE_BT_QSOC_INBAND_SLEEP

#define BT_QSOC_SLEEP_INBAND_SET_WAIT_UNBRK_ACK() \
  ( bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK )\

/* Cancel the TX Path Idle timer */
#define BT_QSOC_INBAND_CLR_TX_FLUSH_TIMER()                \
  {                                                        \
    (void)rex_clr_timer( &bt_qsoc_inband_tx_flush_timer ); \
    (void)rex_clr_sigs( &bt_tcb, BT_QSOC_SLEEP_SIG );      \
  }

#define BT_QSOC_SLEEP_INBAND_WAIT_WAKEUP_ACK() \
  ( ( bt_qsoc_sleep_inband_tx_state==BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK )\
     ? TRUE:FALSE)


#define BT_QSOC_SLEEP_INBAND_TX_NOT_READY() \
  ( ( ( bt_qsoc_sleep_inband_tx_state==BT_QSOC_ISLEEP_MSM_TX_START_BREAK )\
    ||( bt_qsoc_sleep_inband_tx_state==BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK ))\
     ? TRUE:FALSE)

/* Checks if the INBAND SLEEP TX Path  has started Break state */
#define BT_QSOC_SLEEP_INBAND_TX_IN_SLEEP() \
  (( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP )\
     ? TRUE:FALSE )

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

/* Conditional Inband Debug Log Enabling */               

#if defined( FEATURE_BT_QSOC_INBAND_SLEEP ) && defined( BT_HC_DRV_INBAND_TRACE_ENABLE )
#define BT_LOG_INBAND_STATE( fmt )                                         \
                             BT_MSG_HIGH((#fmt " st1:%d st2:%d st3:%d"),   \
                             bt_qsoc_sleep_inband_rx_state,                \
                             bt_qsoc_sleep_inband_tx_state,                \
                             bt_qsoc_sleep_sm_get_msm_state() )
#else
#define BT_LOG_INBAND_STATE( fmt )
#endif /* BT_HC_DRV_INBAND_TRACE_ENABLE */

#if defined( FEATURE_BT_QSOC_INBAND_SLEEP ) && defined( BT_QSOC_INBAND_TRACE_ENABLE )
#define BT_MSG_INBAND_HIGH( fmt, a, b, c )       BT_MSG_HIGH( fmt, a, b, c )
#define BT_MSG_INBAND_DEBUG( fmt, a, b, c )      BT_MSG_DEBUG( fmt, a, b, c )
#else
#define BT_MSG_INBAND_HIGH( fmt, a, b, c )
#define BT_MSG_INBAND_DEBUG( fmt, a, b, c )
#endif /* BT_QSOC_INBAND_TRACE_ENABLE */

/*=========================================================================*/
/*                           FUNCTION INTERFACES                           */
/*=========================================================================*/

/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_RESET

  DESCRIPTION    Sets all sleep variables to reset values.  Ensures the 
                 sleep state machines are in uninitialized states.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_sleep_sm_reset
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_INIT

  DESCRIPTION    Initializes both the MSM and SoC Sleep state machines.  
                 Before initialization, events are tracked but no actions 
                 are taken.

  DEPENDENCIES   bt_qsoc_sleep_sm_reset should be called on BT task
                 initialization.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_sleep_sm_init
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_EVAL_EVENT

  DESCRIPTION    Tracks and evaluates sleep related events for both the MSM
                 and SoC state machines.  Only if the update_msm_state 
                 parameter is TRUE, will the MSM transition to the new state.
                 However, regardless of the update_msm_state parameter, the 
                 SoC will be transition to the new state.

                 If this function is called with the update_msm_state set to 
                 FALSE, the bt_qsoc_sleep_sm_update_msm_state function should 
                 be called later to actually move the MSM to the new state.

  DEPENDENCIES   None.

  PARAMETERS     event            - New sleep related event.
                 update_msm_state - Whether MSM should transition to the 
                                    next sleep state.

  RETURN VALUE   None.

  SIDE EFFECTS   Interrupts are locked while modifying the decision table.

==========================================================================*/
extern void bt_qsoc_sleep_sm_eval_event
(
  uint16 event,
  boolean update_msm_state
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_UPDATE_MSM_STATE

  DESCRIPTION    Updates the MSM state to reflect changes in the decision 
                 table.  Since the MSM state may not get updated in an 
                 interrupt context, this function is called in the task 
                 context to ensure the MSM is moved to the correct state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   Interrupts are locked while reading the decision table.

==========================================================================*/
extern void bt_qsoc_sleep_sm_update_msm_state
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_GET_MSM_STATE

  DESCRIPTION    Returns the current MSM sleep state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current MSM sleep state.

  SIDE EFFECTS   None.

==========================================================================*/
extern bt_qsoc_sleep_msm_state_type bt_qsoc_sleep_sm_get_msm_state
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_GET_PENDING_MSM_STATE

  DESCRIPTION    Since the MSM may not always move to the correct state 
                 immediately, this function will return the state the MSM 
                 should be in. Checking this state will tell the caller if
                 the MSM should update it's state later.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current MSM sleep state based off the decision table.

  SIDE EFFECTS   None.

==========================================================================*/
extern bt_qsoc_sleep_msm_state_type bt_qsoc_sleep_sm_get_pending_msm_state
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_GET_SOC_STATE

  DESCRIPTION    Since the MSM may not always move to the correct state 
                 immediately, this function will return the state the MSM 
                 should be in. Checking this state will tell the caller if
                 the MSM should update it's state later.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current SoC sleep state.

  SIDE EFFECTS   None.

==========================================================================*/
extern bt_qsoc_sleep_soc_state_type bt_qsoc_sleep_sm_get_soc_state
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ISSUE_TX_FLUSH

  DESCRIPTION    Issues a TX flush to SIO so that the sleep state 
                 machine is notified when transmissions are complete.

  DEPENDENCIES   None.

  PARAMETERS     stream_id_to_flush - the SIO stream to flush.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_issue_tx_flush
(
  sio_stream_id_type stream_id_to_flush
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_WAKE_SOC

  DESCRIPTION    Sends a wake signal to the SoC and attempts to wait for the
                 SoC to be completely awake.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   The thread will be suspended for 1 ms, up to 5 times, 
                 while it waits for the SoC to wake.

==========================================================================*/
void bt_qsoc_sleep_wake_soc
(
  void
);

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
/*==========================================================================

  FUNCTION       BT_QSOC_INBAND_RX_UNBREAK

  DESCRIPTION    This callback is called by UART driver when RX line goes high.
  
  DEPENDENCIES   None.

  PARAMETERS     new_msm_state - the new state to which the MSM should move.

  RETURN VALUE   None.

  SIDE EFFECTS  This function executes from ISR context with INTs potentially
                locked. No blocking operations should be done here.

==========================================================================*/
void bt_qsoc_inband_rx_unbreak
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_WAKE_SOC_INBAND

  DESCRIPTION    Sends a Inband wakeup signal to the SoC and  wait for the
                 SoC to be completely awake.  
                 
  DEPENDENCIES   bt_qsoc_sleep_sm_init().

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   The thread will be suspended for 1 ms, up to
                 BT_QSOC_SLEEP_WAKE_ATTEMPTS times, while it waits for the SoC
                 to wake.

==========================================================================*/
void bt_qsoc_sleep_wake_soc_inband
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_TX_ACK_REQD_SIG

  DESCRIPTION    Process the signal to ACK a TX Unbreak from the Peer.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_inband_proc_tx_ack_reqd_sig
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_INIT

  DESCRIPTION    Initializes the MSM inband sleep state machines to low states

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   bt_soc_etc_init() which is called immediatly sends out
                 HCI commands and causes TX line to go high.

==========================================================================*/
void bt_qsoc_sleep_inband_init
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_RX_BRK_EVT

  DESCRIPTION    Process a RX Break Event which means the Inband Sleep
                 RX path has voted to sleep.  

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS
==========================================================================*/
void bt_qsoc_sleep_inband_proc_rx_brk_evt
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_TX_BRK_EVT

  DESCRIPTION    Process a TX Break Event which means the Inband Sleep
                 TX path has voted to sleep.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_inband_proc_tx_brk_evt
(
  void
);

/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_IN_BRK

  DESCRIPTION    Checks if the INBAND SLEEP TX Path  has started Break state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   If TX BREAK state has started, it is non-premptible and no
                 TX should be attempted until TX BREAK state has completed.

==========================================================================*/
boolean bt_qsoc_sleep_inband_in_brk
(
  void
);

/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_RX_NEW_PKT

  DESCRIPTION    Updates the Inband RX state  machine about new Rx activity

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   Rx state machines stays awake.
  
==========================================================================*/
void bt_qsoc_sleep_inband_rx_new_pkt
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_LOG_RX_ACTIVITY

  DESCRIPTION    Logs Inband Rx activity recived in a invalid state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.
  
==========================================================================*/
void bt_qsoc_sleep_inband_log_rx_activity
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_GET_TX_STATE

  DESCRIPTION    Returns the current Inband sleep Tx state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current Inband sleep TX SM state.

  SIDE EFFECTS   None.

==========================================================================*/
bt_qsoc_sleep_inband_msm_tx_state_type bt_qsoc_sleep_inband_get_tx_state
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_GET_RX_STATE

  DESCRIPTION    Returns the current Inband sleep Rx state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current Inband sleep RX SM state.

  SIDE EFFECTS   None.

==========================================================================*/
bt_qsoc_sleep_inband_msm_rx_state_type bt_qsoc_sleep_inband_get_rx_state
(
  void
);

/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_RX_FLUSH_SIG

  DESCRIPTION    This function processes "UART\DMOV RX" buffers are empty evt.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_inband_proc_rx_flush_sig
( 
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_TX_FLUSH_SIG

  DESCRIPTION    This function processes "UART\DMOV RX" buffers are empty evt.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_inband_proc_tx_flush_sig
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_ENTER_TX_BREAK

  DESCRIPTION    This call causes INBAND SM to enter the TX BREAK State indicating
                 that the TX PATH has voted to sleep. This call is triggered after
                 MSM enters the SLEEP_WAIT state and the FLUSH TX timer fires. 

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   No TX can be attempted until MSM exits TX BRK and gets a UNBRK
                 ACK from SOC.

==========================================================================*/
void bt_qsoc_sleep_inband_enter_tx_brk
(
  void
);

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
#endif /* FEATURE_BT_QSOC_SLEEP */
#endif /* FEATURE_BT_QSOC */
#endif /* FEATURE_BT */
#endif /* _BTQSOCSLEEP_H*/
