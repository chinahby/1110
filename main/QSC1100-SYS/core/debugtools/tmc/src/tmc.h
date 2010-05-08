#ifndef TMC_H
#define TMC_H
/*===========================================================================

                    T A S K   M A I N   C O N T R O L L E R

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  the Task Main controller task.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/tmc/tmc.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/09   pk      Reverting back ESN_ME changes as it is benig used by UIM
04/07/09   pk      Moving the ESN_ME and MEID_ME related changes out of TMC
03/30/09   pk      Added support to store MEID_ME for FEATURE_HWID_SUPPORT
12/26/08   br      Reduced TMC_HEAP_SIZE to 24KB under the feature FEATURE_LOWTIER_LOWMEM
07/09/04   jqi     Corrected the parameters for tmc_realloc() API.
06/17/04   jqi     Reduced the tmc heap to 100K if FEATURE_RRC_SIB_HEAP is 
                   defined.
04/09/04   jqi     Doulbed TMC memory heap size.
11/20/03   jqi     Added support for SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM.
07/01/03   jqi     Added support for feature reset counter.
06/04/03   jqi     CR29279-Removed 12K of small heap to large heap
05/15/03   jqi     CR30468-Increase MEM_HEAP_THRESHOLD to 64.
03/27/03   rj      Reduced tmc_heap size to 94208 from 194208.
03/25/03   jqi     Added prototype of tmc_get_mem_heap_size().
12/31/02   jqi     Merged with MSM6200 TMC.H#7
                   1.added tmc_heap_small.
                   
09/26/02   jqi     Remove customer.h header file.
09/25/02   jqi     Added TMC_WAIT_SIG.
08/05/02   jqi     Correct a typo.
07/29/02   jqi     Added TMC_CMD_TYPE_ONLINE. 
                   Added support for TMC command in GSM/WCDAM.
07/24/02   jqi     TMC reorganization.
07/09/02   jqi     Changes for code review.
06/14/02   jqi     Changes per 1st week of I&T.
06/06/02   jqi     Initial release.

===========================================================================*/

#include "comdef.h"               /* Definition for basic types and macros */
#include "memheap.h"                            /* Memory Heap header file */
#include "queue.h"                                    /* Queue header file */
#include "cmd.h"

/*---------------------------------------------------------------------------
                             TMC MACROS
---------------------------------------------------------------------------*/
/*
** If multi mode feature is defined, use tmc_task_start() whenever 
** mc_task_start is called.
*/
/* #define  mc_task_start(a,b,c) tmc_task_start()*/

/*---------------------------------------------------------------------------
                             TMC SIGNALS
---------------------------------------------------------------------------*/
#define  TMC_ACK_SIG           0x0001        /*  State change ack signal   */
#define  TMC_CMD_Q_SIG         0x0002        /*  TMC command signal        */
#define  TMC_RPT_TIMER_SIG     0x0004        /*  Watchdog report signal    */
#define  TMC_NV_CMD_SIG        0x0008        /*  NV command response signal*/
#define  TMC_WAIT_SIG          0x0010        /*  Wait signal               */
/*---------------------------------------------------------------------------
                             TMC COMMAND TYPE
---------------------------------------------------------------------------*/
/* Enum of CM command types.
*/
typedef enum {

    TMC_CMD_TYPE_NONE = -1,
    TMC_CMD_TYPE_ONLINE,
    TMC_CMD_TYPE_OFFLINE,
    TMC_CMD_TYPE_POWERDOWN,
    TMC_CMD_TYPE_RESET,
    TMC_CMD_TYPE_LPM,
    TMC_CMD_TYPE_MAX

} tmc_cmd_e_type;

/* Map TMC command type from 1x to GSW/WCDMA */
#define  TMC_OFFLINE_F   TMC_CMD_TYPE_OFFLINE
#define  TMC_ONLINE_F    TMC_CMD_TYPE_ONLINE
#define  TMC_RESET_F     TMC_CMD_TYPE_RESET
#define  TMC_POWERDOWN_F TMC_CMD_TYPE_POWERDOWN
#define  TMC_LPM_F       TMC_CMD_TYPE_LPM
#define  TMC_MAX_CMDS    TMC_CMD_TYPE_MAX

/*---------------------------------------------------------------------------
                             TMC COMMAND STRUCTURE
---------------------------------------------------------------------------*/
/* Type to hold all TMC commands.
*/
typedef struct tmc_cmd_s{

    cmd_hdr_type      hdr;
         /* Header of command

           This element has to be the first
           element in this data structure
           because of removal of self_ptr
           in q_link_type
         */

    tmc_cmd_e_type    cmd_id;
        /* Type of TMC command */

        /* Add other element here */

} tmc_cmd_type;

