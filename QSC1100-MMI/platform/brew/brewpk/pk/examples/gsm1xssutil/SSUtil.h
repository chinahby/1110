#ifndef __SSUTILCLS_H__
#define __SSUTILCLS_H__

/*=========================================================================
FILE: SSUtil.h

SERVICES: GSM1x SS Utilities
  
GENERAL DESCRIPTION:
    Translation (Pack and Unpack services) of GSM1x SS signaling messages.
    Logging facility for the SS applets.
    Call counters storage.
	 
PUBLIC CLASSES AND STATIC FUNCTIONS:
    ISSUtilCls - AEECLSID_SSUTIL 0x02009015
		
INITIALIZATION & SEQUENCING REQUIREMENTS:
			 
				  (c) Copyright 2000-2003 QUALCOMM Incorporated.
				  All Rights Reserved.
				  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSMsg.h"  // GSM1x SS signaling messages definitions

#include "AEE.h"			// Standard AEE definitions
#include "AEEShell.h"	    // AEE Shell Services
//#include "SSUtil_AEEModGen.h"		// Mandatory include
#include "AEEStdLib.h"		// BREW's standard library


#if !defined(BREW_STATIC_APP)
    // Use the .bid file if compiling as dynamic applet
    #include "SSUtil.bid" // class ID		 
#else
    #include "OEMClassIDs.h" // Applet Ids
#endif

#ifndef AEECLSID_SSUTIL
    #error AEECLSID_SSUTIL must be defined!
#endif

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// enable the GSM1x SS EFS logging facility
#ifndef NDEBUG
#define SS_LOGGING_FEATURE
#endif

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

//
// SSUtil error types
// 

// success
#define SSUTIL_SUCCESS                                      0
// unknown PD. 
// this should never occur.
#define SSUTIL_ERROR_UNKNOWN_PD                             1
// message type not defined or not implemented.
// message shall be ignored (24.008, section 8.4)
#define SSUTIL_ERROR_UNKNOWN_MSG_TYPE                       2
// message too short.
// message shall be ignored (24.008, section 8.2)
#define SSUTIL_ERROR_MSG_TOO_SHORT                          3
// ie shall be ignored (24.008, section 8.7.1)
#define SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE    4
#define SSUTIL_ERROR_INVALID_ELEMENT_LENGTH                 5
#define SSUTIL_FAILURE                                      6

typedef uint8 SSUtil_error_type;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

//********************************************************************************************************************************
//
// ISSUtilCls Interface Definition
//
//********************************************************************************************************************************

#ifndef HACK_A_SHAQ
typedef struct _ISSUtilCls ISSUtilCls;
#define HACK_A_SHAQ
#endif

QINTERFACE(ISSUtilCls)
{
    INHERIT_IBase(ISSUtilCls);
        
    SSUtil_error_type (*Translate_ext_to_int)(ISSUtilCls* po, 
                                              uint8 ext_msg_len,
                                              uint8* ext_msg,
                                              SSMsg_int_msg_type *int_msg);
 
    SSUtil_error_type (*Translate_int_to_ext)(ISSUtilCls *po, 
                                              SSMsg_int_msg_type *int_msg,
                                              uint8* ext_msg_len,
                                              uint8* ext_msg);
    void (*Log)(ISSUtilCls *po, 
                const char *level,   /* debug level string */
                const char *fmt,                
                uint32      code1,   /* 1st parameter to go with format string */
                uint32      code2,   /* 2nd parameter to go with format string */
                uint32      code3    /* 3rd parameter to go with format string */
                );

    int     (*GetCallIndependentCounter)(ISSUtilCls *po); 
    void    (*IncCallIndependentCounter)(ISSUtilCls *po);
    void    (*DecCallIndependentCounter)(ISSUtilCls *po);
    void    (*ResetCallIndependentCounter)(ISSUtilCls *po);

    int     (*GetCallDependentCounter)(ISSUtilCls *po); 
    void    (*IncCallDependentCounter)(ISSUtilCls *po);
    void    (*DecCallDependentCounter)(ISSUtilCls *po);
    void    (*ResetCallDependentCounter)(ISSUtilCls *po);

    const AECHAR*  (*GetVersionString)(ISSUtilCls *po);
};

#define ISSUTILCLS_AddRef(p)				 GET_PVTBL(p,ISSUtilCls)->AddRef(p)
#define ISSUTILCLS_Release(p)				 GET_PVTBL(p,ISSUtilCls)->Release(p)

#define ISSUTILCLS_Translate_ext_to_int(po,ext_msg_len,ext_msg,int_msg)		\
		   GET_PVTBL(po,ISSUtilCls)->Translate_ext_to_int(po,ext_msg_len,ext_msg,int_msg)
