/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEESMS_Priv.h

SERVICES:  
   Provides AEE hooks for SMS interfaces.

GENERAL DESCRIPTION:  
   Provides AEE hooks for SMS interfaces.

PUBLIC CLASSES AND STATIC FUNCTIONS: 
   ISMSMsg, ISMS, ISMSNotifier, ISMSStorage, ISMSBCSrvOpts, ISMSBCConfig

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef AEESMS_PRIV_H
#define AEESMS_PRIV_H /* #ifndef AEESMS_PRIV_H */

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

AEE_EXPORTS int ISMSMsg_New(IShell *piShell, AEECLSID uCls, void **ppif);

AEE_EXPORTS int ISMS_New(IShell *piShell, AEECLSID uCls, void **ppif);

AEE_EXPORTS int ISMSNotifier_New(IShell *piShell, AEECLSID uCls, void **ppif);

AEE_EXPORTS int ISMSStorage_New(IShell *piShell, AEECLSID uCls, void **ppif);

AEE_EXPORTS int ISMSBCSrvOpts_New(IShell *piShell, AEECLSID uCls, void **ppif);

AEE_EXPORTS int ISMSBCConfig_New(IShell *piShell, AEECLSID uCls, void **ppif);

AEE_EXPORTS boolean AEESMS_IsDebugMsgOn(void);

AEE_EXPORTS void AEESMS_EnableDebugMsg(boolean bOn);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef AEESMS_PRIV_H */
