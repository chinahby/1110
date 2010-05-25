#ifndef ERR_H
#define ERR_H

/*===========================================================================

                    Error Handling Service Header File

Description
  The error handling services (ERR) provide services for other units to
  log errors and query on logged errors.  The unit is implemented as a set
  of callable functions.  Errors are stored in the NV error log array
  which is composed of inidividual error records, one per each unique error
  reported (see NV.H for details of error log).  Errors are classified as
  non-fatal or fatal.  Non-fatal errors return to the caller after the error
  is logged.  Fatal errors cause a reset or powerdown of the system after
  the error is logged.  Depending on the level of MSG reporting enabled,
  errors are also sent to the DM via the MSG unit.  Depending on the
  conditional compile switches defined by the MSG utility, error messages
  can selectively be printed to the DM screen during development.  Upper
  layers within ERR allow storing of errors in a write-through memory cache
  so that a record of current error log and error counts is available for
  immediate query.

Copyright (c) 1992-2008 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/err/err.h#7 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/08   tbg     Added FEATURE_EXTERNAL_ERR_FATAL
04/04/08   tbg     FEATURE_GENERIC_ERR_FATAL_MSG for additional configurations
12/18/07   taw     Use IMAGE_APPS_PROC instead of DIAG_APPS_PROC around 
                   err_lcd_message.
01/08/07   tbg     Added support for new err_auto_action API.
12/19/06   as      Reverted FEATURE_ERR_SMALL_BUFFER change.
12/18/06   as      Added support to decrease buffer size under 
                   FEATURE_ERR_SMALL_BUFFER.
11/11/05   tbg     Changed ERR_EXTENDED_STORE to share NV item for max files
                   with debug trace (if feature is defined)
06/29/05   as      Increased ERR_DATA_MAX_SIZE.
04/17/04   ash     Added (FEATURE_MEM_USAGE_AT_EXCEPTION)
                   for application memory usage during crash
08/03/04   tbg     Extended FEATURE_ERR_EXTENDED_STORE to support multiple
                   files on EFS, as well as multiple logs per session.
07/09/04   eav     Added feature to save ERR_FATAL to trace buffer as a message
04/27/04   eav     Moved ERR_DATA_MAGIC_NUMBER and ERR_DATA_RESET_MAGIC_NUMBER
                   here so it is accessible from msg.c.
04/06/04   tbg     Added FAETURE_ERR_EXTENDED_STORE
10/08/03   gr      Redefined ERR_FATAL to make one function call instead of
                   three to save code space.
07/24/03   as      Added ERR_FATAL_REPORT(   ) macro
11/14/02   jct     Added REX core dump to ERR_FATAL
09/13/02   et      added ERR_LOG macro to log err to NV w/o F3 msg
08/20/02   lad     F_DIAG_COUPLED_ARCHITECTURE support.
04/06/01   lad     Added T_THOR to feature list.
02/23/01   lad     Changes for portability and re-use.
02/02/00   lad     Changed macros to parse as single expressions.
06/30/99   jct     Added PC_EMULATOR support for ERR_FATAL, made inclusion
                   of target.h conditional, added decoupling of inclusion of
                   nv.h based on assertion of FEATURE_ERR_DECOUPLE_NVM.
                   This allows resuse of ERR_FATAL in environment where NV is
                   not supported.
06/02/99   lad     Fixed multi-line macros to evaluate as single expression.
04/28/99   ms/sk   Inserted call to clear SL register in ERR_FATAL.
11/05/98   jct     Merged in changes for ARM targets.  Make calls to err_
                   functions const correct.
01/22/97   rdh     Used Pascal directive on err_*_put_log to save ROM.
07/24/95   jah     Changed ERR/ERR_FATAL() macro to use msg_file if MSG_FILE
                   is defined.
06/18/93   jah     Removed MSG_FATAL from phone version of ERR_FATAL() macro,
                   as it takes code space and the phone resets before the DM
                   can get the results from MSG.
10/29/92   ip      Updated for release of full error logging.
08/03/92   ip      Removed '{' and '}' from ERR_...
07/06/92   ip      Fixed target include files, added '{' and '}' to ERR_...
05/13/92   ip      Removed parentheses around 'format' in MSG_... calls.
03/07/92   ip      First porting of file from brassboard to subscriber.

===========================================================================*/

