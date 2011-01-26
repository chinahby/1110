#ifndef _OEMBTMODTABLEEXT_H_
#define _OEMBTMODTABLEEXT_H_

/*======================================================
FILE:  OEMBTModTableExt.c

SERVICES:  Reference Bluetooth OEM specific Static 
           Module/Class Tables.

GENERAL DESCRIPTION:
   This file contains the list of static modules and static classes
   that are specific to Bluetooth OEM platform. These get included as 
   part of the static tables defined in OEMModTable.c.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright (c) 2005-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            Qualcomm Confidential and Proprietary
=====================================================*/
#include "OEMFeatures.h"

#if defined(FEATURE_IBT)

/*======================================================
 * Include Files
 *=====================================================*/

#include "AEEBTExt.h"
#include "AEEBTExtAG.h"
#include "AEEBTExtNA.h"
#include "AEEBTExtSD.h"
#include "AEEBTExtRM.h"
#include "AEEBTExtSPP.h"
#ifdef FEATURE_IOBEX
#include "AEEOBEX.h"
#include "AEEOBEXNotifier.h"
#endif
#ifdef FEATURE_BT_EXTPF_BIP
#include "AEEBTExtBIP.h"
#endif
#ifdef FEATURE_BT_EXTPF_BPP
#include "AEEBTExtBPP.h"
#endif
#ifdef FEATURE_BT_EXTPF_FTP
#include "AEEBTExtFTP.h"
#endif
#include "AEEBTExtL2.h"
#ifdef FEATURE_BT_EXTPF_OPP
#include "AEEBTExtOPP.h"
#endif
#ifdef FEATURE_BT_EXTPF_SML
#include "AEEBTExtSML.h"
#endif
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
#include "AEEBTExtA2DP.h"
#include "AEEBTExtAVRCP.h"
#endif
#ifdef FEATURE_BT_EXTPF_CTP
#include "AEEBTExtCTP.h"
#include "AEEBTExtICP.h"
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
#include "AEEBTExtPBAP.h"
#endif

/*======================================================
 * External declarations
 *=====================================================*/

extern int AEEBTExtAG_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int AEEBTExtNA_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int AEEBTExtSD_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int AEEBTExtRM_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int AEEBTExtSPP_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int AEEBTExtL2_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int AEEBTExtNotifier_New(IShell *ps,AEECLSID cls,void ** ppo);

#ifdef FEATURE_IOBEX
extern int AEEOBEXServer_New(IShell* ps, AEECLSID cls, void** ppif);
extern int AEEOBEXClient_New(IShell* ps, AEECLSID cls, void** ppif);
extern int AEEOBEXNotifier_New( IShell *pIShell, AEECLSID cls, void **ppif );
#endif /* FEATURE_IOBEX */
#ifdef FEATURE_BT_EXTPF_BIP
extern int AEEBTExtBIP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
#ifdef FEATURE_BT_EXTPF_BPP
extern int AEEBTExtBPP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
#ifdef FEATURE_BT_EXTPF_FTP
extern int AEEBTExtFTP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
extern int AEEBTExtL2_New(IShell* ps, AEECLSID cls, void** ppif);
#ifdef FEATURE_BT_EXTPF_OPP
extern int AEEBTExtOPP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
#ifdef FEATURE_BT_EXTPF_SML
extern int AEEBTExtSML_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
extern int AEEBTExtA2DP_New(IShell* ps, AEECLSID cls, void** ppif);
extern int AEEBTExtAVRCP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
extern int AEEBTExtPBAP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
#ifdef FEATURE_BT_EXTPF_CTP
extern int AEEBTExtCTP_New(IShell* ps, AEECLSID cls, void** ppif);
extern int AEEBTExtICP_New(IShell* ps, AEECLSID cls, void** ppif);
#endif
// Bluetooth App
#if defined(FEATURE_APP_BLUETOOTH)
  //extern int BTApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
  extern int BTAppMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif /* FEATURE_APP_BLUETOOTH */

/*======================================================
 * Static mod table entry for BT App 
 *=====================================================*/
#if defined(FEATURE_APP_BLUETOOTH)
//#  define BT_STATIC_MOD_LIST_ENTRY {AEEFS_MIF_DIR"btapp.mif", BTApp_Load},
#  define BT_STATIC_MOD_LIST_ENTRY {AEEFS_MIF_DIR"btapp.mif", BTAppMod_Load},
#else
#  define BT_STATIC_MOD_LIST_ENTRY
#endif

