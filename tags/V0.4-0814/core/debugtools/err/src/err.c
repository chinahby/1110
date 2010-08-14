/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error reporting services for both fatal and
  non-fatal errors.  Functions allow the caller to retrieve and store
  error records from NV and to obtain certain error status.  The error
  log is stored in NV but is maintained in a RAM based cache to allow
  direct read and status capabilities.  This module is not a task, but
  rather a set of callable procedures which run in the context of the
  calling task.

EXTERNALIZED FUNCTIONS
  err_init
    Initialize the memory resident error log cache from the NV.
  err_logged
    Return the number of unique errors logged.
  err_ignored
    Return the total number of errors ignored because the log was full.
  err_get_log
    Get an error record from the memory resident cache.
  err_clr_log
    Clear an error record in both cache and NV.
  err_put_log
    Add a non-fatal error to the NV error log.
  err_fatal_put_log
    Add a fatal error to the NV error log.
  err_lcd_message
    Displays a modem error message on the apps display for dual processor
    builds.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  err_init must be called first upon powerup, to initialize the cache
  from the NV.  Any ERR or ERR_FATAL macros called before then cause
  a reset.  ERR or ERR_FATAL must NOT be called from any of the functions
  contained in this file.

Copyright (c) 1992-2008 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //depot/asic/qsc1100/services/err/err.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/08   tbg     Added support for native rex environment
11/30/07   tbg     Updated automation settings for FEATURE_ERR_DEBUG_HANDLER.
10/08/07   tbg     Some code cleanup + added 6k support.
02/07/07   as      Fixed lint critical.
02/01/07   bfc     Fixed a reference to an obsolete SMEM API
09/26/07   hwu     Use new APIs to reduce multiprocessor specific code.
01/08/07   tbg     Added support for new err_auto_action API.  Also updated
                   ERR_DEBUG_HANDLER to support THIN_UI builds.
12/15/06   as      Fixed critical lint errors.
11/02/06   tbg     Fixed additional issues with MDP driver in new error handler
10/19/06   tbg     Updated ERR_EXTENDED_STORE to work on 7k MSMs.  Also added
                   new feature FEATURE_ERR_DEBUG_HANDLER.
09/26/06   ptm     Use new APIs to reduce multiprocessor specific code.
01/19/06   pc      Removed data in all records in err_cache on err_clr.
12/12/05   tbg     Switch flag for mem_info from boolean to 32bit.
                   Changed sprintf calls to snprintf.
11/11/05   tbg     Changed ERR_EXTENDED_STORE to share NV item for max files
                   with debug trace (if feature is defined)
10/31/05   as      Fixed lint errors.
08/30/05   ih      Added return of 1 to err_reset_check when a reset is 
                   detected.
08/11/05   as      Fixed complier warnings.
06/06/05   as      Added checks to do brew heap dump: if err_reset_flag is
                   true and if BREW heap variables are initialized.
04/17/04   ash     Added (FEATURE_MEM_USAGE_AT_EXCEPTION)
                   for application memory usage during crash
02/16/05   as      Added a check in err_log_store to write to NV only if
                   tasks are not locked.
02/15/05   as      Added FEATURE_DIAG_DEBUG_6275. This temporary feature will
                   be used until Raven code is stabilized.
11/30/04   as      Added FEATURE_AUTO_RESET. This feature resets the phone 
                   automatically without user intervention if the phone 
                   encounters a critical failure 
10/07/04   tbg     Added FEATURE_ERR_SAVE_CORE.
08/03/04   tbg     Extended FEATURE_ERR_EXTENDED_STORE to support multiple
                   files on EFS, as well as multiple logs per session.
05/14/04   eav     Added FEATURE_SAVE_DEBUG_TRACE.
04/27/04   eav     Added FEATURE_SAVE_DEBUG_TRACE.  Moved ERR_DATA_MAGIC_NUMBER
                   and ERR_DATA_RESET_MAGIC_NUMBER to err.h, so they are
                   accessible in msg.c.
04/06/04   tbg     Added FEATURE_ERR_EXTENDED_STORE
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
10/08/03   gr      Redefined ERR_FATAL to make one function call instead of
                   three to save code space.
07/24/03   as      Added ERR_FATAL_REPORT(   ) macro
03/04/03   djm     WPLT only modification to display LED uptime counter when
                   ERR_FATAL is encountered.
08/20/02   lad     PLATFORM_LTK and F_DIAG_COUPLED_ARCHITECTURE support.
06/10/02   cs      Updated err_init to read all the NV log records.
02/12/02    gr     Modified err_fatal_put_log to support five digit line
                   numbers.
06/27/01   lad     Cosmetic changes.
04/06/01   lad     Moved local definition of nv_err_log_type.
                   Changed use of NV_MAX_ERR_LOG to ERR_MAX_LOG.
02/23/01   lad     Featurized coupled code and did some cleanup.
08/28/00   lad     Added T_O to condition for ERR_HAS_LCD_MESSAGE.
05/12/99   lad     Fixed err_log_store() to give fatal errors prescedence
                   over non-fatal errors.
11/15/98   jct     Removed pragmas at top of file, moved _disable prototype
                   inside a check to see if the method is already defined
11/05/98   jct     Merged in changes for ARM
09/02/98   mk      Added hw_power_off() to err_fatal_put_log() for MSM3000.
05/15/98   bns     Added support for SSS2 phones.
08/08/97   dak     Fixed ERR_FATAL handling for WLL targets. Jump to download
                   if phone is on hook. Define ERR_FORCE_DOWNLOAD to jump
                   to downloader on very early ERR_FATALs.
07/23/97   jah     Turn off ringer on MSM 2 phones in err_fatal_put_log().
07/02/97   jjn     Change on 05/23/97 was for Module hardware with 20pin
                   SAMTEC connector.  SYS_WAKE is on a different GPIO pin
                   on 30pin SAMTEC connector for X2 Module hardware.
                   Software has to be changed to accompany that HW change.
06/17/97   dhh     Added target T_Q for the Q Phones.
05/23/97   jjn     Assert SYS_WAKE when ERR_FATAL occurs (for the Module only)
01/22/97   rdh     Used Pascal directive on err_*_put_log to save ROM.
11/02/96   jah     Configured for TGP (T_T), added ERR_HAS_LCD_MESSAGE
05/24/96    ls     Fixed reentrancy problems and other errors.
07/21/95   ras     disabled interrupts in err_fatal_put_log
07/05/95   jah     Added lcd_message() for Gemini
07/16/93   jah     Changed LCD_MESSAGE() to lcd_message(), and updated
                   comments, per code review.
06/29/93   jah     Added LCD_MESSAGE support for LCD display of fatal errors.
04/27/93   jah     Made T_P changes to support fatal error test code download.
10/29/92    ip     Updated for full error logging and reporting.
08/13/92    ip     Release with interim indefinite wait for ERR_FATAL.
03/07/92    ip     Initial creation.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "diagi.h"
#include "diagcmd.h"
#include "diagdiag.h"
#include "err.h"
#include "msg.h"
#include "AEEstd.h"
#include "hw.h"
#include "stdlib.h"

#ifdef FEATURE_ERR_EXTENDED_STORE

#ifdef FEATURE_SAVE_DEBUG_TRACE
#include "diagdebug.h"
  #ifdef FEATURE_SAVE_TRACE_EXTENDED
  #include "diagdebug_nzi.h"
  #endif
#endif

#endif /* FEATURE_ERR_EXTENDED_STORE */

#if !defined(ERR_DECOUPLE_NV)
#include "nv.h"
#endif
#if !defined(ERR_DECOUPLE_ENC)
#include "enc.h"
#endif

#if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)    
#include "AEE_OEM.h"
#include "AEE_OEMComdef.h"
#include "AEEComdef.h"
#endif 

#ifndef FEATURE_WINCE
#ifdef FEATURE_AMSS_RESTART
#include "process.h"
#endif /* FEATURE_AMSS_RESTART */
#endif

#include <string.h>

#include "dog.h"
#include "hs.h"

#if defined(FEATURE_ERR_LCD)
#include "lcd.h"
/* Number of digits in error line number information. Must be smaller
** than HS_SCRN_WIDE, which is typically 12 or so.
*/
#define ERR_LINENUM_DIGITS 5

#endif /* FEATURE_ERR_LCD */

#ifdef FEATURE_SMSM
#include "smsm.h"
#endif /* FEATURE_SMSM */

#ifdef FEATURE_SMEM
#include "smem.h"
#include "smem_diag.h"
#endif /* FEATURE_SMEM */

#ifdef FEATURE_SMEM_LOG
#include "smem_log.h"
#endif /* FEATURE_SMEM */

#ifndef FEATURE_WINCE
#ifdef DIAG_MP
#include "process.h"
#endif /* DIAG_MP */
#endif

#if defined(FEATURE_WCDMA_PLT)
#error code not present
#endif /* FEATURE_WCDMA_PLT */


#ifdef FEATURE_ERR_DEBUG_HANDLER
#include "boothw.h"
#include "keypad.h"
#include "clk.h"
#include "dload.h"
#include "tmcnv.h"
#endif


#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
/* Used to replace all ERR_FATAL message strings.
 * Reduces code size by eliminating other const strings used for errors.
 */
const char err_generic_msg[] = "Error Fatal, parameters: %d %d %d";
#endif


#ifdef FEATURE_ERR_EXTENDED_STORE
/* Misc includes and definitions for this feature.
 */

#include "boot_trap.h"
#include "mobile.h"
#include "rex.h"
#include "dog.h"
#include "rexlog.h"
#include "fs.h"
#include "armasm.h"
#include "boot_stacks.h"
#include "bootmem.h"
#include "fs_public.h"
#include "hw.h"
#include <stdio.h>

#ifdef FEATURE_ERR_SAVE_CORE
#include "err_rle.h"
#include "mdspext.h"
#endif

#if defined (FEATURE_L4) && defined (FEATURE_ERR_EXTENDED_STORE)
#error code not present
#endif

/* Version number of the extended store functionality.
 */
#define EXT_STORE_VERSION 1

extern rex_tcb_type *rex_best_task;
extern LOG_COREDUMP_C_type rex_core;
extern err_data_type err_data;
extern uint32 err_flash_write_counter;
#define ERR_MAX_FLASH_WRITE_ATTEMPTS 2

#ifdef FEATURE_ERR_SAVE_CORE
extern err_core_dump_type err_core_dump;
#endif

static char ver[] = "; Version ";
static char build_id[] = "; Build ID: ";
static char err_line[] = "; Error line: ";
static char err_file[] = "; Error file: ";
static char curr_task[] = "v rex_curr_task = ";
#ifndef FEATURE_L4
static char best_task[] = "v rex_best_task = ";
static char sched_allow[] = "v rex_sched_allow = ";
static char data_dump_indicator[] = "; Data dump";
#endif
static char time_stamp_str[] = "; Error Time Stamp: ";
static char err_file_format[] =  "%s_index%02d_log%02d.txt";
/* Temporary storage area before copying stuff over to uninitialized RAM.
 */
#define TEMP_BUF_SIZE 128
static char temp_buf[TEMP_BUF_SIZE];

typedef struct {
  uint32       valid_flag;
  char         *name;
  void         *address;
  unsigned long length;
} err_meminfo_type;

extern void time_get_julian(time_julian_type*);

  /* Time error occurred */  
  boolean error_time_set = FALSE;  
  time_julian_type error_time;

/* Table of addresses and lengths. A dump of these addresses is made
 * on a fatal error or an abort.
 */ 
#define MEM_INFO_TABLE_LEN 24
#define ERR_DATA_MEM_INFO_IS_VALID  (uint32)0x19283746

/*lint -save -e{785} */
static err_meminfo_type mem_info[MEM_INFO_TABLE_LEN] =
{
  {ERR_DATA_MEM_INFO_IS_VALID, "System Stack", (void *) sys_stack, (unsigned long) Sys_Stack_Size},
  {ERR_DATA_MEM_INFO_IS_VALID, "Abort Stack", (void *) abort_stack, (unsigned long) Abort_Stack_Size},
  {ERR_DATA_MEM_INFO_IS_VALID, "Supervisor Stack", (void *) svc_stack, (unsigned long) SVC_Stack_Size},  
  {ERR_DATA_MEM_INFO_IS_VALID, "IRQ Stack", (void *) irq_stack, (unsigned long) IRQ_Stack_Size}
};
/*lint -restore */

