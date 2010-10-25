/*
  ============================================================================

FILE:  AEEBufBound.h

SERVICES:  
        AEEBufBound APIs

GENERAL DESCRIPTION:
        AEEBufBound provides a "bounded buffer" API that facilitates 
          measuring strings or character output.  It's design accomodates
          the implementation of functions that can have the same exact logic
          for measuring and outputting char buffer content.

REVISION HISTORY: 
        Fri Aug 08 17:38:29 2003: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2003 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef AEEBUFBOUND_H
#define AEEBUFBOUND_H /* #ifndef AEEBUFBOUND_H */


#include "AEEStdLib.h" /* for STRLEN(), MIN() */

typedef struct AEEBufBound
{
   char *pcBuf;   /* original buffer */
   char *pcWrite; /* write pointer */
   char *pcEnd;   /* first illegal write pointer */
} AEEBufBound;

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

static __inline void AEEBufBound_Init(AEEBufBound *me, char *pBuf, int nLen)
{
   me->pcWrite = me->pcBuf = pBuf; 
   me->pcEnd   = ((char *)0 != pBuf) ? pBuf + nLen : 0;
}

static __inline void AEEBufBound_Write(AEEBufBound *me, const char *pc, int nLen)
{
   char *pcStop = MIN(me->pcEnd, me->pcWrite + nLen);
   char *pcWrite = me->pcWrite;

   while (pcWrite < pcStop) {
      *pcWrite++ = *pc++;
   }

   me->pcWrite += nLen;
}

static __inline void AEEBufBound_Putc(AEEBufBound *me, char c)
{
   AEEBufBound_Write(me,&c,1);
}

static __inline void AEEBufBound_ForceNullTerm(AEEBufBound *me)
{
   if (me->pcWrite < me->pcEnd) { /* room left to term */
      *me->pcWrite++ = '\0';
   } else if (me->pcEnd != me->pcBuf) { /* overflow of non-null, 
                                           non-zero length buffer */
      me->pcEnd[-1] = '\0';
   } else {
      /* null buffer or empty buffer */
      me->pcWrite++;
   }
}

#define AEEBufBound_Puts(pbb, psz)   AEEBufBound_Write((pbb),(psz),STRLEN(psz))

#define AEEBufBound_IsCounter(pbb)   ((char *)0 == (pbb)->pcEnd)
#define AEEBufBound_Left(pbb)        ((pbb)->pcEnd - (pbb)->pcWrite)
#define AEEBufBound_ReallyWrote(pbb) (MIN((pbb)->pcEnd,(pbb)->pcWrite)-((pbb)->pcBuf))
#define AEEBufBound_Wrote(pbb)       ((pbb)->pcWrite-((pbb)->pcBuf))
#define AEEBufBound_IsFull(pbb)      (AEEBufBound_Left(pbb) < 1)

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef AEEBUFBOUND_H */
