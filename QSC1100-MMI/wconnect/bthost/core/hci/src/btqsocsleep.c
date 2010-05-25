/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   S O C   S L E E P   M O D U L E

GENERAL DESCRIPTION
  This module contains the Bluetooth SoC sleep data and code.

EXTERNALIZED FUNCTIONS

  bt_qsoc_sleep_sm_reset()
    Sets all sleep related variables back to reset values.

  bt_qsoc_sleep_sm_init()
    Performs intialization sequence for sleep state machine.

  bt_qsoc_sleep_sm_eval_event()
    Processes events, updates SoC state and possibly the MSM state.

  bt_qsoc_sleep_sm_update_msm_state()
    Processes previous events and updates the MSM state.

  bt_qsoc_sleep_sm_get_msm_state()
    Returns the current sleep state of MSM.

  bt_qsoc_sleep_sm_get_pending_msm_state()
    Returns the pending sleep state of MSM.

  bt_qsoc_sleep_sm_get_soc_state()
    Returns current sleep state of SoC.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/src/btqsocsleep.c#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $ 
 
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------\
  2008-07-28  ssk  Merging  'Critical Uninitialized Variable' KlocWork error 
                   fixes from the 2007.11 Depot branch.  
                   
  2008-06-23  sa   Clean out unneeded code in bt_qsoc_sleep_enter_msm_asleep_state() 
                   for feature FEATURE_BT_SOC_BITRATE_460800 
  
  2008-03-20  ssk  Clean out unneeded feature FEATURE_BT_QSOC_RST_ON_WAKEUP_TIMEOUT.
 
 * 
 *   #10         28 Feb 2008          SSK
 *   -Optimise Inband SM to enter TX BREAK only after waiting for TX path to go idle.
 *    BT Host will shutdown UART only after both Inband RX/TX paths independently Vote
 *    for Inband Sleep and wait for the Paths to go idle by using FLUSH timers.
 *   -Remove unneeded checks for SM being in the middle of a Inband TX Break from
 *    HCI driver.The TX BRK condition is non-premptible in BT task context.
 *
 *   #9         28 Feb 2008          SSK
 *   Fix High current issue by avoiding unnecessary calls to 
 *   bt_dc_sio_ioctl()/bt_qsoc_sleep_update_sio() when MSM is already ASLEEP.
 *
 *   #8         14 Feb 2008          SSK
 *   -Recover by doing Host BT Radio Off & On and a SOC Power RST when SOC is
 *    unresponsive. This new FEATURE_BT_QSOC_RST_ON_WAKEUP_TIMEOUT is applicable
 *    on targets where SOC can be Power RST.
 *   -Check to prevent race condition where SOC wakes up just after the check for
 *    voting to UART Shutdown passes. Solved by locking out Interrupts temporarily.
 * 
 *   #7         28 Jan 2008          SSK
 *   Fix to prevent Inband SM from exiting & re-entering TX BRK condition 
 *   unnecessarily when MSM is already in Sleep Wait state(fixes race conditions)
 *
 *   #6         10 Jan 2008          SSK
 *   Fix for "SoC fails to come up after it is powered OFF in Inband sleep enabled
 *   builds" and incorporate code review comments from code collab Review #19060  
 * 
 *   #5         29 Nov 2007          SSK
 *   Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
 *
 *   #4         28 Aug 2007          BH
 *   Modified the number of QSoC wake attempts to BTS4020 maximum.
 *
 *   #3         11 May 2007          BH
 *   Added a fix to prevent an ISR from finishing the sio bringup/bring down
 *   procedure.
 *
 *   #2         26 Apr 2007          BH
 *   Added the following:
 *   - Ensuring that sleep can now be reset and initialized multiple times.
 *   - Added bt_qsoc_sleep_wake_soc, so that the client can attempt to 
 *     fully wake the SoC.
 *
 *   #1         06 Apr 2007          BH
 *   Created new file for Bluetooth SoC sleep.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE
===========================================================================*/

/********************* PUBLIC INCLUDE FILES ********************************/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_QSOC
#ifdef FEATURE_BT_QSOC_SLEEP

#include "rex.h"
#include "task.h"

#include "bt.h"
#include "bti.h"
#include "bthci.h"
#include "btqsoc.h"
#include "btsoc.h"
#include "btsocetc.h"
#include "btmsg.h"

#ifdef FEATURE_BT_QSOC_SLEEP_TRACE
#include "timetick.h"
#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
#include "clk.h"
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

/********************* PRIVATE INCLUDE FILES********************************/

#include "btqsocsleep.h"


/*===========================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/

typedef struct 
{
  uint16 bt_qsoc_sleep_wake_conditions; /* HW, TX, RX, etc.. */
  uint16 bt_qsoc_sleep_wake_control;    /* HCI flow control */
} bt_qsoc_sleep_struct_type;

/* Function pointer type for enter/exit states */
typedef struct
{
  void (*func) (void);
} bt_qsoc_sleep_fptr_struct;

#ifdef FEATURE_BT_QSOC_SLEEP_TRACE
typedef enum
{
  BT_QSOC_SLEEP_TRACE_MSM_SLEEP = 0,
  BT_QSOC_SLEEP_TRACE_MSM_WAKE,
  BT_QSOC_SLEEP_TRACE_SOC_SLEEP,
  BT_QSOC_SLEEP_TRACE_SOC_WAKE
} bt_qsoc_sleep_trace_event_type;

typedef struct
{
  bt_qsoc_sleep_trace_event_type traceEvent; /* trace event */
  timetick_type                 ts;         /* time of event */
} bt_qsoc_sleep_trace_type;

typedef struct
{
  /* MSM sleep/wake stats */
  timetick_type   msm_max_wake_time;
  timetick_type   msm_wake_time;
  timetick_type   msm_sleep_time;
  timetick_type   msm_wake_start_time;
  timetick_type   msm_wake_end_time;
  /* SoC sleep/wake stats */
  timetick_type   soc_max_wake_time;
  timetick_type   soc_wake_time;
  timetick_type   soc_sleep_time;
  timetick_type   soc_wake_start_time;
  timetick_type   soc_wake_end_time;
} bt_qsoc_sleep_max_wake_type;

#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */

/*=========================================================================*/
/*                           FUNCTION INTERFACES                           */
/*=========================================================================*/

/* MSM enter state transitions  */
static void bt_qsoc_sleep_enter_msm_asleep_state       ( void );
static void bt_qsoc_sleep_enter_msm_awake_state        ( void );
static void bt_qsoc_sleep_enter_msm_sleep_wait_state   ( void );

/* SoC enter state transitions  */
static void bt_qsoc_sleep_enter_soc_awake_state        ( void );
static void bt_qsoc_sleep_enter_soc_asleep_state       ( void );

/*  MSM exit state transitions  */
static void bt_qsoc_sleep_exit_msm_asleep_state        ( void );
static void bt_qsoc_sleep_exit_msm_awake_state         ( void );
static void bt_qsoc_sleep_exit_msm_sleep_wait_state    ( void );

#ifdef FEATURE_BT_QSOC_SLEEP_TRACE
/* Function for tracking state transitions */
static void bt_qsoc_sleep_trace_sleep_event
(
  bt_qsoc_sleep_trace_event_type event
);
#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */

/*=========================================================================*/
/*                                MACROS                                   */
/*=========================================================================*/

#define BT_MSG_LAYER            BT_MSG_GN /* necessary for btmsg.h */

#define BT_QSOC_SLEEP_TIMEOUT             10        /* 10ms */

#define BT_QSOC_SLEEP_WAKE_ATTEMPTS       16

#ifdef FEATURE_BT_QSOC_SLEEP_TRACE

#define BT_QSOC_SLEEP_TRACE_BUF_SIZE      500

#define BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( event )  \
            bt_qsoc_sleep_trace_sleep_event( event )

#else

#define BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( event )

#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */

/* Sleep/Wake Bitmasks                                      */
/* -------------------------------------------------------- */

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
#define BT_QSOC_SLEEP_TX_FIXED_CONTROLS (BT_QSOC_SLEEP_SIO_TX_BIT)
#else
#define BT_QSOC_SLEEP_FIXED_CONTROLS    (BT_QSOC_SLEEP_HOST_WAKE_BIT \
                                        |  BT_QSOC_SLEEP_SIO_TX_BIT \
                                        |  BT_QSOC_SLEEP_SIO_RX_BIT)
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

#ifdef FEATURE_BT_QSOC_SLEEP_AGGRESSIVE
/* All SIO and HW conditions must be covered */
#define BT_QSOC_SLEEP_MSM_SLEEP         ( (BT_QSOC_SLEEP_HOST_WAKE_BIT)\
                                        | (BT_QSOC_SLEEP_SIO_TX_BIT)\
                                        | (BT_QSOC_SLEEP_SIO_RX_BIT))

#else
/* All SIO, HW and possible future SIO conditions must be covered */
#define BT_QSOC_SLEEP_MSM_SLEEP         ( (BT_QSOC_SLEEP_HOST_WAKE_BIT)\
                                        | (BT_QSOC_SLEEP_SIO_TX_BIT)\
                                        | (BT_QSOC_SLEEP_SIO_RX_BIT)\
                                        | (BT_QSOC_SLEEP_BT_BIT)\
                                        | (BT_QSOC_SLEEP_CMD_BIT)\
                                        | (BT_QSOC_SLEEP_ACL_BIT))


#endif /* FEATURE_BT_QSOC_SLEEP_AGGRESSIVE */

/* Only TX conditions must be covered*/
#define BT_QSOC_SLEEP_SOC_SLEEP         (BT_QSOC_SLEEP_SIO_TX_BIT)


#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
#ifdef FEATURE_BT_QSOC_SLEEP_AGGRESSIVE
/* All SIO and HW conditions must be covered */
#define BT_QSOC_SLEEP_MSM_TX_PATH_SLEEP (BT_QSOC_SLEEP_SIO_TX_BIT)

#else
/* All SIO, HW and possible future SIO conditions must be covered */
#define BT_QSOC_SLEEP_MSM_TX_PATH_SLEEP ( (BT_QSOC_SLEEP_SIO_TX_BIT)\
                                        | (BT_QSOC_SLEEP_BT_BIT)\
                                        | (BT_QSOC_SLEEP_CMD_BIT)\
                                        | (BT_QSOC_SLEEP_ACL_BIT))
#endif /* FEATURE_BT_QSOC_SLEEP_AGGRESSIVE */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */


/* Constants for msm sleep state machine transitions        */
/* -------------------------------------------------------- */
#define BT_QSOC_SLEEP_ENTER_STATE_MSM_ASLEEP        0

#define BT_QSOC_SLEEP_ENTER_STATE_MSM_AWAKE         \
            (BT_QSOC_SLEEP_ENTER_STATE_MSM_ASLEEP + 1)

#define BT_QSOC_SLEEP_ENTER_STATE_MSM_SLEEP_WAIT    \
            (BT_QSOC_SLEEP_ENTER_STATE_MSM_AWAKE + 1)

#define BT_QSOC_SLEEP_EXIT_STATE_MSM_ASLEEP         \
            (BT_QSOC_SLEEP_ENTER_STATE_MSM_SLEEP_WAIT + 1)

#define BT_QSOC_SLEEP_EXIT_STATE_MSM_AWAKE          \
            (BT_QSOC_SLEEP_EXIT_STATE_MSM_ASLEEP + 1)

#define BT_QSOC_SLEEP_EXIT_STATE_MSM_SLEEP_WAIT     \
            (BT_QSOC_SLEEP_EXIT_STATE_MSM_AWAKE + 1)

#define BT_QSOC_SLEEP_MSM_MAX_ENTER_EXIT_ROUTINE    \
            (BT_QSOC_SLEEP_EXIT_STATE_MSM_SLEEP_WAIT + 1)
     
/* Constants for soc sleep state machine transitions        */
/* -------------------------------------------------------- */
#define BT_QSOC_SLEEP_ENTER_STATE_SOC_ASLEEP        0

#define BT_QSOC_SLEEP_ENTER_STATE_SOC_AWAKE         \
            (BT_QSOC_SLEEP_ENTER_STATE_SOC_ASLEEP + 1)