/* Directory and basename of the file into which err_fatal/abort
 * information is stored.
 */

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else
  #define ERR_DATA_DIR  "err"
  #define ERR_DATA_FILE "err/err_data"
  #define ERR_DATA_INDEX "err/err_index.txt"  
#endif /* FEATURE_MULTIPROCESSOR */


#ifndef FEATURE_L4
static boolean err_reset_flag = FALSE;
extern uint32 rex_get_pc (void);
#endif
extern uint32 rex_get_sp (void);

#endif /* FEATURE_ERR_EXTENDED_STORE */

#if defined (IMAGE_MODEM_PROC) && defined (FEATURE_ERR_DEBUG_HANDLER)
/* Advanced timeout options for use by 7k err_debug_handler */
typedef enum
{
  ERR_IMMEDIATE = 0,
  ERR_TIMEOUT,
  SIZEOF_ERR_TIMEOUT
} err_timeout_type;

/* MACROS for decoding Error Fatal NV item settings */
#define GET_TIMEOUT_VALUE(x)   ((uint8)(((x)&0xff00)>>8))
#define GET_TIMEOUT_OPTION(x)  ((err_timeout_type)(((x)&0x00f0)>>4))

static void err_set_timeout_option(err_timeout_type);
static void err_set_timeout_value(uint8);
static err_timeout_type err_timeout_option = ERR_IMMEDIATE;
static uint8 err_timeout_value = 0;
#endif

/* MACRO for decoding Error Fatal NV item settings */
#define GET_ACTION(x)          ((err_action_type)((x)&0x000f))

#ifdef FEATURE_L4
#error code not present
#else
#define rex_msm_int_lock rex_int_lock
#endif

#if defined(FEATURE_MULTIPROCESSOR) || defined(FEATURE_ERR_LCD)
/* Static for LCD_MESSAGE (also saved to shared memory even without LCD
 * in multiprocessor builds) */
/* 112 is added to fix a critical lint error. */
static char message[ HS_SCRN_HIGH * HS_SCRN_WIDE + 1 + 112]; 
#endif /* FEATURE_MULTIPROCESSOR || FEATURE_ERR_LCD */

#if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)
#define FILENAME_BREW_MEM "err/brew_mem_usage.txt"

extern uint32     AEE_GetModuleMemory(uint32 dwGroupID);

typedef struct _TempAppContext 
{
  struct _TempAppContext *pNext ;
}MYACONTEXT;    

/* WE ARE USING DUMMY ACONTEXT , WHICH IS COPY OF BREW
** ACONTEXT, WE CAN'T TAKE THIS BECAUSE IT IS PRIVATE
** TO BREW
*/  
static int err_print_BrewAppHeap (void ) ;
static int err_dump_brew_mem_info_to_flash(int fs_handle_dump) ;
static int n_count_times = 0 ;

#endif /* FEATURE_MEM_USAGE_AT_EXCEPTION */

static err_action_type err_auto_action = ERR_NO_ACTION;


void err_debug_handler( boolean );


/* -------------------------------------------------------------------------
   Macro Definitions
   ------------------------------------------------------------------------- */
#ifdef FEATURE_SMEM_LOG
/* Event defines for using SMEM to log what occurs during sleep and
 * power collapse.
 */
#define ERR_ERROR_FATAL                (SMEM_LOG_ERROR_EVENT_BASE + 1)
#define ERR_ERROR_FATAL_TASK           (SMEM_LOG_ERROR_EVENT_BASE + 2)

#define ERR_LOG_EVENT(event, data1, data2, data3) \
        SMEM_LOG_EVENT(event, data1, data2, data3)
#define ERR_LOG_EVENT6(event, data1, data2, data3, data4, data5, data6) \
        SMEM_LOG_EVENT6(event, data1, data2, data3, data4, data5, data6)
#else
#define ERR_LOG_EVENT(event, data1, data2, data3) do{} while(0)
#define ERR_LOG_EVENT6(event, data1, data2, data3, data4, data5, data6 ) \
        do{} while(0)
#endif /* FEATURE_SMEM_LOG */



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* Maintain count of unique errors logged.  Note that this is NOT the      */
/* total errors occuring; it is the total number of unique errors in the   */
/* log, since the occurance of an error already in the error log only      */
/* bumps up the error count of that error.                                 */

LOCAL unsigned int err_logged_count = 0;

/* Maintain count of total errors ignored because error log was full.      */

LOCAL unsigned int err_ignored_count = 0;

/* Local error log cache.                                                  */

LOCAL err_log_type err_cache[ERR_MAX_LOG];

/* Specify if error services have been initialized.  Until they are        */
/* initialized any fatal error causes a reset without logging to NV.       */

LOCAL boolean err_online = FALSE;

/* Specify if error should be logged to the filesystem.  
*/
LOCAL boolean err_log_to_fs = TRUE;

/* The threshold of number of errors of a unique type which are allowed    */
/* in order to force reset.  Once this number is exceeded on any reported  */
/* error we force a powerdown.  This ensures that we don't recycle power   */
/* forever (and waste NVM write cycles) for an error which occurs with     */
/* alarming regularity.                                                    */

#define ERR_NV_WRITE_THRESHOLD  20

/* Enum type used for different types of err fatals.
*/
typedef enum {
  ERR_FATAL_C = 0,        /* prints file/line  */
  ERR_FATAL_REPORT_C = 1  /* prints format message */
} err_fatal_enum_type;

/* If the user has requested that ERR be decoupled from NVM, then the
** definition of nv_err_log_type is provided here since nv.h is not
** included.  Note that this is protected in the event nv.h was
** actually included before the inclusion of err.h.  If the feature
** to decouple NVM from ERR is NOT asserted, then include nv.h as
** normal.  This fasciliates target environments which do not have
** NVM support.
*/
#if defined(ERR_DECOUPLE_NV)
  #ifndef nv_err_log_type

    #if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
    #endif

      typedef PACKED struct {
        byte      address;      /* Storage address 0 to       */
                                /*  ERR_MAX_LOG-1          */
        byte      err_count;    /* Number of occurances       */
                                /* (0=empty,FF=full)          */
        byte      file_name[8]; /* File name string */
        word      line_num;     /* Line number in file */
        boolean   fatal;        /* TRUE if fatal error */
      } nv_err_log_type;

    #if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
    #endif

   #endif
#else

/* Prototypes REX core dump feature
*/

#ifdef FEATURE_REX_CORE_DUMP
   void rex_log_core_dump( 
      uint16      line, 
      const char *filename, 
      const char *fmt, 
      uint32      p1, 
      uint32      p2, 
      uint32      p3 
   );
#endif



/*===========================================================================

FUNCTION ERR_CONVERT_TO_NV_LOG

DESCRIPTION
  This routine converts a err_log_type structure to a byte-aligned
  nv_err_log_type.

RETURN VALUE
  Boolean indicating success.

===========================================================================*/
boolean err_convert_to_nv_log(
  err_log_type    *err_log_ptr,
  nv_err_log_type *nv_log_ptr
)
{
  boolean return_val = TRUE;

  if (err_log_ptr && nv_log_ptr) {

    nv_log_ptr->address = err_log_ptr->address;
    nv_log_ptr->err_count = err_log_ptr->err_count;

    memcpy((void *) nv_log_ptr->file_name,
           err_log_ptr->file_name,
           sizeof(nv_log_ptr->file_name));

    nv_log_ptr->line_num = err_log_ptr->line_num;
    nv_log_ptr->fatal = err_log_ptr->fatal;

  } else {
    return_val = FALSE;
  }

  return return_val;
}



/*===========================================================================

FUNCTION ERR_CONVERT_FROM_NV_LOG

DESCRIPTION
  This routine converts a byte-aligned nv_err_log_type structure to
  err_log_type.

RETURN VALUE
  Boolean indicating success.

===========================================================================*/
boolean err_convert_from_nv_log(
  nv_err_log_type *nv_log_ptr,
  err_log_type    *err_log_ptr
)
{
  boolean return_val = TRUE;

  if (err_log_ptr && nv_log_ptr) {

    err_log_ptr->address = nv_log_ptr->address;
    err_log_ptr->err_count = nv_log_ptr->err_count;

    std_strlcpy((char *)err_log_ptr->file_name,
           (char *) nv_log_ptr->file_name,
           sizeof(err_log_ptr->file_name));

    err_log_ptr->line_num = nv_log_ptr->line_num;
    err_log_ptr->fatal = nv_log_ptr->fatal;

  } else {
    return_val = FALSE;
  }

  return return_val;
}
#endif /* ERR_DECOUPLE_NV */


#if !defined(FEATURE_DIAG_NON_STREAMING) && !defined(FEATURE_DIAG_DISABLE_PANIC_MODE)
    /* Prototypes for externalized routines */
    extern void diag_enter_panic_mode (void);
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

void err_lcd_message( void )
{

  /* get message from shared memory */
  smem_diag_get_message( message, sizeof(message) );

  /* copy and display the message */
  lcd_message( message );

#ifdef FEATURE_AMSS_RESTART
  if(!process_restart_enabled())
#endif /* FEATURE_AMSS_RESTART */
  {
    /* Stop the world - we have an err fatal on modem side */
    (void)rex_msm_int_lock();

    /* wait forever */
    while (1)
    {
    }
  }
} /* err_lcd_message */

#endif /* IMAGE_APPS_PROC && FEATURE_ERR_LCD */


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

void err_init (void)
{

  byte              i;
#if defined (IMAGE_MODEM_PROC) && defined (FEATURE_ERR_DEBUG_HANDLER)
  nv_err_fatal_options_type option;
#endif
  
#if !defined(ERR_DECOUPLE_NV)
  nv_stat_enum_type status;
  nv_err_log_type   nv_log;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV is built then read in from NV all error logs. */

  if (nv_built()) {

    for (i = 0; i < ERR_MAX_LOG; i++) {
      err_cache[i].address = i;
      err_cache[i].err_count = 0;
      nv_log.address = i;
      nv_log.err_count = 0;

      status = nv_read_err_log(&nv_log);

      /* Check NV status, convert to cache format, and update logged count.
      */
      if (status == NV_DONE_S) {
        if (err_convert_from_nv_log(&nv_log, &err_cache[i])) {
          if (err_cache[i].err_count > 0) {
            err_logged_count++;
          }
        } else {
          MSG_LOW("Err cache conversion failure", 0, 0, 0);
        }
      } else {
        MSG_LOW("Nv err read failure", 0, 0, 0);
      }
    }

    /* We have been initailized and are online from now on. */
    err_online = TRUE;
  }
#else

  for (i = 0; i < ERR_MAX_LOG; i++) {
    err_cache[i].address = i;
    err_cache[i].err_count = 0;
  }

  /* We have been initailized and are online from now on. */
  err_online = TRUE;

#endif /* ERR_DECOUPLE_NV */

  MSG_LOW("Err service initialized.  Online: %d", err_online, 0, 0);

#ifdef FEATURE_HW_GET_WATCHDOG_RESET

  /* The following code checks the RESET_STATUS register to determine if
  ** the phone was reset due to the HW watchdog timer. If so, a fatal
  ** error entry is written into NV.
  */

  if (hw_get_watchdog_reset_status() == TRUE) {
    if (err_online && err_log_to_fs) {
      err_log_store(0, "HWdogRst", TRUE);
    }
  }
#endif

#if defined (FEATURE_L4) && defined (FEATURE_ERR_EXTENDED_STORE)
#error code not present
#endif
  
  /* Initialize the err_auto_action options
   * 1st set compile-time defaults, then override with NV settings (if present)
   */

#if defined (FEATURE_AUTO_DLOAD_ON_ERROR)
  err_set_auto_action(ERR_DLOAD);
#elif defined (FEATURE_AUTO_RESET_ON_ERROR)
  err_set_auto_action(ERR_RESET);
#else
  err_set_auto_action(ERR_NO_ACTION);
#endif

#if defined (IMAGE_MODEM_PROC) && defined (FEATURE_ERR_DEBUG_HANDLER)

  /* Advanced ERR_FATAL automation settings for 7k processors using
   * FEATURE_ERR_DEBUG_HANDLER. Used by ARM 9 error handler. */
  
  err_set_timeout_option(ERR_IMMEDIATE);

  /* NOTE: Implementation currently assumes err_init is called from the
   * tmc_task, as it uses the tmc_tasks nv read function. If the function
   * is being called from another task then the code should be updated
   * accordingly.*/
  
  status = tmcnv_read (NV_ERR_FATAL_OPTIONS_I, (nv_item_type *) &option);

  if (status == NV_DONE_S)
  {
    /* NOTE on ERR_FATAL_OPTIONS NV item:
     * For the code below, interpretation of the 16-bit NV item is as follows:
     *
     * Bits 15:8 -- Timeout value (in seconds)
     * Bits 7:4  -- Timeout option
     * Bits 3:0  -- Auto action
     *
     * Builds not using the time fields will be unaffected as the auto-action
     * field is unchanged from previous usage.
     */
    
    err_set_auto_action(GET_ACTION(option.option));
    err_set_timeout_option(GET_TIMEOUT_OPTION(option.option));
    err_set_timeout_value(GET_TIMEOUT_VALUE(option.option));    
  }
#endif /* FEATURE_ERR_DEBUG_HANDLER && IMAGE_MODEM_PROC */

} /* err_init */


