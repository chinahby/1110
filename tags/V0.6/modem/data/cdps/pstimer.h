#ifndef PS_TIMERS_H
#define PS_TIMERS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S T I M E R . H

GENERAL DESCRIPTION
  This is the source file for managing the API timers. The implementation is 
  a standard delta list. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_timer_init() MUST be called before any other timer functions are 
  called.

EXTERNALIZED FUNCTIONS
  Initialization functions:
    1) ps_timer_init(void): Initialize the api timers.
    2) ps_timer_tick_init(tick_timer_cb_type *timer_cb_ptr): Initialize
       the Tick Timer that is used to tick other API timers.
    
  API Functions to be called by applications using Timer:
    1) ps_timer_alloc():   Function to allocate a timer from free pool.
    2) ps_timer_start():   Start the timer
    3) ps_timer_cancel():  Cancel the timer
    4) ps_timer_free() :   Free the timer 
    5) ps_timer_is_running:Returns if the timer is running or not.
     
  PS Task to Timer API interface functions:
    1) ps_timer_start_tick: Starts the tick timer
    2) ps_timer_cancel_tick: Cancels the tick timer.
    3) ps_timer_handler: Function to be executed every time the tick 
       timer expires.

  Copyright (c) 2000-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/pstimer.h_v   1.2   06 Nov 2002 13:26:00   ikhan  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/pstimer.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/11/03    jd     Added ps_timer_remaining to check time left on timer
05/04/03    ss     Increased PS_TIMER_SOCKET_TIMERS to account for timer 
                   needed to support SO_LINGER socket option
11/06/02    ifk    Included dssdns.h and incremented PS timers for DNS.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/10/02    mvl    fixed number of timers used for PPP.
08/13/02    mvl    Fixed MM featurization.
08/04/02    mvl    Simplified MM featurization.
08/01/02    usb    Merged in changes for multimode ps under FEATURE_DATA_MM
07/12/02    jd     Added pvcs header tag to edit history
06/06/02    jd     Keep track of elapsed ms instead of ticks, since timer
                   might expire much later than one tick (50ms) as seen in
                   the case of dormancy/mobile ip reregistration.
03/13/02    pjb    There are 4 timers per TCB now not 3.
01/25/02    ss     Increased the maximum timers to 8 to account for the timer
                   used by DNS
07/27/01    mvl    Added 4 extra timers for LCP and IPCP: 2 for each iface
06/07/01    jd     Modified the time argument to ps_start_timer() to a sint31
                   so that longer times can be specified.
05/18/01  mvl/na   Changed the way the max timers is defined to more cleanly
                   support multiple features.
02/06/01    snn    Modified the code to use the clock services instead of
                   maintaining the timer. This ensures that the timer is 
                   more accurate. (specially since PS task has very low
                   priority).
11/27/00    na     Cleanup. Moved timer_entry_type to pstimer.c
11/02/00    snn    Kept PS_TIMER_MAX in #define for FEATURE_DS_SOCKETS
                   Initialized PS_TIMER_FREE to zero. This allows us to
                   memset the timer structure.
                   Deleted unwanted defines from ps_timer_error_type
09/11/00   snn/na  created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dssocket.h"
#include "clk.h"

#ifdef FEATURE_DS_SOCKETS
#include "ps_ppp_defs.h"
#include "dssdns.h"
#endif /* FEATURE_DS_SOCKETS */

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Define the maximum number of the timers. 
  The number of timers are defined as:

    For sockets, 1 timer per socket for the linger timer, 4 timers per TCP 
    socket plus one for each iface (used in the DSSNET state machine when 
    waiting for physical iface to come up)

    For Mobile IP, 2 timers for registration, 1 for solication

    3 timers for async data calls when SOCKETS is not defined & for safety.

    2 timers for each PPP instance (LCP/Auth and IPCP)
    1 more timer per PPP instance in MM data for total setup timeout 

    1 to time out a DNS query (this timer is freed up upon timeout or upon
      receiving a response to the DNS query)

  Note that this number must be lower than the maximum value that the handler
  can have. Since handler is uint8, this value cannot go above 255.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
#define PS_TIMER_PPP_TIMERS (3 * PPP_MAX_DEV)
#else 
#define PS_TIMER_PPP_TIMERS (2 * IFACE_MAX_STACK)
#endif /* FEATURE_DATA_MM */

#ifdef FEATURE_DS_SOCKETS
  #define PS_TIMER_SOCKET_TIMERS (DSS_MAX_TCP_SOCKS * 4 + DSS_MAX_SOCKS)
  #define PS_TIMER_DNS_TIMERS    (DSS_DNS_PS_TIMER)
#else
  #define PS_TIMER_SOCKET_TIMERS 0
  #define PS_TIMER_DNS_TIMERS    (0)