#define ISSUTILCLS_Translate_int_to_ext(po,int_msg,ext_msg_len,ext_msg)		\
		   GET_PVTBL(po,ISSUtilCls)->Translate_int_to_ext(po,int_msg,ext_msg_len,ext_msg)

#define ISSUTILCLS_GetCallIndependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->GetCallIndependentCounter(po)
#define ISSUTILCLS_IncCallIndependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->IncCallIndependentCounter(po)
#define ISSUTILCLS_DecCallIndependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->DecCallIndependentCounter(po)
#define ISSUTILCLS_ResetCallIndependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->ResetCallIndependentCounter(po)

#define ISSUTILCLS_GetCallDependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->GetCallDependentCounter(po)
#define ISSUTILCLS_IncCallDependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->IncCallDependentCounter(po)
#define ISSUTILCLS_DecCallDependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->DecCallDependentCounter(po)
#define ISSUTILCLS_ResetCallDependentCounter(po) \
            GET_PVTBL(po,ISSUtilCls)->ResetCallDependentCounter(po)
#define ISSUTILCLS_GetVersionString(po) \
            GET_PVTBL(po,ISSUtilCls)->GetVersionString(po)

#define ISSUTILCLS_Log(po,level,fmt,a,b,c)     GET_PVTBL(po,ISSUtilCls)->Log(po,level,fmt,a,b,c)

// helper functions
uint8 get_bits(uint8 src, uint8 offset, uint8 len);
void set_bits(uint8* dst, uint8 src, uint8 offset, uint8 len);
uint8 translate_ext_digits(uint8 src_len, const uint8* src, uint8* dst);

#define SSMSG_SAFE_ADVANCE(poffset, advance, limit_offset, return_err) \
    if (*poffset+advance >= limit_offset) \
    { \
        *poffset = limit_offset; \
        return return_err; \
    } \
    *poffset+=advance;


// Macros for GSM1x logging
// ------------------------
#ifdef SS_LOGGING_FEATURE

    #define GSM1X_LOG_HIGH(ssutil,string,a,b,c) \
            { \
                ISSUTILCLS_Log(ssutil,"High",string,a,b,c); \
            }

    #define GSM1X_LOG_ERROR(ssutil,string,a,b,c) \
            { \
                DBGPRINTF(string,a,b,c); \
                ISSUTILCLS_Log(ssutil,"Error",string,a,b,c); \
            }

    #define GSM1X_LOG_FATAL(ssutil,string,a,b,c) \
            { \
                DBGPRINTF(string,a,b,c); \
                ISSUTILCLS_Log(ssutil,"Fatal",string,a,b,c); \
            }
#else

    #define GSM1X_LOG_HIGH(ssutil,string,a,b,c)  
    #define GSM1X_LOG_ERROR(ssutil,string,a,b,c) { DBGPRINTF(string,a,b,c); }
    #define GSM1X_LOG_FATAL(ssutil,string,a,b,c) { DBGPRINTF(string,a,b,c); }

#endif