/*===========================================================================

FUNCTION ERR_LOGGED

DESCRIPTION
  Return to the caller the number of unique errors logged.

DEPENDENCIES
  err_init must be called before this call, otherwise the
  value returned is zero.

RETURN VALUE
  The number of unique errors logged.

SIDE EFFECTS
  None.

===========================================================================*/

unsigned int err_logged (void)
{
  return (err_logged_count);
}


/*===========================================================================

FUNCTION ERR_IGNORED

DESCRIPTION
  Return to the caller the total number of errors ignored.

DEPENDENCIES
  err_init must be called before this call otherwise the value
  returned is zero.

RETURN VALUE
  The total number of errors ignored because NV error log was full.

SIDE EFFECTS
  None.

===========================================================================*/

unsigned int err_ignored (void)
{
  return (err_ignored_count);
}


/*===========================================================================

FUNCTION ERR_GET_LOG

DESCRIPTION
  Copy to the caller's pointer a specific error record in the error
  log cache.  The error record is identified by its number, from 0 to
  ERR_MAX_LOG.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error log existed.
  ERR_NONE_S    - No such log number or operation could not be performed.

SIDE EFFECTS
  None.

===========================================================================*/

#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE
err_stat_type err_get_log_ex
#else
err_stat_type err_get_log
#endif
(
  byte         log_num,  /* The error log number */
  err_log_type *log_ptr  /* Where to copy the log */
)
{

  /* If the log is within range and if we have initialized then  */
  /* copy the log to the caller's buffer and return call status. */

  if ((err_online) && (log_num < ERR_MAX_LOG)) {
    *log_ptr = err_cache[log_num];
    return (ERR_DONE_S);
  } else {
    return (ERR_NONE_S);
  }
} /* err_get_log */


/*===========================================================================

FUNCTION ERR_CLR_LOG

DESCRIPTION
  This function clears a specific error log both in cache and in NV.
  An error log is cleared simply by clearing the error count field
  and fatal-flag.

  If 'log_num' == 0xFF, all logs will be cleared.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error record existed and cleared, or the
                  error did not exists (but OK status is still returned).
  ERR_NONE_S    - Error record out of range or clear to NV has failed.

SIDE EFFECTS
  The specified error log is cleared in NV and in cache and is now
  available to accept a new unique error.

===========================================================================*/

err_stat_type err_clr_log (
  byte  log_num
)
{
  unsigned int idx = 0;
  err_stat_type  status;    /* Function return status */

#if !defined(ERR_DECOUPLE_NV)
  err_log_type    err_save; /* Buffer to temporarily save error record */
  nv_err_log_type nv_log;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (log_num == 0xFF) {
    /* Clear all logs */

    status = ERR_DONE_S;

    /* Recursively clear all error logs */
    for(idx = 0; idx < ERR_MAX_LOG; idx++) {
      if (err_clr_log((byte)idx) == ERR_NONE_S) {
        /* Report failure even if only one fails.
        */
        status = ERR_NONE_S;
      }
    }
  } else {

    /* If we are initialized and in range then check the error */
    /* record.  If it is not empty then clear the error record */
    /* cache and NV, decrement error count and return status.  */

    status = ERR_NONE_S;
    if (err_online && err_log_to_fs && (log_num < ERR_MAX_LOG)) {

      status = ERR_DONE_S;
      if (err_cache[log_num].err_count != 0) {
#if !defined(ERR_DECOUPLE_NV)
        err_save.err_count             = err_cache[log_num].err_count;
        (void) std_strlcpy((char *)err_save.file_name,
                       (char *)err_cache[log_num].file_name,
                       ERR_FILE_NAME_LEN);
        err_save.line_num              = err_cache[log_num].line_num;
        err_save.fatal                 = err_cache[log_num].fatal;
#endif
        err_cache[log_num].err_count   = 0;
        (void) std_strlcpy((char *)err_cache[log_num].file_name,
                       (const char *)"",
                       ERR_FILE_NAME_LEN);
        err_cache[log_num].line_num    = 0;
        err_cache[log_num].fatal       = FALSE;

#if !defined(ERR_DECOUPLE_NV)
        /* Convert cache to PACKED NV type.
        */
        (void) err_convert_to_nv_log(&err_cache[log_num], &nv_log);

        if (nv_write_err_log(&nv_log) == NV_DONE_S) {
          err_logged_count--;

        /* If we could not write successfully to NV then return */
        /* the cache record to its previous state and exit      */
        /* with a fail status.  This is done in the hope        */
        /* that the cache and NV will be in the same state.     */
        } else {
          err_cache[log_num].err_count = err_save.err_count;
          (void) std_strlcpy((char *)err_cache[log_num].file_name,
                         (char *)err_save.file_name,
                         ERR_FILE_NAME_LEN);
          err_cache[log_num].line_num  = err_save.line_num;
          err_cache[log_num].fatal     = err_save.fatal;
          status = ERR_NONE_S;
        }
#endif /* ERR_DECOUPLE_NV */
      }
    }
  }

  return status;
} /* err_clr_log */



/*===========================================================================

FUNCTION ERR_LOG_STORE

DESCRIPTION
  Store a new error record to cache and NV.  If there is room for the
  new record then it is stored and the error count incremented.  If
  there is no room then the record is discarded and the errors ignored
  count is incremented.  If an error log of exact same type (file code
  and line number) already exists then we just increment the error count
  of that log.

DEPENDENCIES
  This internal function assumes that the caller has checked that we
  have been initialized before it is called.

RETURN VALUE
  The return value allows the caller to determine what action should be
  taken after trying to log the error.  The returned value is a boolean:
    TRUE  - This error record was created or incremented and is below
            threshold.
    FALSE - This error record was not created nor incremented, or it has
            been incremented but it had now exceeded threshold.

SIDE EFFECTS
  NV and cache are updated if there was room for new record or if this
  was an existing record whose error count could be incremented.

===========================================================================*/
LOCAL boolean err_log_store (
  word        line,              /* Comes from __LINE__ */
  const char *file_ptr,          /* Comes from __FILE__ */
  boolean     fatal              /* TRUE if fatal error */
)
{
  boolean   found;              /* Error record found flag */
  boolean   wrote;              /* NV written OK flag */
  boolean   ignore;             /* Error record ignore flag */
  word      i, index;           /* Working variables */
  word      non_fatal_index;    /* First non_fatal log in buffer */
  byte      file_buff[ERR_FILE_NAME_LEN];  /* File name buffer */
  dword     isave;              /* Saved interrupt data */

#if !defined(ERR_DECOUPLE_NV)
  nv_err_log_type nv_log;
#endif
 
  memset(file_buff,0,ERR_FILE_NAME_LEN);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy file name to local buffer, strip off the */
  /* file extension and replace it with nulls.     */

  /* NOTE on file_buff[] -- As defined in the ICD, an "Error Record Retrieval
   * Response Message" contains an 8 byte field for the file name.  This is 
   * NOT described as a NULL terminated string.  It is just an eight byte
   * field, where long file names are "truncated to the first eight
   * characters".  As such, file_buff[] is not necessarrily NULL terminated.
   */
  
  for (i = 0; (i < ERR_FILE_NAME_LEN) && (file_ptr[i] != '\0')
           && (file_ptr[i] != '.'); i++)
  {
    file_buff[i] = (byte) file_ptr[i];
  }
  for (; i < ERR_FILE_NAME_LEN; i++) {
    file_buff[i] = '\0';
  }

  /* If there is an existing error record with the same file name    */
  /* and line number then increment the error count of that record.  */
  /* Increment only up to saturation point (255).  Note also that    */
  /* we increment even if we pass ERR_NV_WRITE_THRESHOLD as we want  */
  /* to track number of times error has occured for this error type. */

  index           = 0x00FF;
  non_fatal_index = 0x00FF;
  found           = FALSE;
  ignore          = FALSE;

  INTLOCK_SAV(isave);      /* Disable interrupts */
  for (i = 0; i < ERR_MAX_LOG; i++) {

    /* If we find an empty log, record its position so we don't have to
       loop again later. */

    if (non_fatal_index == 0x00FF && err_cache[i].fatal == FALSE)
      non_fatal_index = i; /* In case we need to overwrite a non-fatal*/

    if (err_cache[i].err_count == 0) {
      if (index == 0x00FF)
        index = i;     /* Save first open entry for new log */

    }
    else if ((err_cache[i].line_num == line) && 
             (strncmp((const char *)err_cache[i].file_name,
                      (const char *)file_buff, ERR_FILE_NAME_LEN) == 0)) {
      if (err_cache[i].err_count == 255) {
        ignore = TRUE;
      } else {
        err_cache[i].err_count++;
      }
      found = TRUE;
      index = i;
      break;
    }
  }

  /* Otherwise find the first empty error record, update all the   */
  /* record entries, and increment the unique errors logged count. */
  /* If this is a fatal error, and there are no empty logs,        */
  /* overwrite a non-fatal log                                     */

  if (!found) {

    /* If fatal and no open logs, overwrite a non-fatal log */
    if ((index != 0x00FF) || (fatal && non_fatal_index != 0x00FF)) {

      /* If replacing a non-fatal err - set index */
      if (index == 0x00FF) index = non_fatal_index;

      /* Check to make sure that we are not accessing out of bound */
      if (index < ERR_MAX_LOG)
      {
        (void) std_strlcpy((char *)err_cache[index].file_name,
                     (const char *)file_buff, ERR_FILE_NAME_LEN);

        err_cache[index].line_num = line;
        err_cache[index].fatal = fatal;
        err_cache[index].err_count = 1;
        err_logged_count++;
        found = TRUE;
      }
      else
        found = FALSE;
    }
  }
  INTFREE_SAV(isave);     /* Reenable interrupts */


  /* If we found the error in the log or if we found room  */
  /* for new error then write the record to NV.  If write  */
  /* failed then increment errors ignored error count and  */
  /* set status.  If there is no empty record then we just */
  /* throw away this error and we increment the count of   */
  /* errors ignored.                                       */

  wrote = TRUE;
  if ((found) && (!(ignore))) {
#if !defined(ERR_DECOUPLE_NV)
    if (TASKS_ARE_LOCKED())
    { 
      MSG_ERROR("ERR failed to write to NV",0,0,0);
      wrote = FALSE;
    }
    else
    {
      /* Convert cache to PACKED NV type.
      */
      if (index < ERR_MAX_LOG) {
        (void)err_convert_to_nv_log(&err_cache[index], &nv_log);
      }
      if (nv_write_err_log(&nv_log) != NV_DONE_S) {
        INC_SAT(err_ignored_count);
        wrote = FALSE;
      }
    }

#endif
  } else {
    INC_SAT(err_ignored_count);
  }

  /* If we found the error record or created a new one and if we wrote   */
  /* the error record successfully to NV and if the error count is less  */
  /* then the threshold, then we return TRUE.  If we could not find room */
  /* for the new record or if NV write failed or if the current record   */
  /* error count exceeds the threshold then we return FALSE.             */

  if ((found) && (wrote) && (index < ERR_MAX_LOG) &&
      (err_cache[index].err_count <= ERR_NV_WRITE_THRESHOLD)) {
    return (TRUE);
  } else {
    return (FALSE);
  }
} /* err_log_store */


