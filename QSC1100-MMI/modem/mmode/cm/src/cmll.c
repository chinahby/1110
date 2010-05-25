/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   L O W E R   L A Y E R

                        I N T E R F A C E   M O D U L E

GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmll.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/07   ic      Lint clean up
03/09/06   pk      Added support for Generic reports
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/18/05   sk      Added CM API cm_sd_rpt_free_buf() to free CM SD reports.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/03/04   ic      CM Lint clean up - cmll.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
02/06/04   ws      Initial jaguar/mainline merge.
04/28/03   AT      Lint cleanup.
01/21/03   vt      Fixed the Header line to display proper file path
10/09/02   vt      Initial version
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"                       /* Customer configuration file */
#include "comdef.h"               /* Definition for basic types and macros */
#include "cmdbg.h"
#include "cmtask.h"
#include "cmll.h"


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/****************************************************************************
              FUNCTIONS CALLED BY SD TO COMMUNICATE WITH CM
                    FOR INTERNAL AS WELL AS SD'S USE
****************************************************************************/


/*===========================================================================

FUNCTION cm_sd_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the cm_mc_rpt_free_q. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_sd_rpt_u_type *cm_sd_rpt_get_buf_else_err_fatal( void )
{
  cm_sd_rpt_u_type *sd_rpt_ptr;
    /* CM SD report pointer */

  /*-----------------------------------------------------------------------*/

  /* Try allocating a CM report command buffer
  */
  sd_rpt_ptr = (cm_sd_rpt_u_type *) cm_mem_malloc(sizeof(cm_sd_rpt_u_type));

  /* If allocation failed, error fatal.
  */
  if( sd_rpt_ptr == NULL )
  {
    CM_ERR_FATAL( "Unable to allocate CM SD Rpt",0,0,0 );
  }

  sd_rpt_ptr->hdr.task_ptr = NULL;

  return sd_rpt_ptr;

} /* cm_sd_rpt_get_buf_else_err_fatal() */



/*===========================================================================

FUNCTION CM_SD_RPT

DESCRIPTION
  Allows MC to queue up messages to the CM.  Grab a buffer from the
  cm_mc_rpt_free_q, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_sd_rpt
(
  cm_sd_rpt_u_type *cmd_ptr               /* Pointer to a CM report buffer */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( cmd_ptr, &cmd_ptr->hdr.link );                /* init link */
  q_put( &cm_rpt_q, &cmd_ptr->hdr.link );                   /* and queue it */
  (void) rex_set_sigs( &cm_tcb, CM_RPT_Q_SIG );       /* signal the CM task */

} /* cm_sd_rpt() */



/*===========================================================================

FUNCTION cm_generic_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the heap. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_generic_rpt_s_type *cm_generic_rpt_get_buf_else_err_fatal( void )
{
  cm_generic_rpt_s_type *generic_rpt_ptr;
    /* CM Generic report pointer */

  /*-----------------------------------------------------------------------*/

  /* Try allocating a CM report command buffer
  */
  generic_rpt_ptr = (cm_generic_rpt_s_type *) cm_mem_malloc(sizeof(cm_generic_rpt_s_type));

  /* If allocation failed, error fatal.
  */
  if( generic_rpt_ptr == NULL )
  {
    CM_ERR_FATAL( "Unable to allocate CM GEN Rpt",0,0,0 );
  }

  generic_rpt_ptr->hdr.task_ptr = NULL;

  return generic_rpt_ptr;

} /* cm_generic_rpt_get_buf_else_err_fatal */



/*===========================================================================

FUNCTION CM_GENERIC_RPT

DESCRIPTION
  Allows generic clients to queue up messages to the CM. Dynamically allocate
  a buffer, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_generic_rpt
(
  cm_generic_rpt_s_type *cmd_ptr               /* Pointer to a CM report buffer */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( cmd_ptr, &cmd_ptr->hdr.link );                /* init link */
  q_put( &cm_rpt_q, &cmd_ptr->hdr.link );                   /* and queue it */
  (void) rex_set_sigs( &cm_tcb, CM_RPT_Q_SIG );       /* signal the CM task */

} /* cm_generic_rpt() */

