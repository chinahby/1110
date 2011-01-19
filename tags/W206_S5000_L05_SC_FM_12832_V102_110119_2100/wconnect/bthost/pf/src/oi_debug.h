#ifndef _OI_DEBUG_H
#define _OI_DEBUG_H

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_debug.h#1 $ 

$Log: $
*
*    #4        26 Jan 2005            JH
* Added macro BT_PF_ERR() which calls MSG(). This is called by OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
===========================================================================*/

/** @file   
 *
 * This file provides a set of debug print functions and macros.
 *
 * The functions are all implemented, regardless of whether or not the preprocessor 
 * symbol OI_DEBUG is defined.  The specific implementation (in oi_debug.c) may differ
 * depending on OI_DEBUG.
 *
 * There are currently 4 debug print macros:
 *    OI_LOG_ERROR
 *    OI_DBGPRINT
 *    OI_DBGPRINT2
 *    OI_DBGTRACE
 * 
 * Whether or not these debug print macros generate code depends on a combination of
 * preprocessor symbol definitions:
 *    OI_DEBUG - if defined, all debugprint macros generate code (default for debug mode)
 *    OI_ENABLE_DBGPRINT_ALL - all debugprint macros generate code
 *    OI_ENABLE_DBGPRINT2    - OI_DBGPRINT2, OI_DBGPRINT and OI_LOG_ERROR generate code
 *    OI_ENABLE_DBGPRINT     - OI_DBGPRINT and OI_LOG_ERROR generate code
 *    OI_ENABLE_LOG_ERROR    - OI_LOG_ERROR generates code (default for release mode)
 *    OI_SUPPRESS_LOG_ERROR  - Suppresses error logging for further code size reduction
 *
 * For debug builds (OI_DEBUG defined), all debug print macros generate calls to generate
 * debug output.  A run-time filter is applied to selectively display output depending on
 * module name and runtime debug level configuration for that module.
 *
 * For release builds (OI_DEBUG not defined), debug print macros can be selectively enabled
 * at compile time defining the appropriate OI_ENABLE_xxxx.  This selective enabling may
 * be applied to any or all files.
 *
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_bt_spec.h"
#include "oi_statustext.h"
#include "oi_varstring.h"
#include "oi_text.h"
#include "oi_debugcontrol.h"
#include "oi_osinterface.h"

#ifdef __OI_MODULE__
#define OI_CURRENT_MODULE __OI_MODULE__
#else
#define OI_CURRENT_MODULE OI_MODULE_UNKNOWN
#endif

/**
 * Utility functions.
 */


OI_CHAR *OI_LinkKeyTypeText(OI_BT_LINK_KEY_TYPE keyType);

OI_CHAR *OI_LinkTypeText(OI_UINT8 linkType);  /**< ACL or SCO? */

OI_CHAR *OI_LinkModeText(OI_UINT8 mode);  /**< active/sniff/park/hold? */

OI_CHAR *OI_BtMfgrIdToText(OI_UINT16 mfgrId);

OI_CHAR *OI_HciDataTypeText(OI_UINT8 hciDataType) ; /**< h4 types */

OI_CHAR *OI_ModuleToString(OI_MODULE module);


OI_STATUS OI_ModuleFromFilename(const OI_CHAR *filename,
                                OI_MODULE *module);

/*
 * Do not call this function directly use the DGPRINT macros below.
 */
void _OI_DbgPrint(const OI_CHAR* format, ...);

/*
 * Do not call this function directly use the DGPRINT macros below.
 */
OI_BOOL _OI_DbgCheck(OI_UINT8 debugLevel,
                     OI_UINT8 module,
                     const OI_CHAR *filename,
                     OI_INT line);

/*
 * When RCLI is in use, it is possible that dbgPrint may be re-entered. For example, consider
 * the following call sequence
 *  OI_DbgPrint
 *      OI_Print (oi_osinterface.c)
 *          rcliWrite
 *              l2capWrite
 *                  OI_DbgCheck
 *                     OI_ASSERT(!dbgInfo.checked);  **** assert fail !!!
 *
 * To avoid this problem, the RCLI server must tell the dbgPrint module that RCLI is active.
 * OI_DbgCheck can then avoid the assert fail when RCLI is running.  If RCLI later becomes
 * inactive, it can inform the debug module of this thereby re-enabling the assert check.
 *
 * @param active    TRUE if RCLI is active, FALSE otherwise.
 *
 */

OI_STATUS   OI_DebugRCLIActive(OI_BOOL active);

