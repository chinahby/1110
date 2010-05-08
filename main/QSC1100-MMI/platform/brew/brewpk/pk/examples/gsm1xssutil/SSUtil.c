/*=========================================================================
FILE:  SSUtil.c

SERVICES:  GSM1x SS Utilities
  
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
#include "SSUtil_AEEModGen.h"
#include "SSUtil.h"     // Our header to distribute for our class
#include "SSFacility.h"
#include "SS_CommonMacros.h"

#include "AEEFile.h"        // AEEFile Services

#ifdef SS_LOGGING_FEATURE
    #include "SSAgentApp.h"
#endif

//#include "ssutil_res.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// version info string
#define SSUTIL_VERSION_STRING "SSUtil Ver 1.0"

// max length of version info string
#define MAX_VERSION_INFO_STRING_LENGTH sizeof(SSUTIL_VERSION_STRING)/sizeof(char) +1

// max length of log string
#define MAX_LOG_STRING_LENGTH 128
// trigger file name
#define SSUTIL_LOGGING_TRIGGER_FILE_NAME "logcfg.txt"
// log file name
#define SSUTIL_LOG_FILE_NAME "log.txt"
// log backup file name
#define SSUTIL_LOG_BACKUP_FILE_NAME "log_bck.txt"
// old log files file prefix
#define OLD_FILENAME_PREFIX "old_"
// limit the amount of log messages in the log file
#define SSUTIL_LOG_MSG_COUNT_LIMIT  100

#define SSUTIL_MSG_HEADER_LEN 2     // length of a signaling message header.
                                    // includes the TI/PD and msg type octets


/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/
typedef struct _SSUtilCls
{
    // Declare our VTable
    // by doing so, we have placed function pointers
    // in the beginning of this structure
    DECLARE_VTBL(ISSUtilCls)

    // Class member variables
    uint32      m_nRefs;        // References to us
    IShell     *m_pIShell;      // copy of Shell pointer
    IModule    *m_pIModule;     // IModule interface pointer

    // call counters
    int         m_CallDependentCounter;
    int         m_CallIndependentCounter;

#ifdef SS_LOGGING_FEATURE
    // EFS logging related members
    boolean     m_bLoggingEnabled; // true if logs can be printed to a file. 
    IFileMgr   *m_pIFileMgr;
    IFile      *m_pIFileLog;
    char m_msgBuffer[MAX_LOG_STRING_LENGTH];
    uint16      m_msgCounter;
#endif // SS_LOGGING_FEATURE

    AECHAR      m_versionInfoBuffer[MAX_VERSION_INFO_STRING_LENGTH];

}SSUtilCls;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

#ifdef BREW_STATIC_APP
//-----------------------------------------------------------------------------
// Module Method Declarations. Required for STATIC compilation
//-----------------------------------------------------------------------------
PFNMODENTRY SSUtilMod_GetModInfo(IShell      *ps,
                                 AEECLSID   **ppClasses,
                                 AEEAppInfo **pApps,
                                 uint16      *pnApps,
                                 uint16      *pwMinPriv);

static int SSUtilMod_Load(IShell   *pIShell,
                          void     *ph,
                          IModule **ppMod);

static int SSUtilMod_CreateInstance(AEECLSID ClsId,
                                    IShell  *pIShell,
                                    IModule *po,
                                    void   **ppObj);

static void SSUtilMod_FreeResources(IModule  *po);

#endif // BREW_STATIC_APP

#ifdef SS_LOGGING_FEATURE
    boolean SSUtilCls_InitLogFile(SSUtilCls* pMe);
#endif // SS_LOGGING_FEATURE

//-----------------------------------------------------------------------------
// Class Method Declarations.
//-----------------------------------------------------------------------------
int SSUtilCls_New(
    AEECLSID clsID,
    IShell   *ps,
    IModule  *pIModule,
    ISSUtilCls **ppObj);

static uint32       SSUtilCls_AddRef(ISSUtilCls * po);
uint32       SSUtilCls_Release(ISSUtilCls * po);

static SSUtil_error_type SSUtilCls_Translate_ext_to_int(ISSUtilCls* po, 
                                                        uint8 ext_msg_len,
                                                        uint8* ext_msg,
                                                        SSMsg_int_msg_type *int_msg);

static SSUtil_error_type SSUtilCls_Translate_int_to_ext(ISSUtilCls *po, 
                                                        SSMsg_int_msg_type *int_msg,
                                                        uint8* ext_msg_len,
                                                        uint8* ext_msg);

static void SSUtilCls_Log(ISSUtilCls *po, 
                          const char *fmt,
                          const char *level,   /* debug level string*/
                          uint32      code1,   /* 1st parameter to go with format string */
                          uint32      code2,   /* 2nd parameter to go with format string */
                          uint32      code3    /* 3rd parameter to go with format string */
                          );

static int  SSUtilCls_GetCallIndependentCounter(ISSUtilCls *po) ;
static void SSUtilCls_DecCallIndependentCounter(ISSUtilCls *po) ; 
static void SSUtilCls_IncCallIndependentCounter(ISSUtilCls *po) ; 
static void SSUtilCls_ResetCallIndependentCounter(ISSUtilCls *po);

static int  SSUtilCls_GetCallDependentCounter(ISSUtilCls *po) ;
static void SSUtilCls_DecCallDependentCounter(ISSUtilCls *po) ; 
static void SSUtilCls_IncCallDependentCounter(ISSUtilCls *po) ; 
static void SSUtilCls_ResetCallDependentCounter(ISSUtilCls *po);
static const AECHAR* SSUtilCls_GetVersionString(ISSUtilCls *po);

#ifdef SS_LOGGING_FEATURE
static int SSUtil_RenameLogFile(
    IFileMgr   *pFileMgr, 
    const char *pCurrLogFileName,
    const char *pNewLogFileName
    );
static int SSUtil_RemoveLogFileIfItExists(
    IFileMgr   *pFileMgr, 
    const char *pLogFileName
    );
static IFile* SSUtil_CreateNewLogFile(
    IFileMgr   *pFileMgr, 
    const char *pLogFileName
    );
#endif // SS_LOGGING_FEATURE

/*===========================================================================
                    GLOBAL DATA 
===========================================================================*/

// IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined for SDK ONLY!
// This variable should NOT BE (1) overwritten and/or (2) USED DIRECTLY
// by BREW SDK users.
// It is used as an entry point to AEEStdLib.
// DO NOT REMOVE next three lines.
#ifdef AEE_SIMULATOR
    AEEHelperFuncs* g_pvtAEEStdLibEntry;
#endif

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

#ifdef BREW_STATIC_APP

static const AEECLSID gSSUtilClasses[] =
{
   0  // Must terminal list with 0
};

static const AEEAppInfo gSSUtilApps[] = {
   {
      AEECLSID_SSUTIL,   // Class ID
      0,                 // File name of resource file
      0,                 // String ID of app title
      0,                 // Bitmap ID of app icon 26x26
      0,                 // Bitmap ID of app icon LARGE
      0,                 // String ID of settings name
      0,                 
      0 // AFLAG_STATIC       // Applet flags
   }
};

#endif // BREW_STATIC_APP

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/////////////////////////////////////////////////////////////////////////////
// For static module we must:
//  1. Provide a GetModInfo() function
//  2. Provide a Load() function
//  3. Provide a CreateInstance() - which is also used by a dynamic module.
/////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSUtilMod_GetModInfo

DESCRIPTION:

PARAMETERS:

RETURN VALUE:
    static module information
=============================================================================*/
#ifdef BREW_STATIC_APP

PFNMODENTRY SSUtilMod_GetModInfo(IShell      *ps,
                                 AEECLSID   **ppClasses,
                                 AEEAppInfo **pApps,
                                 uint16      *pnApps,
                                 uint16      *pwMinPriv)
{
   *ppClasses = (AEECLSID*)gSSUtilClasses;
   *pApps = (AEEAppInfo*)gSSUtilApps;
   *pnApps = sizeof(gSSUtilApps) / sizeof(gSSUtilApps[0]);

   return ((PFNMODENTRY)SSUtilMod_Load);
}

#endif 

/*=============================================================================
FUNCTION: SSUtilMod_Load

DESCRIPTION:
    Called by BREW to initialize the static module.

PARAMETERS:

RETURN VALUE:

=============================================================================*/
#ifdef BREW_STATIC_APP

static int SSUtilMod_Load(IShell   *pIShell,
                          void     *ph,
                          IModule **ppMod)
{
    // send our CreateInstance and FreeResources
    return SSUtil_AEEStaticMod_New(sizeof(AEEMod),
                         pIShell,
                         ph,
                         ppMod,
                         SSUtilMod_CreateInstance,
                         SSUtilMod_FreeResources);
}

#endif

/*=============================================================================
FUNCTION: SSUtilMod_CreateInstance

DESCRIPTION:
    Called by AEEModGen when the class has been requested by BREW

PARAMETERS:

RETURN VALUE:

=============================================================================*/
static int SSUtilMod_CreateInstance(AEECLSID ClsId,
                                    IShell  *pIShell,
                                    IModule *po,
                                    void   **ppObj)
{
    if (ppObj != NULL)
    {
        *ppObj = NULL;
    }
        
    if ((ClsId == AEECLSID_SSUTIL) 
        && (pIShell != NULL)
        && (po != NULL)
        && (ppObj != NULL)
        && (((AEEMod*)po)->pISSUtilCls != NULL))
    {
        *ppObj = ((AEEMod*)po)->pISSUtilCls;
        SSUtilCls_AddRef(((AEEMod*)po)->pISSUtilCls);
        return AEE_SUCCESS;
    }
    return EFAILED;
}

/*=============================================================================
FUNCTION: SSUtilMod_FreeResources

DESCRIPTION:
    Called by BREW when the static module is destroyed.

PARAMETERS:

RETURN VALUE:

=============================================================================*/
#ifdef BREW_STATIC_APP

static void SSUtilMod_FreeResources(IModule  *po)
{
}

#endif // BREW_STATIC_APP

