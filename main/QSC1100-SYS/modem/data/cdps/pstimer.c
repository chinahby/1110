/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
       D A T A   S E R V I C E S   A P I  T I M E R  

GENERAL DESCRIPTION
  This is the source file for managing the PS timer API. The implementation 
  is a standard delta list.
  
  PLEASE NOTE THAT THE FUNCTIONS CAN BE CALLED ONLY FROM A SINGLE TASK 
  CONTEXT, CURRENTLY IT IS ONLY PS TASK CONTEXT.
  

  TERMINOLOGY USED:
    Tick Timer: The timer that expires every PS_TIMER_TICK_INTERVAL 
                duration.
    API Timers: The timers that are given to the application.           

  STEPS FOR USING THE TIMERS:
    To use the timers do the following steps:
        1. Obtain a timer by calling ps_timer_alloc(). If the timer is 
           available in the free pool, then a timer handle is returned, else 
           it returns PS_TIMER_FAILURE. Pass the callback function that 
           should be called on the expiry of the timer. After the timer 
           expires, the callback function is called from the PS task 
           context.
           
        2. After getting a valid timer handle, start the timer by calling
           ps_timer_start(). The time should be given in milli seconds.
           
        3. If you would like to cancel a particular timer, then use the
           function ps_timer_cancel(). Note that the function does not 
           free it, the application can still hold the handle.
           
        4. If you are done with using the timer, call ps_timer_free and
           it will return the timer to the free pool.
        
        5. If you would like to check if a particular timer is running, then 
           use the function, ps_timer_is_running(). This returns TRUE if the
           timer is running.

  GRANULARITY OF THE TIMER:
    The granularity of the timer depends on the PS_TIMER_TICK_INTERVAL. 
    The timer will be executed at a time given by (not considering task 
    context and execution times):
        if(mod (timer_val/PS_TIMER_TICK_INTERVAL) != 0)
            timer expiry will be: 
            PS_TIMER_TICK_INTERVAL - mod (timer_val/PS_TIMER_TICK_INTERVAL)
            more than requested time;
        else 
            timer expiry = Timer value;
            
  TIMING ANALYSIS: 
        When a new timer is allocated for the first time, the 
        PS_TIMER_TICK is set and it will go off at every 
        PS_TIMER_TICK_INTERVAL duration. This interval is maintained by the
        clock services routines. This ensures accuracy of the timer, even if
        PS task is lower priority.
        
        This timer is then used to clock the other timers used in this API.
    
        
        (0)-------(1)-------(2)------(3)-------(4)-------(5)
        (0), (1), (2), (3), (4), (5) are times when PS_TIMER_TICK goes off 
        and the handler routine is executed in the timer ISR executed by the
        clock services. That ISR will then set a signal to the PS task to
        handle the signal. Timer ISR and PS task, have a shared variable and 
        this is used to indicate how many timer ISR's have been executed 
        before the PS task got the signal. This ensures that the accuracy 
        of timer is better.
        
        If you set a timer value to be less than or equal to 
        PS_TIMER_TICK_INTERVAL then it may be executed immediately. So, 
        application has to take care of it. Observe in the above figure that, 
        if you set a timer to value less than or equal to 
        PS_TIMER_TICK_INTERVAL, between (0) and (1), then it will be 
        executed at (1). 

    EXAMPLE OF TIMING ANALYSIS:
        PS_TIMER_TICK_INTERVAL = 50  i.e. 50 milli seconds
    
        ---(1)--(2)(3)------(4)-(4a)(5)-------(6)-(6a)-(7)--------(8)-(8a)-(9)--------(A)(B)
        (1) -> Application allocates the Timer by calling ps_timer_alloc;
               Done in Application task            
        (2) -> Application calls ps_timer_start; 
               Application registers MY_CALLBACK and sets MY_TIMER = 175 
               (i.e. 175 msecs)
               Since this is the first timer, API_TICK_TIMER is also started
               but it is transparent to the application.
        (3) -> API_TICK_TIMER starts and must expire every 50 msecs
        (4) -> API_TICK_TIMER expires; Its 50msecs since (3). 
        (4a) -> Control goes to clock services.
               Clock services ISR will set a signal to the PS task and 
               increments the common counter.
                MY_TIMER = 125;
        (5) -> API_TICK_TIMER is serviced by PS task now. 
        (6) -> API_TICK_TIMER expires; Its 50msecs since (4a)
        (6a) -> Control goes to clock services.
               Clock services ISR will set a signal to the PS task and 
               increments the common counter.
                MY_TIMER = 75;
        (7) -> API_TICK_TIMER is serviced by PS task now;
        (8) -> API_TICK_TIMER expires; Its 50msecs since (6a)
        (8a) -> Control goes to clock services.
               Clock services ISR will set a signal to the PS task and 
               increments the common counter.
                MY_TIMER = 25;
        (9) -> API_TICK_TIMER is serviced by PS task.
        (A) -> API_TICK_TIMER expires; Its 50msecs since (8a)
        (B) -> API_TICK_TIMER is serviced. Call MY_CALLBACK. Cancel API_TICK_TIMER and 
               stop MY_TIMER.
  
        (1) to (2) are done in the application task context.
        (3) to (B) are done in PS task context.

  State Transitions of the Timer:
  
        (Starting State)------(function to be called)--------(Ending State)
        FREE -------------------ps_timer_alloc-------------------> STOP
        
        STOP -------------------ps_timer_start-------------------> RUN
                                with non-zero timer value
                               
        RUN --------------------ps_timer_start ------------------> RUN
                                with non-zero timer value
                                  
        RUN --------------------ps_timer_cancel------------------> STOP
        
        STOP--------------------ps_timer_free--------------------> FREE