#define BT_QSOC_SLEEP_SOC_MAX_ENTER_EXIT_ROUTINE    \
            (BT_QSOC_SLEEP_ENTER_STATE_SOC_AWAKE + 1)
 
#define BT_QSOC_SLEEP_NONE                          0xFF

/*=========================================================================*/
/*                           DATA DECLARATIONS                             */
/*=========================================================================*/

/* State data */
static bt_qsoc_sleep_struct_type     bt_qsoc_sleep_decision_table;

static bt_qsoc_sleep_msm_state_type  bt_qsoc_sleep_current_msm_sleep_state 
                                        = BT_QSOC_SLEEP_MSM_STATE_UNDEF;

static bt_qsoc_sleep_soc_state_type  bt_qsoc_sleep_current_soc_sleep_state 
                                        = BT_QSOC_SLEEP_SOC_STATE_UNDEF;

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
bt_qsoc_sleep_inband_msm_tx_state_type  bt_qsoc_sleep_inband_tx_state
         = BT_QSOC_ISLEEP_MSM_TX_STATE_UNDEF;
         
bt_qsoc_sleep_inband_msm_rx_state_type  bt_qsoc_sleep_inband_rx_state
         = BT_QSOC_ISLEEP_MSM_RX_STATE_UNDEF;
         
/*
  This variable is used to prevent repetitive shutdown calls to UARTDM.
  It is set to TRUE  when both RX & TX paths VOTE to sleep and UARTDM
  is shutdown. It is set to FALSE when a wakeup RX UNBRK is recvd from SOC.
*/
static boolean          bt_qsoc_sleep_inband_msm_shutdown  = FALSE;

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

/* Bring SIO up/down flags - for shifting time consuming operations */
static boolean          bt_qsoc_sleep_bring_up_sio_flg = FALSE;
static boolean          bt_qsoc_sleep_bring_down_sio_flg = FALSE;

/*
  Since bt_qsoc_sleep_sm_reset can now be called multiple times,
  only define the timers once.
*/
static boolean          bt_qsoc_sleep_timers_defined = FALSE;

/* Sleep Timers */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
rex_timer_type   bt_qsoc_inband_rx_brk_timer;
#else
static rex_timer_type   bt_qsoc_sleep_timer;
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

#ifdef FEATURE_BT_QSOC_SLEEP_TRACE
static bt_qsoc_sleep_trace_type     bt_qsoc_sleep_trace_buf
                                    [BT_QSOC_SLEEP_TRACE_BUF_SIZE];

static uint32                       bt_qsoc_sleep_trace_cntr = 0;

static bt_qsoc_sleep_max_wake_type  bt_qsoc_sleep_max_wake_time;
#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */


  /********************************
  * MSM State transition functions* 
  *********************************/

/* State enter/exit functions */
static const bt_qsoc_sleep_fptr_struct
                bt_qsoc_sleep_msm_sleep_routines
                [BT_QSOC_SLEEP_MSM_MAX_ENTER_EXIT_ROUTINE] =
{ 
  /* ENTER_STATE_MSM_ASLEEP */
  bt_qsoc_sleep_enter_msm_asleep_state,
 
  /* ENTER_STATE_MSM_AWAKE  */
  bt_qsoc_sleep_enter_msm_awake_state,
 
  /* ENTER_STATE_MSM_SLEEP_WAIT */
  bt_qsoc_sleep_enter_msm_sleep_wait_state,
 
  /* EXIT_STATE_MSM_ASLEEP */
  bt_qsoc_sleep_exit_msm_asleep_state,
 
  /* EXIT_STATE_MSM_AWAKE */
  bt_qsoc_sleep_exit_msm_awake_state,
 
  /* EXIT_STATE_MSM_SLEEP_WAIT */
  bt_qsoc_sleep_exit_msm_sleep_wait_state
};

static const uint8 bt_qsoc_sleep_msm_state_enter
                    [BT_QSOC_SLEEP_MSM_STATE_MAX_STATE] = 
{
  /* BT_QSOC_SLEEP_MSM_STATE_UNDEF */
  BT_QSOC_SLEEP_NONE,                  
 
  /* BT_QSOC_SLEEP_MSM_STATE_INIT */
  BT_QSOC_SLEEP_NONE,             
 
  /* BT_QSOC_SLEEP_MSM_STATE_ASLEEP */
  BT_QSOC_SLEEP_ENTER_STATE_MSM_ASLEEP,    
 
  /* BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT */
  BT_QSOC_SLEEP_ENTER_STATE_MSM_SLEEP_WAIT,
 
  /* BT_QSOC_SLEEP_MSM_STATE_AWAKE */
  BT_QSOC_SLEEP_ENTER_STATE_MSM_AWAKE      
};

static const uint8 bt_qsoc_sleep_msm_state_exit
                    [BT_QSOC_SLEEP_MSM_STATE_MAX_STATE] = 
{
  /* BT_QSOC_SLEEP_MSM_STATE_UNDEF */
  BT_QSOC_SLEEP_NONE,

  /* BT_QSOC_SLEEP_MSM_STATE_INIT */
  BT_QSOC_SLEEP_NONE,

  /* BT_QSOC_SLEEP_MSM_STATE_ASLEEP */
  BT_QSOC_SLEEP_EXIT_STATE_MSM_ASLEEP,

  /* BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT */
  BT_QSOC_SLEEP_EXIT_STATE_MSM_SLEEP_WAIT,

  /* BT_QSOC_SLEEP_MSM_STATE_AWAKE */
  BT_QSOC_SLEEP_EXIT_STATE_MSM_AWAKE
};

  /********************************
  * SoC State transition functions* 
  *********************************/

/* State enter/exit functions */
static const bt_qsoc_sleep_fptr_struct 
                bt_qsoc_sleep_soc_sleep_routines
                [BT_QSOC_SLEEP_SOC_MAX_ENTER_EXIT_ROUTINE] = 
{
  /* ENTER_STATE_SOC_ASLEEP */
  bt_qsoc_sleep_enter_soc_asleep_state,

  /* ENTER_STATE_SOC_AWAKE */
  bt_qsoc_sleep_enter_soc_awake_state
};

static const uint8 bt_qsoc_sleep_soc_state_enter
                   [BT_QSOC_SLEEP_SOC_STATE_MAX_STATE] =
{
  /* BT_QSOC_SLEEP_SOC_STATE_UNDEF */
  BT_QSOC_SLEEP_NONE,

  /* BT_QSOC_SLEEP_SOC_STATE_INIT */
  BT_QSOC_SLEEP_NONE,

  /* BT_QSOC_SLEEP_SOC_STATE_ASLEEP */
  BT_QSOC_SLEEP_ENTER_STATE_SOC_ASLEEP,

  /* BT_QSOC_SLEEP_SOC_STATE_AWAKE */
  BT_QSOC_SLEEP_ENTER_STATE_SOC_AWAKE
};

static const uint8 bt_qsoc_sleep_soc_state_exit
             [BT_QSOC_SLEEP_SOC_STATE_MAX_STATE] = 
{
  /* BT_QSOC_SLEEP_SOC_STATE_UNDEFINED */
  BT_QSOC_SLEEP_NONE,
  
   /* BT_QSOC_SLEEP_SOC_STATE_INIT */
  BT_QSOC_SLEEP_NONE,                  
  
   /* BT_QSOC_SLEEP_SOC_STATE_ASLEEP */ 
  BT_QSOC_SLEEP_NONE,
  
  /* BT_QSOC_SLEEP_SOC_STATE_AWAKE */
  BT_QSOC_SLEEP_NONE                                     
};


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_CHANGE_MSM_STATE

  DESCRIPTION    Transitions MSM from one sleep state to another and updates
                 the current sleep state.

  DEPENDENCIES   None.

  PARAMETERS     new_msm_state - the new state to which the MSM should move.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_sm_change_msm_state
(
  bt_qsoc_sleep_msm_state_type new_msm_state
)
{
    bt_qsoc_sleep_msm_state_type current_state =
                    bt_qsoc_sleep_current_msm_sleep_state;

  /*
   ** Ensure a state transition actually occurs.
   */
  if (new_msm_state != bt_qsoc_sleep_current_msm_sleep_state)
  { 
    
    /*
     ** exit the current state
     */
    if ( bt_qsoc_sleep_msm_state_exit[current_state] != BT_QSOC_SLEEP_NONE )
    {
      bt_qsoc_sleep_msm_sleep_routines
          [bt_qsoc_sleep_msm_state_exit[current_state]].func();
    };
    
    /* update current state indication */
    bt_qsoc_sleep_current_msm_sleep_state = new_msm_state;
    
    /*
    ** initiate enter state processing for the new state.
    */
    if ( bt_qsoc_sleep_msm_state_enter[new_msm_state] != BT_QSOC_SLEEP_NONE )
    {
      bt_qsoc_sleep_msm_sleep_routines
          [bt_qsoc_sleep_msm_state_enter[new_msm_state]].func();
    }
  }
  return;
} /* bt_qsoc_sleep_sm_change_msm_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_CHANGE_SOC_STATE

  DESCRIPTION    Transitions SoC from one sleep state to another and updates
                 the current sleep state.

  DEPENDENCIES   None.

  PARAMETERS     new_soc_state - the new SoC sleep state.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_sm_change_soc_state
(
  bt_qsoc_sleep_soc_state_type new_soc_state
)
{
  /*
   ** Ensure a state transition actually occurs.
   */
  if (new_soc_state != bt_qsoc_sleep_current_soc_sleep_state)
  {
      bt_qsoc_sleep_soc_state_type current_state =
                        bt_qsoc_sleep_current_soc_sleep_state;
    /*
     ** exit the current state
     */
    if ( bt_qsoc_sleep_soc_state_exit[current_state] != BT_QSOC_SLEEP_NONE )
    {
      bt_qsoc_sleep_soc_sleep_routines
          [bt_qsoc_sleep_msm_state_exit[current_state]].func();
    };
    
    /* update current state indication */
    bt_qsoc_sleep_current_soc_sleep_state = new_soc_state;
    
    /*
    ** initiate enter state processing for the new state.
    */
    if ( bt_qsoc_sleep_soc_state_enter[new_soc_state] != BT_QSOC_SLEEP_NONE )
    {
      bt_qsoc_sleep_soc_sleep_routines
          [bt_qsoc_sleep_soc_state_enter[new_soc_state]].func();
    }
  }

  return;
} /* bt_qsoc_sleep_sm_change_soc_state */


