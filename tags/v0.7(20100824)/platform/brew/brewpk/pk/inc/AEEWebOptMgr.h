/*======================================================

FILE:      AEEWebOptMgr.h

SERVICES:  static WebOptMgr implementaion, used for
          implementation of IWebOpts, IWeb, ISSL, etc.

GENERAL DESCRIPTION:

   WebOptMgr is the code necessary to implement IWebOpts
   correctly.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    WebOptMgr

INITIALIZATION & SEQUENCING REQUIREMENTS:

    See Exported Routines

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef AEEWEBOPTMGR_H
#define AEEWEBOPTMGR_H

#include "AEEWebOpts.h"

typedef struct WebOptMgr WebOptMgr;

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

extern AEE_EXPORTS int    WebOptMgr_GetSize(WebOpt *awozList);
extern AEE_EXPORTS int    WebOptMgr_CtorZ(WebOptMgr *me, WebOpt *awozList);
extern AEE_EXPORTS void   WebOptMgr_Dtor(WebOptMgr *me);
extern AEE_EXPORTS int    WebOptMgr_AddOpt(WebOptMgr *me, ...);
extern AEE_EXPORTS int    WebOptMgr_AddOptV(WebOptMgr *me, WebOpt *awozList);
extern AEE_EXPORTS int    WebOptMgr_RemoveOpt(WebOptMgr *me, int32 nOptId, int32 nIndex);
extern AEE_EXPORTS int    WebOptMgr_GetOpt(WebOptMgr *me, int32 nOptId, int32 nIndex, WebOpt *pwo);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef AEEWEBOPTMGR_H */