RELATIONSHIP BETWEEN PS TASK AND THE TIMER API:
    1. Timer Initializations:
          The API Tick Timer is initialized in the PS task by calling the 
          function ps_timer_tick_init() during the initialization.
          
          The API timers are initialized by calling ps_timer_init in
          the PS task.
    2. When ever Tick timer expires, REX sets a signal to the PS task. The 
       signal is PS_TIMER_TICK_SIG. On receiving this signal, PS should
       call the callback that was registered during the initialization.
    3. PS TASK SHOULD BE CALLED BEFORE ANY APPLICATION CALLS THIS TIMER API.

RELATIONSHIP BETWEEN PS TASK AND CLOCK SERVICES:
   1. The PS task registers the timer with the clock services. They decrement
      the timer every PS_TIMER_TICK_INTERVAL. Then the inform the PS task.
   2. PS task receives a signal from the callback called by clock services. 
   
   Data exchanged between PS task and Clock services:
   1. The ps_ticks_elapsed_cntr is common between the clock services and PS
      task. When ever timer expires, the clock services will increment this
      counter by one. Hence, when the PS task gets control, it will know 
      time elapsed between it saw the signal and when the signal was set by 
      multiplying ps_ticks_elapsed_cntr with PS_TIMER_TICK_INTERVAL.
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_timer_init() MUST be called before any of the other functions can 
  be called and have predictable behaviour.
  Since the API_TICK_TIMER is initialized in PS task, PS task should be 
  initialized before using these timers.
  
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
    1) ps_timer_handler: Function to be executed every time the tick 
       timer expires.
  

Copyright (c) 2000-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/pstimer.c_v   1.2   19 Feb 2003 16:17:42   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/pstimer.c#2 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/24/08    pp     Fixed RVCT compiler warnings.
06/11/03    jd     Added ps_timer_remaining to check time left on timer
02/19/03    jd     Save timer # for MSG_ display in timer handler callback
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/02/02    usb    Merged in changes for multimode ps support under 
                   FEATURE_DATA_MM
06/06/02    jd     Keep track of elapsed ms instead of ticks, since timer
                   might expire much later than one tick (50ms) as seen in
                   the case of dormancy/mobile ip reregistration.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
08/29/01    jd     timer handler was not calling timer callback until 1+ 
                   ticks after timer expired. fixed. Added rev. header field. 
08/08/01    jd     modified ps_timer_handler to handle multiple timers 
                   expiring on the same PS_TIMER_TICK_SIG
06/07/01    jd     Modified the time argument to ps_start_timer() to a sint31
                   so that longer times can be specified.
02/06/01    snn    Modified the code to use the clock services instead of
                   maintaining the timer. This ensures that the timer is 
                   more accurate. (specially since PS task has very low
                   priority).
11/27/00   na      Minor cleanup.
11/11/00   snn     Added new callback functions for PS task interface.
                   Removed errno in the functions because it is never used.
                   Modified ps_timer_tick_init for better decoupling of PS
                     task and timer api module.
8/20/00    snn/na  Created this module.

===========================================================================*/
 
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          INCLUDE FILES FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#if defined (FEATURE_DS) || defined(FEATURE_DATA_PS)

#include "msg.h"
#include "ps_svc.h"
#include "clk.h"
#include "pstimer.h"
#include "assert.h"
#include "task.h"
#include <string.h>

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        LOCAL DECLARATIONS FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*---------------------------------------------------------------------------
  States of the timer  
  FREE: Timer is available in the pool and can be used. This should be 
        initialized to zero, because we use memset to zero initalize the 
        ps_timer_list and so, FREE should equal zero.
  STOP: Timer has been allocated, but not running.
  RUN:  Timer is in active list. 
