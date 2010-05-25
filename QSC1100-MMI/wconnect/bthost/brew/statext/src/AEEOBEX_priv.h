
#ifndef _AEE_OBEX_PRIV_H
#define _AEE_OBEX_PRIV_H

/*===========================================================================
FILE:      AEEOBEX_priv.h

SERVICES:  OBEX BREW extension

GENERAL DESCRIPTION: BREW interface to the OBEX profile (NT-OBEX).
                     This file contains the data structure for IOBEX

PUBLIC CLASSES AND STATIC FUNCTIONS:



        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEOBEX_priv.h#1 $
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

#include "AEE.h"
#include "AEEComdef.h"
#include "AEEOBEX.h"
/* #include "AEEOBEXOPP.h" */

struct _IOBEX
{
    union {
        const AEEVTBL(IOBEXServer) * pvtS;
        const AEEVTBL(IOBEXClient) * pvtC;
    }u;

    IShell*           m_pIShell;   //pointer to my shell
    uint32            m_uRefs;     //reference count
    IModel *          m_pIModel;   //the model used for model notifications
    uint32            m_hOBEX;     //handle to OBEX object (obtained via AEEHANDLE_To)
} ;

/*
struct _IOBEXOPP
{
  union
  {
    const AEEVTBL(IOBEXOPP)* pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject   so;    // "system object", tracked with AEE_LinkSysObject()
#endif
  } u;
   IShell*           m_pIShell;
   uint32            m_uRefs;
   AEECLSID          m_cls;
   IModel *          m_pIModel; //the model used for model notifications
   uint32            m_hOBEX;
};
*/

int    AEEOBEX_QueryInterface( IOBEX* po,
                               AEECLSID class,
                               void** ppo);

/* 
int    AEEOBEXOPP_QueryInterface( IOBEXOPP* po,
                                  AEECLSID class,
                                  void** ppo);
*/


#endif  // _AEE_OBEX_PRIV_H
