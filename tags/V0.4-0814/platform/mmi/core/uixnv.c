/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   N V   M O D U L E

GENERAL DESCRIPTION

  This source file provides the NV interface functions for all UI programs.

EXTERNALIZED FUNCTIONS
  ui_get_nv
    Get an item from NV, handle non-active items

  ui_put_nv
    Write an item to NV

  ui_replace_nv
    Replaces an item in NV

  ui_free_nv
    Free an item from NV

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999-2004 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/uixnv.c#16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/09   kcp     Fixed CR 185690
11/25/08   kcp     Fixed CR 163973
11/09/04   cvs     Fix compiler warnings
05/13/04   AT      Made changes for Thin UI
02/09/04   BPW     Brew 3.0 Changes
01/21/04   BPW     Fixed Lint errors
01/21/04   BPW     Changed app context before handle a key
12/09/03   ram     Removed lint errors.
09/30/03   tml     Fixed compilation warnings
09/26/03   tml     Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <string.h>

#include "ui.h"
#include "uixsnd.h"
#include "ui_base.h"

#include "err.h"
#include "nv.h"
#include "task.h"
#include "cm.h"

#ifndef FEATURE_UI_CORE_REMOVED
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "OEMFeatures.h"
#endif

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================
FUNCTION UI_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type ui_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
  static nv_cmd_type nvi;           /* nv command buffer */

  nvi.tcb_ptr = &ui_tcb;          /* notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  nvi.item = item;                 /* item to read */
  nvi.cmd = NV_READ_F;

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address -= 1;   /* update to real nv address */
  }

  /* Set up NV so that it will read the data into the correct location */
  nvi.data_ptr = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( &ui_tcb, UI_NV_SIG );
  nv_cmd( &nvi );
#if defined(T_QSC1100) || defined(T_QSC60X5)
  //fix for CR 163973
  ui_wait_for_nv(UI_NV_SIG);
#else
  ui_wait(UI_NV_SIG);
#endif

  if (nvi.status == NV_NOTACTIVE_S) {
     MSG_HIGH( "Nonactive NV item 0x%x", nvi.item, 0, 0 );
  } else {
    if( nvi.status != NV_DONE_S ) {
      ERR( "NV Read Failed Item %d Code %d", nvi.item, nvi.status, 0 );
    }
  }

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address += 1;   /* restore to user address */
  }

  return( nvi.status );

}


/* <EJECT> */
/*===========================================================================
FUNCTION UI_GET_NV_FOR_CM

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

  This function is defined for the purpose of supporting two UI callback
  functions: get_call_type, call_is_allowed that are used by the CM to
  determine the call type and whether the call is allowed. This function
  must be completed in a specified time as defined by the CM API.

  This is an example for using the call back functions.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type ui_get_nv_for_cm(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
  static nv_cmd_type cmnvi;

  cmnvi.tcb_ptr = &cm_tcb;          /* notify this task when done */
  cmnvi.sigs = CM_UI_NV_SIG;
  cmnvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  cmnvi.item = item;                 /* item to read */
  cmnvi.cmd = NV_READ_F;

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address -= 1;   /* update to real nv address */
  }

  /* Set up NV so that it will read the data into the correct location */
  cmnvi.data_ptr = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( &cm_tcb, CM_UI_NV_SIG );
  nv_cmd( &cmnvi );

  (void) rex_wait( CM_UI_NV_SIG );

  #ifdef FEATURE_TREX_MM_UI
  /* Clear the signal as rex_wait() does not do it.
     In TREX implementation of nv_cmd(), the message is removed
     from the queue and therefore the signal should be cleared here.
     If the signal is not cleared, the NV task will be signaled
     and since the nv queue might be empty, everything will abort
     in cmnv_q_write_done_cb() on CM_ASSERT( cmnv_q_cnt() > 0 ); */
  rex_clr_sigs( &cm_tcb, CM_UI_NV_SIG );
  #endif /* FEATURE_TREX_MM_UI */

  if (cmnvi.status == NV_NOTACTIVE_S) {
     MSG_HIGH( "Nonactive NV item 0x%x", cmnvi.item, 0, 0 );
  } else {
    if( cmnvi.status != NV_DONE_S ) {
      ERR( "NV Read Failed Item %d Code %d", cmnvi.item, cmnvi.status, 0 );
    }
  }

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address += 1;   /* restore to user address */
  }

  return( cmnvi.status );

}

/* <EJECT> */
/*===========================================================================
FUNCTION UI_PUT_NV

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type ui_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{
  static nv_cmd_type nvi;           /* nv command buffer */

  nvi.tcb_ptr = &ui_tcb;        /* Notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to write */
  nvi.cmd = NV_WRITE_F;

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address -= 1;   /* update to real nv address */
  }

  nvi.data_ptr =  data_ptr;        /* the data to write */


  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( &ui_tcb, UI_NV_SIG );
  nv_cmd( &nvi );
#if defined(T_QSC1100) || defined(T_QSC60X5)
  //fix for CR 163973
  ui_wait_for_nv(UI_NV_SIG);
#else
  ui_wait(UI_NV_SIG);
#endif


  if( nvi.status != NV_DONE_S ) {
    ERR( "NV Write Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address += 1;   /* restore to user address */
  }

  return( nvi.status );

}

/*===========================================================================
FUNCTION UI_REPLACE_NV

DESCRIPTION
  Replace an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type ui_replace_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{
  static nv_cmd_type nvi;           /* nv command buffer */

  nvi.tcb_ptr = &ui_tcb;        /* Notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to write */
  nvi.cmd = NV_REPLACE_F;

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address -= 1;   /* update to real nv address */
  }

  nvi.data_ptr =  data_ptr;        /* the data to write */


  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( &ui_tcb, UI_NV_SIG );
  nv_cmd( &nvi );
#if defined(T_QSC1100) || defined(T_QSC60X5)
  //fix for CR 163973
  ui_wait_for_nv(UI_NV_SIG);
#else
  ui_wait(UI_NV_SIG);
#endif

  if( nvi.status != NV_DONE_S ) {
    ERR( "NV Replace Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address += 1;   /* restore to user address */
  }

  return( nvi.status );
}

/* <EJECT> */
/*===========================================================================
FUNCTION UI_FREE_NV

DESCRIPTION
  Free an item in NV memory.  Wait till free is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type ui_free_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{
  static nv_cmd_type nvi;           /* nv command buffer */

  nvi.tcb_ptr = &ui_tcb;        /* Notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to free */
  nvi.cmd = NV_FREE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */

  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( &ui_tcb, UI_NV_SIG );
  nv_cmd( &nvi );
#if defined(T_QSC1100) || defined(T_QSC60X5)
  //fix for CR 163973
  ui_wait_for_nv(UI_NV_SIG);
#else
  ui_wait(UI_NV_SIG);
#endif


  if( nvi.status != NV_DONE_S ) {
    ERR( "NV Free Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }

  return( nvi.status );

}