/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR(...) macro.  It then calls
  err_log_store to store the error.  Since this is a non fatal error
  we do not bother to check if the error could be logged or not.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/

void err_put_log (
  word   line,                /* from __LINE__ */
  const char   *file_ptr      /* From __FILE__ */
)
{
  if (err_online && err_log_to_fs) {
    (void) err_log_store(line, file_ptr, FALSE);
  }
}


/*===========================================================================

FUNCTION ERROR_FATAL_PUT_INTERNAL

DESCRIPTION
  This function can be invoked from the err_fatal_put_log() and 
  err_fatal_put_report().  First and immediately this function kills the 
  PA.  Next an attempt is made to log the error to NV.

  If the phone is a Portable,and if this function is called from 
  err_fatal_put_log() the file/line are posted to the LCD by the lcd_message(),
  where as if this function is called from  err_fatal_put_report() the 
  format message is posted to the LCD by the lcd_message(). 
  Lcd_message() branches to the fatal error handling logic in boot.

  If the phone is a mobile, and the error could not be logged to NV, the
  phone is powered down, otherwise it is reset.  The purpose of this is to
  let the DMSS reset upon fatal error but only up to a certain threshold.
  After this threshold the unit powers down, to avoid continual reset
  cycle, possibly surpassing the NV write cycle limit.

DEPENDENCIES
  err_init must have been called first, otherwise we reset but we
  do not log anything to NV.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.
  **************************************************************
  ************ THERE IS NO RETURN FROM THIS FUNCTION ***********
  **************************************************************

===========================================================================*/

void err_fatal_put_internal (
  word       line,      /* From __LINE__ */
  const char *file_ptr,  /* From __FILE__ */
  const char *fmt_ptr,   /* Format string */
  err_fatal_enum_type err_fatal_type  
)
{
#if !defined(_disable)
  extern void _disable(void);
#endif

#if defined(FEATURE_ERR_LCD)
  int i;
  int len;
  int msg_line;
#endif /* FEATURE_ERR_LCD */

  static boolean inprogress = FALSE;

#if defined(FEATURE_MULTIPROCESSOR)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */
  
#ifndef FEATURE_WINCE
  /*  boot routine that clears the stack limit (SL) register. */
  boot_clear_sl();

#endif
#ifdef FEATURE_SMEM_LOG
  {
    uint32 name[5];
    uint32 name_len;
    rex_tcb_type *self;

    self = rex_self();
    name_len = MIN( sizeof(name), sizeof(self->task_name) );
    memcpy (name, self->task_name, name_len);
    ERR_LOG_EVENT6 (ERR_ERROR_FATAL_TASK,
                    name[0], name[1], name[2], name[3], name[4], (uint32) self);

    name_len = 1 + strlen(file_ptr);
    memcpy (name, file_ptr, MIN(sizeof(name), name_len));
    ERR_LOG_EVENT6 (ERR_ERROR_FATAL,
                    name[0], name[1], name[2], name[3], name[4], line);
  }
#endif /* FEATURE_SMEM_LOG */

  if (!inprogress)
  {
    inprogress = TRUE;
#if defined(FEATURE_MULTIPROCESSOR)    
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */
    
#if defined(FEATURE_WCDMA_PLT)
#error code not present
#endif /* FEATURE_WCDMA_PLT */

#if !defined(ERR_DECOUPLE_ENC)
      /* Kill the PA first.  If we have not been initialized then   */
      /* just reset.  If we have been initialized then try to store */
      /* the error record.  If the the error record store returns   */
      /* TRUE then we cause reset, else we cause powerdown.         */
      enc_kill_pa();
#endif /* ERR_DECOUPLE_ENC */

      
#ifdef FEATURE_ERR_EXTENDED_STORE
      /* lock interrupts while saving data.  We don't want any of the */
      /* memory to be changing while it's being dumped */
      INTLOCK();
      err_store_info(file_ptr, line);

  #ifndef FEATURE_MULTIPROCESSOR
      /* Send file_ptr and line # to boot code so that it can be  */
      /* displayed later when diag_panic_end() is finally called. */
      /* At that time the user may reset the phone to log the error. */
      boot_record_err_fatal_info(line, file_ptr);
  #endif /* FEATURE_MULTIPROCESSOR */
      
      INTFREE();
#endif /* FEATURE_ERR_EXTENDED_STORE */

      if (err_online && err_log_to_fs  &&
          !TASKS_ARE_LOCKED() && !rex_is_in_irq_mode())
        (void) err_log_store(line, file_ptr, TRUE);

#ifdef USES_RELEASE_VERSION
	hw_reset();
#endif

#ifdef FEATURE_AUTO_RESET
      /* No looping exception handler if auto-reset is enabled.
        Reset immediately */
      boot_auto_reset();
#endif /* FEATURE_AUTO_RESET */
      
#ifdef FEATURE_ERR_LCD 
      /* Copy the filename to the LCD message buffer, max HS_SCRN_WIDE */

      if(err_fatal_type == ERR_FATAL_REPORT_C)
      {
        /* Copy the format pointer, max HS_SCRN_WIDE * HS_SCRN_HIGH */ 
        for (i = 0; i < (HS_SCRN_WIDE * HS_SCRN_HIGH) && fmt_ptr[i] != '\0'; i++)
          message[i] = fmt_ptr[i]; 
        /* padding done below for this case and else */
      }
      else
      {
        #if defined(IMAGE_APPS_PROC)
          memcpy( message, "APP ", 4);
          i = 4;
        #elif defined(IMAGE_MODEM_PROC)
          memcpy( message, "MOD ", 4);
          i = 4;
        #else
          i = 0;
        #endif /* if IMAGE_APPS_PROC elseif IMAGE_MODEM_PROC */

        /* Copy as much of the file name as possible,
           but leave at least 1 text line for displaying the line number */
        len = strlen( file_ptr );
        if(len > (HS_SCRN_HIGH - 1) * HS_SCRN_WIDE - i )
        {
          len = (HS_SCRN_HIGH - 1) * HS_SCRN_WIDE - i;
        }

        memcpy( &message[i], file_ptr, len );

        i += len;
        msg_line = (i + HS_SCRN_WIDE - 1)/HS_SCRN_WIDE;

        /* Pad the rest of the line with spaces */
        for ( ; i < msg_line * HS_SCRN_WIDE; i++)
          message[i] = ' ';
      

        /* Print a line number on the next line - there's always at least 1 */
        for (i += ERR_LINENUM_DIGITS-1; i >= msg_line * HS_SCRN_WIDE; i--)
        {
          message[ i ] = '0' + (line % 10);
          line /= 10;
        }
      
        /* Set up i to pad the message with spaces */
        i = msg_line * HS_SCRN_WIDE + ERR_LINENUM_DIGITS;      
      }
      
      /* Pad the message with spaces */
      for ( ; i < (HS_SCRN_HIGH * HS_SCRN_WIDE); i++)
      {
        message[i] = ' ';  
      }
      
      message[i] = '\0';            /* NUL terminate the string */

      #ifndef FEATURE_MULTIPROCESSOR
      lcd_message( message );
      #endif /* FEATURE_MULTIPROCESSOR */
      
#else /* else for ifdef FEATURE_ERR_LCD */

      (void) fmt_ptr; /* to supress compiler warning */
      (void) err_fatal_type; /* to supress compiler warning */

#endif /* FEATURE_ERR_LCD */
  }

#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMSM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && FEATURE_SMSM */
  
  #if !defined(FEATURE_DIAG_NON_STREAMING) && \
      !defined(FEATURE_DIAG_DISABLE_PANIC_MODE)
    /* Enter panic mode in diag.  It will flush its queues and shut down. */
    diag_enter_panic_mode (); /* no return */
  #endif

  #if defined(FEATURE_ERR_EXTENDED_STORE) && !(defined FEATURE_MULTIPROCESSOR)
    /* Displays error message, and allows user to reset phone manually */
    /* No return from this function call */
    boot_generic_error_handler();
  #endif

/*lint -save -e{527,716} */
  (void)rex_msm_int_lock();
  while (1)
  {
    dog_kick();
  }
/*lint -restore */

} /* err_fatal_put_internal */

/*===========================================================================

FUNCTION ERROR_FATAL_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR_FATAL(...) macro. This 
  function calls err_fatal_put_internal()

  The file name and line number are posted to the LCD by the 
  lcd_message().

DEPENDENCIES
  err_init must have been called first, otherwise we reset but we
  do not log anything to NV.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/
void err_fatal_put_log (
  word       line,      /* From __LINE__ */
  const char *file_ptr  /* From __FILE__ */
)
{

  err_fatal_put_internal (line, 
                          file_ptr,
                          NULL,
                          ERR_FATAL_C);


} /* err_fatal_put_log */

/*===========================================================================

FUNCTION ERROR_FATAL_PUT_REPORT

DESCRIPTION
  This function is invoked from the ERR_FATAL_REPORT(...) macro. This 
  function calls err_fatal_put_internal()

  The format message is posted to the LCD by the lcd_message(). 

DEPENDENCIES
  err_init must have been called first, otherwise we reset but we
  do not log anything to NV.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.
===========================================================================*/
void err_fatal_put_report (
  unsigned int    line,      /* From __LINE__ */
  const char *file_ptr, /* From __FILE__ */
  const char *fmt_ptr   /* format string */
)
{
#if defined (FEATURE_REX_CORE_DUMP)  
  #if !defined FEATURE_L4
    rex_log_core_dump((uint16) line, file_ptr, fmt_ptr, 0, 0, 0);
  #endif
#endif
  
#ifndef FEATURE_DIAG_DEBUG_6275
#if defined(FEATURE_ERR_EXTENDED_STORE) && !(defined FEATURE_MULTIPROCESSOR)
  if(boot_exception_handler_is_running())
  {
    /* We're already processing some type of error -- just log this ERR_FATAL,
     * and call boot handler directly.  No additional processing should be done.
     * Interrupts should already be disabled if the exception handler is already running.
     */
    err_store_info(file_ptr, line);
    boot_record_err_fatal_info((word)line, file_ptr);
    boot_generic_error_handler();
  }
  
#endif /* FEATURE_ERR_EXTENDED_STORE */
#endif /* FEATURE_DIAG_DEBUG_6275 */

  err_fatal_put_internal ((word)line, 
                          file_ptr,
                          fmt_ptr,
                          ERR_FATAL_REPORT_C);
} /* err_fatal_put_report */



/*===========================================================================

FUNCTION ERR_READ_DIAGPKT
DESCRIPTION
  This procedure processes a request to read accumulated error
  information.

============================================================================*/
PACKED void * err_read_diagpkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  byte i;            /* loop index */
  err_log_type err_log;
  DIAG_ERR_READ_F_rsp_type *rsp_ptr;
  const unsigned int rsp_len = sizeof(DIAG_ERR_READ_F_rsp_type);

  (void) req_pkt_ptr; /* suppress compiler warning */
  (void) pkt_len;     /* suppress compiler warning */

  rsp_ptr = (DIAG_ERR_READ_F_rsp_type *) diagpkt_alloc(DIAG_ERR_READ_F, rsp_len);

  /*------------------------------------------------------------------------
    Header ok, get the error info. Give the number accumulated and the
    number ignored, then the error records.
  ------------------------------------------------------------------------*/

  rsp_ptr->log_cnt = (word)err_logged();
  rsp_ptr->ignore_cnt = (word)err_ignored();

  for ( i = 0; i < ERR_MAX_LOG; i++ ) {
    (void) err_get_log ( i, &err_log );

    rsp_ptr->err_logs[i].address = err_log.address;
    rsp_ptr->err_logs[i].err_count = err_log.err_count;

    (void) std_strlcpy((char *)rsp_ptr->err_logs[i].file_name,
                       (char *)err_log.file_name,
                       ERR_FILE_NAME_LEN);
    
    rsp_ptr->err_logs[i].line_num = err_log.line_num;
    rsp_ptr->err_logs[i].fatal = err_log.fatal;

  }

  return (rsp_ptr);

} /* err_read_diagpkt */