/*---------------------------------------------------------------------------
                             TMC STATE
---------------------------------------------------------------------------*/
/* This lists all possible substates for Task and Mode Controller 
*/
typedef enum
{
  TMC_ONLINE,
  /* Phone is in online state */
  TMC_OFFLINE,
  /* Phone is in offline state */
  TMC_RESET,
  /* Phone is reset. */
  TMC_PWROFF,
  /* Phone is power off */
  TMC_LPM,
  /* Phone is in low power mode. */
  TMC_OFFLINE_IF_NOT_FTM,   
  /* This is a state when Phone could be in either FTM mode or offline mode 
  ** based on current phone state. If phone is in FTM mode, it stays in FTM.
  ** OW it will be in offline.*/
  TMC_STATE_MAX
}tmc_state_e_type;

/* --------------------------------------------------------------------------
                   Intertask command heap definitions
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_DYNA_MEM
  #define   MEM_HEAP_THRESHOLD 64
  #ifdef FEATURE_RRC_SIB_HEAP
  	#ifdef FEATURE_LOWTIER_LOWMEM
  	   #define 	 TMC_HEAP_MEM_BUFFER_SIZE    24576
  	#else
      #define   TMC_HEAP_MEM_BUFFER_SIZE    102400
   #endif
  #else
  #define   TMC_HEAP_MEM_BUFFER_SIZE    212992
  #endif
  /* Task command heap */
  extern    mem_heap_type   tmc_heap;
  extern uint8 tmc_heap_mem_buffer[TMC_HEAP_MEM_BUFFER_SIZE];

  #ifndef FEATURE_LOW_MEMORY_USAGE 
    #define   TMC_HEAP_SMALL_MEM_BUFFER_SIZE 49152
    extern    mem_heap_type   tmc_heap_small;
    extern uint8 tmc_heap_small_mem_buffer[TMC_HEAP_SMALL_MEM_BUFFER_SIZE];
  #endif /* FEATURE_LOW_MEMORY_USAGE */

  typedef enum
  {
    TMC_MEM_HEAP_TYPE_THRD,
    TMC_MEM_HEAP_TYPE_BUF
    #ifndef FEATURE_LOW_MEMORY_USAGE
      ,TMC_MEM_HEAP_TYPE_SMALL_BUF
    #endif
  }tmc_mem_heap_e_type;

#endif


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION tmc_task

DESCRIPTION
  This function is entrance of the Task Main Controller. It is responsible 
  for none protocol specific power up initialization and task related 
  operation, such as define, start, offline, power down, stop, and LPM.
  
DEPENDENCIES
  None
    
RETURN VALUE
  None.
  
SIDE EFFECTS
  Reset and powerdown processing are special in that they cause the
  complete DMSS software to either restart or to be shutdown.


===========================================================================*/
extern  void                       tmc_task(
        dword                      param   /* Entry paramater, not used. */
);

/*===========================================================================

FUNCTION tmc_task_start

DESCRIPTION
  This function is called by each task when it is created.  The function
  then performs the startup processing for the calling task.  The function
  first sets the acknowledge signal back to TMC to acknowledge that is had
  been created.  It then waits for the start signal back to it.  While
  waiting for the start signal and keeps kicking the wtachdog timer
  everytime its interval timer has expired.  When it gets the start
  signal it acknowledges it back to TMC and it then returns to the caller.
  The caller task is now formally started and it can start normal application
  processing.


DEPENDENCIES
  TMC task has to be started.

RETURN VALUE
  void

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/
extern  void                       tmc_task_start ( void );

/*===========================================================================

FUNCTION tmc_task_ack

DESCRIPTION
  This function sends acknowledge signal to Task Main Control task by caller

DEPENDENCIES
  Task Main Control task is running.
  
RETURN VALUE
  void
  
SIDE EFFECTS
  none

===========================================================================*/
extern  void                       tmc_task_ack( void );

/*===========================================================================

FUNCTION tmc_task_stop

DESCRIPTION
  This function is called by each task when it stops.  The function then 
  performs the stop processing for the calling task.  The function
  sets the acknowledge signal back to TMC to acknowledge that is had
  been stopped.  

DEPENDENCIES
  Task Main Control task is running.
  
RETURN VALUE
  void
  
SIDE EFFECTS
  none

===========================================================================*/
extern  void                       tmc_task_stop ( void );

/*===========================================================================

FUNCTION tmc_task_offline

DESCRIPTION
  This function is called by each task when it goes offline.  The function 
  then performs the offline processing for the calling task.  The function
  sets the acknowledge signal back to TMC to acknowledge that is had
  been offline.  

DEPENDENCIES
  Task Main Control task is running.
  
RETURN VALUE
  void
  
SIDE EFFECTS
  none

===========================================================================*/
extern  void                       tmc_task_offline ( void );