/*===========================================================================
FUNCTION: SSUtilCls_New

DESCRIPTION:
    This function is called from AEEClsCreateInstance.

PARAMETERS:
    nSize [in]   : Specifies the size (in uint8s) of the structure to be 
                   allocated for the module. Typically, this is the size of 
                   the structure (or class).
    pIShell [in] : Pointer to IShell interface.
    pIModule [in]: Pointer to the IModule interface.
    ppMod [out]  : On return, *ppMod contains a valid pointer to the SSUtilCls 
                   structure (or class).

RETURN VALUE:
    AEE_SUCCESS: If successful.
    Error code: If failed.
==============================================================================*/
int SSUtilCls_New(AEECLSID clsID, IShell   *ps, IModule  *pIModule, ISSUtilCls **ppObj)
{
    SSUtilCls *pMe = NULL;
    VTBL(ISSUtilCls)* interfaceFuncs;
    int16 nSize;

    if( !ps || !pIModule )
        return EFAILED;

    *ppObj = NULL;

    // Allocate memory for the SSUtilCls object and
    // Allocate the VTBL and initialize it. Note that the modules and apps must not
    // have any static data. Hence, we need to allocate the VTBL as well.
    nSize = sizeof(SSUtilCls);
    if( (pMe = (SSUtilCls *)MALLOC(nSize + sizeof(ISSUtilClsVtbl))) == NULL )
        return ENOMEMORY;
   
    // Put the ISSUtilClsVtbl right after SSUtilCls, in the same memory chunk. 
    // Casting to (uint8*) is essential for correct pointer arithmetics.
    interfaceFuncs = (ISSUtilClsVtbl *)((uint8 *)pMe + nSize);

    //Initialize individual entries in the VTBL
    interfaceFuncs->AddRef = SSUtilCls_AddRef;
    interfaceFuncs->Release = SSUtilCls_Release;
    interfaceFuncs->Translate_ext_to_int = SSUtilCls_Translate_ext_to_int;
    interfaceFuncs->Translate_int_to_ext = SSUtilCls_Translate_int_to_ext;
    interfaceFuncs->Log = SSUtilCls_Log;

    interfaceFuncs->GetCallDependentCounter = SSUtilCls_GetCallDependentCounter;
    interfaceFuncs->IncCallDependentCounter = SSUtilCls_IncCallDependentCounter;
    interfaceFuncs->DecCallDependentCounter = SSUtilCls_DecCallDependentCounter;
    interfaceFuncs->ResetCallDependentCounter = SSUtilCls_ResetCallDependentCounter;

    interfaceFuncs->GetCallIndependentCounter = SSUtilCls_GetCallIndependentCounter;
    interfaceFuncs->IncCallIndependentCounter = SSUtilCls_IncCallIndependentCounter;
    interfaceFuncs->DecCallIndependentCounter = SSUtilCls_DecCallIndependentCounter;
    interfaceFuncs->ResetCallIndependentCounter = SSUtilCls_ResetCallIndependentCounter;

    interfaceFuncs->GetVersionString = SSUtilCls_GetVersionString;

    // initialize the VTBL
    INIT_VTBL(pMe, ISSUtilCls, *interfaceFuncs);

    pMe->m_nRefs = 1;
    pMe->m_pIShell = ps;
    pMe->m_pIModule = pIModule;
    
#ifdef SS_LOGGING_FEATURE
    // Init the EFS logging log file.
    // This should only be called from the context of the SSAgent applet!
    SSUtilCls_InitLogFile(pMe);
#endif

    // Add References
    ISHELL_AddRef(ps);
    IMODULE_AddRef(pIModule);

    // Set the pointer in the parameter
    *ppObj = (ISSUtilCls*)pMe;

    return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION: SSUtilCls_AddRef

DESCRIPTION:
    This function increases the reference count for the SSUtilCls object.

PARAMETERS:
    po [in]: Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    The updated reference count.
==============================================================================*/
static uint32 SSUtilCls_AddRef(ISSUtilCls * po)
{
   return (++((SSUtilCls *)po)->m_nRefs);
}

/*===========================================================================
FUNCTION: SSUtilCls_Release

DESCRIPTION:
    This function decreases the reference count for the SSUtilCls object. If
    the reference count reaches zero, it does the required cleanup.

PARAMETERS:
    po [in]: Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    The updated reference count.
==============================================================================*/
uint32 SSUtilCls_Release(ISSUtilCls * po)
{
    SSUtilCls *pMe = (SSUtilCls *)po;

    // Only when Ref count is zero, release memory associated with this object.
    if( --pMe->m_nRefs != 1/*0*/ )
        return pMe->m_nRefs;

#ifdef SS_LOGGING_FEATURE
    pMe->m_bLoggingEnabled = FALSE;

    if (pMe->m_pIFileLog != NULL)
    {
        IFILE_Release(pMe->m_pIFileLog);
        pMe->m_pIFileLog = NULL;
    }

    if (pMe->m_pIFileMgr != NULL)
    {
        IFILEMGR_Release(pMe->m_pIFileMgr);
        pMe->m_pIFileMgr = NULL;
    }
#endif // SS_LOGGING_FEATURE
   
    // Release interfaces
    ISHELL_Release(pMe->m_pIShell);
    IMODULE_Release(pMe->m_pIModule);

    //Free the object itself
    FREE_VTBL(pMe, IModule);
    FREE( pMe );

    return 0;
}

#ifndef BREW_STATIC_APP

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the class which is being loaded

	pIShell: [in]: Contains pointer to the IShell interface. 

	po [in]: Contains pointer to the IModule interface to the current module to which
	this class belongs

	ppObj: [out]: On return, *ppObj must point to a valid IBase object. 

RETURN VALUE
  AEE_SUCCESS: If the class/app creation was successful. 
  EFAILED: Error occurred while creating the class/app. In this case, the app/class will
  not be loaded.
==============================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
    return SSUtilMod_CreateInstance(ClsId, pIShell, po, ppObj);
}

#endif // !BREW_STATIC_APP


//********************************************************************************************************************************
//
// Translator functions
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: get_bits

DESCRIPTION:
    Get the value of bits [offset,offset+len-1] from src.

PARAMETERS:
    src [in]    : the byte to extract from.
    offset [in] : first bit index to extract from.
    len [in]    : number of bits to extract.

RETURN VALUE:
    value of the requested bits.
=============================================================================*/
uint8 get_bits(uint8 src, uint8 offset, uint8 len)
{
    uint8 result;
    if (offset+len > 8)
        return 0;

    result = src << (8-offset-len);
    result >>= (8-len);
    return result;
}

/*=============================================================================
FUNCTION: set_bits

DESCRIPTION:
    Set bits [offset,offset+len-1] of dst with the value in src.

PARAMETERS:
    dst [in,out] : the byte to set the bits inside.
    src [in]     : value to set into the requested bits of dst.
    offset [in]  : first bit index in dst to set.
    len [in]     : number of bits in dst to set.

RETURN VALUE:
    None.
=============================================================================*/
void set_bits(uint8* dst, uint8 src, uint8 offset, uint8 len)
{
    uint8 mask;
    if (offset+len > 8)
        return;

    mask = ((0xff >> (8-offset)) | (0xff << (offset + len)));
    // zero out bits in dst
    *dst &= mask;
    // prepare src
    src <<= (8-len);
    src >>= (8-offset-len);

    *dst |= src;
}

/*=============================================================================
FUNCTION: translate_ext_digits

DESCRIPTION:
    Translate a bcd number from nibbles format into ASCII.
    Nibble values [0,9] are translated to ['0','9'].
    Nibble value 10 is translated to '*'.
    Nibble value 11 is translated to '#'.
    Nibble values [12,14] are translated to ['a','c'].
    Nibble value 15 stands for unused nibble in the case of odd number of digits.

PARAMETERS:
    src_len [in] : length of the src buffer.
    src [in]     : buffer containing the bcd number.
    dst [out]    : buffer for the translated ASCII string.

RETURN VALUE:
    Number of digits written to dst.
=============================================================================*/
uint8 translate_ext_digits(uint8 src_len, const uint8* src, uint8* dst)
{
    uint8 i,j, index=0;
    uint8 nibble;

    for(i=0; i<src_len; ++i)
    {
        for (j=0; j<2; ++j)
        {
            // get nibble
            if (j==0)
                nibble = src[i] & 0x0f;
            else
                nibble = ((src[i] & 0xf0) >> 4);

            switch(nibble)
            {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
                dst[index++] = (nibble+'0');
                break;
            case 0x0a:
                dst[index++] = '*';
                break;
            case 0x0b:
                dst[index++] = '#';
                break;
            case 0x0c:
            case 0x0d:
            case 0x0e:
                dst[index++] = (nibble-0x0c+'a');
                break;
            case 0x0f:
                return index;
            }
        }
    }
    return index;
}

/*=============================================================================
FUNCTION: translate_int_digits

DESCRIPTION:
    Translate an ASCII string of digits (chars) into bcd number in nibbles format.
    Chars ['0','9'] are translated to nibble values [0,9].
    Char '* is translated to nibble value 10.
    Char '#' is translated into nibble value 11.
    Chars ['a','c'] are translated to nibble values [12,14].
    In the case of odd number of chars, last nibble is set to 15.

PARAMETERS:
    src_len [in] : number of chars in src.
    src [in]     : buffer containing the ASCII chars.
    dst [out]    : buffer for the translated bcd number.

RETURN VALUE:
    Number of octets written to dst.
=============================================================================*/
uint8 translate_int_digits(uint8 src_len, const uint8* src, uint8* dst)
{
    uint8 i,j, index=0;
    uint8 nibble = 0;

    for(i=0; i<src_len; i+=2)
    {
        for (j=0; j<2; j++)
        {
            if (i+j < src_len)
            {
                switch (src[i+j])
                {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    nibble = src[i+j]-'0';
                    break;
                case '*':
                    nibble = 0x0a;
                    break;
                case '#':
                    nibble = 0x0b;
                    break;
                case 'a':
                case 'b':
                case 'c':
                    nibble = src[i+j]-'a'+0x0c;
                    break;
                default:
                    assert("NOT REACHABLE" == NULL);
                    break;
                }
            }
            else
                nibble = 0x0f;

            // set nibble
            if (j==0)
                dst[index] = nibble;
            else
                dst[index] |= (nibble << 4);
        }
        index++;
    }
    return index;
}

//********************************************************************************************************************************
//
// External to Internal Translation
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_ext_ie_cause

