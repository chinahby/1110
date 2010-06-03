/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            REX Core Log 

GENERAL DESCRIPTION
   Generate log record with REX core dump information 

Copyright (c) 2002-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rexcorelog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/04    gr     Moved rex_core to outside of FEATURE_REX_CORE_DUMP; this
                   structure is used in other debug functionality.
11/14/02   jct     Created 
===========================================================================*/

/* --------------------------------------------------------------------------- 
** Includes
** ---------------------------------------------------------------------------  */
#include "customer.h"
#include "comdef.h"
#include "AEEstd.h" /* std_strlcpy */
#include "rex.h"
#include "rexlog.h"

extern rex_tcb_type *rex_curr_task;

/* --------------------------------------------------------------------------- 
** Data
** ---------------------------------------------------------------------------  */

/* Core Dump Data
*/
LOG_COREDUMP_C_type rex_core = { 0x00 };

#ifdef FEATURE_REX_CORE_DUMP

/* Set max filename and format length below. Theoretically, 
** the filename and format string could be as much as 64k.
** In practice, the format string is at most several hundred bytes. 
*/
#define REX_CORE_FILENAME_MAX_LENGTH   (360)
#define REX_CORE_FORMAT_MAX_LENGTH   (360)

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
void rex_log_core_dump(
   uint16      line,
   const char *filename,
   const char *fmt,
   uint32      p1,
   uint32      p2,
   uint32      p3
)
{
   LOG_COREDUMP_C_type *core_ptr;
   size_t filename_length; //string length with NULL terminator
   size_t fmt_length; //string length with NULL terminator
   size_t size;

   if (NULL == filename || NULL == fmt)
   {
      return;
   }

   filename_length = strlen(filename) + 1;
   if (filename_length > REX_CORE_FILENAME_MAX_LENGTH)
   {
      filename_length = REX_CORE_FILENAME_MAX_LENGTH;
   }

   fmt_length = strlen(fmt) + 1;
   if (fmt_length > REX_CORE_FORMAT_MAX_LENGTH)
   {
      fmt_length = REX_CORE_FORMAT_MAX_LENGTH ;
   }

   size = sizeof(LOG_COREDUMP_C_type) + filename_length + fmt_length;
   
   core_ptr = (LOG_COREDUMP_C_type *)log_alloc(LOG_COREDUMP_C, size); 

   if( core_ptr != NULL )
   {
      rex_core.cpsr            = rex_get_cpsr();
      rex_core.curr_tcb_ptr    = (uint32)rex_curr_task; 
      rex_core.tasklock_status = (uint32)TASKS_ARE_LOCKED(); 
      rex_core.errlinenum      = line;
      rex_core.errparms[ 0 ]   = p1;
      rex_core.errparms[ 1 ]   = p2;
      rex_core.errparms[ 2 ]   = p3;

      (void)memcpy(
         (void*)&rex_core,
         (void*)core_ptr,
         sizeof( log_hdr_type )
      );
    
      (void)memcpy( 
         (void*)core_ptr, 
         (void*)&rex_core, 
         sizeof(LOG_COREDUMP_C_type) 
      );

      size -= sizeof(LOG_COREDUMP_C_type);
      (void)std_strlcpy((char *)core_ptr->errfile, filename, size);
      size -= filename_length;

      /*lint -e{662} disabling error 662 Possible creation of out-of-bounds 
       ** pointer, since we know it is not out of bound. This lint error 
       ** was caused by "char errfile[1];" in LOG_COREDUMP_C_type. 
       ** Until we find a better solution, we cannot easily remove this 
       ** lint error. 
       */
      (void)std_strlcpy((char *)(core_ptr->errfile + filename_length), fmt, size);

      log_commit( core_ptr );
   }
   return;
} /* END rex_log_core_dump */

#endif