/*===========================================================================

FUNCTION ERR_CLEAR_DIAGPKT
DESCRIPTION
  This procedure processes a request to clear an error record (or all
  records).

============================================================================*/
PACKED void * err_clear_diagpkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  byte i;                     /* loop index */
  DIAG_ERR_CLEAR_F_req_type *req_ptr = (DIAG_ERR_CLEAR_F_req_type *) req_pkt_ptr;
  DIAG_ERR_CLEAR_F_rsp_type *rsp_ptr;
  const unsigned int rsp_len = sizeof(DIAG_ERR_CLEAR_F_rsp_type);

  if (req_ptr->rec > ERR_MAX_LOG && req_ptr->rec != 0xff )
  {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len) );
  }

  /*------------------------------------------------------------------------
    Header ok, get the error info. Clear the records, then send back a copy
    of the request packet.  If the record index is -1 ( 0xFF ), then
    clear the entire set of records, otherwise just do the index specified.
  ------------------------------------------------------------------------*/
  if (req_ptr->rec == 0xFF ) {
    for (i=0; i < ERR_MAX_LOG; i++ ) {
      (void) err_clr_log ( i );
    }
  } else {
    (void) err_clr_log ( req_ptr->rec );
  }

  rsp_ptr = (DIAG_ERR_CLEAR_F_rsp_type *) diagpkt_alloc(DIAG_ERR_CLEAR_F, rsp_len);

  *rsp_ptr = *req_ptr;

  return (rsp_ptr);

} /* err_clear_diagpkt */


/*===========================================================================

FUNCTION ERR_ENABLE_FS_LOG
DESCRIPTION
  Enables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_enable_fs_log (void)
{
  err_log_to_fs = TRUE;
} /* err_enable_fs_log */


/*===========================================================================

FUNCTION ERR_DISABLE_FS_LOG
DESCRIPTION
  Disables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_disable_fs_log (void)
{
  err_log_to_fs = FALSE;
} /* err_disable_fs_log */


/*===========================================================================

FUNCTION ERR_FATAL_CORE_DUMP
DESCRIPTION
  Logs fatal error information, including a core dump.

  NOTE: There is no return from this function.
============================================================================*/
void err_fatal_core_dump (
  unsigned int line,      /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format   /* format string */
)                         
{
  #ifdef FEATURE_REX_CORE_DUMP
    #if !defined FEATURE_L4
      rex_log_core_dump((uint16) line, file_name, format, 0, 0, 0);
    #endif
  #endif

  err_fatal_put_log ((word)line, file_name);
}


#ifdef FEATURE_ERR_EXTENDED_STORE
/*===========================================================================

FUNCTION ERR_DATA_IS_VALID
DESCRIPTION
  Returns TRUE is any error has been logged, FALSE is none have been logged
============================================================================*/
boolean err_data_is_valid()
{
  int i;
  for(i=0; i<ERR_DATA_MAX_LOGS; i++)
  {
    if(err_data.err_log[i].flag == ERR_DATA_MAGIC_NUMBER)
      return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION ERR_STORE_REX_CORE_TO_BUF
DESCRIPTION
  Writes register settings stored in rex_core_ptr to dest_buf.
  Used for error logging.
============================================================================*/
uint32 err_store_rex_core_to_buf(
  LOG_COREDUMP_C_type *rex_core_ptr,
  char *dest_buf,
  uint32 dest_bufsize)
{
  int32 used_len = 0;
  int i;

  used_len = strlen(dest_buf);
  
  /* SVC mode registers.
   */
  for (i = REX_SVC_R0; i <= REX_SVC_R12; i++) {
    used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
      "r.s r%d 0x%lx\n", i, rex_core_ptr->regs[i]); 
    if (used_len < dest_bufsize)
      (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);
  }

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r13_svc 0x%lx\n", rex_core_ptr->regs[REX_SVC_SP]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r14_svc 0x%lx\n", rex_core_ptr->regs[REX_SVC_LR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s spsr_svc 0x%lx\n", rex_core_ptr->regs[REX_SVC_SPSR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* PC is after the SVC-mode SPSR in the rex_core structure.
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s pc 0x%lx\n", rex_core_ptr->regs[REX_SVC_PC]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* CPSR.
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s cpsr 0x%lx\n", rex_core_ptr->cpsr);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* User-mode registers
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r13_usr 0x%lx\n", rex_core_ptr->regs[REX_SYS_SP]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r14_usr 0x%lx\n", rex_core_ptr->regs[REX_SYS_LR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* IRQ-mode registers
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r13_irq 0x%lx\n", rex_core_ptr->regs[REX_IRQ_SP]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r14_irq 0x%lx\n", rex_core_ptr->regs[REX_IRQ_LR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s spsr_irq 0x%lx\n", rex_core_ptr->regs[REX_IRQ_SPSR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* Abort-mode registers
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r13_abt 0x%lx\n", rex_core_ptr->regs[REX_ABT_SP]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r14_abt 0x%lx\n", rex_core_ptr->regs[REX_ABT_LR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s spsr_abt 0x%lx\n", rex_core_ptr->regs[REX_ABT_SPSR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* Undef-mode registers
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r13_und 0x%lx\n", rex_core_ptr->regs[REX_UDF_SP]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r14_und 0x%lx\n", rex_core_ptr->regs[REX_UDF_LR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s spsr_und 0x%lx\n", rex_core_ptr->regs[REX_UDF_SPSR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* FIQ-mode registers. Note that r8-r12 are also banked.
   */
  for (i = 8; i <= 12; i++) {
    used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
      "r.s r%d_fiq 0x%lx\n", i, rex_core_ptr->regs[i+REX_FIQ_R8-8]); 
    if (used_len < dest_bufsize)
      (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);
  }

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r13_fiq 0x%lx\n", rex_core_ptr->regs[REX_FIQ_SP]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s r14_fiq 0x%lx\n", rex_core_ptr->regs[REX_FIQ_LR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
    "r.s spsr_fiq 0x%lx\n", rex_core_ptr->regs[REX_FIQ_SPSR]);
  if (used_len < dest_bufsize)
    (void) std_strlcat (dest_buf, temp_buf, dest_bufsize);

  /* Todo: add JAVA-mode registers here.
   */
  return used_len;
}

/*===========================================================================

FUNCTION ERR_STORE_INFO
DESCRIPTION
  Writes a bunch of information to a designated RAM buffer.
============================================================================*/
void err_store_info (const char *filename, unsigned int line)
{
  uint32 used_len = 0;
  int temp_len = 0, i;
  err_data_log_type *err_log_ptr = NULL;
  char *buf = NULL;
  unsigned int bufsize = ERR_DATA_MAX_SIZE;
#ifndef FEATURE_L4
  uint32 sp_temp;
  uint32 curr_cpsr;
#endif
  
  /* With an L4 build, the err_data buffer management is handled by
   * the QPMD server */
#ifndef FEATURE_L4
  /* If magic numbers are present at the front of the buffer, then previous
   * data has not been written out yet. Return without doing anything in
   * this case.
   */
  
  for(i=0; i<ERR_DATA_MAX_LOGS; i++)
  {
    if(err_data.err_log[i].flag != ERR_DATA_MAGIC_NUMBER)
      break;
  }
  
  if (i >= ERR_DATA_MAX_LOGS)
  {
    return;
  }
  
  err_log_ptr = &err_data.err_log[i];
  buf = (char *) err_log_ptr->data;  
  
  /* Interrupts could be disablesd at this point -- need to make sure we 
   * don't timeout.
   */
  dog_kick();
  
  err_log_ptr->flag = ERR_DATA_MAGIC_NUMBER;
  (void) memset (buf, 0x00, bufsize);

  err_log_ptr->length = bufsize;

  /* 
   * Grab PC and SP of our current location. 
   * We only need to do this if we are still in the same processor 
   * mode that we were when rex_jettison_core was called, unless
   * err_reset_flag is set (resets are a special case, and the SP and
   * PC are set elsewhere)
   */
  curr_cpsr = rex_get_cpsr();
  if(((uint32)(PSR_Mode_Mask & rex_core.cpsr) == (uint32)(PSR_Mode_Mask & curr_cpsr))
     && (!err_reset_flag))
  {
    rex_core.regs[REX_SVC_PC] = rex_get_pc ();
    sp_temp = rex_get_sp ();
    switch(rex_core.cpsr & PSR_Mode_Mask)
    {
      case PSR_Supervisor:
        rex_core.regs[REX_SVC_SP] = sp_temp;
        break;
      case PSR_Abort:
        rex_core.regs[REX_ABT_SP] = sp_temp;
        break;
      case PSR_User:
      case PSR_System:
        rex_core.regs[REX_SYS_SP] = sp_temp;
        break;
      case PSR_Irq:
        rex_core.regs[REX_IRQ_SP] = sp_temp;
        break;
      case PSR_Undef:
        rex_core.regs[REX_UDF_SP] = sp_temp;
        break;
      case PSR_Fiq:
        rex_core.regs[REX_FIQ_SP] = sp_temp;
        break;              
      default:
        break;
    }
  }
#else
#error code not present
#endif /* ! FEATURE_L4 */

  /* Version number and build ID.
   */
#ifndef FEATURE_L4

    used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
      "%s%d\n", ver, EXT_STORE_VERSION);
    if (used_len >= bufsize)
      return;
    (void) std_strlcat (buf, temp_buf, bufsize);
    
    used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
      "%s%s\n", build_id, mob_sw_rev);
    if (used_len >= bufsize)
      return;
    (void) std_strlcat (buf, temp_buf, bufsize);
    
    /* Filename and line number of error.
     */
    used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf), 
      "%s%d\n", err_line, line);
    if (used_len >= bufsize)
      return;
    (void) std_strlcat (buf, temp_buf, bufsize);
    
    used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf),
      "%s%s\n", err_file, filename);
    if (used_len >= bufsize)
      return;
    (void) std_strlcat (buf, temp_buf, bufsize);
    
#else
#error code not present
#endif

  // time_get_julian(&error_time);
  error_time_set = FALSE;
#ifndef FEATURE_L4
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf), 
                        "%s %02d/%02d/%04d %02d:%02d:%02d (M/D/Y H:M:S)\n",
                        time_stamp_str,error_time.month,error_time.day,
                        error_time.year,error_time.hour,error_time.minute,
                        error_time.second);
  if (used_len >= bufsize)
    return;
  (void) std_strlcat (buf, temp_buf, bufsize);
#else
#error code not present
#endif
  
#ifndef FEATURE_L4 
  /* Rex variables.
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf), 
                        "%s0x%p\n", curr_task, rex_self());
  if (used_len >= bufsize)
    return;
  (void) std_strlcat (buf, temp_buf, bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf), 
                        "%s0x%p\n", best_task, rex_best_task);
  if (used_len >= bufsize)
    return;
  (void) std_strlcat (buf, temp_buf, bufsize);

  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf), 
                        "%s%u\n", sched_allow,
    TASKS_ARE_LOCKED());
  if (used_len >= bufsize)
    return;
  (void) std_strlcat (buf, temp_buf, bufsize);

  (void) err_add_to_mem_table("rex_curr_task tcb", (void *)rex_self(), 
      sizeof(rex_tcb_type));
  
  /* Register dump.
   */
  used_len += (uint32)std_strlprintf (temp_buf, sizeof(temp_buf), 
                        "%s\n", "; Register Dump");
  if (used_len >= bufsize)
    return;
  (void) std_strlcat (buf, temp_buf, bufsize);

  used_len = err_store_rex_core_to_buf(&rex_core, buf, bufsize);
  if(used_len >= bufsize)
    return;
  
