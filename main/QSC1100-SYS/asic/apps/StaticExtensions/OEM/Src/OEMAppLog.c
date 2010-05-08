/*============================================================================

FILE:  OEMAppLog.c

SERVICES: Application Logging Extension

==============================================================================

               Copyright © 2006 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

/* Include DMSS headers before BREW headers. */
#ifdef AEE_SIMULATOR
#include "sim_log.h"
#include "sim_diag.h"
#else
#include "log.h"
#include "diag.h"
#endif

#include "AEE.h"            /* Standard AEE declarations */
#include "AEESTDLIB.h"      /* STRTOUL, UTF8TOWSTR*/ 
#include "AEEX509Chain.h"   /* IX509Chain */
#include "AEEShell.h"       /* AEE Shell services, IShell declarations */
#include "AEEError.h"       /* Error codes for return values */
#include "brewversion.h"

#include "AEEAppLog.h"
#include "AEECLSID_APPLOG.bid"

// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715, param) */

// This should only be used for function parameters, not for local variables
// Pointer parameter could be declared as pointing to const
#define PARAM_NOT_CONST(param)       /*lint -esym(818, param) */

#define ALE_ICD_LOG_CODE        0x12F8

// max diag packet should be 512 bytes
// Therefore, ALE_MAX_LOG_SIZE + ALE header (12b) + log header (12b) +
// diag header shouldn't exceed 512 bytes. 
// So we keep ALE_MAX_LOG_SIZE = 475
#define ALE_MAX_LOG_SIZE        475
#define ALE_VERIFY_TIMER        1000
#define ALE_MAX_NONCE_RETRIES   100

// FPOS may be defined in one of the includes for log.h or diag.h. 
// Or else we define it ourselves.
#if (!defined(FPOS))
#define FPOS( type, field ) \
    /*lint -e545 */ ( (dword) &(( type *) 0)-> field ) /*lint +e545 */
#endif
 
//===========================================================================
// DEBUG Messages for AppLog
//
// This ext. uses the following MACROS to display DEBUG messages instead
// of calling ALE_MSG() directly.
//
// How to use:
// 1. To display FATAL messages, e.g:
//      ALE_MSG( FATAL, ("This is FATAL %d msg", nStatus) );
//
// 2. To display ERROR messages, e.g:
//      ALE_MSG( ERROR, ("This is ERROR %d msg", nStatus) );
//
// 3. To display HIGH severity message, e.g:
//      ALE_MSG( HIGH, ("This is HIGH %d msg", nStatus) );
//
// 4. To display MEDIUM severity message, e.g:
//      ALE_MSG( MED, ("This is MED %d msg", nStatus) );
//
// 5. To display LOW severity message, e.g:
//      ALE_MSG( LOW, ("This is LOW %d msg", nStatus) );
//
// 6. For Debugging purpose only, should not be enabled in prod., e.g:
//      ALE_MSG( DEBUG, ("This is LOW %d msg", nStatus) );
//===========================================================================
/*lint -e830*/
/*lint -e750*/
#define ALE_MSG_FATAL               1
#define ALE_MSG_ERROR               1
#define ALE_MSG_HIGH                1
#define ALE_MSG_MED                 0 
#define ALE_MSG_LOW                 0   
#define ALE_MSG_DEBUG               0   

