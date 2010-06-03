/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   D I A G N O S T I C     T A S K
           R E X    O P E R A T I O N    P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains routines that process Rex-related packets.
  Each procedure handles a specific packet.

  Each procedure has the same job:

    1. Check packet for correctness.

    2. Perform processing required by the packet.

    3. Format a response packet.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rexdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/06    ck     Feturized portion of rex profiling code to fix compilation
                   errors when rex profiling was not turned ON
01/19/06    gr     Added inclusion of msg.h.
03/29/05    ck     Fixed an impossible if condition in the functin
                   rex_diag_taskprofile_handler.
08/11/04    is     Added REX profiling dump support (action=3).
11/20/02   jct     Integrated on target.  Added core dump test functionality
07/21/02    gr     Created.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#ifdef CUST_H
#include "customer.h"
#endif
#include "rex.h"
#include "rexdiag.h"
#include "diagpkt.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "msg.h"

extern void rex_enable_task_profile(
   uint32 sys_period,
   uint32 task_period,
   uint32 tasks_per_period
);

extern void rex_disable_task_profile(void);
extern void rextp_log_per_task_profile(void);

/*===========================================================================
FUNCTION REX_CODECOVERAGE_ENABLE

DESCRIPTION
============================================================================*/
void rex_codecoverage_enable(void) {}

/*===========================================================================
FUNCTION REX_CODECOVERAGE_DISABLE

DESCRIPTION
============================================================================*/
void rex_codecoverage_disable(void) {}

/*===========================================================================
FUNCTION REX_DIAG_PCPROFILE_HANDLER

DESCRIPTION
  This function handles the request to start or stop program counter
  profiling.

  action = 1 -> enable PC profiling
  action = 2 -> disable PC profiling
  action = 3 -> clear PC profiling information
============================================================================*/
PACKED void * rex_diag_pcprofile_handler(
   PACKED void   *req_ptr,
   uint16         pkt_len
)
{
#ifdef FEATURE_REX_PC_DUMP
#error code not present
#else
   return NULL;
#endif /* FEATURE_REX_PC_DUMP */

} /* diag_pcprofile_handler */


/*===========================================================================

FUNCTION REX_DIAG_TASKPROFILE_HANDLER

DESCRIPTION
  This function handles the request to start or stop program counter
  profiling.

  action = 1 -> enable task profiling
  action = 2 -> disable task profiling

============================================================================*/
PACKED void * rex_diag_taskprofile_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_REX_TASKPROFILE_F_req_type *taskprofile_req_ptr;
   DIAG_SUBSYS_REX_TASKPROFILE_F_rsp_type *taskprofile_rsp_ptr;

   taskprofile_req_ptr = (DIAG_SUBSYS_REX_TASKPROFILE_F_req_type *) req_ptr;

   /* Verify that the length passed in and the action specified are
   ** valid.
   */
   /*
   if ( pkt_len != sizeof(DIAG_SUBSYS_REX_TASKPROFILE_F_req_type) )
   {
       return diagpkt_err_rsp(
                 DIAG_BAD_LEN_F,
                 taskprofile_req_ptr,
                 sizeof(DIAG_SUBSYS_REX_TASKPROFILE_F_req_type)
              );
   }
*/
   if( ( taskprofile_req_ptr->action == 0 ) ||
       ( taskprofile_req_ptr->action > 3 ) )
   {
       return diagpkt_err_rsp(
                 DIAG_BAD_PARM_F,
                 taskprofile_req_ptr,
                 sizeof(DIAG_SUBSYS_REX_TASKPROFILE_F_req_type)
              );
   }

   taskprofile_rsp_ptr = (DIAG_SUBSYS_REX_TASKPROFILE_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_REX_TASKPROFILE_F_rsp_type)
                         );

   if ( taskprofile_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   (void)memcpy(
      (void*)taskprofile_rsp_ptr,
      (void*)taskprofile_req_ptr,
      sizeof(diagpkt_subsys_header_type)
   );

   switch( taskprofile_req_ptr->action )
   {
#ifdef FEATURE_REX_PROFILE
      case 1:
         MSG_HIGH( "PROF - ENABLE",0,0,0 );

         rex_enable_task_profile(
            taskprofile_req_ptr->sys_period,
            taskprofile_req_ptr->task_period,
            taskprofile_req_ptr->tasks_per_period
         );
         taskprofile_rsp_ptr->status = 1;
         break;
#endif

#ifdef FEATURE_REX_PROFILE
      case 2:
         MSG_HIGH( "PROF - DISABLE",0,0,0 );
         rex_disable_task_profile();
         taskprofile_rsp_ptr->status = 1;
         break;
#endif

#ifdef FEATURE_REX_PROFILE
      case 3:
         rextp_log_per_task_profile();
         break;
#endif

      default:
         taskprofile_rsp_ptr->status = 0;
         break;
   }

  return (void *) taskprofile_rsp_ptr;

} /* rex_diag_taskprofile_handler */