/*=====================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: ISSUtilCls

Description:
    SSUtil provides GSM1x SS utilities as listed below. It is implemented as 
    a singleton (using private SSUtil_AEEModGen framework).
    1. Translate (Pack/Unpack) GSM1x SS signaling messages between external
       format (raw buffer) and internal format (c structs).
       For translation specifications, see GSM1x Supplementary Services
       document.
    2. Logging facility for the SS applets.
       To use the logging facility, call one of the GSM1X_LOG_ZZZ macros. This
       will entail execution of both DBGPRINTF and EFS logging.

       EFS logging means logging to a text file on the phone's file system,
       using IFILE services.
       EFS Logging is enabled only when the compilation flag -
       SS_LOGGING_FEATURE is defined.

       Several definitions are applicable to the EFS logging:
       SSUTIL_LOGGING_TRIGGER_FILE_NAME - name of trigger file name.
       SSUTIL_LOG_FILE_NAME - name of log file.
       SSUTIL_LOG_BACKUP_FILE_NAME - name of backup log file.
       SSUTIL_LOG_MSG_COUNT_LIMIT - limit of log messages in one log file.

       In run-time, SSUtil will search for a file with the 
       SSUTIL_LOGGING_TRIGGER_FILE_NAME. If this file is not found in the
       current directory, EFS logging will be disabled. This allows controlling
       the logging facility without changing the phone's software.
       If the above file is found, SSUtil will create a new file called
       SSUTIL_LOG_FILE_NAME (existing log file will be erased).
       SSUtil will add log messages to this log file, upto the limit of
       SSUTIL_LOG_MSG_COUNT_LIMIT. When this limit is reached, the log file
       is renamed to SSUTIL_LOG_BACKUP_FILE_NAME (overwriting old backup log 
       file) and a new log file is created.

       All SSUtil logging related files are expected to occupy an average of
       about 8KB on the EFS.

    3. Call counters. SSUtil stores two counters:
       - Call dependent counter: number of active call dependent transactions.
         This counter refers to voice calls.
       - Call independent counter: number of active call independent transactions.
         This counter refers to management transactions.

=======================================================================

ISSUTILCLS_Translate_ext_to_int

Description:
    Translate a GSM1x SS signaling messages from external format (raw buffer) 
    into internal format (c structs).

Prototype:
    SSUtil_error_type ISSUTILCLS_Translate_ext_to_int(
                        ISSUtilCls* po, 
                        uint8 ext_msg_len,
                        uint8* ext_msg,
                        SSMsg_int_msg_type *int_msg)

Parameters: 
    po [in]          : Pointer to the ISSUtilCls Interface object
    ext_msg_len [in] : external message buffer length
    ext_msg [in]     : external message buffer
    int_msg [out]    : placeholder for the unpacked message

Return Value:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.
    
=======================================================================

ISSUTILCLS_Translate_int_to_ext

Description:
    Translate a GSM1x SS signaling messages from internal format (c structs)
    into external format (raw buffer).

Prototype:
    SSUtil_error_type ISSUTILCLS_Translate_int_to_ext(
                        ISSUtilCls *po, 
                        SSMsg_int_msg_type *int_msg,
                        uint8* ext_msg_len,
                        uint8* ext_msg)

Parameters: 
    po [in]              : Pointer to the ISSUtilCls Interface object
    int_msg [in]         : internal message to pack
    ext_msg_len [in,out] : max length of external message buffer. returns the
                           length of the packed message.
    ext_msg [out]        : placeholder for the packed message

Return Value:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

Comments: 
    This function assumes that int_msg contains a valid message.

=======================================================================

ISSUTILCLS_Log

Description:
    Output one line to the log file.

Prototype:
    void ISSUTILCLS_Log(
            ISSUtilCls *po, 
            const char *level,
            const char *fmt,              
            uint32      code1,
            uint32      code2,
            uint32      code3)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.
    level [in] : debug level string. level doesn't affect the behavior of the 
                 function. It is only added to the logged string.
    fmt [in]   : format string.
    code1, code2, code3 [in] : parameters to go with the format string.

Return Value:
    None.

Comments:
    The logged string is composed as follows:
        "level: (fmt,code1,code2,code3)\n"
    The log file is limited to a predefined number of log messages. When this
    limit is reached, the log file is renamed into a backup log file and a new
    log file is created.
    
=======================================================================

ISSUTILCLS_GetCallIndependentCounter

Description:
    Get call independent counter.

Prototype:
    int ISSUTILCLS_GetCallIndependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    The current value of the call independent counter.
    
=======================================================================

ISSUTILCLS_IncCallIndependentCounter

Description:
    Increase call independent counter.

Prototype:
    void ISSUTILCLS_IncCallIndependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    None.
    
=======================================================================

ISSUTILCLS_DecCallIndependentCounter

Description:
    Decrease call independent counter.

Prototype:
    void ISSUTILCLS_DecCallIndependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    None.
    
=======================================================================

ISSUTILCLS_ResetCallIndependentCounter

Description:
    Reset call independent counter.

Prototype:
    void ISSUTILCLS_ResetCallIndependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    None.
    
=======================================================================

ISSUTILCLS_GetCallDependentCounter

Description:
    Get call dependent counter.

Prototype:
    int ISSUTILCLS_GetCallDependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    The current value of the call dependent counter.
    
=======================================================================

ISSUTILCLS_IncCallDependentCounter

Description:
    Increase call dependent counter.

Prototype:
    void ISSUTILCLS_IncCallDependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    None.
    
=======================================================================

ISSUTILCLS_DecCallDependentCounter

Description:
    Decrease call dependent counter.

Prototype:
    void ISSUTILCLS_DecCallDependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    None.
    
=======================================================================

ISSUTILCLS_ResetCallDependentCounter

Description:
    Reset call dependent counter.

Prototype:
    void ISSUTILCLS_ResetCallDependentCounter(ISSUtilCls *po)

Parameters: 
    po [in]    : Pointer to the ISSUtilCls Interface object.

Return Value:
    None.
    
=======================================================================*/

/*=============================================================================
FUNCTION: SSUtilCls_GetVersionString

DESCRIPTION:
    Get the ssutil's version info string.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    Version Info string.

=============================================================================*/


#endif  //__SSUTILCLS_H__