#else
#error code not present
#endif

#ifndef FEATURE_L4
  /* Data dumps. Before we do these, add a pointer to the current task's
   * stack into the mem_info array.
   */
  (void) err_add_to_mem_table ("rex_curr_task stack",(void *) (rex_self()->stack_limit),
    (unsigned long) (rex_self()->stack_size));

  for (i = 0; i < MEM_INFO_TABLE_LEN; i++)
  {
    if(mem_info[i].valid_flag == ERR_DATA_MEM_INFO_IS_VALID)
    {
      /* Could be copying large amounts of data here...need to ensure that
       * we don't cause a HW watchdog reset.
       */
      dog_kick();
      
      /* No good way to currently validate string pointer (likely in code space),
       * so we assume it's good if the flag is valid.
       */
      if(mem_info[i].name == NULL)
      {
        /* Leave off name if pointer is bad */
        temp_len = std_strlprintf (temp_buf, sizeof(temp_buf),
          "%s\n0x%p 0x%lx\n", data_dump_indicator,
          mem_info[i].address, mem_info[i].length);
      }
      else
      {
        temp_len = std_strlprintf (temp_buf, sizeof(temp_buf),
          "%s (%s)\n0x%p 0x%lx\n", data_dump_indicator, mem_info[i].name, 
          mem_info[i].address,mem_info[i].length);
      }
      
      if (used_len + (uint32)temp_len + mem_info[i].length >= bufsize)
        break;
      
      memcpy (buf + used_len, temp_buf, (uint32)temp_len);
      used_len += (uint32)temp_len;
      
      if(hw_valid_addr(mem_info[i].address,(word)mem_info[i].length))
      {
        /* Ensure that range is valid -- we don't want a data abort here */
        (void) memcpy (buf + used_len, mem_info[i].address,
                       mem_info[i].length);
        used_len += mem_info[i].length;
        buf[used_len] = '\n';
        used_len++;      
      }
      else
      {
        /* print error message if address+length is out of range */
        temp_len = std_strlprintf (temp_buf, sizeof(temp_buf),
          "Invalid range.\n");
        if (used_len + (uint32)temp_len >= bufsize)
          break;
        memcpy (buf + used_len, temp_buf, (uint32)temp_len);
        used_len += (uint32)temp_len;      
      }
    }
  }
    
  err_log_ptr->length = used_len;

#if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)
  if(err_reset_flag == FALSE)
  {
    if(n_count_times == 0)
    {
      err_print_BrewAppHeap() ; 
    }
    n_count_times++ ;
  }
#endif
#endif
#ifdef FEATURE_L4
#error code not present
#endif

#ifdef FEATURE_ERR_SAVE_CORE
  /* Dump full memory core if we haven't done it already */
  if(err_core_dump.store_to_flash != ERR_CORE_STORE_TO_FLASH_MAGIC)
  {
    err_save_ram_core_rle(filename,line);
  }
#endif /* FEATURE_ERR_SAVE_CORE */  
  
#ifdef FEATURE_L4
#error code not present
#endif
    
  return;
} /* err_store_info */

#ifndef FEATURE_MULTIPROCESSOR
/* Call after memory is accessible, but before RAM is initialized */
int err_reset_check()
{
#ifndef FEATURE_L4
  if(err_data.reset_flag == ERR_DATA_RESET_MAGIC_NUMBER)
  {
    /* Memory has already been initialized, so we have reset */

    /* Set this flag so that we don't alter the PC/SP information. */
    err_reset_flag = TRUE;
    err_store_info("Reset",0);
    err_reset_flag = FALSE;
    return 1;
  }

  err_data.reset_flag = ERR_DATA_RESET_MAGIC_NUMBER;
#endif /* !FEATURE_L4 */
  return 0;
}
#endif /* !FEATURE_MULTIPROCESSORR */

/*===========================================================================

FUNCTION ERR_ADD_TO_MEM_TABLE
DESCRIPTION
  Adds an entry to the table of addresses and lengths that will be stored
  on a fatal error or abort.
  Returns 1 on success and 0 on failure.
============================================================================*/
int err_add_to_mem_table (char *name, void *ptr, unsigned long length)
{
  int i;

  for (i = 0; i < MEM_INFO_TABLE_LEN; i++) {
    if (mem_info[i].valid_flag != ERR_DATA_MEM_INFO_IS_VALID) {
      mem_info[i].name     = name;
      mem_info[i].address  = ptr;
      mem_info[i].length   = length;
      mem_info[i].valid_flag = ERR_DATA_MEM_INFO_IS_VALID;
      return 1;
    }
  }

  return 0;
} /* err_add_to_mem_table */

/*===========================================================================

FUNCTION ERR_DELETE_FROM_MEM_TABLE
DESCRIPTION
  Deletes an entry from the table of addresses and lengths that will be stored
  on a fatal error or abort.
  Returns 1 on success and 0 on failure.
============================================================================*/
int err_delete_from_mem_table (void *ptr)
{
  int i;

  for (i = 0; i < MEM_INFO_TABLE_LEN; i++) {
    if (mem_info[i].address == ptr) {
      mem_info[i].name = NULL;
      mem_info[i].valid_flag = 0;
      mem_info[i].address = NULL;
      mem_info[i].length  = 0;
      return 1;
    }
  }

  return 0;
} /* err_delete_from_mem_table */


#ifdef FEATURE_ERR_SAVE_CORE

/*===========================================================================

FUNCTION ERR_STORE_RAM_TO_FILE
DESCRIPTION
  Helper function to write a file (char *filename) to EFS.
  "*base" contains a pointer to the start of the data to be written, and
  "length" contains the length of the file.

  Returns 0 on success and -1 on failure.
============================================================================*/
int err_store_ram_to_file(char *filename, void *base, uint32 length)
{
  int fs_handle;
  int bytes = 0;
  int total_bytes = 0;

  /* Sanity check to make sure we aren't writing a huge file to EFS */
  /* File size should not exceed size of err_core buffer */
  if(length > sizeof(err_core_dump))
    return -1;
  
  fs_handle = efs_open(filename, O_RDWR | O_CREAT | O_TRUNC, 0755);
  if(fs_handle == -1)
    return -1;
  
  while((total_bytes < length) && (bytes != -1)) {
    bytes = efs_write(fs_handle, (void *)((char *)base + total_bytes), (length - total_bytes));
    total_bytes += bytes;
  }
  
  efs_close(fs_handle);
  
  return 0;
}   


/*===========================================================================

FUNCTION ERR_RLE
DESCRIPTION
  Performs run length encoding on buffer.
  
  Paramaters:
  uint32 *src -- pointer to buffer to be encoded
  uint32 *dst -- pointer to buffer for encoded data
  uint32 srclen -- length of data to be encoded (in 32-bit words)
  uint32 maxdestlen -- maximum size of encoded data in dst buffer (in 32-bit words)

    Run length encoder mapping
    --------------------------
    RLE_ESCAPE                        ->  RLE_ESCAPE RLE_ESCAPED
    Repeated <word> with <run_length> ->  RLE_ESCAPE <non-zero run_length> <word>
    Non-repeated <word>               ->  <word>


  returns number of words used in dst
============================================================================*/
uint32 err_rle(uint32 *src, uint32 *dst, uint32 srclen, uint32 maxdstlen)
{
  uint32 dstlen = 0;
  uint32 i = 0;
  int run_length = 1;
  
  dog_kick();
    
  /* Keep looping until we run out of source or destination space */
  while((i < srclen) && ((dstlen + 4) < maxdstlen)) {
    
    /* XXX This code must only called from exception handlers */
    /* This takes several hundred ms -- we must kick watchdog */
    if((i & 0x1ffff) == 0) {
      dog_kick();
    }
    
    if(((i + 1) < srclen) && (src[i] == src[i + 1])) {
      /* increment run length */
      run_length++;
    }
    else {
      if(run_length > 3 || 
         (src[i] == RLE_ESCAPE && run_length > 1)) {
        /* Output run length */
        dst[dstlen++] = RLE_ESCAPE;
        dst[dstlen++] = run_length;
        dst[dstlen++] = src[i];
      } 
      else {
        /* Too short a run length to be worthwhile -- Output a word at a time */
        do {
          dst[dstlen++] = src[i];
          if(src[i] == RLE_ESCAPE) {
            dst[dstlen++] = RLE_ESCAPED;
          }
          run_length--;
        } while(run_length > 0);
      }
      run_length = 1;
    }
    i++;
  }
  
  return dstlen;
}       
 
/*===========================================================================

FUNCTION ERR_SAVE_RAM_CORE_RLE
DESCRIPTION
  Function called to encoded memory and store it into err_core_dump.
  This function will do the following:
    
    * copy contents of rex_core into err_core_dump structure
    * encode APP_RAM region into err_core_dump structure
    * encode BB_RAM region into err_core_dump structure
    * encode ZI_REGION region into err_core_dump structure
    * encode oem_heap region into err_core_dump structure            

  Dog is kicked inside the err_rle() function.

============================================================================*/
void err_save_ram_core_rle(const char *filename, unsigned int line)  
{
  if(err_core_dump.store_to_flash == ERR_CORE_STORE_TO_FLASH_MAGIC)
  {
    /* Don't overwrite the memory dump if it already exists */
    return;
  }
  
  /* Record values of rle buffer size, input size, and output size */
  /* (output size computed during encoding process) */
  err_core_dump.total_words = ERR_MAX_CORE_WORDS;
  
  err_core_dump.total_input_words = 
    ( (uint32) Image__APP_RAM__Length + 
      (uint32) Image__BB_RAM__Length + 
      (uint32) Image__ZI_REGION__ZI__Length +
      (uint32) oem_dynamic_heap_size) / sizeof(uint32);
  
  err_core_dump.total_output_words = 0;
  
  /* Save rex_core for later use */
  memcpy((void *) (&err_core_dump.rex_core), (void *) &rex_core, sizeof(rex_core));
  
  if(!error_time_set)
  {
    time_get_julian_type(&error_time);
    error_time_set = TRUE;
  }
  memcpy((void *) (&err_core_dump.error_time), (void *) &error_time, sizeof(error_time));
  
  /* Dump APP_RAM */
  err_core_dump.app_ram_core_ptr = err_core_dump.rle_core;
  err_core_dump.app_ram_core_len = err_rle((uint32 *) Image__APP_RAM__Base, 
                                           err_core_dump.app_ram_core_ptr, 
                                           (uint32) Image__APP_RAM__Length / sizeof(uint32), 
                                           ERR_MAX_CORE_WORDS);
  err_core_dump.total_output_words += err_core_dump.app_ram_core_len;
  
  /* Dump BB_RAM */
  err_core_dump.bb_ram_core_ptr = err_core_dump.app_ram_core_ptr + err_core_dump.app_ram_core_len;
  err_core_dump.bb_ram_core_len = err_rle((uint32 *) Image__BB_RAM__Base, 
                                          err_core_dump.bb_ram_core_ptr,
                                          (uint32) Image__BB_RAM__Length / sizeof(uint32), 
                                          ERR_MAX_CORE_WORDS - err_core_dump.total_output_words);
  err_core_dump.total_output_words += err_core_dump.bb_ram_core_len;
  
  /* Dump ZI_REGION */
  err_core_dump.zi_region_core_ptr = err_core_dump.bb_ram_core_ptr + err_core_dump.bb_ram_core_len;
  err_core_dump.zi_region_core_len = err_rle((uint32 *) Image__ZI_REGION__ZI__Base, 
                                             err_core_dump.zi_region_core_ptr,
                                             (uint32) Image__ZI_REGION__ZI__Length / sizeof(uint32), 
                                             ERR_MAX_CORE_WORDS - err_core_dump.total_output_words);
  err_core_dump.total_output_words += err_core_dump.zi_region_core_len;
  
#ifdef FEATURE_ERR_SAVE_CORE_OEM_HEAP
  /* Dump OEM_Heap */
  err_core_dump.oem_heap_core_ptr = err_core_dump.zi_region_core_ptr + err_core_dump.zi_region_core_len;
  err_core_dump.oem_heap_core_len = err_rle((uint32 *) oem_dynamic_heap_pointer, 
                                             err_core_dump.oem_heap_core_ptr,
                                             (uint32) oem_dynamic_heap_size / sizeof(uint32), 
                                             ERR_MAX_CORE_WORDS - err_core_dump.total_output_words);
  err_core_dump.total_output_words += err_core_dump.oem_heap_core_len;
#endif

  /* Update magic number so that core dump will be saved on reboot */
  err_core_dump.store_to_flash = ERR_CORE_STORE_TO_FLASH_MAGIC;
} 

