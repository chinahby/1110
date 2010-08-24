
#ifndef _OEMOBEXNOTIFIER_H_
#define _OEMOBEXNOTIFIER_H_

/*===========================================================================

FILE:      OEMOBEXNotifier.h

SERVICES:  OEM OBEX Notifier

DESCRIPTION: The IOBEXNOTIFIER BREW interface.

PUBLIC CLASSES:  Not Applicable

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMOBEXNotifier.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $
                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/13/04   sp      initial checkin. unit tested for IOBEXServer API
3/24/04    sp      corrected featurization so that header file is not featurized
=============================================================================*/

#include "AEE_OEM.h"
#include "AEEOBEXNotifier.h"
#include "BREWVersion.h"
#include "AEEHandle.h"

#ifdef __cplusplus
extern "C" {
#endif


#if MIN_BREW_VERSION(3,0)
    extern AEEInstanceList gOEMOBEXHandleList;
#else /* MIN_BREW_VERSION at least 3.0 */
    // AEEHandle isn't defined before BREW 3.0, so make stuff up:
#define AEEHandle_To(list,pOEM,pHandle)  (*(pHandle)=(uint32)(pOEM),TRUE)
#define AEEHandle_Clear(x,y)             TRUE
#define AEEHandle_From(list,Handle,pOEM) (*(pOEM)=(OEMINSTANCE)(Handle),TRUE)
#define AEEHandle_Lookup(x,y)            TRUE

    // AEE_APP_RESUME isn't defined before BREW 3.0
#define AEE_APP_RESUME(pcb,pa)      AEE_ResumeEx(pcb,0,pa)

    // uint64 isn't defined until BREW 3.0
    typedef  unsigned __int64   uint64;      /* Unsigned 64 bit value */

#endif /* MIN_BREW_VERSION at least 3.0 */


struct _IOBEXNotifier {
    AEEVTBL(IOBEXNotifier)* pvt;
    IShell*             m_pIShell;
    uint32              m_uRefs;
    AEECLSID            m_cls;
    AEECallback         m_cb;
    AEECallback         m_cbExit;
    uint32              m_hNotifier;      // Handle to the OEM layer notifier
};

    uint32  OEMOBEXNotifier_Init(IOBEXNotifier *pAEENotifier );
    uint32  OEMOBEXNotifier_Release(uint32 hHandle);
    void    OEMOBEXNotifier_SetMask(uint32 hHandle,
                                    const uint32 *pdwMask,
                                    int32 dwCount);
    boolean OEMOBEXNotifier_GetCb( uint32 hHandle,  AEEOBEXNotificationType *pNot );
    int     OEMOBEXNotifier_DoNotify( IOBEXNotifier *pParent,
                                      AEEOBEXNotificationType* pNotification,
                                      ACONTEXT* pac );

#ifdef __cplusplus
}
#endif


#endif //_OEMOBEXNOTIFIER_H_