#ifdef FEATURE_BT_QSOC_SLEEP_TRACE
/*==========================================================================

  FUNCTION       BT_QSOC_TRACE_SLEEP_EVENT

  DESCRIPTION    Tracks sleep and wake states.

  DEPENDENCIES   None.

  PARAMETERS     event - The sleep event to be tracked.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_trace_sleep_event
(
  bt_qsoc_sleep_trace_event_type event
)
{
  /* Calculate current time */
  timetick_type time = timetick_get();
  
  if (bt_qsoc_sleep_trace_cntr >= BT_QSOC_SLEEP_TRACE_BUF_SIZE)
  {
    /* Circular buffer - start over */
    bt_qsoc_sleep_trace_cntr = 0;
  } 

  /* Track state transitions */
  bt_qsoc_sleep_trace_buf[bt_qsoc_sleep_trace_cntr].traceEvent = event;
  bt_qsoc_sleep_trace_buf[bt_qsoc_sleep_trace_cntr].ts = time;
  bt_qsoc_sleep_trace_cntr++; 


  /* Track MSM/SoC wake/sleep stats */
  switch (event)
  {
    case BT_QSOC_SLEEP_TRACE_MSM_SLEEP:
    {
      bt_qsoc_sleep_max_wake_time.msm_wake_end_time = time;

      if (bt_qsoc_sleep_max_wake_time.msm_wake_start_time != 0)
      {
        /* Track MSM wake time */
        bt_qsoc_sleep_max_wake_time.msm_wake_time += 
            (time - bt_qsoc_sleep_max_wake_time.msm_wake_start_time);
        
        
        /* Is this the longest the MSM stayed awake? */
        if ((time - bt_qsoc_sleep_max_wake_time.msm_wake_start_time) >
            bt_qsoc_sleep_max_wake_time.msm_max_wake_time)
        {
            bt_qsoc_sleep_max_wake_time.msm_max_wake_time = 
                (time - bt_qsoc_sleep_max_wake_time.msm_wake_start_time);
        }
      }
      break;
    }
      
    
    case BT_QSOC_SLEEP_TRACE_MSM_WAKE:
    {
      bt_qsoc_sleep_max_wake_time.msm_wake_start_time = time;
      
      if (bt_qsoc_sleep_max_wake_time.msm_wake_end_time != 0)
      {
        /* Track MSM sleep time */
        bt_qsoc_sleep_max_wake_time.msm_sleep_time += 
            (time - bt_qsoc_sleep_max_wake_time.msm_wake_end_time);
      }
      break;
    }
    
    case BT_QSOC_SLEEP_TRACE_SOC_SLEEP:
    {
      bt_qsoc_sleep_max_wake_time.soc_wake_end_time = time;
      
      if (bt_qsoc_sleep_max_wake_time.soc_wake_start_time != 0)
      {
        /* Track SoC wake time */
        bt_qsoc_sleep_max_wake_time.soc_wake_time += 
            (time - bt_qsoc_sleep_max_wake_time.soc_wake_start_time);
        
        
        /* Is this the longest the SoC has been kept awake? */
        if ((time - bt_qsoc_sleep_max_wake_time.soc_wake_start_time) >
            bt_qsoc_sleep_max_wake_time.soc_max_wake_time)
        {
          bt_qsoc_sleep_max_wake_time.soc_max_wake_time = 
              (time - bt_qsoc_sleep_max_wake_time.soc_wake_start_time);
        }
      }
      break;
    }
    
    case BT_QSOC_SLEEP_TRACE_SOC_WAKE:
    {
      bt_qsoc_sleep_max_wake_time.soc_wake_start_time = time;
      
      if (bt_qsoc_sleep_max_wake_time.soc_wake_end_time != 0)
      {
        /* Track SoC sleep time */
        bt_qsoc_sleep_max_wake_time.soc_sleep_time += 
            (time - bt_qsoc_sleep_max_wake_time.soc_wake_end_time);
      }
      break;
    }
      
    default:
      break;
  }
} /* bt_qsoc_sleep_trace_sleep_event */
#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_CALCULATE_MSM_STATE

  DESCRIPTION    Calculates the current MSM sleep state based off the
                 decision table.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   new_msm_state - the sleep state the MSM should be in.

  SIDE EFFECTS   None.

==========================================================================*/
static bt_qsoc_sleep_msm_state_type bt_qsoc_sleep_sm_calculate_msm_state  
(
  void
)
{
  bt_qsoc_sleep_msm_state_type new_msm_state;
  
  if (bt_qsoc_sleep_current_msm_sleep_state == BT_QSOC_SLEEP_MSM_STATE_UNDEF)
  {
    /* Only calculate the state after we have initialized */
    return BT_QSOC_SLEEP_MSM_STATE_UNDEF;
  }
  
  /*  Understanding when the MSM can sleep.
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
  
      MSM state is based off the bt_qsoc_sleep_decision_table. Each event
      marks or clears a bit in the table. If the MSM specific bits are 
      cleared when both variables are AND'ed, then the MSM can sleep.
      If any of the bits are not cleared, the MSM must wake or stay awake.
  */
  
  /* Set Constant Wake Control bits */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control |=
                                        BT_QSOC_SLEEP_TX_FIXED_CONTROLS;
#else /* only FEATURE_BT_QSOC_SLEEP is defined */
  bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control |=
                                        BT_QSOC_SLEEP_FIXED_CONTROLS;
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
  
  
  /* Set BT Queue control bit - Bitwise OR of both HCI flow controls */
  if ((bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control 
                & (BT_QSOC_SLEEP_CMD_BIT | BT_QSOC_SLEEP_ACL_BIT)) != 0)
  {
    /* Set BT control bit */
    bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control |= 
                                        BT_QSOC_SLEEP_BT_BIT;
  } 
  else
  {
    /* Clear BT control bit */
    bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control &=
                                        (~BT_QSOC_SLEEP_BT_BIT);
  }
  
  /* Calculate new state based on decision table */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  if (((bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_conditions &
        bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control) &
        BT_QSOC_SLEEP_MSM_TX_PATH_SLEEP) == 0 )
  {
    /*
      All sleep conditions for TX PATH  are met - MSM can vote for  TX sleep
      after enough time has past for the TX path to be empty.  This delay also
      ensures the MSM doesn't attempt to do sleep too often during a lot of activity.
    */
    new_msm_state = BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT;

  }
#else /* only FEATURE_BT_QSOC_SLEEP is defined */
  if (((bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_conditions &
        bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control) &
        BT_QSOC_SLEEP_MSM_SLEEP) == 0 )
  {
    /* 
      All sleep conditions are met - MSM can try to 
      sleep after enough time has past for the Rx
      path to be empty.  This delay also ensures the
      MSM doesn't attempt to sleep too often during
      a lot of activity.
    */
    new_msm_state = BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT;
  } 
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
  else
  {
    /* No Sleep conditions are met */
    new_msm_state = BT_QSOC_SLEEP_MSM_STATE_AWAKE;
  } 
  
  return new_msm_state;
} /* bt_qsoc_sleep_sm_calculate_msm_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_CALCULATE_SOC_STATE

  DESCRIPTION    Calculates the current SoC sleep state based off the 
                 decision table.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   new_soc_state - the sleep state the SoC should be in.

  SIDE EFFECTS   None.

==========================================================================*/
static bt_qsoc_sleep_soc_state_type bt_qsoc_sleep_sm_calculate_soc_state  
(
  void
)
{
  bt_qsoc_sleep_soc_state_type new_soc_state;
  
  if (bt_qsoc_sleep_current_soc_sleep_state == BT_QSOC_SLEEP_SOC_STATE_UNDEF)
  {
    /* Only calculate the state after we have initialized */
    return BT_QSOC_SLEEP_SOC_STATE_UNDEF;
  }
  
  /*  
      Understanding when the SoC can sleep.
    LSB                                         MSB
   _________________________________________________
   |  0 |  1 |  2 |    3    |  4  |  5  |6|7|...|15|
   |____|____|____|_________|_____|_____|_|_|___|__|
   | HW | TX | RX |   BT    | CMD | ACL |          |
   |    |    |    |   Q     |  Q  |  Q  | RESERVED |
   ------------------------------------------------
           ^ bt_qsoc_sleep_wake_conditions ^
  
      SoC state is based off the bt_qsoc_sleep_decision_table. Each event
      marks or clears a bit in the table. If any of the SoC specific bits are 
      not set, then the SoC can sleep. If any of these bits are not cleared,
      the SoC must wake or stay awake.
  */
  
  
  /* Calculate new state based on decision table */
  if ((bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_conditions &
                                    BT_QSOC_SLEEP_SOC_SLEEP) == 0 )
  {
    /* All sleep conditions are met - SoC can sleep */
    new_soc_state = BT_QSOC_SLEEP_SOC_STATE_ASLEEP;
  } 
  else
  {
    /* No Sleep conditions are met */
    new_soc_state = BT_QSOC_SLEEP_SOC_STATE_AWAKE;
  } 
  
  return new_soc_state;
} /* bt_qsoc_sleep_sm_calculate_soc_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ENTER_MSM_ASLEEP_STATE

  DESCRIPTION    Triggered as MSM enters the BT_QSOC_SLEEP_STATE_MSM_ASLEEP 
                 state. Sets the bt_qsoc_sleep_bring_down_sio_flg flag so 
                 that the SIO driver is flowed-off and disabled.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_enter_msm_asleep_state 
(
  void
)
{
    bt_qsoc_sleep_bring_down_sio_flg = TRUE;
} /* bt_qsoc_sleep_enter_msm_asleep_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_EXIT_MSM_ASLEEP_STATE

  DESCRIPTION    Triggered as MSM exits the BT_QSOC_SLEEP_MSM_STATE_ASLEEP 
                 state. 

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_exit_msm_asleep_state 
(
  void
)
{
  BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( BT_QSOC_SLEEP_TRACE_MSM_WAKE );

#ifdef FEATURE_BT_QSOC_SLEEP_DEBUG
  BT_MSG_DEBUG( "BT QSOC Sleep: New MSM Sleep State: MSM Wake", 0, 0, 0);
#endif /* FEATURE_BT_QSOC_SLEEP_DEBUG */

} /* bt_qsoc_sleep_exit_msm_asleep_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ENTER_MSM_SLEEP_WAIT_STATE

  DESCRIPTION    Triggered as MSM enters the BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT
                 state. Since there may not be a means to poll the SIO 
                 driver for RX activity, the MSM must wait to ensure the 
                 RX path is empty.  To do this a timer is started, after 
                 which the MSM may sleep.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_enter_msm_sleep_wait_state
(
  void
)
{

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  if( BT_QSOC_IS_READY() == FALSE )
  {
    BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: Soc Not ready st1:%d st2:%d st3:$d",
                          bt_qsoc_sleep_inband_rx_state,
                          bt_qsoc_sleep_inband_tx_state,
                          bt_qsoc_sleep_inband_msm_shutdown );
    return;
  }
  else
  {
    /* The TX FLUSH timers should not be running and they are not necessary anymore */
      
    /* Entering TX BRK as TX path is idle */
    bt_qsoc_sleep_inband_enter_tx_brk();
  }
#else
  /* Start sleep timer to confirm that RX path is idle */
  rex_set_timer( &bt_qsoc_sleep_timer, BT_QSOC_SLEEP_TIMEOUT );
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

} /* bt_qsoc_sleep_enter_msm_sleep_wait_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_EXIT_MSM_SLEEP_WAIT_STATE

  DESCRIPTION    Triggered as MSM exits the BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT
                 state.  Clears the sleep timer. 

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_exit_msm_sleep_wait_state
(
  void
)
{

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  /*
    No Need to clear any Sleep Wait Or RX Flush timers here as Inband SM takes
    care of this when INBAND RX\TX SLEEP state transitions. We are either
    transitioning to SLEEP state or to AWAKE state.
   */
  BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: Exit Sleep wait st1:%d st2:%d st3:%d",
                        bt_qsoc_sleep_inband_rx_state,
                        bt_qsoc_sleep_inband_tx_state,
                        bt_qsoc_sleep_inband_msm_shutdown );
#else
  /* Clear the sleep timer, and ensure the signal is cleared */
  (void) rex_clr_timer( &bt_qsoc_sleep_timer );
  (void) rex_clr_sigs( &bt_tcb, BT_QSOC_SLEEP_T_SIG );
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

} /* bt_qsoc_sleep_exit_msm_sleep_wait_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ENTER_MSM_AWAKE_STATE

  DESCRIPTION    Triggered as MSM enters the BT_QSOC_SLEEP_MSM_STATE_AWAKE 
                 state.  Sets bt_qsoc_sleep_bring_up_sio_flg so that the SIO
                 is enabled and flowed-on.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_enter_msm_awake_state
(
  void
)
{
  bt_qsoc_sleep_bring_up_sio_flg = TRUE;
} /* bt_qsoc_sleep_enter_msm_awake_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_EXIT_MSM_AWAKE_STATE

  DESCRIPTION    Triggered as MSM exits the BT_QSOC_SLEEP_MSM_STATE_AWAKE 
                 state.  Disables the SIO driver's inbound flow. 

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_exit_msm_awake_state 
(
  void
)
{
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  /* No need to disable flow as only the Inband SM's TX PATH is voting to sleep
     We still potentially can recv data at this stage although TX path is idle */
  BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: Exit Awake state st1:%d st2:%d st3:%d",
                        bt_qsoc_sleep_inband_rx_state,
                        bt_qsoc_sleep_inband_tx_state,
                        bt_qsoc_sleep_inband_msm_shutdown );
