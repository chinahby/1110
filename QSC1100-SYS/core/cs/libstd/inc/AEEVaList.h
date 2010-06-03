#ifndef AEEVALIST_H
#define AEEVALIST_H
/*
=======================================================================
                  Copyright 2006 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
=======================================================================
*/

#if defined(__ARMCC_VERSION) || (defined(__GNUC__) && defined(__arm__))

#if (__ARMCC_VERSION >= 200000 && !defined(__APCS_ADSABI)) || \
    defined(__GNUC__) && defined(__arm__) && defined(__ARM_EABI__)

# define __AEEVA_AAPCS 1

#else

# define __AEEVA_AAPCS 0

#endif

typedef void* AEEVaList;

#define __AEEVA_ARGALIGN(t)   (((char*)(&((struct{char c;t x;}*)4)->x))-((char*)4))
#define __AEEVA_ARGSIZE(t)    ((sizeof(t)+sizeof(int)-1) & ~(sizeof(int)-1)) 

static __inline void __cpy(char*d, const char*s, int len)
{
   while (len-- > 0) *d++ = *s++;
}

static __inline AEEVaList __AEEVa_Arg(AEEVaList args, void* pv, 
                                      int nArgSize, int nArgAlign)
{
   int   nArgs = (int)args & ~1;
   char* pcArgs = (char*)args;
   int   bAAPCS = (int)args & 1;
   int   nEndOffset = 0;

   if (bAAPCS) { /* caller was compiled with AAPCS */

      if (nArgAlign > (int)sizeof(int)) {
         nArgAlign--; /* make a mask */
         pcArgs += ((nArgs + nArgAlign) & (int)~(unsigned)nArgAlign) - nArgs;
         /* move pv to next alignment */
      }

   }

   if (nArgSize < (int)sizeof(int)) {
      unsigned short s = 0x0001;
      if(*(unsigned char *)&s) {
         nEndOffset = 0; /* Little Endian Host - no offset required */
      } else {
         nEndOffset = (int) sizeof(int) - nArgSize;
      }
   }

   __cpy((char*)pv, (pcArgs - bAAPCS) + nEndOffset, nArgSize);

   /* round up */
   nArgSize = (nArgSize+(int)sizeof(int)-1) & ~((int)sizeof(int)-1);

   return pcArgs + nArgSize; /* increment va */
}

#define AEEVA_START(va,v)     ((va) = (char*)&(v) + __AEEVA_ARGSIZE(v) + __AEEVA_AAPCS)
#define AEEVA_ARG(va,v,t)     ((void)((va) = __AEEVa_Arg(va,&v,sizeof(t),__AEEVA_ARGALIGN(t))))
#define AEEVA_END(va)         ((va) = (AEEVaList)0)
#define AEEVA_COPY(dest, src) ((void)((dest) = (src)))

#else /* defined(__ARMCC_VERSION) || (defined(__GNUC__) && defined(__arm__)) */

#include <stdarg.h>

typedef va_list AEEVaList;

#define AEEVA_START(va,v)     (va_start((va_list)(va), (v)))
#define AEEVA_ARG(va,v,t)     ((v) = va_arg((va_list)(va),t))
#define AEEVA_END(va)         (va_end((va)))
#define AEEVA_COPY(dest, src) (va_copy((dest),(src)))

#endif/* defined(__ARMCC_VERSION) || (defined(__GNUC__) && defined(__arm__)) */

#endif /* #ifndef AEEVALIST_H */