---------------------------------------------------------------------------*/
typedef enum
{
  PS_TIMER_FREE = 0,           
  PS_TIMER_STOP,
  PS_TIMER_RUN
} timer_state_type;

/*---------------------------------------------------------------------------
  The timer structure has the following fields:
    time_left:  Contains the delta value from the timer that is inserted in 
                front of it. If this timer is the first timer in the queue, 
                this value will be decremented by PS_TIMER_INTERVAL every 
                PS_TIMER_INTERVAL milli-seconds. The input should be 
                given only in milliseconds.
                This should be a signed type, because this can have a 
                negative value before it is removed from the list.
                
    timer_callback_fcn:   
                The function pointer points to the function that will be 
                executed when the timer expires.  If this is NULL, no
                callback is called when the timer expires.  It will be 
                intialized to NULL during initialization and freeing the 
                timer.
                
    timer_callback_param:
                This contains the parameter to be passed to the callback
                function.
                
    next:       The pointer points to the next element of the timer list. 
                This pointer should be initialized to NULL during 
                initialization and freeing the timer.
                
    prev:       The pointer points to the previous element of the timer list.
                This pointer should be initialized to NULL during 
                initialization and freeing the timer.
                
    timer_state: This holds the state of the timer. If the timer is defined
                but not being used, it is in PS_TIMER_STOP mode. If it is 
                not yet defined, it is in PS_TIMER_FREE mode. If the timer 
                is ticking then it is in PS_TIMER_RUN mode. During 
                initialization this field should be made as PS_TIMER_FREE.

---------------------------------------------------------------------------*/
typedef struct time_list
{
  timer_cb_type     *timer_callback_fcn;      /* callback function pointer */
  void              *timer_callback_param;  /* callback function parameter */
  struct time_list  *next;             /* pointer to the next node in list */
  struct time_list  *prev;             /* ptr to the previous node in list */
  sint31            time_left;                 /* this is in milli-seconds */
  timer_state_type  timer_state;                  /* contains timers state */
} timer_entry_type;

/*---------------------------------------------------------------------------
  The API Tick timer. This timer expires every PS_TIMER_TICK_INTERVAL given
  in milliseconds.
  Global variable and will be shared by the clock services and PS task. This
  stores the timer charectaristics. 
---------------------------------------------------------------------------*/
clk_cb_type ps_tick_timer_struct;

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
sint31 ps_ms_elapsed_cntr = 0;

/*---------------------------------------------------------------------------
  This list contains all the API timers. 
  The head of the list is ps_timer_list, it is a timer entry like the 
  nodes in the list.  This was done to facilitate insertions and removals as 
  it gets rid of special cases and simplifies the code.
---------------------------------------------------------------------------*/
static timer_entry_type ps_timer_list[PS_TIMER_MAX+1];

/*---------------------------------------------------------------------------
  The function pointers for the Timer tick callbacks. These are called from 
  the PS task context. We have 3 callback functions: To start tick timer,
  to stop tick timer, to process when tick timer expires.

   To start timer tick call:               ps_timer_tick_start_cb_ptr
   To stop timer tick call:                ps_timer_tick_stop_cb_ptr
   To handle timer tick expiration call:   ps_timer_tick_handle_cb_ptr
   
   We made these functions as callback because, we wanted to make the 
   pstimer code decoupled from ps task code.
---------------------------------------------------------------------------*/
void (*ps_timer_tick_start_cb_ptr)(int4);
tick_timer_cb_type *ps_timer_tick_stop_cb_ptr;
tick_timer_cb_type *ps_timer_tick_handle_cb_ptr;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
timer_entry_type *psi_timerhandle_to_timer
(
  uint8 handle
);

static void psi_timer_insert
(
  timer_entry_type *timer  /* pointer to the timer being inserted          */
);

static void psi_timer_remove
(
  timer_entry_type *timer  /* the timer to remove */
);




/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                       REGIONAL DEFINITIONS FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_TIMER_INIT()

DESCRIPTION
  This function initializes the timer data structures.
    timer_callback_fcn:   Initialize to NULL 
    timer_callback_param: Initialize to NULL
    next:                 Initialize to NULL 
    prev:                 Initialize to NULL
    time_left:            Initialize to 0;
    timer_state:          Initialize to PS_TIMER_FREE
    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the ps_timer_list structure contents.