#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_EXTERNAL_ERR_FATAL

/* This is a generic feature to allow code such as third party operating
 * systems not specifically defined below to remove the definitions of
 * ERR_FATAL found in this file and provide their own.
 */

/* Currently this block is intentionally left empty of code */

#elif defined (FEATURE_L4_KERNEL)
#error code not present
#elif defined (TEST_FRAMEWORK)
/* Used for off-target testing of 4G modem stacks */

#include <tf_assert.h>
#include <stdlib.h>
#include "msg.h"

#define  ERR_FATAL(fmt, p1, p2, p3) \
  ERR( fmt, p1, p2, p3 );           \
  tf_external_assert_3( fmt, p1, p2, p3, __FILE__, __LINE__ )
#define ERR(str, a, b, c) MSG_ERROR("ERR: " str, a, b, c)

#elif defined (FEATURE_Q6SIM)
/* Used for off-target testing using qdsp6-sim */

#include <stdlib.h>
#include "msg.h"

#define ERR(str, a, b, c) MSG_ERROR("ERR: " str, a, b, c)
#define ERR_FATAL(str, a, b, c) \
  do {                          \
    ERR(str, a, b, c);          \
    exit(1);                    \
  } while(0)

#elif defined (FEATURE_WINCE)
#error code not present
#else

#ifdef TG
#include "target.h"
#endif

#include "msg.h"

/* -------------------------------------------------------------------------
   Definitions and Declarations
   ------------------------------------------------------------------------- */
#define ERR_FILE_NAME_LEN 8

#if defined(FEATURE_ERR_NV_LOG)
#define ERR_MAX_LOG 20             /* NV stores 20 error logs */
#else
#define ERR_MAX_LOG 20             /* This number can change */
#endif

/* ERR_DLOAD=0 and ERR_RESET=1 for backwards compatability reasons. */
typedef enum
{
  ERR_DLOAD = 0,
  ERR_RESET,
  ERR_NO_ACTION,
  SIZEOF_ERR_ACTION
} err_action_type;

#if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)

    #define MAX_NUMBER_OF_BREW_APPS     50 
    typedef struct _tagAPPMEMUSAGE
    {
        uint32   app_class_id ;
        uint32   app_memory_usage ;
        uint32   app_max_memory_usage ;
    
    }APPMEMUSAGE;
#endif 

typedef struct
{
  byte address;         /* Storage address 0 to ERR_MAX_LOG-1 */
  byte err_count;       /* Number of occurances (0=empty,FF=full) */
  byte file_name[ERR_FILE_NAME_LEN];

  word line_num;
  boolean fatal;
}
err_log_type;

/* Status returned from err_get_log and err_clr_log. */

typedef enum
{
  ERR_DONE_S,           /* Request completed OK */
  ERR_NONE_S            /* No such log number */
}
err_stat_type;

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
extern const char err_generic_msg[];
#endif


/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(IMAGE_APPS_PROC) && defined(FEATURE_ERR_LCD)
/*===========================================================================

FUNCTION ERR_LCD_MESSAGE

DESCRIPTION
  This function is called when an ERR_FATAL has occurred on the modem 
  processor. It copies the error message placed in shared memory
  by the modem processor into local memory and then uses lcd_message to 
  display it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Hangs the apps processor because the modem processor hit error_fatal.

===========================================================================*/
  void err_lcd_message( void );
#endif /* defined(IMAGE_APPS_PROC) && defined(FEATURE_ERR_LCD) */


/*===========================================================================

FUNCTION ERR_INIT

DESCRIPTION
  This function checks if NV has been built.  If so then it loads up the
  error log cache from NV, it initializes error counts and local variables,
  and it sets error services to online.  If NV has not been built then
  error log is not loaded and error services remain offline and error
  logging does not take place.  Any access to NV is performed here directly,
  without relying on the NV task.  Thus error service can be initialized
  and used early in the life of the DMSS and right after boot.  Note
  that if NV has not been built, error services provide no error logging
  capabilities.  However NV is built once, after initial factory startup,
  and rarely thereafter.  Thus except for the first ever powerup of the
  unit after NV build or re-build, error logging will take place as usual.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Error log is loaded from NVM into RAM resident cache.

===========================================================================*/
  void err_init (void);

