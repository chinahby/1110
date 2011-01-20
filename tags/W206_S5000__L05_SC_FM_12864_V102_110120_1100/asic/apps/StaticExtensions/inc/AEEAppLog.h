#ifndef AEEAppLog_H
#define AEEAppLog_H
/*============================================================================
               Copyright © 2006 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
==============================================================================
FILE:  AEEAppLog.h

SERVICES: Qualcomm's Application logging service
 
GENERAL DESCRIPTION:
  Static extension providing access to logging api for external applications
  to log packets and data to MobileView

REVISION HISTORY: 

============================================================================*/

#include "AEE.h"               /* Required for EVT_USER */
#include "AEEError.h"          /* Required for SUCCESS and ERROR_USER */


/*=============================================================================
                             MACROS AND DEFINITIONS
=============================================================================*/

/* interface id */
#define AEEIID_IAppLog 0x01023c45

#define IAPPLOG_MODULE_VERSION              "0.3.01"

// Event used by this extension to communicate with the calling application
#define EVT_APPLOG                           ((EVT_USER) + (0x245))

// AppLog Error codes
// General AppLog errors
#define  APPLOG_ERR_BASE                     (ERROR_USER)
// Error during last operation
#define  APPLOG_ERR_INVALID_OPERATION        (APPLOG_ERR_BASE + 1)
// Memory unavailable to proceed.
#define  APPLOG_ERR_MEM_UNAVAIL              (APPLOG_ERR_BASE + 2)
// Requested feature is unavailable
#define  APPLOG_ERR_FEATURE_UNAVAIL          (APPLOG_ERR_BASE + 3)

// Errors during logging (AppLog, SetAppClsId)
#define  APPLOG_ERR_LG_BASE                  (APPLOG_ERR_BASE + 0x100)
// Cannot allocate space for log
#define  APPLOG_ERR_LG_ALLOC_FAILED          (APPLOG_ERR_LG_BASE + 1)
// Random number invalid e.g. zero
#define  APPLOG_ERR_LG_NONCE_INVALID         (APPLOG_ERR_LG_BASE + 2)
// Random number doesn't match the one available with extension. Unlock again.
#define  APPLOG_ERR_LG_NONCE_BOGUS           (APPLOG_ERR_LG_BASE + 3)
// ClassId provided is invalid.
#define  APPLOG_ERR_LG_CLSID_INVALID         (APPLOG_ERR_LG_BASE + 4)
// ClassId not set in extension. Unlock again.
#define  APPLOG_ERR_LG_CLSID_UNAVAIL         (APPLOG_ERR_LG_BASE + 5)
// Log size provided exceeds max. allowed by extension
#define  APPLOG_ERR_LG_LOG_SIZE              (APPLOG_ERR_LG_BASE + 6)
// Insufficient privileges to call applog. Unlock again.
#define  APPLOG_ERR_LG_PRIV_LEVEL            (APPLOG_ERR_LG_BASE + 7)


// Certificate verification errors
#define  APPLOG_ERR_VRFY_BASE                (APPLOG_ERR_BASE + 0x200)
// Error getting certificate chain creation api
#define  APPLOG_ERR_VRFY_CHN_CREAT           (APPLOG_ERR_VRFY_BASE + 1)
// Error adding root certificate to chain
#define  APPLOG_ERR_VRFY_CHN_ADD_ROOT        (APPLOG_ERR_VRFY_BASE + 2)
// Error adding client certificate to chain
#define  APPLOG_ERR_VRFY_CHN_ADD_CLIENT      (APPLOG_ERR_VRFY_BASE + 3)
// The certificate chain in invalid
#define  APPLOG_ERR_VRFY_CHN_INVALID         (APPLOG_ERR_VRFY_BASE + 4)
// Error setting verification options
#define  APPLOG_ERR_VRFY_SET_OPTS            (APPLOG_ERR_VRFY_BASE + 5)
// Error allocating space to complete verification callback
#define  APPLOG_ERR_VRFY_CB_ALLOC            (APPLOG_ERR_VRFY_BASE + 6)
// Error during random number generation
#define  APPLOG_ERR_VRFY_RNG                 (APPLOG_ERR_VRFY_BASE + 7)
// Cert size is invalid
#define  APPLOG_ERR_VRFY_CRT_SIZE            (APPLOG_ERR_VRFY_BASE + 8)
// Certificate trust verification failed
#define  APPLOG_ERR_VRFY_CRT_TRUST           (APPLOG_ERR_VRFY_BASE + 9)
// Error allocating memory to copy client cert
#define  APPLOG_ERR_VRFY_CRT_COPY            (APPLOG_ERR_VRFY_BASE + 10)
// Cannot retrieve CN field from the cert
#define  APPLOG_ERR_VRFY_CRT_CN_GET          (APPLOG_ERR_VRFY_BASE + 11)
// CN field from the cert is invalid
#define  APPLOG_ERR_VRFY_CRT_CN_INVALID      (APPLOG_ERR_VRFY_BASE + 12)
// Calling app failed basic privilege check
#define  APPLOG_ERR_VRFY_PRIV_LVL            (APPLOG_ERR_VRFY_BASE + 13)
// Cannot retrieve result of verification
#define  APPLOG_ERR_VRFY_NO_RESULT           (APPLOG_ERR_VRFY_BASE + 14)
// Certificate Verification process timed out
#define  APPLOG_ERR_VRFY_TIMEOUT             (APPLOG_ERR_VRFY_BASE + 15)
// Insufficient memory to complete verification
#define  APPLOG_ERR_VRFY_MEM_UNAVAIL         (APPLOG_ERR_VRFY_BASE + 16)