#else
  bt_soc_disable_flow();
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
} /* bt_qsoc_sleep_exit_msm_awake_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ENTER_SOC_AWAKE_STATE

  DESCRIPTION    Triggered as SoC enters the BT_QSOC_SLEEP_SOC_STATE_AWAKE 
                 state.  Sends a wakeup message to the SoC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_enter_soc_awake_state 
(
  void
)
{
  /* Send wakeup message to SoC */
  if (bt_soc_ifc.bt_soc_wakeup_func_ptr != NULL)
  {
    bt_soc_ifc.bt_soc_wakeup_func_ptr();
  }
  else
  {
    BT_ERR( "BT QSOC Sleep: bt_qsoc_sleep_enter_soc_awake_state - SoC wakeup function not set", 0, 0, 0 );
  }
  
  BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( BT_QSOC_SLEEP_TRACE_SOC_WAKE );

#ifdef FEATURE_BT_QSOC_SLEEP_DEBUG
  BT_MSG_DEBUG( "BT QSOC Sleep: New SoC Sleep State: SOC Wake", 0, 0, 0);
#endif /* FEATURE_BT_QSOC_SLEEP_DEBUG */

} /* bt_qsoc_sleep_enter_soc_awake_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ENTER_SOC_ASLEEP_STATE

  DESCRIPTION    Triggered as SoC enters the BT_QSOC_SLEEP_SOC_STATE_ASLEEP 
                 state.  Sends a sleep message to the SoC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_enter_soc_asleep_state 
(
  void
)
{
  /* Send sleep message to SoC */
  if (bt_soc_ifc.bt_soc_sleep_func_ptr != NULL)
  {
    bt_soc_ifc.bt_soc_sleep_func_ptr();
  }
  else
  {
    BT_ERR( "BT QSOC Sleep: bt_qsoc_sleep_enter_soc_asleep_state - SoC sleep function not set", 0, 0, 0 );
  }
  
  BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( BT_QSOC_SLEEP_TRACE_SOC_SLEEP );

#ifdef FEATURE_BT_QSOC_SLEEP_DEBUG
  BT_MSG_DEBUG( "BT QSOC Sleep: New SoC Sleep State: SOC Sleep", 0, 0, 0);
#endif /* FEATURE_BT_QSOC_SLEEP_DEBUG */

} /* bt_qsoc_sleep_enter_soc_asleep_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_UPDATE_SIO

  DESCRIPTION    If bt_qsoc_sleep_bring_up_sio_flg is set the SIO driver
                 will be enabled and the inbound flow enabled. If 
                 bt_qsoc_sleep_bring_down_sio_flg is set, the inbound
                 flow will be disabled and the SIO will be disabled.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_update_sio 
(
  void
)
{
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  sio_ioctl_param_type  ioctl_param;
  uint32 isave;  
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */   
    
  /* Enable or disable the SIO driver */
  if (bt_qsoc_sleep_bring_up_sio_flg == TRUE)
  {
    bt_qsoc_sleep_bring_up_sio_flg = FALSE;

    BT_MSG_INBAND_DEBUG( "BT QSOC SLEEP: Bringup Sio st1:%d st2:%d sio_st:%d",
                          bt_qsoc_sleep_inband_rx_state,
                          bt_qsoc_sleep_inband_tx_state,
                          bt_dc.sio_state );

    /* Enable the SIO driver */
    if (bt_dc.sio_state == BT_SIO_SLEEPING)
    {
      bt_dc_update_sio_state( BT_SIO_AWAKE, TRUE );
    }

    bt_soc_enable_flow();
  }
  else if (bt_qsoc_sleep_bring_down_sio_flg == TRUE)
  {
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP     
    boolean   rx_line_high = TRUE;

    /* 
      If soc_to_msm WM is not empty, then UART can't be shutdown. In any case
      bt_qsoc_sleep_bring_down_sio_flg flag is cleared as the MSM sleep state
      processing functions set it again before calling update sio.
     */     
    bt_qsoc_sleep_bring_down_sio_flg = FALSE;

    if (bt_qsoc_sleep_inband_msm_shutdown == FALSE )
    {
      /* Make ioctl call to check RX Level only if other Sleep coditions are met*/
      ioctl_param.rx_line_state = &rx_line_high;
      bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                       SIO_IOCTL_CHECK_RX_STATE,
                       &ioctl_param );
    }

    /* 
      Prevent race condition where SOC wakes up just after this IF condition
      passes. So we protect the RX INBAND State from changing due to ISRs.
    */
    INTLOCK_SAV( isave ); 
    if ( (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED) &&
         (bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP) &&      
         (rx_line_high                  == FALSE                            ) &&
         (bt_soc_to_msm_sio_wm.current_cnt == 0                             ) &&
         (bt_qsoc_sleep_inband_msm_shutdown == FALSE                       )  )
    {
      /* We are ready to go for MSM sleep and shutdown the SIO */
      bt_qsoc_sleep_inband_msm_shutdown = TRUE;    
      
      BT_MSG_INBAND_DEBUG( "BT QSOC SLEEP: INBAND SLEEP VOT SUCCESSFUL st1:%d st2:%d st3:%d",
                            bt_qsoc_sleep_inband_rx_state,
                            bt_qsoc_sleep_inband_tx_state,
                            bt_qsoc_sleep_inband_msm_shutdown );     

      /* Disable inbound flow */
      bt_soc_disable_flow();
      
      /* Register RX high callback function that wakes up inband sleep SM */
      ioctl_param.rx_high_event_func_ptr = bt_qsoc_inband_rx_unbreak;
      bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                       SIO_IOCTL_RX_HIGH_EVENT,
                       &ioctl_param );
      
      /* Disable the SIO driver */
      bt_dc_update_sio_state( BT_SIO_SLEEPING, TRUE );
      INTFREE_SAV( isave );      
      BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( BT_QSOC_SLEEP_TRACE_MSM_SLEEP );
    }
    else
    {
      if (bt_soc_to_msm_sio_wm.current_cnt != 0 )
      {
        /* Restart the flush timer */
        (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
        (void) rex_clr_sigs( &bt_tcb, BT_QSOC_INBAND_RX_FLUSH_SIG);
        (void) rex_set_timer( &bt_qsoc_inband_rx_flush_timer, BT_QSOC_INBAND_RX_FLUSH_TIMEOUT );         
      }
       INTFREE_SAV( isave );
      
      /* Inband Sleep SM is not  ready for UART shutdown. */
      BT_MSG_INBAND_HIGH( "INBAND SLEEP VOT UNSUCCESSFUL RXSt:%d WMCnt:%d uart_shut:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_soc_to_msm_sio_wm.current_cnt,
                           bt_qsoc_sleep_inband_msm_shutdown );
  }
#else
      bt_qsoc_sleep_bring_down_sio_flg = FALSE;
      /* We are ready to go for MSM sleep and shutdown the SIO */
    /* Disable inbound flow */
    bt_soc_disable_flow();

    /* Disable the SIO driver */
    bt_dc_update_sio_state( BT_SIO_SLEEPING, TRUE );

    BT_QSOC_SLEEP_TRACE_SLEEP_EVENT( BT_QSOC_SLEEP_TRACE_MSM_SLEEP );
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

#ifdef FEATURE_BT_QSOC_SLEEP_DEBUG
    BT_MSG_DEBUG( "BT QSOC Sleep: New MSM Sleep State: MSM Sleep", 0, 0, 0); 
#endif /* FEATURE_BT_QSOC_SLEEP_DEBUG */

  }
}


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_UPDATE_DECISION_TABLE

  DESCRIPTION    This function will set or clear the specific bits in the
                 decision table.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_update_decision_table 
(
  uint16 event
)
{
  /* Update Sleep decision table - condition or control */
  if ((event & BT_QSOC_SLEEP_CONTROL_BIT) == BT_QSOC_SLEEP_CONTROL_BIT)
  {
    if ((event & BT_QSOC_SLEEP_ENABLE_BIT) == BT_QSOC_SLEEP_ENABLE_BIT)
    {
      /* Set the bit */
      bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control |=
                                        (event & BT_QSOC_SLEEP_BITS);
    }
    else
    {
      /* clear the bit */
      bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control &=
                                        (~(event & BT_QSOC_SLEEP_BITS));
    }
  }
  else /* Affects conditions variable */
  {
    if ((event & BT_QSOC_SLEEP_ENABLE_BIT) == BT_QSOC_SLEEP_ENABLE_BIT)
    {
      /* Set the bit */
      bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_conditions |= 
                                        (event & BT_QSOC_SLEEP_BITS);
    }
    else
    {
      /* clear the bit */
      bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_conditions &=
                                        (~(event & BT_QSOC_SLEEP_BITS));
    }
  }
}


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_RESET

  DESCRIPTION    Sets all sleep variables to reset values.  Ensures the 
                 sleep state machines are in uninitialized states.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_sm_reset