// Lint complained about the following constant evaluations   
// 506 Constant value boolean
// 774 Boolean always evaluates to false/true
#define ALE_MSG(x,p) \
    /*lint -e506 -e774 */ \
    if ( ALE_MSG_##x == 1 ) DBGPRINTF p \
    /*lint +e506 +e774 */

#ifdef AEE_SIMULATOR
#pragma pack( push, 1 )              // VC++ PACKED
#endif
/*lint -esym(754, header)*/
typedef PACKED struct {
  log_hdr_type    header;          // log code header
  AEECLSID        dwClsid;         // Brew Class Id
  uint32          dwAppLogId;      // App. speciic log id (not log code)
  uint16          wAppLogSize;     // Size of the application log (max 500)
  byte            pbAppLogData[1]; // Logging data sent by app (flexible)
} AppLog_LogType;
#ifdef AEE_SIMULATOR
#pragma pack( pop )
#endif

typedef struct {
  uint32      * m_poutRes;
  AEECallback * m_pCallerCB;
} AppLog_UnlockUD;

typedef struct IAppLog AppLog;
struct IAppLog
{
  AEEVTBL(IAppLog)    * pvt;
  
  // Standard data members
  IShell              * pIShell;
#if !defined (AEE_STATIC)
  IModule             * pIModule;
#endif
  uint32              nRefs;
  uint32              dwNonce32;
  uint16              wNonce16;

  AEECLSID            dwAppClsId;

  X509TrustResult     * pResVerify;
  IX509Chain          * pX509Chain;
  AppLog_UnlockUD     * pUnlock;

  AEECallback         * pcbVerify;
  WebOpt              * pWebOpts;
  uint8               * pUserCert;
};

/*============================================================================
STATIC DATA
============================================================================*/
// The DER encoded root X509 cert (public key). Supplied client certs are 
// checked to be signed using this CA. If the check fails, we reject the 
// client cert.
static const uint8 cacert[]={
0x30, 0x82, 0x04, 0x41, 0x30, 0x82, 0x03, 0x29, 0xa0, 0x03, 0x02, 0x01, 0x02, 
0x02, 0x10, 0x31, 0x37, 0xed, 0x55, 0xe3, 0xa0, 0x8a, 0xb2, 0x42, 0xf9, 0x8f, 
0xc4, 0x48, 0xf1, 0xda, 0x58, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 
0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x81, 0xb2, 0x31, 0x37, 0x30, 
0x35, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 
0x28, 0x63, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x2d, 
0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x2e, 0x61, 0x64, 0x6d, 
0x69, 0x6e, 0x40, 0x71, 0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 0x2e, 0x63, 
0x6f, 0x6d, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 
0x55, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x0a, 
0x43, 0x61, 0x6c, 0x69, 0x66, 0x6f, 0x72, 0x6e, 0x69, 0x61, 0x31, 0x12, 0x30, 
0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 0x09, 0x53, 0x61, 0x6e, 0x20, 0x44, 
0x69, 0x65, 0x67, 0x6f, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x0a, 
0x13, 0x08, 0x51, 0x55, 0x41, 0x4c, 0x43, 0x4f, 0x4d, 0x4d, 0x31, 0x2e, 0x30, 
0x2c, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x25, 0x4d, 0x6f, 0x62, 0x69, 0x6c, 
0x65, 0x56, 0x69, 0x65, 0x77, 0x2d, 0x49, 0x41, 0x70, 0x70, 0x4c, 0x6f, 0x67, 
0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x20, 
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x36, 0x31, 
0x30, 0x32, 0x35, 0x31, 0x30, 0x33, 0x32, 0x30, 0x33, 0x5a, 0x17, 0x0d, 0x33, 
0x31, 0x31, 0x30, 0x32, 0x35, 0x31, 0x30, 0x33, 0x39, 0x30, 0x34, 0x5a, 0x30, 
0x81, 0xb2, 0x31, 0x37, 0x30, 0x35, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 
0x0d, 0x01, 0x09, 0x01, 0x16, 0x28, 0x63, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 
0x63, 0x61, 0x74, 0x65, 0x2d, 0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 
0x79, 0x2e, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x40, 0x71, 0x75, 0x61, 0x6c, 0x63, 
0x6f, 0x6d, 0x6d, 0x2e, 0x63, 0x6f, 0x6d, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 
0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 
0x55, 0x04, 0x08, 0x13, 0x0a, 0x43, 0x61, 0x6c, 0x69, 0x66, 0x6f, 0x72, 0x6e, 
0x69, 0x61, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 0x09, 
0x53, 0x61, 0x6e, 0x20, 0x44, 0x69, 0x65, 0x67, 0x6f, 0x31, 0x11, 0x30, 0x0f, 
0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x08, 0x51, 0x55, 0x41, 0x4c, 0x43, 0x4f, 
0x4d, 0x4d, 0x31, 0x2e, 0x30, 0x2c, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x25, 
0x4d, 0x6f, 0x62, 0x69, 0x6c, 0x65, 0x56, 0x69, 0x65, 0x77, 0x2d, 0x49, 0x41, 
0x70, 0x70, 0x4c, 0x6f, 0x67, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 
0x63, 0x61, 0x74, 0x65, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x30, 0x82, 
0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 
0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 
0x02, 0x82, 0x01, 0x01, 0x00, 0x97, 0xad, 0x42, 0x75, 0xbe, 0x8d, 0xd0, 0x9f, 
0x7c, 0x09, 0x29, 0xd0, 0xb4, 0x9f, 0xdf, 0x60, 0x5d, 0xa6, 0xe6, 0x69, 0xbc, 
0x30, 0x25, 0x1d, 0x32, 0x54, 0xb7, 0xd1, 0x2a, 0x6b, 0x84, 0xf1, 0xb0, 0x42, 
0x4d, 0x71, 0x3a, 0x93, 0x9c, 0xa5, 0x1e, 0xb1, 0x16, 0x7d, 0x92, 0xe6, 0xca, 
0x34, 0x11, 0xea, 0x3e, 0xd8, 0x04, 0x07, 0xc4, 0x15, 0xa0, 0xa2, 0xf9, 0x01, 
0xe5, 0x6b, 0xc3, 0x1b, 0xd7, 0x9b, 0x75, 0x9a, 0xb6, 0xb8, 0xd5, 0x3b, 0x6a, 
0x60, 0xd6, 0x16, 0x9a, 0x9b, 0xb0, 0x4f, 0x05, 0x7c, 0x78, 0x95, 0x64, 0x8f, 
0xcc, 0xc0, 0x20, 0xf8, 0xc6, 0x48, 0x02, 0x4d, 0x73, 0x57, 0x98, 0x21, 0xb4, 
0xb7, 0x73, 0x98, 0x0a, 0x4f, 0xfe, 0x9b, 0xeb, 0x18, 0xaa, 0xf0, 0xa1, 0x7b, 
0x02, 0x14, 0x63, 0xb5, 0x6e, 0x58, 0x24, 0x3e, 0xda, 0x7f, 0xc4, 0x6c, 0x08, 
0xc6, 0x91, 0xa6, 0xc4, 0x7f, 0x16, 0xec, 0xb2, 0xdd, 0x55, 0xba, 0x12, 0x55, 
0x45, 0xb2, 0x99, 0xe9, 0x8c, 0x03, 0xba, 0x98, 0x7d, 0x91, 0xbc, 0xf9, 0xca, 
0xe8, 0xd3, 0x84, 0xd6, 0x74, 0x97, 0x70, 0xd8, 0x30, 0x76, 0x10, 0x45, 0xd3, 
0x82, 0x21, 0xc4, 0x8c, 0x41, 0xf7, 0xd6, 0x49, 0xa1, 0x5b, 0xa9, 0x24, 0x85, 
0x2f, 0xaa, 0xc3, 0xbb, 0x11, 0xa9, 0x79, 0x12, 0xdf, 0x0e, 0xc6, 0x4c, 0xa1, 
0xfd, 0x99, 0x5f, 0x92, 0x7a, 0xca, 0xc8, 0x09, 0xe0, 0xb8, 0x88, 0x16, 0x69, 
0x82, 0x56, 0x4d, 0x61, 0xf2, 0xf5, 0x29, 0xed, 0x3b, 0x61, 0x76, 0xed, 0x6c, 
0x6d, 0x90, 0x09, 0xd7, 0x28, 0x94, 0xf3, 0x8a, 0xc0, 0x5e, 0xe3, 0x6f, 0x0c, 
0x9d, 0x3d, 0x3a, 0x31, 0x82, 0x71, 0xa1, 0xa2, 0x95, 0x5d, 0xae, 0x72, 0xbe, 
0xe0, 0x89, 0x28, 0xb3, 0xa8, 0x76, 0x1a, 0xd2, 0x0a, 0x49, 0x8b, 0x95, 0x7b, 
0x09, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x51, 0x30, 0x4f, 0x30, 0x0b, 0x06, 
0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x01, 0x86, 0x30, 0x0f, 0x06, 
0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 
0xff, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x03, 
0x89, 0x70, 0x62, 0xe1, 0xb4, 0x5f, 0x2f, 0xf1, 0x20, 0xa5, 0x5d, 0xd0, 0x8f, 
0x51, 0x58, 0x97, 0xec, 0x45, 0x0a, 0x30, 0x10, 0x06, 0x09, 0x2b, 0x06, 0x01, 
0x04, 0x01, 0x82, 0x37, 0x15, 0x01, 0x04, 0x03, 0x02, 0x01, 0x00, 0x30, 0x0d, 
0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 
0x03, 0x82, 0x01, 0x01, 0x00, 0x2a, 0x41, 0xbe, 0x49, 0x47, 0xdd, 0x97, 0x34, 
0x03, 0xc6, 0x75, 0xcd, 0x55, 0x5c, 0x00, 0x75, 0xf6, 0x99, 0xe5, 0xcd, 0x28, 
0xfe, 0x0c, 0x1a, 0x06, 0x04, 0x2d, 0xfb, 0xb9, 0x3d, 0xe4, 0xc9, 0xa7, 0x8a, 
0x13, 0xe3, 0x3b, 0xbf, 0x03, 0x00, 0x1a, 0x30, 0x62, 0x51, 0x15, 0xac, 0xd6, 
0x65, 0x20, 0x22, 0x9e, 0xe6, 0x56, 0x42, 0x17, 0xee, 0xb2, 0x50, 0x81, 0xa8, 
0xb9, 0x78, 0xcc, 0x53, 0xab, 0x1d, 0xc4, 0x6b, 0x2e, 0xb7, 0x16, 0xf1, 0x8e, 
0xb3, 0x0c, 0x1e, 0xfa, 0x3b, 0x4e, 0x29, 0xcd, 0xb7, 0x1a, 0xfc, 0xe1, 0x30, 
0x57, 0x54, 0x56, 0x2e, 0xea, 0x20, 0xf7, 0x96, 0xdc, 0x2a, 0xa6, 0xd4, 0x97, 
0xff, 0xe4, 0xb9, 0x5a, 0x4d, 0x3b, 0x21, 0xc4, 0x53, 0xdf, 0x7b, 0x45, 0x6e, 
0x54, 0xf3, 0x9c, 0x88, 0x08, 0x21, 0x72, 0xf6, 0x76, 0x0b, 0x4f, 0x0d, 0x46, 
0x6e, 0x4a, 0x9f, 0x08, 0x9d, 0xed, 0x36, 0x76, 0xe0, 0xec, 0x0c, 0xa8, 0xdc, 
0x41, 0xfe, 0x33, 0xcc, 0xe9, 0xd9, 0xb2, 0x13, 0x69, 0xa6, 0x28, 0x19, 0x28, 
0xd3, 0x88, 0x1c, 0x2a, 0x29, 0x4c, 0x0e, 0xa4, 0x06, 0x79, 0x14, 0xc9, 0x8d, 
0xe7, 0x4e, 0xa3, 0x4e, 0xba, 0x31, 0x39, 0x93, 0x0b, 0x88, 0x0e, 0x8c, 0x27, 
0x15, 0x30, 0xc1, 0xf5, 0x71, 0x73, 0xd4, 0x14, 0x10, 0x69, 0x95, 0x11, 0xb7, 
0x7a, 0x31, 0x5e, 0x71, 0xbc, 0x77, 0x10, 0x8e, 0xdc, 0x24, 0x97, 0x24, 0x79, 
0x56, 0x3b, 0x25, 0x6c, 0x39, 0x65, 0x0d, 0x62, 0xe9, 0x8b, 0xd5, 0x22, 0x77, 
0x94, 0x5d, 0x5f, 0xae, 0x51, 0xee, 0x05, 0x3b, 0x8f, 0x88, 0x31, 0x1f, 0xd1, 
0x0e, 0x46, 0x3b, 0x33, 0xc2, 0x52, 0xd3, 0x9d, 0xfb, 0xe5, 0xed, 0xda, 0xcf, 
0xd3, 0x7d, 0x75, 0x43, 0x80, 0xd9, 0x16, 0x01, 0xe9, 0xa1, 0x22, 0xfc, 0xcb, 
0xe2};
#define CERT_LEN    sizeof(cacert)

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static uint32 AppLog_AddRef (IAppLog *pMe);
static uint32 AppLog_Release (IAppLog *pMe);
static int    AppLog_QueryInterface(IAppLog *me, AEECLSID idReq, 
                                       void **ppo);

static uint16 AppLog_SendLog(IAppLog *pAppLog, uint32 dwLogId, 
                               uint16 wLogSz, void *pvLogData,
                               uint32 dwNonce32);
static uint16 AppLog_SendLog_Locked(IAppLog *pAppLog, uint32 dwLogId,
                                      uint16 wLogSz, void *pvLogData,
                                      uint32 dwNonce32);
static uint16 AppLog_SetAppClsId(IAppLog *pAppLog, 
                                    AEECLSID dwAppClsId);
#if defined(FEATURE_BNMAPP_STRIPPED)
static uint16 AppLog_SetAppClsId_Locked(IAppLog *pAppLog, 
                                           AEECLSID dwAppClsId);
#endif
static uint16 AppLog_Unlock(IAppLog *pMe, const void *pvCert, 
                               int nCertSz, AEECallback *pcb, 
                               uint32 *poutRes, uint16 wRand);

static void   AppLog_DiagLogListener (uint32 dwSeqnum, const byte *pvLog, 
                                      unsigned int nLength, void *pvParam);
static uint16 AppLog_VerifyCert(IAppLog *pMe, const void *pvCert, 
                                    int nCertSz);
static void   AppLog_HandleVerifyCB (void *pvUD);
static void   AppLog_ReleaseVerifyTCB (void *pvUD);
static uint32 AppLog_GetNonce (IAppLog *pMe);


#if defined (AEE_STATIC)
/*lint -esym(714, gvtIAppLog) */
IAppLogVtbl gvtIAppLog = {
  AppLog_AddRef,
  AppLog_Release,
  AppLog_QueryInterface,
  AppLog_SendLog,
  AppLog_SetAppClsId,
  AppLog_Unlock
};
#endif

/*============================================================================
FUNCTION DEFINITIONS
============================================================================*/
/*===========================================================================

FUNCTION: AEE01023c45_New

DESCRIPTION
  Instantiates an instance of IAppLog.

PROTOTYPE:
   int AEE01023c45_New (IShell * pIShell, AEECLSID clsid, void ** ppif)

PARAMETERS:
  pIShell : Instance of ISHELL interface
  clsId : ClassId to instantiate
  ppif : pointer to assign the new IAppLog interface

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS : Success
  ENOMEMORY : Not enough memory to instantiate AppLog
  EUNSUPPORTED : Unsupported or bad params

SIDE EFFECTS
  none
DEPENDENCIES
  FEATURE_BNMAPP_STRIPPED is used to differentiate stipped/unstripped codes.
===========================================================================*/
/*lint -esym(714,AEE01023c45_New) AEE01023c45_New is not referenced */
#if !defined (AEE_STATIC)
static
#endif
int AEE01023c45_New (IShell * pIShell, AEECLSID clsid, void ** ppif)
{
  AppLog             * pMe = NULL;
  AEEVTBL(IAppLog)   * pModMethods = NULL;

  if((NULL == ppif)  || 
     (AEECLSID_APPLOG != clsid) ||
     (NULL == pIShell)) {
    ALE_MSG(FATAL, ("ALE:_New:San. Chk. F (pMe, clsId, pIShell)"));
    return EUNSUPPORTED;
  }

  *ppif = 0;  
 
  pMe = (AppLog *) MALLOC (sizeof(AppLog) + sizeof(VTBL(IAppLog)));
  
  if (NULL == pMe) {
    return ENOMEMORY;
  }

  // Point the Mod functions to the memory location
  pModMethods = (VTBL(IAppLog) *)(void *)((byte *)pMe + sizeof(AppLog));
  
  pModMethods->AddRef = AppLog_AddRef;
  pModMethods->Release =  AppLog_Release;
  pModMethods->QueryInterface = AppLog_QueryInterface;
  pModMethods->SendLog = AppLog_SendLog_Locked;
  // Do not include SetAppClsId in stripped builds


#if defined(FEATURE_BNMAPP_STRIPPED)
    pModMethods->SetAppClsId = AppLog_SetAppClsId_Locked;
#else
    pModMethods->SetAppClsId = AppLog_SetAppClsId;
#endif
  pModMethods->Unlock = AppLog_Unlock;
  
  INIT_VTBL(pMe, IAppLog, *pModMethods);
  
  pMe->nRefs = 1;
  pMe->pIShell = pIShell;
  pMe->dwAppClsId = 0;

  pMe->pUnlock      = NULL;
  pMe->pcbVerify    = NULL;
  pMe->pResVerify   = NULL;
  pMe->pWebOpts     = NULL;
  pMe->pX509Chain   = NULL;
  pMe->pUserCert    = NULL;
  
  (void)ISHELL_AddRef(pIShell);
  
  *ppif = (void *)pMe;
  
  // Add a log listener to enable logging for ALE_ICD_LOG_CODE
  // HACK: If no listener is present for a log code, log_alloc returns a null 
  //       ptr. To enable logs even when MV is not present on the device, we 
  //       add a dummy log listener here. This will be removed when IAppLog 
  //       instance is released. Another (lengthier) way to enable logs would 
  //       be to set a log mask. We chose the simpler approach i.e. a dummy 
  //       log listener.
  (void)diag_add_log_listener (ALE_ICD_LOG_CODE,
				                       AppLog_DiagLogListener,
				                       NULL);
  return AEE_SUCCESS;

} // end - AEE01023c45_New

/*===========================================================================

FUNCTION: AppLog_Delete

DESCRIPTION
  This routine frees all resources (memory, hooks to lower layers, etc).

PROTOTYPE:
  void AppLog_Delete (AppLog *pMe)

PARAMETERS:
  pMe : Instance of AppLog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void
AppLog_Delete (AppLog *pMe)
{
  //Free the buffer, if allocated
  if (pMe)
  {
    ALE_MSG(DEBUG, ("ALE:Delete:Deleting data"));

    // If a caller is waiting for unlock callback when IAppLog is
    // unloaded. Resume the callback with status APPLOG_ERR_INVALID_OPERATION
    // before clearing pMe->pUnlock and releasing ishell
    if((NULL != pMe->pUnlock) &&
       (NULL != pMe->pUnlock->m_pCallerCB) &&
       (NULL != pMe->pIShell)) {
      if(NULL != pMe->pUnlock->m_poutRes) {
        *(pMe->pUnlock->m_poutRes) = APPLOG_ERR_INVALID_OPERATION;
      }
      ISHELL_Resume(pMe->pIShell, pMe->pUnlock->m_pCallerCB);
    }
    
    // Cancel all registered callbacks
    if(NULL != pMe->pcbVerify) {
      CALLBACK_Cancel(pMe->pcbVerify);
      ALE_MSG(DEBUG, ("ALE:Delete:pMe->pcbVerify:%x,",pMe->pcbVerify) );
      FREEIF(pMe->pcbVerify);
    }

    // Release the x509 chain
    if(NULL != pMe->pX509Chain) {
      (void)IX509CHAIN_Release(pMe->pX509Chain);
    }

    //Release all listeners, if applicable    
    if(NULL != pMe->pIShell) {
      (void)ISHELL_Release(pMe->pIShell);
    }
#if !defined (AEE_STATIC)
    if(NULL != pMe->pIModule) {
      (void)IMODULE_Release(pMe->pIModule);
    }
#endif
    
    // Remove the log listener that was registered when IAppLog was created
    (void)diag_remove_log_listener (ALE_ICD_LOG_CODE,
				                            AppLog_DiagLogListener,
				                            NULL);
    
    ALE_MSG(DEBUG, ("ALE:Delete:pMe->pWebOpts:%x",pMe->pWebOpts));
    FREEIF(pMe->pWebOpts);
    ALE_MSG(DEBUG, ("ALE:Delete:pMe->pResVerify:%x",pMe->pResVerify) );
    FREEIF(pMe->pResVerify);
    ALE_MSG(DEBUG, ("ALE:Delete:pMe->pUnlock:%x",pMe->pUnlock) );
    FREEIF(pMe->pUnlock);
    ALE_MSG(DEBUG, ("ALE:Delete:pMe->pUserCert:%x", pMe->pUserCert) );
    FREEIF(pMe->pUserCert);

    ALE_MSG(DEBUG, ("ALE:Delete:pMe:%x,",pMe) );
    FREE((void*)pMe);
  }
} // end - AppLog_Delete
#ifndef AEE_STATIC
/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
   This function is invoked while the app is being loaded. The module must 
   verifies the ClassID and then invoke the AEE01023c45_New() to instantiate
   IAppLog.

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,
                            void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: [in]: Contains pointer to the IModule object to the current 
    module to which this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. 
    Allocation of memory for this structure and initializing the base data 
    members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation 
               was successful
  EFAILED: If the app does not need to be loaded or if errors occurred. 

SIDE EFFECTS
  none
===========================================================================*/
int 
AEEClsCreateInstance 
(
  AEECLSID ClsId, 
  IShell * pIShell, 
  IModule * po,
  void ** ppObj
)
{
   AppLog   *pMe;
  
   if((NULL == ppObj) ||
      (NULL == pIShell) ||
      (NULL == po)) {
     ALE_MSG(FATAL, ("ALE:CreateInstance:S C F (ppObj, pIShell, po)"));
     return(EFAILED);
   }

   *ppObj = NULL;
    
   if(AEECLSID_APPLOG == ClsId) {
      if(AEE01023c45_New(pIShell, ClsId, ppObj) == AEE_SUCCESS) {

         // Add your code here
         pMe = (AppLog *)(*ppObj);
         // double check pMe
         if(NULL == pMe) {
           return EFAILED;
         }

         pMe->pIModule = po;
         (void)IMODULE_AddRef(po);
         return (AEE_SUCCESS);

      }
   }
   return (EFAILED);
} // end - AEEClsCreateInstance
#endif /* !AEE_STATIC */
/*===========================================================================

FUNCTION: AppLog_AddRef

DESCRIPTION
  This function increments the reference count of IAppLog Interface 
  object, allowing the object to be shared by multiple callers. The object 
  is freed when the reference count reaches 0 (zero).

PROTOTYPE:
  uint32 AppLog_AddRef (IAppLog * po)

PARAMETERS:
  po : Instance of IAppLog. 0 if po is invalid.

DEPENDENCIES
  none

RETURN VALUE
  dwRef : Updated reference count

SIDE EFFECTS
  none

SEE ALSO
  AppLog_Release()
===========================================================================*/
static uint32
AppLog_AddRef (IAppLog * po)
{
  AppLog          *pMe = (AppLog *) po;

  ALE_MSG(DEBUG, ("ALE:AddRef called")); 

  if(NULL != pMe) {
    return (++pMe->nRefs);
  } else {
    // cannot access pMe
    return 0;
  }    
} // end - AppLog_AddRef

/*===========================================================================

FUNCTION: AppLog_Release

DESCRIPTION
  This function decrements the reference count of IAppLog Interface 
  object. The object is freed from memory and is no longer valid when the 
  reference  count reaches 0 (zero). 

PROTOTYPE:
  uint32 AppLog_Release (IAppLog * po)

PARAMETERS:
  po : Instance of IAppLog

DEPENDENCIES
  none

RETURN VALUE
  dwRef : Updated reference count. 0 if po is freed or invalid.

SIDE EFFECTS
  none

SEE ALSO
  AppLog_AddRef()
===========================================================================*/
static uint32
AppLog_Release (IAppLog *po)
{
  AppLog          * pMe = (AppLog *) po;
  unsigned int    nRefs = 0;

  if(NULL != pMe) {
    nRefs = --pMe->nRefs;

    if (0 == nRefs) {
      AppLog_Delete(pMe);
    }
  }

  return (uint32) nRefs;
} // end - AppLog_Release

/*===========================================================================

FUNCTION: AppLog_QueryInterface

DESCRIPTION
  This function asks an object for another API contract from the object in 
  question. 

PROTOTYPE:
  int AppLog_QueryInterface (IAppLog * po)

PARAMETERS:
  pMe : Instance of IAppLog
  idReq : Requested ClassID exposed by the object.
  ppo :   Returned object. Filled by this function.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS : Interface found
  ECLASSNOTSUPPORT : Otherwise

SIDE EFFECTS
  none

SEE ALSO
  IQI_QueryInterface()
===========================================================================*/
static int 
AppLog_QueryInterface (IAppLog *pMe, AEECLSID idReq, void **ppo)
{
  PARAM_NOT_REF(pMe)
  // Sanity check
  if((NULL == pMe) ||
     (NULL == ppo)) {
    ALE_MSG(DEBUG, ("ALE:QI:San. Chk. F (pMe, ppo)")); 
    return ECLASSNOTSUPPORT;
  }

  if(IQI_SELF (pMe,idReq,ppo,AEECLSID_APPLOG) != NULL) {
    (void)IAppLog_AddRef (*ppo);
    return AEE_SUCCESS;
  } else if ( AEECLSID_APPLOG == idReq ) {
    *ppo = (void *)pMe;
    (void)IAppLog_AddRef (*ppo);
    return AEE_SUCCESS;
  } else {
    return ECLASSNOTSUPPORT;
  }
} // end - AppLog_QueryInterface

/*===========================================================================

FUNCTION: AppLog_SendLog

DESCRIPTION
  This function can be called only after BREW cls Id has been set.
  If BREW cls ID has not been set, then this function will return error code.
  Max. log size is 475-bytes.

  dwLogId : log_id from the BREW application calling this function.
  Each application can keep track of its own log_id’s.
  
  New extension will use this information to create the header.

PROTOTYPE:
   uint16 AppLog_SendLog(IAppLog *piAppLog, uint32 dwLogId, 
                           uint16 wLogSz, void *pvLogData,uint32 dwNonce32) 

PARAMETERS:
  piAppLog : Instance of IAppLog
  dwLogId : Application level Log id
  wLogSz : size of the pLogData
  pvLogData : log data the application wants to send to the MobileView.
  dwNonce32 : 32 bit random number
  
DEPENDENCIES
  none

RETURN VALUE
  SUCCESS:                     Success
  APPLOG_ERR_INVALID_PARAM1:   IAppLog instance is invalid 
  APPLOG_ERR_INVALID_PARAM4:   LogData ptr is null
  APPLOG_ERR_LG_NONCE_INVALID: Random # provided in dwNonce32 is zero
  APPLOG_ERR_LG_NONCE_BOGUS:   Incorrect random # provided in dwNonce32
  APPLOG_ERR_LG_CLSID_UNAVAIL: ClassId not set. Please unlock extension.
  APPLOG_ERR_LG_LOG_SIZE:      Log Size is too large or small
  APPLOG_ERR_LG_ALLOC_FAILED:  Cannot allocate space for log in diag, try later
  APPLOG_ERR_LG_PRIV_LEVEL:    Incorrect privilege level. Unlock extension.
  APPLOG_ERR_INVALID_OPERATION:General error

SIDE EFFECTS
  none

DEPENDENCIES
  log_alloc() and log_commit() defined in log.h and implemented as part of 
  the AMSS logging API.
===========================================================================*/
static uint16 
AppLog_SendLog
(
  IAppLog * piAppLog, 
  uint32  dwLogId, 
  uint16  wLogSz, 
  void    * pvLogData,
  uint32  dwNonce32
) 
{
  PARAM_NOT_CONST(pvLogData)

  AppLog            * pMe = (AppLog *)piAppLog;
  AppLog_LogType    * pLogBuf = NULL;
  log_code_type     nLogCode = ALE_ICD_LOG_CODE;
  
  ALE_MSG(DEBUG, ("ALE:AppLog Called"));
  
  // Check params
  if((NULL == piAppLog)     ||
     (NULL == pvLogData)    ||
     (0    == dwNonce32)       ||
     (dwNonce32 != pMe->dwNonce32) ||
     (0    == pMe->dwAppClsId)) {

    ALE_MSG(FATAL, ("ALE:AppLog:Invalid params"));
    if ( NULL == piAppLog ) {
      return APPLOG_ERR_INVALID_PARAM1;
    } else if ( NULL == pvLogData ) {
      return APPLOG_ERR_INVALID_PARAM4;
    } else if ( 0 == dwNonce32 ) {
      return APPLOG_ERR_LG_NONCE_INVALID;
    } else if ( dwNonce32 != pMe->dwNonce32 ) {
      return APPLOG_ERR_LG_NONCE_BOGUS;
    } else if ( 0 == pMe->dwAppClsId ) {
      return APPLOG_ERR_LG_CLSID_UNAVAIL;
    } else {
      return APPLOG_ERR_INVALID_OPERATION;
    }
  }

  // Validations on log data
  // log data can only be a max of 500 bytes
  if(wLogSz > ALE_MAX_LOG_SIZE) {
    ALE_MSG(HIGH, ("ALE:AppLog: Log sz too long:%d", wLogSz));
    return APPLOG_ERR_LG_LOG_SIZE;
  }

  // Allocate log buffer using log_alloc:
  ALE_MSG(LOW, ("ALE:AppLog: Requesting log_alloc of %d bytes", 
                (sizeof(AppLog_LogType)+wLogSz)));
  pLogBuf = (AppLog_LogType *)log_alloc(nLogCode, 
                                           (FPOS(AppLog_LogType, 
                                                 pbAppLogData) +
                                            wLogSz));
  if(NULL == pLogBuf) {
    ALE_MSG(HIGH, ("ALE:AppLog: log_alloc failed"));
    return APPLOG_ERR_LG_ALLOC_FAILED;
  } else {
    pLogBuf->dwAppLogId  = dwLogId;
    pLogBuf->dwClsid     = pMe->dwAppClsId;
    pLogBuf->wAppLogSize = wLogSz;
    (void)MEMCPY((void *)pLogBuf->pbAppLogData, (void *)pvLogData, wLogSz);
    log_commit(pLogBuf);
    ALE_MSG(MED, ("ALE:AppLog:Committed log: %x", pLogBuf));
  }

  return SUCCESS;
} // end - AppLog_SendLog

/*===========================================================================

FUNCTION: AppLog_SendLog_Locked

DESCRIPTION
  Locked version of AppLog_SendLog.

PROTOTYPE:
   uint16 AppLog_SendLog_Locked(IAppLog *piAppLog, uint32 dwLogId, 
                           uint16 wLogSz, void *pvLogData,uint32 dwNonce32) 

PARAMETERS:
  piAppLog : Instance of IAppLog
  dwLogId : Application level Log id
  wLogSz : size of the pvLogData
  pvLogData : log data the application wants to send to the MobileView.
  dwNonce32 : 32 bit random number

DEPENDENCIES
  none

RETURN VALUE
  APPLOG_ERR_LG_PRIV_LEVEL:    Always.

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static uint16 
AppLog_SendLog_Locked
(
  IAppLog *piAppLog, 
  uint32 dwLogId, 
  uint16 wLogSz, 
  void *pvLogData,
  uint32 dwNonce32
)
{
  PARAM_NOT_REF(piAppLog)
  PARAM_NOT_REF(dwLogId)
  PARAM_NOT_REF(wLogSz)
  PARAM_NOT_REF(pvLogData)
  PARAM_NOT_REF(wNonce16)
  PARAM_NOT_REF(dwNonce32)
  PARAM_NOT_CONST(piAppLog)
  PARAM_NOT_CONST(pvLogData)

  return APPLOG_ERR_LG_PRIV_LEVEL;
} // end - AppLog_SendLog_Locked

/*===========================================================================

FUNCTION: AppLog_SetAppClsId

DESCRIPTION
  Sets the classid of the calling application.

PROTOTYPE:
  uint16 AppLog_SetAppClsId (IAppLog *piAppLog, AEECLSID dwAppClsId)

PARAMETERS:
  piAppLog : Instance of IAppLog
  dwAppClsId : ClassId to use

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS:                     Success
  APPLOG_ERR_INVALID_PARAM1:   IAppLog instance invalid
  APPLOG_ERR_LG_CLSID_INVALID: Invalid classId

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static uint16 
AppLog_SetAppClsId (IAppLog *piAppLog, AEECLSID dwAppClsId) 
{
  AppLog           * pMe = (AppLog *)piAppLog;
  AEEVTBL(IAppLog) * pModMethods = NULL;
  
  ALE_MSG(MED, ("ALE:SetAppClsId. dwAppClsId=0x%x", dwAppClsId));

  // Check params
  if((NULL == pMe) || 
     (0 == dwAppClsId)) {
    if (NULL == pMe) {
      return APPLOG_ERR_INVALID_PARAM1;
    } else {
      return APPLOG_ERR_LG_CLSID_INVALID;
    }
  } 
  pModMethods = pMe->pvt;
  pModMethods->SendLog = AppLog_SendLog;

  pMe->dwAppClsId = dwAppClsId;
  pMe->dwNonce32  = 1;
  pMe->wNonce16   = 1;

  return SUCCESS;
} // end - AppLog_SetAppClsId

#if defined(FEATURE_BNMAPP_STRIPPED)
/*===========================================================================

FUNCTION: AppLog_SetAppClsId_Locked

DESCRIPTION
  Locked version ofAppLog_SetAppClsId.

PROTOTYPE:
  uint16 AppLog_SetAppClsId_Locked (IAppLog *piAppLog, 
                                        AEECLSID dwAppClsId)

PARAMETERS:
  piAppLog : Instance of IAppLog
  dwAppClsId : ClassId to use

DEPENDENCIES
  none

RETURN VALUE
  APPLOG_ERR_FEATURE_UNAVAIL:  Always.

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static uint16 
AppLog_SetAppClsId_Locked (IAppLog *piAppLog, AEECLSID dwAppClsId) 
{
  PARAM_NOT_REF(piAppLog)
  PARAM_NOT_REF(dwAppClsId)
  PARAM_NOT_CONST(piAppLog)

  // SetAppClsId will be unavailble in stripped builds.
  return APPLOG_ERR_FEATURE_UNAVAIL;
} // end - AppLog_SetAppClsId_Locked
#endif

/*=========================================================================

FUNCTION: AppLog_DiagLogListener

DESCRIPTION
  This is a dummy log listener that is used to enable logging

PROTOTYPE:
  void AppLog_DiagLogListener (uint32 dwSeqNum, const byte *pvLog, 
                                   unsigned int nLength, void *pvParam)

PARAMETERS:
  dwSeqNum: Sequence number
  pvLog : log data
  nLength : length of the log
  pvParam : log parameters

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static void 
AppLog_DiagLogListener 
(
  uint32       dwSeqNum, 
  const byte   * pvLog, 
  unsigned int nLength, 
  void         * pvParam
)
{
  PARAM_NOT_REF(dwSeqNum)
  PARAM_NOT_REF(pvLog)
  PARAM_NOT_REF(nLength)
  PARAM_NOT_REF(pvParam)
  PARAM_NOT_CONST(pvParam)

  ALE_MSG(DEBUG, ("ALE:DiagLogListener:Log listener called"));
  return;
} // end - AppLog_DiagLogListener

/*===========================================================================

FUNCTION: AppLog_Unlock

DESCRIPTION
  Unlocks IAppLog if all checks pass. A callback should be provided to 
  recieve the status of unlock request. The application should unlock the
  instance of IAppLog and save it for future as long as logging will be
  done.

Prototype:
  uint16 IAppLog_Unlock(IAppLog *piAppLog, const void *pvCert, 
                        uint32 nCertSz, AEECallback *pcb,  
                        uint32 *poutRes, uint16 wRand)

Parameters:
  piAppLog :  Pointer to IAppLog Interface object
  pvCert :       Pointer to buffer containing DER encoded certificate
  nCertSz :     Size of certificate
  pcb :         Callback to receive result of certificate verification
  poutRes :     Pointer to the variable where the result of the Unlock process
                will be stored
  wRand :       16 bit random # generated by the application. The application 
                should only honor EVT_APPLOG if wParam matches this value.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS:                   Success
  APPLOG_ERR_INVALID_PARAM1: IAppLog instance is invalid
  APPLOG_ERR_INVALID_PARAM2: Cerificate ptr is null
  APPLOG_ERR_INVALID_PARAM4: Callback ptr is null
  APPLOG_ERR_INVALID_PARAM5: pdwRes is null
  APPLOG_ERR_VRFY_CRT_SIZE:  Certificate size is negative or zero
  APPLOG_ERR_VRFY_CHN_CREAT: Unable to initialize certificate chain
  APPLOG_ERR_VRFY_CRT_COPY:  Cannot copy the provided cert
  APPLOG_ERR_VRFY_SET_OPTS:  Cannot set verification options
  APPLOG_ERR_VRFY_CB_ALLOC:  Insufficient memory for verification callback
  APPLOG_ERR_VRFY_CHN_ADD_ROOT:   Cannot add root cert to chain
  APPLOG_ERR_VRFY_CHN_ADD_CLIENT: Cannot add client cert to chain
  APPLOG_ERR_VRFY_MEM_UNAVAIL:    Insufficient memory to complete verification
  APPLOG_ERR_INVALID_OPERATION:   General error

SIDE EFFECTS
  none

DEPENDENCIES
  Brew 3.X
===========================================================================*/
static uint16 
AppLog_Unlock
(
  IAppLog       *pMe, 
  const void    *pvCert, 
  int           nCertSz,
  AEECallback   * pcb, 
  uint32        * poutRes, 
  uint16        wRand
) 
{
  uint16 wRet = SUCCESS;
  int    nRet = SUCCESS;

  // Check params
  if((NULL == pMe)      ||
     (NULL == pvCert)   ||
     (NULL == pcb)      ||
     (NULL == poutRes)  ||
     (nCertSz <= 0)) {
    ALE_MSG(FATAL, ("ALE:Unlock:S C F(pcb, poutRes, pvCert, pMe, nCertSz)"));
    if ( NULL == pMe ) {
      return APPLOG_ERR_INVALID_PARAM1;
    } else if ( NULL == pvCert ) {
      return APPLOG_ERR_INVALID_PARAM2;
    } else if ( NULL == pcb ) {
      return APPLOG_ERR_INVALID_PARAM4;
    } else if ( NULL == poutRes ) {
      return APPLOG_ERR_INVALID_PARAM5;
    } else if ( nCertSz <= 0 ) {
      return APPLOG_ERR_VRFY_CRT_SIZE;
    } else {
      return APPLOG_ERR_INVALID_OPERATION;
    }
  }
  
  if(NULL == pMe->pUnlock) {
    pMe->pUnlock = (AppLog_UnlockUD *)MALLOC(sizeof(AppLog_UnlockUD));
    if (NULL == pMe->pUnlock) {
      ALE_MSG(FATAL, ("ALE:Unlock: MALLOC F.(pUnlock)"));
      return APPLOG_ERR_VRFY_MEM_UNAVAIL;
    }
  } 
  pMe->pUnlock->m_pCallerCB = pcb;
  pMe->pUnlock->m_poutRes   = poutRes;
  wRet = AppLog_VerifyCert(pMe, pvCert, nCertSz);
  if(SUCCESS != wRet) {
    FREEIF(pMe->pUnlock);
    ALE_MSG(ERROR, ("ALE:Unlock:Verify Cert failure."));
    // TODO: Should we return security error codes?
    return wRet;
  }

  pMe->wNonce16 = wRand;

  nRet = ISHELL_SetTimer(pMe->pIShell, 
                        ALE_VERIFY_TIMER, 
                        AppLog_ReleaseVerifyTCB, 
                        (void *)pMe);
  ALE_MSG(LOW, ("ALE:Unlock:SetTimer returned: %d", nRet));
  return SUCCESS;
} // end - AppLog_Unlock

/*===========================================================================

FUNCTION: AppLog_ReleaseVerifyTCB

DESCRIPTION
  Called when verify timer expires. Cancels the unlock request and notifies
  the caller.

Prototype:
  uint16 AppLog_ReleaseVerifyTCB(void *pvUD)

Parameters:
  pvUD :  UserData - Instance of IAppLog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static void 
AppLog_ReleaseVerifyTCB (void *pvUD) 
{
  IAppLog * pMe = (IAppLog *)pvUD;

  ALE_MSG(DEBUG, ("ALE:ReleaseVerifyTCB:Called"));

  // Sanity check.
  if(NULL == pMe) {
    ALE_MSG(HIGH, ("ALE:RelVrfyTCb: San. Chk. F (pMe)"));
    return;
  }
  
  // Cancel callback if it hasn't happened and notify caller.
  if(NULL != pMe->pcbVerify) {
    CALLBACK_Cancel(pMe->pcbVerify);
    FREEIF(pMe->pcbVerify);
  }
  if((NULL != pMe->pUnlock) && 
     (NULL != pMe->pUnlock->m_poutRes) &&
     (NULL != pMe->pUnlock->m_pCallerCB)) {
    // return invalid operation status
    *(pMe->pUnlock->m_poutRes) = APPLOG_ERR_VRFY_TIMEOUT;
    ISHELL_Resume(pMe->pIShell, pMe->pUnlock->m_pCallerCB);
  }
  // Free userCert as a double check. (It would freed anyway before any 
  // new malloc is done on it or if instance is released. but freeing now
  // saves mem. that we dont need nemore)
  FREEIF(pMe->pUnlock);
  FREEIF(pMe->pUserCert);
} // end - AppLog_ReleaseVerifyTCB

/*===========================================================================

FUNCTION: AppLog_HandleVerifyCB

DESCRIPTION
  Callback for IX509Chain_VerifyV. Called after the certificate has been 
  verified and results populated.

Prototype:
  void AppLog_HandleVerifyCB(void *pvUD)

Parameters:
  pvUD : User data set in callback (IAppLog instance)

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static void 
AppLog_HandleVerifyCB (void *pvUD) 
{
  PARAM_NOT_CONST(pvUD)

  IAppLog             * pMe = (IAppLog *)pvUD;
  X509PartRequest     * pPartReq = NULL;
  X509CertPart        * pCertPart = NULL;
  AECHAR              * pzStrSubject = NULL;
  AEECLSID            dwClsId = 0;
  AEEVTBL(IAppLog)    * pModMethods = NULL;

  char                * pszSubject = NULL;
  int                 i = 0;
  int                 nRet = 0;
  boolean             bRet = FALSE;
  uint32              nStatus = SUCCESS;

  // Sanity check. We can't really do anything if this fails.
  if(NULL == pMe) {
    ALE_MSG(HIGH, ("ALE:HandleVrfyCb: San. Chk. F (pMe)"));
    return;
  }

  // Cancel ReleaseVerify timer
  (void)ISHELL_CancelTimer(pMe->pIShell, 
                           AppLog_ReleaseVerifyTCB, 
                           (void *)pMe);

  ALE_MSG(DEBUG, ("ALE:HandleVerifyCB:Callback called. pResVerify:%d", 
                  pMe->pResVerify));
  if(NULL != pMe->pResVerify) {
    if((SUCCESS != pMe->pResVerify->nResult) || 
       (X509CHAIN_TRUST_OK != pMe->pResVerify->uErrorSummary)) {

      // Cert did not pass verification
      ALE_MSG(MED, ("verify Result:%d, uErrorSummary:%d", 
                    pMe->pResVerify->nResult, 
                    pMe->pResVerify->uErrorSummary));
      nStatus = APPLOG_ERR_VRFY_CRT_TRUST;
    } else {
      for(i=0; i < pMe->pResVerify->nChainLen; i++) {
        // Found erors in one of the certs in the chain
        ALE_MSG(MED, ("ALE:Chain error %02d:%d", 
                      i, 
                      pMe->pResVerify->auChainErrors[i]));
        if(X509CHAIN_TRUST_OK != pMe->pResVerify->auChainErrors[i]) {
          nStatus = APPLOG_ERR_VRFY_CRT_TRUST;
          break;
        }
      }
    }
  } else {
    nStatus = APPLOG_ERR_VRFY_NO_RESULT;
  }

  // Process the certificate if no errors
  if((NULL != pMe->pX509Chain) &&
     (SUCCESS == nStatus)) {
    
    // Status will now be success only if all checks pass
    nStatus = APPLOG_ERR_VRFY_MEM_UNAVAIL;

    pPartReq   = (X509PartRequest *)MALLOC(sizeof(X509PartRequest));
    pCertPart  = (X509CertPart *)MALLOC(sizeof(X509CertPart));
    if((NULL  != pPartReq) &&
       (NULL!= pCertPart )) {
      pPartReq->nCertIndex  = 0;
      pPartReq->nCertWebOpt = WEBOPT_X509_LEAF_CERT;
      pPartReq->uCertPart   = X509CHAIN_FIELD_SUBJECT;
      pPartReq->pcRawOID    = NULL;
      pPartReq->nInstance   = 0;
      pPartReq->uASNOID     = ASNOID_X500_CN;

      // Get the classid from CN field
      nRet = IX509CHAIN_GetFieldPart(pMe->pX509Chain,
                                     pPartReq,
                                     pCertPart);
      ALE_MSG(LOW, ("ALE:IX509CHAIN_GetFieldPart returned:%d", nRet));
      // Proceed only if success and no int overflows will occur
      if((SUCCESS == nRet) &&
         ((int)((pCertPart->uLength+1) * sizeof(AECHAR)) > 0)) {

        // Get the classid from CN field and convert it into uint32
        pzStrSubject = (AECHAR *)MALLOC((pCertPart->uLength+1)*
                                         sizeof(AECHAR));
        pszSubject = (char *)MALLOC((pCertPart->uLength+1)*sizeof(char));
        if((NULL != pzStrSubject) && 
           (NULL != pszSubject)) {
          // Lint uint to int. Checked in if condition
          /*lint -e713*/
          (void)UTF8TOWSTR(pCertPart->pcValue, pCertPart->uLength, 
                           pzStrSubject, ((pCertPart->uLength)+1) *
                                            sizeof(AECHAR));
          pzStrSubject[pCertPart->uLength] = '\0';
          (void)WSTRTOSTR(pzStrSubject, pszSubject, 
                         (int)(pCertPart->uLength+1));
          
          // Get the class Id and check if it is in the privelege list
          // 16 is the BASE to be used when converting hex numbers
          dwClsId = (AEECLSID)STRTOUL(pszSubject, NULL, 16);

          // Check if clsid is in priv. list
#if MIN_BREW_VERSION(3,0)
          bRet    = ISHELL_CheckPrivLevel(pMe->pIShell,dwClsId,TRUE);
          if(!bRet) {
            nStatus = APPLOG_ERR_VRFY_PRIV_LVL;
          } else { 
#endif
            // Cert and priv. verified, now generate random numbers
            pMe->dwNonce32 = AppLog_GetNonce(pMe);

            if(0 != pMe->dwNonce32) {
              // Unlock success. unlock applog
              pMe->dwAppClsId = dwClsId;
              pModMethods = pMe->pvt;
              pModMethods->SendLog = AppLog_SendLog;
  
              // Send random #s
              (void)ISHELL_SendEvent(pMe->pIShell, 
                                     dwClsId, 
                                     EVT_APPLOG, 
                                     pMe->wNonce16, 
                                     pMe->dwNonce32);

              ALE_MSG(MED, ("ALE_HandleVerifyCB: Unlock Success. dw:%x, w:%x",
                             pMe->dwNonce32,
                             pMe->wNonce16));

              nStatus = SUCCESS;
            } else {
              ALE_MSG(HIGH, ("ALE:HandleVerifyCB: Error gen. Nonces"));
              // Set status to APPLOG_ERR_VRFY_RNG if we cant generate rand #s
              nStatus = APPLOG_ERR_VRFY_RNG;
            } // endif - dwNonce
#if MIN_BREW_VERSION(3,0)
          } // endif - bRet
#endif

          ALE_MSG(LOW, ("ALE:HandleVerifyCB:Len:%d CN:%s :%u", 
                        pCertPart->uLength, pszSubject, 
                        STRTOUL(pszSubject, NULL, 16)) );
        } // endif - pzStrSubject, pszSubject
      } else if (SUCCESS != nRet) {
        // Cannot get CN field
        nStatus = APPLOG_ERR_VRFY_CRT_CN_GET;
      } else {
        // CN field i too long
        nStatus = APPLOG_ERR_VRFY_CRT_CN_INVALID;
      }// endif - nRet
    } // endif - pPartReq, pCertPart
  } else if (SUCCESS == nStatus){
    // pX509Chain was null. Invalid op.
    nStatus = APPLOG_ERR_VRFY_CHN_INVALID;
  } // endif - pMe->pX509Chain

  ALE_MSG(DEBUG, ("ALE:HandleVerifyCB:Calling CB"));
  if((NULL != pMe->pUnlock) && 
    (NULL != pMe->pUnlock->m_poutRes) &&
    (NULL != pMe->pUnlock->m_pCallerCB)) {
    // TODO: Should we return security error codes?
    *(pMe->pUnlock->m_poutRes) = nStatus;
    ISHELL_Resume(pMe->pIShell, pMe->pUnlock->m_pCallerCB);
  }  

  ALE_MSG(DEBUG, ("ALE:HandleVerifyCB:CALLED CB"));
  // The FREEIF Macro also sets the params to null
  // lint complains about the boolean being true for pCertPart
  /*lint -e774*/
  FREEIF(pzStrSubject);       
  FREEIF(pCertPart);        
  FREEIF(pPartReq);
  FREEIF(pszSubject);
  FREEIF(pMe->pUnlock);
  FREEIF(pMe->pUserCert);
  FREEIF(pMe->pcbVerify);
  ALE_MSG(DEBUG, ("ALE:HandleVerifyCB:Returned"));
} // end - AppLog_HandleVerifyCB

