#ifndef REXDIAG_H
#define REXDIAG_H
/*===========================================================================

DESCRIPTION
  Definitions of DIAG subsystem packets for configuring task profile,
  memory debug, PC profile, and code coverage 
 
Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/core/kernel/rex/main/latest/rexdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/02   jct     Integrated onto target
07/20/02    gr     Created

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "diagpkt.h"
#include "memheap.h"

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */

/* REX Subsystem Command IDs
*/
#define REX_DIAG_PCPROFILE_F     0x0001
#define REX_DIAG_TASKPROFILE_F   0x0002
#define REX_DIAG_CODECOVERAGE_F  0x0003
#define REX_DIAG_COREDUMP_F      0x0004

#define APPS_DIAG_TASKPROFILE_F  0x0100

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*============================================================================

PACKET   Program Counter Profiling

ID       DIAG_SUBSYS_REX_PCPROFILE_F

PURPOSE  Sent from the DM to the DMSS requesting the DMSS to either start or
         stop program counter profiling.

         action: A value of 1 indicates that collection of program counter
                 profiling information should be started. A value of 2
                 indicates that collection should be stopped. Starting
                 collection will reset previously collected information.

RESPONSE The DMSS sends back a packet containing the result of the request
         (success or failure, with 1 = success and 0 = failure)

============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( REX, PCPROFILE_F )
   uint32 action;
DIAGPKT_SUBSYS_REQ_END


DIAGPKT_SUBSYS_RSP_DEFINE( REX, PCPROFILE_F )
   uint32 status;
DIAGPKT_SUBSYS_RSP_END


/*============================================================================

PACKET   Task Profiling

ID       DIAG_SUBSYS_REX_TASKPROFILE_F

PURPOSE  Sent from the DM to the DMSS requesting the DMSS to either start or
         stop task profiling.

         action: A value of 1 indicates that collection of task profiling
                 information should be started. A value of 2 indicates
                 that collection should be stopped. Starting collection
                 will reset previously collected information.

         sys_period: The periodicity for reporting system profiling
                 information (i.e sending out the LOG_SYSPROFILE_C log packet)
                 in milliseconds. For instance, if this field has a value
                 of 500, system profiling information will be sent out every
                 500 milliseconds.

         task_period: The periodicity for reporting task profiling information
                 (i.e sending out the LOG_SYSPROFILE_C log packet) in
                 milliseconds. For instance, if this field has a value of 500,
                 task profiling information will be sent out every 500
                 milliseconds.

         tasks_per_period: The number of tasks for which information should be
                 sent out using the LOG_TASKPROFILE_C packet. If this is set
                 to 5, for example, profiling information for the first five
                 tasks will be sent out after one task_period. After the
                 next task_period, information for the next five tasks will
                 be sent out, and so on. If num_tasks is greater than the
                 number of tasks in the system, information for all tasks is
                 sent out after each task_period.
                
         task_period should typically be less than sys_period, because the
         LOG_TASKPROFILE_C log packet contains information that changes more
         frequently than the information in the LOG_SYSPROFILE_C packet.

RESPONSE The DMSS sends back a packet containing the result of the request
         (success or failure, with 1 = success and 0 = failure)

============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( REX, TASKPROFILE_F )
   uint32 action;
   uint32 sys_period;
   uint32 task_period;
   uint32 tasks_per_period;
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( REX, TASKPROFILE_F )
   uint32 status;
DIAGPKT_SUBSYS_RSP_END


/*============================================================================

PACKET   Code Coverage

ID       DIAG_SUBSYS_REX_CODECOVERAGE_F

PURPOSE  Sent from the DM to the DMSS requesting the DMSS to start/stop
         generartion of code coverage information.

         action: A value of 1 indicates that collection of code coverage
                 information should be started. A value of 2 indicates
                 that collection should be stopped. Starting collection
                 will reset previously collected information.

RESPONSE The DMSS sends back a packet containing the result of the request
         (success or failure, with 1 = success and 0 = failure)

============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( REX, CODECOVERAGE_F )
   uint32 action;
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( REX, CODECOVERAGE_F )
   uint32 status;
DIAGPKT_SUBSYS_RSP_END

/*============================================================================

PACKET   Core Dump

ID       DIAG_SUBSYS_REX_COREDUMP_F

PURPOSE  Sent from the DM to the DMSS requesting the REX to capture
         the register state of the ARM

         action: 1 - request to jettison the core
                 2 - request to send core dump log record

RESPONSE The DMSS echoes the request

============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( REX, COREDUMP_F )
   uint32 action;
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( REX, COREDUMP_F )
DIAGPKT_SUBSYS_RSP_END

#endif /* REXDIAG_H */