DESCRIPTION:
    Translate cause IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_cause [out]  : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    If diagnostic field is too long, it is truncated.
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_cause(
    uint8 buf_len,
    uint8* buf,
    uint8* poffset,
    SSMsg_ie_cause_type* int_cause)
{
    uint8 offset = *poffset;
    uint8 skip_ie_offset;    // offset to the octet following this ie
    uint8 location;
    uint8 cause_value;
    boolean unknown_coding_standard = FALSE;

    skip_ie_offset = offset+1+buf[offset];

    if (buf[offset] < 2)
    {
        // ie too short. skip it
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    if (offset+buf[offset] >= buf_len)
    {
        // ie exceeds message length. jump to end of message.
        *poffset = buf_len;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    offset++;   // skip length octet

    // coding standard
    switch(get_bits(buf[offset], 5, 2))
    {
    case SSMSG_CODING_STANDARD_GSM:
        int_cause->coding_standard = SSMSG_CODING_STANDARD_GSM;
        break;
    default:
        int_cause->coding_standard = SSMSG_CODING_STANDARD_NOT_SUPPORTED;
        // If a cause IE indicating a coding standard not supported by the receiver is received,
        // cause "interworking, unspecified" shall be assumed. (24.008, section 10.5.4.11)
        unknown_coding_standard = TRUE;
        int_cause->cause_value = SSMSG_CAUSE_VALUE_INTERWORKING_UNSPECIFIED;
        break;
    }

    //location
    switch(location = get_bits(buf[offset], 0, 4))
    {
    case SSMSG_LOCATION_USER:
    case SSMSG_LOCATION_PRIVATE_NETWORK_LOCAL_USER:
    case SSMSG_LOCATION_PUBLIC_NETWORK_LOCAL_USER:
    case SSMSG_LOCATION_TRANSIT_NETWORK:
    case SSMSG_LOCATION_PUBLIC_NETWORK_REMOTE_USER:
    case SSMSG_LOCATION_PRIVATE_NETWORK_REMOTE_USER:
    case SSMSG_LOCATION_INTERNATIONAL_NETWORK:
    case SSMSG_LOCATION_BEYOND_INTERWORKING_POINT:
        int_cause->location = location;
        break;
    default:
        // all other values are reserved. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    if (unknown_coding_standard)
    {
        // if the coding standard is unknown, rest of the octets are skipped
        int_cause->diagnostic_len = 0;
        int_cause->hdr.present = TRUE;
        *poffset = skip_ie_offset;
        return SSUTIL_SUCCESS;
    }
    offset++;   // skip to next octet

    // cause value
    switch(cause_value = get_bits(buf[offset], 0, 7))
    {
    case SSMSG_CAUSE_VALUE_UNASSIGNED_NUMBER:
    case SSMSG_CAUSE_VALUE_NO_ROUTE_TO_DESTINATION:
    case SSMSG_CAUSE_VALUE_CHANNEL_UNACCEPTABLE:
    case SSMSG_CAUSE_VALUE_OPERATOR_DETERMINED_BARRING:
    case SSMSG_CAUSE_VALUE_NORMAL_CALL_CLEARING:
    case SSMSG_CAUSE_VALUE_USER_BUSY:
    case SSMSG_CAUSE_VALUE_NO_USER_RESPONDING:
    case SSMSG_CAUSE_VALUE_USER_ALERTING_NO_ANSWER:
    case SSMSG_CAUSE_VALUE_CALL_REJECTED:
    case SSMSG_CAUSE_VALUE_NUMBER_CHANGED:
    case SSMSG_CAUSE_VALUE_PRE_EMPTION:
    case SSMSG_CAUSE_VALUE_NON_SELECTED_USER_CLEARING:
    case SSMSG_CAUSE_VALUE_DESTINATION_OUT_OF_ORDER:
    case SSMSG_CAUSE_VALUE_INVALID_NUMBER_FORMAT:
    case SSMSG_CAUSE_VALUE_FACILITY_REJECTED:
    case SSMSG_CAUSE_VALUE_RESPONSE_TO_STATUS_ENQUIRY:
    case SSMSG_CAUSE_VALUE_NORMAL_UNSPECIFIED:
    case SSMSG_CAUSE_VALUE_NO_CIRCUIT_CHANNEL_AVAILABLE:
    case SSMSG_CAUSE_VALUE_NETWORK_OUT_OF_ORDER:
    case SSMSG_CAUSE_VALUE_TEMPORARY_FAILURE:
    case SSMSG_CAUSE_VALUE_SWITCHING_EQUIPMENT_CONGESTION:
    case SSMSG_CAUSE_VALUE_ACCESS_INFORMATION_DISCARDED:
    case SSMSG_CAUSE_VALUE_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE:
    case SSMSG_CAUSE_VALUE_RESOURCES_UNAVAILABLE_UNSPECIFIED:
    case SSMSG_CAUSE_VALUE_QUALITY_OF_SERVICE_UNAVAILABLE:
    case SSMSG_CAUSE_VALUE_REQUESTED_FACILITY_NOT_SUBSCRIBED:
    case SSMSG_CAUSE_VALUE_INCOMING_CALLS_BARRED_WITHIN_THE_CUG:
    case SSMSG_CAUSE_VALUE_BEARER_CAPABILITY_NOT_AUTHORIZED:
    case SSMSG_CAUSE_VALUE_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE:
    case SSMSG_CAUSE_VALUE_SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED:
    case SSMSG_CAUSE_VALUE_BEARER_SERVICE_NOT_IMPLEMENTED:
    case SSMSG_CAUSE_VALUE_ACM_EQUAL_TO_OR_GREATER_THAN_ACMMAX:
    case SSMSG_CAUSE_VALUE_REQUESTED_FACILITY_NOT_IMPLEMENTED:
    case SSMSG_CAUSE_VALUE_ONLY_RESTRICTED_DIGITAL_INFORMATION_BEARER_CAPABILITY_IS_AVAILABLE:
    case SSMSG_CAUSE_VALUE_SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED:
    case SSMSG_CAUSE_VALUE_INVALID_TRANSACTION_IDENTIFIER_VALUE:
    case SSMSG_CAUSE_VALUE_USER_NOT_MEMBER_OF_CUG:
    case SSMSG_CAUSE_VALUE_INCOMPATIBLE_DESTINATION:
    case SSMSG_CAUSE_VALUE_INVALID_TRANSIT_NETWORK_SELECTION:
    case SSMSG_CAUSE_VALUE_SEMANTICALLY_INCORRECT_MESSAGE:
    case SSMSG_CAUSE_VALUE_INVALID_MANDATORY_INFORMATION:
    case SSMSG_CAUSE_VALUE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
    case SSMSG_CAUSE_VALUE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE:
    case SSMSG_CAUSE_VALUE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED:
    case SSMSG_CAUSE_VALUE_CONDITIONAL_IE_ERROR:
    case SSMSG_CAUSE_VALUE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE:
    case SSMSG_CAUSE_VALUE_RECOVERY_ON_TIMER_EXPIRY:
    case SSMSG_CAUSE_VALUE_PROTOCOL_ERROR_UNSPECIFIED:
    case SSMSG_CAUSE_VALUE_INTERWORKING_UNSPECIFIED:
        int_cause->cause_value = cause_value;
        break;
    default:
        // all other values are reserved. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    offset++;   // skip to next octet (could be beyond ie)

    // rest of the octets are copied as is to the diagnostic field up to the
    // buffer size.
    int_cause->diagnostic_len = 
        MIN(skip_ie_offset-offset, sizeof(int_cause->diagnostic));
    MEMCPY(int_cause->diagnostic, buf+offset, int_cause->diagnostic_len);

    int_cause->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_calling_party_bcd_number

DESCRIPTION:
    Translate Calling Party BCD Number IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_calling_party_bcd_number [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_calling_party_bcd_number(
    uint8 buf_len,
    uint8* buf,
    uint8* poffset,
    SSMsg_ie_calling_party_bcd_number_type* int_calling_party_bcd_number)
{
    uint8 offset = *poffset;
    uint8 ie_len = buf[offset];
    uint8 skip_ie_offset = offset+1+ie_len;
    uint8 ton, np_id, pi;

    // check ie length
    if (ie_len < 1)
    {
        // ie too short
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // type of number
    switch(ton = get_bits(buf[offset],4,3))
    {
    case SSMSG_TON_UNKNOWN:
    case SSMSG_TON_INTERNATIONAL_NUMBER:
    case SSMSG_TON_NATIONAL_NUMBER:
    case SSMSG_TON_NETWORK_SPECIFIC_NUMBER:
    case SSMSG_TON_DEDICATED_ACCESS_SHORT_CODE:
        int_calling_party_bcd_number->ton = ton;
        break;
    default:
        // reserved type of number. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // numbering plan identification
    switch(np_id = get_bits(buf[offset],0,4))
    {
    case SSMSG_NP_ID_UNKNOWN:
    case SSMSG_NP_ID_ISDN_TELEPHONY:
    case SSMSG_NP_ID_DATA:
    case SSMSG_NP_ID_TELEX:
    case SSMSG_NP_ID_NATIONAL:
    case SSMSG_NP_ID_PRIVATE:
        int_calling_party_bcd_number->np_id = np_id;
        break;
    default:
        // reserved numbering plan identification. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    // check extension bit
    if (get_bits(buf[offset],7,1) == 0)
    {
        // skip first octet
        SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
        
        // check extension bit = 1
        if (get_bits(buf[offset],7,1) != 1)
        {
            // unexpected extension bit
            *poffset = skip_ie_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        }
        
        // presentation indicator
        switch(pi = get_bits(buf[offset],5,2))
        {
        case SSMSG_PI_PRESENTATION_ALLOWED:
        case SSMSG_PI_PRESENTATION_RESTRICTED:
        case SSMSG_PI_NUMBER_NOT_AVAILABLE_DUE_TO_INTERWORKING:
            int_calling_party_bcd_number->pi = pi;
            break;
        default:
            // reserved presentation indicator. skip ie
            *poffset = skip_ie_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
        // screening indicator, no reserved values
        int_calling_party_bcd_number->si = get_bits(buf[offset],0,2);
    }
    else
    {
        // default pi and si
        int_calling_party_bcd_number->pi = SSMSG_PI_PRESENTATION_ALLOWED;
        int_calling_party_bcd_number->si = SSMSG_SI_USER_PROVIDED_NOT_SCREENED;
    }

    // skip to number field
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);

    // check address length
    if ((skip_ie_offset-offset) > SSMSG_MAX_ADDRESS_LENGTH)
    {
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // translate number from 4-bit to ascii
    int_calling_party_bcd_number->number_len = 
        translate_ext_digits((uint8)(skip_ie_offset-offset),  buf+offset, int_calling_party_bcd_number->number);
    
    int_calling_party_bcd_number->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_calling_party_subaddress

DESCRIPTION:
    Translate Calling Party Subaddress IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_calling_party_subaddress [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    If subaddress field is too long, it is truncated.
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_calling_party_subaddress(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_ie_calling_party_subaddress_type* int_calling_party_subaddress)
{
    uint8 offset = *poffset;
    uint8 ie_len = buf[offset];
    uint8 skip_ie_offset = offset+1+ie_len;
    uint8 tos;

    // check ie length
    if (ie_len < 1)
    {
        // ie too short
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // check extension bit = 1
    if (get_bits(buf[offset],7,1) != 1)
    {
        // unexpected extension bit
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // type of subaddress
    switch(tos = get_bits(buf[offset],4,3))
    {
    case SSMSG_TOS_NSAP:
    case SSMSG_TOS_USER_SPECIFIED:
        int_calling_party_subaddress->tos = tos;
        break;
    default:
        // reserved type of subaddress. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // odd/even indicator
    int_calling_party_subaddress->odd = (get_bits(buf[offset],3,1) == 1);

    // skip first octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // copy subaddress info as is up to the buffer size
    int_calling_party_subaddress->subaddress_len = 
        MIN(ie_len-1, sizeof(int_calling_party_subaddress->subaddress));
    MEMCPY(int_calling_party_subaddress->subaddress, buf+offset, 
        int_calling_party_subaddress->subaddress_len);
    
    int_calling_party_subaddress->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_called_party_bcd_number

DESCRIPTION:
    Translate Called Party BCD Number IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_called_party_bcd_number [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_called_party_bcd_number(
        uint8 buf_len,
        uint8* buf,
        uint8* poffset,
        SSMsg_ie_called_party_bcd_number_type* int_called_party_bcd_number)
{
    uint8 offset = *poffset;
    uint8 ie_len = buf[offset];
    uint8 skip_ie_offset = offset+1+ie_len;
    uint8 ton, np_id;

    // check ie length
    if (ie_len < 1)
    {
        // ie too short
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // type of number
    switch(ton = get_bits(buf[offset],4,3))
    {
    case SSMSG_TON_UNKNOWN:
    case SSMSG_TON_INTERNATIONAL_NUMBER:
    case SSMSG_TON_NATIONAL_NUMBER:
    case SSMSG_TON_NETWORK_SPECIFIC_NUMBER:
    case SSMSG_TON_DEDICATED_ACCESS_SHORT_CODE:
        int_called_party_bcd_number->ton = ton;
        break;
    default:
        // reserved type of number. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // numbering plan identification
    switch(np_id = get_bits(buf[offset],0,4))
    {
    case SSMSG_NP_ID_UNKNOWN:
    case SSMSG_NP_ID_ISDN_TELEPHONY:
    case SSMSG_NP_ID_DATA:
    case SSMSG_NP_ID_TELEX:
    case SSMSG_NP_ID_NATIONAL:
    case SSMSG_NP_ID_PRIVATE:
        int_called_party_bcd_number->np_id = np_id;
        break;
    default:
        // reserved numbering plan identification. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    // check extension bit = 1
    if (get_bits(buf[offset],7,1) != 1)
    {
        // unexpected extension bit
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip to number field
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);

    // check address length
    if (ie_len-1 > SSMSG_MAX_ADDRESS_LENGTH)
    {
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // translate number from 4-bit to ascii
    int_called_party_bcd_number->number_len = 
        translate_ext_digits((uint8)(ie_len-1),  buf+offset, int_called_party_bcd_number->number);
    
    int_called_party_bcd_number->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_called_party_subaddress

DESCRIPTION:
    Translate Called Party Subaddress IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_called_party_subaddress [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    If subaddress field is too long, it is truncated.
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_called_party_subaddress(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,  // offset to the length octet of the called party subaddress ie
            SSMsg_ie_called_party_subaddress_type* int_called_party_subaddress)
{
    uint8 offset = *poffset;
    uint8 ie_len = buf[offset];
    uint8 skip_ie_offset = offset+1+ie_len;
    uint8 tos;

    // check ie length
    if (ie_len < 1)
    {
        // ie too short
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // check extension bit = 1
    if (get_bits(buf[offset],7,1) != 1)
    {
        // unexpected extension bit
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // type of subaddress
    switch(tos = get_bits(buf[offset],4,3))
    {
    case SSMSG_TOS_NSAP:
    case SSMSG_TOS_USER_SPECIFIED:
        int_called_party_subaddress->tos = tos;
        break;
    default:
        // reserved type of subaddress. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // odd/even indicator
    int_called_party_subaddress->odd = (get_bits(buf[offset],3,1) == 1);

    // skip first octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // copy subaddress info as is up to the buffer size
    int_called_party_subaddress->subaddress_len = 
        MIN(ie_len-1, sizeof(int_called_party_subaddress->subaddress));
    MEMCPY(int_called_party_subaddress->subaddress, buf+offset, 
        int_called_party_subaddress->subaddress_len);
    
    int_called_party_subaddress->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_cause_of_no_cli

DESCRIPTION:
    Translate Cause Of No CLI IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_cause_of_no_cli [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_cause_of_no_cli(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_ie_cause_of_no_cli_type* int_cause_of_no_cli)
{
    uint8 offset = *poffset;
    uint8 skip_ie_offset = offset+1+buf[offset];
    uint8 cause_of_no_cli_value;

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected length
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_ie_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
    
    switch (cause_of_no_cli_value = buf[offset])
    {
    case SSMSG_CAUSE_OF_NO_CLI_UNAVAILABLE:
    case SSMSG_CAUSE_OF_NO_CLI_REJECT_BY_USER:
    case SSMSG_CAUSE_OF_NO_CLI_INTERACTION_WITH_OTHER_SERVICE:
    case SSMSG_CAUSE_OF_NO_CLI_COIN_LINE_PAYPHONE:
        int_cause_of_no_cli->value = cause_of_no_cli_value;
        offset++;      // skip value octet
        break;
    default:
        // unknown cause of no cli. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    int_cause_of_no_cli->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_signal

DESCRIPTION:
    Translate Signal IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_signal [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_signal(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_ie_signal_type* int_signal)
{
    uint8 offset = *poffset;
    uint8 skip_ie_offset = offset+1;
    uint8 signal_value;
    
    switch (signal_value = buf[offset])
    {
    case SSMSG_SIGNAL_VALUE_DIAL_TONE_ON:
    case SSMSG_SIGNAL_VALUE_RING_BACK_TONE_ON:
    case SSMSG_SIGNAL_VALUE_INTERCEPT_TONE_ON:
    case SSMSG_SIGNAL_VALUE_NETWORK_CONGESTION_TONE_ON:
    case SSMSG_SIGNAL_VALUE_BUSY_TONE_ON:
    case SSMSG_SIGNAL_VALUE_CONFIRM_TONE_ON:
    case SSMSG_SIGNAL_VALUE_ANSWER_TONE_ON:
    case SSMSG_SIGNAL_VALUE_CALL_WAITING_TONE_ON:
    case SSMSG_SIGNAL_VALUE_OFF_HOOK_WARNING_TONE_ON:
    case SSMSG_SIGNAL_VALUE_TONES_OFF:
    case SSMSG_SIGNAL_VALUE_ALERTING_OFF:
        int_signal->signal_value = signal_value;
        offset++;      // skip value octet
        break;
    default:
        // unknown cause of no cli. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    int_signal->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_ie_progress_indicator

DESCRIPTION:
    Translate Progress Indicator IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_progress_indicator [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_progress_indicator(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_ie_progress_indicator_type* int_progress_indicator)
{
    uint8 offset = *poffset;
    uint8 skip_ie_offset;    // offset to the octet following this ie
    uint8 location;
    uint8 progress_description;
    boolean unknown_coding_standard = FALSE;

    skip_ie_offset = offset+1+buf[offset];

    if (buf[offset] < 4)
    {
        // ie too short. skip it
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    if (offset+buf[offset] >= buf_len)
    {
        // ie exceeds message length. jump to end of message.
        *poffset = buf_len;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    offset++;   // skip length octet

    // coding standard
    switch(get_bits(buf[offset], 5, 2))
    {
    case SSMSG_CODING_STANDARD_GSM:
        int_progress_indicator->coding_standard = SSMSG_CODING_STANDARD_GSM;
        break;
    default:
        int_progress_indicator->coding_standard = SSMSG_CODING_STANDARD_NOT_SUPPORTED;
        // If a cause IE indicating a coding standard not supported by the receiver is received,
        // progress description "Unspecific" shall be assumed. (24.008, section 10.5.4.21)
        unknown_coding_standard = TRUE;
        int_progress_indicator->progress_description = SSMSG_PROGRESS_DESCRIPTION_UNSPECIFIC;
        break;
    }

    //location
    switch(location = get_bits(buf[offset], 0, 4))
    {
    case SSMSG_LOCATION_USER:
    case SSMSG_LOCATION_PRIVATE_NETWORK_LOCAL_USER:
    case SSMSG_LOCATION_PUBLIC_NETWORK_LOCAL_USER:
    case SSMSG_LOCATION_PUBLIC_NETWORK_REMOTE_USER:
    case SSMSG_LOCATION_PRIVATE_NETWORK_REMOTE_USER:
    case SSMSG_LOCATION_BEYOND_INTERWORKING_POINT:
        int_progress_indicator->location = location;
        break;
    default:
        // all other values are reserved. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    if (unknown_coding_standard)
    {
        // if the coding standard is unknown, rest of the octets are skipped
        *poffset = skip_ie_offset;
        return SSUTIL_SUCCESS;
    }
    offset++;   // skip to next octet

    // progress description
    switch(progress_description = get_bits(buf[offset], 0, 7))
    {
    case SSMSG_PROGRESS_DESCRIPTION_NOT_END_TO_END_PLMN_ISDN_IN_BAND_MAY_BE_AVAILABLE:
    case SSMSG_PROGRESS_DESCRIPTION_DESTINATION_ADDRESS_IN_NON_PLMN_ISDN:
    case SSMSG_PROGRESS_DESCRIPTION_ORIGINATION_ADDRESS_IN_NON_PLMN_ISDN:
    case SSMSG_PROGRESS_DESCRIPTION_RETURNED_TO_THE_PLMN_ISDN:
    case SSMSG_PROGRESS_DESCRIPTION_IN_BAND_NOW_AVAILABLE:
    case SSMSG_PROGRESS_DESCRIPTION_END_TO_END_PLMN_ISDN:
    case SSMSG_PROGRESS_DESCRIPTION_QUEUEING:
    case SSMSG_PROGRESS_DESCRIPTION_UNSPECIFIC:
        int_progress_indicator->progress_description = progress_description;
        break;
    default:
        // all other values are reserved. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    int_progress_indicator->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_setup

DESCRIPTION:
    Translate SETUP message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_setup(uint8 buf_len,
                                      uint8* buf,
                                      SSMsg_msg_setup_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header

    int_msg->bearer_capability_1.hdr.present = FALSE;
    int_msg->signal.hdr.present = FALSE;
    int_msg->called_party_bcd_number.hdr.present = FALSE;
    int_msg->called_party_subaddress.hdr.present = FALSE;
    int_msg->calling_party_bcd_number.hdr.present = FALSE;
    int_msg->calling_party_subaddress.hdr.present = FALSE;
    int_msg->cause_of_no_cli.hdr.present = FALSE;

    // optional ie's
    while (offset < buf_len)
    {
        switch (buf[offset])
        {
        case SSMSG_IE_TAG_SIGNAL:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_signal(buf_len, buf, &offset, &int_msg->signal);
            break;
        case SSMSG_IE_TAG_CALLING_PARTY_BCD_NUMBER:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_calling_party_bcd_number(buf_len, buf, &offset, &int_msg->calling_party_bcd_number);
            break;
        case SSMSG_IE_TAG_CALLING_PARTY_SUBADDRESS:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_calling_party_subaddress(buf_len, buf, &offset, &int_msg->calling_party_subaddress);
            break;
        case SSMSG_IE_TAG_CALLED_PARTY_BCD_NUMBER:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_called_party_bcd_number(buf_len, buf, &offset, &int_msg->called_party_bcd_number);
            break;
        case SSMSG_IE_TAG_CALLED_PARTY_SUBADDRESS:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_called_party_subaddress(buf_len, buf, &offset, &int_msg->called_party_subaddress);
            break;
        case SSMSG_IE_TAG_CAUSE_OF_NO_CLI:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_cause_of_no_cli(buf_len, buf, &offset, &int_msg->cause_of_no_cli);
            break;
        default:
            // unknown IEI in the optional part of the message shall be ignored (24.008, section 8.6)
            // from 24.007, section 11.2.4:
            // Bit 8 of the IEI octet is set to "1" indicates a TV formatted type 1 standard IE or a T formatted type 2 IEs, and to
            // "0" indicates a TLV formatted type 4 IE. Hence, a 1 valued bit 8 indicates that the whole IE is one octet long,
            // and a 0 valued bit 8 indicates that the following octet is a length octet.
            if (get_bits(buf[offset], 8, 1) == 1)
                offset++;
            else
                // skip the IEI, length octet, value part
                offset+=(2+buf[offset+1]);
        }
    }

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_hold_reject

DESCRIPTION:
    Translate HOLD REJECT message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_hold_reject(uint8 buf_len,
                                            uint8* buf,
                                            SSMsg_msg_hold_reject_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header
    SSUtil_error_type result;

    int_msg->cause.hdr.present = FALSE;

    // mandatory ie's
    result = translate_ext_ie_cause(buf_len, buf, &offset, &int_msg->cause);
    if (result != SSUTIL_SUCCESS)
        return result;

    // no optional ie's

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_retrieve_reject

DESCRIPTION:
    Translate RETRIEVE REJECT message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_retrieve_reject(uint8 buf_len,
                                                uint8* buf,
                                                SSMsg_msg_retrieve_reject_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header
    SSUtil_error_type result;

    int_msg->cause.hdr.present = FALSE;

    // mandatory ie's
    result = translate_ext_ie_cause(buf_len, buf, &offset, &int_msg->cause);
    if (result != SSUTIL_SUCCESS)
        return result;

    // no optional ie's

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_disconnect

DESCRIPTION:
    Translate DISCONNECT message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_disconnect(uint8 buf_len,
                                                uint8* buf,
                                                SSMsg_msg_disconnect_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header
    SSUtil_error_type result;

    int_msg->cause.hdr.present = FALSE;
    int_msg->progress_indicator.hdr.present = FALSE;

    // mandatory ie's
    result = translate_ext_ie_cause(buf_len, buf, &offset, &int_msg->cause);
    if (result != SSUTIL_SUCCESS)
        return result;

    // optional ie's
    while (offset < buf_len)
    {
        switch (buf[offset])
        {
        case SSMSG_IE_TAG_PROGRESS_INDICATOR:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_progress_indicator(buf_len, buf, &offset, &int_msg->progress_indicator);
            break;
        default:
            // unknown IEI in the optional part of the message shall be ignored (24.008, section 8.6)
            // from 24.007, section 11.2.4:
            // Bit 8 of the IEI octet is set to "1" indicates a TV formatted type 1 standard IE or a T formatted type 2 IEs, and to
            // "0" indicates a TLV formatted type 4 IE. Hence, a 1 valued bit 8 indicates that the whole IE is one octet long,
            // and a 0 valued bit 8 indicates that the following octet is a length octet.
            if (get_bits(buf[offset], 8, 1) == 1)
                offset++;
            else
                // skip the IEI, length octet, value part
                offset+=(2+buf[offset+1]);
        }
    }

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_release

DESCRIPTION:
    Translate RELEASE message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_release(uint8 buf_len,
                                                uint8* buf,
                                                SSMsg_msg_release_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header
    boolean first_cause_translated = FALSE;

    int_msg->cause.hdr.present = FALSE;
    int_msg->second_cause.hdr.present = FALSE;

    // optional ie's
    while (offset < buf_len)
    {
        switch (buf[offset])
        {
        case SSMSG_IE_TAG_CAUSE:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            if (!first_cause_translated)
            {
                first_cause_translated = TRUE;
                translate_ext_ie_cause(buf_len, buf, &offset, &int_msg->cause);
            }
            else
                translate_ext_ie_cause(buf_len, buf, &offset, &int_msg->second_cause);
            break;
        default:
            // unknown IEI in the optional part of the message shall be ignored (24.008, section 8.6)
            // from 24.007, section 11.2.4:
            // Bit 8 of the IEI octet is set to "1" indicates a TV formatted type 1 standard IE or a T formatted type 2 IEs, and to
            // "0" indicates a TLV formatted type 4 IE. Hence, a 1 valued bit 8 indicates that the whole IE is one octet long,
            // and a 0 valued bit 8 indicates that the following octet is a length octet.
            if (get_bits(buf[offset], 8, 1) == 1)
                offset++;
            else
                // skip the IEI, length octet, value part
                offset+=(2+buf[offset+1]);
        }
    }

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_release_complete

DESCRIPTION:
    Translate RELEASE COMPLETE message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_release_complete(uint8 buf_len,
                                                 uint8* buf,
                                                 SSMsg_msg_release_complete_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header

    int_msg->cause.hdr.present = FALSE;
    int_msg->facility.hdr.present = FALSE;

    // optional ie's
    while (offset < buf_len)
    {
        switch (buf[offset])
        {
        case SSMSG_IE_TAG_CAUSE:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_cause(buf_len, buf, &offset, &int_msg->cause);
            break;
        case SSMSG_IE_TAG_FACILITY:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, buf_len, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE);
            // unpack
            translate_ext_ie_facility(buf_len, buf, &offset, &int_msg->facility);
            break;
        default:
            // unknown IEI in the optional part of the message shall be ignored (24.008, section 8.6)
            // from 24.007, section 11.2.4:
            // Bit 8 of the IEI octet is set to "1" indicates a TV formatted type 1 standard IE or a T formatted type 2 IEs, and to
            // "0" indicates a TLV formatted type 4 IE. Hence, a 1 valued bit 8 indicates that the whole IE is one octet long,
            // and a 0 valued bit 8 indicates that the following octet is a length octet.
            if (get_bits(buf[offset], 8, 1) == 1)
                offset++;
            else
                // skip the IEI, length octet, value part
                offset+=(2+buf[offset+1]);
        }
    }

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_facility

DESCRIPTION:
    Translate FACILITY message from external to internal format.

PARAMETERS:
    buf_len [in]  : length of the external buffer.
    buf [in]      : external buffer holding the packed message.
    int_msg [out] : placeholder for the unpacked message.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    This function provides a best-effort behavior. 
    Incorrect mandatory IE's will cause it to exit with error value.
    Incorrect or unexpected optional IE's will be skipped.
=============================================================================*/
SSUtil_error_type translate_ext_facility(uint8 buf_len,
                                         uint8* buf,
                                         SSMsg_msg_facility_type *int_msg)
{
    uint8 offset = SSUTIL_MSG_HEADER_LEN;    // skip the header
    SSUtil_error_type result;

    int_msg->facility.hdr.present = FALSE;

    // mandatory ie's
    result = translate_ext_ie_facility(buf_len, buf, &offset, &int_msg->facility);
    if (result != SSUTIL_SUCCESS)
        return result;

    // no optional ie's

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: generic_hdr_ext_to_int

DESCRIPTION:
    Translate GSM1x message header from external to internal format.
    It unpacks the PD/TI octet and also the message type octet.

PARAMETERS:
    buf [in]  : external buffer holding the packed message.
    hdr [out] : placeholder for the unpacked header.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_UNKNOWN_PD for an unknown PD.

COMMENTS:
    This function assumes that buf contains at least 2 bytes.
=============================================================================*/
SSUtil_error_type generic_hdr_ext_to_int(uint8* buf, SSMsg_header_type* hdr)
{
    // PD
    switch (get_bits(buf[0], 0, 4))
    {
    case SSMSG_PD_CC:
        hdr->pd = SSMSG_PD_CC;
        break;
    case SSMSG_PD_SS:
        hdr->pd = SSMSG_PD_SS;
        break;
    default:
        return SSUTIL_ERROR_UNKNOWN_PD;
    }
    
    // TI flag
    if (get_bits(buf[0], 7, 1) == 0)
        hdr->tiflag = FALSE;
    else
        hdr->tiflag = TRUE;
    
    // TI
    hdr->tivalue = get_bits(buf[0], 4, 3);

    // message type
    hdr->msg_type = buf[1];

    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: SSUtilCls_Translate_ext_to_int

DESCRIPTION:
    Translate a GSM1x SS signaling messages from external format (raw buffer) 
    into internal format (c structs).

PARAMETERS:
    po [in]          : Pointer to the ISSUtilCls Interface object
    ext_msg_len [in] : external message buffer length
    ext_msg [in]     : external message buffer
    int_msg [out]    : placeholder for the unpacked message

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.
=============================================================================*/
SSUtil_error_type SSUtilCls_Translate_ext_to_int(ISSUtilCls* po, 
                                                 uint8 ext_msg_len,
                                                 uint8* ext_msg,
                                                 SSMsg_int_msg_type *int_msg)
{
    SSUtil_error_type result;

    if (po == NULL || ext_msg == NULL || int_msg == NULL)
        return SSUTIL_FAILURE;

    if (ext_msg_len < SSUTIL_MSG_HEADER_LEN)
        // message too short
        return SSUTIL_ERROR_MSG_TOO_SHORT;

    if ((result = generic_hdr_ext_to_int(ext_msg, &int_msg->gen_hdr)) != SSUTIL_SUCCESS)
        return result;

    switch(int_msg->gen_hdr.msg_type)
    {
    case SSMSG_MSG_TYPE_ALERTING:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_CALL_PROCEEDING:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_SETUP:
        return translate_ext_setup(ext_msg_len, ext_msg, &int_msg->setup);
        break;
    case SSMSG_MSG_TYPE_CONNECT:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_CONNECT_ACKNOWLEDGE:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_HOLD_REJECT:
        return translate_ext_hold_reject(ext_msg_len, ext_msg, &int_msg->hold_reject);
        break;
    case SSMSG_MSG_TYPE_RETRIEVE_ACKNOWLEDGE:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_RETRIEVE_REJECT:
        return translate_ext_retrieve_reject(ext_msg_len, ext_msg, &int_msg->retrieve_reject);
        break;
    case SSMSG_MSG_TYPE_DISCONNECT:
        return translate_ext_disconnect(ext_msg_len, ext_msg, &int_msg->disconnect);
        break;
    case SSMSG_MSG_TYPE_RELEASE:
        return translate_ext_release(ext_msg_len, ext_msg, &int_msg->release);
        break;
    case SSMSG_MSG_TYPE_RELEASE_COMPLETE:
        return translate_ext_release_complete(ext_msg_len, ext_msg, &int_msg->release_complete);
        break;
    case SSMSG_MSG_TYPE_FACILITY:
        return translate_ext_facility(ext_msg_len, ext_msg, &int_msg->facility);
        break;
    default:
        return SSUTIL_ERROR_UNKNOWN_MSG_TYPE;
    }

    return SSUTIL_SUCCESS;
}

//********************************************************************************************************************************
//
// Internal to External Translation
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_int_ie_cause

DESCRIPTION:
    Translate cause IE from internal to external format.

PARAMETERS:
    int_cause [in]   : IE data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the IE
                       will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed IE.
    buf [out]        : external buffer to place the packed IE into.

RETURN VALUE:
    None.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_ie_cause(SSMsg_ie_cause_type* int_cause, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the ie length octet
    uint8 ie_len_offset = offset++;

    // ext bit
    set_bits(buf+offset, 1, 7, 1);
    // coding standard
    set_bits(buf+offset, (uint8)int_cause->coding_standard, 5, 2);
    // spare bit
    set_bits(buf+offset, 0, 4, 1);
    //location
    set_bits(buf+offset, (uint8)int_cause->location, 0, 4);

    offset++;   // skip to next octet

    // ext bit
    set_bits(buf+offset, 1, 7, 1);
    // cause value
    set_bits(buf+offset, (uint8)int_cause->cause_value, 0, 7);

    offset++;

    // rest of the octets are copied as is to the diagnostic field.
    MEMCPY(buf+offset, int_cause->diagnostic, int_cause->diagnostic_len);
    offset+=int_cause->diagnostic_len;

    // set ie length
    buf[ie_len_offset] = offset-ie_len_offset-1;
    *buf_len = offset;
}

/*=============================================================================
FUNCTION: translate_int_ie_calling_party_subaddress

DESCRIPTION:
    Translate Calling Party Subaddress IE from internal to external format.

PARAMETERS:
    int_calling_party_subaddress [in] : IE data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the IE
                       will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed IE.
    buf [out]        : external buffer to place the packed IE into.

RETURN VALUE:
    None.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_ie_calling_party_subaddress(
        SSMsg_ie_calling_party_subaddress_type* int_calling_party_subaddress, 
        uint8* buf_len, 
        uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the ie length octet
    uint8 ie_len_offset = offset++;

    // extension bit = 1
    set_bits(buf+offset, 1, 7, 1);
    // type of subaddress
    set_bits(buf+offset, (uint8)int_calling_party_subaddress->tos, 4, 3);
    // odd/even indicator
    set_bits(buf+offset, (uint8)int_calling_party_subaddress->odd, 3, 1);
    // spare
    set_bits(buf+offset, 0, 0, 3);

    offset++;

    // rest of the octets are copied as is to the subaddress field.
    MEMCPY(buf+offset, int_calling_party_subaddress->subaddress, int_calling_party_subaddress->subaddress_len);
    offset+=int_calling_party_subaddress->subaddress_len;

    // set ie length
    buf[ie_len_offset] = offset-ie_len_offset-1;
    *buf_len = offset;
}

/*=============================================================================
FUNCTION: translate_int_ie_called_party_bcd_number

DESCRIPTION:
    Translate Called Party BCD Number IE from internal to external format.

PARAMETERS:
    int_called_party_bcd_number [in]: IE data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the IE
                       will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed IE.
    buf [out]        : external buffer to place the packed IE into.

RETURN VALUE:
    None.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_ie_called_party_bcd_number(
        SSMsg_ie_called_party_bcd_number_type* int_called_party_bcd_number, 
        uint8* buf_len, 
        uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the ie length octet
    uint8 ie_len_offset = offset++;
    uint8 translated_number_len;

    // extension bit = 1
    set_bits(buf+offset, 1, 7, 1);
    // type of number
    set_bits(buf+offset, (uint8)int_called_party_bcd_number->ton, 4, 3);
    // numbering plan identification
    set_bits(buf+offset, (uint8)int_called_party_bcd_number->np_id, 0, 4);

    offset++;

    // translate number from ascii to 4-bit
    translated_number_len = translate_int_digits(int_called_party_bcd_number->number_len, 
                                                 int_called_party_bcd_number->number,
                                                 buf+offset);
    offset+=translated_number_len;
    
    // set ie length
    buf[ie_len_offset] = offset-ie_len_offset-1;
    *buf_len = offset;
}

/*=============================================================================
FUNCTION: translate_int_ie_called_party_subaddress

DESCRIPTION:
    Translate Called Party Subaddress IE from internal to external format.

PARAMETERS:
    int_called_party_subaddress [in] : IE data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the IE
                       will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed IE.
    buf [out]        : external buffer to place the packed IE into.

RETURN VALUE:
    None.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_ie_called_party_subaddress(
        SSMsg_ie_called_party_subaddress_type* int_called_party_subaddress, 
        uint8* buf_len, 
        uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the ie length octet
    uint8 ie_len_offset = offset++;

    // extension bit = 1
    set_bits(buf+offset, 1, 7, 1);
    // type of subaddress
    set_bits(buf+offset, (uint8)int_called_party_subaddress->tos, 4, 3);
    // odd/even indicator
    set_bits(buf+offset, (uint8)int_called_party_subaddress->odd, 3, 1);
    // spare
    set_bits(buf+offset, 0, 0, 3);

    offset++;

    // rest of the octets are copied as is to the subaddress field.
    MEMCPY(buf+offset, int_called_party_subaddress->subaddress, int_called_party_subaddress->subaddress_len);
    offset+=int_called_party_subaddress->subaddress_len;

    // set ie length
    buf[ie_len_offset] = offset-ie_len_offset-1;
    *buf_len = offset;
}

/*=============================================================================
FUNCTION: translate_int_ie_bearer_capability

DESCRIPTION:
    Translate Bearer Capability IE from internal to external format.

PARAMETERS:
    int_bearer_capability [in] : IE data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the IE
                       will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed IE.
    buf [out]        : external buffer to place the packed IE into.

RETURN VALUE:
    None.

COMMENTS:
    Current implementation only copies the internal data to the external
    buffer. No further translation is done.
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_ie_bearer_capability(
        SSMsg_ie_bearer_capability_type* int_bearer_capability, 
        uint8* buf_len, 
        uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the ie length octet
    uint8 ie_len_offset = offset++;

    // octets are copied as is to the contents field.
    MEMCPY(buf+offset, int_bearer_capability->contents, int_bearer_capability->contents_len);
    offset+=int_bearer_capability->contents_len;

    // set ie length
    buf[ie_len_offset] = offset-ie_len_offset-1;
    *buf_len = offset;
}

//********************************************************************************************************************************
//
// Facility ie - Internal to External
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: int_add_invoke_id

DESCRIPTION:
    Add a dummy invoke id parameter.

PARAMETERS:
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       parameter will be done into buf, starting at buf_len
                       offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed 
                       parameter.
    buf [out]        : external buffer to place the packed parameter into.

RETURN VALUE:
    None.

COMMENTS:
    This parameter is required only because it is mandatory in the GSM messages
=============================================================================*/
void int_add_invoke_id(uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    // invoke id TLV
    buf[offset++] = SSMSG_PARAMETER_TAG_INVOKE_ID;
    buf[offset++] = 1;
    buf[offset++] = 0;  // dummy value

    *buf_len = offset;
}

/*=============================================================================
FUNCTION: translate_int_component_invoke

DESCRIPTION:
    Translate an Invoke component from internal to external format.

PARAMETERS:
    int_invoke [in]  : component data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       component will be done into buf, starting at buf_len
                       offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed 
                       component.
    buf [out]        : external buffer to place the packed component into.

RETURN VALUE:
    None.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_component_invoke(
        SSMsg_component_invoke_type* int_invoke, 
        uint8* buf_len, 
        uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the component length octet
    uint8 component_len_offset = offset++;

    // mandatory parameters

    // add invoke id parameter
    int_add_invoke_id(&offset, buf);
    assert(int_invoke->operation_code.hdr.present);

    // operation code TLV
    buf[offset++] = SSMSG_PARAMETER_TAG_OPERATION_CODE;
    buf[offset++] = 1;
    buf[offset++] = int_invoke->operation_code.value;

    // set component length
    buf[component_len_offset] = offset-component_len_offset-1;
    *buf_len = offset;
}

/*=============================================================================
FUNCTION: translate_int_ie_facility

DESCRIPTION:
    Translate Facility IE from internal to external format.

PARAMETERS:
    int_facility [in] : IE data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the IE
                       will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed IE.
    buf [out]        : external buffer to place the packed IE into.

RETURN VALUE:
    None.

COMMENTS:
    Current implementation supports only one component in a Facility IE.
    Current implementation supports only Invoke component.
    This function assumes that the internal data contains valid information.
=============================================================================*/
void translate_int_ie_facility(
        SSMsg_ie_facility_type* int_facility, 
        uint8* buf_len, 
        uint8* buf)
{
    uint8 offset = *buf_len;
    // keep a pointer to the ie length octet
    uint8 ie_len_offset = offset++;

    // only a single component in a Facility ie is supported
    if (int_facility->component.gen_hdr.present)
    {
        buf[offset++] = int_facility->component.gen_hdr.component_type;
        switch(int_facility->component.gen_hdr.component_type)
        {
        case SSMSG_COMPONENT_TYPE_INVOKE:
            translate_int_component_invoke(&int_facility->component.invoke, &offset, buf);
            break;

        // other components are currently not expected
/*
        case SSMSG_COMPONENT_TYPE_RETURN_RESULT:
        case SSMSG_COMPONENT_TYPE_RETURN_ERROR:
        case SSMSG_COMPONENT_TYPE_REJECT:
*/
        default:
            ASSERT_NOT_REACHABLE;
            break;
        }
    }

    // set ie length
    buf[ie_len_offset] = offset-ie_len_offset-1;
    *buf_len = offset;
}

//********************************************************************************************************************************
//
// Messages - Internal to External
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_int_setup

DESCRIPTION:
    Translate SETUP message from internal to external format.

PARAMETERS:
    int_msg [in]     : message data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       message will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed message.
    buf [out]        : external buffer to place the packed message into.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function assumes that the internal data contains valid information.
    Specifically, it asserts for the presence of mandatory IE's.
=============================================================================*/
SSUtil_error_type translate_int_setup(SSMsg_msg_setup_type *int_msg, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    if (int_msg->calling_party_subaddress.hdr.present)
    {
        buf[offset++] = SSMSG_IE_TAG_CALLING_PARTY_SUBADDRESS;
        translate_int_ie_calling_party_subaddress(&int_msg->calling_party_subaddress, &offset, buf);
    }

    // mandatory ies
    assert(int_msg->bearer_capability_1.hdr.present);

    buf[offset++] = SSMSG_IE_TAG_BEARER_CAPABILITY;
    translate_int_ie_bearer_capability(&int_msg->bearer_capability_1, &offset, buf);

    assert(int_msg->called_party_bcd_number.hdr.present);

    buf[offset++] = SSMSG_IE_TAG_CALLED_PARTY_BCD_NUMBER;
    translate_int_ie_called_party_bcd_number(&int_msg->called_party_bcd_number, &offset, buf);

    // optional ies
    if (int_msg->called_party_subaddress.hdr.present)
    {
        buf[offset++] = SSMSG_IE_TAG_CALLED_PARTY_SUBADDRESS;
        translate_int_ie_called_party_subaddress(&int_msg->called_party_subaddress, &offset, buf);
    }

    *buf_len = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_int_call_confirmed

DESCRIPTION:
    Translate CALL CONFIRMED message from internal to external format.

PARAMETERS:
    int_msg [in]     : message data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       message will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed message.
    buf [out]        : external buffer to place the packed message into.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
SSUtil_error_type translate_int_call_confirmed(SSMsg_msg_call_confirmed_type *int_msg, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    if (int_msg->cause.hdr.present)
    {
        buf[offset++] = SSMSG_IE_TAG_CAUSE;
        translate_int_ie_cause(&int_msg->cause, &offset, buf);
    }

    *buf_len = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_int_disconnect

DESCRIPTION:
    Translate DISCONNECT message from internal to external format.

PARAMETERS:
    int_msg [in]     : message data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       message will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed message.
    buf [out]        : external buffer to place the packed message into.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
SSUtil_error_type translate_int_disconnect(SSMsg_msg_disconnect_type *int_msg, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    if (int_msg->cause.hdr.present)
    {
        // mandatory IE, no tag.
        translate_int_ie_cause(&int_msg->cause, &offset, buf);
    }

    *buf_len = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_int_release

DESCRIPTION:
    Translate RELEASE message from internal to external format.

PARAMETERS:
    int_msg [in]     : message data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       message will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed message.
    buf [out]        : external buffer to place the packed message into.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
SSUtil_error_type translate_int_release(SSMsg_msg_release_type *int_msg, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    if (int_msg->cause.hdr.present)
    {
        buf[offset++] = SSMSG_IE_TAG_CAUSE;
        translate_int_ie_cause(&int_msg->cause, &offset, buf);
    }
    if (int_msg->second_cause.hdr.present)
    {
        buf[offset++] = SSMSG_IE_TAG_CAUSE;
        translate_int_ie_cause(&int_msg->second_cause, &offset, buf);
    }

    *buf_len = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_int_release_complete

DESCRIPTION:
    Translate RELEASE COMPLETE message from internal to external format.

PARAMETERS:
    int_msg [in]     : message data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       message will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed message.
    buf [out]        : external buffer to place the packed message into.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
SSUtil_error_type translate_int_release_complete(SSMsg_msg_release_complete_type *int_msg, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    if (int_msg->cause.hdr.present)
    {
        buf[offset++] = SSMSG_IE_TAG_CAUSE;
        translate_int_ie_cause(&int_msg->cause, &offset, buf);
    }

    *buf_len = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_int_facility

DESCRIPTION:
    Translate FACILITY message from internal to external format.

PARAMETERS:
    int_msg [in]     : message data to pack.
    buf_len [in,out] : current length of the external buffer. Packing of the 
                       message will be done into buf, starting at buf_len offset.
                       on return, set to the new length of the external buffer
                       which has changed due to the addition of the packed message.
    buf [out]        : external buffer to place the packed message into.

RETURN VALUE:
    SSUTIL_SUCCESS.

COMMENTS:
    This function assumes that the internal data contains valid information.
    Specifically, it asserts for the presence of mandatory IE's.
=============================================================================*/
SSUtil_error_type translate_int_facility(SSMsg_msg_facility_type *int_msg, uint8* buf_len, uint8* buf)
{
    uint8 offset = *buf_len;

    // mandatory ie, no tag
    assert(int_msg->facility.hdr.present);

    translate_int_ie_facility(&int_msg->facility, &offset, buf);

    *buf_len = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: generic_hdr_int_to_ext

DESCRIPTION:
    Translate GSM1x message header from internal to external format.
    It packs the PD/TI data and also the message type data.

PARAMETERS:
    hdr [in]          : message header containing the data to pack.
    ext_msg_len [out] : set to the length of the external buffer after packing
                        the header into it.
    ext_msg [out]     : external buffer to place the packed header into.

RETURN VALUE:
    None.

COMMENTS:
    This function assumes that the internal data contains valid information.
=============================================================================*/
void generic_hdr_int_to_ext(SSMsg_header_type* hdr, uint8* ext_msg_len, uint8* ext_msg)
{
    // PD
    set_bits(ext_msg, (uint8)hdr->pd, 0, 4);
    // TI flag
    set_bits(ext_msg, hdr->tiflag, 7, 1);
    // TI
    set_bits(ext_msg, hdr->tivalue, 4, 3);
    
    (*ext_msg_len)++;

    // message type
    ext_msg[(*ext_msg_len)++] = hdr->msg_type;
}

/*=============================================================================
FUNCTION: SSUtilCls_Translate_int_to_ext

DESCRIPTION:
    Translate a GSM1x SS signaling messages from internal format (c structs)
    into external format (raw buffer).

PARAMETERS:
    po [in]              : Pointer to the ISSUtilCls Interface object
    int_msg [in]         : internal message to pack
    ext_msg_len [in,out] : max length of external message buffer. returns the
                           length of the packed message.
    ext_msg [out]        : placeholder for the packed message

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    This function assumes that int_msg contains a valid message.
=============================================================================*/
SSUtil_error_type SSUtilCls_Translate_int_to_ext(ISSUtilCls *po, 
                                                 SSMsg_int_msg_type *int_msg,
                                                 uint8* ext_msg_len,
                                                 uint8* ext_msg)
{
    uint8 max_ext_msg_len = *ext_msg_len;
    uint8 tmp_byte;
    
    if (po == NULL || ext_msg == NULL || int_msg == NULL)
        return SSUTIL_FAILURE;
    
    *ext_msg_len = 0;
    
    generic_hdr_int_to_ext(&int_msg->gen_hdr, ext_msg_len, ext_msg);
    // remember the first octet. it will be used to check for overflow.
    tmp_byte = ext_msg[0];
    
    switch(int_msg->gen_hdr.msg_type)
    {
    case SSMSG_MSG_TYPE_ALERTING:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_SETUP:
        return translate_int_setup(&int_msg->setup, ext_msg_len, ext_msg);
        break;
    case SSMSG_MSG_TYPE_CONNECT:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_CALL_CONFIRMED:
        return translate_int_call_confirmed(&int_msg->call_confirmed, ext_msg_len, ext_msg);
        break;
    case SSMSG_MSG_TYPE_CONNECT_ACKNOWLEDGE:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_HOLD:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_RETRIEVE:
        // trivial message. nothing to do
        break;
    case SSMSG_MSG_TYPE_DISCONNECT:
        return translate_int_disconnect(&int_msg->disconnect, ext_msg_len, ext_msg);
        break;
    case SSMSG_MSG_TYPE_RELEASE:
        return translate_int_release(&int_msg->release, ext_msg_len, ext_msg);
        break;
    case SSMSG_MSG_TYPE_RELEASE_COMPLETE:
        return translate_int_release_complete(&int_msg->release_complete, ext_msg_len, ext_msg);
        break;
    case SSMSG_MSG_TYPE_FACILITY:
        return translate_int_facility(&int_msg->facility, ext_msg_len, ext_msg);
        break;
    default:
        return SSUTIL_ERROR_UNKNOWN_MSG_TYPE;
    }

    // check overflows
    assert((*ext_msg_len <= max_ext_msg_len) &&
           (ext_msg[0] == tmp_byte));
    
    return SSUTIL_SUCCESS;
}

#ifdef SS_LOGGING_FEATURE
//********************************************************************************************************************************
//
// Logging
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: SSUtilCls_HandleLogFileLimit

DESCRIPTION:
    Called when a log file reaches its limit.
    The full log file is renamed as the backup log file (while overwriting the 
    previous one).
    A new log file is then created.

PARAMETERS:
    pMe [in] : Pointer to a SSUtilCls object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_HandleLogFileLimit(SSUtilCls* pMe)
{
    int result;

    // close the log file and rename it as the backup log file
    IFILE_Release(pMe->m_pIFileLog);

    result = SSUtil_RenameLogFile(
                pMe->m_pIFileMgr,
                SSUTIL_LOG_FILE_NAME,
                SSUTIL_LOG_BACKUP_FILE_NAME
                );

    if (result == SUCCESS)
    {
        pMe->m_pIFileLog = SSUtil_CreateNewLogFile(
                              pMe->m_pIFileMgr, 
                              SSUTIL_LOG_FILE_NAME
                              );
        if (pMe->m_pIFileLog == NULL) result = EFAILED;
    }

    // exit point
    if (result == SUCCESS)
    {
        pMe->m_msgCounter = 0;
    }
    else
    {
        // can't continue logging; logging is deactivated
        pMe->m_bLoggingEnabled = FALSE;
        DBGPRINTF("SSUtil:Logging Deactivated!", 0, 0, 0);
    }
}

/*=============================================================================
FUNCTION: SSUtilCls_InitLogFile

DESCRIPTION:
    Called to initialize the EFS log file.
    If the trigger file doesn't exists, the log file will not be created and
    EFS logging will be disabled.
    If a previous log file exists, it is removed and a new log file is created.

PARAMETERS:
    pMe [in] : Pointer to a SSUtilCls object.

RETURN VALUE:
    TRUE if logs can now be printed to a file.

COMMENTS:
=============================================================================*/
boolean SSUtilCls_InitLogFile(SSUtilCls* pMe)
{
    int result;

    pMe->m_pIFileMgr = NULL;
    pMe->m_pIFileLog = NULL;
    pMe->m_msgCounter = 0;

    // create a FILEMGR
    result = ISHELL_CreateInstance(
                pMe->m_pIShell, 
                AEECLSID_FILEMGR,
                (void **)&pMe->m_pIFileMgr
                );
    if (result != SUCCESS)
    {
        DBGPRINTF("SSUtil:Can't create fileMgr (%d)", result, 0, 0);
    }

    if (result == SUCCESS)
    {
        // check if the trigger file exists
        result = IFILEMGR_Test(pMe->m_pIFileMgr, SSUTIL_LOGGING_TRIGGER_FILE_NAME);
    }

    if (result == SUCCESS)
    {   // the logging trigger file exists and no failure occurred.

        // if log file exists then rename it
        if (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, SSUTIL_LOG_FILE_NAME))
        {
            // if log file exists then rename it
            result = SSUtil_RenameLogFile(
                        pMe->m_pIFileMgr, 
                        SSUTIL_LOG_FILE_NAME,
                        OLD_FILENAME_PREFIX SSUTIL_LOG_FILE_NAME
                        );
        }

        if (result == SUCCESS)
        {
            // if backup log file exists then rename it
            if (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, SSUTIL_LOG_BACKUP_FILE_NAME))
            {
                result = SSUtil_RenameLogFile(
                            pMe->m_pIFileMgr, 
                            SSUTIL_LOG_BACKUP_FILE_NAME, 
                            OLD_FILENAME_PREFIX SSUTIL_LOG_BACKUP_FILE_NAME
                            );
            }
        }

        if (result == SUCCESS)
        {
            // create the log file
            pMe->m_pIFileLog = 
                SSUtil_CreateNewLogFile(pMe->m_pIFileMgr, SSUTIL_LOG_FILE_NAME);

            if (pMe->m_pIFileLog == NULL) result = EFAILED;
        }
    }

    // exit point, if something failed cleanup and disable logging
    if (result == SUCCESS)
    {
        pMe->m_bLoggingEnabled = TRUE;
        return TRUE;
    }
    else
    {
        pMe->m_bLoggingEnabled = FALSE;
        DBGPRINTF("SSUtil:Failed to setup Logging!", 0, 0, 0);
        return FALSE;
    }
}

#endif // SS_LOGGING_FEATURE

/*=============================================================================
FUNCTION: SSUtilCls_Log

DESCRIPTION:
    Output one line to the log file.

PARAMETERS:
    po [in]    : Pointer to the ISSUtilCls Interface object.
    level [in] : debug level string. level doesn't affect the behavior of the 
                 function. It is only added to the logged string.
    fmt [in]   : format string.
    code1, code2, code3 [in] : parameters to go with the format string.

RETURN VALUE:
    None.

COMMENTS:
1.  The logged string is composed as follows:
        "level: (fmt,code1,code2,code3)\n"
    The log file is limited to a predefined number of log messages. When this
    limit is reached, the log file is renamed into a backup log file and a new
    log file is created.

2.  If compilation flag SS_LOGGING_FEATURE is not defined, this function 
    becomes an empty stub.

3.  SNPRINTF is not documented in the API reference. It stays in the buffer, 
    but does not guarantee null termination. Upon failure, like attempting %f, 
    it returns (-1). Upon success, it returns the number of characters written. 
    So writing N characters (and no null), 
    or N-1 characters and a null, will both return N.

    Pseudo code:
    SNPRINTF (buf, sizeof(buf), format, ...) 
    {
        if (error)
            return -1

        else if (strlen(expanded-string) >= sizeof(buf))
            write sizeof(buf) characters, and no null termination
            return sizeof(buf) 

        else
            write strlen(expanded-string) characters, and null termination
            return (strlen(expanded-string) + 1)
    }

=============================================================================*/
void SSUtilCls_Log(ISSUtilCls *po, 
                   const char *level,
                   const char *fmt,
                   uint32      code1,
                   uint32      code2,
                   uint32      code3)
{
#ifdef SS_LOGGING_FEATURE
    SSUtilCls *pMe = (SSUtilCls *) po;

    if (pMe->m_bLoggingEnabled == TRUE)
    {
        // print level string
        int limit = sizeof(pMe->m_msgBuffer)-1; 
        int offset = SNPRINTF(pMe->m_msgBuffer, limit, "%s: ", level);
        int advance;

        // append the given message
        offset--; // overwrite null character.
        limit -= offset;
        advance = SNPRINTF((pMe->m_msgBuffer)+offset, limit,
                                fmt, code1, code2, code3);

        if (advance < 0)
        {            
            return; // error
        }

        offset += advance;

        // add LF instead of null or last character
        pMe->m_msgBuffer[offset-1] = '\n';

        // null termination
        pMe->m_msgBuffer[offset] = '\0';

        // write to file
        IFILE_Write(pMe->m_pIFileLog, pMe->m_msgBuffer, STRLEN(pMe->m_msgBuffer));

        ++pMe->m_msgCounter;

        // temp solution in order to cause the SSUtilCls_HandleLogFileLimit() function 
        // to be called in the context of the SSAgent applet, which is always up and thus
        // keeps the opened files valid (when the app, in whose context the file was opened,
        // closes the open files is forcibly closed!)
        if (pMe->m_msgCounter == SSUTIL_LOG_MSG_COUNT_LIMIT-1)
        {
            
            ISHELL_SendEvent(
                pMe->m_pIShell, 
                AEECLSID_SS_AGENT, 
                EVT_USER_SWAP_LOG_FILES_BY_CALLING_LOG, 
                0, 
                0
                );
        }
        else
        {
            if (pMe->m_msgCounter >= SSUTIL_LOG_MSG_COUNT_LIMIT)
            {
                SSUtilCls_HandleLogFileLimit(pMe);
            }
        }
    }
#endif // SS_LOGGING_FEATURE
}

//********************************************************************************************************************************
//
// Call Counters
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: SSUtilCls_GetCallIndependentCounter

DESCRIPTION:
    Get call independent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    The current value of the call independent counter.
=============================================================================*/
int SSUtilCls_GetCallIndependentCounter(ISSUtilCls *po) { 
    return ((SSUtilCls *)po)->m_CallIndependentCounter; 
}

/*=============================================================================
FUNCTION: SSUtilCls_DecCallIndependentCounter

DESCRIPTION:
    Decrease call independent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_DecCallIndependentCounter(ISSUtilCls *po) { 
    assert(((SSUtilCls *)po)->m_CallIndependentCounter > 0);

    (((SSUtilCls *)po)->m_CallIndependentCounter)--;
}

/*=============================================================================
FUNCTION: SSUtilCls_IncCallIndependentCounter

DESCRIPTION:
    Increase call independent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_IncCallIndependentCounter(ISSUtilCls *po) { 
    (((SSUtilCls *)po)->m_CallIndependentCounter)++;
}

/*=============================================================================
FUNCTION: SSUtilCls_ResetCallIndependentCounter

DESCRIPTION:
    Reset call independent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_ResetCallIndependentCounter(ISSUtilCls *po) { 
    ((SSUtilCls *)po)->m_CallIndependentCounter = 0; 
}


/*=============================================================================
FUNCTION: SSUtilCls_GetCallDependentCounter

DESCRIPTION:
    Get call dependent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    The current value of the call dependent counter.
=============================================================================*/
int SSUtilCls_GetCallDependentCounter(ISSUtilCls *po) { 
    return ((SSUtilCls *)po)->m_CallDependentCounter; 
}

/*=============================================================================
FUNCTION: SSUtilCls_DecCallDependentCounter

DESCRIPTION:
    Decrease call dependent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_DecCallDependentCounter(ISSUtilCls *po) { 
    assert(((SSUtilCls *)po)->m_CallDependentCounter > 0);

    (((SSUtilCls *)po)->m_CallDependentCounter)--;
}

/*=============================================================================
FUNCTION: SSUtilCls_IncCallDependentCounter

DESCRIPTION:
    Increase call dependent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_IncCallDependentCounter(ISSUtilCls *po) { 
    (((SSUtilCls *)po)->m_CallDependentCounter)++;
}

/*=============================================================================
FUNCTION: SSUtilCls_ResetCallDependentCounter

DESCRIPTION:
    Reset call dependent counter.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    None.
=============================================================================*/
void SSUtilCls_ResetCallDependentCounter(ISSUtilCls *po) { 
    ((SSUtilCls *)po)->m_CallDependentCounter = 0; 
}

/*=============================================================================
FUNCTION: SSUtilCls_GetVersionString

DESCRIPTION:
    Get the ssutil's version info string.

PARAMETERS:
    po [in] : Pointer to the ISSUtilCls Interface object.

RETURN VALUE:
    Version Info string.

=============================================================================*/
const AECHAR* SSUtilCls_GetVersionString(ISSUtilCls *po)
{
    SSUtilCls *pMe = (SSUtilCls *) po;

    // make sure that the buffer's initial value is valid
    pMe->m_versionInfoBuffer[0] = '\0';

    /* 
	// this does not work, as it executes in the calling's applet's context.
	ISHELL_LoadResString(
        pMe->m_pIShell, 
        SSUTIL_RES_FILE,
        IDS_VERSION, 
        pMe->m_versionInfoBuffer, 
        sizeof(pMe->m_versionInfoBuffer)
        );
	*/

	STRTOWSTR(
	   SSUTIL_VERSION_STRING,
	   pMe->m_versionInfoBuffer,
	   sizeof(pMe->m_versionInfoBuffer)
	   );

    return pMe->m_versionInfoBuffer;
}

#ifdef SS_LOGGING_FEATURE
/*=============================================================================
FUNCTION: SSUtil_RenameLogFile

DESCRIPTION: Renames a log file, if the a file with the new name already exists 
             then it tries to remove it.

PARAMETERS:
    pFileMgr [in]: pointer to a valid file manager object
    pCurrLogFileName [in]: current log file name. a null terminated string.
    pNewLogFileName [in]: new log file name. a null terminated string.

RETURN VALUE:
    SUCCESS, if successful.
    EFAILED, if unsuccessful.

=============================================================================*/
int SSUtil_RenameLogFile(
    IFileMgr   *pFileMgr, 
    const char *pCurrLogFileName,
    const char *pNewLogFileName
    )
{
    if ((pFileMgr == NULL) || (pCurrLogFileName == NULL) 
                           || (pNewLogFileName == NULL)) return NULL;

    if (SUCCESS == SSUtil_RemoveLogFileIfItExists(pFileMgr, pNewLogFileName))
    {
        return IFILEMGR_Rename(pFileMgr, pCurrLogFileName, pNewLogFileName);
    }
    else
    {
        return EFAILED;
    }
}

/*=============================================================================
FUNCTION: SSUtil_RemoveLogFileIfItExists

DESCRIPTION: Remove a log file if it exists.

PARAMETERS:
    pFileMgr [in]: pointer to a valid file manager object
    pLogFileName [in]: log file name. a null terminated string.
 
RETURN VALUE:
    SUCCESS, if successful.
    EFAILED, if unsuccessful.

=============================================================================*/
int SSUtil_RemoveLogFileIfItExists(
    IFileMgr   *pFileMgr, 
    const char *pLogFileName
    )
{
    if ((pFileMgr == NULL) || (pLogFileName == NULL)) return EFAILED;

    // check if file exists
    if (SUCCESS == IFILEMGR_Test(pFileMgr, pLogFileName))
    {
        // try to remove the file
        if (SUCCESS == IFILEMGR_Remove(pFileMgr, pLogFileName))
        {
            return SUCCESS;
        }
        else
        {
            // can't remove log file.
            DBGPRINTF("SSUtil:Can't remove logfile (%d)", IFILEMGR_GetLastError(pFileMgr), 0, 0);
            return EFAILED;
        }
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION: SSUtil_CreateNewLogFile

DESCRIPTION: Creates a new log file with a specified name.

PARAMETERS:
    pFileMgr [in]: pointer to a valid file manager object
    pLogFileName [in]: log file name. a null terminated string.
 
RETURN VALUE:
    New IFile instance if successful,
    NULL if unsuccessful.

=============================================================================*/
IFile* SSUtil_CreateNewLogFile(
    IFileMgr   *pFileMgr, 
    const char *pLogFileName
    )
{
    IFile* result;
    if ((pFileMgr == NULL) || (pLogFileName == NULL)) return NULL;

    result = IFILEMGR_OpenFile(pFileMgr, pLogFileName, _OFM_CREATE);
    if (result == NULL)
    {
        DBGPRINTF("SSUtil:Can't create logfile (%d)", IFILEMGR_GetLastError(pFileMgr), 0, 0);
    }

    return result;
}

#endif // SS_LOGGING_FEATURE

/*========================End of File ================================================*/

