/*
  ============================================================================

FILE:  AEETU_TLS.h

SERVICES:  
        AEEThread TLS helpers

GENERAL DESCRIPTION:
        BREW and IThread together don't provide a full TLS solution, here
        is a sample implementation

REVISION HISTORY: 
        Wed Apr 24 13:03:45 2002: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef AEETU_TLS_H
#define AEETU_TLS_H /* #ifndef AEETU_TLS_H */

#include "AEEStdLib.h" /* for SETTLS() */
#include "AEEThread.h" /* for IThread * */

typedef struct AEETlsSlot AEETlsSlot;

typedef struct AEETls
{
   IThread    *piThread;
   AEETlsSlot *ptlssList;

} AEETls;

/* delare an AEETls structure on your stack, in your IThread start function, 
   *no lower* on the stack, unless you're sure you're not calling BREW from
   anywhere higher on your IThread's stack.  Then, invoke this macro to tell
   BREW what your thread's TLS is, and where your stack top is! */
#define AEETLS_INIT(t,th) do {\
   (t).piThread = (th);\
   (t).ptlssList = 0;\
\
   SETTLS(&t);\
} while(0);

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

IThread *IThread_Self(void);

int IThread_SetTLS(const char *cpszKey, void *pVal);
int IThread_GetTLS(const char *cpszKey, void **ppVal);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef AEETU_TLS_H */
