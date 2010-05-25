/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S . C

GENERAL DESCRIPTION
  Collection of utility functions being used by various modules in PS.  
  Most of these functions assume that the caller is in PS task context.

  Copyright (c) 1995-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.c_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_utils.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/05/02    usb    Moved lcsum() from this file to ps_iputil.c
08/05/02    usb    Fixed get/set nv functions, moved them out of FEATURE_MIP
07/31/02    usb    Renamed the file from psmisc.c
06/14/02    usb    Removed byte manipulation functions.  Use dsbyte.h now.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS
12/21/01    dwp    Wrap get16 in !FEATURE_DATA_PS as it is defined else where
                   in MSM5200 archive.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
05/24/00    hcg    Added TARGET_OS_SOLARIS to compile with Solaris DSPE.
04/21/00    mvl    Fixed a #define so compiles properly under COMET
01/09/99    jjw    Changed to generic Browser interface
10/27/98    ldg    For T_ARM included C version of TCP checksum routine.
06/16/98    na     Converted the routine that calculates the TCP checksum 
                   into 186 assembly.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
07/22/95    jjw    Created Initial Version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#include "msg.h"
#include "err.h"
#include "assert.h"

#include "ps_utils.h"
#include "psi.h"
#include "qw.h"
#include "clk.h"
//#include "ps_ip.h"

#if(TG==T_PC)
#error code not present
#endif

/*===========================================================================

                      LOCAL DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Command item to NV.
---------------------------------------------------------------------------*/
LOCAL nv_cmd_type  ps_nv_cmd_buf;

/*===========================================================================

                      DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================

FUNCTION PS_START_TIMER

DESCRIPTION
  This function will set the specified timer to the specified duration. The 
  time units are in milliseconds.
  
DEPENDENCIES
  The timer should already be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ps_start_timer
(
  rex_timer_type  *timer_ptr,            /* ptr to timer to be started     */
  dword duration                         /* time interval, in msecs        */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( timer_ptr != NULL );
  
  /*------------------------------------------------------------------------
    Set the specified timer with the passed duration (in milliseconds)
  -------------------------------------------------------------------------*/
  rex_set_timer( timer_ptr, duration );

} /* ps_start_timer() */


/*===========================================================================

FUNCTION PS_CANCEL_TIMER

DESCRIPTION
  This function will cancel the specified timer and the associated signal 
  mask will also be cleared. NOTE that the signals are associated with 
  timers during initialization by the REX_DEF_TIMER function.

DEPENDENCIES
  The timer should already be initialized. The signal associated with the 
  timer should be a PS task signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ps_cancel_timer
(
  rex_timer_type  *timer_ptr,            /* ptr to timer to be cancelled   */
  ps_sig_enum_type sig                   /* signal associated with a timer */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( timer_ptr != NULL );
  
  /*------------------------------------------------------------------------
    Cancel the specified timer and clear the associated signal
  -------------------------------------------------------------------------*/

  MSG_MED( "Cancelling timer 0x%x", timer_ptr, 0, 0 );

  rex_clr_timer( timer_ptr );
  PS_CLR_SIGNAL( sig );

} /* ps_cancel_timer() */


/*===========================================================================

FUNCTION PS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.  

===========================================================================*/

nv_stat_enum_type ps_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT( rex_self() == &ps_tcb );

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  ps_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
  ps_nv_cmd_buf.tcb_ptr    = &ps_tcb;               /* Notify this task    */
  ps_nv_cmd_buf.sigs       = 1 << PS_NV_CMD_SIGNAL; /* With this signal    */
  ps_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  ps_nv_cmd_buf.item       = item_code;             /* Item to get         */
  ps_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  PS_CLR_SIGNAL( PS_NV_CMD_SIGNAL );               /* Clear signal for NV  */
  nv_cmd( &ps_nv_cmd_buf );                        /* Issue the request    */
  psi_wait( (rex_sigs_type) 1 << PS_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( ps_nv_cmd_buf.status != NV_DONE_S &&
      ps_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    ERR( "NV Read Failed Item %d Code %d",
         ps_nv_cmd_buf.item, ps_nv_cmd_buf.status, 0 );
  }
  return( ps_nv_cmd_buf.status );
} /* ps_get_nv_item() */


/*===========================================================================
FUNCTION PS_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done

DEPENDENCIES
  This function can only be called from PS task.  Also it is not 
  reentrant. Shouldn't be a problem, as it doesn't exit till we're done, and 
  it's only called from the PS task.
  
RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.  
  
===========================================================================*/
nv_stat_enum_type ps_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( rex_self() == &ps_tcb );

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
  -------------------------------------------------------------------------*/
  ps_nv_cmd_buf.cmd        = NV_WRITE_F;            /* Write request       */
  ps_nv_cmd_buf.tcb_ptr    = &ps_tcb;               /* Notify this task    */
  ps_nv_cmd_buf.sigs       = 1 << PS_NV_CMD_SIGNAL; /* With this signal    */
  ps_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  ps_nv_cmd_buf.item       = item_code;             /* Item to put         */
  ps_nv_cmd_buf.data_ptr   = data_ptr;              /* Data to write       */


  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  PS_CLR_SIGNAL( PS_NV_CMD_SIGNAL );
  nv_cmd( &ps_nv_cmd_buf );
  psi_wait( (rex_sigs_type) 1 << PS_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( ps_nv_cmd_buf.status != NV_DONE_S ) 
  {
    ERR( "NV Write Failed Item %d Code %d",
         ps_nv_cmd_buf.item, ps_nv_cmd_buf.status, 0 );
  }

  return( ps_nv_cmd_buf.status );
} /* ps_put_nv_item() */


/*===========================================================================

FUNCTION msclock

DESCRIPTION
  This function will return the time in milliseconds since ....

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#if(TG==T_PC)
#error code not present
#else

dword msclock( void)
{
  qword qw_time;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  clk_read_ms( qw_time);

  return( qw_time[0]);

} /* msclock() */

#endif

#endif /* FEATURE_DS || FEATURE_DATA_PS */