#endif /* FEATURE_DS_SOCKETS */
  
#ifdef FEATURE_DS_MOBILE_IP
// fix this - should be 1 + 3 * (max # sessions)
  #define PS_TIMER_MIP_TIMERS 4
#else
  #define PS_TIMER_MIP_TIMERS 0
#endif

#define PS_TIMER_MAX (8 +                       \
                      PS_TIMER_SOCKET_TIMERS +  \
                      PS_TIMER_MIP_TIMERS +     \
                      PS_TIMER_DNS_TIMERS +     \
                      PS_TIMER_PPP_TIMERS)

/*---------------------------------------------------------------------------
  This interval decides how often, the timer in the head of the timer list
  is reduced. Thus, this also decides the clock's granularity. 
  After the first timer is registered, every PS_TIMER_TICK_INTERVAL 
  interval the tick timer is reduced. After the last timer is freed, the tick 
  timer is disabled.  
  We currently chose 50 because only TCP timers are currently using this API
  and granularity of 50 milliseconds is fine for TCP.
---------------------------------------------------------------------------*/
#define PS_TIMER_TICK_INTERVAL 50

/*---------------------------------------------------------------------------
  The type of the callback function: the signature of the function should be:
     void function(int)
---------------------------------------------------------------------------*/
typedef void(timer_cb_type)(void *);

/*---------------------------------------------------------------------------
  Errors that the API's are passed
  The values should be only 0 and -1. Donot add to these. 
---------------------------------------------------------------------------*/
typedef enum
{
  PS_TIMER_FAILURE = -1,
  PS_TIMER_SUCCESS = 0 
} ps_timer_error_type;
                       
extern clk_cb_type ps_tick_timer_struct;

/*---------------------------------------------------------------------------
  This stores the number of timer ticks that elapsed before the PS task gets
  to service the timer tick signal.
  
  Purpose of this variable is: PS task is a low priority task. And the timer 
  goes off every PS_TIMER_TICK_INTERVAL. If the value of this interval is 
  reduced there is no guarantee that PS task gets control that often. So, 
  clock services will update this value every tick i.e.PS_TIMER_TICK_INTERVAL
  And the PS task will update its timer value by ps_ticks_elapsed_cntr times 
  PS_TIMER_TICK_INTERVAL. This gives an accuracy of 5msec, because that's 
  current granularity of clock services.
  
  type: int16. This is made int16 because of following reason: This variable
  shows the maximum time for which PS task did not read the signal.The 
  maximum time is limited by the DOG kick time, which is currently 59 secs. 
  So by making this as int16, the time is 32K * 50msecs(currently). This can
  never occur and we need not check for overflow.
  
  This has been made as global because it is used by the clock services and
  PS task. PS task will read the value and clear it to zero. The clock 
  services will always increment it by 1 and never read it. (Note that the
  overflow check need not be done on this as explained above). Since Clock
  services is higher priority than the PS task, always use INTLOCK/FREE 
  before modifying this value.
---------------------------------------------------------------------------*/
extern sint31 ps_ms_elapsed_cntr; 

/*---------------------------------------------------------------------------
  The callback functions type used by this module.
---------------------------------------------------------------------------*/
typedef void(tick_timer_cb_type)(void);

/*---------------------------------------------------------------------------
  The function pointers for the Timer tick call backs. These are called from 
  the PS task context. We have 3 callback functions: To start tick timer,
  to stop tick timer, to process when tick timer expires.

   To start timer tick call:               ps_timer_tick_start_cb_ptr
   To stop timer tick call:                ps_timer_tick_stop_cb_ptr
   To handle timer tick expiration call:   ps_timer_tick_handle_cb_ptr
   
   We made these functions as call back because, we wanted to make the 
   pstimer code decoupled from ps task code.
---------------------------------------------------------------------------*/
extern void (*ps_timer_tick_start_cb_ptr)(int4);
extern tick_timer_cb_type *ps_timer_tick_stop_cb_ptr;
extern tick_timer_cb_type *ps_timer_tick_handle_cb_ptr;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_TIMER_INIT()

DESCRIPTION
  This function initializes the timer data structures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ps_timer_init(void);

/*===========================================================================
FUNCTION PS_TIMER_ALLOC()

DESCRIPTION
  This function allocates a timer. Before any task uses the timer, it should
  first acquire a timer by calling alloc_timer. This function returns a timer 
  handle to the caller. This timer handle can be used in the future to access
  this timer (until it is freed).
  
  It is assumed that every timer should have a valid call back associated 
  with it. So, it takes the function callback as an argument.  The callback 
  function must have the following signature: void fname(void *); 
  
DEPENDENCIES
  After the timer is used, call FREE_TIMER. This will return the timer back 
  to the free pool.
  
RETURN VALUE
   Success: Returns value between 1 to PS_TIMER_MAX.
   Failure: PS_TIMER_FAILURE 

SIDE EFFECTS
  None
===========================================================================*/
extern int ps_timer_alloc
(
  void (* callback) (void *),
  void *cb_param
);