(
  void
)
{
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP  
  sio_ioctl_param_type  ioctl_param;  
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */    
  /* reset the current MSM & SoC states */
  bt_qsoc_sleep_current_msm_sleep_state = BT_QSOC_SLEEP_MSM_STATE_UNDEF;
  bt_qsoc_sleep_current_soc_sleep_state = BT_QSOC_SLEEP_SOC_STATE_UNDEF;
  
  /* clear all tracked events */
  memset( &bt_qsoc_sleep_decision_table,
          0,
          sizeof(bt_qsoc_sleep_struct_type) );
  
  /* clear SIO bring up/down flags */
  bt_qsoc_sleep_bring_up_sio_flg = FALSE;  
  bt_qsoc_sleep_bring_down_sio_flg = FALSE;

  if (bt_qsoc_sleep_timers_defined == FALSE)
  {
    bt_qsoc_sleep_timers_defined = TRUE;
    /* Initialize timers */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
    rex_def_timer( &bt_qsoc_inband_rx_brk_timer, &bt_tcb, BT_QSOC_INBAND_BREAK_RX_SIG );
    rex_def_timer( &bt_qsoc_inband_rx_flush_timer, &bt_tcb, BT_QSOC_INBAND_RX_FLUSH_SIG);
    /* 
       Reuse the QSOC Sleep SIG for a TX path IDLE timer, This SIG is used by
       Out of band sleep and when TX FLush completes i.e bt_qsoc_sleep_tx_flush_cb()
     */
    rex_def_timer( &bt_qsoc_inband_tx_flush_timer, &bt_tcb, BT_QSOC_SLEEP_SIG);
#else
    rex_def_timer( &bt_qsoc_sleep_timer, &bt_tcb, BT_QSOC_SLEEP_T_SIG );
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
  }

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  /* Clear sleep related timers/signals */
  (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
  (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
  (void) rex_clr_timer( &bt_qsoc_inband_tx_flush_timer );
  (void) rex_clr_sigs(  &bt_tcb, (BT_QSOC_INBAND_BREAK_RX_SIG    | 
                                  BT_QSOC_INBAND_TX_ACK_REQD_SIG |
                                  BT_QSOC_SLEEP_SIG) );
#else
  /* Clear sleep related timers/signals */
  (void) rex_clr_timer( &bt_qsoc_sleep_timer );
  (void) rex_clr_sigs( &bt_tcb, (BT_QSOC_SLEEP_SIG | BT_QSOC_SLEEP_T_SIG) );
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */  

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  bt_qsoc_sleep_inband_msm_shutdown  = FALSE; 

  /* Faciliate non-inband sleep activity when SOC Driver is not yet fully ready */
  bt_qsoc_sleep_inband_tx_state      = BT_QSOC_ISLEEP_MSM_TX_ACTIVE_TRANSMIT;
  bt_qsoc_sleep_inband_rx_state      = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;

  bt_qsoc_sleep_bring_up_sio_flg     = FALSE;
  bt_qsoc_sleep_bring_down_sio_flg   = FALSE;

  BT_MSG_DEBUG( "BT QSOC Sleep Inband: Disable RX HIGH ISR s1:%d st2:%d st3:%d",
                 bt_qsoc_sleep_inband_rx_state,
                 bt_qsoc_sleep_inband_tx_state,
                 bt_qsoc_sleep_current_msm_sleep_state );

  /* Try to disable the TRAMP HIGH RX WAKEUP ISR */
  ioctl_param.rx_high_event_func_ptr = NULL;
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_RX_HIGH_EVENT,
                   &ioctl_param ); 
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
  
#ifdef FEATURE_BT_QSOC_SLEEP_TRACE
  memset( bt_qsoc_sleep_trace_buf,
          0,
          sizeof(bt_qsoc_sleep_trace_buf) );

  bt_qsoc_sleep_trace_cntr = 0;                          
                                                     
  memset( &bt_qsoc_sleep_max_wake_time,
          0,
          sizeof(bt_qsoc_sleep_max_wake_type) );
#endif /* FEATURE_BT_QSOC_SLEEP_TRACE */ 
} /* bt_qsoc_sleep_sm_reset */                    


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_INIT

  DESCRIPTION    Initializes both the MSM and SoC Sleep state machines.  
                 Before initialization, events are tracked but no actions 
                 are taken.

  DEPENDENCIES   bt_qsoc_sleep_sm_reset *should* be called first.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_sm_init
(
  void
)
{
  uint32 isave;
  bt_qsoc_sleep_msm_state_type msm_state;
  bt_qsoc_sleep_soc_state_type soc_state;
  
  
  INTLOCK_SAV( isave );
  bt_qsoc_sleep_current_msm_sleep_state = BT_QSOC_SLEEP_MSM_STATE_INIT;
  bt_qsoc_sleep_current_soc_sleep_state = BT_QSOC_SLEEP_SOC_STATE_INIT;
  
  msm_state = bt_qsoc_sleep_sm_calculate_msm_state();
  soc_state = bt_qsoc_sleep_sm_calculate_soc_state();
  
  bt_qsoc_sleep_sm_change_soc_state( soc_state );
  bt_qsoc_sleep_sm_change_msm_state( msm_state );
  INTFREE_SAV( isave );
  
  /* Enable or disable the SIO driver */
  bt_qsoc_sleep_update_sio();

  /* 
     Issue flush just in case there are any TX's pending.
     This will ensure that the MSM and SoC don't stay awake until
     the next transmit.
  */
  bt_qsoc_sleep_issue_tx_flush( bt_dc.sio_stream_id_ext_soc );

} /* bt_qsoc_sleep_sm_init */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_EVAL_EVENT

  DESCRIPTION    Tracks and evaluates sleep related events for both the MSM
                 and SoC state machines.  Only if the update_msm_state 
                 parameter is TRUE, will the MSM transition to the new state.
                 However, regardless of the update_msm_state parameter, the 
                 SoC will transition to the new state.

                 If this function is called with the update_msm_state set to 
                 FALSE (typically ISR context), 
                 the bt_qsoc_sleep_sm_update_msm_state function should 
                 be called later to actually move the MSM to the new state.

  DEPENDENCIES   None.

  PARAMETERS     event            - New sleep related event.
                 update_msm_state - Whether MSM should transition to the 
                                    next sleep state.

  RETURN VALUE   None.

  SIDE EFFECTS   Interrupts are locked while deciding and changing state.

==========================================================================*/
void bt_qsoc_sleep_sm_eval_event
(
  uint16 event,
  boolean update_msm_state
)
{
  uint32 isave;
  bt_qsoc_sleep_msm_state_type new_msm_state;
  bt_qsoc_sleep_soc_state_type new_soc_state;
  
#ifdef FEATURE_BT_QSOC_SLEEP_DEBUG
      BT_MSG_DEBUG( "BT QSOC Sleep: Eval Event e %x, cond %x, ctrl %x",
                     event,
                     bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_conditions,
                     bt_qsoc_sleep_decision_table.bt_qsoc_sleep_wake_control);
#endif /* FEATURE_BT_QSOC_SLEEP_DEBUG */
  
  INTLOCK_SAV( isave );

  /* Update Sleep decision table */
  bt_qsoc_sleep_update_decision_table( event );

  /* Calculate and move to new SoC state */
  new_soc_state = bt_qsoc_sleep_sm_calculate_soc_state();

  bt_qsoc_sleep_sm_change_soc_state( new_soc_state );
  
  /* Calculate new MSM state if necessary. */
  if (update_msm_state == TRUE)
  {
    new_msm_state = bt_qsoc_sleep_current_msm_sleep_state;

    switch (bt_qsoc_sleep_current_msm_sleep_state)
    {
      case BT_QSOC_SLEEP_MSM_STATE_AWAKE:
      {
        /* Does this event clear a bit and allow the MSM to sleep? */
        if (BT_QSOC_SLEEP_ENABLE_BIT != (event & BT_QSOC_SLEEP_ENABLE_BIT))
        {
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
          /* 
            Any of the following events might allow the MSM to Indicate the
             Tx path is idle and go to INBAND SLEEP WAIT sleep wait state. Note
            that TX BRK and RX BRK is still needed before MSM can go to sleep 
           */
          if ((event &  (BT_QSOC_SLEEP_EVENT_SIO_TX_IDLE
                        | BT_QSOC_SLEEP_EVENT_BT_Q_EMPTY
                        | BT_QSOC_SLEEP_EVENT_CMD_Q_EMPTY
                        | BT_QSOC_SLEEP_EVENT_ACL_Q_EMPTY)) != 0)
#else
          /* Any of the following events might allow the MSM to sleep */
          if ((event & (BT_QSOC_SLEEP_EVENT_HOST_WAKE_DISABLED
                        | BT_QSOC_SLEEP_EVENT_SIO_RX_IDLE
                        | BT_QSOC_SLEEP_EVENT_SIO_TX_IDLE
                        | BT_QSOC_SLEEP_EVENT_BT_Q_EMPTY
                        | BT_QSOC_SLEEP_EVENT_CMD_Q_EMPTY
                        | BT_QSOC_SLEEP_EVENT_ACL_Q_EMPTY)) != 0)
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
          {
            /* Calculate the state to see if the MSM can really sleep */
            new_msm_state = bt_qsoc_sleep_sm_calculate_msm_state();
          }
          else if ((event & (BT_QSOC_SLEEP_EVENT_HCI_CMD_FLOWED_OFF |
                             BT_QSOC_SLEEP_EVENT_HCI_ACL_FLOWED_OFF)) != 0)
          {
              /* Calculate the state to see if the MSM can really sleep */
              new_msm_state = bt_qsoc_sleep_sm_calculate_msm_state();
          }
        }
        break;
      }
      
      case BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT:
      {
        switch (event)
        {
          case BT_QSOC_SLEEP_EVENT_HOST_WAKE_ENABLED:  
          case BT_QSOC_SLEEP_EVENT_SIO_RX_BUSY:
          case BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY:
            new_msm_state = BT_QSOC_SLEEP_MSM_STATE_AWAKE;
            break;
          
          case BT_QSOC_SLEEP_EVENT_SLEEP_TIMER:
            /* RX path should now be clear */
            new_msm_state = BT_QSOC_SLEEP_MSM_STATE_ASLEEP;
            break;
          
          default:
            /* No State Change */
            break;
          
        }/* End switch (event) */
        break;
      }
      
      case BT_QSOC_SLEEP_MSM_STATE_ASLEEP:
      {
        switch (event)
        {
          case BT_QSOC_SLEEP_EVENT_HOST_WAKE_ENABLED:
          case BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY:
          {
            new_msm_state = BT_QSOC_SLEEP_MSM_STATE_AWAKE;
            break;
          }
          
          default: 
            /* No State Change */
            break;
          
        } /* End switch (event) */
        break;
      }
      
      default:
        /* 
        case BT_QSOC_SLEEP_MSM_STATE_UNDEFINED: 
        case BT_QSOC_SLEEP_MSM_STATE_INIT:
         - No state changes allowed -
        */
        break;

    } /* End switch (bt_qsoc_sleep_current_msm_sleep_state) */
    
    BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: newstate: %d curstate2: %d evt %d",
                          new_msm_state,
                          bt_qsoc_sleep_current_msm_sleep_state,
                          event );
    
    bt_qsoc_sleep_sm_change_msm_state( new_msm_state );
  }
  
  INTFREE_SAV( isave );
  
  /*  Bringing the SIO up/down takes too much time, so it is 
      done outside of the INTLOCK.  Addtionally, since this
      function can be called from an ISR context, ensure that
      we only update SIO if the update_msm_state is TRUE.
  */
  if (update_msm_state == TRUE)
  {
    bt_qsoc_sleep_update_sio(); 
  }


} /* bt_qsoc_sleep_sm_eval_event */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_UPDATE_MSM_STATE

  DESCRIPTION    Updates the MSM state to reflect changes in the decision 
                 table.  Since the MSM state may not get updated in an 
                 interrupt context, this function is called in the task 
                 context to ensure the MSM is moved to the correct state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   Interrupts are locked while moving to new state.

==========================================================================*/
void bt_qsoc_sleep_sm_update_msm_state
(
  void
)
{
  uint32 isave;
  bt_qsoc_sleep_msm_state_type new_state;
  
  INTLOCK_SAV( isave );
  new_state = bt_qsoc_sleep_sm_calculate_msm_state();
  bt_qsoc_sleep_sm_change_msm_state( new_state );
  INTFREE_SAV( isave );
  
  /*  Bringing up/down the SIO takes too much time, 
      so it is done outside of the INTLOCK.
  */
  bt_qsoc_sleep_update_sio(); 

} /* bt_qsoc_sleep_sm_update_msm_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_GET_MSM_STATE

  DESCRIPTION    Returns the current MSM sleep state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current MSM sleep state.

  SIDE EFFECTS   None.

==========================================================================*/
bt_qsoc_sleep_msm_state_type bt_qsoc_sleep_sm_get_msm_state
(
  void
)
{
  return bt_qsoc_sleep_current_msm_sleep_state;
} /* bt_qsoc_sleep_sm_get_msm_state */


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
bt_qsoc_sleep_msm_state_type bt_qsoc_sleep_sm_get_pending_msm_state
(
  void
)
{
  return bt_qsoc_sleep_sm_calculate_msm_state();
} /* bt_qsoc_sleep_sm_get_pending_msm_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_SM_GET_SOC_STATE

  DESCRIPTION    Returns the current SoC state.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   The current SoC sleep state.

  SIDE EFFECTS   None.

==========================================================================*/
bt_qsoc_sleep_soc_state_type bt_qsoc_sleep_sm_get_soc_state
(
  void
)
{
  return bt_qsoc_sleep_current_soc_sleep_state;
} /* bt_qsoc_sleep_sm_get_soc_state */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_TX_FLUSH_CB

  DESCRIPTION    This is the TX flush callback function. This function will
                 alert the Bluetooth task when the TX flush is complete.
                 This will allow the SoC to sleep and possible allow the
                 MSM to sleep.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sleep_tx_flush_cb
(
  void
)
{

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  /* Wait for MSM TX PATH to be idle Flushed before entering  SLEEP WAIT state ->TX BRK */  

  /* Reset the Flush timer if it is already running */
  (void) rex_clr_timer( &bt_qsoc_inband_tx_flush_timer );
  (void) rex_clr_sigs( &bt_tcb, BT_QSOC_SLEEP_SIG );
  (void) rex_set_timer( &bt_qsoc_inband_tx_flush_timer, BT_QSOC_INBAND_TX_FLUSH_TIMEOUT );    

#else
  bt_qsoc_sleep_msm_state_type msm_pending_state;

  bt_qsoc_sleep_sm_eval_event( BT_QSOC_SLEEP_EVENT_SIO_TX_IDLE, FALSE );
  
  /*
      The SIO TX path should now be clear.  If the MSM can now sleep,
      signal the BT task to do so.
  */
  msm_pending_state = bt_qsoc_sleep_sm_get_pending_msm_state();

  if (msm_pending_state == BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT ||
      msm_pending_state == BT_QSOC_SLEEP_MSM_STATE_ASLEEP)
  {
    (void) rex_set_sigs(&bt_tcb, BT_QSOC_SLEEP_SIG);
  }
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
  
} /* bt_qsoc_sleep_tx_flush_cb */


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_ISSUE_TX_FLUSH

  DESCRIPTION    Issues a TX flush to SIO so that the sleep state 
                 machine is notified when transmissions are complete.

  DEPENDENCIES   bt_qsoc_sleep_sm_init().

  PARAMETERS     stream_id_to_flush - the SIO stream to flush.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_issue_tx_flush