/*===========================================================================

FUNCTION tmc_cmd

DESCRIPTION
  This function is called by other tasks to queue a command and send a
  command signal to tmc task.Then function returns to the caller. This does 
  not mean that the command has been executed yet, just that it is waiting to 
  be executed.

DEPENDENCIES
  Free command queue and command queue have be initialized with tmc_task_init.
  
RETURN VALUE
  boolean  If command queue is full, FALSE is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                    tmc_cmd(
        tmc_cmd_e_type             cmd
);

/*===========================================================================

FUNCTION  TMC_GET_STATE

DESCRIPTION

  This function returns the current TMC state

DEPENDENCIES

  None.

RETURN VALUE

  Current TMC state.

SIDE EFFECTS

  None.

===========================================================================*/
extern  tmc_state_e_type           tmc_get_state( void );

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */
                             
#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION tmc_get_stored_esn_me

DESCRIPTION
  This function returns the stored esn_me .
  

DEPENDENCIES
  None

RETURN
  dword
  
SIDE EFFECTS
  None

===========================================================================*/
extern  dword                      tmc_get_stored_esn_me (void);
#endif /* FEATURE_UIM_RUIM */


/*===========================================================================

FUNCTION tmc_is_init_done

DESCRIPTION
  This function returns if the tmc initialization is done.
  

DEPENDENCIES
  None

RETURN
  TRUE if the tmc initialization is done.Otherwise return FALSE.
  
SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                    tmc_is_init_done (void);


#ifdef FEATURE_REX_DYNA_MEM
/*===========================================================================

FUNCTION tmc_get_mem_heap_size

DESCRIPTION
  This function returns the size of requested memory heap.
  

DEPENDENCIES
  None

RETURN
  dword
  
SIDE EFFECTS
  None

===========================================================================*/
extern  dword                      tmc_get_mem_heap_size (
        tmc_mem_heap_e_type        p_type);
#endif /* FEATURE_REX_DYNA_MEM */


#ifdef FEATURE_RESET_COUNTER
/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_read

DESCRIPTION
  Read reset counter from flash memeory.
  
DEPENDENCIES
  TMC,FS task has already been started.

RETURN VALUE
  TRUE read operation succeed.
  FALSE read operation fail.  

SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                    tmc_reset_counter_read(
        int                        *cntr_ptr
                                      /* Pointer to returned reset counter */
);


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_reset

DESCRIPTION
  Reset reset counter in flash memeory.
  
DEPENDENCIES
  TMC,FS task has already been started.

RETURN VALUE
  TRUE reset operation succeed.
  FALSE reset operation fail.  

SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                    tmc_reset_counter_reset(void);
#endif /* FEATURE_RESET_COUNTER */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_malloc

DESCRIPTION
  This function allocate the memmory from tmc heap.
  If the number of bytes to allocate is less than the memory threshold, use
  tmc_heap_small_size otherwise use the tmc_heap_size.

DEPENDENCIES
  None

RETURN VALUE
  the pointer to the tmc heap.

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_MEM_DEBUG
#error code not present
#else
extern  void                       *tmc_malloc(       
        size_t                     size
                                   /* Number of bytes to allocate
                                   */
);
#endif

#ifdef FEATURE_MEM_DEBUG
#error code not present
#endif



/* <EJECT> */
/*===========================================================================

FUNCTION tmc_free

DESCRIPTION
  This function free the memmory allocated tmc heap.
  If the memory is in the range of tmc heap or tmc small heap, free per 
  request.
  Otherwise, return the error message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_MEM_DEBUG
#error code not present
#else
extern  void                       tmc_free(       
        void                       *ptr
                                   /* pointer to the memory to free.
                                   */
);
#endif

#ifdef FEATURE_MEM_DEBUG
#error code not present
#endif



/* <EJECT> */
/*===========================================================================

FUNCTION tmc_realloc

DESCRIPTION
  This function allocate the memmory from tmc heap.
  If the memory is in the range of tmc heap or tmc small heap, reallocate the
  memory per request.
  Otherwise, return the error message.

DEPENDENCIES
  None

RETURN VALUE
  the pointer to the tmc heap.

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_MEM_DEBUG
#error code not present
#else
extern  void                       *tmc_realloc(       
        void                       *ptr, 
                                   /* pointer to the memory for realloc.
                                   */
        size_t                     size
                                   /* Number of bytes to allocate
                                   */
);
#endif

#ifdef FEATURE_MEM_DEBUG
#error code not present
#endif



#endif /* TMC_H */

