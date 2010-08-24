#ifndef _OEMBTEXT_H_
#define _OEMBTEXT_H_

/*===========================================================================

FILE:      OEMBTExt.h
 
SERVICES:  AEE Bluetooth Data Structures
 
DESCRIPTION: Definitions shared by all IBTExt BREW interfaces.

PUBLIC CLASSES:  Not Applicable

     Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExt.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "AEE_OEM.h"
#include "BREWVersion.h"
#include "AEEHandle.h"
#include "bt.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// String conversion macros
// ---------------------------------------------------------------------------
#define AEEBT_TO_WSTR( s, d, b ) {                    \
  uint8 len = STRLEN( s );                            \
  MEMSET((void*)(d), 0x0, (b));                       \
  UTF8TOWSTR( (uint8*)(s), len, (d), (b) );           }

#define AEEBT_FROM_WSTR( s, d, b) {                   \
  uint8 len = WSTRLEN( s );                           \
  MEMSET((void*)(d), 0x0, (b));                       \
  WSTRTOUTF8( (s), len, (uint8*)(d), (b) );           }

// ---------------------------------------------------------------------------
// App context macros
// ---------------------------------------------------------------------------
#if MIN_BREW_VERSION(3, 0)
#define ENTER_APP_CONTEXT AEE_EnterAppContext
#define LEAVE_APP_CONTEXT AEE_LeaveAppContext
#else
#define ENTER_APP_CONTEXT AEE_SetAppContext
#define LEAVE_APP_CONTEXT AEE_SetAppContext
#endif

// ---------------------------------------------------------------------------
// AEEHandle macros
// ---------------------------------------------------------------------------
typedef uint32 IBTHandle;
#if MIN_BREW_VERSION(3,0)
extern AEEInstanceList gOEMBTExtHandleList;
#else /* MIN_BREW_VERSION at least 3.0 */
  // AEEHandle isn't defined before BREW 3.0, so make stuff up:
# define AEEHandle_To(list,pOEM,pHandle)  (*(pHandle)=(IBTHandle)(pOEM),TRUE)
# define AEEHandle_Clear(x,y)             TRUE
# define AEEHandle_From(list,Handle,pOEM) (*(pOEM)=(OEMINSTANCE)(Handle),TRUE)
# define AEEHandle_Lookup(x,y)            TRUE

  // AEE_APP_RESUME isn't defined before BREW 3.0
#define AEE_APP_RESUME(pcb,pa)      AEE_ResumeEx(pcb,0,pa)

  // uint64 isn't defined until BREW 3.0
typedef  unsigned __int64   uint64;      /* Unsigned 64 bit value */

#endif /* MIN_BREW_VERSION at least 3.0 */

// ---------------------------------------------------------------------------
// Notifier
// ---------------------------------------------------------------------------
AEEINTERFACE(IBTExtNotifier)
{
   INHERIT_INotifier(IBTExtNotifier);
   int (*DoNotify)(IBTExtNotifier* po, AEEBTNotification* pNotification,
                   ACONTEXT* pac);
   AEEBTNotification* (*GetFreeNotification)( IBTExtNotifier* po );
   void (*ReleaseNotification)( IBTExtNotifier* po, AEEBTNotification*  pNotification );
};

#define IBTEXTNOTIFIER_DoNotify(p,pn,pac)         \
        AEEGETPVTBL((p),IBTExtNotifier)->DoNotify((p),(pn),(pac))
        
#define IBTEXTNOTIFIER_Release(p)                 \
        AEEGETPVTBL((p),IBTExtNotifier)->Release((p))

#define IBTEXTNOTIFIER_GetFreeNotification(p)     \
        AEEGETPVTBL((p),IBTExtNotifier)->GetFreeNotification((p))

#define IBTEXTNOTIFIER_ReleaseNotification(p,pn)  \
        AEEGETPVTBL((p),IBTExtNotifier)->ReleaseNotification((p),(pn))

uint32  OEMBTExtNotifier_Init(IBTExtNotifier *pAEENotifier );
uint32  OEMBTExtNotifier_Release(IBTHandle hHandle);
#if MIN_BREW_BUILD(3,1,4,7)
void    OEMBTExtNotifier_SetMask(IBTHandle hHandle, 
								 uint32 *pdwMask,
                                 int32 dwCount);
#else
void    OEMBTExtNotifier_SetMask(IBTHandle hHandle, 
								 const uint32 *pdwMask,
                                 int32 dwCount);
#endif // MIN_BREW_BUILD(3,1,4,7)
boolean OEMBTExtNotifier_GetCb( IBTHandle hHandle, AEEBTNotification *pNot );
int     OEMBTExtNotifier_DoNotify( IBTExtNotifier *pParent, 
                                   AEEBTNotification* pNotification,
                                   ACONTEXT* pac );
AEEBTNotification* OEMBTExtNotifier_GetFreeNotification( IBTExtNotifier* po );
void OEMBTExtNotifier_ReleaseNotification( IBTExtNotifier* po, AEEBTNotification*  pNotification );

// ---------------------------------------------------------------------------
// Utility functions
// ---------------------------------------------------------------------------
AEEBTError OEMBTExt_MapCmdStatus( bt_cmd_status_type status );
void OEMBTExt_ResumeCallback( AEECallback* pCb );

#ifdef __cplusplus
}
#endif

#endif  //_OEMBTEXT_H_

