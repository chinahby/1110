#ifndef AEE_STATIC_H
#define AEE_STATIC_H /* #ifndef AEE_STATIC_H */

/*======================================================
FILE:  AEE_static.h

SERVICES:  BREW helper functions for static
           applications and modules.

GENERAL DESCRIPTION:
   Static linkage variant of AEE.h.

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#if !defined(AEE_STATIC)
#error AEE_static.h is for inclusion by static apps and modules: those compiled with the BREW library directly, or as part of an OEM build.  If you are statically linking with BREW, please define AEE_STATIC in your Makefile.
#endif 

#if !defined(AEE_STATIC_H__INC__BY__AEE_H)
#error AEE_static.h is not for intended for direct inclusion.  Please remove any include directives that reference AEE_static.h directly and change them to include AEE.h.
#endif


/* ==========================================================================
  ||
  || Static linkers get these
  ||
  ========================================================================== */

#undef  CALLBACK_Cancel

static __inline void CALLBACK_Cancel(AEECallback *pcb)
{
   //We need this stack variable so we operate on the copy of the
   //cancel function in the off chance that 2 or more tasks
   //are cancelling at the same time.
   
   PFNCBCANCEL pfnCancel = (pcb)->pfnCancel;
   
   if (0 != pfnCancel) 
   { 
      pfnCancel(pcb);
   }
}

/* ==========================================================================
  ||
  || end Static
  ||
  ========================================================================== */


#endif /* #ifndef AEE_STATIC_H */