/*===========================================================================
FUNCTION PS_TIMER_START()

DESCRIPTION
  This function takes a timer handle, the time in milli-seconds for the timer 
  and a pointer to error number as arguments. 
  
DEPENDENCIES
  None

RETURN VALUE
 PS_TIMER_FAILURE if there is an error setting the timer.
 PS_TIMER_SUCCESS: if the setting is success. 

SIDE EFFECTS
  If the timer is the first timer in the list, then a PS timer is started
  which goes off every PS_TIMER_TICK_INTERVAL duration.
  If the the time_left is set to zero, the timer will be stopped i.e. removed
  from the list.
  
===========================================================================*/
extern ps_timer_error_type ps_timer_start
(
  uint8 handle,
  sint31 time
);

/*===========================================================================
FUNCTION PS_TIMER_CANCEL()

DESCRIPTION
  This function cancels the timer.
  
DEPENDENCIES
  None

RETURN VALUE
 PS_TIMER_FAILURE if there is an error cancelling the timer.
 PS_TIMER_SUCCESS: if the cancelling is success. 

SIDE EFFECTS
  
===========================================================================*/
ps_timer_error_type ps_timer_cancel( uint8 handle);

/*===========================================================================
FUNCTION PS_TIMER_FREE()

DESCRIPTION
  This function frees a timer from the delta list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern ps_timer_error_type ps_timer_free
(
      uint8 handle
);


/*===========================================================================
FUNCTION PS_TIMER_TICK_INIT()

DESCRIPTION
  This function is called from the PS task context.
  This function initializes the Tick timer by defining it and also the 
  function pointer that should be called when timer tick expires.

  The function takes 3 parameters for initializing the call back function
  pointers. ps_timer_tick_init is called by ps task and informs pstimer 
  module the functions to be called for starting, stopping and handling the 
  tick timer. This ensures that the PSTIMER routine does not know anything 
  about ps task.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  updates the global variables, ps_timer_tick_start_cb_ptr 
  ps_timer_tick_stop_cb_ptr, ps_timer_tick_handle_cb_ptr.

===========================================================================*/
extern void ps_timer_tick_init
(
    tick_timer_cb_type *timer_cb_ptr,
    void(*tick_timer_start) (int4),
    tick_timer_cb_type *tick_timer_stop
);



/*===========================================================================
FUNCTION PS_TIMER_START_TICK()

DESCRIPTION
  This function is called from the PS task context.
  This function starts the TICK timer.

  Note that we use a separate function to start the tick timer instead
  of using genaralized PS start timer routine, because, we are interested
  in decoupling the functionality of the PS API timer from all other
  PS timers.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ps_timer_start_tick(void);

/*===========================================================================
FUNCTION PS_TIMER_CANCEL_TICK()

DESCRIPTION
  This function is called from the PS task context.
  This function cancels the TICK timer.

  Note that we use a separate function to cancel the tick timer instead
  of using genaralized PS start timer routine, because, we are interested
  in decoupling the functionality of the PS API timer from all other
  PS timers.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ps_timer_cancel_tick(void);

/*===========================================================================
FUNCTION PS_TIMER_HANDLER()

DESCRIPTION
  This function is the handler which is called when the INTERVAL Timer  goes
  off.  It checks if the timer on the head has expired, if yes, then it 
  dispatches the callbacks associated with the first timer and any other
  timers that are to go off at the same time.

DEPENDENCIES
 None
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ps_timer_handler(void);

/*===========================================================================
FUNCTION PS_TIMER_IS_RUNNING()

DESCRIPTION
  This function can be used to check if a timer is running.
  
  Note that, even if a non valid handle is passed, this function returns 
  FALSE, because that timer is not running. 
  An example of where this function can be used is, in TCP delayed ack
  timers, always check if the timer is running before starting the timer 
  again.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the Timer is running
  False: If the Timer is not Running

SIDE EFFECTS
  None
===========================================================================*/
extern boolean ps_timer_is_running(uint8 handle);

/*===========================================================================
FUNCTION PS_TIMER_REMAINING()

DESCRIPTION
  This function is used to check the time left on a running timer.

DEPENDENCIES
  None

RETURN VALUE
  If timer is running, time left (in ms)
  If timer is stopped, 0
  If timer does not exist, -1

SIDE EFFECTS
  None
===========================================================================*/
extern sint31 ps_timer_remaining(uint8 handle);

#endif /* PS_TIMERS_H */