/*===========================================================================

FUNCTION ERR_GET_LOG

DESCRIPTION
  Copy to the caller's pointer a specific error record in the error
  log cache.  The error record is identified by its number, from 0 to
  NV_MAX_ERR_LOG.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error log existed
  ERR_NONE_S    - No such log number (out of range).

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE
  
  /* A dependency in uismenu.c requires this workaround. */
  #define err_get_log(a, b) err_get_log_ex (a, (err_log_type *) b)

  err_stat_type err_get_log_ex  (byte log_num, err_log_type * log_ptr);

#else

  err_stat_type err_get_log (byte log_num, err_log_type * log_ptr);

#endif

/*===========================================================================

FUNCTION ERR_CLR_LOG

DESCRIPTION
  This function clears a specific error record both in cache and in NV.
  An error record is cleared simply by clearing the error count field
  in it.
  
  If 'log_num' == 0xFF, all logs will be cleared.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error record existed and cleared.
  ERR_NONE_S    - Error record out of range or clear failed.

SIDE EFFECTS
  The specified error record is cleared in NV and in cache and is now
  available to accept a new unique error.

===========================================================================*/
  err_stat_type err_clr_log (byte log_num);

/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR(...) macro.  It then calls
  other local functions to store the error.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place.  This function must not be called directly; it may
  be called only from the ERR macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/
  void err_put_log (word line, const char *file_ptr);

/*===========================================================================

FUNCTION ERROR_FATAL_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR_FATAL(...) macro.  First and
  immediately this function kills the PA.  Next an attempt is made to
  log the error to NV.  If there was room in the error log for a new
  error record, or if incrementing an existing error record count was
  below threshold then we force reset of the DMSS.  If the error
  could not be added to the log, or if it had exceeded count threshold
  then we force a powerdown.  The purpose of this is to let the DMSS
  reset upon fatal error but only up to a certain threshold.  After
  this threshold the unit will powerdown, to avoid continual reset
  cycle and possible thrashing of NV write cycle limit.

DEPENDENCIES
  err_init must have been called first, otherwise we reset but we
  do not log anything to NV.  This function must not be called
  directly; it may only be called from the ERR_FATAL macro.

RETURN VALUE
  None.

SIDE EFFECTS
  ***********************************************************
  ********** THERE IS NO RETURN FROM THIS FUNCTION **********
  ***********************************************************

===========================================================================*/

  /* lint -function(abort, err_fatal_put_log) */
  void err_fatal_put_log (word line, const char *file_ptr);


/*===========================================================================

FUNCTION ERR_ENABLE_FS_LOG
DESCRIPTION
  Enables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_enable_fs_log (void);


/*===========================================================================

FUNCTION ERR_DISABLE_FS_LOG
DESCRIPTION
  Disables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_disable_fs_log (void);

/*===========================================================================

FUNCTION ERR_FATAL_CORE_DUMP
DESCRIPTION
  Logs fatal error information, including a core dump.

============================================================================*/
void err_fatal_core_dump (unsigned int line, const char *file_name,
  const char *format);

/*===========================================================================

FUNCTION ERROR_FATAL_PUT_REPORT
DESCRIPTION
  Logs fatal error information, including a core dump.

============================================================================*/
void err_fatal_put_report (unsigned int line, const char *file_name,
  const char *format);

/*===========================================================================

MACRO ERR

DESCRIPTION
  Log an error to error log and update cache and return to caller.
  Error is sent to the MSG utility to be routed to DM or PC screen,
  depending on conditional compile switches defined in MSG.  If host
  is a PC then NV logging does not take place and only MSG output
  is used.

DEPENDENCIES
  If MSG_FILE is defined, ERR uses it as the filename.

RETURN VALUE
  None.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR must only be called because of
  an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effect are OK.

===========================================================================*/

#define ERR_FILE_NAME   msg_file

#define  ERR(format, code1, code2, code3)          \
  do {                                             \
    MSG_ERROR (format, (code1), (code2), (code3)); \
    err_put_log ((word) __LINE__, ERR_FILE_NAME);  \
/*lint -save -e{717} */                                    \
  } while (0)                                      \
/*lint -restore */