===========================================================================*/
void ps_timer_init()
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
   /*---------------------------------------------------------------------
     Initialize the timer fields to default values. 
     
     All the pointers are initialized to NULL.
     The timer state should be made as FREE, since it can be used.
     The Time value is made as zero.
   ---------------------------------------------------------------------*/
  memset( ps_timer_list, 
          0, 
          ((PS_TIMER_MAX + 1) *(sizeof(timer_entry_type)) )
        ); 

} /* ps_timer_init() */



/*===========================================================================
FUNCTION PS_TIMER_ALLOC

DESCRIPTION
  This function allocates a timer. Before any task uses the timer, it should
  first acquire a timer by calling alloc_timer. This function returns a timer 
  handle to the caller. This timer handle can be used in the future to access
  this timer (until it is freed).
  
  Every timer may have a callback associated with it.  So, it takes the 
  function callback as an argument.  The callback function must have the 
  following signature: void fname(void *);  If NULL is specified as the 
  callback parameter, no function is called.
  
DEPENDENCIES
  After the timer is used, call FREE_TIMER. This will return the timer back 
  to the free pool.
  
RETURN VALUE
   Success: Returns value between 1 to PS_TIMER_MAX.
   Failure: PS_TIMER_FAILURE 

SIDE EFFECTS
  None
===========================================================================*/
int ps_timer_alloc
(
  void (* callback) (void *),
  void *cb_param
)
{
  /*-------------------------------------------------------------------------
    timer_handle returns the handle of the free timer. 
    
    Initialize timer_handle to zero, because, if we allocate a timer handle 
    this value becomes non-zero. We check for the value to confirm if any 
    timers are available. 
  -------------------------------------------------------------------------*/
  uint8 timer_handle = 0; 
  
  /*-------------------------------------------------------------------------
    looping index
  -------------------------------------------------------------------------*/
  uint8 loop;

  /*-------------------------------------------------------------------------
    index is used to allocate the timer handle. It stores the value of the 
    of the index allocated previously. Hence, its made as static. This allows
    application to allocate different indices every time a new timer is 
    requested. Even if a timer is allocated and freed alternatively, we 
    ensure that different handles are returned.
  -------------------------------------------------------------------------*/
  static uint8 index = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find out if there are timers available.
        Search the array from 1 instead of 0, because 0 is not used as timer 
        but only acts as the head of the list. 
        Search until the index becomes PS_TIMER_MAX because this will 
        give the last valid timer.
        
    If there are no free timers available then return PS_TIMER_FAILURE 
    
    Note that we use loop and index to obtain the timer handle. This is 
    useful because, it ensures that we always allocate different timer 
    handles when more than one free timer exists. 
  -------------------------------------------------------------------------*/
  for (loop = 1; loop <= PS_TIMER_MAX; loop++ ) 
  {
      /*---------------------------------------------------------------------
        If the index reaches its maximum, then wrap around to make it initial
        value.
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Find out if any of the timers are in free pool. If yes, then copy 
        the index into handle of that timer and break. 
      ---------------------------------------------------------------------*/
      index++;
      if( index > PS_TIMER_MAX)
      {
        index =1;
      }
      if (ps_timer_list[index].timer_state == PS_TIMER_FREE) 
      {
          timer_handle = index;
          break; 
      }
  }
  
  /*-------------------------------------------------------------------------
    If timer_handle equals 0, it implies there are no free timers available.
    Inform this to the calling application by returning error.
  -------------------------------------------------------------------------*/
  if(timer_handle == 0 )
  {
    MSG_ERROR("No more ds timers available!", 0,0,0);
    return PS_TIMER_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Modify the state of the timer to STOP
    Reinitalize the prev and next pointer values; 
    the timer value to zero; and 
    place the callback in the appropriate slot.
    Place the callback parameter. This parameter should be passed to the 
    callback function, when it is called.
    The timer handle can be returned. 
  -------------------------------------------------------------------------*/
  ps_timer_list[timer_handle].timer_state          = PS_TIMER_STOP;
  ps_timer_list[timer_handle].prev                 = NULL;
  ps_timer_list[timer_handle].next                 = NULL;
  ps_timer_list[timer_handle].time_left            = 0;
  ps_timer_list[timer_handle].timer_callback_fcn   = callback;
  ps_timer_list[timer_handle].timer_callback_param = cb_param;

  return timer_handle;

} /* ps_timer_alloc() */



/*===========================================================================
FUNCTION PS_TIMER_START()

DESCRIPTION
  This function takes a timer handle, the time in milli-seconds for the timer 
  and a pointer to error number as arguments. 
  
DEPENDENCIES
  If this function is called for the first time, then we start the timer 
  tick. Since, timer tick is initialized in PS task context, the PS task 
  should already be initialized before calling this function. One simple way
  to confirm this is, to check the priority of the PS task and the task 
  calling this function. If PS Task priority is higher than calling task, 
  then we are safe.

RETURN VALUE
 PS_TIMER_FAILURE if there is an error setting the timer.
 PS_TIMER_SUCCESS: if the setting is success. 

SIDE EFFECTS
  If the timer is the first timer in the list, then a PS timer is started
  which goes off every PS_TIMER_TICK_INTERVAL duration.
  If the the time_left is set to zero, the timer will be stopped i.e. removed
  from the list.
  
===========================================================================*/
ps_timer_error_type ps_timer_start
(
  uint8 handle,
  sint31 time
)
{

  timer_entry_type *timer;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_LOW ("Set timer #%d (%d ms)", handle, time, 0);

  /*-------------------------------------------------------------------------
    Check if the handle passed is valid and obtain the valid timer address.
  -------------------------------------------------------------------------*/
  
  if( (timer = psi_timerhandle_to_timer(handle)) == NULL)
  {
    MSG_ERROR(" %d is not a valid handle!\n", handle,0,0);
    return PS_TIMER_FAILURE;
  }                                            
  
  /*-------------------------------------------------------------------------
    Check if this first timer in the list. If yes, then start the tick timer. 
    (This expires every PS_TIMER_TICK_INTERVAL).
  -------------------------------------------------------------------------*/
  if(ps_timer_list->next == NULL)
  {
    ps_timer_tick_start_cb_ptr(PS_TIMER_TICK_INTERVAL);
  }

  /*-------------------------------------------------------------------------
    If the timer is on the list, take it off the list.
    Note that the timer is on the list only if it is in RUN. If it is STOP,
    or FREE it is not in the list. So, the prev field should be NULL in all
    other cases except RUN.
  -------------------------------------------------------------------------*/
  if(timer->prev != NULL)
  {
    psi_timer_remove(timer);
  }

  /*-------------------------------------------------------------------------
    If the timer is not being set to zero, then insert it onto the list. 
    Note that if the time is set to zero, then it is not appended to the 
    list.  Must also check that the time is not negative.
    Also note that time_left field may be modified by psi_timer_insert 
    depending on location of this timer in the timer list.
  -------------------------------------------------------------------------*/
  if(time != 0)
  {
    if (time > 0)
    {
      timer->time_left = time;
      psi_timer_insert(timer);
    }
    else
    {
      MSG_ERROR("Negative timeout (%d) specified!\n", time,0,0);
      return PS_TIMER_FAILURE;
    }
  }

  return (  PS_TIMER_SUCCESS);
 } /* ps_timer_start() */



/*===========================================================================
FUNCTION PS_TIMER_CANCEL()

DESCRIPTION
  Removes the API Timer from the active list of timers. 
  
DEPENDENCIES
  None

RETURN VALUE
 PS_TIMER_FAILURE if there is an error cancelling the timer.
 PS_TIMER_SUCCESS: if the cancelling is success. 

SIDE EFFECTS
  
===========================================================================*/
ps_timer_error_type ps_timer_cancel
(
  uint8 handle
)
{

  timer_entry_type *timer;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_LOW ("Cancel timer #%d", handle, 0, 0);

  /*-------------------------------------------------------------------------
    Check if the handle passed is valid and obtain the valid timer address.
  -------------------------------------------------------------------------*/
  
  if( (timer = psi_timerhandle_to_timer(handle)) == NULL)
  {
    MSG_ERROR(" %d is not a valid handle!\n", handle,0,0);
    return PS_TIMER_FAILURE;
  }                                            
  
  /*-------------------------------------------------------------------------
    If the timer is on the list, take it off the list.
    Note that the timer is on the list only if it is in RUN. If it is STOP,
    or FREE it is not in the list. So, the prev field should be NULL in all
    other cases except RUN.
  -------------------------------------------------------------------------*/
  if(timer->prev != NULL)
  {
    psi_timer_remove(timer);
  }

  return (PS_TIMER_SUCCESS);
 } /* ps_timer_cancel() */



/*===========================================================================
FUNCTION PS_TIMER_FREE()

DESCRIPTION
  This function frees a timer from the delta list.

DEPENDENCIES
  Note that the application cannot free a timer twice. If a same timer is
  freed twice, it may be usurping on another timer. So, this application
  returns ERROR, if a free timer is again requested to be freed.
  
RETURN VALUE
  PS_TIMER_SUCCESS: If free'd.
  PS_TIMER_FAILURE: If cannot free

SIDE EFFECTS
  None
===========================================================================*/
ps_timer_error_type ps_timer_free
(
      uint8 handle
)
{
  
  timer_entry_type *timer;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the handle passed is valid and obtain the pointer to the timer.
  -------------------------------------------------------------------------*/
  if( (timer = psi_timerhandle_to_timer(handle)) == NULL)
  {
    MSG_ERROR(" %d is not a valid handle!\n", handle,0,0);
    return PS_TIMER_FAILURE;
  }                                            

  MSG_MED("freeing timer %d ", timer - ps_timer_list,0,0);

  /*-------------------------------------------------------------------------
    If the timer is on the list, take it off the list. 
    Note that if this timer happens to be last on the list, then the tick 
    timer is also cancelled.
  -------------------------------------------------------------------------*/
  if(timer->prev != NULL)
  {
    psi_timer_remove(timer);
  }

  /*-------------------------------------------------------------------------
    Make sure this timer is not attached to anything and update its state.
  -------------------------------------------------------------------------*/
  timer->next                 = NULL;
  timer->prev                 = NULL;
  timer->timer_state          = PS_TIMER_FREE;
  timer->time_left            = 0;
  timer->timer_callback_fcn   = NULL;
  timer->timer_callback_param = NULL;

  return PS_TIMER_SUCCESS;

} /* ps_timer_free() */



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
boolean ps_timer_is_running
(
  uint8 handle
)
{ 
  timer_entry_type *timer;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the handle passed is valid and obtain the pointer to the timer.
  -------------------------------------------------------------------------*/
  if( (timer = psi_timerhandle_to_timer(handle)) == NULL)
  {
    MSG_ERROR(" %d is not a valid handle!\n", handle,0,0);
    return FALSE;
  }                                            

  /*-------------------------------------------------------------------------
    Check the timer state. If the timer is running then return TRUE, 
    else return FALSE.
  -------------------------------------------------------------------------*/
  if(timer->timer_state == PS_TIMER_RUN)
  {
    return TRUE;
  }               
  else
  {
    return FALSE;
  } 
} /* ps_timer_is_running() */


/*===========================================================================
FUNCTION PS_TIMER_REMAINING()

DESCRIPTION
  This function is used to check the time left on a running timer.

  Accurate to 1 ps timer tick (50ms)
  
DEPENDENCIES
  None

RETURN VALUE
  If timer is running, time left (in ms)
  If timer is stopped, 0
  If timer does not exist, -1

SIDE EFFECTS
  None
===========================================================================*/
sint31 ps_timer_remaining
(
  uint8 handle
)
{ 
  timer_entry_type *timer;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the handle passed is valid and obtain the pointer to the timer.
  -------------------------------------------------------------------------*/
  if( (timer = psi_timerhandle_to_timer(handle)) == NULL)
  {
    MSG_ERROR(" %d is not a valid handle!\n", handle,0,0);
    return -1;
  }                                            

  /*-------------------------------------------------------------------------
    Check the timer state to determine how much time is left. 
  -------------------------------------------------------------------------*/
  if (timer->timer_state == PS_TIMER_RUN)
  {
    /*-----------------------------------------------------------------------
      Note to be exact here, would have to check rex time
    -----------------------------------------------------------------------*/
    sint31 time_left = -ps_ms_elapsed_cntr;

    MSG_MED ("ps_ms_elapsed_cntr = %d", ps_ms_elapsed_cntr,0,0);

    /*-----------------------------------------------------------------------
      If the timer is running then return the remaining time.
    -----------------------------------------------------------------------*/
    while (timer != ps_timer_list)
    {
      MSG_MED ("timer %d left %d", timer - ps_timer_list, timer->time_left,0);
      time_left += timer->time_left;
      timer = timer->prev;
    }
    
    MSG_MED ("Timer %d has %d ms left", handle,time_left,0);

    return time_left;
  }               
  else
  {
    /*-----------------------------------------------------------------------
      If the timer is not running then return 0 time remaining.
    -----------------------------------------------------------------------*/
    MSG_MED ("Timer %d not running", handle,0,0);
    return 0;
  }
} /* ps_timer_remaining() */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         LOCAL FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PSI_TIMERHANDLE_TO_TIMER()

DESCRIPTION
  This function converts the handle passed into the address of the timer. 
  Checks if the handle passed is valid or not. If it is not valid, it returns
  FALSE; If it is valid, it returns TRUE.

DEPENDENCIES
  This should be called only after a valid timer handle is obtained by 
  calling pstimer_alloc_timer

RETURN VALUE
   
  NULL: if timer handle is not valid
  address:  If passed handle is valid.

SIDE EFFECTS
  None
===========================================================================*/
timer_entry_type *psi_timerhandle_to_timer(uint8 handle)
{
  /*-----------------------------------------------------------------------
    Confirm that the handle is within the range it should be.
  -----------------------------------------------------------------------*/
  if( (handle == 0 ) || (handle > PS_TIMER_MAX))
  {
    return NULL;
  }
    
  /*-------------------------------------------------------------------------
    Confirm that the timer is not FREE. This confirms that a valid timer
    is being used. 
  -------------------------------------------------------------------------*/
  if(ps_timer_list[handle].timer_state == PS_TIMER_FREE)
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Use handle to get pointer to timer and return it.
  -------------------------------------------------------------------------*/
  return(ps_timer_list + handle);                

} /* psi_timerhandle_to_timer */



/*===========================================================================
FUNCTION PSI_TIMER_INSERT()

DESCRIPTION
  This function inserts a timer into the delta list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Timer list is modified.
===========================================================================*/
static void psi_timer_insert
(
  timer_entry_type *timer  /* pointer to the timer being inserted          */
)
{
  timer_entry_type *curr_timer;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(timer == NULL)
  {
    return;
  }
  curr_timer = ps_timer_list;

  /*-------------------------------------------------------------------------
    find insertion point
  -------------------------------------------------------------------------*/
  while(curr_timer->next != NULL)
  {
    /*-----------------------------------------------------------------------
      ONLY insert if the time left on the next node is greater than - thus if
      it is equal it will be inserted AFTER - this maintains FIFO ordering on
      dispatching timer callbacks.
      
      Note:  timer->time_left -= curr_timer->time_left cannot result in 
      a negative number.
    -----------------------------------------------------------------------*/
    if(curr_timer->next->time_left > timer->time_left)
    {
      break;
    }
    else
    {
      curr_timer = curr_timer->next;

      /*-----------------------------------------------------------------------
        Note:  timer->time_left -= curr_timer->time_left cannot result in 
        a negative number.
      -----------------------------------------------------------------------*/
      timer->time_left -= curr_timer->time_left;
    }
  } /* while(insertion point not found) */

  /*-------------------------------------------------------------------------
    we are now pointing to the node after which we want to insert the timer
    so insert it.
  -------------------------------------------------------------------------*/
  timer->next = curr_timer->next;
  timer->prev = curr_timer;
  curr_timer->next = timer;
  
  /*-------------------------------------------------------------------------
    Update state of the timer
  -------------------------------------------------------------------------*/
  timer->timer_state = PS_TIMER_RUN;

  /*-------------------------------------------------------------------------
    If there is a timer following the one we inserted, update its time, and
    make sure that it points to us
  -------------------------------------------------------------------------*/
  if(timer->next != NULL)
  {
    timer->next->time_left -= timer->time_left;
    timer->next->prev = timer;
  }

} /* psi_timer_insert() */


/*===========================================================================
FUNCTION PSI_TIMER_REMOVE()

DESCRIPTION
  This function removes a timer from the delta list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Timer list is modified.
===========================================================================*/
static void psi_timer_remove
(
  timer_entry_type *timer  /* the timer to remove */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(timer == NULL)
  {
    return;
  }

  MSG_MED("removing timer %d ", timer - ps_timer_list,0,0);

  /*-------------------------------------------------------------------------
    If there is a timer following the current one, update the links, so
    previous points to it, and update the delta on the next link.
  -------------------------------------------------------------------------*/
  if(timer->next != NULL)
  {
    timer->next->prev = timer->prev;
    timer->next->time_left += timer->time_left;
  }

  /*-------------------------------------------------------------------------
    Timers are removed from the list if they are known to be on the list - 
    if they are on the list they have a previous node - even if it is the 
    head of the list.
  -------------------------------------------------------------------------*/
  timer->prev->next = timer->next;


  /*-------------------------------------------------------------------------
    Make sure this timer is not attached to anything.
  -------------------------------------------------------------------------*/
  timer->next = NULL;
  timer->prev = NULL;
  
  /*-------------------------------------------------------------------------
    Update state of the timer
  -------------------------------------------------------------------------*/
  timer->timer_state = PS_TIMER_STOP;

} /* psi_timer_remove() */


/*===========================================================================
FUNCTION PS_TIMER_TICK_INIT()

DESCRIPTION
  This function is called from the PS task context.
  This function initializes the Tick timer by defining it and also the 
  function pointer that should be called when timer tick expires.

  The function takes 3 parameters for initializing the callback function
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
void ps_timer_tick_init
(
    tick_timer_cb_type *timer_cb_ptr,
    void(*tick_timer_start) (int4),
    tick_timer_cb_type *tick_timer_stop

)
{

  /*-------------------------------------------------------------------------
    Initialize the timer used by clk service.
  -------------------------------------------------------------------------*/  
  clk_def(&ps_tick_timer_struct);

  /*-------------------------------------------------------------------------
    Copy the function pointer that should be executed when the tick timer 
    expires.
  -------------------------------------------------------------------------*/
  ps_timer_tick_handle_cb_ptr = timer_cb_ptr;
     
  /*-----------------------------------------------------------------------
    Copy the callback functions to start or stop the tick timer.
  -----------------------------------------------------------------------*/
  ps_timer_tick_start_cb_ptr = tick_timer_start;
  ps_timer_tick_stop_cb_ptr  = tick_timer_stop;

} /* ps_timer_tick_init() */



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
void ps_timer_handler
(
  void
)
{

  timer_cb_type* cb_fcn; /* Temporary storage of call back function        */
  void * cb_param;       /* Temp storage of call back function parameters  */
  sint31  elapsed_ms;    /* local copy to avoid cntr contention issues     */
  timer_entry_type *  was_timer;   /* the timer being handled/removed      */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /*-------------------------------------------------------------------------
     Clear the Timer Signal
   -------------------------------------------------------------------------*/
   PS_CLR_SIGNAL(PS_TIMER_TICK_SIGNAL);

   /*-------------------------------------------------------------------------
    Check if any of the timers are active or not. If they are not active,
    this signal need not be executed. So cancel the API_TIMER and return. 
    Once a while we might hit the MSG_HIGH, it is still okay.
  -------------------------------------------------------------------------*/
  if(ps_timer_list->next == NULL)
  {
    MSG_HIGH("timer_handler() called with no timers!",0,0,0);
    ps_timer_tick_stop_cb_ptr();
    return;
  }
  
  /*-------------------------------------------------------------------------
    Reset the value of ps_ticks_elapsed_cntr to zero.  This should be kept
    inside INTLOCK and INTFREE, because it is a global variable and clock
    ISR can change its value. 
  -------------------------------------------------------------------------*/
  INTLOCK();
  elapsed_ms = ps_ms_elapsed_cntr;
  ps_ms_elapsed_cntr = 0;
  INTFREE();

  /*-------------------------------------------------------------------------
    Reduce the timer. The value by which we reduce the timer depends on the
    ps_ticks_elapsed_cntr. The value is: ps_ticks_elapsed_cntr times 
    PS_TIMER_TICK_INTERVAL. This ensures that, even if PS did not respond
    to its signal within 50msecs, we can still have accurate count.
    
    Note that ps_ticks_elapsed cntr may be zero under some racing conditions
    and this looks okay.  

    It's ok for time_left to be negative... remove timer will subtract the
    extra time from the next timer in the list.
  -------------------------------------------------------------------------*/
  ps_timer_list->next->time_left -= elapsed_ms;

  /*-----------------------------------------------------------------------
    After reducing the timer, if the timer has not expired, 
    then return back, because, we have nothing else to do.

    Loop to handle the case where multiple timers expire.
  -----------------------------------------------------------------------*/
  while ((was_timer = ps_timer_list->next) != NULL && was_timer->time_left <= 0) 
  {
    /*-----------------------------------------------------------------------
      Copy the callback function and its parameter so that, we can remove 
      the timer before calling the callback function.
    -----------------------------------------------------------------------*/
    cb_fcn = was_timer->timer_callback_fcn;
    cb_param = was_timer->timer_callback_param;

    /*-----------------------------------------------------------------------
      Remove the timers from the list.  ps_timer_list is a doubly linked 
      list and psi_timer_remove will modify the timers on either side of 
      the one being removed to point at each other.

      NOTE: We should remove the timer before calling the call back function 
      because, the callback function can start the same timer, and we should
      not be removing it.
      Since existing timer is negative we will subtract the value of the 
      subsequent timer before removing it.
    -----------------------------------------------------------------------*/
    psi_timer_remove(was_timer);
    
    /*-----------------------------------------------------------------------
      Before calling a callback confirm that the callback is not NULL. Then, 
      dispatch all the timers that are ready, by calling their callbacks. Do 
      this until all the timers with delta values equal to zero as head of 
      the list.
    -----------------------------------------------------------------------*/
    if( cb_fcn != NULL)
    {
      MSG_MED ("Timer #%d expired - calling callback", 
               was_timer - ps_timer_list, 0, 0); 
      cb_fcn( cb_param);
    }
  }

  return;

} /* ps_timer_handler() */
#endif /*  FEATURE_DS || FEATURE_DATA_PS */