(
  sio_stream_id_type stream_id_to_flush
)
{

  /* State Machine must be initialized */
  if ( bt_qsoc_sleep_current_msm_sleep_state != BT_QSOC_SLEEP_MSM_STATE_UNDEF &&
       bt_qsoc_sleep_current_soc_sleep_state != BT_QSOC_SLEEP_SOC_STATE_UNDEF)
  {
    sio_ioctl_param_type  ioctl_param;
    
    /* Set the flush callback function */
    ioctl_param.record_flush_func_ptr = bt_qsoc_sleep_tx_flush_cb;
    
    /* 
       This IOCTL call can result in an immediate bt_qsoc_sleep_tx_flush_cb()
       call in BT context  or a future call in ISR context. When the 
       CB is called in ISR context, we need to protect against potential
       race conditions by Locking out ISRs 
   */
    bt_dc_sio_ioctl( stream_id_to_flush,
                     SIO_IOCTL_FLUSH_TX,
                     &ioctl_param );
  }
} /* bt_qsoc_sleep_issue_tx_flush */


#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_WAKEUP_FAIL

  DESCRIPTION    Try to recover the BT HOST from a INBAND SOC wakeup failure.
                 
  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   The thread will be suspended for 1 ms, up to
                 BT_QSOC_SLEEP_WAKE_ATTEMPTS times, while it re-attempts SOC wakeup.

==========================================================================*/
void bt_qsoc_sleep_inband_wakeup_fail
(
  void
)
{
  sio_ioctl_param_type        ioctl_param;
  boolean                     rx_line_high;
  uint8                       wake_attempts = 0;  
  ioctl_param.rx_line_state = &rx_line_high;

  /* Check if RX line is already High although the RX HIGH CB hasn't fired*/
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                       SIO_IOCTL_CHECK_RX_STATE,
                       &ioctl_param );   
     
  BT_ERR("BT QSOC Sleep Inband: SOC Wakeup attempt fails,Retrying.. st1:%d st2:%d st3:%d",
          bt_qsoc_sleep_inband_rx_state,
          bt_qsoc_sleep_inband_tx_state,
          rx_line_high );

  /* If the SOC is not responding, do a TX BREAK and UNBREAK to wakeup SOC again*/
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_TX_START_BREAK,
                   NULL );

  clk_busy_wait( BT_QSOC_SLEEP_TX_BRK_INTVL );

  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_TX_END_BREAK,
                   NULL );
                   
  /* Attempt again for another BT_QSOC_SLEEP_WAKE_ATTEMPTS to see if SOC wakes up */
  (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
  (void) rex_clr_sigs( &bt_tcb,BT_QSOC_INBAND_BREAK_RX_SIG );

  while((bt_qsoc_sleep_inband_tx_state==BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK)
         && ( wake_attempts < BT_QSOC_SLEEP_WAKE_ATTEMPTS ) )
  {
    rex_timed_wait( BT_QSOC_INBAND_BREAK_RX_SIG, &bt_qsoc_inband_rx_brk_timer, 1 );
    wake_attempts++;
  }

  if ( wake_attempts >= BT_QSOC_SLEEP_WAKE_ATTEMPTS)
  {
    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_CHECK_RX_STATE,
                     &ioctl_param );
                     
    BT_ERR( "BT QSOC Sleep: Final SOC wakeup attempt fails,Giving up st1:%d st2:%d st3:%d",
            bt_qsoc_sleep_inband_rx_state,
            bt_qsoc_sleep_inband_tx_state,
            rx_line_high );
  }
  else
  {
    BT_MSG_HIGH( "BT QSOC Sleep: Final SOC Wakeup attempt suceeds st1:%d st2:%d st3:%d",
                  bt_qsoc_sleep_inband_rx_state,
                  bt_qsoc_sleep_inband_tx_state,
                  bt_qsoc_sleep_inband_msm_shutdown );
  }    
}


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
)
{
  if (bt_qsoc_sleep_current_soc_sleep_state != BT_QSOC_SLEEP_SOC_STATE_UNDEF &&
      bt_qsoc_sleep_current_msm_sleep_state != BT_QSOC_SLEEP_MSM_STATE_UNDEF)
  {
    uint8                 wake_attempts = 0;
 
    BT_MSG_INBAND_DEBUG( "BT QSOC SLEEP: Inband Wake SOC st1:%d st2:%d st3:%d",
                   bt_qsoc_sleep_inband_rx_state,
                   bt_qsoc_sleep_inband_tx_state,
                   bt_qsoc_sleep_current_msm_sleep_state );    

    if( BT_QSOC_IS_READY() == TRUE )
    {
      /* 
        We need to wake up the SOC from Inband Sleep.Clear the RX BRK Timer\Sig
        as we no longer intend to go to RX sleep. At this point, wc are going to
        wakeup UARTDM and reuse RX BRK Sig for this purpose.
       */
      (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
      (void) rex_clr_sigs( &bt_tcb,BT_QSOC_INBAND_BREAK_RX_SIG );
    }
  
    if (bt_qsoc_sleep_current_soc_sleep_state == BT_QSOC_SLEEP_SOC_STATE_ASLEEP)
    {
      /* SoC is asleep, wake it and wait for it to be fully awake. */
      bt_qsoc_sleep_sm_change_soc_state(BT_QSOC_SLEEP_SOC_STATE_AWAKE);

      /* change_msm_state() will eventually set TX to UNBREAK to wakeup SOC */
      
      /* The MSM will be polling SIO, so the MSM will need to be awake */
      bt_qsoc_sleep_sm_change_msm_state(BT_QSOC_SLEEP_MSM_STATE_AWAKE);
      
      /* Wake up the SIO */
      bt_qsoc_sleep_update_sio();
    }
    else
    {
      BT_MSG_HIGH( "BT QSOC SLEEP: SOC already Awake  st1:%d st2:%d st3:%d",
               bt_qsoc_sleep_current_soc_sleep_state,
               bt_qsoc_sleep_inband_tx_state,
               bt_qsoc_sleep_inband_msm_shutdown );

    }

    if ( bt_qsoc_sleep_inband_tx_state != BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK )
    {
      BT_MSG_INBAND_HIGH( "BT QSOC SLEEP: MSM already Awake  st1:%d st2:%d st3:%d",
                     bt_qsoc_sleep_inband_rx_state,
                     bt_qsoc_sleep_inband_tx_state,
                     bt_qsoc_sleep_inband_msm_shutdown );  
    }

    (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
    (void) rex_clr_sigs( &bt_tcb,BT_QSOC_INBAND_BREAK_RX_SIG );
    /* 
      We arrived here becz UART is shutdown or INBAND TX PATH has voted to sleep
      We need to wait for UNBRK ACK from SOC to indicate that it has woken up.
     */
    while((bt_qsoc_sleep_inband_tx_state==BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK)
           && ( wake_attempts < BT_QSOC_SLEEP_WAKE_ATTEMPTS ) )
    {
      /* 
        Wait for 1 ms each time for the SoC to wake up. As RX Path needs to be awakened at
        this point,we re-use  bt_qsoc_inband_rx_brk_timer
       */   
      rex_timed_wait( BT_QSOC_INBAND_BREAK_RX_SIG, &bt_qsoc_inband_rx_brk_timer, 1 );
      wake_attempts++;      
    }

    if ( wake_attempts >= BT_QSOC_SLEEP_WAKE_ATTEMPTS)
    {        
      bt_qsoc_sleep_inband_wakeup_fail();
    }
    else
    {
      BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: # of Inband attempts Cnt:%d St1:%d St2:%d",
                     wake_attempts,
                     bt_qsoc_sleep_inband_rx_state,
                     bt_qsoc_sleep_inband_tx_state );
    }

    /* Restart the RX BREAK timer now that RX PATH is awake  */
    (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
    (void) rex_clr_sigs( &bt_tcb,BT_QSOC_INBAND_BREAK_RX_SIG );
    (void) rex_set_timer( &bt_qsoc_inband_rx_brk_timer, BT_QSOC_SLEEP_RX_BRK_INTVL );
  }
} /* bt_qsoc_sleep_wake_soc_inband */
#else
/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_WAKE_SOC

  DESCRIPTION    Sends a wake signal to the SoC and attempts to wait for the
                 SoC to be completely awake.  Since this is a general purpose
                 waking function, the SoC will not sleep until an event occurs
                 which will once again allow it to do so.

  DEPENDENCIES   bt_qsoc_sleep_sm_init().

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   The thread will be suspended for 1 ms, up to 
                 BT_QSOC_SLEEP_WAKE_ATTEMPTS times, while it waits for the SoC
                 to wake. 

==========================================================================*/
void bt_qsoc_sleep_wake_soc
(
  void
)
{
  if (bt_qsoc_sleep_current_soc_sleep_state != BT_QSOC_SLEEP_SOC_STATE_UNDEF &&
      bt_qsoc_sleep_current_msm_sleep_state != BT_QSOC_SLEEP_MSM_STATE_UNDEF)
  {
    sio_ioctl_param_type  ioctl_param;
   /* Initialize wakeup status variables appropriately */
    boolean               rts_deasserted = FALSE;
    uint8                 wake_attempts  = 0;

    if (bt_qsoc_sleep_current_soc_sleep_state == BT_QSOC_SLEEP_SOC_STATE_ASLEEP)
    {
      /* SoC is asleep, wake it and wait for it to be fully awake. */
      bt_qsoc_sleep_sm_change_soc_state(BT_QSOC_SLEEP_SOC_STATE_AWAKE);

      /* The MSM will be polling SIO, so the MSM will need to be awake */
      bt_qsoc_sleep_sm_change_msm_state(BT_QSOC_SLEEP_MSM_STATE_AWAKE);
      bt_qsoc_sleep_update_sio();
    }

    /* 
      The SIO interface assumes active high, so rts_asserted
      will be rts_deasserted for QSoC's active low case.
    */
    ioctl_param.rts_asserted = &rts_deasserted;
    /* Check if the SoC is still sleeping */
    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_GET_CURRENT_RTS, &ioctl_param );

    /* */
    while (rts_deasserted == TRUE && wake_attempts < BT_QSOC_SLEEP_WAKE_ATTEMPTS)
    {
      /* wait for 1 ms for the SoC to wake up */
      rex_timed_wait( BT_QSOC_SLEEP_T_SIG, &bt_qsoc_sleep_timer, 1 );

      bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                       SIO_IOCTL_GET_CURRENT_RTS, &ioctl_param );

      wake_attempts++;
    }

    if (wake_attempts >= BT_QSOC_SLEEP_WAKE_ATTEMPTS)
    {
      BT_ERR( "BT QSOC Sleep: bt_qsoc_sleep_wake_soc - SoC wakeup attempts exceeded", 0, 0, 0 );
    }

    /* Cleanup the signal and timer for later use */
    (void) rex_clr_timer( &bt_qsoc_sleep_timer );
    (void) rex_clr_sigs( &bt_tcb, BT_QSOC_SLEEP_T_SIG );
  }
} /* bt_qsoc_sleep_wake_soc */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/


