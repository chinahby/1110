#ifndef IXCALLBACKDEF_H
#define IXCALLBACKDEF_H

#include "IxErrno.h"

#ifdef __cplusplus
extern "C" {
#endif

 //
// Generic callback header.  This may be used for various interfaces where
// memory allocation for callbacks is prohibitive.
//
// pfnCancel - Filled by the handler.  It can be called at any time to cancel the callback
// pfnNotify - Filled by the caller.  It is called when the callback is issued.
// pNotifyData - Filled by the caller.  It is passed to the callback.
//

typedef struct _AEECallbackIx AEECallbackIx;

// IxCallback, uses AEECallback.
typedef struct _IxCallback IxCallback;

typedef void (*PFNCBCANCELIx)(IxCallback * pcb);
typedef void (*PFNNOTIFYIx)(IxCallback * pcb);

struct _AEECallbackIx
{
  AEECallbackIx *pNext;               // RESERVED
  void          *pmc;                 // RESERVED
  PFNCBCANCELIx  pfnCancel;           // Filled by callback handler
  void          *pCancelData;         // Filled by callback handler
  uint32         cancelDataSize;      // Filled by callback handler
  PFNNOTIFYIx    pfnNotify;           // Filled by caller
  void          *pNotifyData;         // Filled by caller
  uint32         notifyDataSize;      // Filled by caller 
  uint32         pUserData;           // Filled by caller  -- Not to be handled by callback
  uint32         pClientData;         // Filled by caller  -- Not to be handled by callback
  uint32         pServerData;         // Filled by callback handler -- leave as uint32 for now
  uint32         pServerCancelCB;     // Filled by callback handler
  uint32         pServerCancelCBData; // Filled by callback handler
  void         **ppStream;
  void          *pReserved;           // RESERVED - Used by handler
};

struct _IxCallback
{
  struct _AEECallbackIx  AEECallback;
  IxErrnoType           NotifyStatus;    // Filled by caller
};

#ifdef __cplusplus
}
#endif

#endif