/*===========================================================================

MACRO ERR_LOG

DESCRIPTION
  Log an error to error log and update cache and return to caller.

DEPENDENCIES
  err_init must have been called

RETURN VALUE
  None.

SIDE EFFECTS
  updates NV memory with the file name and line# of the error

===========================================================================*/
#define ERR_LOG() \
  do {                                             \
    err_put_log ( __LINE__, ERR_FILE_NAME );       \
  } while(0)                                       \


/*===========================================================================

MACRO ERR_FATAL

DESCRIPTION
  Log an error to error log, update the cache (for debug purpose only)
  and RESET THE SYSTEM.  Control DOES NOT RETURN to caller.  Error is
  sent to the MSG utility to be routed to DM or PC screen, depending
  on conditional compile switches defined in MSG.  If host is a PC
  then NV logging does not take place.  Instead a MSG output is made
  and the program exits (simulating hardware reset of sorts).

DEPENDENCIES
  If MSG_FILE is defined, ERR_FATAL uses it as the filename.

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR_FATAL must only be called because
  of an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effects are OK.

===========================================================================*/

/*===========================================================================

MACRO ERR_FATAL_REPORT

DESCRIPTION
  Log an error to error log, update the cache (for debug purpose only)
  and RESET THE SYSTEM.  Control DOES NOT RETURN to caller.  Error is
  sent to the MSG utility to be routed to DM or PC screen, depending
  on conditional compile switches defined in MSG.  If host is a PC
  then NV logging does not take place.  Instead a MSG output is made
  and the program exits (simulating hardware reset of sorts).

DEPENDENCIES
  If MSG_FILE is defined, ERR_FATAL_REPORT uses it as the filename.

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR_FATAL_REPORT must only be called because
  of an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effects are OK.

===========================================================================*/

  /* Prototype for a boot routine that clears the stack limit (SL) register. 
  */
  void boot_clear_sl (void);

  /* Prototypes REX core dump feature
  */
#ifdef FEATURE_REX_CORE_DUMP
   extern void rex_jettison_core( void );
#endif

#ifdef PC_EMULATOR_H

   #define PC_EMULATOR_ERR
      #include PC_EMULATOR_H
   #undef PC_EMULATOR_ERR

#else

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)

#ifdef FEATURE_REX_CORE_DUMP

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  
  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
        rex_jettison_core();                                                     \
        {                                                                        \
                XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, err_generic_msg );\
                msg_save_3(&xx_msg_const, (uint32) (code1), \
                              (uint32) (code2), (uint32) (code3), NULL); \
        err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, err_generic_msg );        \
        }                                                                        \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */

#else

  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
        rex_jettison_core();                                                     \
        {                                                                        \
                XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, format ); \
                msg_save_3(&xx_msg_const, (uint32) (code1), \
                              (uint32) (code2), (uint32) (code3), NULL); \
        err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, format );                 \
        }                                                                        \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */

#endif /* FEATURE_GENERIC_ERR_FATAL_MSG */

  #define ERR_FATAL_NO_SAVE(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
        rex_jettison_core();                                                     \
        err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, format );                 \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */

#else /* FEATURE_REX_CORE_DUMP not defined */

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
       XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, err_generic_msg ); \
       msg_save_3(&xx_msg_const, (uint32) (code1), \
                  (uint32) (code2), (uint32) (code3), NULL); \
         err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, err_generic_msg );       \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */
#else
  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
       XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, format ); \
       msg_save_3(&xx_msg_const, (uint32) (code1), \
                  (uint32) (code2), (uint32) (code3), NULL); \
         err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, format );                 \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */
#endif  

  #define ERR_FATAL_NO_SAVE(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                   \
        err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, format );                 \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */
#endif /* FEATURE_REX_CORE_DUMP */
       
       
#else  /* (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE) */
#ifdef FEATURE_REX_CORE_DUMP

  #ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
        rex_jettison_core();                                                     \
        err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, err_generic_msg );        \
        /*lint -save -e{717} */                                                  \
     } while (0)                                                                 \
        /* lint -restore */
  #else
    #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
        rex_jettison_core();                                                     \
        err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, format );                 \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */
  #endif /* FEATURE_GENERIC_ERR_FATAL_MSG */
  