#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
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
)
{

  if( BT_QSOC_IS_READY() == TRUE )
  {
    if ( (bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_ACTIVE_TRANSMIT)
      || (bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_DONE) )
    {
      BT_MSG_INBAND_HIGH( "BT QSOC INBAND Sleep Wait :MSM Sending TX BRK st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           bt_qsoc_sleep_inband_msm_shutdown );

      /* Start inband TX Break */
      bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_START_BREAK;
      bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                       SIO_IOCTL_TX_START_BREAK,
                       NULL );

      /* Do a busy wait for > 3.125 micro secs  =~ 10 bits at 3.2 mbps */
      clk_busy_wait( BT_QSOC_SLEEP_TX_BRK_INTVL );

      /* Vote the  Inband Sleep TX PATH  to Sleep */
      bt_qsoc_sleep_inband_proc_tx_brk_evt();

    }
    else
    {
      BT_MSG_INBAND_HIGH( "BT QSOC INBAND Sleep Wait:MSM Not Sending TX BRK st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           bt_qsoc_sleep_inband_msm_shutdown );        
    }
  }
}


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
)
{
  bt_qsoc_sleep_msm_state_type msm_state;  

  bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_DONE_BREAK;
  
  /* Check if SOC driver is enabled */
  if( BT_QSOC_IS_READY() == FALSE )
  {
    BT_ERR( "BT QSOC Sleep: TX BREAK Evt before SOC Init st1:%d st2:%d st3:%d",
             bt_qsoc_sleep_inband_rx_state,
             bt_qsoc_sleep_inband_tx_state,
             bt_qsoc_sleep_inband_msm_shutdown );
    return;
  }

  BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: TX BREAK Evt Recvd st1:%d st2:%d msm_sleep:%d",
                        bt_qsoc_sleep_inband_rx_state,
                        bt_qsoc_sleep_inband_tx_state,
                        bt_qsoc_sleep_inband_msm_shutdown );

  /* 
    The TX BREAK condition is not pre-emptible as we use clk_busy_wait() to 
    achieve TX BREAK for 1 Byte worth of Transmission.So no need to be concerned
    about intermediate INBAND TX SM states like BT_QSOC_ISLEEP_MSM_TX_START_BREAK
   */  
  
  bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP; 
  
    /* Check if the MSM can sleep, signal BT Task to do so */
  msm_state = bt_qsoc_sleep_sm_get_msm_state();
  if (msm_state == BT_QSOC_SLEEP_MSM_STATE_ASLEEP ||
      msm_state == BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT)
  {
    if ( bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED &&
         bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP    )
    {
      BT_MSG_INBAND_HIGH( "BT QSOC: Both Rx and Tx Paths Idle st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           bt_qsoc_sleep_inband_msm_shutdown );

      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SLEEP_TIMER, TRUE);
    }
    else
    {
      BT_MSG_INBAND_HIGH( "BT QSOC INBAND Sleep Vote failed st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           msm_state );          
    }
  }
  else
  {
    BT_MSG_INBAND_HIGH( "BT QSOC INBAND Not in Sleep Wait state  st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         msm_state );
  }  

} /* bt_qsoc_sleep_inband_proc_tx_brk_evt*/


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
)
{
  sio_ioctl_param_type  ioctl_param;
  
  /* Make  sure all RX BREAK sigs are cleared */
  (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
  (void) rex_clr_sigs( &bt_tcb, BT_QSOC_INBAND_BREAK_RX_SIG );

  /* Make sure SOC driver is enabled */
  if( BT_QSOC_IS_READY() == FALSE )
  {
    BT_MSG_INBAND_HIGH( "BT QSOC Sleep: BRK Evt recvd before SOC Init st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_inband_msm_shutdown );   
    return;
  }
  else
  {
    BT_MSG_INBAND_HIGH( "BT QSOC INBAND Sleep:RECVD PROB RX BRK FROM SOC st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_inband_msm_shutdown );
  }

  bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP;

  /* Set the RX high callback function */
  ioctl_param.rx_high_event_func_ptr = bt_qsoc_inband_rx_unbreak;
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_RX_HIGH_EVENT,
                   &ioctl_param );

      /* 
    If the MSM Inband path can sleep, signal Inband SM to do so
        Start sleep timer to confirm that RX path is idle.
        We might have been awakened  by UART activity since the flush timer was
        last started and hence Sleep failed. So clear the Flush signal and timer.
       */
  (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
  (void) rex_clr_sigs( &bt_tcb, BT_QSOC_INBAND_RX_FLUSH_SIG);
      
  /* If we are still in RX VOTE SLEEP State and not been awakened yet */
  if ( bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP )
  {
    BT_MSG_INBAND_DEBUG( "BT QSOC RX start RXIDLE PATH Timer st1:%d st2:%d st3:%d",
                          bt_qsoc_sleep_inband_rx_state,
                          bt_qsoc_sleep_inband_tx_state,
                          bt_qsoc_sleep_current_msm_sleep_state );

    rex_set_timer( &bt_qsoc_inband_rx_flush_timer, BT_QSOC_INBAND_RX_FLUSH_TIMEOUT );
  }
  else
  {
    BT_MSG_INBAND_HIGH( "BT QSOC INBAND Failed to start RX FLush timer st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_current_msm_sleep_state );
  }
} /* bt_qsoc_sleep_inband_proc_rx_brk_evt */


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
)
{
  sio_ioctl_param_type  ioctl_param;

  if( BT_QSOC_IS_READY() == FALSE )
  {
    /* SOC is not fully initialized and hence Inband Sleep can't be enabled */
    BT_MSG_HIGH( "BT QSOC Sleep: UNBREAK Evt recived before SOC is initialized",
                  0, 0, 0 );
    return;
  }
  else
  {
     /* UART must already have been woken up: Change the flag*/    
     bt_qsoc_sleep_inband_msm_shutdown = FALSE;
  }

  /* 
    SOC is Acking as it is woken up by UNBRK/MSM TX activity OR SOC is trying to
    wake up MSM by sending TX UNBRK. Either way we need to uninstall the handler
   */
  ioctl_param.rx_high_event_func_ptr = NULL;
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_RX_HIGH_EVENT,
                   &ioctl_param );

  if ( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK )
  {
    BT_MSG_INBAND_HIGH( "BT QSOC Inband: RECVD TX UNBRK ACK FROM SOC st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_inband_msm_shutdown );
    
    bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_ACTIVE_TRANSMIT;    
    /* 
      We arrived here because a TX was requested. The request CMD
      was requeued in the BT Q when it was first requested. So we revisit
      the BT CMD Q here Signal BT task to process Queued Cmd (if any) 
    */

    /* 
      This is a unbreak ACK meant for TX machine,But we need to send it to RX SM
      too. Rst the RX timer so that we can go to RX sleep again on inactivity.
      This timer will be rst on each subsequent RX packet. Since SOC has exited
      BRK on its TX line, it will have to BRK  again  before it can vot for sleep
      PS: At this point MSM doesn't need to send any UNBRK Ack to the SOC .
     */
    /* Unvote Inband RX SLEEP on reciept of RX unbreak & Rst the RX IDLE timer */
    bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;
    /* Clear RX FLUSH and RX BREAK signal & timer*/
    (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
    (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
    (void) rex_clr_sigs(  &bt_tcb, (BT_QSOC_INBAND_BREAK_RX_SIG | 
                                    BT_QSOC_INBAND_RX_FLUSH_SIG) );    
    (void) rex_set_timer( &bt_qsoc_inband_rx_brk_timer, BT_QSOC_SLEEP_RX_BRK_INTVL );

    /*
      This may not be a ACK UNBRK from SOC but a UNBRK to wakeup MSM. Both MSM &
      SOC can send wakeup TX UNBRKs at same instance. This race condition is ok.
      The MSM will view the UNBREAK wakeup from SOC as a UNBREAK Ack. The SOC may
      see the UNBRK wakeup from MSM as a UNBRK ACK too. In any case when  MSM\SOC
      transmits it will be viewed as a UNBRK ack by SOC\MSM.
     */
  }
  else  if ( bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP  ||
             bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED )
  {
    /* 
      MSM's TX PATH is already awake ,OR MSM is totally asleep! Any case,MSM RX
      path should wake up and send TX BRK ACK to SOC. RX SM requests TX SM to do
      so through a SIG as Inband TX and RX SMs are independent &  asynchronous.
     */ 
    BT_MSG_INBAND_HIGH( "BT QSOC INBAND Sleep: RECVD RX UNBRK FROM SOC st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_current_msm_sleep_state);
  
     /* Clear the RX Flush timer as we are exiting the RX SLEEP state */
    (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
    (void) rex_clr_sigs( &bt_tcb, BT_QSOC_INBAND_RX_FLUSH_SIG);
    
    bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_UNBRK_RCVD;

    /* This SIG is set from ISR context & may not get processed immediately */
    rex_set_sigs(&bt_tcb, BT_QSOC_INBAND_TX_ACK_REQD_SIG);

    /*
      The SIO driver would  have been awakend by SIO high activity at this stage,
      We need to do update the BT_DC states here
     */
  }
  else
  {
    BT_ERR( "BT QSOC Sleep: Unexpected Unbrk evt st1: %d st2: %d st3 %d", 
             bt_qsoc_sleep_inband_rx_state,
             bt_qsoc_sleep_inband_tx_state,
             bt_qsoc_sleep_inband_msm_shutdown );
  }

} /* bt_qsoc_inband_rx_unbreak */


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
)
{
  sio_ioctl_param_type  ioctl_param;

  if( BT_QSOC_IS_READY() == TRUE )
  {
    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_TX_START_BREAK,
                     NULL );

    /* Do a busy wait for 10 us =~ 10 bits of break at 3.2 mbps */
    clk_busy_wait( BT_QSOC_SLEEP_TX_BRK_INTVL );
  }
  else
  {
    BT_ERR( "BT QSOC Sleep: SOC not ready for Inband st:%d st2:%d st3:%d",
             bt_qsoc_sleep_inband_rx_state,
             bt_qsoc_sleep_inband_tx_state,
             bt_qsoc_sleep_inband_msm_shutdown );
  }

    /* Cleanup the signal and timers */
  (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
  (void) rex_clr_sigs( &bt_tcb,BT_QSOC_INBAND_BREAK_RX_SIG );

  bt_qsoc_sleep_inband_msm_shutdown  = FALSE;
  bt_qsoc_sleep_inband_tx_state      = BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP;
  bt_qsoc_sleep_inband_rx_state      = BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP;
  bt_qsoc_sleep_bring_up_sio_flg     = FALSE;
  bt_qsoc_sleep_bring_down_sio_flg   = FALSE;

  bt_qsoc_sleep_sm_init();
  
  /* 
     Register for  RX high Callback, this ISR triggered Call back can
     be called almost immediately causing a task context switch.
   */
  ioctl_param.rx_high_event_func_ptr = bt_qsoc_inband_rx_unbreak;
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_RX_HIGH_EVENT,
                   &ioctl_param );

  /* Now send out the remaining Initialization Cmds. */
  bt_soc_etc_init();
  
  BT_MSG_HIGH( "BT QSOC Sleep:Inband Sleep SM initialized: st1:%d st2:%d st3:%d",
                bt_qsoc_sleep_inband_rx_state,
                bt_qsoc_sleep_inband_tx_state,
                bt_qsoc_sleep_inband_msm_shutdown );
  
} /* bt_qsoc_sleep_inband_init */


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
)
{

  /* 
    First check whether TX path is already high before attempting UNBRK ACK.
    UNVOT TX BRK sleep on reciept of any kind of TX Unbreak ACKrequest, 
    restart the TX BRK condition and change state to 'TX BREAK START'.

    The TX BREAK condition is not pre-emptible as we use clk_busy_wait() to 
    achieve TX BREAK for 1 Byte worth of Transmission.So no need to be concerned
    about intermediate INBAND TX SM states like BT_QSOC_ISLEEP_MSM_TX_START_BREAK    
   */

  if( BT_QSOC_IS_READY() == FALSE )
  {
    BT_MSG_INBAND_HIGH( "BT QSOC Sleep:UnBrk Ack Req,SOC not ready st:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_inband_msm_shutdown );            
    /* Although MSM Inband SM is not intialized, we don't return here but still
       ACK the SOC.This is to handle the bug in  R3 SOCs where INband signalling
       starts before SOC recieves HCI RST */                         
  }

  if( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP )
  {
    /* No need to unmask RX HIGH ISR; we are acking TX UNBREAK;No need to TX */ 
    if( BT_QSOC_IS_READY() == TRUE )
    {
      BT_MSG_INBAND_HIGH( "BT QSOC INBAND :MSM Sending RX UNBRK ACK to SOC st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           bt_qsoc_sleep_inband_msm_shutdown );

      if (bt_qsoc_sleep_current_msm_sleep_state == BT_QSOC_SLEEP_MSM_STATE_ASLEEP)
      {
        /* Since SIO was already awakened due to RX HIGH now, We need to Change the MSM State to 
          AWAKE(from ASLEEP), need to update BT DC SIO State state..*/                    
        BT_MSG_INBAND_HIGH( "BT QSOC INBAND :MSM Updating SIO to wake st1:%d st2:%d st3:%d",
                             bt_qsoc_sleep_inband_rx_state,
                             bt_qsoc_sleep_inband_tx_state,
                             bt_qsoc_sleep_inband_msm_shutdown );

        /* The MSM will be polling SIO, so the MSM will need to be awake */
         bt_qsoc_sleep_sm_change_msm_state(BT_QSOC_SLEEP_MSM_STATE_AWAKE);
         /* Update the SIO State*/
        bt_qsoc_sleep_update_sio();
      }
    
      /* Ack TX unbreak */
      bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                       SIO_IOCTL_TX_END_BREAK,
                       NULL ); 
      bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_DONE;
    
     /*
      We may have been awakened by UART RX activity since 'RX FLUSH TIMER' was
      last started and hence Sleep on Rx flush failed. Clear Sig\Timer if it was
      previously running.
      RX PATH is awake,Rst RX timer so we can go to sleep on inactivity
     */
      (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
      (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );      
      (void) rex_clr_sigs(  &bt_tcb, (BT_QSOC_INBAND_BREAK_RX_SIG | 
                                      BT_QSOC_INBAND_RX_FLUSH_SIG) );    
      (void) rex_set_timer( &bt_qsoc_inband_rx_brk_timer, BT_QSOC_SLEEP_RX_BRK_INTVL );
      bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;

      /* 
       Vote the  Inband Sleep TX PATH  to Sleep again. Since the Inband RX PATH
       is awake now, this will not result in UART Shutdown conditions being met.
       No need to worry about RX path, No need to reset the TX flush timer as we
       directly are going to TX BREAK state & vote for inband sleep without any Idle wait now.    
      */

      if (bt_qsoc_sleep_current_msm_sleep_state != BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT)
      { 
        /* Trigger the SM to change to WAIT state and do a TX BREAK */
        bt_qsoc_sleep_sm_change_msm_state(BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT);
      }
      else
      {
        /* As SM was already in WAIT state, just enter the TX BRK condition again */
        bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_START_BREAK;
        bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                         SIO_IOCTL_TX_START_BREAK,
                         NULL );
        clk_busy_wait( BT_QSOC_SLEEP_TX_BRK_INTVL );  
        bt_qsoc_sleep_inband_proc_tx_brk_evt();
      }
    }
    else
    {
      BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: Inband SM not initialized st1:%d st2:%d st3:%d",
                            bt_qsoc_sleep_inband_rx_state,
                            bt_qsoc_sleep_inband_tx_state,
                            bt_qsoc_sleep_inband_msm_shutdown );

      /* Assert that RX\TX PATHs are awake as Inband sleep is not yet enabled */
      bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;
      bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_ACTIVE_TRANSMIT;
    }

  }
  else if( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_WAIT_UNBREAK_ACK
           || bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_ACTIVE_TRANSMIT)
  {
    /*
      No need to UNBRK ACK on MSM's TX line as the line is already High. Just
      poll TX line to make sure and assert a failure, if it is not the case
      But while Actively TXing, the line might be transitioning!. So we need
      to rely on the State info instead of the physical TX line state.
    */
    BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: UNBREAK ACK reqd st1:%d st2:%d st3:%d",
                          bt_qsoc_sleep_inband_rx_state,
                          bt_qsoc_sleep_inband_tx_state,
                          bt_qsoc_sleep_current_msm_sleep_state );

    
    /*
      Restart the RX FLUSH and BREAK timers so that we can go to RX sleep 
      again on inactivity 
    */
    (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
    (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );      
    (void) rex_clr_sigs(  &bt_tcb, (BT_QSOC_INBAND_BREAK_RX_SIG | 
                                      BT_QSOC_INBAND_RX_FLUSH_SIG) );    
    (void) rex_set_timer( &bt_qsoc_inband_rx_brk_timer, BT_QSOC_SLEEP_RX_BRK_INTVL );
    bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;    
  }
  else if( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_DONE )
  {
    BT_MSG_INBAND_HIGH( "BT QSOC Sleep: FATAL UNBRK ACK reqst st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_inband_msm_shutdown );
  }
  else if( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_DONE_BREAK )
  {
    BT_MSG_INBAND_DEBUG( "BT QSOC Sleep: FATAL UNBREAK ACK reqst st1:%d st2:%d st3:%d",
                          bt_qsoc_sleep_inband_rx_state,
                          bt_qsoc_sleep_inband_tx_state,
                          bt_qsoc_sleep_inband_msm_shutdown );
  }
  else if( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_START_BREAK )
  {
    /*
      This should never happen, The TX BREAK condition is not pre-emptible as we
      use clk_busy_wait() to achieve TX BREAK for 1 Byte worth of Transmission.
      The TX state machine can't be in the middle of a TX break 10 bit interval
    */ 
    BT_ERR( "BT QSOC Sleep: FATAL ERRPR,Inband TX BREAK was prempted st1: %d st2: %d st3: %d",
             bt_qsoc_sleep_inband_rx_state,
             bt_qsoc_sleep_inband_tx_state,
             bt_qsoc_sleep_inband_msm_shutdown );
  }  
  else
  {
    /* ACK TX UNBREAK Anycase. This also handles the bug in R3 SOCs where INband
       signalling starts before SOC recieves HCI RST */
    BT_MSG_INBAND_HIGH( "BT QSOC Sleep:MSM Sending RX UNBRK ACK to SOC st1:%d st2:%d st3:%d",
                         bt_qsoc_sleep_inband_rx_state,
                         bt_qsoc_sleep_inband_tx_state,
                         bt_qsoc_sleep_inband_msm_shutdown );

    /* No need to unmask RX HIGH ISR; we are acking TX UNBREAK;No need to TX */
    /* Ack TX Unbreak */
    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_TX_END_BREAK,
                     NULL );

    /* Re-enter TX break condition */
    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_TX_START_BREAK,
                     NULL );
    bt_qsoc_sleep_inband_tx_state = BT_QSOC_ISLEEP_MSM_TX_START_BREAK;

    /* Do a busy wait for > 3.125 micro secs  =~ 10 bits at 3.2 mbps */
    clk_busy_wait( BT_QSOC_SLEEP_TX_BRK_INTVL );

    /* 
       Vote the  Inband Sleep TX PATH  to Sleep again. Since the Inband RX PATH
       is awake now, this will not result in UART Shutdown conditions being met.
       No need to restart the TX flush timer as we are going to TX BREAK state
       without any Idle wait now.
    */
    bt_qsoc_sleep_inband_proc_tx_brk_evt();
    
    /*
      Restart the RX FLUSH and BREAK timers so that we can go to RX sleep 
      again on inactivity 
    */
    (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
    (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );      
    (void) rex_clr_sigs(  &bt_tcb, (BT_QSOC_INBAND_BREAK_RX_SIG | 
                                    BT_QSOC_INBAND_RX_FLUSH_SIG) );    
    (void) rex_set_timer( &bt_qsoc_inband_rx_brk_timer, BT_QSOC_SLEEP_RX_BRK_INTVL );
    bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;   
  }
} /* bt_qsoc_sleep_inband_proc_tx_ack_reqd_sig */


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
)
{

  if ( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_START_BREAK )  
  {
    /*Can't TX now, set a flag and process TX REQs  after TX BRK ends */
    /* 
      NOTE: The Inband TX state machine can never be in  the middle of a TX break
      10 bit interval when a Transmissoni is attempted by HCI Driver.
    */ 
    return TRUE;
  }
  
  return FALSE;
}/* bt_qsoc_sleep_inband_in_brk */


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
)
{
  if( BT_QSOC_IS_READY() == TRUE )
  {
    if ( (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_UNBRK_RCVD) ||
         (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP)||
         (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED))
    {
      BT_LOG_INBAND_STATE( "BT QSOC SLEEP: INBAND Stale RX activity");

     /* We recvd stale data from the DMOV\UART buffers. The SOC might be actually
        in BRK yet. So just clear the timers but not the RX state. If we change 
        the RX state it might result in lost RX HIGH(wakeup) from SOC.
            This is a essential consequence of the Sleep FLUSH Timeout value being
        10 millsecs  which is less then RX BRK INTVL = 3 msecs.  Restart the Flush
        timer as we are seeing more RX activity here. No need to change the Inband
        Sleep RX Path state  to BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT*/
      (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
      (void) rex_clr_sigs( &bt_tcb,BT_QSOC_INBAND_RX_FLUSH_SIG);
      (void) rex_set_timer( &bt_qsoc_inband_rx_flush_timer, BT_QSOC_INBAND_RX_FLUSH_TIMEOUT );
    }
    else
    {
      /* 
        Restart the RX BREAK Timer so that we can go to RX sleep again on inactivity
        This timer will be reset on each subsequent RX packet.
        Also Cancel the Flush timer as we are seeing more RX activity here
       */
      (void) rex_clr_timer( &bt_qsoc_inband_rx_brk_timer );
      (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
      (void) rex_clr_sigs(  &bt_tcb,(BT_QSOC_INBAND_BREAK_RX_SIG | 
                                     BT_QSOC_INBAND_RX_FLUSH_SIG) );
      (void) rex_set_timer( &bt_qsoc_inband_rx_brk_timer, BT_QSOC_SLEEP_RX_BRK_INTVL );
      bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_ACTIVITY_WAIT;
    }
  }
  
}/* bt_qsoc_sleep_inband_rx_new_pkt */


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
)
{
  if ( (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_VOTED_SLEEP) ||
       (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_UNBRK_RCVD) ||
       (bt_qsoc_sleep_inband_rx_state == BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED) )
  {
    if( BT_QSOC_IS_READY() == TRUE )
    {
       BT_MSG_HIGH( "BT QSOC INBAND Stale Inband RX activity st1:%d st2:%d st3%d",
                     bt_qsoc_sleep_inband_rx_state,
                     bt_qsoc_sleep_inband_tx_state,
                     bt_qsoc_sleep_current_msm_sleep_state );
    }
  }

}/* bt_qsoc_sleep_inband_log_rx_activity */


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
)
{
  return bt_qsoc_sleep_inband_tx_state;
} /* bt_qsoc_sleep_inband_get_tx_state */

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
)
{
  return bt_qsoc_sleep_inband_rx_state;
} /* bt_qsoc_sleep_inband_get_rx_state */

/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_RX_FLUSH_SIG

  DESCRIPTION    This function processes "UART\DMOV RX" buffers empty evt.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_inband_proc_rx_flush_sig
( 
  void
)
{
  if( BT_QSOC_IS_READY() == TRUE )      
  {
    /* Clear the signal and timer*/
    (void) rex_clr_timer( &bt_qsoc_inband_rx_flush_timer );
    (void) rex_clr_sigs( &bt_tcb, BT_QSOC_INBAND_RX_FLUSH_SIG);
         
    bt_qsoc_sleep_inband_rx_state = BT_QSOC_ISLEEP_MSM_RX_PATH_FLUSHED;

    if ( bt_qsoc_sleep_inband_tx_state == BT_QSOC_ISLEEP_MSM_TX_VOTED_SLEEP )
    {
      BT_MSG_INBAND_HIGH( "BT QSOC:  Both Rx and Tx Paths Idle st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           bt_qsoc_sleep_current_msm_sleep_state );                   
        
      /* Notify SM that sleep shutdown can be started as TX/RX buffers are empty */
    bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SLEEP_TIMER, TRUE);
  }
    else
    {
      BT_MSG_INBAND_HIGH( "BT QSOC:Tx Path not Idle st1:%d st2:%d st3:%d",
                           bt_qsoc_sleep_inband_rx_state,
                           bt_qsoc_sleep_inband_tx_state,
                           bt_qsoc_sleep_current_msm_sleep_state );
    }
  }
}


/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP_INBAND_PROC_TX_FLUSH_SIG

  DESCRIPTION    This function processes "MSM TX" buffers empty evt.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep_inband_proc_tx_flush_sig
(
  void
)
{
  /*
    The SIO TX path should now be clear.  If the MSM can now sleep,
    signal the BT task to do so.
  */  
  BT_MSG_INBAND_HIGH( "BT QSOC INBAND: TX Flush done st1:%d st2:%d msm_st:%d",
                       bt_qsoc_sleep_inband_rx_state,
                       bt_qsoc_sleep_inband_tx_state,
                       bt_qsoc_sleep_current_msm_sleep_state );
   
  bt_qsoc_sleep_sm_eval_event( BT_QSOC_SLEEP_EVENT_SIO_TX_IDLE, TRUE );
  
}
    
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
#endif /* FEATURE_BT_QSOC_SLEEP */
#endif /* FEATURE_BT_QSOC */
#endif /* FEATURE_BT */