/**
 *  The debug print function uses printf style formatting and allows multiple
 *  arguments. The debug print function should always be called via the macros
 *  listed below, not directly. The macros set global line and file variables
 *  that are used by the debug print function.
 *
 *  In addition the usual printf style format codes (%d, %s, etc), there are
 *  some custom formats that print some of the standard BM3 data structures.
 *
 *  %!    requires an OI_STATUS value: prints the string for the error.
 *
 *   OI_DBGPRINT(("There was an error %!", status));
 *
 *  %@    requires a pointer to a buffer and a length, prints a hex dump
 *        of the buffer.
 *
 *   OI_DBGPRINT(("Contents of buffer %@", buffer, sizeof(buffer)));
 *
 *  %:    requires a pointer to an OI_BDADDR: prints a Bluetooth address
 *        in the form HH:HH:HH:HH:HH:HH 
 *
 *   OI_DBGPRINT(("Contents of buffer %:", &bdaddr));
 *
 *  %?s   requires a pointer to a string and a length parameter, prints a
 *        substring with a maximum length as specified by the length parameter.
 *
 *   OI_DBGPRINT(("String prefix is %?s", str, 3));
 *
 *  %^    require a pointer to an OI_DATAELEM: decodes and prints a data element
 *        as formatted XML.
 *  
 *   OI_DBGPRINT(("Service attribute list is:\n%^", &attributes));
 *
 *  %S    requires a pointer to an OI_CHAR16: does a crude conversion from a
 *        unicode (double byte chars) string to ASCII.
 *
 *  %b    prints an integer in base 2.
 *  %lb   prints a long integer in base 2.
 *
 *   OI_DBGPRINT(("Bits are %b", I));
 *
 *  %?b   requires an integer (or long integer) and a length parameter. Prints the
 *  %lb   least significant N bits in base 2, where N is the value of the length
 *        parameter.
 *
 *   OI_DBGPRINT(("Bottom 4 bits are: %?b", I, 4));
 *
 *  %/   requires a pointer to a null terminated string. Prints the base file
 *       name, that is, the final substring following a '/', or '\' character.
 *
 *   OI_DBGPRINT(("File %/", "c:\\foo\\bar\\file.txt"));
 *
 *  The custom formats are also available with OI_Printf.
 *
 *
 *  The level parameter specifies what level of debugging is being printed. If
 *  the level specified is less than or equal to the current debug level
 *  configured for the module, the debug output will be printed. Example: If HCI
 *  debug level is set to 1, debug output from HCI will be printed if the debug
 *  output level is set to 1. If the OI_DbgPrint from HCI is specified as 2 or
 *  greater, it will not be printed.
 *      
 *  Using the OI_DBGPRINT macros: 
 *
 *  OI_LOG_ERROR outputs at level 0 
 * 
 *  OI_LOG_ERROR is used for anomalous error conditions.  If a function returns
 *  an error status, OI_LOG_ERROR output describing the reason is appropriate.
 *  If a connection times out in the middle of a transaction, OI_LOG_ERROR
 *  output is appropriate. Errors should not be reported with any of the other
 *  DBGPRINT macros.
 *  
 *  OI_DBGPRINT outputs at level 1 
 * 
 *  OI_DBGPRINT is used to trace high-level data flow, state changes, and
 *  unusual paths (if any) through the code. The object is to follow stack
 *  operation without being overwhelmed by debug output. All connect and
 *  disconnect indications should use OI_DBGPRINT. OI_DBGPRINT should be used to
 *  trace the send and receive of each high-level packet. For example, L2CAP
 *  would DBGPRINT when a complete L2CAP packet had been sent or received, HCI
 *  would DBGPRINT when an ACL packet had been sent or received, and OBEX would
 *  DBGPRINT when an OBEX packet had been sent or received. Including context
 *  information (CID, ACL handle, etc.) is helpful.
 * 
 *  OI_DBGPRINT2 outputs at level 2 
 * 
 *  OI_DBGPRINT2 is used for verbose debug information.
 * 
 *
 * @note that whe way the OI_DBGPRINT macro is defined, the arguments list must
 * always be enclosed in double parentheses:
 * 
 *    OI_DBGPRINT(("this is an error: status = %d", status));
 */


/*
 * Set the following flags according to the various defines
 */
#undef DEBUG_OI_LOG_ERROR
#undef RELEASE_OI_LOG_ERROR
#undef IMPLEMENT_OI_DBGPRINT
#undef IMPLEMENT_OI_DBGPRINT2


#if (defined(OI_DEBUG) || defined(OI_ENABLE_DBGPRINT_ALL) || defined(OI_ENABLE_DBGPRINT2))

#define DEBUG_OI_LOG_ERROR
#define IMPLEMENT_OI_DBGPRINT
#define IMPLEMENT_OI_DBGPRINT2

#elif defined(OI_ENABLE_DBGPRINT)
/* not debug, only some debug output is enabled */
#define DEBUG_OI_LOG_ERROR
#define IMPLEMENT_OI_DBGPRINT

#elif defined(OI_ENABLE_LOG_ERROR)
/* not debug, only some debug output is enabled */
#define DEBUG_OI_LOG_ERROR

#else
/* not debug, implement a code space efficient log error */
#define RELEASE_OI_LOG_ERROR

#endif

/*
 * The debug print macros OI_LOG_ERROR, OI_DBGPRINT, OI_DBGPRINT2
 */

#ifndef OI_CODE
extern void bt_pf_err_oi (const char * format, ...);
#endif

#ifdef OI_SUPPRESS_LOG_ERROR
    #define OI_LOG_ERROR(msg)
#else
    #ifdef DEBUG_OI_LOG_ERROR 
#error code not present
    #else // RELEASE_OI_LOG_ERROR
        #ifdef OI_CODE
#error code not present
        #else
            #define OI_LOG_ERROR(msg)   { bt_pf_err_oi msg ; }
        #endif    
    #endif
#endif

#ifdef IMPLEMENT_OI_DBGPRINT
#error code not present
#else
#define OI_DBGPRINT(msg)
#endif

#ifdef IMPLEMENT_OI_DBGPRINT2
#error code not present
#else
#define OI_DBGPRINT2(msg)
#endif

/** OI_DBGTRACE  
 *      Macro for tracing control flow.
 */

#define OI_DBGTRACE(msg)    OI_DBGPRINT(msg)


/**
 * DEBUG_ONLY - a little macro for single-line statements and variable declarations
 * Avoids the clutter of #ifdef OI_DEBUG in the source code.
 *
 * For example, to declare a variable that is only used when OI_DEBUG is
 * defined:
 *
*     DEBUGONLY( OI_UINT counter = 0; )
*     ...
*     DEBUGONLY( ++counter; );
*     ...
*     OI_DBGPRINT(("Counter is %d", counter));
*/

#ifdef  OI_DEBUG
#error code not present
#else
#define DEBUG_ONLY(stmt)
#endif

/*****************************************************************************/
#endif /* _OI_DEBUG_H */