/*===========================================================================

FUNCTION ERR_STORE_ARM_STATE_CMM
DESCRIPTION
  Creates the .cmm file that will load the ARM RAM core.  This file is created
  in software so that the offsets can be easily determined.

============================================================================*/
void err_store_arm_state_cmm (void) 
{
  /* reuse the rle_core since we're now done with it */
  char *buf = (char *) err_core_dump.rle_core;
  int bufsize = ERR_MAX_CORE_WORDS * sizeof(int);
  int used_len = 0;
  time_julian_type *time = &err_core_dump.error_time;

  /* Add statistical information */
  used_len += std_strlprintf (temp_buf, sizeof(temp_buf),
    "; Total size of rle buffer (in words): 0x%08lx\n",
    err_core_dump.total_words);
  if(used_len >= bufsize)
    return;
  std_strlcpy(buf, temp_buf, bufsize);

  used_len += std_strlprintf (temp_buf, sizeof(temp_buf),
    "; Total size of uncompressed memory (in words): 0x%08lx\n",
    err_core_dump.total_input_words);
  if(used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);
  
  used_len += std_strlprintf (temp_buf, sizeof(temp_buf),
    "; Total size of compressed memory (in words): 0x%08lx\n",
    err_core_dump.total_output_words);
  if(used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);
  
#ifdef FEATURE_PMIC_RTC
  used_len += std_strlprintf (temp_buf, sizeof(temp_buf),
    "%s %02d/%02d/%04d %02d:%02d:%02d (M/D/Y H:M:S)\n",
    time_stamp_str,time->month,time->day,time->year,
    time->hour,time->minute,time->second);
  if (used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);
#endif
  
  /* Restore the ARM RAM core dump */
  used_len += std_strlprintf (temp_buf, sizeof(temp_buf), 
    "data.load.binary arm_app_ram_dump.lst (0x%08p)--(0x%08p) /noclear\n",
    (void *) Image__APP_RAM__Base, 
    (void *)((dword) Image__APP_RAM__Base + (dword) Image__APP_RAM__Length));
  if(used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);
                       
  used_len += std_strlprintf (temp_buf, sizeof(temp_buf), 
    "data.load.binary arm_bb_ram_dump.lst (0x%08p)--(0x%08p) /noclear\n",
    (void *) Image__BB_RAM__Base,
    (void *)((dword) Image__BB_RAM__Base + (dword) Image__BB_RAM__Length));
  if(used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);

  used_len += std_strlprintf (temp_buf, sizeof(temp_buf), 
    "data.load.binary arm_zi_region_ram_dump.lst (0x%08p)--(0x%08p) /noclear\n", 
    (void *) Image__ZI_REGION__ZI__Base,
    (void *)((dword) Image__ZI_REGION__ZI__Base + (dword) Image__ZI_REGION__ZI__Length));
  if(used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);
  
#ifdef FEATURE_ERR_SAVE_CORE_OEM_HEAP  
  used_len += std_strlprintf (temp_buf, sizeof(temp_buf), 
    "data.load.binary arm_oem_heap_ram_dump.lst (0x%08p)--(0x%08p) /noclear\n", 
    (void *) oem_dynamic_heap_pointer,
    (void *)((dword)oem_dynamic_heap_pointer + (dword)oem_dynamic_heap_size));
  if(used_len >= bufsize)
    return;
  std_strlcat (buf, temp_buf, bufsize);  
#endif
          
  /* Load up rex_core for ease of use */
  used_len = err_store_rex_core_to_buf(&rex_core, buf, bufsize);
  if(used_len >= bufsize)
    return;

  err_store_ram_to_file(ARM_DATA_LOAD_FILENAME, (void *) buf, used_len);
}


/*===========================================================================

FUNCTION ERR_CORE_CLEANUP
DESCRIPTION
  Helper function to erase all files created by FEATURE_ERR_SAVE_CORE.
  This is done prior to creating a new set of log files.

============================================================================*/
void err_core_cleanup(void)
{
  fs_rsp_msg_type fs_rsp;
  
  /* This function should erase any files associated with FEATURE_ERR_SAVE_CORE */
  fs_remove(ARM_APP_RAM_FILENAME, NULL, &fs_rsp);
  fs_remove(ARM_BB_RAM_FILENAME, NULL, &fs_rsp);
  fs_remove(ARM_ZI_REGION_FILENAME, NULL, &fs_rsp);
#ifdef FEATURE_ERR_SAVE_CORE_OEM_HEAP    
  fs_remove(ARM_OEM_HEAP_REGION_FILENAME, NULL, &fs_rsp);
#endif  
#ifdef FEATURE_ERR_SAVE_CORE_MDSP_RAM
  fs_remove(MDSP_RAMA_FILENAME, NULL, &fs_rsp);
  fs_remove(MDSP_RAMB_FILENAME, NULL, &fs_rsp);
  fs_remove(MDSP_RAMC_FILENAME, NULL, &fs_rsp);
#endif  
  fs_remove(ARM_DATA_LOAD_FILENAME, NULL, &fs_rsp);
}

/*===========================================================================

FUNCTION ERR_STORE_RAM_CORE_TO_FLASH
DESCRIPTION
  Function to create err_core_dump files in EFS.  Files are created using data
  stored in err_core_dump structure.

============================================================================*/
void err_store_ram_core_to_flash (void) 
{   
  /* Note -- any additional files created here should also be added to the */
  /* err_store_cleanup() function */
  
    if(err_core_dump.store_to_flash != ERR_CORE_STORE_TO_FLASH_MAGIC) {
        return;
    }

    /* Just in case something goes wrong during the save to file, 
        don't keep the UE in a bad state */  
  err_core_dump.store_to_flash = 0;
  
  err_store_ram_to_file(ARM_APP_RAM_FILENAME, 
                        (void *) err_core_dump.app_ram_core_ptr,
                        err_core_dump.app_ram_core_len * sizeof(uint32));
  
  err_store_ram_to_file(ARM_BB_RAM_FILENAME, 
                        (void *) err_core_dump.bb_ram_core_ptr,
                        err_core_dump.bb_ram_core_len * sizeof(uint32));
  
  err_store_ram_to_file(ARM_ZI_REGION_FILENAME, 
                        (void *) err_core_dump.zi_region_core_ptr,
                        err_core_dump.zi_region_core_len * sizeof(uint32));
  
#ifdef FEATURE_ERR_SAVE_CORE_OEM_HEAP
  err_store_ram_to_file(ARM_OEM_HEAP_REGION_FILENAME, 
                        (void *) err_core_dump.oem_heap_core_ptr,
                        err_core_dump.oem_heap_core_len * sizeof(uint32));    
#endif
  
 /* XXX Make sure this happens after the rle_core is saved to EFS since
  it corrupts rle_core */
  err_store_arm_state_cmm();
  
#ifdef FEATURE_ERR_SAVE_CORE_MDSP_RAM
    /* Save MDSP RAM state */
  /* (similar to mdsp_memory_dump_to_file) */
  err_store_ram_to_file(MDSP_RAMA_FILENAME, 
                        (void *)mdsp_bank_address(MDSP_BANK_RAMA, 0), 
                        (MDSP_MEMA_SIZE16 * 2));
  err_store_ram_to_file(MDSP_RAMB_FILENAME, 
                        (void *)mdsp_bank_address(MDSP_BANK_RAMB, 0), 
                        (MDSP_MEMB_SIZE16 * 2));
  err_store_ram_to_file(MDSP_RAMC_FILENAME, 
                        (void *)mdsp_bank_address(MDSP_BANK_RAMC, 0),
                        (MDSP_MEMC_SIZE32 * 4));    
#endif
} 

#endif /* FEATURE_ERR_SAVE_CORE */


