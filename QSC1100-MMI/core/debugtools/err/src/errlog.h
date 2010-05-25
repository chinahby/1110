#ifndef ERRLOG_H
#define ERRLOG_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  E R R

DESCRIPTION
  This header file contains the definitions of log structure for core dump
 
Copyright (c) 2007      by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/err/errlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/07   tbg     Initial revision (pulled code from rexlog.h)

===========================================================================*/


/* -----------------------------------------------------------------------
** Includes 
** ----------------------------------------------------------------------- */
#include "customer.h"
#include "comdef.h"
#include "log.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */


/*============================================================================

PACKET   Core Dump Information

ID       LOG_COREDUMP_C

DESCRIPTION
  Contains core dump information.
  
  regs: contents of the ARM registers, ordered as follows
        regs[0]  to regs[16] -> supervisor mode registers, spsr
        regs[17] to regs[19] -> r13, r14, spsr in system/user mode
        regs[20] to regs[22] -> r13, r14, spsr in irq mode
        regs[23] to regs[25] -> r13, r14, spsr in abort mode
        regs[26] to regs[28] -> r13, r14, spsr in undef mode
        regs[29] to regs[36] -> r8 to r14, spsr in fiq mode
        regs[37] to regs[39] -> r13, r14, spsr in java mode mode

  cpsr: the current program status register.

  curr_tcb_ptr: pointer to the TCB of the current task.

  tasklock_status: 1 if TASKLOCK is in effect, 0 otherwise.

  stack_frame: the contents of the last frame pushed on to the current
               stack. if the current mode is system mode, the frame will
               be taken from the system stack. if the current mode is
               supervisor mode, the frame will be taken from the stack of
               the current task.
 
  errlinenum:  the line number of the ERR_FATAL call. This field may be zero
               if the core dump was caused by something other than an
               ERR_FATAL.

  errparms:    The parameters to ERR_FATAL. All the entries in this array may
               be zero if the core dump was caused by something other than an
               ERR_FATAL.

  errfile:     a null-terminated string containing the name of the file
               from which ERR_FATAL was called. If the core dump was caused
               by something other than an ERR_FATAL, this string may be empty.

  errmsg:      a null-terminated string containing the string passed in as a
               parameter to ERR_FATAL. If the core dump was caused by
               something other than an ERR_FATAL, this string may be empty.

============================================================================*/

#define LOG_STACK_FRAME_SIZE  16
#define LOG_ERR_NUM_PARMS      4

enum
{
  REX_SVC_R0 = 0,
  REX_SVC_R1,
  REX_SVC_R2,
  REX_SVC_R3,
  REX_SVC_R4,
  REX_SVC_R5,
  REX_SVC_R6,
  REX_SVC_R7,
  REX_SVC_R8,
  REX_SVC_R9,
  REX_SVC_R10,
  REX_SVC_R11,
  REX_SVC_R12,
  REX_SVC_SP,
  REX_SVC_LR,
  REX_SVC_SPSR,
  REX_SVC_PC,
  REX_SYS_SP,
  REX_SYS_LR,
  REX_IRQ_SP,
  REX_IRQ_LR,
  REX_IRQ_SPSR,
  REX_ABT_SP,
  REX_ABT_LR,
  REX_ABT_SPSR,
  REX_UDF_SP,
  REX_UDF_LR,
  REX_UDF_SPSR,
  REX_FIQ_R8,
  REX_FIQ_R9,
  REX_FIQ_R10,
  REX_FIQ_R11,
  REX_FIQ_R12,
  REX_FIQ_SP,
  REX_FIQ_LR,
  REX_FIQ_SPSR,
  REX_JVA_SP,
  REX_JVA_LR,
  REX_JVA_SPSR,
  SIZEOF_LOG_COREDUMP_REGISTERS
};

LOG_RECORD_DEFINE(LOG_COREDUMP_C)

   uint32   regs[SIZEOF_LOG_COREDUMP_REGISTERS];
   uint32   cpsr;
   uint32   curr_tcb_ptr;
   uint32   tasklock_status;
   uint32   stack_frame[LOG_STACK_FRAME_SIZE];
   uint32   errlinenum;
   uint32   errparms[LOG_ERR_NUM_PARMS];
   char     errfile[1]; 
   char     errmsg[1]; 

LOG_RECORD_END

/*===========================================================================
FUNCTION REX_CORE_LOG_GET_ERR_FATAL_DUMP

DESCRIPTION
   Returns a pointer to the err_fatal_dump structure and its size so that it
can be saved to a log.

PARAMETERS
  size_ptr - Receives the size of the err_fatal_dump buffer

RETURNS 
  Pointer to err_fatal_dump buffer

============================================================================*/
void* rex_core_log_get_err_fatal_dump(uint32* size_ptr);

#endif /* ERRLOG_H */
