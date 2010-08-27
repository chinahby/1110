#ifndef AEEOLDVALIST_H
#define AEEOLDVALIST_H
/*============================================================================
FILE:  AEEOldVaList.h

SERVICES:  ADSABI compatible Old va_list declarations.

GENERAL DESCRIPTION:
        Old va_list and conversion functions to convert va_list to Oldva_list.
        

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include <stdarg.h> //va_list 

#if defined(__arm)
typedef int **AEEOldVaList; // Old AEE va_list standard, conforms to adsabi
#else
typedef int *AEEOldVaList;
#endif

static __inline AEEOldVaList AEEOldVaList_From_va_list(va_list *arg)
{
#if (__ARMCC_VERSION >= 200000) && (!defined (__APCS_ADSABI))
   return (AEEOldVaList)&((*arg).__ap);
#elif defined(__arm) /*ADS 1.2, or RVCT with adsabi*/
   return (AEEOldVaList)*(void**)arg;
#else /*everything else*/
   return (AEEOldVaList)*arg;
#endif
}

static __inline void AEEOldVaList_To_va_list(AEEOldVaList aee_args,
                                             va_list *args)
{
#if (__ARMCC_VERSION >= 200000) && !defined (__APCS_ADSABI)
   (*args).__ap = (void *)*((void **)aee_args);
#elif defined(__arm) /* ADS 1.2, or RVCT with adsabi */
   *(*args) = *aee_args;
#else /* everything else */
   *args = (va_list)aee_args;
#endif
}
#endif // AEEOLDVALIST_H