/*======================================================
 * Static class list entry for IBT interfaces
 *=====================================================*/
#ifdef FEATURE_BT_EXTPF_BIP
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_BIP \
          {AEECLSID_BLUETOOTH_BIP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtBIP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_BIP
#endif

#ifdef FEATURE_BT_EXTPF_BPP
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_BPP \
          {AEECLSID_BLUETOOTH_BPP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtBPP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_BPP
#endif

#ifdef FEATURE_BT_EXTPF_FTP
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_FTP \
          {AEECLSID_BLUETOOTH_FTP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtFTP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_FTP
#endif

#ifdef FEATURE_BT_EXTPF_OPP
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_OPP \
          {AEECLSID_BLUETOOTH_OPP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtOPP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_OPP
#endif

#ifdef FEATURE_BT_EXTPF_SML
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_SML \
          {AEECLSID_BLUETOOTH_SML,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtSML_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_SML
#endif

#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_SAP
#endif

#ifdef FEATURE_BT_EXTPF_AV
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_AV \
          {AEECLSID_BLUETOOTH_A2DP,     ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtA2DP_New},\
          {AEECLSID_BLUETOOTH_AVRCP,    ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtAVRCP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_AV
#endif
#ifdef FEATURE_BT_EXTPF_CTP
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_CTP \
          {AEECLSID_BLUETOOTH_CTP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtCTP_New},\
          {AEECLSID_BLUETOOTH_ICP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtICP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_CTP
#endif
#ifdef FEATURE_IOBEX
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_OBEX \
          {AEECLSID_OBEXSERVER,         ASCF_PRIV, PL_NETWORK, NULL, AEEOBEXServer_New},\
          {AEECLSID_OBEXCLIENT,         ASCF_PRIV, PL_NETWORK, NULL, AEEOBEXClient_New},\
          {AEECLSID_OBEX_NOTIFIER,      ASCF_PRIV, PL_NETWORK, NULL, AEEOBEXNotifier_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_OBEX
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_PBAP \
          {AEECLSID_BLUETOOTH_PBAP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtPBAP_New},
#else
#  define BT_QBT_STATIC_CLASS_LIST_ENTRY_PBAP
#endif

#define BT_QBT_STATIC_CLASS_LIST_ENTRY \
   BT_QBT_STATIC_CLASS_LIST_ENTRY_BIP\
   BT_QBT_STATIC_CLASS_LIST_ENTRY_BPP\
   BT_QBT_STATIC_CLASS_LIST_ENTRY_FTP\
   BT_QBT_STATIC_CLASS_LIST_ENTRY_OPP\
   BT_QBT_STATIC_CLASS_LIST_ENTRY_SML\
   BT_QBT_STATIC_CLASS_LIST_ENTRY_SAP\
   BT_QBT_STATIC_CLASS_LIST_ENTRY_AV \
   BT_QBT_STATIC_CLASS_LIST_ENTRY_CTP \
   BT_QBT_STATIC_CLASS_LIST_ENTRY_PBAP \
   BT_QBT_STATIC_CLASS_LIST_ENTRY_OBEX

#define BT_STATIC_CLASS_LIST_ENTRY \
   {AEECLSID_BLUETOOTH_SPP,      ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtSPP_New},\
   {AEECLSID_BLUETOOTH_RM,       ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtRM_New},\
   {AEECLSID_BLUETOOTH_SD,       ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtSD_New},\
   {AEECLSID_BLUETOOTH_NA,       ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtNA_New},\
   {AEECLSID_BLUETOOTH_AG,       ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtAG_New},\
   {AEECLSID_BLUETOOTH_NOTIFIER, ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtNotifier_New},\
   {AEECLSID_BLUETOOTH_L2,       ASCF_PRIV, PL_NETWORK, NULL, AEEBTExtL2_New},\
   BT_QBT_STATIC_CLASS_LIST_ENTRY

#else /* FEATURE_IBT */

/* Bluetooth BREW interface is not supported. */
#define BT_STATIC_MOD_LIST_ENTRY
#define BT_STATIC_CLASS_LIST_ENTRY

#endif /* FEATURE_IBT */
#endif /* _OEMBTMODTABLEEXT_H_ */

