#ifndef OEMMTJMP_H
#define OEMMTJMP_H
/*============================================================================

FILE:  OEMMTjmp.h

SERVICES:  
       jmpbuf manipulation for multi-threading apps.  Differs slightly from
       OEM_jmpbuf, OEM_longjmp, and OEM_setjmp implementations defined in
       OEMStdLib.h for other purposes in other parts of the code.


               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

#include "AEEStdLib.h"

#if   defined(WIN32) && defined(_M_IX86)

// Win32
//
// MSVC's provided setjmp() and longjmp() include stack unwiding nonsense that
// spoils their usefulness for thread swapping.
//
// Inline assembly cannot be used to define functions, only to insert assembly
// in a C function.  Since the C function has its own rules for initializing
// the stack frame and saving/restoring registers, we can't write a setjmp()
// function using inline assembler without nasty compilation dependencies.
// This macro works, but it cannot "return" a value.  It sets a variable to
// TRUE or FALSE.

#define OEMMT_JB_LEN      6
#define OEMMT_JB_SP       4
#define OEMMT_JB_BP       0

typedef int OEMMT_jmp_buf[OEMMT_JB_LEN];

#define OEMMT_JB_MOVESTACK(jb, nOffset)        ( (jb)[OEMMT_JB_BP] += (nOffset), (jb)[OEMMT_JB_SP] += (nOffset) )

#define OEMMT_setjmp(jb, bJump) \
   {                                                   \
      OEMMT_jmp_buf *__pjmp = &(jb);                   \
      _asm mov         edx, __pjmp                     \
      _asm mov         dword ptr [edx], ebp            \
      _asm mov         dword ptr [edx+4], ebx          \
      _asm mov         dword ptr [edx+8], edi          \
      _asm mov         dword ptr [edx+0Ch], esi        \
      _asm mov         dword ptr [edx+10h], esp        \
      _asm lea         eax, OEMMT_setjmp_resume##bJump \
      _asm mov         dword ptr [edx+14h], eax        \
      _asm sub         eax, eax                        \
      _asm OEMMT_setjmp_resume##bJump:                 \
      _asm mov         bJump, eax                      \
   }

/* 
   this pragma warning (disable : 4731) diables the warning about the 
   base pointer ebp, which we're modifying here to restore 

   this warning seemed to appear when I installed the processor pack...
*/
#pragma warning( disable : 4731 )
static __inline void OEMMT_longjmp(OEMMT_jmp_buf pjmp, int nRet)
{
   _asm {
      mov         edx, pjmp
      mov         ebp, dword ptr [edx];
      mov         ebx, dword ptr [edx+4];
      mov         edi, dword ptr [edx+8];
      mov         esi, dword ptr [edx+0Ch];
      mov         esp, dword ptr [edx+10h];
      mov         eax, 1;
      jmp         dword ptr [edx+14h];
   }
}
#pragma warning( default : 4731 )



#else /* #if defined(WIN32) && defined(_M_IX86) */

typedef OEM_jmpbuf OEMMT_jmp_buf;

#define OEMMT_setjmp(jb, var)              (var = OEM_setjmp(jb))
#define OEMMT_longjmp(jb, val)             OEM_longjmp((jb),(val))

#ifdef __ARMCC_VERSION
#if __ARMCC_VERSION >= 200000 // RVCT 2.0

#define OEMMT_JB_SP 9

static __inline void OEMMT_JB_MOVESTACK(OEMMT_jmp_buf jb, int32 offset)
{
   int32 * njb = (int32 *)jb;
   njb[OEMMT_JB_SP] += offset;
}

#else

#define OEMMT_JB_SP  (sizeof(OEMMT_jmp_buf)==(sizeof(int)*32)?9:sizeof(OEMMT_jmp_buf)==(sizeof(int)*22)?8:0)

#define OEMMT_JB_MOVESTACK(jb, offset)      ( (jb)[OEMMT_JB_SP] += (offset) )

#endif

#else

#define OEMMT_JB_SP 9

static __inline void OEMMT_JB_MOVESTACK(OEMMT_jmp_buf jb, int32 offset)
{
   int32 * njb = (int32 *)jb;
   njb[OEMMT_JB_SP] += offset;
}

#endif

#endif /* #if defined(WIN32) && defined(_M_IX86) */ 

#endif /* #ifndef OEMMTJMP_H */
