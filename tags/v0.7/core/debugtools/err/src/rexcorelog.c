/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            REX Core Log 

GENERAL DESCRIPTION
   Generate log record with REX core dump information 

Copyright (c) 2002-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //depot/asic/msmshared/services/err/rexcorelog.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/07   tbg     Renamed rexlog.h to errlog.h
06/13/07   sep     Put a limit on copying past the end of the stack, added 
                   task_name field.  Increased saved stack to 1kB.
03/15/04    gr     Moved rex_core to outside of FEATURE_REX_CORE_DUMP; this
                   structure is used in other debug functionality.
11/14/02   jct     Created 
===========================================================================*/

/* --------------------------------------------------------------------------- 
** Includes
** ---------------------------------------------------------------------------  */
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "errlog.h"

#ifdef FEATURE_REX_CORE_DUMP
#ifdef FEATURE_QPMD
#include "qpmd/qpmd.h"
#endif /* FEATURE_QPMD */

#include "fs.h"
#include "err.h"
#endif /* FEATURE_REX_CORE_DUMP */

extern uint32 rex_get_sp (void);

/* --------------------------------------------------------------------------- 
** Data
** ---------------------------------------------------------------------------  */

/* Core Dump Data
*/
/*lint -esym(765,rex_core) supress warning 765 */
LOG_COREDUMP_C_type rex_core = { 0x00 };

#ifdef FEATURE_REX_CORE_DUMP

#define REX_CORE_FILENAME_MAX_LENGTH FS_FILENAME_MAX_LENGTH
/* Set max format length below. Theoretically, based on constants in
** boot and diag header files, the format string could be as much as 64k.
** In practice, the format string is only several hundred bytes. If the
** smaller static buffer defined below turns out to be inadequate, we will
** need to look at ways of dynamically allocating more memory (even from
** IST or TASK_LOCK mode).
*/
#define REX_CORE_FORMAT_MAX_LENGTH   (512)

#define CORE_DUMP_TASK_NAME_LEN (15)

typedef struct
{
   uint32 curr_tcb_ptr;
   uint32 tasklock_status;
   char   task_name[CORE_DUMP_TASK_NAME_LEN+1];
   uint32 stackframe[256];
   uint32 errlinenum;
   uint32 errparms[4];
   char   err_strings[REX_CORE_FILENAME_MAX_LENGTH + REX_CORE_FORMAT_MAX_LENGTH];
} t_core_dump;

static t_core_dump err_fatal_dump;

#ifdef FEATURE_QPMD
/*===========================================================================
FUNCTION REX_INIT_CORE_DUMP

DESCRIPTION
   Sends a log with the current core dump information.  Can be used from
   a services such as ERR_FATAL to additionally provide line number, file name,
   and parameters

   Caller is responsible for calling rex_jettison_core to obtain the register
   set at the time the caller desires, otherwise, r0-r4, and sp are munged
   at this point vs the likely desired effect.  This call simply floats the
   buoy if the ship still remains.
============================================================================*/
/*lint -esym(765,rex_init_core_dump) supress warning 765 */
/*lint -esym(714,rex_init_core_dump) supress warning 714 */
void rex_init_core_dump(void)
{
    /* Register ERR_FATAL memory with the QPMD server */
    (void) qpmd_register_memory (QPMD_ERR_FATAL_DUMP,
                                 sizeof(err_fatal_dump),
                                 (void *)&err_fatal_dump);

    /* Register the memory used to save the registers */
    (void) qpmd_register_memory (QPMD_REGISTER_DUMP,
                                 sizeof (LOG_COREDUMP_C_type),
                                 (void *)&rex_core);
}
#endif /* FEATURE_QPMD */