/*===========================================================================
FUNCTION REX_DIAG_CODECOVERAGE_HANDLER

DESCRIPTION
  This function handles the request to start or stop collection of code
  coverage information.
============================================================================*/
PACKED void * rex_diag_codecoverage_handler(
   PACKED void *req_ptr,
   word         pkt_len
)
{
   DIAG_SUBSYS_REX_CODECOVERAGE_F_req_type *codecoverage_req_ptr;
   DIAG_SUBSYS_REX_CODECOVERAGE_F_rsp_type *codecoverage_rsp_ptr;

   codecoverage_req_ptr = (DIAG_SUBSYS_REX_CODECOVERAGE_F_req_type *) req_ptr;

   /* Verify that the length passed in and the action specified are
   ** valid.
   */
   if ( pkt_len != sizeof(DIAG_SUBSYS_REX_CODECOVERAGE_F_req_type) )
   {
       return diagpkt_err_rsp(
                 DIAG_BAD_LEN_F,
                 codecoverage_req_ptr,
                 sizeof(DIAG_SUBSYS_REX_CODECOVERAGE_F_req_type)
              );
   }

   if( ( codecoverage_req_ptr->action != 1 ) &&
       ( codecoverage_req_ptr->action != 2 ) )
   {
       return diagpkt_err_rsp(
                 DIAG_BAD_PARM_F,
                 codecoverage_req_ptr,
                 sizeof(DIAG_SUBSYS_REX_CODECOVERAGE_F_req_type)
              );
   }

   codecoverage_rsp_ptr = (DIAG_SUBSYS_REX_CODECOVERAGE_F_rsp_type *)
                          diagpkt_alloc(
                             diagpkt_get_cmd_code( req_ptr ),
                             sizeof(DIAG_SUBSYS_REX_CODECOVERAGE_F_rsp_type)
                          );

   if ( codecoverage_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   switch( codecoverage_req_ptr->action )
   {
      case 1:
         rex_codecoverage_enable( );
         codecoverage_rsp_ptr->status = 1;
         break;

      case 2:
         rex_codecoverage_disable( );
         codecoverage_rsp_ptr->status = 1;
         break;

      default:
         /* This should never happen.
         */
         codecoverage_rsp_ptr->status = 0;
         break;
   }

  return (void *) codecoverage_rsp_ptr;

} /* END rex_diag_codecoverage_handler */

#ifdef FEATURE_REX_CORE_DUMP
   extern void rex_jettison_core( void );
   extern void rex_log_core_dump(
      uint16      line,
      const char *filename,
      const char *fmt,
      uint32      p1,
      uint32      p2,
      uint32      p3
   );
#endif

/*===========================================================================
FUNCTION REX_DIAG_COREDUMP_HANDLER

DESCRIPTION
   Forces a core dump
============================================================================*/
PACKED void * rex_diag_coredump_handler(
   PACKED void *in_req_ptr,
   word         pkt_len
)
{
   DIAG_SUBSYS_REX_COREDUMP_F_req_type *req_ptr;
   DIAG_SUBSYS_REX_COREDUMP_F_rsp_type *rsp_ptr;

   req_ptr = (DIAG_SUBSYS_REX_COREDUMP_F_req_type *) in_req_ptr;

   /* Verify that the length passed in and the action specified are
   ** valid.
   */
   if ( pkt_len != sizeof(DIAG_SUBSYS_REX_COREDUMP_F_req_type) )
   {
       return diagpkt_err_rsp(
                 DIAG_BAD_LEN_F,
                 req_ptr,
                 sizeof(DIAG_SUBSYS_REX_COREDUMP_F_req_type)
              );
   }

   rsp_ptr = (DIAG_SUBSYS_REX_COREDUMP_F_rsp_type *)
                          diagpkt_alloc(
                             diagpkt_get_cmd_code( req_ptr ),
                             sizeof(DIAG_SUBSYS_REX_COREDUMP_F_rsp_type)
                          );

   if ( rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   (void)memcpy(
      (void*)rsp_ptr,
      (void*)req_ptr,
      sizeof(diagpkt_subsys_header_type)
   );

#ifdef FEATURE_REX_CORE_DUMP
   switch( req_ptr->action )
   {
      case 1:
         rex_jettison_core();
         break;

      case 2:
         rex_log_core_dump( __LINE__, __FILE__, "CORE_DUMP: %d %d %d", 1, 2, 3 );
         break;

      case 3:
         ERR_FATAL( "REX CORE DUMP TEST",0,0,0 );
         break;
   }
#endif

   return (void *) rsp_ptr;
} /* END rex_diag_coredump_handler */

/* =========================================================================
** REX SUBSYS DIAG TABLE
** ========================================================================= */
static const diagpkt_user_table_entry_type rex_diag_tbl[] =
{
  {REX_DIAG_PCPROFILE_F,    REX_DIAG_PCPROFILE_F,    rex_diag_pcprofile_handler },
  {REX_DIAG_TASKPROFILE_F,  REX_DIAG_TASKPROFILE_F,  rex_diag_taskprofile_handler },
  {REX_DIAG_CODECOVERAGE_F, REX_DIAG_CODECOVERAGE_F, rex_diag_codecoverage_handler },
  {REX_DIAG_COREDUMP_F,     REX_DIAG_COREDUMP_F,     rex_diag_coredump_handler }
};

#if defined FEATURE_DIAG_MANUAL_DISPATCH_TABLE
extern void rexdiag_init (void);

/*===========================================================================
FUNCTION REXDIAG_INIT

DESCRIPTION
============================================================================*/
void rexdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER ((word)DIAG_SUBSYS_REX, rex_diag_tbl);
}
#else
DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_REX, rex_diag_tbl);
#endif