#else
  #ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
       err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, err_generic_msg );         \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */
  #else
  #define ERR_FATAL(format, code1, code2, code3)                                 \
    do                                                                           \
    {                                                                            \
       err_fatal_core_dump ( __LINE__, ERR_FILE_NAME, format );                 \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */
  #endif /* FEATURE_GENERIC_ERR_FATAL_MSG */
  
#endif
#endif /* not FEATURE_SAVE_DEBUG_TRACE */
  #define ERR_FATAL_REPORT(format, code1, code2, code3)                          \
     do {                                                                        \
        err_fatal_put_report ( __LINE__, ERR_FILE_NAME, format );                \
        /*lint -save -e{717} */                                                          \
     } while (0)                                                                 \
        /* lint -restore */

#endif



/*===========================================================================

FUNCTION err_set_auto_action

DESCRIPTION
  Set the action to be taken when an error is encountered.

DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void err_set_auto_action(err_action_type action);

/*===========================================================================

FUNCTION err_get_auto_action

DESCRIPTION
  Return the action to be taken when an error is encountered.

DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

err_action_type err_get_auto_action(void);

  
#ifdef FEATURE_ERR_EXTENDED_STORE

/* Number of info files stored before we begin overwriting them. */
/* Only applicacable if FEATURE_SAVE_TACE_EXTENDED is not defined. */
/* Otherwise, we use the same value as diag_debug_max_files */
#ifndef FEATURE_SAVE_TRACE_EXTENDED
#define ERR_DATA_NUM_FILES 4
#endif

/* Magic number that determine whether the data in uninitialized RAM has
 * already been saved to flash or not.
 */
#define ERR_DATA_MAGIC_NUMBER       (uint64)0x5252452041544144ULL
#define ERR_DATA_RESET_MAGIC_NUMBER (uint64)0xABCD06261974EA29ULL

/* Maximum number of error logs to store per session */
#ifndef ERR_DATA_MAX_LOGS
#define ERR_DATA_MAX_LOGS 2
#endif  
  
/* Maximum size of the extended info written out to flash.
 */
#ifndef ERR_DATA_MAX_SIZE  
#define ERR_DATA_MAX_SIZE 98304
#endif  
  
typedef uint64 err_data_flag_type;

typedef struct {
  err_data_flag_type flag;
  uint32 length;
  char   data[ERR_DATA_MAX_SIZE];
} err_data_log_type;

typedef struct {
  err_data_flag_type reset_flag;
  err_data_log_type err_log[ERR_DATA_MAX_LOGS];
  
  #if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)
    APPMEMUSAGE  g_application_memory_usage[MAX_NUMBER_OF_BREW_APPS] ;  
  #endif 
} err_data_type;

/*===========================================================================

FUNCTION ERR_DATA_IS_VALID
DESCRIPTION
  Returns the number of error logs that are filled.
============================================================================*/
boolean err_data_is_valid( void );

/*===========================================================================

FUNCTION ERR_STORE_INFO
DESCRIPTION
  Writes a bunch of information to a designated RAM buffer.
============================================================================*/
void err_store_info (const char *filename, unsigned int line);

/*===========================================================================

FUNCTION ERR_ADD_TO_MEM_TABLE
DESCRIPTION
  Adds an entry to the table of addresses and lengths that will be stored
  on a fatal error or abort.
  Returns 1 on success and 0 on failure.
============================================================================*/
int err_add_to_mem_table (char *name, void *ptr, unsigned long length);

/*===========================================================================

FUNCTION ERR_DELETE_FROM_MEM_TABLE
DESCRIPTION
  Deletes an entry from the table of addresses and lengths that will be stored
  on a fatal error or abort.
  Returns 1 on success and 0 on failure.
============================================================================*/
int err_delete_from_mem_table (void *ptr);

/*===========================================================================

FUNCTION ERR_STORE_INFO_TO_FLASH
DESCRIPTION
  Stores information associated with a fatal error or an exception to
  flash.
============================================================================*/
void err_store_info_to_flash (void);

#endif /* FEATURE_ERR_EXTENDED_STORE */

#ifdef __cplusplus
}
#endif

#endif /* from #ifdef FEATURE_EXTERNAL_ERR_FATAL #elif tree */

#endif /* ERR_H */