// The following error codes indicate a sanity check failure on  
// one of the input params. E.g. In a call to function f(p1, p2, p3), 
// if p2 is NULL or invalid, error APPLOG_ERR_INVALID_PARAM2 will be raised
#define  APPLOG_ERR_INVALID_PARAMS           (APPLOG_ERR_BASE + 0x300)
#define  APPLOG_ERR_INVALID_PARAM1           (APPLOG_ERR_INVALID_PARAMS + 1)
#define  APPLOG_ERR_INVALID_PARAM2           (APPLOG_ERR_INVALID_PARAMS + 2)
#define  APPLOG_ERR_INVALID_PARAM3           (APPLOG_ERR_INVALID_PARAMS + 3)
#define  APPLOG_ERR_INVALID_PARAM4           (APPLOG_ERR_INVALID_PARAMS + 4)
#define  APPLOG_ERR_INVALID_PARAM5           (APPLOG_ERR_INVALID_PARAMS + 5)
#define  APPLOG_ERR_INVALID_PARAM6           (APPLOG_ERR_INVALID_PARAMS + 6)
#define  APPLOG_ERR_INVALID_PARAM7           (APPLOG_ERR_INVALID_PARAMS + 7)
#define  APPLOG_ERR_INVALID_PARAM8           (APPLOG_ERR_INVALID_PARAMS + 8)
#define  APPLOG_ERR_INVALID_PARAM9           (APPLOG_ERR_INVALID_PARAMS + 9)


/*=============================================================================
                              DATA STRUCTURES
=============================================================================*/
typedef struct IAppLog IAppLog;


/*=============================================================================
                                 INTERFACES
=============================================================================*/

#define INHERIT_IAppLog(iname) \
   INHERIT_IQueryInterface(iname); \
   uint16 (*SendLog)(IAppLog *piAppLog, uint32 dwLogId, uint16 wLogSz, void *pvLogData, uint32 dwNonce32); \
   uint16 (*SetAppClsId)(IAppLog *piAppLog, uint32 dwAppClsId); \
   uint16 (*Unlock)(IAppLog *piAppLog, const void *cpvCert, int nCertSz, AEECallback *pcb, uint32 *pdwRes, uint16 wRand)

AEEINTERFACE(IAppLog)
{
  INHERIT_IAppLog(IAppLog);
};


static __inline uint32 IAppLog_AddRef(IAppLog *piAppLog)
{
   return AEEGETPVTBL(piAppLog,IAppLog)->AddRef(piAppLog);
}

static __inline uint32 IAppLog_Release(IAppLog *piAppLog)
{
   return AEEGETPVTBL(piAppLog,IAppLog)->Release(piAppLog);
}

static __inline int IAppLog_QueryInterface(IAppLog *piAppLog, AEECLSID clsId, 
                                           void **ppo)
{
   return AEEGETPVTBL(piAppLog,IAppLog)->QueryInterface(piAppLog, clsId, ppo);
}

static __inline uint16 IAppLog_SendLog(IAppLog *piAppLog, uint32 dwLogId, 
                                    uint16 wLogSz, void *pvLogData, 
                                    uint32 dwNonce32)
{
   return AEEGETPVTBL(piAppLog,IAppLog)->SendLog(piAppLog, dwLogId, wLogSz, 
                                              pvLogData, dwNonce32);
}

static __inline uint16 IAppLog_SetAppClsId(IAppLog *piAppLog, uint32 dwAppClsId)
{
   return AEEGETPVTBL(piAppLog,IAppLog)->SetAppClsId(piAppLog, dwAppClsId);
}

static __inline uint16 IAppLog_Unlock(IAppLog *piAppLog, const void *cpvCert, 
                                      int nCertSz, AEECallback *pcb, 
                                      uint32 *pdwRes, uint16 wRand)
{
   return AEEGETPVTBL(piAppLog,IAppLog)->Unlock(piAppLog, cpvCert, nCertSz, 
                                                pcb, pdwRes, wRand);
}


/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================
  No new datatypes
===============================================================================
*/

/*
===============================================================================
MACROS DOCUMENTATION
===============================================================================
  No new macros
===============================================================================
*/

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================
Interface Name: IAppLog

Description: 
  This module is the MSM software logging access extension.
  IAppLog supports logging api for external applications
  to log packets and data to MobileView

This extension allows the user to:
  1.  Send log data to DMSS

