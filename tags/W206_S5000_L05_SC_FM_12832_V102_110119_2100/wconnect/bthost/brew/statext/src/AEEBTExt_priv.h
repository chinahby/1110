#ifndef _AEEBTEXT_PRIV_H_
#define _AEEBTEXT_PRIV_H_

/*===========================================================================

FILE:      AEEBTExt_priv.h

SERVICES:  AEE Bluetooth Data Structures

DESCRIPTION: This is the header file for Bluetooth BREW Extension

PUBLIC CLASSES:  Not Applicable

        Copyright © 2003-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEBTExt.h"
#include "AEEBTExtAG.h"
#include "AEEBTExtNA.h"
#include "AEEBTExtRM.h"
#include "AEEBTExtSD.h"
#include "AEEBTExtSPP.h"
#include "AEEBTExtL2.h"
#include "AEEBTExtOPP.h"
#include "AEEBTExtFTP.h"
#include "AEEBTExtBIP.h"
#include "AEEBTExtBPP.h"
#include "AEEBTExtSML.h"
#include "AEEBTExtCTP.h"
#include "AEEBTExtICP.h"
#include "AEEBTExtSAP.h"
#include "AEEBTExtA2DP.h"
#include "AEEBTExtAVRCP.h"
#include "AEEBTExtPBAP.h"
#include "OEMBTExt.h"

// Notifier structure. Used by AEEBTExt, but needs to be read by OEMBTExt.
struct _IBTExtNotifier
{
  AEEVTBL(IBTExtNotifier)* pvt;
  IShell*             m_pIShell;
  uint32              m_uRefs;
  AEECLSID            m_cls;
  AEECallback         m_cb;
  AEECallback         m_cbExit;
  IBTHandle           m_hBTNotifier;      // Handle to the OEM layer notifier
};

struct _IBTExtSD
{
  union
  {
    const AEEVTBL(IBTExtSD)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  uint32              nRefs;
  IShell*             pIShell;
  IBTHandle           m_hBT;
};

struct _IBTExtSPP
{
  union
  {
    const AEEVTBL(IBTExtSPP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  uint32              nRefs;
  IShell*             pIShell;
  IBTHandle           m_hBT;
};

struct _IBTExtAG
{
  AEEVTBL(IBTExtAG)*  pvt;
  IShell*             m_pIShell;
  uint32              m_uRefs;
  AEECLSID            m_cls;
  AEECallback         m_cbExit;
  IBTHandle           m_hBT;
};

struct _IBTExtRM
{
  union
  {
    const AEEVTBL(IBTExtRM)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*                 m_pIShell;
  uint32                  nRefs;
  IBTHandle               m_hBT;
  AEECLSID                m_cls;
};

struct _IBTExtNA
{
  AEEVTBL(IBTExtNA)* pvt;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtL2
{
  union
  {
    const AEEVTBL(IBTExtL2)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*                 m_pIShell;
  uint32                  m_uRefs;
  IBTHandle               m_hBT;
};

struct _IBTExtOPP
{
  union
  {
    const AEEVTBL(IBTExtOPP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtFTP
{
  union
  {
    const AEEVTBL(IBTExtFTP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtBIP
{
  union
  {
    const AEEVTBL(IBTExtBIP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtBPP
{
  union
  {
    const AEEVTBL(IBTExtBPP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtSML
{
  union
  {
    const AEEVTBL(IBTExtSML)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtCTP
{
  union
  {
    const AEEVTBL(IBTExtCTP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtICP
{
  union
  {
    const AEEVTBL(IBTExtICP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtSAP
{
  union
  {
    const AEEVTBL(IBTExtSAP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtA2DP
{
  union
  {
    const AEEVTBL(IBTExtA2DP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtAVRCP
{
  union
  {
    const AEEVTBL(IBTExtAVRCP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

struct _IBTExtPBAP
{
  union
  {
    const AEEVTBL(IBTExtPBAP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
  IShell*           m_pIShell;
  uint32            m_uRefs;
  AEECLSID          m_cls;
  IBTHandle         m_hBT;
};

#endif /* _AEEBTEXT_PRIV_H_ */