/*===========================================================================
FUNCTION REX_LOG_CORE_DUMP

DESCRIPTION
   Sends a log with the current core dump information.  Can be used from
   a services such as ERR_FATAL to additionally provide line number, file name,
   and parameters

   Caller is responsible for calling rex_jettison_core to obtain the register
   set at the time the caller desires, otherwise, r0-r4, and sp are munged
   at this point vs the likely desired effect.  This call simply floats the
   buoy if the ship still remains.
============================================================================*/
/*lint -esym(765,rex_log_core_dump) supress warning 765 */
/*lint -esym(714,rex_log_core_dump) supress warning 714 */
void rex_log_core_dump(uint16      line,
                       const char *filename,
                       const char *fmt,
                       uint32      p1,
                       uint32      p2,
                       uint32      p3)
{
    uint32        i;
    char *string_ptr;
    /*lint -e{740} suppress "Unusual pointer cast" */
    /*lint -e{740} suppress "Unusual pointer cast" */
    uint32      *rex_stack_ptr = (uint32 *)(rex_core.regs[REX_SYS_SP]);
    uint32        length;    
    uint32        task_name_len;
    rex_tcb_type *tcb_ptr;
    uint32        stack_end;
    uint32        stack_ptr;
    uint32        bytes_to_copy;

    /* Perform a sanity check on the rex_core. If there is no saved CPSR
     * then save it here to allow for reconstruction later */
    if (NULL == rex_core.cpsr) {
        rex_core.cpsr = rex_get_cpsr ();
    }

    if(rex_core.curr_tcb_ptr != 0)
    {
        tcb_ptr = (rex_tcb_type*)rex_core.curr_tcb_ptr;
    }
    else
    {
        tcb_ptr = rex_self();
    }
    err_fatal_dump.curr_tcb_ptr         = (uint32) tcb_ptr; 
    err_fatal_dump.tasklock_status      = (unsigned long)TASKS_ARE_LOCKED();

    /* REX_TASK_NAME_LEN is either 11 or 127 bytes. CORE_DUMP_TASK_NAME_LEN 
     * is 15 bytes. We need to make sure CORE_DUMP_TASK_NAME_LEN is consistent 
     * with the length in core_to_t32.py. Here we need to copy either 11 or 
     * 15 bytes */
    /*lint -e{506} suppress "Constant value Boolean". See above comments. */     
    if (CORE_DUMP_TASK_NAME_LEN < REX_TASK_NAME_LEN)
    {
        task_name_len = CORE_DUMP_TASK_NAME_LEN;
    }
    else
    {
        task_name_len = REX_TASK_NAME_LEN;    
    }

    /*Copy the task name into the err_fatal_dump*/
    for(i = 0; i < task_name_len ; i++)
    {
        err_fatal_dump.task_name[i] = tcb_ptr->task_name[i];
        if(tcb_ptr->task_name[i] == '\0')
        {
            break;
        }
    }
    /*Make sure it is null terminated*/
    err_fatal_dump.task_name[task_name_len] = '\0';

    /*Get the address of the end of the stack for this task, and make sure
      we don't read beyond it.*/
    stack_end = (uint32)(tcb_ptr->stack_limit) + tcb_ptr->stack_size;

    /* Check for a valid stack pointer in the core dump. If none is
     * available then use the current thread stack pointer */
    if(NULL != (rex_stack_ptr))
    {
        stack_ptr = (uint32)rex_stack_ptr;
    }
    else
    {
        stack_ptr = (uint32)rex_get_sp();
    }

    /*Copy the stack.  Only do the copy if the stack pointer is within the 
       stack for the TCB*/
    if((stack_ptr < stack_end) && (stack_ptr > (uint32)tcb_ptr->stack_limit))
    {
        /*Get the number of bytes to copy.  Make sure it doesn't go past 
          the end of the stack in the TCB.*/
        bytes_to_copy =  sizeof (err_fatal_dump.stackframe);
        if(stack_ptr + bytes_to_copy > stack_end)
        {
            bytes_to_copy = stack_end - stack_ptr;
        }

        memcpy ((char *)err_fatal_dump.stackframe,
                (char *)stack_ptr,
                bytes_to_copy);
    }
        
    err_fatal_dump.errlinenum           = line;
    err_fatal_dump.errparms[0]          = p1;
    err_fatal_dump.errparms[1]          = p2;
    err_fatal_dump.errparms[2]          = p3;
    err_fatal_dump.errparms[3]          = 0;

    string_ptr = &(err_fatal_dump.err_strings[0]);
    if (filename)
    {
        length=strlen(filename);
        if (length >= REX_CORE_FILENAME_MAX_LENGTH)
        {
            length = REX_CORE_FILENAME_MAX_LENGTH - 1;
        }
        memcpy(string_ptr, filename, length);
        string_ptr[length]=0;

        string_ptr += length + 1;
    }

    if (fmt)
    {
        length=strlen(fmt);
        if (length >= REX_CORE_FORMAT_MAX_LENGTH)
        {
            length = REX_CORE_FORMAT_MAX_LENGTH - 1;
        }
        memcpy(string_ptr, fmt, length);
        string_ptr[length]=0;

        string_ptr += length + 1;
    }
        
}

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
void* rex_core_log_get_err_fatal_dump(uint32* size_ptr)
{
    if(size_ptr != 0)
    {
        /*Make sure the size is rounded to a multiple of 4*/
        *size_ptr = sizeof(err_fatal_dump);
    }

    return (void*)&err_fatal_dump;
}


#endif