/*===========================================================================

FUNCTION: AppLog_VerifyCert

DESCRIPTION
  Implements the code to start the verification of certificates provided 
  by calling app. Called by AppLog_Unlock after the preliminary privilege
  checks have passed. Verification results are processed by the 
  callback AppLog_HandleVerifyCB

Prototype:
  boolean AppLog_VerifyCert(IAppLog *pMe, const void *pvCert, 
                                uint32 nCertSz)

Parameters:
  pMe :  Pointer to IAppLog Interface object
  pvCert :  Pointer to buffer containing DER encoded certificate
  nCertSz :     Size of certificate

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS:                   Success
  APPLOG_ERR_INVALID_PARAMS: A param is invalid
  APPLOG_ERR_VRFY_CRT_SIZE:  Certificate size is negative or zero
  APPLOG_ERR_VRFY_CHN_CREAT: Unable to initialize certificate chain
  APPLOG_ERR_VRFY_CRT_COPY:  Cannot copy the provided cert
  APPLOG_ERR_VRFY_SET_OPTS:  Cannot set verification options
  APPLOG_ERR_VRFY_CB_ALLOC:  Insufficient memory for verification callback
  APPLOG_ERR_VRFY_CHN_ADD_ROOT:   Cannot add root cert to chain
  APPLOG_ERR_VRFY_CHN_ADD_CLIENT: Cannot add client cert to chain
  APPLOG_ERR_INVALID_OPERATION:   General error

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static uint16
AppLog_VerifyCert (IAppLog *pMe, const void *pvCert, int nCertSz) 
{
  int nRet = 0;
  // override default trust settings: 0=>Apply to all
  // 1. disable host check as this is not an https cert
  // 2. disable all date checks as Brew uses user-defined date which gets 
  //    reset when phone power-cycles, thereby disabling all certs
  X509TrustOverride trustOverRide = 
    {0,X509CHAIN_HOST_MISMATCH  | 
       X509CHAIN_NO_TARGET_HOST |
       X509CHAIN_CERT_EXIPRED   | 
       X509CHAIN_BAD_DATE}; 

  // Sanity check. 
  if((NULL == pMe)   ||
     (NULL == pvCert) ||
     (nCertSz <= 0)) {
    ALE_MSG(HIGH, ("ALE:VrfyCert: San. Chk. F (pMe, pCert, nCertSz)"));
    // Should never be reached. Same checks done in unlock.
    return APPLOG_ERR_INVALID_PARAMS;
  }


  ALE_MSG(DEBUG, ("ALE:VerifyCert: Creating X509Chain"));
  if(NULL == pMe->pX509Chain) {
    nRet = ISHELL_CreateInstance(pMe->pIShell, AEECLSID_X509CHAIN,
                                 (void*)&pMe->pX509Chain);
    if(SUCCESS != nRet) {
      return APPLOG_ERR_VRFY_CHN_CREAT;
    }
  }
  
  ALE_MSG(LOW, ("ALE:VerifyCert:X509Chain: %d", pMe->pX509Chain));
  if(NULL == pMe->pX509Chain) {
    return APPLOG_ERR_VRFY_CHN_CREAT;
  }

  // Clear any old certs we might have not cleared
  if(NULL != pMe->pUserCert) {
    FREEIF(pMe->pUserCert);
  }

  // Note: int to uint: we checked nCertSz to be > 0 in unlock
  pMe->pUserCert = (uint8 *)MALLOC((uint32)nCertSz);
  if(NULL == pMe->pUserCert) {
    (void)IX509CHAIN_Release(pMe->pX509Chain);
    pMe->pX509Chain = NULL;
    return APPLOG_ERR_VRFY_CRT_COPY;
  } else {
    (void)MEMCPY((void *)pMe->pUserCert, pvCert, (uint32)nCertSz);
  }

  //Add the root cert as the webopt root
  nRet = IX509CHAIN_AddCert(pMe->pX509Chain, 
                            WEBOPT_X509_ROOT_CERTS, 
                            cacert,
                            CERT_LEN);
  if(SUCCESS != nRet) {
    (void)IX509CHAIN_Release(pMe->pX509Chain);
    pMe->pX509Chain = NULL;
    FREEIF(pMe->pUserCert);
    return APPLOG_ERR_VRFY_CHN_ADD_ROOT;
  }
  // Add user given cert as leaf cert
  nRet = IX509CHAIN_AddCert(pMe->pX509Chain, 
                            WEBOPT_X509_LEAF_CERT, 
                            pMe->pUserCert,
                            nCertSz);
  if(SUCCESS != nRet) {
    (void)IX509CHAIN_Release(pMe->pX509Chain);
    pMe->pX509Chain = NULL;
    FREEIF(pMe->pUserCert);
    return APPLOG_ERR_VRFY_CHN_ADD_CLIENT;
  }

  // Cancel any ongoing unlock process
  if(NULL != pMe->pcbVerify) {
    CALLBACK_Cancel(pMe->pcbVerify);
    FREEIF(pMe->pcbVerify);
  }
  // Initiate cert chain verification process
  pMe->pcbVerify   = (AEECallback *)MALLOC(sizeof(AEECallback));
  
  if(NULL == pMe->pResVerify) { 
    pMe->pResVerify = (X509TrustResult *)MALLOC(sizeof(X509TrustResult));
  }

  if((NULL != pMe->pcbVerify) && 
     (NULL != pMe->pResVerify)) {
    
    if(NULL == pMe->pWebOpts) {
      pMe->pWebOpts = (WebOpt *) MALLOC(2*sizeof(WebOpt));
      if(NULL == pMe->pWebOpts) {
        (void)IX509CHAIN_Release(pMe->pX509Chain);
        pMe->pX509Chain = NULL;
        FREEIF(pMe->pUserCert);
        FREEIF(pMe->pcbVerify);
        return APPLOG_ERR_VRFY_SET_OPTS;
      }
    }
    pMe->pWebOpts->nId  = WEBOPT_X509_OVERRIDE;
    pMe->pWebOpts->pVal = &trustOverRide;
    (pMe->pWebOpts+1)->nId = WEBOPT_END;
  
    CALLBACK_Init(pMe->pcbVerify, AppLog_HandleVerifyCB, pMe);  
    IX509CHAIN_VerifyV(pMe->pX509Chain, 
                       pMe->pResVerify,
                       pMe->pcbVerify,
                       pMe->pWebOpts);
  } else {
    (void)IX509CHAIN_Release(pMe->pX509Chain);
    pMe->pX509Chain = NULL;
    FREEIF(pMe->pUserCert);
    FREEIF(pMe->pcbVerify);
    return APPLOG_ERR_VRFY_CB_ALLOC;
  }
  
  return SUCCESS;
} // end - AppLog_VerifyCert

/*===========================================================================

FUNCTION: AppLog_GetNonce

DESCRIPTION
  Generates cryptographic quality 32 bit random number using AEECLSID_RANDOM

Prototype:
  uint32 AppLog_GetNonce(IAppLog *pMe) 

Parameters:
  pMe : IAppLog instance

DEPENDENCIES
  none

RETURN VALUE
  0   - in case of error
  +ve - Random value if success

SIDE EFFECTS
  none

DEPENDENCIES
  none
===========================================================================*/
static uint32 
AppLog_GetNonce (IAppLog *pMe) 
{
  PARAM_NOT_CONST(pMe)

  ISource *pRandSrc = NULL;
  uint32   dwRand   = 0;
  int      nRetVal  = 0;
  uint16   wRetries = 0;

  if((NULL == pMe) ||
     (NULL == pMe->pIShell)) {
    return 0;
  }

  nRetVal = ISHELL_CreateInstance(pMe->pIShell, 
                                  AEECLSID_RANDOM,
                                  (void*)&pRandSrc);
  if(SUCCESS != nRetVal) {
    // Use GetRand if ISource creation fails
    ALE_MSG(HIGH, ("ALE:GetNonce:Fallback on bckup RNG"));

    // if RNG returns zero, get a different random #.
    while((0 == dwRand) &&
          (wRetries <= ALE_MAX_NONCE_RETRIES)) {
      GETRAND((byte *)&dwRand, sizeof(uint32));
      wRetries++;
    }
    return dwRand;
  }

  // if RNG returns zero, get a different random #.
  wRetries = 0;
  while((0 == dwRand) &&
        (wRetries <= ALE_MAX_NONCE_RETRIES)) {
    nRetVal= ISOURCE_Read(pRandSrc, (char *)&dwRand, sizeof(uint32));
    wRetries++;
  }
  (void)ISOURCE_Release(pRandSrc);
  return dwRand;
} // end - AppLog_GetNonce