To use an IAppLog extension:
  1.  Call ISHELL_CreateInstance() to create an instance of an IAppLog,
      specifying the ClassID of this extension.
  2.  Unlock the instance using IAppLog_Unlock() supplying the right
      credentials.
  3.  Send logs to IAppLog_SendLog().
  4.  When you no longer need the AppLog instance, call 
      IAppLog_Release() to release it.
 
==============================================================================

Function: IAppLog_AddRef()

Description:
  This function increments the reference count for the IAppLog Interface 
  object.

Prototype:
  uint32 IAppLog_AddRef (IAppLog_AddRef * piAppLog)

Parameters:
  piAppLog : [IN] Pointer to IAppLog Interface object

Return Value:
  Updated reference count

Comments: 
  None

Side Effects: 
  None

See Also: 
  IAppLog_Release()
    
==============================================================================

Function: IAppLog_Release()

Description:
  This function decerements the reference count for the IAppLog 
  Interface object and, if ref count becomes 0, releases resource in use
  by this object.

Prototype:
  uint32 IAppLog_Release (IAppLog * piAppLog)

Parameters:
  piAppLog : [IN] Pointer to IAppLog Interface object

Return Value:
  Updated reference count

Comments: 
  None

Side Effects: 
  None

See Also: 
  IAppLog_AddRef()

==============================================================================

Function: IAppLog_QueryInterface()

Description:
  This function is inherited from IQI_QueryInterface
   
Prototype:
  int IAppLog_QueryInterface (IAppLog *piAppLog, AEECLSID idReq, 
                              void **ppo)
   
Parameters:
  piAppLog:[IN] Pointer to IAppLog Interface object
  idReq :  [IN] Requested class ID exposed by the object 
  ppo :    [OUT] Returned object. Filled by this method 

Return Value:
  ECLASSNOTSUPPORT: Class is not supported
  AEE_SUCCESS:      Success

Comments: 

Side Effects: 
  None

See Also: 
  None

==============================================================================

Function: IAppLog_Unlock()

Description: 
  This function can be used by an application to unlock this instance of 
  IAppLog.  A callback should be provided to recieve the status of 
  unlock request. The application should unlock the instance of 
  IAppLog and save it for future as long as logging will be done. 
  
  Once unlock succeeds, it sends an event EVT_APPLOGEXT to the application with
  the matching the classId found in the provided cert. The dwParam value of
  this event is a 32 bit random number genrated by IAppLog and should be
  used when calling AppLog. wParam value of this Event will match wRand 
  supplied in the call to this function.

Prototype:
  uint16 IAppLog_Unlock(IAppLog *piAppLog, const void *cpvCert, 
                           uint32 nCertSz, AEECallback *pcb,  
                           uint32 *pdwRes, uint16 wRand)

Parameters:
  piAppLog: [IN]  Pointer to IAppLog Interface object
  cpvCert : [IN]  Pointer to buffer containing DER encoded certificate
  nCertSz : [IN]  Size of certificate
  pcb :     [IN]  Callback to receive result of certificate verification
  pdwRes :  [OUT] Pointer to the variable where the result of the Unlock process
                  will be stored. If not SUCCESS, the result corresponds to 
                  IAppLog error codes defined above.
  wRand :   [IN]  16 bit random # generated by the application. The application 
                  should only honor EVT_APPLOGEXT if wParam matches this value.

Return Value:
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


Comments:
  
Side Effects:
  Variable pointed to by pdwRes will be modified  

See Also:
  None

==============================================================================

Function: IAppLog_SendLog()

Description: 
  This function can be called only after the instance of AppLog is unlocked.
  If BREW cls ID has not been set, then this function will return error code.
  Max. log size is 475-bytes.

Prototype:
  uint16 IAppLog_SendLog(IAppLog *piAppLog, uint32 dwLogId, 
                         uint16 wLogSz, void *pvLogData, uint32 dwNounce32);

Parameters:
  piAppLog :   [IN] Pointer to IAppLog Interface object
  dwLogId :    [IN] Log id to associate this log with.
  wLogSz :     [IN] Size of the log_data
  pvLogData :  [IN] Log data the application wants to send to the MobileView.
  dwNonce32 :  [IN] 32 bit random number provided after Unlock.

Return Value:
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


Comments:
  
Side Effects:
  None

See Also:
  None

==============================================================================

Function: IAppLog_SetAppClsId()

Description: 
  This function is only available on test phones and is for testing purposes
  only.

  It sets the classId and unlocks IAppLog instace and then sets random 
  number value to 1.

Prototype:
  uint16 IAppLog_SetAppClsId(IAppLog *piAppLog, uint32 dwAppClsId);

Parameters:
  piAppLog :   [IN] Pointer to IAppLog Interface object
  dwAppClsId : [IN] ClassId to set.

Return Value:
  SUCCESS:                     Success
  APPLOG_ERR_INVALID_PARAM1:   IAppLog instance invalid
  APPLOG_ERR_LG_CLSID_INVALID: Invalid classId
  APPLOG_ERR_FEATURE_UNAVAIL:  Feature not available

Comments:
  
Side Effects:
  None

See Also:
  None

==============================================================================

=======================================================================*/
#endif /* #ifndef AEEAppLog_H */