/*===========================================================================

FUNCTION ERR_STORE_INFO_TO_FLASH
DESCRIPTION
  Stores information associated with a fatal error or an exception to
  flash.

============================================================================*/
void err_store_info_to_flash (void)
{
#ifdef FEATURE_EFS2
  int fs_handle;
  struct fs_stat sbuf;
  int bytes;
  uint32 total_bytes;
  int err_data_num_files;
#else
  fs_handle_type fs_handle;
  fs_open_xparms_type fs_open_xparms;
#endif
  boolean first_log = TRUE;
  fs_rsp_msg_type fs_rsp;
  err_data_log_type *err_log_ptr = NULL;
  int file_index,j,log_index;
  char temp_filename[80];
  char read_buf[3];
#if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)
  char filename_brew_mem[120];
  int fs_handle_brew_mem;
#endif

  /* Check first to see if there are valid log files to write to EFS */
  if(err_data_is_valid())
  {
    /* Before proceeding, ensure that the last crash did not occur while
     * attempting to write a log file. If our counter is over the threshold
     * then abandon writing the log file.
     */
    if(err_flash_write_counter < ERR_MAX_FLASH_WRITE_ATTEMPTS)
    {
      /* Okay to proceed with flash write...increment flash write counter */
      err_flash_write_counter++;
      
      /* If the error directory does not exist, create it.
       */
      #ifdef FEATURE_EFS2
      if(efs_stat(ERR_DATA_DIR,&sbuf) != 0)
      {
        if(efs_mkdir(ERR_DATA_DIR, 0755) == -1)
          return;
      }
      #else /* FEATURE_EFS2 */
      fs_mkdir(ERR_DATA_DIR, NULL, &fs_rsp);
      if ((fs_rsp.mkdir.status != FS_OKAY_S) &&
        (fs_rsp.mkdir.status != FS_DIR_ALREADY_EXISTS_S))
        return;
      #endif /* FEATURE_EFS2 */
      
      #ifdef FEATURE_EFS2
      fs_handle = efs_open(ERR_DATA_INDEX, O_RDWR);
      
      if(fs_handle == -1)
      {
        file_index = 0;
        fs_handle = efs_open(ERR_DATA_INDEX, O_RDWR | O_CREAT | O_TRUNC, 0755);
      }
      else
      {
        (void) efs_read(fs_handle, &read_buf[0], sizeof(read_buf));
        file_index = atoi(read_buf);
        file_index++;
        
        #ifdef FEATURE_SAVE_TRACE_EXTENDED
        /* Keep max index in sync with f3 trace files */
        /* This gets initialized when diag task starts, which will 
         * be prior to this function being called.
         */
        err_data_num_files = diag_debug_max_files;
        #else /* FEATURE_SAVE_TRACE_EXTENDED */
        err_data_num_files = ERR_DATA_NUM_FILES;
        #endif /* FEATURE_SAVE_TRACE_EXTENDED */
        
        if((file_index < 0) || (file_index >= err_data_num_files))
        {
          file_index = 0;
        }
        /* Move back to beginning of file */
        (void) efs_lseek(fs_handle, 0, SEEK_SET);
      }
      
      (void) std_strlprintf(read_buf,sizeof(read_buf),"%02d",file_index);
      (void) efs_write(fs_handle, &read_buf[0], sizeof(read_buf));
      (void) efs_close(fs_handle);
      #else /* FEATURE_EFS2 */
      /* Multiple error file support not yet implemented for EFS1. */
      /* Will always write to file index 0. */
      file_index = 0;
      #endif /* FEATURE_EFS2 */
      
      /* Create error data file(s).
       */  
      for(log_index=0; log_index<ERR_DATA_MAX_LOGS; log_index++)
      {
        if (err_data.err_log[log_index].flag == ERR_DATA_MAGIC_NUMBER)
        {
          err_log_ptr = &err_data.err_log[log_index];
          #ifdef FEATURE_EFS2
          bytes = 0;
          total_bytes = 0;
          
          if(first_log)
          {
            /* Erase all files at this log_index, to avoid
             * confusion when retrieving logs.
             */
            for(j=0; j<ERR_DATA_MAX_LOGS; j++)
            {
              (void) std_strlprintf (temp_filename, sizeof(temp_filename),
                err_file_format, ERR_DATA_FILE, file_index, j);
              /* EFS2 interface for file removal not implemented yet?
               * Will use EFS1 interface for now...
               */
              fs_remove (temp_filename, NULL, &fs_rsp);
            }
            #ifdef FEATURE_ERR_SAVE_CORE
            /* Also erase all ERR_SAVE_CORE files to avoid confusion */
            err_core_cleanup();
            #endif
            first_log = FALSE;
          }
          
          (void) std_strlprintf (temp_filename, sizeof(temp_filename),
            err_file_format, ERR_DATA_FILE, file_index, log_index);  
          fs_handle = efs_open(temp_filename, O_RDWR | O_CREAT | O_TRUNC, 0755);
          if(fs_handle == -1)
            return;
          
          /* Make sure the length isn't corrupted (or at least not too big) */
          if(err_log_ptr->length > ERR_DATA_MAX_SIZE)
          {
            err_log_ptr->length = ERR_DATA_MAX_SIZE;
          }
          
          while((total_bytes < err_log_ptr->length) && (bytes != -1))  
          {
            bytes = efs_write(fs_handle,
              (void *)(err_log_ptr->data + total_bytes),
              (err_log_ptr->length - total_bytes));
            total_bytes += (uint32)bytes;
          }
          
          (void) efs_close(fs_handle);
          #else /* FEATURE_EFS2 */
          if(log_index == 0)
          {
            /* Need to overwrite an existing file. Could compare timestamps
             ** and overwrite the oldest one, but for now, just overwrite 
             ** the file with the smallest log_index.
             ** 
             ** Also overwrite any additional files at this log_index, to avoid
             ** confusion when retrieving logs.
             */    
            for(j=0; j<ERR_DATA_MAX_LOGS; j++)
            {
              std_strlprintf (temp_filename, sizeof(temp_filename),
                err_file_format, ERR_DATA_FILE, file_index, j);
              fs_remove (temp_filename, NULL, &fs_rsp);
            }
          }
          
          std_strlprintf (temp_filename, sizeof(temp_filename), err_file_format,
            ERR_DATA_FILE, file_index, log_index);
          
          fs_open_xparms.create.cleanup_option = FS_OC_CLOSE;
          fs_open_xparms.create.buffering_option = FS_OB_PROHIBIT;
          fs_open_xparms.create.attribute_mask = FS_FA_UNRESTRICTED;
          
          fs_open(temp_filename, FS_OA_CREATE, &fs_open_xparms, NULL, &fs_rsp);
          if (fs_rsp.open.status != FS_OKAY_S)
            return;
          
          fs_handle = fs_rsp.open.handle;
          
          /* Write error data to the file.
           */
          fs_write (fs_handle, (void *) err_log_ptr->data, err_log_ptr->length,
            NULL, &fs_rsp);
          
          fs_close(fs_handle, NULL, &fs_rsp);
          #endif /* FEATURE_EFS2 */
        }
        /* flags will be cleared after calling other error logging functions,
         * some of which may check the flags independently before writing
         * their log files. */
      }
      
      #if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)
      
      std_strlprintf (filename_brew_mem, sizeof(filename_brew_mem),
        err_file_format, FILENAME_BREW_MEM, file_index, 0); 
      fs_handle_brew_mem = efs_open(filename_brew_mem,
        O_RDWR | O_CREAT | O_TRUNC, 0755);
      if(fs_handle_brew_mem != -1)
      {
        err_dump_brew_mem_info_to_flash(fs_handle_brew_mem) ;
        efs_close(fs_handle_brew_mem) ;
      }
      
      #endif /* FEATURE_MEM_USAGE_AT_EXCEPTION */
      
      #ifdef FEATURE_SAVE_DEBUG_TRACE
      /* We logged at least one error, so also log F3 messages */
      diag_debug_save_trace_to_efs(file_index);
      #endif /*  FEATURE_SAVE_DEBUG_TRACE */
      
      #ifdef FEATURE_ERR_SAVE_CORE
      err_store_ram_core_to_flash();
      #endif /* FEATURE_ERR_SAVE_CORE */
    }
    else /* err_flash_counter is >= ERR_MAX_FLASH_WRITE_ATTEMPTS... */
    {
      ERR("Error: Previous failures detected.  Aborting log file creation.",
        0,0,0);
    }
  } /* End of if err_data_is_valid() section */
  
  /* Clear all flags.  Done even if no log files are written */
  for(log_index=0; log_index<ERR_DATA_MAX_LOGS; log_index++)
  {
    err_data.err_log[log_index].flag = 0;
    err_data.err_log[log_index].length = 0;    
  } 
  err_flash_write_counter = 0;
  
} /* err_store_info_to_flash */

#ifdef EXTENDED_STORE_TEST
/* Stuff to test the extended store functionality. */
typedef void (*err_test_func_type)(void);
uint32 err_debug_flag = 0;
uint32 err_debug_value = 0;

__swi(0x1) void err_swi();

void err_store_test (int test_type)
{
  static volatile int aaa;
  err_test_func_type bbb = (err_test_func_type) 0xeeeeeeee;
  err_test_func_type ccc = (err_test_func_type) 0x0;  

  switch (test_type) {
    
  case 1:
    /* Soft reset */
    ccc();
    break;

  case 2:
    /* Watchdog reset */
    TASKLOCK();
    INTLOCK();
    while(1)
    {
      err_debug_flag++;
      err_debug_value++;
    }
    INTFREE();
    TASKFREE();
    break;
    
  case 3:
    exit(-1);
    break;
     
  case 4:
    /* Divide by Zero */
    aaa = 0;
    aaa = 10/aaa;
    break;      

  case 5:
    /* SWI */
    err_swi();
  break;
      
  case 6:
    /* Fatal error */
    ERR_FATAL ("Testing extended store", 0, 0, 0);
    break;
    
  case 7:
    /* Data abort */
    aaa = *((int *) 0xeeeeeeee);
    break;
    
  case 8:
    /* Prefetch abort */
    bbb ();
    break;
    
  case 9:
    /* Undefined Instruction abort */
    //boot_undefined();
    break;

    default:
      break;
  }
}
#endif /* EXTENDED_STORE_TEST */


#if defined (FEATURE_MEM_USAGE_AT_EXCEPTION)

static int err_print_BrewAppHeap (void )
{
  int iCurrApp = 0 ;
  MYACONTEXT *pac ;
  uint32 heap;
  PMCONTEXT mcontent;
  AEECLSID classid;
  if(AEE_IsInitialized() == FALSE)
    return 0 ;
  for(iCurrApp = 0 ; iCurrApp < MAX_NUMBER_OF_BREW_APPS ; iCurrApp++)
  {
    err_data.g_application_memory_usage[iCurrApp].app_class_id = 0 ;
    err_data.g_application_memory_usage[iCurrApp].app_memory_usage = 0 ;
    err_data.g_application_memory_usage[iCurrApp].app_max_memory_usage = 0 ;       
  }
  iCurrApp = 0 ;
  pac = (void*) AEE_GetStackedAppContext();
  while(pac)
  {
    classid = AEE_GetAppContextCls((PACONTEXT)pac);
    mcontent = AEE_GetAppModuleContext((PACONTEXT)pac);
    if(mcontent)
    {
      heap = AEE_GetModuleMemory((uint32)mcontent);
      err_data.g_application_memory_usage[iCurrApp].app_class_id = classid ;
      err_data.g_application_memory_usage[iCurrApp].app_memory_usage = heap ;
      err_data.g_application_memory_usage[iCurrApp].app_max_memory_usage = 0 ;
    }       
    if (iCurrApp > MAX_NUMBER_OF_BREW_APPS)
    {
      return 0 ;
    }
    iCurrApp++ ;
   
    pac = pac->pNext ;
  }
  return 1 ;
}

static int err_dump_brew_mem_info_to_flash(int fs_handle_dump)
{
  uint32 appID ;
  uint32 app_memory_usage ;
  uint32 app_max_memory_usage ;
  int n_curr_bytes = 0 ;
  char sz_mem_buff[128] ;
  int iCurrApp ;
  int n_total_len = 0;
  int n_total_app_bytes = 0 ;
  if(fs_handle_dump <0)
    return 0 ;

  n_curr_bytes = std_strlprintf(sz_mem_buff, sizeof(sz_mem_buff),
    "---APPLICATION MEMORY USAGE---\n") ;
  n_total_len += n_curr_bytes ;
  efs_write(fs_handle_dump,sz_mem_buff,n_curr_bytes) ;
  for(iCurrApp = 0 ; iCurrApp < MAX_NUMBER_OF_BREW_APPS ; iCurrApp++)
  {
    appID = err_data.g_application_memory_usage[iCurrApp].app_class_id;
    app_memory_usage = err_data.g_application_memory_usage[iCurrApp].app_memory_usage  ;
    app_max_memory_usage = err_data.g_application_memory_usage[iCurrApp].app_max_memory_usage  ;
    if(appID !=0)       
    {
      n_curr_bytes = std_strlprintf(sz_mem_buff,sizeof(sz_mem_buff),
        "APP: 0x%x %ld %ld\n",appID,app_memory_usage,app_max_memory_usage) ;
      n_total_app_bytes += app_memory_usage ;
      n_total_len += n_curr_bytes ;
      efs_write(fs_handle_dump,sz_mem_buff,n_curr_bytes);
    }
  }
  n_curr_bytes = std_strlprintf(sz_mem_buff,sizeof(sz_mem_buff),
    "Total Bytes %d\n---APPLICATION MEMORY USAGE END---\n",n_total_app_bytes);
  n_total_len += n_curr_bytes ;
  efs_write(fs_handle_dump,sz_mem_buff,n_curr_bytes);
  n_count_times = 0 ;  
  return n_total_len ;
}
#endif 

#endif /* FEATURE_ERR_EXTENDED_STORE */

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

void err_set_auto_action(err_action_type action)
{
  /* Include GET_ACTION mask here for backwards compatiblity
   * (may be called with unmasked NV value from older code).
   */
  if(GET_ACTION(action) < SIZEOF_ERR_ACTION)
  {
    err_auto_action = GET_ACTION(action);
  }
  return;
}

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

err_action_type err_get_auto_action(void)
{
  return err_auto_action;
}

#if defined (IMAGE_MODEM_PROC) && defined (FEATURE_ERR_DEBUG_HANDLER)
/*===========================================================================

FUNCTION err_set_timeout_option

DESCRIPTION
  Set the option to be used related to the timeout value.  Use of the timeout
  option is left to the error handler. 

DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

static void err_set_timeout_option(err_timeout_type option)
{
  if(option < SIZEOF_ERR_TIMEOUT)
  {
    err_timeout_option = option;
  }
  return;
}


/*===========================================================================

FUNCTION err_set_timeout_value

DESCRIPTION
  Set the value of the timeout to be used (in seconds).

DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

static void err_set_timeout_value(uint8 val)
{
  err_timeout_value = val;
  return;
}

/*===========================================================================

FUNCTION err_get_timeout_option

DESCRIPTION
  Return the timeout option value.

DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

static err_timeout_type err_get_timeout_option(void)
{
  return err_timeout_option;
}

/*===========================================================================

FUNCTION err_get_timeout_value

DESCRIPTION
  Return the timeout value.

DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

static uint8 err_get_timeout_value(void)
{
  if(err_get_timeout_option()!=ERR_IMMEDIATE)
    return err_timeout_value;
  else
    return 0;
}
#endif /* FEATURE_ERR_DEBUG_HANDLER && IMAGE_MODEM_PROC */

#if defined (FEATURE_MULTIPROCESSOR) && defined (FEATURE_SMSM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && FEATURE_SMSM */

